#include <fstream>
#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <filesystem>
#include <string>
#include <vector>
#include <algorithm>

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

namespace fs = std::filesystem;

const float scale = 2.0f;

const int WINDOW_SIZE_WIDTH = 992;
const int WINDOW_SIZE_HEIGHT = 992;
const int WORLD_SIZE = 864;

Rectangle WORLD_BORDER = (Rectangle){64,64,WORLD_SIZE,WORLD_SIZE};
const int WORLD_BORDER_X = 64;
const int WORLD_BORDER_Y = 96;

const int TILE_AMOUNT = 2;
const int TILE_SIZE = 16;

const int TILES_PER_ROW = WINDOW_SIZE_WIDTH / (TILE_SIZE * scale);

const Vector2 ORIGIN = (Vector2){0,0};
const Vector2 UP = (Vector2){0, -TILE_SIZE};
const Vector2 DOWN = (Vector2){0, TILE_SIZE};
const Vector2 LEFT = (Vector2){-TILE_SIZE,0};
const Vector2 RIGHT = (Vector2){TILE_SIZE, 0};

// types of elements in the asset pack
enum Element{
    WALL = 0,
    ITEM = 1,
    SPRITE = 2,
    FLOOR = 3,
};

// Represents a single tile element in the world
struct Tile{
    
    Element tileType;
    std::string name;
    Rectangle scrPos;
    Vector2 src;
    bool walk;
    bool valid;
};

Tile worldTiles[27][27];
Tile worldItems[27][27];
Tile worldFloors[27][27];

// scale the rectangle to be proportionate to the screen
void scaleRec(Rectangle& rec, float scale){
    rec.x *= scale;
    rec.y *= scale;
    rec.width *= scale;
    rec.height *= scale;
}

// use the location of the src png to draw the image
void drawTile(Tile tile, Vector2 position, float scale, Texture2D texture){
    
    // create the src and dst rect
    Rectangle src = Rectangle{tile.src.x, tile.src.y, TILE_SIZE, TILE_SIZE};
    Rectangle dst = Rectangle{position.x, position.y, TILE_SIZE, TILE_SIZE};

    scaleRec(dst, scale);
    DrawTexturePro(texture, src, dst, (Vector2){0,0}, 0, WHITE);
}

// displays every tile in asset png
void displayTiles(std::vector<Tile>& dict, Vector2 position, float scale, Texture2D texture, Rectangle& editorArea){

    Vector2 currPos = Vector2{-TILE_SIZE, 0};
    
    for(int i = 0; i < (int)dict.size(); i++){

        // move to the next row when we have exahusted the current one
        if(int(i + scale) % TILES_PER_ROW == 0 && i != 0){
            currPos = Vector2Add(currPos, (Vector2){-currPos.x, currPos.y + TILE_SIZE});
            // std::cout << i << "new y: " << currPos.y << std::endl;
        }
        
        else
            currPos = Vector2Add(currPos, RIGHT);
        
        dict[i].scrPos = Rectangle{currPos.x, currPos.y, TILE_SIZE, TILE_SIZE};
        scaleRec(dict[i].scrPos, scale); currPos.x++;
        drawTile(dict[i], currPos, scale, texture);
    }
}

// reads every type of tile
void readTiles(std::vector<Tile>& dict){
    std::ifstream inFile;
    inFile.open("src/tiles.txt");

    if(!inFile){
        std::cout << "ERROR OPENING FILE \n";
        return;
    }
    
    float x, y;
    bool walk;
    std::string name;
    int element;

    while (inFile >> x >> y >> walk >> element >> name) {
        Tile tile;
        tile.src = Vector2{x, y};
        tile.walk = walk;
        tile.name = name;
        tile.tileType = (Element)element;
        dict.push_back(tile); 
    }

    inFile.close();
}

