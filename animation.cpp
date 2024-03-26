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
    // x and y speeds
    float sx;
    float sy;
};

struct Projectile{
    Sprite sprite;
    float angle;
};

std::vector<Sprite> worldSprites;
std::vector<Projectile> worldProjectiles;

// adjusted mouse pos 
Vector2 mouse = {0, 0};

void unloadTextures(Texture2D BOW, Texture2D ARROW){
    UnloadTexture(BOW);
    UnloadTexture(ARROW);
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
void animateSprites(Texture2D ARROW){
    // inc the frame count fo every sprite, then based on its name, find the appropriate texture to draw
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
    } 
}

// animates all projectiles in world
void animateProjectiles(Texture2D ARROW, Camera2D camera){

    for(int i = 0; i < (int)worldProjectiles.size(); i++){
        if(worldProjectiles[i].sprite.name == "arrow")
            DrawTexturePro(ARROW, {0,8,16,16}, {worldProjectiles[i].sprite.pos.x, worldProjectiles[i].sprite.pos.y, 32, 32}, {0,0}, worldProjectiles[i].angle, WHITE);

        // update projectile movement
        worldProjectiles[i].sprite.pos.x += 10 * worldProjectiles[i].sprite.sx;
        worldProjectiles[i].sprite.pos.y += 10 * worldProjectiles[i].sprite.sy; 

        if(worldProjectiles[i].sprite.pos.x > camera.target.x + camera.offset.x 
			|| worldProjectiles[i].sprite.pos.x < camera.target.x - camera.offset.x 
			|| worldProjectiles[i].sprite.pos.y > camera.target.y + camera.offset.y 
			|| worldProjectiles[i].sprite.pos.y < camera.target.y - camera.offset.y)
            worldProjectiles.erase(worldProjectiles.begin() + i);
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

    // use mouse pos adjusted for camera
    float x = mouse.x - player.pos.x;
    float y = mouse.y - player.pos.y;
    float rotation = atan2(x, y) * -57.29578f;

    // drawing the users bow
    DrawTexturePro(BOW, {0,0,16,16}, {float(player.pos.x + cos((rotation + 45) * DEG2RAD) * 32) + 16, float(player.pos.y + sin((rotation + 45) * DEG2RAD) * 32) + 16, 32, 32}, {0,0}, rotation + 135, WHITE);

    // when user shoots bow
    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        worldProjectiles.push_back({{"arrow", player.pos.x + 16, player.pos.y + 16, 0, float(cos((rotation + 90) * DEG2RAD)), float(sin((rotation + 90) * DEG2RAD))}, rotation});
}

int main(){

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

    // init camera
    Camera2D camera = {0};
    camera.target = player.pos;
    camera.offset = (Vector2){GetScreenWidth()/2.0f, GetScreenHeight()/2.0f};
    // camera.rotation = 0.0f;
    camera.zoom = 1.0f;


    while (!WindowShouldClose()) {
	// set camera target
	camera.target = worldSprites[0].pos;
        camera.offset = (Vector2){GetScreenWidth()/2.0f, GetScreenHeight()/2.0f};

	// mouse position must be translated to camera target
    	mouse = GetScreenToWorld2D(GetMousePosition(), camera);
        BeginDrawing();
        	ClearBackground(BLACK);
		// everything in 2D mode will move according to camera
		BeginMode2D(camera);
			DrawRectangleRec((Rectangle){0, 0, 750, 750}, GRAY);
        		drawPlayer(worldSprites[0], BOW, ARROW);
        		animateSprites(ARROW);
        		animateProjectiles(ARROW, camera);
		EndMode2D();
		// everything outside of 2D mode is static
		DrawFPS(0, 0);
        EndDrawing();
    }

    unloadTextures(BOW, ARROW);
    CloseWindow();
    return 0;
}
