#include <raylib.h>
#include <string>
#include <vector>

const int ANIMATION_SPEED = 20;

const std::string SKELETON_PATH = "assets/Character_animation/monsters_idle/skeleton1/v2/skeleton_v2_";
const std::string VAMPIRE_PATH = "assets/Character_animation/monsters_idle/vampire/v2/vampire_v2_";
const std::string PREIST_PATH = "assets/Character_animation/priests_idle/priest1/v1/priest1_v1_";
const std::string SKULL_PATH = "assets/Character_animation/monsters_idle/skull/v2/skull_v2_";

Texture2D skeletonTexture[4];
Texture2D vampireTexture[4];
Texture2D preistTexture[4];
Texture2D skullTexture[4];

struct Sprite{
    std::string name;
    Vector2 pos;
    int fc;
};

std::vector<Sprite> worldSprites;

void unloadTextures(){
    for(int i = 0; i < 4; i++){
        UnloadTexture(skeletonTexture[i]);
        UnloadTexture(vampireTexture[i]);
        UnloadTexture(preistTexture[i]);
        UnloadTexture(skullTexture[i]);
    }
}

// load the textures for each type of mob
void initAnimation(){
    Image si;
    Image vi;
    Image pi;
    Image ski;

    for(int i = 0; i < 4; i++){
        si = LoadImage((SKELETON_PATH + std::to_string(i + 1) + ".png").c_str());
        vi = LoadImage((VAMPIRE_PATH + std::to_string(i + 1) + ".png").c_str());
        pi = LoadImage((PREIST_PATH + std::to_string(i + 1) + ".png").c_str());
        ski = LoadImage((SKULL_PATH + std::to_string(i + 1) + ".png").c_str());

        skeletonTexture[i] = LoadTextureFromImage(si);
        vampireTexture[i] = LoadTextureFromImage(vi);
        preistTexture[i] = LoadTextureFromImage(pi);
        skullTexture[i] = LoadTextureFromImage(ski);

        UnloadImage(si); UnloadImage(vi); UnloadImage(pi); UnloadImage(ski);
    }
}

// to animate a sprite
void animate(){
    // inc the frame count fo every sprite, then based on its name, find the appropriate textture to draw
    for(int i = 0; i < (int)worldSprites.size(); i++){
        worldSprites[i].fc++;
        int fp = worldSprites[i].fc / ANIMATION_SPEED;
        if(fp > 3){
            fp = 0;
            worldSprites[i].fc = 0;
        }
        // based on the sprites name, draw the approprate frame
        if(worldSprites[i].name == "skeleton")
            DrawTexturePro(skeletonTexture[fp], {0,0,16,16}, {worldSprites[i].pos.x, worldSprites[i].pos.x, 16 * 2, 16 * 2}, {0,0}, 0, WHITE);
        else if(worldSprites[i].name == "vampire")
            DrawTexturePro(vampireTexture[fp], {0,0,16,16}, {worldSprites[i].pos.x, worldSprites[i].pos.x, 16 * 2, 16 * 2}, {0,0}, 0, WHITE);
        else if(worldSprites[i].name == "priest")
            DrawTexturePro(preistTexture[fp], {0,0,16,16}, {worldSprites[i].pos.x, worldSprites[i].pos.x, 16 * 2, 16 * 2}, {0,0}, 0, WHITE);
        else 
            DrawTexturePro(skullTexture[fp], {0,0,16,16}, {worldSprites[i].pos.x, worldSprites[i].pos.x, 16 * 2, 16 * 2}, {0,0}, 0, WHITE);
    } 
}

int main(){
    SetTraceLogLevel(LOG_ERROR);
    InitWindow(500, 500, "animation");
    SetTargetFPS(60);
    initAnimation();
    int offset = 50;
      
    for(int i = 0; i < 5; i++){
        worldSprites.push_back({"priest", {float(offset + GetScreenWidth() * 0.5), float(GetScreenHeight() * 0.5)}, 0});
        offset += 50;
    }
   
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        animate();
        EndDrawing();
    }

    unloadTextures();
    CloseWindow();
}