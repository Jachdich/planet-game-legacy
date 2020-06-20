#include "planetsurface.h"
#include <jsoncpp/json/json.h>

PlanetSurface::PlanetSurface() {
    generated = false;
}
void PlanetSurface::generate(int rad) {
    generated = true;
    for (int i = 0; i < rad * 2; i++) {
        for (int j = 0; j < rad * 2; j++) {
            tiles.push_back(0);
        }
    }
}

Json::Value PlanetSurface::asJson() {
    Json::Value res;
    for (int i = 0; i < tiles.size(); i++) {
        res["tiles"].append(tiles[i]);
    }
    return Json::Value();
}
