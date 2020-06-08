#ifndef __CLIENT_H
#define __CLIENT_H
#include "FastNoise.h"
#include <string.h>
#include <jsoncpp/json/json.h>
extern FastNoise noiseGen;
extern int WIDTH;
extern int HEIGHT;
extern std::mutex cache_mutex;
extern std::mutex netq_mutex;
extern std::condition_variable netq;
extern std::vector<Json::Value> netRequests;
Json::Value makeJSON(std::string x);
#endif