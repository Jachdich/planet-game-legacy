#include "sectormap.h"
#include "sector.h"
#include "server.h"
#include <mutex>

SectorMap::SectorMap() {
    this->cache = std::vector<std::vector<Sector>>(0);
}

Sector * SectorMap::getSectorAt(int x, int y) {
    std::lock_guard<std::mutex> lock(m);
    if (x < 0 || y < 0) {
        return &this->cache[0][0];
    }
    bool needToGenerate = false;
    if ((signed)cache.size() <= y) {
        cache.resize(y + 1);
        cache[y] = std::vector<Sector>();
        needToGenerate = true;
    }
    if ((signed)cache[y].size() <= x) {
        cache[y].resize(x + 1);
        needToGenerate = true;
    }
    if (needToGenerate || !this->cache[y][x].generated) {
        Sector a(x, y, 256);
        a.generate();

        cache[y][x] = a;
    }
    return &this->cache[y][x];
}