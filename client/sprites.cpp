#include "olcPixelGameEngine.h"
#include "sprites.h"

std::vector<olc::Sprite *> sprites;
std::vector<olc::Decal *> decals;

void loadSprites() {
    sprites.push_back(new olc::Sprite("ground_iso.png"));
    decals.push_back(new olc::Decal(sprites[sprites.size() - 1]));
    
    sprites.push_back(new olc::Sprite("tree_iso.png"));
    decals.push_back(new olc::Decal(sprites[sprites.size() - 1]));
}