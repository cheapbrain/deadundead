#pragma once

#include "utils.h"

typedef struct World World;
typedef struct Entity Entity;

enum EntityTag
{
	UPDATE = 1<<0,
	RENDER = 1<<1,
	RENDER_BACK = 1<<2,
	RENDER_FRONT = 1<<3,
	DYNAMIC_COLLIDE = 1<<4,
	STATIC_COLLIDE = 1<<5,
	HITTABLE = 1<<6,
	ACTIVE_EVENT = 1<<7,
	PASSIVE_EVENT = 1<<8
};

enum EntityListType
{
	UPDATE_LIST = 0,
	RENDER_LIST,
	RENDER_BACK_LIST,
	RENDER_FRONT_LIST,
	DYNAMIC_COLLIDE_LIST,
	STATIC_COLLIDE_LIST,
	HITTABLE_LIST,
	ACTIVE_EVENT_LIST,
	PASSIVE_EVENT_LIST,
	_LIST_COUNT
};

enum PlayerStatus {
	MOVING,
	STUNNED
};

const EntityTag list_tags[_LIST_COUNT] = {
	UPDATE,
	RENDER,
	RENDER_BACK,
	RENDER_FRONT,
	DYNAMIC_COLLIDE,	//collidono solo con static
	STATIC_COLLIDE,		//collidono solo con dynamic
	HITTABLE,			//con salute
	ACTIVE_EVENT,		//interagibili
	PASSIVE_EVENT		//si attivano alla collisione
};

typedef void (*update_func)(Entity *, World *world, double delta);
typedef void (*render_func)(Entity *);

struct Entity {
	int id;
	int tag;
	float x, y, width, height; //per la hitbox
	float old_x, old_y;
	float speed_x, speed_y;
	float health;
	Texture *texture;
	int is_on_floor;
	int player_id;
	float bounce_coeff;

	PlayerStatus status;
	double score;				//punteggio del giocatore (quanto tempo ha tenuto il token)
	float skill_bar;			//quanto è carica la barra dell'abilità
	float timer;				//timer generico (per attacchi, stato,...)
	int type_in_hand;			//tipo di oggetto in mano (fendente, lanciabile, sparabile,...)
	int id_in_hand;				//quale oggetto di quel tipo ho in mano
	SpriterInstance *animation; //per settare quale animazione fare

	Entity *thrower;			//utilizzato per proiettili, in modo che quando vengono creati non collidano con chi li ha creati

	update_func update;
	render_func render;
	void (*on_collide)(Entity *source, Entity *target);
	void (*on_hit)(Entity *source, Entity *target);
	void (*on_enter)(Entity *source, Entity *target);
	void (*on_interact)(Entity *source, Entity *target);

	int indexes[_LIST_COUNT];
};

struct EntityNode {
	char *name;
	Entity *entity;
	EntityNode *next;
};

struct EntityArray {
	Entity *entities;
	char **names;
	int count;
	int capacity;

	EntityNode **table;
	int mask;
};

struct EditorEntity {
	Entity *original;
	char *name;
	float x, y;
};

struct EditorWorld {
	EditorEntity *entities;
	int count;
	int capacity;
};

struct EntityList {
	int id;
	int count;
	int capacity;
	int *entity_id;
};

struct World {
	Vec2 size;
	Entity *entities;
	int entity_count;
	int entity_capacity;
	
	EntityList lists[_LIST_COUNT];
};

void world_init(World *world, int capacity);

void world_render(World *world, double delta);

void world_update(World *world, double delta);

Entity *world_new_entity(World *world, int tag);

Entity *world_new_entity(World *world, Entity *entity);

void world_remove_entity(World *world, int id);

Entity *world_get_entity(World *world, int id);

void world_resize_entity_list(World *world, int new_capacity);

void entity_remove_list(World *world, Entity *entity, EntityListType list_id);

void entity_add_list(World *world, Entity *entity, EntityListType list_id);


void player_update(Entity *e, World *world, double delta);

void player_render(Entity *e);

void wall_render(Entity *e);

enum RenderFunctions {
	RENDER_PLAYER = 0,
	RENDER_WALL,
	_RENDER_FUNCTION_COUNT
};

enum UpdateFunctions {
	UPDATE_PLAYER = 0,
	_UPDATE_FUNCTION_COUNT
};

extern const render_func render_functions[_RENDER_FUNCTION_COUNT];

extern const char *render_func_names[_RENDER_FUNCTION_COUNT];

extern const update_func update_functions[_UPDATE_FUNCTION_COUNT];

extern const char *update_func_names[_UPDATE_FUNCTION_COUNT];