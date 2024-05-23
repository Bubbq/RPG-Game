#include <stddef.h>
#include <stdio.h>
#include <raylib.h>

enum Element
{
	WALL = 0,
	FLOOR = 1,
	DOOR = 2,
	HEALTH_BUFF = 3,
	DAMAGE_BUFF = 4,
	INTERACTABLE = 5,
	SPAWN = 6,
};
typedef struct
{
	double startTime;
	double lifeTime;
} Timer;

typedef struct
{
	const char* name;
	Vector2 pos;
	float damage;
	float angle;
	float speed;
	// how many of that weapon do we have
	int count;
	float interval_time;
	int frames;
	int anim_speed;
	float duration_time;
	float hit_time;
	// time in-between weapon activations 
	Timer interval_timer;
	Timer duration_timer;
	Timer hit_timer;
	Texture2D texture;
	bool on_screen;
	bool add;
	int fc;
	float dx, dy;
	int dir;
	int target_id;
} Weapon;

typedef struct
{
	Weapon* list;
	int size;
	size_t cap;
} Weapons;

typedef struct
{
	Vector2 src;
	Vector2 sp;
	Texture2D tx;
	enum Element tt;
	char fp[512];
	bool active;
	bool anim;
	int fc;
	int frames;
	int anim_speed;
	bool on_screen;
} Tile;

typedef struct
{
	char* name;
	int frame_count;
	Texture2D tx;
	float health;
	float speed;
	Vector2 pos;
	bool alive;
	bool on_screen;
	// flag for diagonal speed adjustment
	bool adjsp;
	bool move;
	Timer timer;
	Vector2 path;
	int xfp;
	int yfp;
	float exp;
	int level;
	int id;
	Timer hit_timer;
	float angle;
	float dx;
	float dy;
	int anim_speed;
	float damage;
	Weapons weapon;
} Entity;

typedef struct
{
	int size;
	size_t cap;
	Tile* list;
} TileList;

typedef struct
{
	Texture2D tx;
	char fp[512];
} BetterTexture;

typedef struct
{
	BetterTexture better_textures[25];
	int size;
} Textures;



typedef struct
{
	Entity* entities;
	int size;
	size_t cap;
} Entities;

typedef struct
{
	Weapons weapons;
	Entities entities;
	TileList walls;
	TileList floors;
	TileList doors;
	TileList health_buffs;
	TileList damage_buffs;
	TileList interactables;
	Textures textures;
	Vector2 spawn;
	Rectangle area;
	Timer spawn_timer;
	float spawn_time;
	Timer level_timer;
	float level_time;
} World;

const size_t TILE_CAP = (25 * sizeof(Tile));
const size_t ENTITY_CAP = (3 * sizeof(Entity));
const size_t WEAPON_CAP = (25 * sizeof(Weapon));
const int TEXTURE_CAP = 25;
const int TILE_SIZE = 16;
const float SCALE = 2.0f;
const int SCREEN_TILE_SIZE = TILE_SIZE * SCALE;


