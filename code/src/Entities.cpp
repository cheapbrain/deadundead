#include <stdlib.h>
#include <malloc.h>
#include "entities.h"

//
//COMMON
//
Type get_type(const void *e) {
	return *(Type*)e;
}
int is_in(Type t, Type_list list) {
	for (int i = 0; i < list->size; i++)
		if (list->list[i] == t) return 1;
	return 0;
}
int colliding(const void* entity_1, const void* entity_2) {
	Entity e1 = (Entity)entity_1;
	Entity e2 = (Entity)entity_2;
	if (collisions[e1->type*ENTITY_NUMBER + e2->type] == 1) { //devono collidere
		//TO-DO collisione
		return 0;
	}
	else return 0;
}

//
//TYPE_LIST
//
Type_list new_type_list(int size) {
	Type_list l = (Type_list)malloc(sizeof(Type_list_struct));
	l->size = size;
	l->list = (Type*)malloc(sizeof(Type)*size);
	return l;
}

//
//ENTITY
//
Entity new_entity() {
	Entity e = (Entity)malloc(sizeof(Entity_struct));
	e->id = 0;
	return e;
}

//
//PLATFORM
//
Platform new_platform() {
	Platform p = (Platform)malloc(sizeof(Platform_struct));
	p->e = new_entity();
	p->e->type = PLATFORM;
	p->update = &update_platform;
	return p;
}
void update_platform(const void* self) {
	Platform ego = (Platform)self;
}

//
//PLAYER
//
Player new_player() {
	Player p = (Player)malloc(sizeof(Player_struct));
	p->e = new_entity();
	p->e->type = PLAYER;
	p->update = &update_player;
	p->collides = &colliding;
	return p;
}
void update_player(const void* self) {
	Player ego = (Player)self;
}

//
//WEAPON
//
Weapon new_weapon() {
	Weapon w = (Weapon)malloc(sizeof(Weapon_struct));
	w->e = new_entity();
	w->e->type = WEAPON;
	w->update = &update_weapon;
	return w;
}
void update_weapon(const void* self) {
	Weapon ego = (Weapon)self;
}

//
//ACTIVE
//
Active new_active() {
	Active w = (Active)malloc(sizeof(Active_struct));
	w->e = new_entity();
	w->e->type = ACTIVE;
	w->update = &update_active;
	return w;
}
void update_active(const void* self) {
	Active ego = (Active)self;
}

//
//PASSIVE
//
Passive new_passive() {
	Passive w = (Passive)malloc(sizeof(Passive_struct));
	w->e = new_entity();
	w->e->type = PASSIVE;
	w->update = &update_passive;
	return w;
}
void update_passive(const void* self) {
	Passive ego = (Passive)self;
}

//
//CONSUMABLE
//
Consumable new_consumable() {
	Consumable w = (Consumable)malloc(sizeof(Consumable_struct));
	w->e = new_entity();
	w->e->type = CONSUMABLE;
	w->update = &update_consumable;
	return w;
}
void update_consumable(const void* self) {
	Consumable ego = (Consumable)self;
}

//
//CONTROLLER
//
Controller new_controller() {
	Controller w = (Controller)malloc(sizeof(Controller_struct));
	w->e = new_entity();
	w->e->type = CONTROLLER;
	w->update = &update_controller;
	return w;
}
void update_controller(const void* self) {
	Controller ego = (Controller)self;
}

//
//PROJECTILE
//
Projectile new_projectile() {
	Projectile w = (Projectile)malloc(sizeof(Projectile_struct));
	w->e = new_entity();
	w->e->type = PROJECTILE;
	w->update = &update_projectile;
	return w;
}
void update_projectile(const void* self) {
	Projectile ego = (Projectile)self;
}