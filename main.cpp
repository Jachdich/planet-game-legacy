#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "FastNoise.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <vector>

uint32_t nProcGen;
FastNoise noiseGen;

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
    
    float x, y;
    Planet() {}
    Planet(float x, float y) {
        this->x = x;
        this->y = y;
        this->radius = rndInt(8, 28);
        this->numColours = (this->radius - 5) / 23.0 * 3;
        
        this->generationChances = std::vector<double>(this->numColours);
        this->generationColours = std::vector<olc::Pixel>(this->numColours);
        this->generationZValues = std::vector<int>(this->numColours);
        this->generationNoise   = std::vector<double>(this->numColours);
        
        for (int i = 0; i < this->numColours; i++) {
            this->generationChances[i] = rndDouble(0.4, 0.7);
            this->generationColours[i] = rndPixel(0, 255);
            this->generationZValues[i] = rndInt(0, 1000);
            this->generationNoise[i]   = (rand() % 900 + 100) / 1000.0;
        }
        this->baseColour = rndPixel(80, 255);
    }
    
    void draw(olc::PixelGameEngine * e) {
        for (int ya = 0; ya < this->radius * 2; ya++) {
            for (int xa = 0; xa < this->radius * 2; xa++) {
                int x = xa - this->radius;
                int y = ya - this->radius;
                if ((x * x + y * y) >= (this->radius * this->radius)) continue;
          
                float xc = x + this->x;
                float yc = y + this->y;
                
                int r = 0;
                int g = 0;
                int b = 0;
                int total = 0;
                for (int i = 0; i < this->numColours; i++) {
                    if ((noiseGen.GetNoise(xc / this->generationNoise[i], yc / this->generationNoise[i], this->generationZValues[i]) + 1) / 2 > this->generationChances[i]) {
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
    Star(float x, float y, int num, std::vector<Planet> planets, olc::Pixel colour, double radius) {
        this->x = x;
        this->y = y;
        this->num = num;
        this->radius = radius;
        this->planets = planets;
        this->colour = colour;
    }
    
    Star(float x, float y) {
        this->x = x;
        this->y = y;
        
        this->num = rand() % 8;
        this->radius = rand() % 10 + 2;
        this->colour = olc::Pixel(rand() % 100 + 155, rand() % 100 + 155, rand() % 100 + 155);
        
        this->planets = std::vector<Planet>(this->num);
        for (int i = 0; i < this->num; i++) {
            this->planets[i] = Planet(0, 0);
        }
        this->selected = false;
    }
    
    void draw(olc::PixelGameEngine * e, float translateX, float translateY) {
        if (this->selected) {
            e->FillCircle(this->x + translateX, this->y + translateY, this->radius, this->colour);
        } else {
            e->DrawCircle(this->x + translateX, this->y + translateY, this->radius, this->colour);
        }
    }
    
    void select() {
        this->selected = true;
    }
};

class Sector {
public:
    int x, y, r, numStars;
    std::vector<Star> stars;
    Sector() {}
    Sector(int sx, int sy, int sr) {
        this->x = sx;
        this->y = sy;
        this->r = sr;
    }
    
    Star * getStarAt(int x, int y) {
        for (int i = 0; i < numStars; i++) {
            int dx = x - stars[i].x;
            int dy = y - stars[i].y;
            if (dx * dx + dy * dy < stars[i].radius * stars[i].radius) {
                //this->stars[i].select();
                return &this->stars[i];
            }
        }
        return nullptr;
    }
    
    void generate() {
        this->numStars = rand() % 8 + 2;
        this->stars = std::vector<Star>(numStars);
        for (int i = 0; i < this->numStars; i++) {
            this->stars[i] = Star(rand() % this->r + this->x, rand() % this->r + this->y);
        }
    }
    
    void load(std::string filename) {
        
    }
    
    void draw(olc::PixelGameEngine * e, float translateX, float translateY) {
        e->DrawRect(this->x * this->r + translateX, this->y * this->r + translateY, r, r, olc::Pixel(255, 255, 255));
        for (int i = 0; i < this->numStars; i++) {
            this->stars[i].draw(e, translateX + this->x * this->r, translateY + this->y * this->r);
        }
    }
};

class SectorMap {
public:
    std::vector<std::vector<Sector>> cache;
    
    SectorMap() {
        this->cache = std::vector<std::vector<Sector>>(0);
    }
    
    Sector * getSectorAt(int x, int y) {
        if (x < 0 || y < 0) {
            return &this->cache[0][0];
        }
        bool needToGenerate = false;
        if (cache.size() <= y) {
            cache.resize(y + 1);
            cache[y] = std::vector<Sector>();
            needToGenerate = true;
        }
        if (cache[y].size() <= x) {
            cache[y].resize(x + 1);
            needToGenerate = true;
        }
        if (needToGenerate) {
            Sector a(x, y, 256);
            a.generate();

            cache[y][x] = a;
        }
        return &this->cache[y][x];
    }
    
    void draw(olc::PixelGameEngine * e, float translateX, float translateY) {
        int sectorX = -translateX / 256;
        int sectorY = -translateY / 256;
        getSectorAt(sectorX, sectorY)->draw(e, translateX, translateY);
        //getSectorAt(sectorX + 1, sectorY).draw(e, translateX, translateY);
        //getSectorAt(sectorX, sectorY + 1).draw(e, translateX, translateY);
        //getSectorAt(sectorX + 1, sectorY + 1).draw(e, translateX, translateY);
    }
};

class Game : public olc::PixelGameEngine {
public:
	Game() {
		sAppName = "Example";
	}

public:
    int lastMouseX;
    int lastMouseY;
    float translateX = 0;
    float translateY = 0;
    SectorMap map;
    bool galaxyView = true;
    bool planetView = false;
    bool starView = false;

	bool OnUserCreate() override {
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override {
        
		Clear(olc::BLACK);
        if (galaxyView) {
            map.draw(this, this->translateX, this->translateY);
        } else if (starView) {
            
        } else if (planetView) {
            
        }
        
        if (GetMouse(0).bPressed) {
            if (galaxyView) {
                Sector * s = map.getSectorAt((GetMouseX() - translateX) / 256, (GetMouseY() - translateY) / 256);
                Star * st = s->getStarAt(GetMouseX() - translateX, GetMouseY() - translateY);
                if (st != NULL) {
                    st->select();
                }
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
        return true;
    }
};

int main() {
    srand((unsigned)time(NULL));
    nProcGen = rand();
    noiseGen.SetNoiseType(FastNoise::Simplex);
	Game app;
	if (app.Construct(256, 256, 2, 2))
		app.Start();
	return 0;
}
