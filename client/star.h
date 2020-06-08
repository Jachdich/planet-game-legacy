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
    
    Star();
    Star(Json::Value root);
    
    void drawWithPlanets(olc::PixelGameEngine * e, float fElapsedTime, float translateX, float translateY);
    void draw(olc::PixelGameEngine * e, float translateX, float translateY);
    void select();
};
#endif