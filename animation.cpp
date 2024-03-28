#include <cmath>
#include <iostream>
#include <fstream>
#include <raylib.h>
#include <string>
#include <vector>


// how fast we change each frame
const int ANIMATION_SPEED = 20;
// size of tile in src png
const int TILE_SIZE = 16;
// making image from src png proportionate to the screen
const float SCALE = 2.0f;
const int WORLD_SIZE = 32;


const std::string BOUND_CADAVER_PATH = "EnemyAssets/Basic Undead Animations/Bound Cadaver/BoundCadaver.png";
const std::string BRITTLE_ARCHER_PATH = "EnemyAssets/Basic Undead Animations/Brittle Archer/BrittleArcher.png";
const std::string CARCASS_FEEDER_PATH = "EnemyAssets/Basic Undead Animations/Carcass Feeder/CarcassFeeder.png";
const std::string DISMEMBERED_CRAWLER_PATH = "EnemyAssets/Basic Undead Animations/Dismembered Crawler/DismemberedCrawler.png";
const std::string GHASTLY_EYE_PATH = "EnemyAssets/Basic Undead Animations/Ghastly Eye/GhastlyEye.png";
const std::string GIANT_ROYAL_SCARAB_PATH = "EnemyAssets/Basic Undead Animations/Giant Royal Scarab/GiantRoyalScarab.png";
const std::string GRAVE_REVANENT_PATH = "EnemyAssets/Basic Undead Animations/Grave Revenant/GraveRevenant.png";
const std::string MUTILATED_STUMBLER_PATH = "EnemyAssets/Basic Undead Animations/Mutilated Stumbler/MutilatedStumbler.png";
const std::string SAND_GHOUL_PATH = "EnemyAssets/Basic Undead Animations/Sand Ghoul/SandGhoul.png";
const std::string SKITTERING_HAND_PATH = "EnemyAssets/Basic Undead Animations/Skittering Hand/SkitteringHand.png";
const std::string TOXIC_HAND_PATH = "EnemyAssets/Basic Undead Animations/Toxic Hound/ToxicHound.png";
const std::string UNRAVELING_CRAWLER_PATH = "EnemyAssets/Basic Undead Animations/Toxic Hound/ToxicHound.png";
const std::string VAMPIRE_BAT_PATH = "EnemyAssets/Basic Undead Animations/Vampire Bat/VampireBat.png";
const std::string ARROW_PATH = "asset/arrow_4.png";
const std::string BOW_PATH = "asset/bow.png";


const std::string ASSET_PATH = "asset/character and tileset/Dungeon_Tileset_v2.png";
const std::string ITEM_PATH = "asset/character and tileset/Dungeon_item_props_v2.png";


// temporary texture of a sprite
Texture2D TMP;
Texture2D BOW;
Texture2D ARROW;
Texture2D BOUND_CADAVER;
Texture2D BRITTLE_ARCHER;
Texture2D CARCASS_FEEDER;
Texture2D DISMEMBERED_CRAWLER;
Texture2D GHASTLY_EYE;
Texture2D GIANT_ROYAL_SCARAB;
Texture2D GRAVE_REVANENT;
Texture2D MUTILATED_STUMBLER;
Texture2D SAND_GHOUL;
Texture2D SKITTERING_HAND;
Texture2D TOXIC_HAND;
Texture2D UNRAVELING_CRAWLER;
Texture2D VAMPIRE_BAT;
// tile and item textures
Texture2D ITEMS;
Texture2D TILES;
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
   Rectangle screenPos;
   std::string name;
   Element tileType;
   bool walkable;
};


// representing the current world a user is creating
struct World{
   std::vector<Tile> walls;
   std::vector<Tile> floors;
   std::vector<Tile> items;
   Vector2 spawn;
};


World world;


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


void unloadTextures(){
   UnloadTexture(ITEMS);
   UnloadTexture(TILES);
   UnloadTexture(TMP);
   UnloadTexture(BOW);
   UnloadTexture(ARROW);
   UnloadTexture(BOUND_CADAVER);
   UnloadTexture(BRITTLE_ARCHER);
   UnloadTexture(CARCASS_FEEDER);
   UnloadTexture(DISMEMBERED_CRAWLER);
   UnloadTexture(GHASTLY_EYE);
   UnloadTexture(GIANT_ROYAL_SCARAB);
   UnloadTexture(GRAVE_REVANENT);
   UnloadTexture(MUTILATED_STUMBLER);
   UnloadTexture(SAND_GHOUL);
   UnloadTexture(SKITTERING_HAND);
   UnloadTexture(TOXIC_HAND);
   UnloadTexture(UNRAVELING_CRAWLER);
   UnloadTexture(VAMPIRE_BAT);
}


