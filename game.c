#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game.h"

#define MAX_WEAPONS 5
#define MAX_WEAPON_COUNT 6

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#undef RAYGUI_IMPLEMENTATION

const int SCREEN_WIDTH = 992;
const int SCREEN_HEIGHT = 992;

// update path to world you have previously saved
const char* WORLD_PATH = "world.txt";
const char* SPAWN_PATH = "spawn.txt";

// paths to entities textures
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
// const char* ARROW = "Assets/arrow.png";
const char* SHURIKEN_PATH = "Assets/SawBladeSuriken.png";

const int ANIMATION_SPEED = 20;
const int FPS = 60;

// game times (in seconds)
const float SPAWN_TIME = 1.00;
const float LEVEL_TIME = 30.00;

// null elements (using lazy deletion)
const Entity NULL_ENTITY = (Entity){0};
const Weapon NULL_WEAPON = (Weapon){0};

// mouse position relative to 2D screen
Vector2 mp;

const Weapon SHURIKEN = {"shuriken", (Vector2){0,0}, 20, 0, 4, 3, 1.00, 1, 15};
const Weapon WHIP = {"whip", (Vector2){0,0}, 30, 0, 0, 1, 2.00, 0, 0, 0.30, 0.50};
const Weapon AOE = {"aoe", (Vector2){0,0}, 2, 0, 0, 3, 3, 0, 0, 3, .1};
const Weapon FIREBALL = {"fireball", (Vector2){0,0}, 50, 0, 2, 1, 5, 1, 1, 0, 0};
const Weapon MAGIC_MISSLE = {"missle", (Vector2){0,0}, 10, 0, 2, 3, 2, 1, 1, 0, 0};

Timer weapon_activation_times[MAX_WEAPONS][MAX_WEAPON_COUNT];
bool add_weapon[MAX_WEAPONS][MAX_WEAPON_COUNT];

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

void clearWeapons(Weapons* world_weapons)
{
	free(world_weapons->list);
	world_weapons->list = malloc(WEAPON_CAP);
	world_weapons->cap = WEAPON_CAP;
	world_weapons->size = 0;
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

void resizeWeapons(Weapons* world_weapons)
{
	world_weapons->cap *= 2;
	world_weapons->list = realloc(world_weapons->list, world_weapons->cap);
	
	if(world_weapons->list == NULL)
	{
		printf("ERROR RESZING WEAPONS \n");
		exit(1);
	}
}

void resizeLayer(TileList* layer)
{
    layer->cap *= 2;
    layer->list = realloc(layer->list, layer->cap);
    
    if(layer == NULL)
    {
        printf("ERROR RESIZING TILE LAYER\n");
        exit(1);
    }
}

void addEntity(Entities* world_entities, Entity entity)
{
	if(world_entities->size * sizeof(Entity) == world_entities->cap) resizeEntities(world_entities);

	world_entities->entities[world_entities->size++] = entity;
}

// returns angle (in degrees) from 2 cartesian coordinates
float getAngle(Vector2 v1, Vector2 v2)
{
	float dx = v1.x - v2.x;
	float dy = v1.y - v2.y;

	float angle = atan2f(dy, dx) * RAD2DEG;
	
	// angle correction
	if(angle > 360) angle -= 360;
	else if(angle < 0) angle += 360;

	return angle;
}

void addWeapon(Weapons* world_weapons, Weapon* wp, Vector2 pp)
{
	wp->on_screen = true;
	wp->dx = cosf(wp->angle * DEG2RAD) * wp->speed;
	wp->dy = sinf(wp->angle * DEG2RAD) * wp->speed;
	if(world_weapons->size * sizeof(Weapon) == world_weapons->cap) resizeWeapons(world_weapons);
	world_weapons->list[world_weapons->size++] = *wp;
}

void addTile(TileList* layer, Tile tile)
{
    if(layer->size * sizeof(Tile) == layer->cap) resizeLayer(layer);

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
		
		if(!tile->active) continue;

		int frame_pos = 0;

		// animate tiles
		if(tile->anim)
		{
			tile->fc++;
			frame_pos = (tile->fc / tile->anim_speed);
			if(frame_pos > tile->frames) frame_pos = tile->fc = 0;
		}

		// only draw when in screen
		Rectangle tile_area = (Rectangle){tile->sp.x, tile->sp.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE};
		if(CheckCollisionRecs(tile_area, *world_area))
		{
			tile->on_screen = true;

			Rectangle src_area = (Rectangle){tile->src.x + (frame_pos * TILE_SIZE), tile->src.y, TILE_SIZE, TILE_SIZE};
			Rectangle dst_area = (Rectangle){tile->sp.x, tile->sp.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE};
			DrawTexturePro(tile->tx, src_area, dst_area, (Vector2){0,0}, 0, WHITE);
		}

		else tile->on_screen = false;
    }
}

