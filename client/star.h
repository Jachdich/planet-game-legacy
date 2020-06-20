#ifndef __STAR_H
#define __STAR_H

#include <jsoncpp/json/json.h>
#include "planet.h"
#include "olcPixelGameEngine.h"

class Star {
public:
    double radius;
    olc::Pixel colour;
    std::vector<Planet> planets;
    float x = 0;
    float y = 0;
    int num = 0;
    bool selected;
    int randomDissapearChance;
    
    Star();
    Star(Json::Value root);
    
    void drawWithPlanets(olc::PixelGameEngine * e, float fElapsedTime, CamParams trx);
    void draw(olc::PixelGameEngine * e, CamParams trx, int secOffsetX, int secOffsetY);
    void select();
    Planet * getPlanetAt(int x, int y, CamParams trx);
};
#endif