// load the textures for every mob
void loadTextures(){
   ITEMS = LoadTextureFromImage(LoadImage(ITEM_PATH.c_str()));
   TILES = LoadTextureFromImage(LoadImage(ASSET_PATH.c_str()));
   BOW = LoadTextureFromImage(LoadImage(BOW_PATH.c_str()));
   ARROW = LoadTextureFromImage(LoadImage(ARROW_PATH.c_str()));
   BOUND_CADAVER = LoadTextureFromImage(LoadImage(BOUND_CADAVER_PATH.c_str()));
   BRITTLE_ARCHER = LoadTextureFromImage(LoadImage(BRITTLE_ARCHER_PATH.c_str()));
   CARCASS_FEEDER = LoadTextureFromImage(LoadImage(CARCASS_FEEDER_PATH.c_str()));
   DISMEMBERED_CRAWLER = LoadTextureFromImage(LoadImage(DISMEMBERED_CRAWLER_PATH.c_str()));
   GHASTLY_EYE = LoadTextureFromImage(LoadImage(GHASTLY_EYE_PATH.c_str()));
   GIANT_ROYAL_SCARAB = LoadTextureFromImage(LoadImage(GIANT_ROYAL_SCARAB_PATH.c_str()));
   GRAVE_REVANENT = LoadTextureFromImage(LoadImage(GRAVE_REVANENT_PATH.c_str()));
   MUTILATED_STUMBLER = LoadTextureFromImage(LoadImage(MUTILATED_STUMBLER_PATH.c_str()));
   SAND_GHOUL = LoadTextureFromImage(LoadImage(SAND_GHOUL_PATH.c_str()));
   SKITTERING_HAND = LoadTextureFromImage(LoadImage(SKITTERING_HAND_PATH.c_str()));
   TOXIC_HAND = LoadTextureFromImage(LoadImage(TOXIC_HAND_PATH.c_str()));
   UNRAVELING_CRAWLER = LoadTextureFromImage(LoadImage(UNRAVELING_CRAWLER_PATH.c_str()));
   VAMPIRE_BAT = LoadTextureFromImage(LoadImage(VAMPIRE_BAT_PATH.c_str()));
}


// load world content from file
void loadWorld(int& cl){


   std::ifstream inFile;
   inFile.open("./levels/level" + std::to_string(cl) + "/walls.txt");
   if(!inFile){
       std::cerr << "NO WALLS SAVED \n";
       return;
   }


   int x,y;
   Vector2 src;
   Vector2 screenPos;
   std::string name;
   int tileType;
   bool walkable;


   while(inFile >> x >> y >> src.x >> src.y >> name >> tileType >> walkable >> screenPos.x >> screenPos.y)
       world.walls.push_back({{src.x, src.y}, {screenPos.x, screenPos.y}, name, (Element)tileType, walkable});


   inFile.close();
  
   inFile.open("./levels/level" + std::to_string(cl) + "/floors.txt");
   if(!inFile){
       std::cerr << "NO FLOORS SAVED \n";
       return;
   }


   while(inFile >> x >> y >> src.x >> src.y >> name >> tileType >> walkable >> screenPos.x >> screenPos.y)
       world.floors.push_back({src, {screenPos.x, screenPos.y}, name, (Element)tileType, walkable});


   inFile.close();


   inFile.open("./levels/level" + std::to_string(cl) + "/items.txt");
   if(!inFile){
       std::cerr << "NO ITEMS SAVED \n";
       return;
   }


   while(inFile >> x >> y >> src.x >> src.y >> name >> tileType >> walkable >> screenPos.x >> screenPos.y)
       world.items.push_back({{src.x, src.y}, {screenPos.x, screenPos.y}, name, (Element)tileType, walkable});


   inFile.close();
}