// returns the spawn point of the world from 'spawn.txt' file
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
	world->weapons.size = 0;

	world->walls.list = malloc(TILE_CAP);
	world->floors.list = malloc(TILE_CAP);
	world->doors.list = malloc(TILE_CAP);
	world->health_buffs.list = malloc(TILE_CAP);
	world->damage_buffs.list = malloc(TILE_CAP);
	world->interactables.list = malloc(TILE_CAP);
	world->entities.entities = malloc(ENTITY_CAP);
	world->weapons.list =  malloc(WEAPON_CAP);

    world->walls.cap = TILE_CAP;
    world->floors.cap = TILE_CAP;
    world->doors.cap = TILE_CAP;
    world->health_buffs.cap = TILE_CAP;
    world->damage_buffs.cap = TILE_CAP;
    world->interactables.cap = TILE_CAP;
	world->entities.cap = ENTITY_CAP;
	world->weapons.cap = WEAPON_CAP;

    loadWorld(world, WORLD_PATH);
	
	world->spawn = getSpawn();

	player->name = "player";
	player->move = false;
	player->alive = true;
	player->adjsp = false;
	player->speed = 5;
	player->health = 100;
	player->frame_count = 0;
	player->angle = 0.00;
	player->anim_speed = ANIMATION_SPEED;
	player->pos = (Vector2){world->spawn.x, world->spawn.y};
	player->tx = addTexture(&world->textures, PLAYER_PATH);
	
	player->weapon.size = 0;
	player->weapon.list = malloc(WEAPON_CAP);
	player->weapon.cap = WEAPON_CAP;

	// starting weapon
	Weapon wp = SHURIKEN;
	wp.texture = addTexture(&world->textures, SHURIKEN_PATH);

	Weapon wp2 = WHIP;
	wp2.add = true;

	Weapon wp3 = AOE;
	wp3.add = true;

	Weapon wp4 = FIREBALL;
	wp4.add = true;

	Weapon wp5 = MAGIC_MISSLE;
	wp5.add = true;

	// starting weapn row add flag should be true
    memset(add_weapon, false, sizeof(add_weapon));
    memset(add_weapon[0], true, sizeof(add_weapon[0]));

	addWeapon(&player->weapon, &wp, player->pos);
	addWeapon(&player->weapon, &wp2, player->pos);
	addWeapon(&player->weapon, &wp3, player->pos);
	addWeapon(&player->weapon, &wp4, player->pos);
	addWeapon(&player->weapon, &wp5, player->pos);

	camera->zoom = 1.00f;
	camera->target = player->pos;
	camera->offset = (Vector2){GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f};
}

void deinit(World* world, Entity* player)
{
	free(world->walls.list);
	free(world->floors.list);
	free(world->doors.list);
	free(world->health_buffs.list);
	free(world->damage_buffs.list);
	free(world->interactables.list);
	free(world->entities.entities);
	free(world->weapons.list);
	free(player->weapon.list);

	for(int i = 0; i < world->textures.size; i++) UnloadTexture(world->textures.better_textures[i].tx);

	CloseWindow();
}

// returns the index of the tile from some layer that a projectile collides with
int projectileCollisionWorld(Weapon* wp, TileList* layer)
{
	for(int i = 0; i < layer->size; i++)
	{
		Tile* tile = &layer->list[i];

		// only interested in tiles on the screen
		if(!tile->on_screen) continue;
		
		Rectangle wp_area  = (Rectangle){wp->pos.x, wp->pos.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE};
		Rectangle en_area = (Rectangle){tile->sp.x, tile->sp.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE};

		if(CheckCollisionRecs(wp_area,en_area) && tile->active) return i;
	}

	return -1;
}

