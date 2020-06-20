#ifndef __SECTORCACHE_H
#define __SECTORCACHE_H

#include "olcPixelGameEngine.h"
#include "sector.h"
#include "helperfunctions.h"
#include <asio.hpp>
#include <unordered_map>

using asio::ip::tcp;

class SectorCache {
public:
    int secNum = 6;
    std::unordered_map<long long, Sector> secs;
    tcp::socket * sock;
    
    SectorCache(tcp::socket * sock);
    void getSectorFromNetwork(int x, int y);
    void setSectorAt(int sx, int sy, Sector s);
    Sector * getSectorAt(int x, int y);
    void draw(olc::PixelGameEngine * e, CamParams trx);
};
#endif