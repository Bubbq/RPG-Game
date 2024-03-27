#include <fstream>
#include <iostream>
#include <raylib.h>
#include <filesystem>
#include <raymath.h>
#include <string>
#include <vector>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

namespace fs = std::filesystem;

const int WINDOW_SIZE = 992;
const float SCALE = 2.0f;
const int TILE_SIZE = 16;
const int TILES_PER_ROW = WINDOW_SIZE / (TILE_SIZE * SCALE);
const int WORLD_SIZE = 32;
const std::string ASSET_PATH = "assets/character and tileset/Dungeon_Tileset.png";

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
    std::string name;
    Element tileType;
    bool walkable;
};

// representing the current world a user is creating
struct World{
    Tile floors[WORLD_SIZE][WORLD_SIZE];
    Tile walls[WORLD_SIZE][WORLD_SIZE];
    Tile items[WORLD_SIZE][WORLD_SIZE];
};

World world;

Rectangle scaleRect(Rectangle rect, float scale){
    rect.x *= scale;
    rect.y *= scale;
    rect.width *= scale;
    rect.height *= scale;

    return rect;
}

// draws a single tile scaled to fit the scree
void drawTile(Tile tile, Vector2 pos, Texture2D texture){

    // create the src rect to extract from and dst rect to draw in screen
    Rectangle src = {tile.src.x, tile.src.y, TILE_SIZE, TILE_SIZE};
    Rectangle dst = {pos.x, pos.y, TILE_SIZE, TILE_SIZE};

    // scale the dst
    dst = scaleRect(dst, SCALE);
    DrawTexturePro(texture, src, dst, {0,0}, 0, WHITE);
}

// displays every tile availible in asset png
void displayTiles(std::vector<Tile>& allTiles, Texture2D texture, Tile& currTile){

    // where we're drawing the ith tile
    Vector2 currPos = {-TILE_SIZE, 0};

    for(int i = 0; i < (int)allTiles.size(); i++){
        
        // moving to next row once we've exhausted the current one
        if(i % TILES_PER_ROW == 0 && i !=0)
            currPos = Vector2Add(currPos, {-currPos.x, TILE_SIZE});

        else
            currPos.x += TILE_SIZE;

        // update the current tile upon selection
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            if(CheckCollisionPointRec(GetMousePosition(), scaleRect({currPos.x, currPos.y, TILE_SIZE, TILE_SIZE}, SCALE)))
                currTile = allTiles[i];
            
        drawTile(allTiles[i], currPos, texture);
    }
}

// to reset the current worlds tiles
void clearTiles(){
    for(int i = 0; i < 32; i++){
        for(int j = 0; j < 32; j++){
            world.floors[i][j] = {};
            world.walls[i][j] = {};
            world.items[i][j] = {};
        }
    }
}

// write world info to file
void saveWorld(Tile elements[WORLD_SIZE][WORLD_SIZE], std::string filePath){

    std::ofstream outFile;
    outFile.open(filePath);
    if(!outFile){
        std::cerr << "ERROR TRYING TO SAVE " + filePath << std::endl;
        return;
    }

    for(int i = 0; i < 32; i++){
        for(int j = 0; j < 32; j++){
            if(!elements[i][j].name.empty()){
                outFile << i << " " << j << " "
                << elements[i][j].src.x << " "
                << elements[i][j].src.y << " "
                << elements[i][j].name << " "
                << elements[i][j].tileType << " "
                << elements[i][j].walkable << std::endl;
            }
        }
    }
    outFile.close();
}

// load world content from file
void loadWorld(Tile elements[WORLD_SIZE][WORLD_SIZE], std::string filePath){

    std::ifstream inFile;
    inFile.open(filePath);
    if(!inFile){
        std::cerr << "NOTHING TO SAVE \n";
        return;
    }

    int x,y;
    Vector2 src;
    std::string name;
    int tileType;
    bool walkable;

    while(inFile >> x >> y >> src.x >> src.y >> name >> tileType >> walkable)
        elements[x][y] = {src, name, (Element)tileType, walkable};

    inFile.close();
}