// returns the index of the enity that some projectile collides with
int projectileCollisionEntity(Weapon* wp, Entities* world_entities)
{
	for(int i = 0; i < world_entities->size; i++)
	{
		Entity* en = &world_entities->entities[i];
		
		// only interested in entities that we can see
		if(!en->on_screen) continue;

		Rectangle wp_area = (Rectangle){wp->pos.x, wp->pos.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE};
		Rectangle en_area = (Rectangle){en->pos.x, en->pos.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE};

		if(CheckCollisionRecs(wp_area, en_area)) return i;
	}

	return -1;
}

// returns index of the tile from some layer that an entity collides with
int entityCollisionWorld(Entity* en, TileList* layer)
{
	for(int i = 0; i < layer->size; i++)
	{
		Tile* tile = &layer->list[i];

		Rectangle en_area = (Rectangle){en->pos.x, en->pos.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE};
		Rectangle tile_area = (Rectangle){tile->sp.x, tile->sp.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE};

		if(CheckCollisionRecs(en_area, tile_area) && tile->active) return i;
	}

	return -1;
}

// returns index of the entity that some entity collides with
int entityCollisionEntity(Entity* entity, Entities* world_entities)
{
	for(int i = 0; i < world_entities->size; i++)
	{
		Entity* en = &world_entities->entities[i];
		
		// only care about alive entities
		if(!en->alive) continue;
		
		Vector2 curr_en_pos = (Vector2){entity->pos.x + TILE_SIZE, entity->pos.y + TILE_SIZE};
		Rectangle en_area = (Rectangle){en->pos.x, en->pos.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE};

		if(CheckCollisionPointRec(curr_en_pos, en_area) && (en->id != entity->id)) return i;
	}

	return -1;
}

// enemy movement, animation, and damage dealing to the player
void updateEntities(Entities* world_entities, Entity* player, World* world)
{
	int wall_col, en_col = -1;

	for(int i = 0; i < world_entities->size; i++)
	{
		Entity* en = &world_entities->entities[i];
		
		// only interested in alive entities
		if(!en->alive) continue;

		// animation
		en->frame_count++;
		en->xfp = (en->frame_count / en->anim_speed);

		// resetting frames
		if(en->xfp > 3) en->frame_count = en->xfp = 0;

		// finding angular speed
		en->angle = getAngle(player->pos, en->pos);
		en->dx = cosf(en->angle * DEG2RAD) * en->speed;
		en->dy = sinf(en->angle * DEG2RAD) * en->speed;

		// x movement
		en->pos.x += en->dx;
		wall_col = entityCollisionWorld(en, &world->walls);
		en_col = entityCollisionEntity(en, &world->entities);
		if(wall_col >= 0 || en_col >= 0) en->pos.x -= en->dx;

		// movement
		en->pos.y += en->dy;
		wall_col = entityCollisionWorld(en, &world->walls);
		en_col = entityCollisionEntity(en, &world->entities);
		if(wall_col >= 0 || en_col >= 0) en->pos.y -= en->dy;

		// doing damage to the player
		Vector2 ppos = (Vector2){player->pos.x + TILE_SIZE, player->pos.y + TILE_SIZE};
		Rectangle en_area = (Rectangle){en->pos.x, en->pos.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE};
		
		if(CheckCollisionPointRec(ppos, en_area))
		{
			// deal damage based on hit speed
			if(TimerDone(en->hit_timer))
			{		
				player->health -= en->damage;
				if(player->health <= 0) player->alive = false; 

				StartTimer(&en->hit_timer, 1.00);
			}
		}

		// only draw entity when in bounds
		if(CheckCollisionRecs(en_area, world->area))
		{
			en->on_screen = true;
			// DrawRectangleLines(en->pos.x, en->pos.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE, RED);
			
			Rectangle src_area = (Rectangle){en->xfp * TILE_SIZE, 0, TILE_SIZE, TILE_SIZE};
			Rectangle dst_area = (Rectangle){en->pos.x, en->pos.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE};
			DrawTexturePro(en->tx, src_area, dst_area, (Vector2){0,0}, 0, WHITE);
		}

		else en->on_screen = false;
	}
}

