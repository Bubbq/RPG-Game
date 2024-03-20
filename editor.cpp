#include <fstream>
#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include <vector>

const int TILE_AMOUNT = 2;
const int TILE_SIZE = 16;
const int WINDOW_SIZE = 512;

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
void displayTiles(std::vector<Tile>& dict, Vector2 position, float scale, Texture2D texture){

    Vector2 currPos = Vector2{-TILE_SIZE, 0};

    for(int i = 0; i < (int)dict.size(); i++){

        // move to the next row when we have exahusted the current one
        if(i % (WINDOW_SIZE / TILE_SIZE) == 0 && i != 0)
            currPos = Vector2Add(currPos, (Vector2){-currPos.x, currPos.y + TILE_SIZE});
        else
            currPos = Vector2Add(currPos, RIGHT);

        // init the tiles position on the screen for collision checking
        dict[i].scrPos = Rectangle{currPos.x, currPos.y, TILE_SIZE, TILE_SIZE};
        scaleRec(dict[i].scrPos, scale);
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

int main() {

    // write down the src rect of every tile in the game
    // TODO: handle when user chooses a tile from the editor
    // when user presses on the map, then update the 'world' structure and draw
    // save the world upon exit

    // init
    SetTraceLogLevel(LOG_ERROR);
    SetTargetFPS(60);
    InitWindow(WINDOW_SIZE,WINDOW_SIZE, "Dungeon Fighters!");
    
    Image image = LoadImage("assets/character and tileset/Dungeon_Tileset.png");
    Texture2D texture = LoadTextureFromImage(image);
    std::vector<Tile> dict;
    readTiles(dict);
    float scale = 2.0f;
   
    // game loop
    while(!WindowShouldClose()){

        BeginDrawing();
        ClearBackground(WHITE);
        displayTiles(dict, ORIGIN, scale, texture);

        // say the name of the tile that you have selected
        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
            Vector2 mcp = GetMousePosition();
            for(int i = 0; i < (int)dict.size(); i++)
                if(CheckCollisionPointRec(mcp, dict[i].scrPos))
                    std::cout << "SELECTED: " << dict[i].name << std::endl;
        }

        EndDrawing();
    }

    // prevent leak
    UnloadImage(image);
    UnloadTexture(texture);
    CloseWindow();

    return 0;
}
