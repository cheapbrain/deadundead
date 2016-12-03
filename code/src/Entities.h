#pragma once

//
//TIPI FUNZIONI: 1a LETTERA TIPO RITORNO, POI TIPI INGRESSO
// v void, i int, c char, f float, d double 
//(puntatori omessi, v come argomento è sempre puntatore)
//
typedef void(*vv)(const void*);
typedef void(*vvf)(const void*, float);
typedef int(*ivv)(const void*, const void*);
typedef void(*vvv) (const void*, const void*);

//lista delle interazioni delle entità non raccoglibili
void damage(const void*, const void*);
void boost(const void*, const void*);
const vvv steady_interactions[2] = {&damage, &boost};

//lista usi e azioni player/strumenti
void hit(const void*);
void slash(const void*);
const vv actions[2] = {&hit, &slash};

enum Interactable_type {
	TRAP,
	BOUNCING
};

enum Pickup_type {
	NOTHING,
	WEAPON
};

//
//STRUTTURE ENTITA'
//
struct Entity {
	float x, y, width, height;
};

struct Platform {
	float x, y, width, height;
	vvf update;
};
Platform* new_platform();
void update_platform(const void*, float);

struct Projectile {
	float x, y, width, height;
	vvf update;
	float speed_x, speed_y;
	vv move;
	ivv colliding;
};
Projectile* new_projectile();
void update_projectile(const void*, float);

struct Interactable {
	float x, y, width, height;
	vvf update;
	Interactable_type type;
	vvv interact;
};
Interactable* new_interactable(Interactable_type);

struct Pickup {
	float x, y, width, height;
	vvf update;
	Pickup_type type;
	vvv pickupped;
	vvv use;
};
Pickup* new_pickup(Pickup_type);

struct Player {
	float x, y, width, height;
	vvf update;
	float speed_x, speed_y;
	vv move;
	ivv colliding;
	char *name;
	Pickup_type held;
	vv action;
	float health;
};
Player* new_player(char*);
void update_player(const void*, float);