// to animate and move player
void move(TileList* world_walls, Entity* player)
{
	// animation
	player->frame_count++;
	player->xfp = (player->frame_count / player->anim_speed);

	// resetting frames position after last image 
	if(player->xfp > 3) player->xfp = player->frame_count = 0;

	// flag for diagonal movement
    bool diagonal = (IsKeyDown(KEY_W) && IsKeyDown(KEY_D)) 
                	||(IsKeyDown(KEY_W) && IsKeyDown(KEY_A)) 
                	||(IsKeyDown(KEY_A) && IsKeyDown(KEY_S)) 
                	||(IsKeyDown(KEY_S) && IsKeyDown(KEY_D));

	// fix diag movement speed up using pythag thrm
    if (diagonal)
    {
        if (!player->adjsp)
        {
            player->speed = sqrt(8 * (pow(player->speed, 2))) / 4;
			player->adjsp = true;
        }
    } else {
        player->speed = 3;
		player->adjsp = false;
    }
	
	// to hold the index from the list of walls that we collide with
	int wall_col = -1;

	if(IsKeyDown(KEY_W))
	{
		player->pos.y -= player->speed;
		player->yfp = 1;
		player->move = true;

		// collision
		wall_col = entityCollisionWorld(player, world_walls);
		if(wall_col >= 0) player->pos.y += player->speed;
	}

	if(IsKeyDown(KEY_A))
	{
		player->pos.x -= player->speed;
		player->yfp = 3;
		player->move = true;

		// collision
		wall_col = entityCollisionWorld(player, world_walls);
		if(wall_col >= 0) player->pos.x += player->speed;
	}

	if(IsKeyDown(KEY_S))
	{
		player->pos.y += player->speed;
		player->yfp = 0;
		player->move = true;

		// collision
		wall_col = entityCollisionWorld(player, world_walls);
		if(wall_col >= 0) player->pos.y -= player->speed;
	}

	if(IsKeyDown(KEY_D))
	{
		player->pos.x += player->speed;
		player->yfp = 2;
		player->move = true;

		// collision
		wall_col = entityCollisionWorld(player, world_walls);
		if(wall_col >= 0) player->pos.x -= player->speed;
	}

	// afk
	if(!player->move) player->xfp = player->yfp = 0;

	// resetting movement flag
	player->move = false;
}

// draws health and level bar of the player
void displayPlayerStats(Entity* player)
{
	// health 
	DrawRectangle(GetScreenWidth() - 310, GetScreenHeight() - 60, (300) * (player->health / 100),50, RED);
	DrawRectangleLines(GetScreenWidth() - 310, GetScreenHeight() - 60, 300,50, BLACK);
	DrawText("HEALTH", GetScreenWidth() - 300, GetScreenHeight() - 80, 20, RED);
	
	// level
	DrawRectangle(10, GetScreenHeight() - 60, (300) * (player->exp / 100), 50, GREEN);
	DrawRectangleLines(10, GetScreenHeight() - 60, 300, 50, BLACK);

	char lvl_dsc[100]; sprintf(lvl_dsc, "LEVEL: %d", player->level);
	DrawText(lvl_dsc, 10, GetScreenHeight() - 80, 20, GREEN);
}

// mechanics to heal
void heal(TileList* world_heals, Entity* player)
{
	int h_col =  entityCollisionWorld(player, world_heals);
	if(h_col >= 0)
	{
		DrawText("PRESSING H WILL HEAL", GetScreenWidth() - 310, GetScreenHeight() - 110, 20, GREEN);
		if(IsKeyPressed(KEY_H) && player->health < 100)
		{
			player->health += 20;
			if(player->health > 100) player->health = 100;
			
			// disable the used health pot
			world_heals->list[h_col].active = false;
		}
	}
}

// awarding xp and removing killed entity from map
void killEntity(Entity* killed_en, Entity* player)
{
	player->exp += 20;

	if(player->exp >= 100)
	{
		player->exp = 0;
		player->level++;
	}

	*killed_en = NULL_ENTITY;
}