// drawing the world
void drawWorld(Texture2D& texture){
    for(int i = 0; i < 27; i++){
        for(int j = 0; j < 27; j++){
            if(worldFloors[i][j].valid == true){
                Rectangle floor_src = Rectangle{worldFloors[i][j].src.x, worldFloors[i][j].src.y, TILE_SIZE, TILE_SIZE};
                Rectangle floor_dst = Rectangle{worldFloors[i][j].scrPos.x, worldFloors[i][j].scrPos.y, TILE_SIZE * scale, TILE_SIZE * scale};
                DrawTexturePro(texture, floor_src, floor_dst, {0,0}, 0, WHITE);
            }
            if(worldItems[i][j].valid == true){
                Rectangle item_src = Rectangle{worldItems[i][j].src.x, worldItems[i][j].src.y, TILE_SIZE, TILE_SIZE};
                Rectangle item_dst = Rectangle{worldItems[i][j].scrPos.x, worldItems[i][j].scrPos.y, TILE_SIZE * scale, TILE_SIZE * scale};
                DrawTexturePro(texture, item_src,item_dst, {0,0}, 0, WHITE);
            }
            if(worldTiles[i][j].valid == true){
                Rectangle tile_src = Rectangle{worldTiles[i][j].src.x, worldTiles[i][j].src.y, TILE_SIZE, TILE_SIZE};
                Rectangle tile_dst = Rectangle{worldTiles[i][j].scrPos.x, worldTiles[i][j].scrPos.y, TILE_SIZE * scale, TILE_SIZE * scale};
                DrawTexturePro(texture, tile_src,tile_dst, {0,0}, 0, WHITE);
            }
        }
    }
}

// stopping the drawing of every tile in the level
void clearTiles(){
    for(int i = 0; i < 27; i++)
        for(int j = 0; j < 27; j++){
            worldFloors[i][j].valid = false;
            worldItems[i][j].valid = false;
            worldTiles[i][j].valid = false;
        }
}

// method to handle drawing tiles and updating the world map when the user does
void editMap(std::vector<Tile>& dict, Rectangle& editorArea, Tile& currTile){

    DrawRectangle(WORLD_BORDER_X, WORLD_BORDER_Y, WORLD_BORDER.width, WORLD_BORDER.height,LIGHTGRAY);

    // clear editing board
    if(IsKeyPressed(KEY_E)){
        clearTiles();
        std::cout << "CLEARED BOARD \n";
    }

    // eraser
    if(IsMouseButtonDown(MOUSE_RIGHT_BUTTON)){
        int mcpx = (((int)GetMousePosition().x >> (int)log2(32)) << (int)log2(32));
        int mcpy = (((int)GetMousePosition().y >> (int)log2(32)) << (int)log2(32));
        worldFloors[int((mcpx - WORLD_BORDER_X) / 32)][int((mcpy - WORLD_BORDER_Y) / 32)].valid = false;
        worldItems[int((mcpx - WORLD_BORDER_X) / 32)][int((mcpy - WORLD_BORDER_Y) / 32)].valid = false;
        worldTiles[int((mcpx - WORLD_BORDER_X) / 32)][int((mcpy - WORLD_BORDER_Y) / 32)].valid = false;
    }

    // say the name of the tile that you have selected
    if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)){
        Vector2 mcp = GetMousePosition();
        // need to update the world tile
        if(CheckCollisionPointRec(GetMousePosition(),WORLD_BORDER) && !currTile.name.empty()){
            // find the nearest point x,y thats divisible by 32
            int mcpx = (((int)GetMousePosition().x >> (int)log2(32)) << (int)log2(32));
            int mcpy = (((int)GetMousePosition().y >> (int)log2(32)) << (int)log2(32));
            // std::cout << "(" << mcpx << "," << mcpy << ")" << std::endl;
            currTile.valid = true; currTile.scrPos.x = mcpx; currTile.scrPos.y = mcpy;

            // update the map at that position
            if(currTile.tileType == FLOOR)
                worldFloors[int((mcpx - WORLD_BORDER_X) / 32)][int((mcpy - WORLD_BORDER_Y) / 32)] = currTile;
            else if(currTile.tileType == ITEM)
                worldItems[int((mcpx - WORLD_BORDER_X) / 32)][int((mcpy - WORLD_BORDER_Y) / 32)] = currTile;
            else
                worldTiles[int((mcpx - WORLD_BORDER_X) / 32)][int((mcpy - WORLD_BORDER_Y) / 32)] = currTile;
        }

        for(int i = 0; i < (int)dict.size(); i++)
            if(CheckCollisionPointRec(mcp, dict[i].scrPos)){
                std::cout << "SELECTED: " << dict[i].name << std::endl;
                currTile = dict[i];
            }
        }
}

