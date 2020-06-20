#ifndef __PLANET_H
#define __PLANET_H
#include <jsoncpp/json/json.h>
#include "olcPixelGameEngine.h"
#include "planetsurface.h"
#include "helperfunctions.h"

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
    
    PlanetSurface surface;
    
    Planet();
    Planet(Json::Value res);
    void draw(olc::PixelGameEngine * e, double x, double y, CamParams trx);
    void drawSurface(olc::PixelGameEngine * e, CamParams trx);
};

#endif