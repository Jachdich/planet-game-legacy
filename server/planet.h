#ifndef __PLANET_H
#define __PLANET_H
#include "generation.h"
#include "planetsurface.h"
#include <jsoncpp/json/json.h>

class Planet {
public:
    double mass;
    int radius;
    int numColours;
    
    double * generationChances;
    Pixel * generationColours;
    int * generationZValues;
    double * generationNoise;
    
    PlanetSurface * surface;
    
    Pixel baseColour;
    int posFromStar;
    double theta;
    double angularVelocity;

    Planet();
    Planet(int posFromStar);
    Json::Value asJson();
    PlanetSurface * getSurface();
};
#endif
