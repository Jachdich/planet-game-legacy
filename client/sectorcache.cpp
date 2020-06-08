#include "sectorcache.h"
#include "olcPixelGameEngine.h"
#include "client.h"
#include <asio.hpp>

//PLEASE DELETE TODO
#include <iostream>

using asio::ip::tcp;


SectorCache::SectorCache(tcp::socket * sock) {
    this->sock = sock;
    x = 0;
    y = 0;
    secs.resize(secNum * secNum);
}

void SectorCache::getSectorFromNetwork(int x, int y) {
    std::cout << "Requesting sector " << x << " " << y << "\n";
    Json::Value json;
    json["request"] = "getSector";
    json["x"] = x;
    json["y"] = y;
    std::lock_guard<std::mutex> lock(netq_mutex);
    netRequests.push_back(json);
    netq.notify_all();
}

void SectorCache::setSectorAt(int sx, int sy, Sector s) {
    std::lock_guard<std::mutex> lock(cache_mutex);
    int localX = sx - x;
    int localY = sy - y;
    if (localX + localY * secNum < secNum * secNum) {
        secs[localX + localY * secNum] = s;
    }
}

void SectorCache::shiftLeft() {
    std::unique_lock<std::mutex> lk(cache_mutex);
    for (int y = 0; y < secNum; y++) {
        for (int x = 1; x < secNum; x++) {
            secs[y * secNum + x - 1] = secs[y * secNum + x];
        }
        secs[y * secNum + secNum - 1].generated = false;
    }
    this->x += 1;
    lk.unlock();
}

void SectorCache::shiftRight() {
    std::unique_lock<std::mutex> lk(cache_mutex);
    for (int y = 0; y < secNum; y++) {
        for (int x = secNum - 1; x > 0; x--) {
            secs[y * secNum + x] = secs[y * secNum + x - 1];
        }
        secs[y * secNum].generated = false;
    }
    this->x -= 1;
    lk.unlock();
}

void SectorCache::shiftUp() {
    std::unique_lock<std::mutex> lk(cache_mutex);
    for (int x = 0; x < secNum; x++) {
        for (int y = 1; y < secNum; y++) {
            secs[(y - 1) * secNum + x] = secs[y * secNum + x];
        }
        secs[(secNum - 1) * secNum + x].generated = false;
    }
    this->y += 1;
    lk.unlock();
}

void SectorCache::shiftDown() {
    std::unique_lock<std::mutex> lk(cache_mutex);
    for (int x = 0; x < secNum; x++) {
        for (int y = secNum - 1; y > 0; y--) {
            secs[y * secNum + x] = secs[(y - 1) * secNum + x];
        }
        secs[x].generated = false;
    }
    lk.unlock();
    this->y -= 1;
}

Sector * SectorCache::getSectorAt(int sx, int sy) {
    int localX = sx - x;
    int localY = sy - y;
    
    if (localX > 5) {
        if (localX > 6) {
            secs.clear();
            x = sx + 5;
        } else {
            shiftLeft();
        }
    }
    if (localY > 5) {
        if (localY > 6) {
            secs.clear();
            y = sy + 5;
        } else {
            shiftUp();
        }
    }
    
    if (localX < 1) {
        if (localX < 0) {
            secs.clear();
            x = sx + 1;
        } else {
            shiftRight();
        }
    }
    
    if (localY < 1) {
        if (localY < 0) {
            secs.clear();
            y = sy - 5;
        } else {
            shiftDown();
        }
    }
    
    localX = sx - x;
    localY = sy - y;
    
    int index = localY * secNum + localX;
    if (!secs[index].generated && !secs[index].requested) {
        getSectorFromNetwork(sx, sy);
        secs[index].setRequested();
    }
    return &secs[index];
}

void SectorCache::draw(olc::PixelGameEngine * e, float translateX, float translateY) {
    int sectorX = -translateX / 256;
    int sectorY = -translateY / 256;
    float num = ceil(WIDTH / 256.0);
    //std::lock_guard<std::mutex> lock(cache_mutex);
    for (int x = 0; x < num + 1; x++) {
        for (int y = 0; y < num + 1; y++) {
            Sector * s = getSectorAt(sectorX + x, sectorY + y);
            if (s->generated) {
                s->draw(e, translateX, translateY);
            }
        }
    }
}