// drawing the world
void drawWorld(Texture2D& texture, Texture2D itemTexture){


   for(int i = 0; i < world.walls.size(); i++)
       if(!world.walls[i].name.empty())
           DrawTexturePro(texture, {world.walls[i].src.x, world.walls[i].src.y, TILE_SIZE, TILE_SIZE}, {world.walls[i].screenPos.x, world.walls[i].screenPos.y, TILE_SIZE * SCALE, TILE_SIZE * SCALE}, {0,0}, 0, WHITE);
  
   for(int i = 0; i < world.floors.size(); i++)
       if(!world.floors[i].name.empty())
           DrawTexturePro(texture, {world.floors[i].src.x, world.floors[i].src.y, TILE_SIZE, TILE_SIZE}, {world.floors[i].screenPos.x, world.floors[i].screenPos.y, TILE_SIZE * SCALE, TILE_SIZE * SCALE}, {0,0}, 0, WHITE);
  
   for(int i = 0; i < world.items.size(); i++)
       if(!world.items[i].name.empty())
           DrawTexturePro(itemTexture, {world.items[i].src.x, world.items[i].src.y, TILE_SIZE, TILE_SIZE}, {world.items[i].screenPos.x, world.items[i].screenPos.y, TILE_SIZE * SCALE, TILE_SIZE * SCALE}, {0,0}, 0, WHITE);
}
 
// to animate world sprites, POSSIBLE ERROR, NOT UNLOADING THE TMP TEXTURE
void animateSprites(){


   // finding the appropriate frame position
   for(int i = 0; i < worldSprites.size(); i++){
       worldSprites[i].fc++;
       int fp = worldSprites[i].fc / ANIMATION_SPEED;
       if(fp > 3){
           fp = 0;
           worldSprites[i].fc = 0;
       }


       // drawing said textures
       if(worldSprites[i].name == "bounded_cadaver")
           TMP = BOUND_CADAVER;
       else if(worldSprites[i].name == "brittle_archer")
           TMP = BRITTLE_ARCHER;
       else if(worldSprites[i].name == "carcass_feeder")
           TMP = CARCASS_FEEDER;
       else if(worldSprites[i].name == "dismembered_crawler")
           TMP = DISMEMBERED_CRAWLER;
       else if(worldSprites[i].name == "ghastly_eye")
           TMP = GHASTLY_EYE;
       else if(worldSprites[i].name == "giant_royal_scarab")
           TMP = GIANT_ROYAL_SCARAB;
       else if(worldSprites[i].name == "grave_revanent")
           TMP = GRAVE_REVANENT;
       else if(worldSprites[i].name == "mutilated_stumbler")
           TMP = MUTILATED_STUMBLER;
       else if(worldSprites[i].name == "sand_ghoul")
           TMP = SAND_GHOUL;
       else if(worldSprites[i].name == "toxic_hand")
           TMP = SKITTERING_HAND;
       else if(worldSprites[i].name == "unraveling_crawler")
           TMP = UNRAVELING_CRAWLER;
       else if(worldSprites[i].name == "vampire_bat")
           TMP = VAMPIRE_BAT;


       // draw the sprite with its proper animation frame
       DrawTexturePro(TMP, {float(fp * TILE_SIZE), 0, TILE_SIZE, TILE_SIZE}, {worldSprites[i].pos.x, worldSprites[i].pos.y, TILE_SIZE * SCALE, TILE_SIZE * SCALE}, {0,0}, 0, WHITE);       
   }
}


