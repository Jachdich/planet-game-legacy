#ifndef __SPRITES_H
#define __SPRITES_H
#include "olcPixelGameEngine.h"
#include <vector>

void loadSprites();
extern std::vector<olc::Sprite *> sprites;
extern std::vector<olc::Decal *> decals;

#endif