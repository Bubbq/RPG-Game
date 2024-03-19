// TO HANDLE TILE MAPPING FUNCTIONALITIES

#include <raylib.h>

// start/end of game life cycle
void initGraphics();
void unloadGraphics();

// size of one tile in the png
const int TILE_SIZE = 16;

// the position of the next tile down of room
const Vector2 DOWN = (Vector2){0,TILE_SIZE};

// position of tile on right side
const Vector2 RIGHT = (Vector2){TILE_SIZE * 5, 0};

struct Graphics{
    
    // png path of the tile image we're interested in
    Image tile_image;

    // using GPU
    Texture2D tile_texture;
    
    // representing the entire png
    Rectangle tile_screen = Rectangle{0,0,160, 160};
};

Graphics graphics;

// the objects of the asset back, ie) walls, doors, floors, etc.
// using bit flags to fall through eaiser, ie) room | wall would draw a room and a wall
enum Elements{

    // entire room, all walls closed
    ROOM = 1 << 0,

    // opening to the right
    DOOR_LEFT_OPEN = 1 << 1,
    
    // opening to the left
    DOOR_RIGHT_OPEN = 1 << 2,
    
    // simple wall
    WALL_H_1 = 1 << 3,
    
    // wall with corner (top right)
    WALL_H_2 = 1 << 4,
    
    // wall with corner (top left)
    WALL_H_3 = 1 << 5,
    
    // floors for left and right openings respectivley
    ROOM_FLOOR = 1 << 6,
};

// enlarging the src rectangle to fit the screen
void scaleRec(Rectangle&, float);

// draws specific tile elements at a position 
void renderGraphics(int, Vector2, float scale);

// renders a room with open doors left and right
void draw_room_lrd(Vector2 pos, float scale);

