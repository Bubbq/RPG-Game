#include <bits/types/FILE.h>
#include <math.h>
#include <raylib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#undef RAYGUI_IMPLEMENTATION

const int SCREEN_WIDTH = 992;
const int SCREEN_HEIGHT = 992;

// update path to world you have previously saved
const char* WORLD_PATH = "test.txt";
const char* SPAWN_PATH = "spawn.txt";
const char* PLAYER_PATH = "Assets/player.png";

const char* BOUND_CADAVER = "EnemyAssets/Basic Undead Animations/Bound Cadaver/BoundCadaver.png";
const char* BRITTLE_ARCHER = "EnemyAssets/Basic Undead Animations/Brittle Archer/BrittleArcher.png";
const char* CARCASS_FEEDER = "EnemyAssets/Basic Undead Animations/Carcass Feeder/CarcassFeeder.png";
const char* DISMEMBERED_CRAWLER = "EnemyAssets/Basic Undead Animations/Dismembered Crawler/DismemberedCrawler.png";
const char* GHASTLY_EYE = "EnemyAssets/Basic Undead Animations/Ghastly Eye/GhastlyEye.png";
const char* GIANT_ROYAL_SCARAB = "EnemyAssets/Basic Undead Animations/Giant Royal Scarab/GiantRoyalScarab.png";
const char* GRAVE_REVANENT = "EnemyAssets/Basic Undead Animations/Grave Revenant/GraveRevenant.png";
const char* MUTILATED_STUMBLER = "EnemyAssets/Basic Undead Animations/Mutilated Stumbler/MutilatedStumbler.png";
const char* SAND_GHOUL = "EnemyAssets/Basic Undead Animations/Sand Ghoul/SandGhoul.png";
const char* SKITTERING_HAND = "EnemyAssets/Basic Undead Animations/Skittering Hand/SkitteringHand.png";
const char* TOXIC_HAND = "EnemyAssets/Basic Undead Animations/Toxic Hound/ToxicHound.png";
const char* UNRAVELING_CRAWLER = "EnemyAssets/Basic Undead Animations/Toxic Hound/ToxicHound.png";
const char* VAMPIRE_BAT = "EnemyAssets/Basic Undead Animations/Vampire Bat/VampireBat.png";
const char* DEATH_PROMPT = "YOU DIED, RESPAWN?"; 

const int ANIMATION_SPEED = 20;
const int FPS = 60;

const float SPAWN_TIME = 1.00;
const float LEVEL_TIME = 30.00;

void StartTimer(Timer *timer, double lifetime)
{
	timer->startTime = GetTime();
	timer->lifeTime = lifetime;
}

bool TimerDone(Timer timer)
{
	return GetTime() - timer.startTime >= timer.lifeTime;
}

void clearEntities(Entities* world_entities)
{
	free(world_entities->entities);
	world_entities->entities = malloc(ENTITY_CAP);
	world_entities->cap = ENTITY_CAP;
	world_entities->size = 0;
}

void resizeEntities(Entities* world_entities)
{
	world_entities->cap *= 2;
	world_entities->entities = realloc(world_entities->entities, world_entities->cap);
	
	if(world_entities->entities == NULL)
	{
		printf("ERROR RESZING ENTITIES \n");
		exit(1);
	}
}

void resizeLayer(TileList* layer)
{
    layer->cap *= 2;
    layer->list = realloc(layer->list, layer->cap);
    
    if(layer == NULL)
    {
        printf("ERROR RESIZING \n");
        exit(1);
    }
}

void addEntity(Entities* world_entities, Entity entity)
{
	if(world_entities->size * sizeof(Entity) == world_entities->cap)
	{
		resizeEntities(world_entities);
	}

	world_entities->entities[world_entities->size++] = entity;
}

void addTile(TileList* layer, Tile tile)
{
    if(layer->size * sizeof(Tile) == layer->cap)
    {
        resizeLayer(layer);
    }

    layer->list[layer->size++] = tile;
}

