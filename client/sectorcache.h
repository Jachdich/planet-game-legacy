#ifndef __SECTORCACHE_H
#define __SECTORCACHE_H

#include "olcPixelGameEngine.h"
#include "sector.h"
#include <asio.hpp>
using asio::ip::tcp;

class SectorCache {
public:
    int x = 0;
    int y = 0;
    int secNum = 6;
    std::vector<Sector> secs;
    tcp::socket * sock;
    
    SectorCache(tcp::socket * sock);
    void getSectorFromNetwork(int x, int y);
    void setSectorAt(int sx, int sy, Sector s);
    void shiftLeft();
    void shiftRight();
    void shiftUp();
    void shiftDown();
    Sector * getSectorAt(int sx, int sy);
    void draw(olc::PixelGameEngine * e, float translateX, float translateY);
};
#endif