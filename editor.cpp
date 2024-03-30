#include <raylib.h>
#include <filesystem>
#include <raymath.h>
#include <string>
#include <vector>
#include <algorithm>
#include "tile_generation.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

namespace fs = std::filesystem;

const int WINDOW_SIZE = 992;
const int TILES_PER_ROW = WINDOW_SIZE / (SCREEN_TILE_SIZE);

Texture2D TILES;
Texture2D ITEMS;

World world;
bool spawnPoint = false;

Rectangle scaleRect(Rectangle rect)
{
    rect.x *= SCALE;
    rect.y *= SCALE;
    rect.width *= SCALE;
    rect.height *= SCALE;
    return rect;
}

// displays every tile availible in asset png
void displayTiles(std::vector<Tile>& allTiles, Tile& currTile)
{
    // position of ith tile
    Vector2 currPos = {-TILE_SIZE, 0};

    for(int i = 0; i < (int)allTiles.size(); i++)
    {
        // moving to next row once we've exhausted the current one
        if(i % TILES_PER_ROW == 0 && i !=0)
            currPos = Vector2Add(currPos, {-currPos.x, TILE_SIZE});
        else
            currPos.x += TILE_SIZE;

        // update the current tile upon selection
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            if(CheckCollisionPointRec(GetMousePosition(), scaleRect({currPos.x, currPos.y, TILE_SIZE, TILE_SIZE})))
                currTile = allTiles[i];
            
        if(allTiles[i].tileType == ITEM)
            DrawTexturePro(ITEMS, {allTiles[i].src.x, allTiles[i].src.y, TILE_SIZE, TILE_SIZE}, {currPos.x * SCALE, currPos.y * SCALE, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE}, {0,0}, 0, WHITE);
        else
            DrawTexturePro(TILES, {allTiles[i].src.x, allTiles[i].src.y, TILE_SIZE, TILE_SIZE}, {currPos.x * SCALE, currPos.y * SCALE, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE}, {0,0}, 0, WHITE);
    }
}

// empties tiles of the world
void eraseWorld()
{
    for(int i = 0; i < world.walls.size(); i++)
        world.walls[i] = {};
    for(int i = 0; i < world.floors.size(); i++)
        world.floors[i] = {};
    for(int i = 0; i < world.items.size(); i++)
        world.items[i] = {};
}

// editing the world
void editWorld(std::vector<Tile>& allTiles, Tile& currTile, Rectangle& mapArea, int& cl)
{
    Texture2D tmp;
    // show the user which tile are we drawing with
    if(!currTile.name.empty())
    {
        if(currTile.tileType == ITEM)
            DrawTexturePro(ITEMS, {currTile.src.x, currTile.src.y, TILE_SIZE, TILE_SIZE}, {GetMousePosition().x - 20, GetMousePosition().y - 20, 20, 20}, {0,0}, 0, WHITE);
        else
            DrawTexturePro(TILES, {currTile.src.x, currTile.src.y, TILE_SIZE, TILE_SIZE}, {GetMousePosition().x - 20, GetMousePosition().y - 20, 20, 20}, {0,0}, 0, WHITE);
    }

    // empty all tiles
    if(IsKeyPressed(KEY_E))
        eraseWorld();

    // get the nearest rectangle from the users mouse
    int mpx = (((int)GetMousePosition().x >> (int)log2(SCREEN_TILE_SIZE)) << (int)log2(SCREEN_TILE_SIZE));
    int mpy = (((int)GetMousePosition().y >> (int)log2(SCREEN_TILE_SIZE)) << (int)log2(SCREEN_TILE_SIZE));

    if(CheckCollisionPointRec(GetMousePosition(), mapArea))
    {
        // erasing tiles
        if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        {
            for(int i = 0; i < world.walls.size(); i++)
                if(CheckCollisionPointRec(GetMousePosition(), {world.walls[i].screenPos.x, world.walls[i].screenPos.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE}))
                    world.walls.erase(world.walls.begin() + i);
            for(int i = 0; i < world.floors.size(); i++)
                if(CheckCollisionPointRec(GetMousePosition(), {world.floors[i].screenPos.x, world.floors[i].screenPos.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE}))
                    world.floors.erase(world.floors.begin() + i);
            for(int i = 0; i < world.items.size(); i++)
                if(CheckCollisionPointRec(GetMousePosition(), {world.items[i].screenPos.x, world.items[i].screenPos.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE}))
                    world.items.erase(world.items.begin() + i);
        }

        // updating map with selected tile
        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            // setting spawn point
            if(spawnPoint)
                world.spawn = GetMousePosition();

            else if(currTile.tileType == FLOOR)
            {
                // Check if screenPos already exists in world.floors
                Vector2 newPos = {mpx * 1.0f, mpy * 1.0f};
                auto it = std::find_if(world.floors.begin(), world.floors.end(), [&](const Tile& tile) {
                    return (tile.screenPos.x == newPos.x && tile.screenPos.y == newPos.y);
                });
                if (it == world.floors.end()) {
                    world.floors.push_back({currTile.src, newPos, currTile.name, FLOOR});
                }
            }

            else if(currTile.tileType == WALL)
            {
                // Check if screenPos already exists in world.walls
                Vector2 newPos = {mpx * 1.0f, mpy * 1.0f};
                auto it = std::find_if(world.walls.begin(), world.walls.end(), [&](const Tile& tile) {
                    return (tile.screenPos.x == newPos.x && tile.screenPos.y == newPos.y);
                });
                if (it == world.walls.end()) {
                    world.walls.push_back({currTile.src, newPos, currTile.name, WALL});
                }
            }

            else if(currTile.tileType == ITEM)
            {
                // Check if screenPos already exists in world.items
                Vector2 newPos = {mpx * 1.0f, mpy * 1.0f};
                auto it = std::find_if(world.items.begin(), world.items.end(), [&](const Tile& tile) {
                    return (tile.screenPos.x == newPos.x && tile.screenPos.y == newPos.y);
                });
                if (it == world.items.end()) {
                    world.items.push_back({currTile.src, newPos, currTile.name, ITEM});
                }
            }
        }
    }
}

