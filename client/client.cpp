#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "FastNoise.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include <math.h>
#include <string>
#include <iostream>
#include <json/json.h>
#include <fstream>
#include <asio.hpp>
#include <thread>

using asio::ip::tcp;
//TODO
//planet noise generation settings
//tidally lock planets/have planets rotate
//star colours/textures
//planets go offscreen
//move around on starsystem view
//starsystem view scrolls on star view

uint32_t nProcGen;
FastNoise noiseGen;

const int WIDTH = 960;
const int HEIGHT = 540;

uint32_t rnd() {
    nProcGen += 0xe120fc15;
    uint64_t tmp;
    tmp = (uint64_t)nProcGen * 0x4a39b70d;
    uint32_t m1 = (tmp >> 32) ^ tmp;
    tmp = (uint64_t)m1 * 0x12fad5c9;
    uint32_t m2 = (tmp >> 32) ^ tmp;
    return m2;
}

double rndDouble(double min, double max) {
    return ((double)rnd() / (double)(0x7FFFFFFF)) * (max - min) + min;
}

int rndInt(int min, int max) {
    return (rnd() % (max - min)) + min;
}

olc::Pixel rndPixel(int low, int high) {
    return olc::Pixel(rndInt(low, high), rndInt(low, high), rndInt(low, high));
}

Json::Value makeJSON(std::string x) {
    Json::CharReaderBuilder builder;
    Json::CharReader* reader = builder.newCharReader();

    Json::Value root;
    std::string errors;

    bool parsingSuccessful = reader->parse(
        x.c_str(),
        x.c_str() + x.size(),
        &root,
        &errors
    );
    delete reader;
    return root;
}

class Planet {
public:
    double mass;
    int radius;
    int numColours;
    std::vector<double> generationChances;
    std::vector<olc::Pixel> generationColours;
    std::vector<int> generationZValues;
    std::vector<double> generationNoise;
    olc::Pixel baseColour;
    int posFromStar;
    double theta;
    double angularVelocity;

    Planet() {}
    Planet(Json::Value res) {
        mass = res["mass"].asDouble();
        radius = res["radius"].asInt();
        numColours = res["numColours"].asInt();
        int col = res["baseColour"].asInt();
        baseColour = olc::Pixel(col >> 16, (col >> 8) & 0xFF, col & 0xFF);
        posFromStar = res["posFromStar"].asInt();
        theta = res["theta"].asDouble();
        angularVelocity = res["angularVelocity"].asDouble();
        
        this->generationChances = std::vector<double>(this->numColours);
        this->generationColours = std::vector<olc::Pixel>(this->numColours);
        this->generationZValues = std::vector<int>(this->numColours);
        this->generationNoise   = std::vector<double>(this->numColours);
        
        for (int i = 0; i < numColours; i++) {
            int col = res["generationColours"][i].asInt();
            generationColours[i] = olc::Pixel(col >> 16, (col >> 8) & 0xFF, col & 0xFF);
            
            generationChances[i] = res["generationChances"][i].asDouble();
            generationZValues[i] = res["generationZValues"][i].asInt();
            generationNoise[i]   = res["generationNoise"][i].asDouble();
        }
        
    }

    void draw(olc::PixelGameEngine * e, float x, float y) {
        for (int ya = 0; ya < this->radius * 2; ya++) {
            for (int xa = 0; xa < this->radius * 2; xa++) {
                int xb = xa - this->radius;
                int yb = ya - this->radius;
                if ((xb * xb + yb * yb) >= (this->radius * this->radius)) continue;

                float xc = xb + x;
                float yc = yb + y;

                int r = 0;
                int g = 0;
                int b = 0;
                int total = 0;
                for (int i = 0; i < this->numColours; i++) {
                    if ((noiseGen.GetNoise(xb / this->generationNoise[i], yb / this->generationNoise[i], this->generationZValues[i]) + 1) / 2 > this->generationChances[i]) {
                        r += this->generationColours[i].r;
                        g += this->generationColours[i].g;
                        b += this->generationColours[i].b;
                        total += 1;
                    }
                }
                if (total == 0) {
                    r = this->baseColour.r;
                    g = this->baseColour.g;
                    b = this->baseColour.b;
                } else {
                    r /= total;
                    g /= total;
                    b /= total;
                }
                e->Draw(xc, yc, olc::Pixel(r, g, b));
            }
        }
    }
};

class Star {
public:
    double radius;
    olc::Pixel colour;
    std::vector<Planet> planets;
    float x = 0;
    float y = 0;
    int num = 0;
    bool selected;

    Star() {}