// drawing the world
void drawWorld(Texture2D& texture, Rectangle& mapArea){
    for(int i = 0; i < WORLD_SIZE; i++){
        for(int j = 0; j < WORLD_SIZE; j++){
            Rectangle dst = Rectangle{float(i * 32), float(j * 32) + mapArea.y , TILE_SIZE * SCALE, TILE_SIZE * SCALE};
            if(!world.floors[i][j].name.empty()){
                Rectangle floor_src = Rectangle{world.floors[i][j].src.x, world.floors[i][j].src.y, TILE_SIZE, TILE_SIZE};
                DrawTexturePro(texture, floor_src, dst, {0,0}, 0, WHITE);
            }
            if(!world.items[i][j].name.empty()){
                Rectangle floor_src = Rectangle{world.items[i][j].src.x, world.items[i][j].src.y, TILE_SIZE, TILE_SIZE};
                DrawTexturePro(texture, floor_src, dst, {0,0}, 0, WHITE);
            }
            if(!world.walls[i][j].name.empty()){
                Rectangle floor_src = Rectangle{world.walls[i][j].src.x, world.walls[i][j].src.y, TILE_SIZE, TILE_SIZE};
                DrawTexturePro(texture, floor_src, dst, {0,0}, 0, WHITE);
            }
        }
    }
}

// to handle the drawing of tiles and updating the user's map
void editWorld(std::vector<Tile>& allTiles, Tile& currTile, Rectangle& mapArea, Texture2D& texture, int& cl){
    
    // show the user which tile are we drawing with
    if(!currTile.name.empty())
        DrawTexturePro(texture, {currTile.src.x, currTile.src.y, TILE_SIZE, TILE_SIZE}, {GetMousePosition().x - 20, GetMousePosition().y - 20, 20, 20}, {0,0}, 0, WHITE);
       
    // empty all tiles
    if(IsKeyPressed(KEY_E))
        clearTiles();

    // set all tiles to void
    if(IsKeyPressed(KEY_F))
        for(int i = 0; i < WORLD_SIZE; i++)
            for(int j = 0; j < WORLD_SIZE; j++) 
                world.floors[i][j] = {{128, 112}, "void", (Element)3, 1};

    // get the nearest rectangle from the users mouse
    int mpx = (((int)GetMousePosition().x >> (int)log2(TILE_SIZE * SCALE)) << (int)log2(TILE_SIZE * SCALE));
    int mpy = (((int)GetMousePosition().y >> (int)log2(TILE_SIZE * SCALE)) << (int)log2(TILE_SIZE * SCALE));

    if(CheckCollisionPointRec(GetMousePosition(), mapArea)){

        // erasing tiles
        if(IsMouseButtonDown(MOUSE_BUTTON_RIGHT)){
            world.floors[mpx / WORLD_SIZE][int(mpy - mapArea.y) / WORLD_SIZE] = {};
            world.walls[mpx / WORLD_SIZE][int(mpy - mapArea.y) / WORLD_SIZE] = {};
            world.items[mpx / WORLD_SIZE][int(mpy - mapArea.y) / WORLD_SIZE] = {};
        }

        // updating map with selected tile
        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
            if(currTile.tileType == FLOOR)
                world.floors[mpx / WORLD_SIZE][int(mpy - mapArea.y) / WORLD_SIZE] = currTile;
            else if (currTile.tileType == WALL)
                world.walls[mpx / WORLD_SIZE][int(mpy - mapArea.y) / WORLD_SIZE] = currTile;
            else if(currTile.tileType == ITEM)
                world.items[mpx / WORLD_SIZE][int(mpy - mapArea.y) / WORLD_SIZE] = currTile;
        }
    }

    // back button
    if(GuiButton({WINDOW_SIZE - 45,32, 40, 30}, "Back")){
        saveWorld(world.walls, "./levels/level" + std::to_string(cl) + "/walls.txt");
        saveWorld(world.floors, "./levels/level" + std::to_string(cl) + "/floors.txt");
        saveWorld(world.items, "./levels/level" + std::to_string(cl) + "/items.txt");
        clearTiles();
        cl = -1;
    }
}

// finds the amount of levels a user has created to display
void loadLevels(int& lc){
    for(const auto entry : fs::directory_iterator("./levels")){
        if(entry.is_directory())
            if(entry.path().string().find("level"))
                lc++;
    }
}

// reads and stores the information of every tile in a vector
void readTiles(std::vector<Tile>& allTiles){
    std::ifstream inFile;
    inFile.open("src/tiles.txt");

    if(!inFile){
        std::cerr << "ERROR READING TILES \n" << std::endl;
        return;
    }

    Vector2 src;
    std::string name;
    int tileType;
    bool walkable;

    while(inFile >> src.x >> src.y >> walkable >> tileType >> name)
        allTiles.push_back((Tile){src, name, (Element)tileType, walkable});
    
    inFile.close();
}

