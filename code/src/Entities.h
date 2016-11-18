#pragma once

#define ENTITY_NUMBER 8

const int collisions[ENTITY_NUMBER*ENTITY_NUMBER] = {
	0,1,0,0,0,0,0,1,
	1,0,0,1,1,0,0,1,
	0,0,0,0,0,0,0,0,
	0,1,0,0,0,0,0,1,
	0,1,0,0,0,0,0,1,
	0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,
	1,1,0,1,1,0,0,0
};

enum Type {
	PLATFORM,
	PLAYER,
	WEAPON,
	ACTIVE,
	PASSIVE,
	CONSUMABLE,
	CONTROLLER,
	PROJECTILE
};

struct Type_list_struct {
	Type* list;
	int size;
};
typedef Type_list_struct* Type_list;
Type_list new_type_list(int);

//
//TIPI FUNZIONI: 1a LETTERA TIPO RITORNO, POI TIPI INGRESSO
// v void, i int, c char, f float, d double (puntatori omessi)
//
typedef void(*vv)(const void*);
typedef int(*ivv)(const void*, const void*);

struct Entity_struct {
	Type type;
	int id;
	float x, y;
};
typedef Entity_struct* Entity;
Entity new_entity();

struct Platform_struct {
	Entity e;
	vv update;
};
typedef Platform_struct* Platform;
Platform new_platform();
void update_platform(const void*);

struct Player_struct {
	Entity e;
	char *name;
	vv update;
	ivv collides;
};
typedef Player_struct* Player;
Player new_player();
void update_player(const void*);

struct Weapon_struct {
	Entity e;
	vv update;
};
typedef Weapon_struct* Weapon;
Weapon new_weapon();
void update_weapon(const void*);

struct Active_struct {
	Entity e;
	vv update;
};
typedef Active_struct* Active;
Active new_active();
void update_active(const void*);

struct Passive_struct {
	Entity e;
	vv update;
};
typedef Passive_struct* Passive;
Passive new_passive();
void update_passive(const void*);

struct Consumable_struct {
	Entity e;
	vv update;
};
typedef Consumable_struct* Consumable;
Consumable new_consumable();
void update_consumable(const void*);

struct Controller_struct {
	Entity e;
	vv update;
};
typedef Controller_struct* Controller;
Controller new_controller();
void update_controller(const void*);

struct Projectile_struct {
	Entity e;
	vv update;
};
typedef Projectile_struct* Projectile;
Projectile new_projectile();
void update_projectile(const void*);