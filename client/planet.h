#ifndef __PLANET_H
#define __PLANET_H
#include <jsoncpp/json/json.h>
#include "olcPixelGameEngine.h"

class Planet {
public:
    double mass;
    int radius;
    int numColours;
    std::vector<double> generationChances;
    std::vector<olc::Pixel> generationColours;
    std::vector<int> generationZValues;
    std::vector<double> generationNoise;
    olc::Pixel baseColour;
    int posFromStar;
    double theta;
    double angularVelocity;
    
    Planet();
    Planet(Json::Value res);
    void draw(olc::PixelGameEngine * e, float x, float y);
};

#endif