// finds the amount of levels a user has created to display
void loadLevels(int& lc)
{
    for(const auto entry : fs::directory_iterator("./levels"))
    {
        if(entry.is_directory())
            if(entry.path().string().find("level"))
                lc++;
    }
}

// menu for user to choose from his or her levels
void chooseLevel(int&cl, int& lc)
{
    int offset = 50;
    
    for(int i = 1; i < lc + 1; i++)
    {
        if(GuiButton({float(64 + offset), 96, 45, 30}, ("level " + std::to_string(i)).c_str())){
            cl = i;
            loadWorld(world, cl);
        }
        offset += 50;
    }

    // creating a new level
    if(GuiButton({float(GetScreenWidth() * 0.5 - 25), float(GetScreenHeight() * 0.55), 50, 50}, "CREATE LEVEL"))
    {
        system(("mkdir ./levels/level" + std::to_string(++lc)).c_str());
    }
}

void init(std::vector<Tile>& allTiles, Rectangle& editingArea, Rectangle& mapArea, int& lc, int& cl){
    SetTraceLogLevel(LOG_ERROR);
    SetTargetFPS(120);
    InitWindow(WINDOW_SIZE,WINDOW_SIZE, "Dungeon Fighters!");
    loadLevels(lc);
    readTiles(allTiles);

    TILES = LoadTextureFromImage(LoadImage(TILE_ASSET_PATH.c_str()));
    ITEMS = LoadTextureFromImage(LoadImage(ITEM_ASSET_PATH.c_str()));

    // increase the height of editing area based on the number of tiles to choose from
    for(int i = 0; i < std::ceil(float(allTiles.size())) / TILES_PER_ROW; i++)
    {
        editingArea.height += SCREEN_TILE_SIZE;
        mapArea.y += SCREEN_TILE_SIZE;
    }

    // create levels direcctory if there isint one already
    system("mkdir ./levels");
}

// prevent leak
void deinit(World& world, int& cl)
{
    if(cl != -1)
        saveWorld(world, cl);
    UnloadTexture(TILES);
    UnloadTexture(ITEMS);
    CloseWindow();
}

int main()
{
    // contains the info of every type of tile the asset has
    std::vector<Tile> allTiles;
    // amount of levels a user has created
    int lc = 0;
    // current level a user is on
    int cl = -1;
    // current tile that a user is  adding to the map
    Tile currTile = {};
    // space reserved for selecting tiles
    Rectangle editingArea = {0,0,WINDOW_SIZE, 0};
    // space for map
    Rectangle mapArea = {0,0,WINDOW_SIZE, WINDOW_SIZE};
    
    init(allTiles, editingArea, mapArea,lc, cl);

    while(!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(LIGHTGRAY);

        // case one, user has no levels saved, prompt them to create one
        if(lc == 0)
        {
            ClearBackground(BLACK);
            DrawText("YOU DONT HAVE ANY LEVELS TO CHOOSE FROM, CREATE ONE?", GetScreenWidth() * 0.5 - MeasureText("YOU DONT HAVE ANY LEVELS TO CHOOSE FROM, CREATE ONE?", 20) * 0.5, GetScreenHeight() * 0.5, 20, RAYWHITE);
            if(GuiButton({float(GetScreenWidth() * 0.5 - 25), float(GetScreenHeight() * 0.55), 50, 50}, "CREATE LEVEL")){
                system("mkdir ./levels/level1");
                lc++;
            }
        }

        // user has levels, when they choose it will load that selected level
        else if (cl == -1)
        {
            ClearBackground(BLACK);
            chooseLevel(cl, lc);
        }
        
        // in editing window
        else
        {
            ClearBackground(LIGHTGRAY);
            displayTiles(allTiles, currTile);
            drawWorld(world, TILES, ITEMS, mapArea.y);
            editWorld(allTiles, currTile, mapArea, cl);
            
            // set spawn point
            if(GuiButton({WINDOW_SIZE - 90, 32, 40, 30}, "spawn"))
                spawnPoint = !spawnPoint;

            // back button
            if(GuiButton({WINDOW_SIZE - 45,32, 40, 30}, "Back"))
            {
                saveWorld(world, cl);
                eraseWorld();
                cl = -1;
            }
        }
        EndDrawing();
    }
    deinit(world, cl);
    return 0;
}