// 	to use the whip weapon
void whip(Weapon* whip, Entities* world_entities, Entity* player)
{	
	if(TimerDone(whip->duration_timer))
	{
		// start cooldown
		whip->add = true;
		StartTimer(&whip->interval_timer, whip->interval_time);
	}

	// draw rectangle for now where whip is going to hit enemeies
	Vector2 start = whip->dir ? (Vector2){player->pos.x - 100, player->pos.y} : (Vector2){player->pos.x + 30, player->pos.y};
	Rectangle aoe = (Rectangle){start.x, start.y, 100, 20};
	DrawRectangleLinesEx(aoe, 3, WHITE);

	// dealing damage
	for(int i = 0; i < world_entities->size; i++)
	{
		Entity* en = &world_entities->entities[i];
		Rectangle en_area = (Rectangle){en->pos.x, en->pos.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE};
		if(!en->alive) continue;
		
		if(CheckCollisionRecs(aoe, en_area))
		{
			if(TimerDone(en->hit_timer))
			{
				en->health -= whip->damage;
				if(en->health <= 0) killEntity(en, player);
				StartTimer(&en->hit_timer, whip->hit_time);
			}
		}
	}
}

// to use the aoe weapon
void aoe(Weapon* aoe, Entities* world_entities, Entity* player)
{
	if(TimerDone(aoe->duration_timer))
	{
		aoe->add = true;
	 	StartTimer(&aoe->interval_timer, aoe->interval_time);
	}
	
	Rectangle aoe_area = {aoe->pos.x, aoe->pos.y, 200, 200};
	DrawRectangleLinesEx(aoe_area, 2, YELLOW);

	for(int i = 0; i < world_entities->size; i++)
	{
		Entity* en = &world_entities->entities[i];
		Rectangle en_area = {en->pos.x, en->pos.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE};
		if(!en->alive) continue;
		
		if(CheckCollisionRecs(aoe_area, en_area))
		{
			en->speed = .75;
			if(TimerDone(en->hit_timer))
			{
				en->health -= aoe->damage;
				if(en->health <= 0) killEntity(en, player);
				StartTimer(&en->hit_timer, aoe->hit_time);
			}
		}

		// restoring org speed
		else en->speed = 1.00;
	}
}

// returns the current position of an entity w a specfied id
Entity* entityInfo(int id, Entities* world_entities)
{
	for(int i = 0; i < world_entities->size; i++)
	{
		Entity* en = &world_entities->entities[i];
		if(id == en->id) return en; 
	}

	return &world_entities->entities[0];
}

int closestEntity(Vector2 pp, Entities* world_entities)
{
	// to hold the smallest distance between player and entity
	float d = 100000000;
	int id = -1;
	
	if(world_entities->size == 0) return id;

	for(int i = 0; i < world_entities->size; i++)
	{
		// only care about alive entities
		if(!world_entities->entities[i].alive) continue;

		Vector2 en_pos = world_entities->entities[i].pos;
		int en_id = world_entities->entities[i].id;

		if(d > Vector2Distance(pp, en_pos))
		{
			d = Vector2Distance(pp, en_pos);
			id = en_id; 
		}
	}

	return id;
}

