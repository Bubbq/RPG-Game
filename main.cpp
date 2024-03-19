#include <raylib.h>
#include "Graphics.cpp"

int main(){

    // init
    Rectangle screen = (Rectangle){0,0, 512, 512};
    float scale = 2.0f;
    SetTraceLogLevel(LOG_ERROR);
    SetTargetFPS(60);
    InitWindow(screen.width, screen.height, "Dungeon Fighters!");
    initGraphics();

    // game loop
    while(!WindowShouldClose()){

        BeginDrawing();

        ClearBackground(WHITE);

        // renders a room with open doors left and right
        draw_room_lrd(Vector2{screen.x, screen.y}, scale);

        EndDrawing();
    }

    // prevent leak
    unloadGraphics();
    CloseWindow();
    return 0;
}