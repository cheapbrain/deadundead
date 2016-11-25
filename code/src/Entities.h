#pragma once

//lista delle interazioni delle entità non raccoglibili
vvv steady_interactions[2] = {damage, boost};

enum Interactable_type {
	TRAP,
	BOUNCING
};

enum Pickup_type {
	NOTHING,
	WEAPON
};

//
//TIPI FUNZIONI: 1a LETTERA TIPO RITORNO, POI TIPI INGRESSO
// v void, i int, c char, f float, d double 
//(puntatori omessi, v come argomento è sempre puntatore)
//
typedef void(*vv)(const void*);
typedef void(*vvf)(const void*, float);
typedef int(*ivv)(const void*, const void*);
typedef void(*vvv) (const void*, const void*);


//
//STRUTTURE ENTITA'
//
struct Entity {
	float x, y, width, height;
};

struct Platform_struct {
	float x, y, width, height;
	vvf update;
};
typedef Platform_struct* Platform;
Platform new_platform();
void update_platform(const void*, float);

struct Projectile_struct {
	float x, y, width, height;
	vvf update;
	float speed_x, speed_y;
	vv move;
	ivv colliding;
};
typedef Projectile_struct* Projectile;
Projectile new_projectile();
void update_projectile(const void*, float);

struct Interactable_struct {
	float x, y, width, height;
	vvf update;
	Interactable_type type;
	vvv interact;
};
typedef Interactable_struct* Interactable;
Interactable new_interactable(Interactable_type);

struct Pickup_struct {
	float x, y, width, height;
	vvf update;
	Pickup_type type;
	vvv pickupped;
};
typedef Pickup_struct* Pickup;
Pickup new_pickup(Pickup_type);

struct Player_struct {
	float x, y, width, height;
	vvf update;
	float speed_x, speed_y;
	vv move;
	ivv colliding;
	char *name;
	Pickup_type held;
	float health;
};
typedef Player_struct* Player;
Player new_player(char*);
void update_player(const void*, float);
