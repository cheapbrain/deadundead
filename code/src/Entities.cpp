#include <stdlib.h>
#include <malloc.h>
#include "entities.h"

//
//COMMON
//
vvv get_interaction(Interactable i) {
	return steady_interactions[i->type];
}
void move(const void* entity) {
	Projectile e = (Projectile)entity;
	e->x += e->speed_x;
	e->y += e->speed_y;
}
int colliding(const void* entity_1, const void* entity_2) {
	return 0;
}

//INTERAZIONI
void pickupped(const void* entity_1, const void* entity_2) {
	Pickup item = (Pickup)entity_1;
	Player player = (Player)entity_2;
	Pickup_type held = player->held;
	player->held = item->type;
	player->action = actions[held];
	free(item);
	item = new_pickup(held);
}
void boost(const void* entity_1, const void* entity_2) {
	Interactable item = (Interactable)entity_1;
	Player player = (Player)entity_2;
	player->speed_y = -10.f;
}
void damage(const void* entity_1, const void* entity_2) {
	Interactable item = (Interactable)entity_1;
	Player player = (Player)entity_2;
	player->health -= 10.f;
}

//AZIONI
void hit(const void* entity_1) {
	Player player = (Player)entity_1;
}
void slash(const void* entity_1) {
	Player player = (Player)entity_1;
}


//
//PLATFORM
//
Platform new_platform() {
	Platform p = (Platform)malloc(sizeof(Platform_struct));
	p->x = 0;
	p->y = 0;
	p->update = &update_platform;
	return p;
}
void update_platform(const void* self, float delta) {
	Platform ego = (Platform)self;
}

//
//PROJECTILE
//
Projectile new_projectile() {
	Projectile p = (Projectile)malloc(sizeof(Projectile_struct));
	p->update = &update_projectile;
	p->colliding = &colliding;
	p->move = &move;
	return p;
}
void update_projectile(const void* self, float delta) {
	Projectile ego = (Projectile)self;
}

//
//INTERACTABLE
//
Interactable new_interactable(Interactable_type type, Platform p) {
	if (type>=0) {
		Interactable i = (Interactable)malloc(sizeof(Interactable_struct));
		i->x = p->x;
		i->y = p->y;
		i->type = type;
		//TO-DO
		//i->update;
		i->interact = get_interaction(i);
		return i;
	}
	else return NULL;
}

//
//PICKUP
//
Pickup new_pickup(Pickup_type type) {
	if (type) {
		Pickup p = (Pickup)malloc(sizeof(Pickup_struct));
		p->x = 0;
		p->y = 0;
		p->type = type;
		//TO-DO
		//i->update;
		p->pickupped = &pickupped;
		return p;
	}
	else return NULL;
}

//
//PLAYER
//
Player new_player(char* name) {
	Player p = (Player)malloc(sizeof(Player_struct));
	p->x = 0;
	p->y = 0;
	p->speed_x = 0;
	p->speed_y = 0;
	p->held = NOTHING;
	p->action = &hit; // = actions[NOTHING];
	p->health = 100.f;
	p->name = name;
	p->update = &update_player;
	p->colliding = &colliding;
	p->move = &move;
	return p;
}
void update_player(const void* self, float delta) {
	Player ego = (Player)self;
}



void prova() {
	//inizializzazioni varie
	int i;
	float delta = 0; //ottenuto in altra maniera
	Platform platforms[10+2];
	Player players[4];
	Pickup pickups[1];
	Interactable interactables[2];
	for (i = 2; i < 10+2; i++)
		platforms[i]=new_platform();
	for (i = 0; i < 4; i++)
		players[i] = new_player((char*)(&i));

	pickups[0] = new_pickup(WEAPON);
	interactables[0] = new_interactable(TRAP, platforms[0]);
	interactables[1] = new_interactable(BOUNCING, platforms[1]);
	
	//questa dovrebbe raccogliere l'item
	pickups[0]->pickupped(pickups[0], players[0]);
	//questa dovrebbe fare danno al player
	interactables[0]->interact(interactables[0], players[0]);
	//questa dovrebbe lanciare il player in alto
	interactables[1]->interact(interactables[1], players[1]);
}