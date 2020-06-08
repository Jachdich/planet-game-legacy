#ifndef __STAR_H
#define __STAR_H
#include "generation.h"
#include "planet.h"
#include <jsoncpp/json/json.h>
class Star {
public:
    double radius;
    Pixel colour;
    std::vector<Planet> planets;
    float x = 0;
    float y = 0;
    int num = 0;

    Star();
    Star(float x, float y);
    Json::Value asJson();
    
};
#endif