// moves shot projectiles and deal damage with collided entites
void updateProjectiles(Weapons* world_weapons, TileList* world_walls, Entities* world_entities, Entity* player, Rectangle screen)
{
	for(int i = 0; i < world_weapons->size; i++)
	{
		Weapon* wp = &world_weapons->list[i];

		// only onscreen projectiles relavent, offscreen projectiles will be removed
		if(!CheckCollisionPointRec(wp->pos, screen)) *wp = NULL_WEAPON;
		if(!wp->on_screen) continue;
		
		// moving projectile
		if(strcmp(wp->name, "missle") == 0)
		{
			if(wp->target_id == -1) wp->target_id = closestEntity(player->pos, world_entities); 
			
			Entity* target_en = entityInfo(wp->target_id, world_entities);

			// switching targets incase entity died before missle activation
			while(!target_en->alive) 
			{
				wp->target_id = closestEntity(player->pos, world_entities);
				target_en = entityInfo(wp->target_id, world_entities);
			}

			// directly targets that specific entity
			Vector2 target_pos = target_en->pos;
			if(target_pos.x > wp->pos.x) wp->pos.x += wp->speed;
			if(target_pos.x < wp->pos.x) wp->pos.x -= wp->speed;
			if(target_pos.y > wp->pos.y) wp->pos.y += wp->speed;
			if(target_pos.y < wp->pos.y) wp->pos.y -= wp->speed;
		}
		
		else wp->pos = Vector2Add(wp->pos, (Vector2){wp->dx, wp->dy});

		// animation
		wp->fc++;
		int frame_pos = (wp->fc / wp->anim_speed);
		if(frame_pos > wp->frames) wp->fc = frame_pos = 0;

		// drawing weapon on-screen
		Rectangle src_area = (Rectangle){frame_pos * 25, 36, 25, 25};
		Rectangle dst_area = (Rectangle){wp->pos.x, wp->pos.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE};

		if(strcmp(wp->name, "shuriken") == 0) DrawTexturePro(wp->texture, src_area, dst_area, (Vector2){0,0}, 0, WHITE);
		else if(strcmp(wp->name, "fireball") == 0) DrawRectangle(wp->pos.x, wp->pos.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE, BLUE);
		else if(strcmp(wp->name, "missle") == 0 ) DrawRectangleLines(wp->pos.x, wp->pos.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE, PURPLE);
		
		// wall collision
		int wall_col = projectileCollisionWorld(wp, world_walls);
		if(wall_col >= 0) *wp = NULL_WEAPON;

		// hitting an entity
		int en_col = projectileCollisionEntity(wp, world_entities);
		if(en_col >= 0)
		{
			// doing damage
			Entity* hit_en = &world_entities->entities[en_col];
			hit_en->health -= wp->damage;
			*wp = NULL_WEAPON;

			// killing enemy
			if(hit_en->health <= 0) killEntity(hit_en, player);
		}
	}
}

// returns the id of the closest, alive entity

void updatePlayer(TileList* world_walls, Weapons* world_weapons, Entities* world_entities, Entity* player, Camera2D* camera)
{
	
	// using weapons
	for(int i = 0; i < player->weapon.size; i++)
	{
		Weapon* wp = &player->weapon.list[i];
		
		// whip use
		if(strcmp(wp->name, "whip") == 0)
		{
			if(TimerDone(wp->interval_timer))
			{
				// use flag so we dont start timer over and over again in the window that the interval timer is done
				if(wp->add)
				{
					// choose side, left(0) or right(1) of the player to use the whip
					wp->dir = GetRandomValue(0, 1);
					
					StartTimer(&wp->duration_timer, wp->duration_time);
					wp->add = false;
				} 
				
				whip(wp, world_entities, player);
			}
		}

		// aoe use
		else if(strcmp(wp->name, "aoe") == 0)
		{
			if(TimerDone(wp->interval_timer))
			{
				if(wp->add)
				{
					// subtract by aoe width so its always in the screen
					float cox = camera->offset.x - 200;
					float coy = camera->offset.y - 200;
					float ctx = camera->target.x;
					float cty = camera->target.y;

					wp->pos = (Vector2){GetRandomValue(ctx - cox, ctx + cox),
									GetRandomValue(cty - coy, cty + coy)};

					StartTimer(&wp->duration_timer, wp->duration_time);
					wp->add = false;
				}

				aoe(wp, world_entities, player);
			}
		}

		// shoots 3 projectiles as a burst
		else if(strcmp(wp->name, "fireball") == 0)
		{
			if(TimerDone(wp->interval_timer))
			{
				wp->pos = player->pos;
				wp->angle = getAngle(mp, player->pos);
				
				Weapon fb1 = *wp;
				Weapon fb2 = *wp;
				Weapon fb3 = *wp;

				fb1.angle = wp->angle - 15;
				fb2.angle = wp->angle;
				fb3.angle = wp->angle + 15;

				addWeapon(world_weapons, &fb1, player->pos);
				addWeapon(world_weapons, &fb2, player->pos);
				addWeapon(world_weapons, &fb3, player->pos);

				StartTimer(&wp->interval_timer, wp->interval_time);
			}
		}

		// throwing starts that you can aim
		else
		{
			for(int j = 0; j < wp->count; j++)
			{
				// update flag and add weapon to the screen
				if(TimerDone(weapon_activation_times[i][j]) && add_weapon[i][j] && world_entities->size != 0)
				{
					wp->target_id = -1; 
					wp->pos = player->pos;
					add_weapon[i][j] = false;
					wp->angle = getAngle(mp, player->pos);
					addWeapon(world_weapons, wp, player->pos);
				}
			}

			// start timers of all based on last weapon timer
			if(TimerDone(weapon_activation_times[i][wp->count - 1]))
			{
				for(int k = 0; k < wp->count; k++)
				{
					StartTimer(&weapon_activation_times[i][k], wp->interval_time + (k / 10.00));
					add_weapon[i][k] = true;
				}
			}
		}
	}

	// movement and animation
	move(world_walls, player);

	// drawing sprite
	Rectangle src_area = (Rectangle){player->xfp * TILE_SIZE, player->yfp * TILE_SIZE, TILE_SIZE, TILE_SIZE};
	Rectangle dst_area = (Rectangle){player->pos.x, player->pos.y, SCREEN_TILE_SIZE, SCREEN_TILE_SIZE};
	DrawTexturePro(player->tx, src_area, dst_area, (Vector2){0,0}, 0, WHITE);
}