// either adds a new texture to list of textures or returns the existing texture of the one we want
Texture2D addTexture(Textures* textures, const char* filePath)
{
	int ctxi = -1;
	bool unique = true;

	for(int i = 0; i < textures->size; i++)
	{
		// if we alr have that texture, load it from the ith position
		if(strcmp(textures->better_textures[i].fp, filePath) == 0)
		{
			unique = false;
			ctxi = i;
			break;
		}
	}

	if(unique)
	{
		textures->better_textures[textures->size] = (BetterTexture){LoadTexture(filePath), "NULL"};
		strcpy(textures->better_textures[textures->size].fp, filePath);
		textures->size++;
	}

	// if we already have this tile, use it at that index, if not, the newest texture should hold the correct one to load
	ctxi = (ctxi == -1) ? (textures->size - 1) : ctxi;

	// return the texture of new or recurring element
	return textures->better_textures[ctxi].tx;
}

void loadWorld(World* world, const char* filePath)
{
    FILE* inFile = fopen(filePath, "r");
    char line[512];
    if(inFile == NULL)
    {
        printf("ERROR LOADING WORLD \n");
        return;
    }

    Vector2 src;
    Vector2 sp;
    enum Element tt;
    char* fp;
	bool anim;
	int fc;
	int frames;
	int anim_speed;

    while(fgets(line, sizeof(line), inFile))
    {
        src.x = atoi(strtok(line, ","));
        src.y = atoi(strtok(NULL, ","));
        sp.x = atoi(strtok(NULL, ","));
        sp.y = atoi(strtok(NULL, ","));
        tt = (enum Element)atoi(strtok(NULL, ","));
        fp = strtok(NULL, ",");
		anim = atoi(strtok(NULL, ","));
		fc = atoi(strtok(NULL, ","));
		frames = atoi(strtok(NULL, ","));
		anim_speed = atoi(strtok(NULL, "\n"));

        Tile tile = (Tile){src, sp, addTexture(&world->textures, fp), tt, "NULL", true, anim, fc, frames, anim_speed};
        strcpy(tile.fp, fp);

        switch (tt)
		{
			case WALL:
				addTile(&world->walls, tile);
				break;
			case FLOOR:
				addTile(&world->floors, tile);
				break;
			case DOOR:
				addTile(&world->doors, tile);
				break;
			case HEALTH_BUFF:
				addTile(&world->health_buffs, tile);
				break;
			case DAMAGE_BUFF:
				addTile(&world->damage_buffs, tile);
				break;
			case INTERACTABLE:
				addTile(&world->interactables, tile);
				break;
			default:
				break;
		}
    }

    fclose(inFile);
}

void drawLayer(TileList* layer, Rectangle* world_area)
{
    for(int i = 0; i < layer->size; i++)
    {
		Tile* tile = &layer->list[i];
		
		if(!tile->active)
		{
			continue;
		}

		int frame_pos = 0;

		// animate tiles
		if(tile->anim)
		{
			tile->fc++;
			frame_pos = tile->fc / tile->anim_speed;

			if(frame_pos > tile->frames)
			{
				frame_pos = 0;
				tile->fc = 0;
			}
		}

		// only draw when in screen
		if(CheckCollisionPointRec((Vector2){tile->sp.x + SCREEN_TILE_SIZE, tile->sp.y + SCREEN_TILE_SIZE}, *world_area))
		{
			DrawTexturePro(tile->tx, (Rectangle){tile->src.x + (frame_pos * TILE_SIZE), tile->src.y, TILE_SIZE, TILE_SIZE},
										(Rectangle){tile->sp.x, tile->sp.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE}, (Vector2){0,0}, 0, WHITE);
		}
    }
}

Vector2 getSpawn()
{
	FILE* inFile = fopen(SPAWN_PATH, "r");
	if(inFile == NULL)
	{
		printf("NO SPAWN POINT SAVED! \n");
		return (Vector2){0,0};
	}

	char line[512];
	Vector2 spawn;
	while(fgets(line, sizeof(line), inFile))
	{
		spawn.x = atoi(strtok(line, " "));
		spawn.y = atoi(strtok(NULL, "\n"));
	}

	fclose(inFile);
	return spawn;
}

