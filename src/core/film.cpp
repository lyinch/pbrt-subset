//
// Created by Thierry Backes on 2019-06-20.
//

#include "film.h"


namespace pbrt{

    Film::Film(const Point2i &resolution, const Bounds2f &cropWindow, std::unique_ptr<Filter> filt,
               const std::string &filename, float scale):
            fullResolution(resolution),
            filter(std::move(filt)),
            filename(filename),
            scale(scale){
        croppedPixelBounds =
                Bounds2i(Point2i(std::ceil(fullResolution.x * cropWindow.pMin.x),
                                 std::ceil(fullResolution.y * cropWindow.pMin.y)),
                         Point2i(std::ceil(fullResolution.x * cropWindow.pMax.x),
                                 std::ceil(fullResolution.y * cropWindow.pMax.y)));

        pixels = std::unique_ptr<Pixel[]>(new Pixel[croppedPixelBounds.Area()]);

        int offset = 0;
        for (int y = 0; y < filterTableWidth; ++y) {
            for (int x = 0; x < filterTableWidth; ++x, ++offset) {
                Point2f p;
                p.x = (x + 0.5f) * filter->radius.x / filterTableWidth;
                p.y = (y + 0.5f) * filter->radius.y / filterTableWidth;
                filterTable[offset] = filter->Evaluate(p);
            }
        }
    }

    Bounds2i Film::GetSampleBounds() const {
        Bounds2f floatBounds(Floor(Point2f(croppedPixelBounds.pMin) +
                  Vector2f(0.5f, 0.5f) - filter->radius),
        Ceil(Point2f(croppedPixelBounds.pMax) -
             Vector2f(0.5f, 0.5f) + filter->radius));
        return (Bounds2i)floatBounds;
    }

    std::unique_ptr<FilmTile> Film::GetFilmTile(const Bounds2i &sampleBounds) {
        Vector2f halfPixel = Vector2f(0.5f, 0.5f);
        Bounds2f floatBounds = (Bounds2f)sampleBounds;
        Point2i p0 = (Point2i)Ceil(floatBounds.pMin - halfPixel - filter->radius);
        Point2i p1 = (Point2i)Floor(floatBounds.pMax - halfPixel + filter->radius) +
                     Point2i(1, 1);
        Bounds2i tilePixelBounds = Intersect(Bounds2i(p0, p1), croppedPixelBounds);
        return std::unique_ptr<FilmTile>(new FilmTile(
                tilePixelBounds, filter->radius, filterTable, filterTableWidth));
    }

    void Film::MergeFilmTile(std::unique_ptr<FilmTile> tile) {
        std::lock_guard<std::mutex> lock(mutex);
        for (Point2i pixel : tile->GetPixelBounds()) {
            // Merge _pixel_ into _Film::pixels_
            const FilmTilePixel &tilePixel = tile->GetPixel(pixel);
            Pixel &mergePixel = GetPixel(pixel);
            float xyz[3];
            tilePixel.contribSum.ToXYZ(xyz);
            for (int i = 0; i < 3; ++i) mergePixel.xyz[i] += xyz[i];
            mergePixel.filterWeightSum += tilePixel.filterWeightSum;
        }
    }

    Film *CreateFilm(std::unique_ptr<Filter> filter) {
        std::string filename;
        filename = PbrtOptions.imageFile;
        int xres = 1024;
        int yres = 768;
        Bounds2f crop;
        crop = Bounds2f(Point2f(Clamp(PbrtOptions.cropWindow[0][0], 0, 1),
                                Clamp(PbrtOptions.cropWindow[1][0], 0, 1)),
                        Point2f(Clamp(PbrtOptions.cropWindow[0][1], 0, 1),
                                Clamp(PbrtOptions.cropWindow[1][1], 0, 1)));
        return new Film(Point2i(xres,yres),crop,std::move(filter),filename,1);
    }
}
