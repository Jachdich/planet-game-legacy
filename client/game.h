#ifndef __GAME_H
#define __GAME_H
#include "olcPixelGameEngine.h"
#include "helperfunctions.h"

class Game : public olc::PixelGameEngine {
public:
    std::vector<std::string> args;
    int lastMouseX;
    int lastMouseY;
    CamParams galaxyTrx;
    CamParams trx;
    Star * selectedStar = nullptr;
    Planet * selectedPlanet = nullptr;
    bool galaxyView = true;
    bool planetView = false;
    bool starView = false;
    asio::io_context io_context;
    tcp::socket sock;
    SectorCache map;
    std::thread * netThread;
    
    Game(int argc, char ** argv);
    
    void drawTile(int x, int y, int thing, olc::PixelGameEngine * e);
    void destruct();
	bool OnUserCreate() override;
	bool OnUserUpdate(float fElapsedTime) override;
    void zoom(int count);
};

#endif