// spawns entity outside view of the player whose rate over time
void createEnemy(World* world, Camera2D* camera)
{
	if(TimerDone(world->spawn_timer))
	{
		// basic mob properties
		Entity new_en;
		new_en.health = 100;
		new_en.speed = 1.00;
		new_en.alive = true;
		new_en.anim_speed = ANIMATION_SPEED;
		new_en.damage = 20.0;
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

		// continue to update position until we get an entity that has no collisions with any walls or any other entity
		do
		{
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

		} while((entityCollisionEntity(&new_en, &world->entities) >= 0) || (entityCollisionWorld(&new_en, &world->walls) >= 0));	

		// add entity to world and reset timer
		addEntity(&world->entities, new_en);
		StartTimer(&world->spawn_timer, world->spawn_time);
	}

	// increase spawn rate after some duration of time
	if(TimerDone(world->level_timer))
	{
		world->spawn_time -= .05;
		StartTimer(&world->level_timer, world->level_time);
	}
}

// returns true if player quits game
bool deathWindow(World* world, Entity* player)
{
	DrawText(DEATH_PROMPT, (GetScreenWidth() / 2.0f) - (MeasureText(DEATH_PROMPT, 30) / 2.0f), GetScreenHeight() / 2.0f, 30, RED);

	// respawn button
	if(GuiButton((Rectangle){(GetScreenWidth() / 2.0f) - 125, (GetScreenHeight() / 2.0f) + 50, 100, 50}, "RESPAWN"))
	{
		clearEntities(&world->entities);
		clearWeapons(&world->weapons);

		player->pos = world->spawn;
		player->alive = true;
		player->health = 100;
		player->level = 0;
		
		world->spawn_time = 1.00;
		StartTimer(&world->spawn_timer, world->spawn_time);

		world->level_time = 30.00;
		StartTimer(&world->level_timer, world->level_time);
	}

	// quit button
	if(GuiButton((Rectangle){(GetScreenWidth() / 2.0f) + 50, (GetScreenHeight() / 2.0f) + 50, 100, 50}, "QUIT")) return true;

	else return false;
}

int main()
{
    World world;
	Entity player;
	Camera2D camera;
	init(&world, &player, &camera);

	while(!WindowShouldClose())
    {
		mp = GetScreenToWorld2D(GetMousePosition(), camera);
		camera.target = player.pos;
		world.area = (Rectangle){camera.target.x - camera.offset.x, camera.target.y - camera.offset.y, SCREEN_WIDTH, SCREEN_HEIGHT};

		createEnemy(&world, &camera);
	    
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
					updatePlayer(&world.walls, &world.weapons, &world.entities, &player, &camera);
					updateProjectiles(&world.weapons, &world.walls, &world.entities, &player, world.area);
				EndMode2D();

				displayPlayerStats(&player);
				heal(&world.health_buffs, &player);
			}

			// death screen
			else 
			{
				ClearBackground(GRAY);
				if(deathWindow(&world, &player)) break;
			}

			DrawFPS(0, 0);
		EndDrawing();
    }

    deinit(&world, &player);
    return 0;
}