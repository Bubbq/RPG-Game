#include <cmath>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <raylib.h>
#include <string>
#include <vector>
#include "tile_generation.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

// how fast we change each frame
const int ANIMATION_SPEED = 20;
int SPAWN = 60;

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
Texture2D TILES;
Texture2D ITEMS;

struct Timer {
	double startTime;   // Start time (seconds)
	double lifeTime;    // Lifetime (seconds)
};

Timer spawnTimer;
Timer hitTimer;

struct Sprite{
  std::string name;
  Vector2 pos;
  int fc;
};

struct Projectile{
  Sprite sprite;
  float angle;
  float sx;
  float sy;
};

struct Entity{
   Sprite sprite;
   float health;
   float speed;
   bool alive;
};

std::vector<Entity> worldEntity;
std::vector<Projectile> worldProjectiles;

World world;

// adjusted mouse pos
Vector2 mouse = {0, 0};
void StartTimer(Timer *timer, double lifetime)
{
	timer->startTime = GetTime();
	timer->lifeTime = lifetime;
}

bool TimerDone(Timer timer)
{
	return GetTime() - timer.startTime >= timer.lifeTime;
}

void unloadTextures()
{
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
void loadTextures()
{
   TILES = LoadTextureFromImage(LoadImage(TILE_ASSET_PATH.c_str()));
   ITEMS = LoadTextureFromImage(LoadImage(ITEM_ASSET_PATH.c_str()));
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
// to animate world sprites
void animateSprites()
{
   // finding  appropriate frame position
   for(int i = 0; i < worldEntity.size(); i++)
   {
	   worldEntity[i].sprite.fc++;
	   int fp = worldEntity[i].sprite.fc / ANIMATION_SPEED;
	   if(fp > 3){
		   fp = 0;
		   worldEntity[i].sprite.fc = 0;
	   }

	   // drawing said textures
	   if(worldEntity[i].sprite.name == "bounded_cadaver")
		   TMP = BOUND_CADAVER;
	   else if(worldEntity[i].sprite.name == "brittle_archer")
		   TMP = BRITTLE_ARCHER;
	   else if(worldEntity[i].sprite.name == "carcass_feeder")
		   TMP = CARCASS_FEEDER;
	   else if(worldEntity[i].sprite.name == "dismembered_crawler")
		   TMP = DISMEMBERED_CRAWLER;
	   else if(worldEntity[i].sprite.name == "ghastly_eye")
		   TMP = GHASTLY_EYE;
	   else if(worldEntity[i].sprite.name == "giant_royal_scarab")
		   TMP = GIANT_ROYAL_SCARAB;
	   else if(worldEntity[i].sprite.name == "grave_revanent")
		   TMP = GRAVE_REVANENT;
	   else if(worldEntity[i].sprite.name == "mutilated_stumbler")
		   TMP = MUTILATED_STUMBLER;
	   else if(worldEntity[i].sprite.name == "sand_ghoul")
		   TMP = SAND_GHOUL;
	   else if(worldEntity[i].sprite.name == "toxic_hand")
		   TMP = SKITTERING_HAND;
	   else if(worldEntity[i].sprite.name == "unraveling_crawler")
		   TMP = UNRAVELING_CRAWLER;
	   else if(worldEntity[i].sprite.name == "vampire_bat")
		   TMP = VAMPIRE_BAT;

	   // draw the sprite with its proper animation frame
	   DrawTexturePro(TMP, {float(fp * TILE_SIZE), 0, TILE_SIZE, TILE_SIZE}, {worldEntity[i].sprite.pos.x, worldEntity[i].sprite.pos.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE}, {0,0}, 0, WHITE);      
   }
}

// animates all projectiles in world
void animateProjectiles(Camera2D camera)
{
   for(int i = 0; i < worldProjectiles.size(); i++)
   {
	   // finding the right projectile to draw
	   if(worldProjectiles[i].sprite.name == "arrow")
		   TMP = ARROW;
	  
	   // drawing the ith sprite
	   DrawTexturePro(TMP, {0,8, TILE_SIZE,TILE_SIZE}, {worldProjectiles[i].sprite.pos.x, worldProjectiles[i].sprite.pos.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE}, {0,0}, worldProjectiles[i].angle, WHITE);
	  
	   // update projectile movement
	   worldProjectiles[i].sprite.pos.x += 10 * worldProjectiles[i].sx;
	   worldProjectiles[i].sprite.pos.y += 10 * worldProjectiles[i].sy;
 
	   // removal when projectile s out of the screen
	   if(worldProjectiles[i].sprite.pos.x > camera.target.x + camera.offset.x
		   || worldProjectiles[i].sprite.pos.x < camera.target.x - camera.offset.x
		   || worldProjectiles[i].sprite.pos.y > camera.target.y + camera.offset.y
		   || worldProjectiles[i].sprite.pos.y < camera.target.y - camera.offset.y)
		   worldProjectiles.erase(worldProjectiles.begin() + i);
	  
	   // colliding with walls
	   for(int k = 0; k < world.walls.size(); k++){
		   if(CheckCollisionRecs({worldProjectiles[i].sprite.pos.x, worldProjectiles[i].sprite.pos.y, SCREEN_TILE_SIZE,SCREEN_TILE_SIZE}, {world.walls[k].screenPos.x, world.walls[k].screenPos.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE}))
			   if(!worldProjectiles.empty())
				   worldProjectiles.erase(worldProjectiles.begin() + i);
	   }

	   // deal damage to enemies
	   for(int j = 1; j < worldEntity.size(); j++){
		   if(CheckCollisionRecs({worldProjectiles[i].sprite.pos.x, worldProjectiles[i].sprite.pos.y, TILE_SIZE, TILE_SIZE}, {worldEntity[j].sprite.pos.x, worldEntity[j].sprite.pos.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE}) && !worldProjectiles.empty())
		   {
			   worldEntity[j].health -= 100;
			   if(worldEntity[j].health <= 0)
			   {
				   worldProjectiles.erase(worldProjectiles.begin() + i);
				   worldEntity.erase(worldEntity.begin() + j);
			   }
		   }
	   }
   }
}

void updateMobs()
{
   // go through every entity and make them move towards the player
   for(int i = 1; i < worldEntity.size(); i++)
   {
	   if(worldEntity[i].sprite.pos.x < worldEntity[0].sprite.pos.x)
	   {
		   worldEntity[i].sprite.pos.x += worldEntity[i].speed;
		   for(int j = 0; j < world.walls.size(); j++){
			
				if(CheckCollisionRecs({worldEntity[i].sprite.pos.x, worldEntity[i].sprite.pos.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE}, {world.walls[j].screenPos.x, world.walls[j].screenPos.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE}))
			   		worldEntity[i].sprite.pos.x -= worldEntity[i].speed;
		   }
	   }
	   if(worldEntity[i].sprite.pos.x > worldEntity[0].sprite.pos.x)
	   {
		   worldEntity[i].sprite.pos.x -= worldEntity[i].speed;
			for(int j = 0; j < world.walls.size(); j++)
			{

				if(CheckCollisionRecs({worldEntity[i].sprite.pos.x, worldEntity[i].sprite.pos.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE}, {world.walls[j].screenPos.x, world.walls[j].screenPos.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE}))
			   		worldEntity[i].sprite.pos.x += worldEntity[i].speed;
			}
	   }
	   if(worldEntity[i].sprite.pos.y < worldEntity[0].sprite.pos.y)
		{
		   worldEntity[i].sprite.pos.y += worldEntity[i].speed;
			for(int j = 0; j < world.walls.size(); j++)
			{

				if(CheckCollisionRecs({worldEntity[i].sprite.pos.x, worldEntity[i].sprite.pos.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE}, {world.walls[j].screenPos.x, world.walls[j].screenPos.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE}))
			   		worldEntity[i].sprite.pos.y -= worldEntity[i].speed;
			}
	   }
	   if(worldEntity[i].sprite.pos.y > worldEntity[0].sprite.pos.y)
		{
		   worldEntity[i].sprite.pos.y -= worldEntity[i].speed;
			for(int j = 0; j < world.walls.size(); j++)
			{

				if(CheckCollisionRecs({worldEntity[i].sprite.pos.x, worldEntity[i].sprite.pos.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE}, {world.walls[j].screenPos.x, world.walls[j].screenPos.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE}))
			   		worldEntity[i].sprite.pos.y += worldEntity[i].speed;
			}
	   }
   }
}

void updatePlayer(Entity& player)
{
   // movement and collisions
   if(IsKeyDown(KEY_W))
   {
	   player.sprite.pos.y -= 5;
	   for(int i = 0; i < world.walls.size(); i++)
		   if(CheckCollisionPointRec(player.sprite.pos, {world.walls[i].screenPos.x, world.walls[i].screenPos.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE}))
			   player.sprite.pos.y += 5;
   }

   if(IsKeyDown(KEY_A))
   {
	   player.sprite.pos.x -= 5;
	   for(int i = 0; i < world.walls.size(); i++)
		   if(CheckCollisionPointRec(player.sprite.pos, {world.walls[i].screenPos.x, world.walls[i].screenPos.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE}))
			   player.sprite.pos.x += 5;
   }

   if(IsKeyDown(KEY_S))
   {
	   player.sprite.pos.y += 5;
	   for(int i = 0; i < world.walls.size(); i++)
		   if(CheckCollisionPointRec({player.sprite.pos.x, player.sprite.pos.y + TILE_SIZE}, {world.walls[i].screenPos.x, world.walls[i].screenPos.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE}))
			   player.sprite.pos.y -= 5;
   }

   if(IsKeyDown(KEY_D))
   {
	   player.sprite.pos.x += 5;
	   for(int i = 0; i < world.walls.size(); i++)
		   if(CheckCollisionPointRec({player.sprite.pos.x + TILE_SIZE, player.sprite.pos.y}, {world.walls[i].screenPos.x, world.walls[i].screenPos.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE}))
			   player.sprite.pos.x -= 5;
   }

   // use mouse pos adjusted for camera
   float x = mouse.x - player.sprite.pos.x;
   float y = mouse.y - player.sprite.pos.y;
   float rotation = atan2(x, y) * -57.29578f;

   // drawing the users bow
   DrawTexturePro(BOW, {0,0,TILE_SIZE, TILE_SIZE}, {float(player.sprite.pos.x + cos((rotation + 45) * DEG2RAD) * SCREEN_TILE_SIZE) + TILE_SIZE, float(player.sprite.pos.y + sin((rotation + 45) * DEG2RAD) * SCREEN_TILE_SIZE) + TILE_SIZE,SCREEN_TILE_SIZE,SCREEN_TILE_SIZE}, {0,0}, rotation + 135, WHITE);

   // when user shoots bow
   if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
   {
	   Sprite tmp = {"arrow", {player.sprite.pos.x, player.sprite.pos.y}, 0};
	   float sx = cos((rotation + 90) * DEG2RAD);
	   float sy = sin((rotation + 90) * DEG2RAD);
	   Projectile pro = {tmp, rotation, sx, sy};
	   worldProjectiles.push_back(pro);
   }

   // getting hit by enemy
   for(int i = 1; i < worldEntity.size(); i++)
   {
	   if(CheckCollisionRecs({player.sprite.pos.x, player.sprite.pos.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE}, {worldEntity[i].sprite.pos.x, worldEntity[i].sprite.pos.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE}) && TimerDone(hitTimer)) 
		{
			StartTimer(&hitTimer, 1.00);
			player.health -= 20;
			std::cout << player.health << std::endl;
			if(player.health <= 0)
				player.alive = false;
		}
   }
}



// create mobs (inc over time)
void spawnMobs(int& fc, Camera2D camera)
{
	// incmenting the spawn rate every min
	if((TimerDone(spawnTimer)))
	{ 
		std::cout << std::ceil(GetTime()) << std::endl;
		fc = 0;
		SPAWN -= 3;
		StartTimer(&spawnTimer, 60);
	}
	
	if(fc % SPAWN == 0)
	{
		fc = 0;
		int pos = (rand() % 4) + 1;
		switch (pos)
		{
			case 1:
				worldEntity.push_back({"ghastly_eye", (Vector2){camera.target.x - camera.offset.x, static_cast<float>(GetRandomValue(camera.target.y - camera.offset.y,camera.target.y + camera.offset.y))}, 0, 100, 1, true});
				break;
			case 2: 
				worldEntity.push_back({"ghastly_eye", (Vector2){static_cast<float>(GetRandomValue(camera.target.x - camera.offset.x,camera.target.x + camera.offset.x)), camera.target.y - camera.offset.y}, 0, 100, 1, true});
				break;
			case 3:
				worldEntity.push_back({"ghastly_eye", (Vector2){camera.target.x + camera.offset.x, static_cast<float>(GetRandomValue(camera.target.y - camera.offset.y,camera.target.y + camera.offset.y))}, 0, 100, 1, true});
				break;
			default:
				worldEntity.push_back({"ghastly_eye", (Vector2){static_cast<float>(GetRandomValue(camera.target.x - camera.offset.x,camera.target.x + camera.offset.x)), camera.target.y + camera.offset.y}, 0, 100, 1, true});
				break; 
			}
	}
}

// returns the spawn point of the player in the ith level
Vector2 getSpawn(int& cl)
{
  std::ifstream inFile;
  inFile.open("levels/level" + std::to_string(cl) + "/spawn.txt");
  if(!inFile)
  {
   std::cerr << "ERROR GETTING SPAWN POINT \n";
   return {0,0};
  }
  Vector2 spawnPoint;
  inFile >> spawnPoint.x >> spawnPoint.y;
  inFile.close();
  return spawnPoint;
}

int main()
{
	SetTraceLogLevel(LOG_ERROR);
  	InitWindow(900,900, "animation");
	SetTargetFPS(60);
	int cl = 1;
	loadWorld(world, cl);
	loadTextures();
	Entity player = {{"grave_revanent", getSpawn(cl), 0}, 100, 5, true};
	worldEntity.push_back(player);
	// init camera
	StartTimer(&spawnTimer, 60.00);
	StartTimer(&hitTimer, 1.00);
	Camera2D camera = {0};
	camera.target = player.sprite.pos;
	camera.offset = (Vector2){GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
	camera.zoom = 1.5f;
  	int fc = 0;

  while (!WindowShouldClose())
  {
	  // update camera target
	  camera.target = worldEntity[0].sprite.pos;
	  camera.offset = (Vector2){GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
	  // mouse position must be translated to camera target
	  mouse = GetScreenToWorld2D(GetMousePosition(), camera);
	  BeginDrawing();
		  ClearBackground(BLACK);
		  // everything in 2D mode will move according to camera
		  BeginMode2D(camera);
			  if(worldEntity[0].alive)
			  {
				spawnMobs(++fc, camera);
				  drawWorld(world, TILES, ITEMS, 0);
				  updatePlayer(worldEntity[0]);
				  updateMobs();
				  animateSprites();
				  animateProjectiles(camera);
			  }
			  else
			  {
				  ClearBackground(GRAY);
				  DrawText("YOU DIED, TRY AGAIN? (PRESS ENTER)", player.sprite.pos.x - MeasureText("YOU DIED, TRY AGAIN? (PRESS ENTER)", 20) / 2.0f, player.sprite.pos.y, 20, RED);
				  if(IsKeyPressed(KEY_ENTER))
				  {
				   worldEntity[0].alive = true;
				   worldEntity.clear();
				   worldEntity.push_back(player);
				  }
			  }
		  EndMode2D();
		  // everything outside of 2D mode is static
		  DrawFPS(0, 0);
	  EndDrawing();
  }
  unloadTextures();
  CloseWindow();
  return 0;
}