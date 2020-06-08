#include <asio.hpp>

#include "olcPixelGameEngine.h"
#include "sectorcache.h"
#include "sector.h"
#include "star.h"
#include "game.h"
#include "client.h"
#include "network.h"

Game::Game(int argc, char ** argv) : io_context(), sock(io_context), map(&sock) {
    sAppName = "Example";
    for (int i = 0; i < argc; i++) {
        args.push_back(std::string(argv[i]));
    }
}

void Game::destruct() {
    
}

bool Game::OnUserCreate() {
    std::string address = "127.0.0.1";
    if (args.size() > 1) {
        address = args[1];
    }
    
    tcp::resolver resolver(io_context);
    tcp::resolver::results_type endpoints =
      resolver.resolve(address, "5555");
    
    asio::connect(sock, endpoints);
    
    std::thread(handleNetwork, &sock, &map).detach();        
    return true;
}

bool Game::OnUserUpdate(float fElapsedTime) {

    Clear(olc::BLACK);
    if (galaxyView) {
        map.draw(this, this->translateX, this->translateY);
    } else if (starView) {
        selectedStar->drawWithPlanets(this, fElapsedTime, translateX, translateY);
    } else if (planetView) {

    }

    if (GetMouse(0).bPressed) {
            Sector * s = map.getSectorAt(floor((GetMouseX() - translateX) / 256), floor((GetMouseY() - translateY) / 256));
            std::cout << s->requested << " " << s->x << " " << s->y << "\n";
            Star * st = s->getStarAt(
                    GetMouseX() - translateX - floor((GetMouseX() - translateX) / 256) * 256,
                    GetMouseY() - translateY - floor((GetMouseY() - translateY) / 256) * 256);
            if (st != nullptr) {
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
    
    if (netRequests.size() > 0) {
        std::lock_guard<std::mutex> lock(netq_mutex);
        netq.notify_all();
    }

    return true;
}