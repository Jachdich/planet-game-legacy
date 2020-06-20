#ifndef __SECTOR_H
#define __SECTOR_H
#include "star.h"
#include "olcPixelGameEngine.h"
#include "helperfunctions.h"
#include <jsoncpp/json/json.h>

class Sector {
public:
    int x, y, r, numStars;
    bool generated = false;
    bool requested = false;
    std::vector<Star> stars;
    
    Sector();
    void setRequested();
    Sector(Json::Value root);
    Star * getStarAt(int x, int y);
    void draw(olc::PixelGameEngine * e, CamParams trx);
};
#endif