// menu for user to choose from his or her levels
void chooseLevel(int&cl, int& lc){
    int offset = 50;
    for(int i = 1; i < lc + 1; i++){
        if(GuiButton({float(64 + offset), 96, 45, 30}, ("level " + std::to_string(i)).c_str())){
            loadWorld(world.walls, "./levels/level" + std::to_string(i) + "/walls.txt");
            loadWorld(world.floors, "./levels/level" + std::to_string(i) + "/floors.txt");
            loadWorld(world.items, "./levels/level" + std::to_string(i) + "/items.txt");
            // need to load that levels data
            cl = i;
        }
        offset += 50;
    }

    if(GuiButton({float(GetScreenWidth() * 0.5 - 25), float(GetScreenHeight() * 0.55), 50, 50}, "CREATE LEVEL")){
        system(("mkdir ./levels/level" + std::to_string(lc + 1)).c_str());
        lc++;
    }

}

void init(std::vector<Tile>& allTiles, Rectangle& editingArea, Rectangle& mapArea, Image& image, Texture2D& texture, int& lc, int& cl){
    SetTraceLogLevel(LOG_ERROR);
    SetTargetFPS(120);
    InitWindow(WINDOW_SIZE,WINDOW_SIZE, "Dungeon Fighters!");
    loadLevels(lc);
    readTiles(allTiles);

    image = LoadImage(ASSET_PATH.c_str());
    texture = LoadTextureFromImage(image);

    // increase the height of editing area based on the number of tiles in teh src png
    for(int i = 0; i < std::ceil(float(allTiles.size())) / TILES_PER_ROW; i++){
        editingArea.height += TILE_SIZE * SCALE;
        mapArea.y += TILE_SIZE * SCALE;
    }
        system("mkdir ./levels");
        loadWorld(world.walls, "./levels/level" + std::to_string(cl) + "/walls.txt");
        loadWorld(world.floors, "./levels/level" + std::to_string(cl) + "/floors.txt");
        loadWorld(world.items, "./levels/level" + std::to_string(cl) + "/items.txt");
        
}

// prevent leak
void deinit(Texture2D& texture, Image& image, int& cl){
    if(cl != -1){
        saveWorld(world.walls, "./levels/level" + std::to_string(cl) + "/walls.txt");
        saveWorld(world.floors, "./levels/level" + std::to_string(cl) + "/floors.txt");
        saveWorld(world.items, "./levels/level" + std::to_string(cl) + "/items.txt");
    }
    UnloadImage(image);
    UnloadTexture(texture);
    CloseWindow();
}

int main(){

    // contains the info of every type of tile the asset has
    std::vector<Tile> allTiles;
    // amount of levels a user has created
    int lc = 0;
    // current level a user is on
    int cl = -1;
    // current tile that a user is  adding to the map
    Tile ct = {};
    // space reserved for selecting tiles
    Rectangle editingArea = {0,0,WINDOW_SIZE, 0};
    // space for map
    Rectangle mapArea = {0,0,WINDOW_SIZE, WINDOW_SIZE};
    Image image;
    Texture2D texture;
    
    init(allTiles, editingArea, mapArea, image, texture, lc, cl);

    while(!WindowShouldClose()){

        BeginDrawing();
        ClearBackground(LIGHTGRAY);

        // case one, user has no levels saved, prompt them to create one
        if(lc == 0){
            ClearBackground(BLACK);
            DrawText("YOU DONT HAVE ANY LEVELS TO CHOOSE FROM, CREATE ONE?", GetScreenWidth() * 0.5 - MeasureText("YOU DONT HAVE ANY LEVELS TO CHOOSE FROM, CREATE ONE?", 20) * 0.5, GetScreenHeight() * 0.5, 20, RAYWHITE);
            if(GuiButton({float(GetScreenWidth() * 0.5 - 25), float(GetScreenHeight() * 0.55), 50, 50}, "CREATE LEVEL")){
                system("mkdir ./levels/level1");
                lc++;
            }
        }

        // user has levels, when they choose it will load that selected level
        else if (cl == -1){
            ClearBackground(BLACK);
            chooseLevel(cl, lc);
        }
        
        else{
            DrawRectangle(editingArea.x, editingArea.y, editingArea.width, editingArea.height, LIGHTGRAY);
            displayTiles(allTiles, texture, ct);
            drawWorld(texture, mapArea);
            editWorld(allTiles, ct, mapArea, texture, cl);
        }
        EndDrawing();
    }

    deinit(texture, image, cl);
    return 0;
}