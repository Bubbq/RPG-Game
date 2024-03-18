#include <raylib.h>
#include "constants.h"

int main(){

    // init
    SetTraceLogLevel(LOG_ERROR);
    SetTargetFPS(60);
    InitWindow(WINDOW.width, WINDOW.height, "Dungeon Fighters!");

    // game loop
    while(!WindowShouldClose()){
        BeginDrawing();
        EndDrawing();
    }

    // prevent leak
    CloseWindow();
    return 0;
}