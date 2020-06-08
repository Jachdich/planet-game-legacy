#include "planet.h"
#include "generation.h"
#include <jsoncpp/json/json.h>

Planet::Planet() {}
Planet::Planet(int posFromStar) {
    this->theta = (rndInt(0, 360) / 180.0) * 3.14159265358979323;
    this->posFromStar = posFromStar;
    this->radius = rndInt(genConf["p_radMin"].asInt(), genConf["p_radMax"].asInt());
    this->numColours = (this->radius - 5) / (genConf["p_radMax"].asInt() - 5.0) * 3;

    this->generationChances = std::vector<double>(this->numColours);
    this->generationColours = std::vector<Pixel>(this->numColours);
    this->generationZValues = std::vector<int>(this->numColours);
    this->generationNoise   = std::vector<double>(this->numColours);

    for (int i = 0; i < this->numColours; i++) {
        this->generationChances[i] = rndDouble(genConf["p_genChanceMin"].asDouble(), genConf["p_genChanceMax"].asDouble());
        this->generationColours[i].rand();
        this->generationZValues[i] = rndInt(0, 1000);
        this->generationNoise[i]   = rndDouble(genConf["p_genNoiseMin"].asDouble(), genConf["p_genNoiseMax"].asDouble());
    }
    this->baseColour.rand(genConf["p_baseColMin"].asInt() % 256, genConf["p_baseColMax"].asInt() % 256);
    this->angularVelocity = 1.0 / (posFromStar * posFromStar) * genConf["p_angularVelMultiplier"].asDouble();
}

Json::Value Planet::asJson() {
    Json::Value res;
    res["mass"] = mass;
    res["radius"] = radius;
    res["numColours"] = numColours;
    res["baseColour"] = baseColour.asInt();
    res["posFromStar"] = posFromStar;
    res["theta"] = theta;
    res["angularVelocity"] = angularVelocity;
    
    for (int i = 0; i < numColours; i++) {
        res["generationColours"].append(generationColours[i].asInt());
        res["generationChances"].append(generationChances[i]);
        res["generationZValues"].append(generationZValues[i]);
        res["generationNoise"].append(generationNoise[i]);
    }
    
    return res;
}