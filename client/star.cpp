#include "olcPixelGameEngine.h"

#include "star.h"
#include "planet.h"
#include "client.h"

Star::Star() {}

Star::Star(Json::Value root) {
    x = root["x"].asInt();
    y = root["y"].asInt();
    num = root["num"].asInt();
    int col = root["colour"].asInt();
    colour = olc::Pixel(col >> 16, (col >> 8) & 0xFF, col & 0xFF);
    radius = root["radius"].asInt();
    for (int i = 0; i < num; i++) {
        planets.push_back(Planet(root["planets"][i]));
    }
    this->selected = false;
}

void Star::draw(olc::PixelGameEngine * e, float translateX, float translateY) {
    float ax = this->x + translateX;
    float ay = this->y + translateY;
    if (ax >= (WIDTH + radius) || ay >= (HEIGHT + radius) || ax <= (0 - radius) || ay <= (0 - radius)) {
        return;
    }
    e->FillCircle(ax, ay, this->radius, this->colour);
}

void Star::drawWithPlanets(olc::PixelGameEngine * e, float fElapsedTime, float translateX, float translateY) {
    e->FillCircle(WIDTH / 2 + translateX, HEIGHT / 2 + translateY, this->radius * 6, this->colour);
    for (Planet & p: this->planets) {
        p.theta += p.angularVelocity * fElapsedTime;
        float ax = p.posFromStar * cos(p.theta) + translateX;
        float ay = p.posFromStar * sin(p.theta) + translateY;
        e->DrawCircle(WIDTH / 2 + translateX, HEIGHT / 2 + translateY, p.posFromStar, olc::Pixel(60, 60, 60, 200));
        p.draw(e, ax + WIDTH / 2, ay + HEIGHT / 2);
    }
}

void Star::select() {
    this->selected = true;
}