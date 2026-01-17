#ifndef VIDEOGAME_H
#define VIDEOGAME_H

#include <string>
#include <catalogueItem.h>

class VideoGame : public CatalogueItem {
public:
    VideoGame(const std::string& t, const std::string& a, int y, int i,
              const std::string& f, const std::string& c,
              const std::string& s,
              const std::string& g, const std::string& r)
        : CatalogueItem(t, a, y, i, f, c, s), genre(g), ratingInfo(r) {};

    std::string get_genre() { return genre; }
    std::string get_rating(){ return ratingInfo; }
private:
    std::string genre;
    std::string ratingInfo;
};

#endif // VIDEOGAME_H
