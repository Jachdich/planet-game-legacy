#ifndef __GAME_H
#define __GAME_H
#include "olcPixelGameEngine.h"

class Game : public olc::PixelGameEngine {
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
    std::thread * netThread;
    
    Game(int argc, char ** argv);
    void destruct();
	bool OnUserCreate() override;
	bool OnUserUpdate(float fElapsedTime) override;
};

#endif