void saveTiles(std::string levelPath){

    // write out every attribute of each tile, floor, and item, including i and j position 
    std::ofstream outFile;
    std::string fileName = "levels/" + levelPath + "/worldFloors.txt";
    outFile.open(fileName);

    if(!outFile){
        std::cerr << "ERROR SAVING WORLD FLOORS \n";
        return;
    }

    for(int i = 0; i < 27; i++){
        for(int j = 0; j < 27; j++){
            if(worldFloors[i][j].valid == true){
                outFile << i << " " << j << " " 
                << worldFloors[i][j].tileType << " " 
                << worldFloors[i][j].valid <<  " " 
                << worldFloors[i][j].name << " " 
                << worldFloors[i][j].scrPos.x  << " " 
                << worldFloors[i][j].scrPos.y << " " 
                << worldFloors[i][j].scrPos.width << " " 
                << worldFloors[i][j].scrPos.height << " " 
                << worldFloors[i][j].src.x << " " 
                << worldFloors[i][j].src.y << " " 
                << worldFloors[i][j].walk << std::endl; 
            }
        }
    }

    outFile.close();

    fileName = "levels/" + levelPath + "/worldTiles.txt";
    outFile.open(fileName);

    if(!outFile){
        std::cerr << "ERROR SAVING WORLD TILES \n";
        return;
    }

     for(int i = 0; i < 27; i++){
        for(int j = 0; j < 27; j++){
            if(worldTiles[i][j].valid){
                outFile << i << " " << j << " " 
                << worldTiles[i][j].tileType << " " 
                << worldTiles[i][j].valid <<  " " 
                << worldTiles[i][j].name << " " 
                << worldTiles[i][j].scrPos.x  << " " 
                << worldTiles[i][j].scrPos.y << " " 
                << worldTiles[i][j].scrPos.width << " " 
                << worldTiles[i][j].scrPos.height << " " 
                << worldTiles[i][j].src.x << " " 
                << worldTiles[i][j].src.y << " " 
                << worldTiles[i][j].walk << std::endl; 
            }
        }
    }

    outFile.close();

    fileName = "levels/" + levelPath + "/worldItems.txt";
    outFile.open(fileName);
    
    if(!outFile){
        std::cerr << "ERROR SAVING WORLD ITEMS \n";
        return;
    }

    for(int i = 0; i < 27; i++){
        for(int j = 0; j < 27; j++){
            if(worldItems[i][j].valid){
                outFile << i << " " << j << " " 
                << worldItems[i][j].tileType << " " 
                << worldItems[i][j].valid <<  " " 
                << worldItems[i][j].name << " " 
                << worldItems[i][j].scrPos.x  << " " 
                << worldItems[i][j].scrPos.y << " " 
                << worldItems[i][j].scrPos.width << " " 
                << worldItems[i][j].scrPos.height << " " 
                << worldItems[i][j].src.x << " " 
                << worldItems[i][j].src.y << " " 
                << worldItems[i][j].walk << std::endl; 
            }
        }
    }

    outFile.close();
}

void loadTiles(std::string levelPath){
    
    std::string name;
    Rectangle scrPos;
    Vector2 src;
    bool walk, valid;
    int x,y, tileType;
    
    std::ifstream inFile;
    std::string fileName = "levels/" + levelPath + "/worldTiles.txt";

    inFile.open(fileName);

    if(!inFile)
        std::cerr << "NO TILES SAVED \n";

    else{
        while(inFile >> x >> y >> tileType >> valid >> name >> scrPos.x >> scrPos.y >> scrPos.width >> scrPos.height >> src.x >> src.y >> walk){
            worldTiles[x][y].tileType = (Element)tileType;
            worldTiles[x][y].valid = valid;
            worldTiles[x][y].name = name;
            worldTiles[x][y].scrPos = scrPos;
            worldTiles[x][y].src = src;
        }
    }

    inFile.close();
    
    fileName = "levels/" + levelPath + "/worldFloors.txt";

    inFile.open(fileName);

    if(!inFile)
        std::cerr << "NO FLOORS SAVED \n";
    
    else{
        while(inFile >> x >> y >> tileType >> valid >> name >> scrPos.x >> scrPos.y >> scrPos.width >> scrPos.height >> src.x >> src.y >> walk){
            worldFloors[x][y].tileType = (Element)tileType;
            worldFloors[x][y].valid = valid;
            worldFloors[x][y].name = name;
            worldFloors[x][y].scrPos = scrPos;
            worldFloors[x][y].src = src;
        }
    }

    inFile.close();
    fileName = "levels/" + levelPath + "/worldItems.txt";
    inFile.open(fileName);

    if(!inFile)
        std::cerr << "NO ITEMS SAVED \n";

    else{
        while(inFile >> x >> y >> tileType >> valid >> name >> scrPos.x >> scrPos.y >> scrPos.width >> scrPos.height >> src.x >> src.y >> walk){
            worldItems[x][y].tileType = (Element)tileType;
            worldItems[x][y].valid = valid;
            worldItems[x][y].name = name;
            worldItems[x][y].scrPos = scrPos;
            worldItems[x][y].src = src;
        }   
    }

    inFile.close();
}