void init(World* world, Entity* player, Camera2D* camera)
{
	// SetTraceLogLevel(LOG_ERROR);
	InitWindow(SCREEN_WIDTH,SCREEN_HEIGHT, "2D-Engine");
	SetTargetFPS(FPS);

	world->spawn_time = SPAWN_TIME;
	StartTimer(&world->spawn_timer, world->spawn_time);

	world->level_time = LEVEL_TIME;
	StartTimer(&world->level_timer, world->level_time);

	world->walls.size = 0;
	world->floors.size = 0;
	world->doors.size = 0;
	world->health_buffs.size = 0;
	world->damage_buffs.size = 0;
	world->interactables.size = 0;
	world->textures.size = 0;
	world->entities.size = 0;

	world->walls.list = malloc(TILE_CAP);
	world->floors.list = malloc(TILE_CAP);
	world->doors.list = malloc(TILE_CAP);
	world->health_buffs.list = malloc(TILE_CAP);
	world->damage_buffs.list = malloc(TILE_CAP);
	world->interactables.list = malloc(TILE_CAP);
	world->entities.entities = malloc(ENTITY_CAP);

    world->walls.cap = TILE_CAP;
    world->floors.cap = TILE_CAP;
    world->doors.cap = TILE_CAP;
    world->health_buffs.cap = TILE_CAP;
    world->damage_buffs.cap = TILE_CAP;
    world->interactables.cap = TILE_CAP;
	world->entities.cap = ENTITY_CAP;

    loadWorld(world, WORLD_PATH);
	
	world->spawn = getSpawn();

	player->name = "player";
	player->move = false;
	player->alive = true;
	player->adjsp = false;
	player->speed = 5;
	player->health = 100;
	player->frame_count = 0;
	player->anim_speed = ANIMATION_SPEED;
	player->pos = (Vector2){world->spawn.x, world->spawn.y};
	player->tx = addTexture(&world->textures, PLAYER_PATH);
	
	camera->zoom = 1.50f;
	camera->target = player->pos;
	camera->offset = (Vector2){GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
}

void deinit(World *world)
{
	free(world->walls.list);
	free(world->floors.list);
	free(world->doors.list);
	free(world->health_buffs.list);
	free(world->damage_buffs.list);
	free(world->interactables.list);
	free(world->entities.entities);

	for(int i = 0; i < world->textures.size; i++)
	{
		UnloadTexture(world->textures.better_textures[i].tx);
	}

	CloseWindow();
}

// returns the index of the tile of a layer you collided with
int entityCollisionWorld(Entity* en, TileList* layer)
{
	for(int i = 0; i < layer->size; i++)
	{
		if(CheckCollisionRecs((Rectangle){en->pos.x, en->pos.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE},
							(Rectangle){layer->list[i].sp.x, layer->list[i].sp.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE}) && layer->list[i].active)
		{
			return i;
		}
	}

	return -1;
}

// returns index of entity that an entity 'en' has collided with
int entityCollisionEntity(Entity* en, Entities* ents)
{
	for(int i = 0; i < ents->size; i++)
	{
		if(CheckCollisionPointRec((Vector2){en->pos.x + TILE_SIZE, en->pos.y + TILE_SIZE},
							(Rectangle){ents->entities[i].pos.x, ents->entities[i].pos.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE}) && ents->entities[i].id != en->id)
		{
			return i;
		}
	}

	return -1;
}

void updateEntities(Entities* world_entities, Entity* player, World* world)
{
	int wall_col, en_col = -1;

	for(int i = 0; i < world_entities->size; i++)
	{
		Entity* en = &world_entities->entities[i];
		
		if(!en->alive)
		{
			continue;
		}

		en->frame_count++;
		en->xfp = (en->frame_count / en->anim_speed);
		if(en->xfp > 3)
		{
			en->frame_count = 0;
			en->xfp = 0;
		}

		// find entities angle 
		float dx = player->pos.x - en->pos.x;
		float dy = player->pos.y - en->pos.y;

		// getting angular speed
		en->dx = cosf(en->angle * DEG2RAD) * en->speed;
		en->dy = sinf(en->angle * DEG2RAD) * en->speed;

		// make it nearest number divisible by 32
		en->angle = atan2f(dy, dx) * RAD2DEG;

		// angle correction
		if(en->angle > 360)
		{
			en->angle -= 360;
		}

		if(en->angle < 0)
		{
			en->angle += 360;
		}

		// setting sprite direction based on angle
		if(en->angle > 45 && en->angle < 135)
		{
			en->yfp = 0;
		}
		
		else if(en->angle > 225 && en->angle < 315)
		{
			en->yfp = 1;
		}

		else if(en->angle > 135 && en->angle < 215)
		{
			en->yfp = 3;
		}

		else
		{
			en->yfp = 2;
		}

		// collisions between walls and other enitites
		en->pos.x += en->dx;
		wall_col = entityCollisionWorld(en, &world->walls);
		en_col = entityCollisionEntity(en, &world->entities);

		if(wall_col >= 0 || en_col >= 0)
		{
			en->pos.x -= en->dx;
		}

		en->pos.y += en->dy;
		wall_col = entityCollisionWorld(en, &world->walls);
		en_col = entityCollisionEntity(en, &world->entities);

		if(wall_col >= 0 || en_col >= 0)
		{
			en->pos.y -= en->dy;
		}

		// displaying health bar
		DrawRectangle(en->pos.x + 5, en->pos.y - 7, 20 * (en->health / 100), 7, RED);
		DrawRectangleLines(en->pos.x + 5, en->pos.y - 7, 20, 7, BLACK);

		// only draw entity when in bounds
		if(CheckCollisionPointRec((Vector2){en->pos.x + SCREEN_TILE_SIZE, en->pos.y + SCREEN_TILE_SIZE}, world->area))
		{
			DrawTexturePro(en->tx, (Rectangle){en->xfp * TILE_SIZE, en->yfp * TILE_SIZE, TILE_SIZE, TILE_SIZE},
										(Rectangle){en->pos.x, en->pos.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE}, (Vector2){0,0}, 0, WHITE);
		}
	}
}

void move(TileList* world_walls, Entity* player)
{
	// flag for diagonal movement
    bool diagonal = (IsKeyDown(KEY_W) && IsKeyDown(KEY_D)) || 
                      (IsKeyDown(KEY_W) && IsKeyDown(KEY_A)) || 
                      	(IsKeyDown(KEY_A) && IsKeyDown(KEY_S)) || 
                      		(IsKeyDown(KEY_S) && IsKeyDown(KEY_D));

    if (diagonal)
    {
        if (!player->adjsp)
        {
			// through pythag thm: 2x^2 - player's speed^2 = 0, solve for x to find diag speed movement to prevent moving faster diagonly
            player->speed = sqrt(8 * (pow(player->speed, 2))) / 4;
			player->adjsp = true;
        }
    } else {
        player->speed = 3;
		player->adjsp = false;
    }
	
	int wall_col = -1;

	if(IsKeyDown(KEY_W))
	{
		player->pos.y -= player->speed;
		player->yfp = 1;
		player->move = true;

		wall_col = entityCollisionWorld(player, world_walls);
		if(wall_col >= 0)
		{
			player->pos.y += player->speed;
		}
	}

	if(IsKeyDown(KEY_A))
	{
		player->pos.x -= player->speed;
		player->yfp = 3;
		player->move = true;

		wall_col = entityCollisionWorld(player, world_walls);
		if(wall_col >= 0)
		{
			player->pos.x += player->speed;
		}
	}

	if(IsKeyDown(KEY_S))
	{
		player->pos.y += player->speed;
		player->yfp = 0;
		player->move = true;

		wall_col = entityCollisionWorld(player, world_walls);
		if(wall_col >= 0)
		{
			player->pos.y -= player->speed;
		}
	}

	if(IsKeyDown(KEY_D))
	{
		player->pos.x += player->speed;
		player->yfp = 2;
		player->move = true;

		wall_col = entityCollisionWorld(player, world_walls);
		if(wall_col >= 0)
		{
			player->pos.x -= player->speed;
		}
	}

	// afk
	if(!player->move)
	{
		player->xfp = player->yfp = 0;
	}

	// resetting movement flag
	player->move = false;
}

void heal(TileList* world_heals, Entity* player)
{
	int h_col =  entityCollisionWorld(player, world_heals);
	if(h_col >= 0)
	{
		DrawText("PRESSING H WILL HEAL", GetScreenWidth() - 310, GetScreenHeight() - 110, 20, GREEN);
		if(IsKeyPressed(KEY_H) && player->health < 100)
		{
			player->health = player->health + 20 > 100 ? 100 : player->health + 20;
			world_heals->list[h_col].active = false;
		}
	}

	// drawing the player's health bar
	DrawRectangle(GetScreenWidth() - 310, GetScreenHeight() - 60, (300) * (player->health / 100),50, RED);
	DrawRectangleLines(GetScreenWidth() - 310, GetScreenHeight() - 60, 300,50, BLACK);
	DrawText("HEALTH", GetScreenWidth() - 300, GetScreenHeight() - 80, 20, RED);
}

void fight(Entity* player, Entities* enemies)
{
		int mob_col = entityCollisionEntity(player, enemies);
		if(mob_col >= 0)
		{
			Entity* fight_en = &enemies->entities[mob_col];
			
			// hits are based on how fast mobs can hit player
			if(TimerDone(fight_en->hit_timer))
			{
				StartTimer(&fight_en->hit_timer, 1);
				player->health = (player->health - 20 < 0) ? 0 : player->health - 20;
				
				// when you die
				if(player->health == 0)
				{
					player->alive = false;
				}
			}

			// doing damage to mob
			if(IsKeyPressed(KEY_E))
			{
				fight_en->health = fight_en->health - 20 > 0 ? fight_en->health - 20 : 0;
				
				// killing an enemy
				if(fight_en->health == 0)
				{
					fight_en->alive = false;
					fight_en->pos = (Vector2){-1000000, -1000000};
					
					// gaining experience/leveling up
					player->exp += 20;
					if(player->exp >= 100)
					{
						player->exp = 0;
						player->level++;
					}
				}
			}
		}
		
		// drawing and updating players level
		DrawRectangle(10, GetScreenHeight() - 60, (300) * (player->exp / 100), 50, GREEN);
		DrawRectangleLines(10, GetScreenHeight() - 60, 300, 50, BLACK);
		
		char lvl_dsc[100]; sprintf(lvl_dsc, "LEVEL: %d", player->level);
		DrawText(lvl_dsc, 10, GetScreenHeight() - 80, 20, GREEN);
}

void updatePlayer(TileList* world_walls, Entity* player)
{
	// animation
	player->frame_count++;
	player->xfp = (player->frame_count / player->anim_speed);
	if(player->xfp > 3)
	{
		player->xfp = 0;
		player->frame_count = 0;
	}
	
	move(world_walls, player);

	// drawing player
	DrawTexturePro(player->tx, (Rectangle){player->xfp * TILE_SIZE, player->yfp * TILE_SIZE, TILE_SIZE, TILE_SIZE},
										(Rectangle){player->pos.x, player->pos.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE}, (Vector2){0,0}, 0, WHITE);
}

Entity createEnemy(World* world, Camera2D* camera)
{
	// basic mob properties
	Entity new_en;
	new_en.health = 100;
	new_en.speed = 1;
	new_en.alive = true;
	new_en.anim_speed = ANIMATION_SPEED;
	new_en.id = GetRandomValue(0, 1000000000);

	// randomly generate enemy texture
	Texture2D tx;
	int rand_tx = GetRandomValue(1, 13);

	switch (rand_tx)
	{
		case 1:	tx = addTexture(&world->textures, BOUND_CADAVER); break;
		case 2:	tx = addTexture(&world->textures, BRITTLE_ARCHER); break;
		case 3:	tx = addTexture(&world->textures, CARCASS_FEEDER); break;
		case 4:	tx = addTexture(&world->textures, DISMEMBERED_CRAWLER); break;
		case 5:	tx = addTexture(&world->textures, GHASTLY_EYE); break;
		case 6:	tx = addTexture(&world->textures, GIANT_ROYAL_SCARAB); break;
		case 7:	tx = addTexture(&world->textures, GRAVE_REVANENT); break;
		case 8:	tx = addTexture(&world->textures, MUTILATED_STUMBLER); break;
		case 9:	tx = addTexture(&world->textures, SAND_GHOUL); break;
		case 10: tx = addTexture(&world->textures, SKITTERING_HAND); break;
		case 11: tx = addTexture(&world->textures, TOXIC_HAND); break;
		case 12: tx = addTexture(&world->textures, UNRAVELING_CRAWLER); break;
		case 13: tx = addTexture(&world->textures, VAMPIRE_BAT); break;
		default:
			break;
	}

	new_en.tx = tx;
	
	// randomly spawn entity outside view of the player
	float cox = camera->offset.x;
	float coy = camera->offset.y;
	float ctx = camera->target.x;
	float cty = camera->target.y;
	int rand_pos = GetRandomValue(1, 4);

	switch (rand_pos)
	{
		// left side, varaible y
		case 1: new_en.pos = (Vector2){ctx - cox, GetRandomValue(cty - coy, cty + coy)}; break;
		// top side, varaible x
		case 2: new_en.pos = (Vector2){GetRandomValue(ctx - cox, ctx + cox), cty - coy}; break;
		// right side, varaible y
		case 3: new_en.pos = (Vector2){ctx + cox, GetRandomValue(cty - coy, cty + coy)}; break;
		// bottom side, varaible x
		case 4: new_en.pos = (Vector2){GetRandomValue(ctx + cox, ctx + cox), cty + coy}; break;
		default:
			break;
	}

	return new_en;
}

void spawnEnemies(World* world, Camera2D* camera)
{
	// spawn enemies every second
	if(TimerDone(world->spawn_timer))
	{
		// create enemy that has no collisions
		Entity en = createEnemy(world, camera);
		while(entityCollisionEntity(&en, &world->entities) >= 0) en = createEnemy(world, camera);
		addEntity(&world->entities, en);
		StartTimer(&world->spawn_timer, world->spawn_time);
	}

	// increase spawn rate every minutel
	if(TimerDone(world->level_timer))
	{
		world->spawn_time -= .05;
		StartTimer(&world->level_timer, world->level_time);
	}
}

bool deathWindow(World* world, Entity* player)
{
	ClearBackground(GRAY);

	DrawText(DEATH_PROMPT, (GetScreenWidth() / 2.0f) - (MeasureText(DEATH_PROMPT, 30) / 2.0f),
					GetScreenHeight() / 2.0f, 30, RED);

	if(GuiButton((Rectangle){(GetScreenWidth() / 2.0f) - 125, 
					(GetScreenHeight() / 2.0f) + 50, 100, 50}, "RESPAWN"))
	{
		clearEntities(&world->entities);
		player->pos = world->spawn;
		player->alive = true;
		player->health = 100;
		player->level = 0;
		
		world->spawn_time = 1.00;
		StartTimer(&world->spawn_timer, world->spawn_time);

		world->level_time = 30.00;
		StartTimer(&world->level_timer, world->level_time);

	}

	if(GuiButton((Rectangle){(GetScreenWidth() / 2.0f) + 50,
				(GetScreenHeight() / 2.0f) + 50, 100, 50}, "QUIT"))
	{
		return true;
	}

	return false;
}

int main()
{
	// Vector2 mp;
    World world;
	Entity player;
	Camera2D camera;
	init(&world, &player, &camera);

	while(!WindowShouldClose())
    {
		// mp = GetScreenToWorld2D(GetMousePosition(), player.camera);
		camera.target = player.pos;
		world.area = (Rectangle){camera.target.x - camera.offset.x, camera.target.y - camera.offset.y, SCREEN_WIDTH, SCREEN_HEIGHT};

	    BeginDrawing();
			if(player.alive)
			{
				ClearBackground(BLACK);
				BeginMode2D(camera);
					drawLayer(&world.floors, &world.area);
					drawLayer(&world.health_buffs, &world.area);
					drawLayer(&world.damage_buffs, &world.area);
					drawLayer(&world.doors, &world.area);
					drawLayer(&world.interactables, &world.area);
					drawLayer(&world.walls, &world.area);
					updateEntities(&world.entities, &player, &world);
					updatePlayer(&world.walls, &player);
				EndMode2D();
				
				spawnEnemies(&world, &camera);
				heal(&world.health_buffs, &player);
				fight(&player, &world.entities);
			}

			// death screen
			else
			{
				if(deathWindow(&world, &player)) break;
			}

		DrawFPS(0, 0);
		EndDrawing();
    }

    deinit(&world);
    return 0;
}