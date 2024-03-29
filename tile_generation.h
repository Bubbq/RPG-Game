// contains consts and methods needed for tile generation
#include <iostream>
#include <raylib.h>
#include <string>
#include <vector>
#include <fstream>

// size of one item in src png
const int TILE_SIZE = 16;
// making image from src png proportionate to the screen
const float SCALE = 2.0f;
const float SCREEN_TILE_SIZE = TILE_SIZE * SCALE;

const std::string TILE_ASSET_PATH = "asset/character and tileset/Dungeon_Tileset_v2.png";
const std::string ITEM_ASSET_PATH = "asset/character and tileset/Dungeon_item_props_v2.png";

// representing the types of tiles a user can generate
enum Element{
    WALL = 0,
    ITEM = 1,
    SPRITE = 2,
    FLOOR = 3,
};

// representing a single tile in user created world
struct Tile{
    // position in asset png
    Vector2 src;
    Vector2 screenPos;
    std::string name;
    Element tileType;
};

// representing the current world a user is creating
struct World{
    std::vector<Tile>walls;
    std::vector<Tile>floors;
    std::vector<Tile>items;
    Vector2 spawn;
};

// load world content from file
 void loadWorld(World&, int&);

void drawWorld(World&, Texture2D, Texture2D, int);

void saveWorld(World&, int&);