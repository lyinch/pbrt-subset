//
// Created by Thierry Backes on 2019-07-08.
//
#include "parallel.h"
#include "memory.h"
#include <list>
#include <thread>

namespace pbrt{
    static std::vector<std::thread> threads;
    class ParallelForLoop;
    static ParallelForLoop *workList = nullptr;
    static std::mutex workListMutex;

    class ParallelForLoop {
    public:
        // ParallelForLoop Public Methods
        ParallelForLoop(std::function<void(int64_t)> func1D, int64_t maxIndex,
                        int chunkSize)
                : func1D(std::move(func1D)),
                  maxIndex(maxIndex),
                  chunkSize(chunkSize){}
        ParallelForLoop(const std::function<void(Point2i)> &f, const Point2i &count)
                : func2D(f),
                  maxIndex(count.x * count.y),
                  chunkSize(1){
            nX = count.x;
        }

    public:
        // ParallelForLoop Private Data
        std::function<void(int64_t)> func1D;
        std::function<void(Point2i)> func2D;
        const int64_t maxIndex;
        const int chunkSize;
        int64_t nextIndex = 0;
        int activeWorkers = 0;
        ParallelForLoop *next = nullptr;
        int nX = -1;

        // ParallelForLoop Private Methods
        bool Finished() const {
            return nextIndex >= maxIndex && activeWorkers == 0;
        }
    };

    int MaxThreadIndex() {
        return PbrtOptions.nThreads == 0 ? NumSystemCores() : PbrtOptions.nThreads;
    }

    int NumSystemCores() {
        return std::max(1u, std::thread::hardware_concurrency());
    }

    static std::condition_variable workListCondition;

    void ParallelFor2D(std::function<void(Point2i)> func, const Point2i &count) {
        assert(threads.size() > 0 || MaxThreadIndex() == 1);

        if (threads.empty() || count.x * count.y <= 1) {
            for (int y = 0; y < count.y; ++y)
                for (int x = 0; x < count.x; ++x) func(Point2i(x, y));
            return;
        }

        ParallelForLoop loop(std::move(func), count);
        {
            std::lock_guard<std::mutex> lock(workListMutex);
            loop.next = workList;
            workList = &loop;
        }

        std::unique_lock<std::mutex> lock(workListMutex);
        workListCondition.notify_all();

        // Help out with parallel loop iterations in the current thread
        while (!loop.Finished()) {
            // Run a chunk of loop iterations for _loop_

            // Find the set of loop iterations to run next
            int64_t indexStart = loop.nextIndex;
            int64_t indexEnd = std::min(indexStart + loop.chunkSize, loop.maxIndex);

            // Update _loop_ to reflect iterations this thread will run
            loop.nextIndex = indexEnd;
            if (loop.nextIndex == loop.maxIndex) workList = loop.next;
            loop.activeWorkers++;

            // Run loop indices in _[indexStart, indexEnd)_
            lock.unlock();
            for (int64_t index = indexStart; index < indexEnd; ++index) {
                if (loop.func1D) {
                    loop.func1D(index);
                }
                    // Handle other types of loops
                else {
                    assert(loop.func2D);
                    loop.func2D(Point2i(index % loop.nX, index / loop.nX));
                }
            }
            lock.lock();

            // Update _loop_ to reflect completion of iterations
            loop.activeWorkers--;
        }
    }

}