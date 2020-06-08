#include "sector.h"
#include "star.h"
#include "olcPixelGameEngine.h"

Sector::Sector() {}

void Sector::setRequested() {
    requested = true;
}

Sector::Sector(Json::Value root) {
    this->x = root["x"].asInt();
    this->y = root["y"].asInt();
    this->r = root["r"].asInt();
    this->generated = true;
    this->requested = false;
    this->numStars = root["numStars"].asInt();
    for (int i = 0; i < numStars; i++) {
        stars.push_back(Star(root["stars"][i]));
    }
    
}

Star * Sector::getStarAt(int x, int y) {
    for (int i = 0; i < numStars; i++) {
        int dx = x - stars[i].x;
        int dy = y - stars[i].y;
        if (dx * dx + dy * dy < stars[i].radius * stars[i].radius) {
            return &this->stars[i];
        }
    }
    return nullptr;
}

void Sector::draw(olc::PixelGameEngine * e, float translateX, float translateY) {
    e->DrawRect(this->x * this->r + translateX, this->y * this->r + translateY, r, r, olc::Pixel(255, 255, 255));
    for (int i = 0; i < this->numStars; i++) {
        this->stars[i].draw(e, translateX + this->x * this->r, translateY + this->y * this->r);
    }
}