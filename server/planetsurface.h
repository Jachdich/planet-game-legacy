#ifndef __PLANETSURFACE_H
#define __PLANETSURFACE_H
#include <jsoncpp/json/json.h>
#include <vector>

class PlanetSurface {
public:
    std::vector<int> tiles;
    bool generated = false;
    
    PlanetSurface();
    void generate(int rad);
    Json::Value asJson();
};
    
#endif