    Star(Json::Value root) {
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

    void draw(olc::PixelGameEngine * e, float translateX, float translateY) {
        float ax = this->x + translateX;
        float ay = this->y + translateY;
        if (ax >= (WIDTH + radius) || ay >= (HEIGHT + radius) || ax <= (0 - radius) || ay <= (0 - radius)) {
            return;
        }
        e->FillCircle(ax, ay, this->radius, this->colour);
    }

    void drawWithPlanets(olc::PixelGameEngine * e, float fElapsedTime, float translateX, float translateY) {
        e->FillCircle(WIDTH / 2 + translateX, HEIGHT / 2 + translateY, this->radius * 6, this->colour);
        for (Planet & p: this->planets) {
            p.theta += p.angularVelocity * fElapsedTime;
            float ax = p.posFromStar * cos(p.theta) + translateX;
            float ay = p.posFromStar * sin(p.theta) + translateY;
            e->DrawCircle(WIDTH / 2 + translateX, HEIGHT / 2 + translateY, p.posFromStar, olc::Pixel(60, 60, 60, 200));
            p.draw(e, ax + WIDTH / 2, ay + HEIGHT / 2);
        }
    }

    void select() {
        this->selected = true;
    }
};

class Sector {
public:
    int x, y, r, numStars;
    bool generated = false;
    std::vector<Star> stars;
    Sector() {
        
    }
    
    Sector(Json::Value root) {
        this->x = root["x"].asInt();
        this->y = root["y"].asInt();
        this->r = root["r"].asInt();
        this->generated = true;
        this->numStars = root["numStars"].asInt();
        for (int i = 0; i < numStars; i++) {
            stars.push_back(Star(root["stars"][i]));
        }
        
    }

    Star * getStarAt(int x, int y) {
        for (int i = 0; i < numStars; i++) {
            int dx = x - stars[i].x;
            int dy = y - stars[i].y;
            if (dx * dx + dy * dy < stars[i].radius * stars[i].radius) {
                return &this->stars[i];
            }
        }
        return nullptr;
    }

    void draw(olc::PixelGameEngine * e, float translateX, float translateY) {
        e->DrawRect(this->x * this->r + translateX, this->y * this->r + translateY, r, r, olc::Pixel(255, 255, 255));
        for (int i = 0; i < this->numStars; i++) {
            this->stars[i].draw(e, translateX + this->x * this->r, translateY + this->y * this->r);
        }
    }
};

class SectorCache {
public:
    int x = 0;
    int y = 0;
    int secNum = 6;
    std::vector<Sector> secs;
    tcp::socket * sock;
    SectorCache(tcp::socket * sock) {
        this->sock = sock;
        x = 0;
        y = 0;
        secs.resize(secNum * secNum);
        /*
        for (int a = 0; a < secNum; a++) {
            for (int b = 0; b < secNum; b++) {
                int index = a * secNum + b;
                secs[index] = getSectorFromNetwork(x, y);
            }
        }*/
    }
    
    Sector getSectorFromNetwork(int x, int y) {
        asio::streambuf buf;
        asio::error_code error;
        
        Json::Value json;
        json["request"] = "getSector";
        json["x"] = x;
        json["y"] = y;
        Json::StreamWriterBuilder builder;
        builder["indentation"] = ""; 
        const std::string output = Json::writeString(builder, json);
        asio::write(*sock, asio::buffer(output + "\n"), error);
        size_t len = asio::read_until(*sock, buf, "\n");
        std::istream is(&buf);
        std::string line;
        std::getline(is, line);
        if (error && error != asio::error::eof) {
            throw asio::system_error(error);
        }
        
        Json::Value root = makeJSON(line);
        return Sector(root["result"]);
    }
    
    void shiftLeft() {
        for (int y = 0; y < secNum; y++) {
            for (int x = 1; x < secNum; x++) {
                secs[y * secNum + x - 1] = secs[y * secNum + x];
            }
            secs[y * secNum + secNum - 1].generated = false;
        }
        this->x += 1;
    }
    
    void shiftRight() {
        for (int y = 0; y < secNum; y++) {
            for (int x = secNum - 1; x > 0; x--) {
                secs[y * secNum + x] = secs[y * secNum + x - 1];
            }
            secs[y * secNum].generated = false;
        }
        this->x -= 1;
    }
    
    void shiftUp() {
        for (int x = 0; x < secNum; x++) {
            for (int y = 1; y < secNum; y++) {
                secs[(y - 1) * secNum + x] = secs[y * secNum + x];
            }
            secs[(secNum - 1) * secNum + x].generated = false;
        }
        this->y += 1;
    }
    
