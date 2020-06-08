#ifndef __PLANET_H
#define __PLANET_H
#include "generation.h"
#include <jsoncpp/json/json.h>

class Planet {
public:
    double mass;
    int radius;
    int numColours;
    std::vector<double> generationChances;
    std::vector<Pixel> generationColours;
    std::vector<int> generationZValues;
    std::vector<double> generationNoise;
    Pixel baseColour;
    int posFromStar;
    double theta;
    double angularVelocity;

    Planet();
    Planet(int posFromStar);
    Json::Value asJson();
};
#endif