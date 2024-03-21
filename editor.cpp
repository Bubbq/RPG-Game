#include <fstream>
#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <vector>

const float scale = 2.0f;

const int WINDOW_SIZE_WIDTH = 992;
const int WINDOW_SIZE_HEIGHT = 992;
const int WORLD_SIZE = 848;

Rectangle WORLD_BORDER = (Rectangle){float(WINDOW_SIZE_WIDTH - WORLD_SIZE) / 2,float(WINDOW_SIZE_HEIGHT - WORLD_SIZE) / 2,WORLD_SIZE,WORLD_SIZE};

const int TILE_AMOUNT = 2;
const int TILE_SIZE = 16;

const int TILES_PER_ROW = WINDOW_SIZE_WIDTH / (TILE_SIZE * scale);

// changes based on the height of the editor

const Vector2 ORIGIN = (Vector2){0,0};
const Vector2 UP = (Vector2){0, -TILE_SIZE};
const Vector2 DOWN = (Vector2){0, TILE_SIZE};
const Vector2 LEFT = (Vector2){-TILE_SIZE,0};
const Vector2 RIGHT = (Vector2){TILE_SIZE, 0};

// Represents a single tile element in the world
struct Tile{
    std::string name;
    Rectangle scrPos;
    Vector2 src;
    bool walk;
};

Tile worldMap[WORLD_SIZE / TILE_SIZE][WORLD_SIZE / TILE_SIZE];

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

    while (inFile >> x >> y >> walk >> name) {
        Tile tile;
        tile.src = Vector2{x, y};
        tile.walk = walk;
        tile.name = name;
        dict.push_back(tile); 
    }

    inFile.close();
}

// drawing the world
void drawWorld(Texture2D& texture){
    for(int i = 0; WORLD_BORDER.width / TILE_SIZE; i++){
        for(int j = 0; j < WORLD_BORDER.width / TILE_SIZE; j++){
            if(sizeof(worldMap[i][j]) != 0)
                drawTile(worldMap[i][j], {worldMap[i][j].scrPos.x,worldMap[i][j].scrPos.y}, scale, texture);
        }
    }
}

// method to handle drawing tiles and updating the world map when the user does
void editMap(std::vector<Tile>& dict, Rectangle& editorArea, Tile& currTile){

    DrawRectangle(WORLD_BORDER.x, WORLD_BORDER.y, WORLD_BORDER.width, WORLD_BORDER.height,GREEN);

    // deselect current tile
    if(IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)){
        currTile = {};
        std::cout << "DESELECTED TILE \n";
    }

    // say the name of the tile that you have selected
    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
        Vector2 mcp = GetMousePosition();
            
        // need to update the wolrd tile
        if(CheckCollisionPointRec(mcp,WORLD_BORDER) && !currTile.name.empty()){
            
            // find the nearest point x,y thats divisible by 16
            int mcpx = (((int)mcp.x >> (int)log2(16)) << (int)log2(16));
            int mcpy = (((int)mcp.y >> (int)log2(16)) << (int)log2(16));
            std::cout << "(" << mcpx << "," << mcpy << ")" << std::endl;

            // update the map at that position
            worldMap[mcpx / TILE_SIZE][mcpy / TILE_SIZE] = currTile;
        }

        for(int i = 0; i < (int)dict.size(); i++)
            if(CheckCollisionPointRec(mcp, dict[i].scrPos)){
                std::cout << "SELECTED: " << dict[i].name << std::endl;
                currTile = dict[i];
            }
        }
}

int main() {

    // when user presses on the map, then update the 'world' structure and draw
        // find the nearst x and y value thats divisble by 16
        // set the world array at that [i][j] to be the tile
        // have some function to draw the world
        
    // save the world upon exit
    // load the world if availible

    // ERRORS: when 62+ assets, skips by 32 rather than 16

    // init
    SetTraceLogLevel(LOG_ERROR);
    SetTargetFPS(60);
    InitWindow(WINDOW_SIZE_WIDTH,WINDOW_SIZE_HEIGHT, "Dungeon Fighters!");
    
    Image image = LoadImage("assets/character and tileset/Dungeon_Tileset.png");
    Texture2D texture = LoadTextureFromImage(image);
    std::vector<Tile> dict;
    Tile currTile = {};
    readTiles(dict);
    Rectangle editorArea = {0,0, WINDOW_SIZE_WIDTH, 0 * scale};
    
    // get the ceil of number of tile / # of assets per row in the screen
    for(int i = 0; i < std::ceil(float(dict.size()) / TILES_PER_ROW); i++){
        editorArea.height += TILE_SIZE * scale;
        WORLD_BORDER.y += TILE_SIZE;
    }

    // game loop
    while(!WindowShouldClose()){

        BeginDrawing();
        ClearBackground(WHITE);
        DrawRectangle(editorArea.x, editorArea.y, editorArea.width, editorArea.height, BROWN);
        displayTiles(dict, ORIGIN, scale, texture, editorArea);
        editMap(dict, editorArea, currTile);
        // drawWorld(texture);
        EndDrawing();
    }

    // prevent leak
    UnloadImage(image);
    UnloadTexture(texture);
    CloseWindow();

    return 0;
}
