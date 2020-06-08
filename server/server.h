#ifndef __SERVER_H
#define __SERVER_H
#include "logging.h"
#include <jsoncpp/json/json.h>
#include <mutex>

extern std::mutex m;
extern Json::Value generation_config;
extern Logger logger;
#endif