// animates all projectiles in world
void animateProjectiles(Camera2D camera){
    for(int i = 0; i < worldProjectiles.size(); i++){
        // finding the right projectile to draw
        if(worldProjectiles[i].sprite.name == "arrow")
            TMP = ARROW;
        
        // drawing the ith sprite
        DrawTexturePro(TMP, {0,8, TILE_SIZE,TILE_SIZE}, {worldProjectiles[i].sprite.pos.x, worldProjectiles[i].sprite.pos.y, TILE_SIZE * SCALE, TILE_SIZE * SCALE}, {0,0}, worldProjectiles[i].angle, WHITE);
        
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


void drawWalls(World& world){
   for(int j = 0;  j < world.walls.size(); j++){
       if(!world.walls[j].name.empty()){
           DrawRectangleLines( world.walls[j].screenPos.x,world.walls[j].screenPos.y, TILE_SIZE * SCALE, TILE_SIZE * SCALE, RED);
       }
   }
}


void drawPlayer(Sprite& player){


   int ppx = (((int)player.pos.x >> (int)log2(TILE_SIZE * SCALE)) << (int)log2(TILE_SIZE * SCALE));
   int ppy = (((int)player.pos.y >> (int)log2(TILE_SIZE * SCALE)) << (int)log2(TILE_SIZE * SCALE));
   // std::cout << "PLAYER POS: (" << ppx << "," << ppy << ")" << std::endl;
      
   // movement and collisions
   if(IsKeyDown(KEY_W)){
       player.pos.y -= 5;
       for(int i = 0; i < world.walls.size(); i++)
           if(CheckCollisionPointRec(player.pos, {world.walls[i].screenPos.x, world.walls[i].screenPos.y, TILE_SIZE * SCALE, TILE_SIZE * SCALE}))
               player.pos.y += 5;
      
   }
   if(IsKeyDown(KEY_A)){
       player.pos.x -= 5;
       for(int i = 0; i < world.walls.size(); i++)
           if(CheckCollisionPointRec(player.pos, {world.walls[i].screenPos.x, world.walls[i].screenPos.y, TILE_SIZE * SCALE, TILE_SIZE * SCALE}))
               player.pos.x += 5;
   }


   if(IsKeyDown(KEY_S)){
       player.pos.y += 5;
       for(int i = 0; i < world.walls.size(); i++)
           if(CheckCollisionPointRec({player.pos.x, player.pos.y + TILE_SIZE}, {world.walls[i].screenPos.x, world.walls[i].screenPos.y, TILE_SIZE * SCALE, TILE_SIZE * SCALE}))
               player.pos.y -= 5;
   }


   if(IsKeyDown(KEY_D)){
       player.pos.x += 5;
       for(int i = 0; i < world.walls.size(); i++)
           if(CheckCollisionPointRec({player.pos.x + TILE_SIZE, player.pos.y}, {world.walls[i].screenPos.x, world.walls[i].screenPos.y, TILE_SIZE * SCALE, TILE_SIZE * SCALE}))
               player.pos.x -= 5;
   }
  
   // use mouse pos adjusted for camera
   float x = mouse.x - player.pos.x;
   float y = mouse.y - player.pos.y;
   float rotation = atan2(x, y) * -57.29578f;


   // drawing the users bow
   DrawTexturePro(BOW, {0,0,16,16}, {float(player.pos.x + cos((rotation + 45) * DEG2RAD) * 32) + 16, float(player.pos.y + sin((rotation + 45) * DEG2RAD) * 32) + 16, 32, 32}, {0,0}, rotation + 135, WHITE);

   // when user shoots bow
   if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
       worldProjectiles.push_back({{"arrow", player.pos.x + 16 , player.pos.y + 16, 0, float(cos((rotation + 90) * DEG2RAD)), float(sin((rotation + 90) * DEG2RAD))}, rotation});
}


Vector2 getSpawn(int cl){
   std::ifstream inFile;
   inFile.open("levels/level" + std::to_string(cl) + "/spawn.txt");
   Vector2 spawnPoint;
   inFile >> spawnPoint.x >> spawnPoint.y;
   inFile.close();
   return spawnPoint;
}


int main(){


   SetTraceLogLevel(LOG_ERROR);
   InitWindow(900,900, "animation");
   SetTargetFPS(60);
   int cl = 1;
   loadWorld(cl);
   loadTextures();
   Sprite player = {"vampire_bat", getSpawn(cl), 0,0,0};
   worldSprites.push_back(player);


   // init camera
   Camera2D camera = {0};
   camera.target = player.pos;
   camera.offset = (Vector2){GetScreenWidth()/2.0f, GetScreenHeight()/2.0f};
   camera.zoom = 1.5f;




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
       drawWorld(TILES,ITEMS);
    //    drawWalls(world);
               drawPlayer(worldSprites[0]);
               animateSprites();
               animateProjectiles(camera);
       EndMode2D();
       // everything outside of 2D mode is static
       DrawFPS(0, 0);
       EndDrawing();
   }
   unloadTextures();
   CloseWindow();
   return 0;
}