    void shiftDown() {
        for (int x = 0; x < secNum; x++) {
            for (int y = secNum - 1; y > 0; y--) {
                secs[y * secNum + x] = secs[(y - 1) * secNum + x];
            }
            secs[x].generated = false;
        }
        this->y -= 1;
    }
    
    Sector * getSectorAt(int sx, int sy) {
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
        Sector sec = secs[index];
        if (!sec.generated) {
            secs[index] = getSectorFromNetwork(sx, sy);
        }
        return &secs[index];
    }
    
    void draw(olc::PixelGameEngine * e, float translateX, float translateY) {
        int sectorX = -translateX / 256;
        int sectorY = -translateY / 256;
        float num = ceil(WIDTH / 256.0);
        for (int x = 0; x < num + 1; x++) {
            for (int y = 0; y < num + 1; y++) {
                //std::cout << x << " " << y << "\n";
                getSectorAt(sectorX + x, sectorY + y)->draw(e, translateX, translateY);
            }
        }
    }
};

class Game : public olc::PixelGameEngine {
public:
	Game(int argc, char ** argv) : io_context(), sock(io_context), map(&sock) {
		sAppName = "Example";
        for (int i = 0; i < argc; i++) {
            args.push_back(std::string(argv[i]));
        }
	}

public:
    std::vector<std::string> args;
    int lastMouseX;
    int lastMouseY;
    float translateX = 0;
    float translateY = 0;
    float galaxyTranslateX = 0;
    float galaxyTranslateY = 0;
    SectorCache map;
    Star * selectedStar = nullptr;
    bool galaxyView = true;
    bool planetView = false;
    bool starView = false;
    asio::io_context io_context;
    tcp::socket sock;

	bool OnUserCreate() override {
        
        std::string address = "127.0.0.1";
        if (args.size() > 1) {
            address = args[1];
        }
        
        tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints =
          resolver.resolve(address, "5555");
        
        asio::connect(sock, endpoints);
        return true;
	}

	bool OnUserUpdate(float fElapsedTime) override {

		Clear(olc::BLACK);
        if (galaxyView) {
            map.draw(this, this->translateX, this->translateY);
        } else if (starView) {
            selectedStar->drawWithPlanets(this, fElapsedTime, translateX, translateY);
        } else if (planetView) {

        }

        if (GetMouse(0).bPressed) {
//            std::cout << "Real coords: " << (int)GetMouseX() - translateX - floor((GetMouseX() - translateX) / 256) * 256 << " " << (int)GetMouseY() - translateY - floor((GetMouseY() - translateY) / 256) * 256
//            << ", Screen coords: " << GetMouseX() << " " << GetMouseY()
//            << ", Sector coords: " << floor((GetMouseX() - translateX) / 256) << floor((GetMouseY() - translateY) / 256) << "\n";
                Sector * s = map.getSectorAt(floor((GetMouseX() - translateX) / 256), floor((GetMouseY() - translateY) / 256));
                Star * st = s->getStarAt(
                        GetMouseX() - translateX - floor((GetMouseX() - translateX) / 256) * 256,
                        GetMouseY() - translateY - floor((GetMouseY() - translateY) / 256) * 256);
                if (st != NULL) {
                    this->selectedStar = st;
                    this->starView = true;
                    this->galaxyView = false;
                    galaxyTranslateX = translateX;
                    galaxyTranslateY = translateY;
                    translateX = 0;
                    translateY = 0;
                }

        }

        if (GetMouse(1).bPressed) {
            lastMouseX = GetMouseX();
            lastMouseY = GetMouseY();
        }

        if (GetMouse(1).bHeld) {
            int offX = GetMouseX() - lastMouseX;
            int offY = GetMouseY() - lastMouseY;
            translateX += offX;
            translateY += offY;
            lastMouseX = GetMouseX();
            lastMouseY = GetMouseY();
        }
        
        if (GetKey(olc::Key::L).bPressed) {
            std::cout << "L pressed\n";
        }

        if (GetKey(olc::Key::ESCAPE).bPressed) {
            if (starView) {
                starView = false;
                galaxyView = true;
                translateX = galaxyTranslateX;
                translateY = galaxyTranslateY;
                this->selectedStar = nullptr;
            } else if (galaxyView) {
                return false;
            }
        }

        return true;
    }
};

int main(int argc, char ** argv) {
    const unsigned int LEVEL_SEED = 12345;
    srand(LEVEL_SEED);
    nProcGen = rand();
    noiseGen.SetNoiseType(FastNoise::Simplex);
	Game app(argc, argv);
	if (app.Construct(WIDTH, HEIGHT, 2, 2))
		app.Start();
	return 0;
}
