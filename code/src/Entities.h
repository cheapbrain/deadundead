#pragma once

#define TYPES_NUMBER 5

//matrice per decidere le collisioni, basata sui tipi di entità
const int collisions[TYPES_NUMBER*TYPES_NUMBER] = {
	0,1,0,0,1,
	1,0,0,1,1,
	0,0,0,0,0,
	0,1,0,0,1,
	1,1,0,1,0
};

//lista delle interazioni delle entità non raccoglibili
vvv steady_interactions[2] = {damage, boost};

enum Type {
	PLATFORM,
	PLAYER,
	PICKUP,
	STEADY,
	PROJECTILE
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
struct Platform_struct {
	Type type;
	float x, y;
	vvf update;
};
typedef Platform_struct* Platform;
Platform new_platform();
void update_platform(const void*, float);

struct Projectile_struct {
	Type type;
	float x, y;
	vvf update;
	float speed_x, speed_y;
	vv move;
	ivv colliding;
};
typedef Projectile_struct* Projectile;
Projectile new_projectile();
void update_projectile(const void*, float);

struct Interactable_struct {
	Type type;
	float x, y;
	vvf update;
	int id;
	vvv interact;
};
typedef Interactable_struct* Interactable;
Interactable new_interactable(Type,int);

struct Player_struct {
	Type type;
	float x, y;
	vvf update;
	float speed_x, speed_y;
	vv move;
	ivv colliding;
	char *name;
	int held;
	float health;
	vvv interact;
};
typedef Player_struct* Player;
Player new_player(char*);
void update_player(const void*, float);
void interact_player(const void*, const void*);
