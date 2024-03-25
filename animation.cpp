#include <cmath>
#include <raylib.h>
#include <string>
#include <vector>

const int ANIMATION_SPEED = 20;

const std::string SKELETON_PATH = "assets/Character_animation/monsters_idle/skeleton1/v2/skeleton_v2_";
const std::string VAMPIRE_PATH = "assets/Character_animation/monsters_idle/vampire/v2/vampire_v2_";
const std::string PREIST_PATH = "assets/Character_animation/priests_idle/priest1/v1/priest1_v1_";
const std::string SKULL_PATH = "assets/Character_animation/monsters_idle/skull/v2/skull_v2_";
const std::string ARROW_PATH = "assets/arrow_4.png";
const std::string BOW_PATH = "assets/bow.png";

Texture2D skeletonTexture[4];
Texture2D vampireTexture[4];
Texture2D preistTexture[4];
Texture2D skullTexture[4];

struct Sprite{
    std::string name;
    Vector2 pos;
    int fc;
    float sx;
    float sy;
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

// to animate world sprites
void animate(Texture2D ARROW){
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
            DrawTexturePro(skeletonTexture[fp], {0,0,16,16}, {worldSprites[i].pos.x, worldSprites[i].pos.y, 16 * 2, 16 * 2}, {0,0}, 0, WHITE);
        else if(worldSprites[i].name == "vampire")
            DrawTexturePro(vampireTexture[fp], {0,0,16,16}, {worldSprites[i].pos.x, worldSprites[i].pos.y, 16 * 2, 16 * 2}, {0,0}, 0, WHITE);
        else if(worldSprites[i].name == "priest")
            DrawTexturePro(preistTexture[fp], {0,0,16,16}, {worldSprites[i].pos.x, worldSprites[i].pos.y, 16 * 2, 16 * 2}, {0,0}, 0, WHITE);
        else if(worldSprites[i].name == "skull") 
            DrawTexturePro(skullTexture[fp], {0,0,16,16}, {worldSprites[i].pos.x, worldSprites[i].pos.y, 16 * 2, 16 * 2}, {0,0}, 0, WHITE);
        else if(worldSprites[i].name == "arrow")
            DrawTexturePro(ARROW, {0,8,16,16}, {worldSprites[i].pos.x, worldSprites[i].pos.y, 32, 32}, {0,0}, 0, WHITE);

        // update sprites movement  
        if(worldSprites[i].sx != 0 && worldSprites[i].sy != 0){
            worldSprites[i].pos.x +=  7 * worldSprites[i].sx;
            worldSprites[i].pos.y += 7 * worldSprites[i].sy;
        }
    } 
}

void drawPlayer(Sprite& player, Texture2D BOW, Texture2D ARROW){
    if(IsKeyDown(KEY_W))
        player.pos.y -= 5;
    if(IsKeyDown(KEY_A))
        player.pos.x -= 5;
    if(IsKeyDown(KEY_S))
        player.pos.y += 5;
    if(IsKeyDown(KEY_D))
        player.pos.x += 5;

    // collisions



    float x = GetMouseX() - player.pos.x;
    float y = GetMouseY() - player.pos.y;
    float rotation = atan2(x, y) * -57.29578f;

    // drawing the users bow
    DrawTexturePro(BOW, {0,0,16,16}, {float(player.pos.x + cos((rotation + 45) * DEG2RAD) * 32) + 16, float(player.pos.y + sin((rotation + 45) * DEG2RAD) * 32) + 16, 32, 32}, {0,0}, rotation + 135, WHITE);

    // when user shoots bow
    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        worldSprites.push_back({"arrow", {player.pos.x + 16, player.pos.y + 16}, 0, float(cos((rotation + 90) * DEG2RAD)), float(sin((rotation + 90) * DEG2RAD))}); 
}

int main(){

    // TODO make a projectile strut to handle the arrows and other future projectiles, literally only going to have arrows
    // handle collisions

    SetTraceLogLevel(LOG_ERROR);
    InitWindow(750,750, "animation");
    SetTargetFPS(60);
    initAnimation();
    Texture2D BOW = LoadTextureFromImage(LoadImage(BOW_PATH.c_str()));
    Texture2D ARROW = LoadTextureFromImage(LoadImage(ARROW_PATH.c_str()));
    Sprite player;
    player.name = "priest";
    player.pos = {float(GetScreenWidth() * 0.5), float(GetScreenHeight() * 0.5)};
    player.fc = 0;
    player.sx = 0;
    worldSprites.push_back(player);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);
        drawPlayer(worldSprites[0], BOW, ARROW);
        animate(ARROW);
        EndDrawing();
    }

    UnloadTexture(ARROW);
    UnloadTexture(BOW);
    unloadTextures();
    CloseWindow();
}