// loads the name of every level a user has saved
void loadLevels(std::vector<std::string>& levels){
    // look in the levels directory
    for(const auto entry : fs::directory_iterator("./levels")){
        if(entry.is_directory()){
            std::string name = entry.path().string();
            if(name.find("level"))
                levels.push_back(entry.path().filename());
        }
    }

    std::sort(levels.begin(), levels.end());
}

void chooseLevel(std::vector<std::string>& levels, bool& choseLevel, int& currLevel){
    int offset = 50;
    for(int i = 0; i < (int)levels.size(); i++){
        if(GuiButton({float(WORLD_BORDER_X + offset), 96, 45, 30}, levels[i].c_str())){
            loadTiles(levels[i]);
            // need to load that levels data
            choseLevel = !choseLevel;
            currLevel = i;
        }
        offset += 50;
    }

    if(GuiButton({float(GetScreenWidth() * 0.5 - 25), float(GetScreenHeight() * 0.55), 50, 50}, "CREATE LEVEL")){
        system(("mkdir ./levels/level" + std::to_string(levels.size() +1)).c_str());
        levels.push_back("level" + std::to_string(levels.size() + 1));
    }

}

// before editing, display every type  level that a user has made

int main() {

    // refactor code, make modular, only change someone should make using another asset png is altering the pngs src position

    // init
    SetTraceLogLevel(LOG_ERROR);
    SetTargetFPS(120);
    InitWindow(WINDOW_SIZE_WIDTH,WINDOW_SIZE_HEIGHT, "Dungeon Fighters!");
    std::vector<Tile> dict;
    std::vector<std::string> levels;
    int currLevel = -1;
    loadLevels(levels);
    readTiles(dict);
    
    Rectangle editorArea = {0,0, WINDOW_SIZE_WIDTH, 0 * scale};
    Image image = LoadImage("assets/character and tileset/Dungeon_Tileset.png");
    Texture2D texture = LoadTextureFromImage(image);
    Tile currTile = {};
    currTile.valid = true;
    bool choseLevel = false;
    
    // get the ceil of number of tile / # of assets per row in the screen
    for(int i = 0; i < std::ceil(float(dict.size()) / TILES_PER_ROW); i++){
        editorArea.height += TILE_SIZE * scale;
        WORLD_BORDER.y += TILE_SIZE;
    }

    // game loop
    while(!WindowShouldClose()){

        BeginDrawing();
        ClearBackground(BLACK);

        // case one, user has no levels saved, prompt them to create one
        if(levels.empty()){
            DrawText("YOU DONT HAVE ANY LEVELS TO CHOOSE FROM, CREATE ONE?", GetScreenWidth() * 0.5 - MeasureText("YOU DONT HAVE ANY LEVELS TO CHOOSE FROM, CREATE ONE?", 20) * 0.5, GetScreenHeight() * 0.5, 20, RAYWHITE);
            if(GuiButton({float(GetScreenWidth() * 0.5 - 25), float(GetScreenHeight() * 0.55), 50, 50}, "CREATE LEVEL")){
                system("mkdir ./levels/level1");
                levels.push_back("level1");
                currLevel = 1;
            }
        }

        // user has levels, when they choose it will load that selected level
        else if (!choseLevel) 
           chooseLevel(levels, choseLevel, currLevel);
        
        // load the world and editor HUD
        else{
            if(GuiButton({800,48, 25, 30}, "Back")){
                saveTiles(levels[currLevel]);
                clearTiles();
                choseLevel = !choseLevel;
                currLevel = -1;
            }
            DrawRectangle(editorArea.x, editorArea.y, editorArea.width, editorArea.height, BROWN);
            DrawFPS(800, 32);
            displayTiles(dict, ORIGIN, scale, texture, editorArea);
            editMap(dict, editorArea, currTile);
            drawWorld(texture);
        }

        EndDrawing();
    }
    
    // prevent leak
    if(!levels.empty() && currLevel != -1)
        saveTiles(levels[currLevel]);
    UnloadImage(image);
    UnloadTexture(texture);
    CloseWindow();

    return 0;
}
