#include <stdlib.h>
#include <malloc.h>
#include "entities.h"

//
//COMMON
//
Type get_type(const void *super) {
	return *(Type*)super;
}
vvv get_interaction(Interactable i) {
	switch (i->type) {
	case PICKUP: return pickup; break;
	case STEADY: return steady_interactions[i->id];	break;
	}
}
void move(const void* entity) {
	Projectile e = (Projectile)entity;
	e->x += e->speed_x;
	e->y += e->speed_y;
}
int colliding(const void* entity_1, const void* entity_2) {
	Type t1=get_type(entity_1), t2=get_type(entity_2);
	if (collisions[t1*TYPES_NUMBER + t2] == 1)
		return 0; //TO-DO collisione
	else 
		return 0;
}

//
//PLATFORM
//
Platform new_platform() {
	Platform p = (Platform)malloc(sizeof(Platform_struct));
	p->type = PLATFORM;
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
	p->type = PROJECTILE;
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
Interactable new_interactable(Type t, int id) {
	if (id>=0) {
		Interactable i = (Interactable)malloc(sizeof(Interactable_struct));
		i->type = t;
		i->id = 0;
		i->x = 0;
		i->y = 0;
		//TO-DO
		//i->update;
		i->interact = get_interaction(i);
		return i;
	}
	else return NULL;
}

//
//PLAYER
//
Player new_player(char* name) {
	Player p = (Player)malloc(sizeof(Player_struct));
	p->type = PLAYER;
	p->x = 0;
	p->y = 0;
	p->speed_x = 0;
	p->speed_y = 0;
	p->held = -1;
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
void interact_player(const void* self, const void* target) {
	Player player = (Player)self;
	Interactable interactable = (Interactable)target;
	interactable->interact(interactable, player);
}

//INTERAZIONI
void pickup(const void* entity_1, const void* entity_2) {
	Interactable item = (Interactable)entity_1;
	Player player = (Player)entity_2;
	int held = player->held;
	player->held = item->id;
	free(item);
	item = new_interactable(PICKUP, held);
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



#define PLAYERS_NUM 4
#define PLATFORMS_NUM 10
#define INTERACTABLES_NUM 3
void prova() {
	//inizializzazioni varie
	int i;
	float delta = 0; //ottenuto in altra maniera
	Platform platforms[PLATFORMS_NUM];
	Player players[PLAYERS_NUM];
	Interactable interactables[INTERACTABLES_NUM];
	for (i = 0; i < PLATFORMS_NUM; i++)
		platforms[i]=new_platform();
	for (i = 0; i < PLAYERS_NUM; i++)
		players[i] = new_player((char*)(&i));
	interactables[0] = new_interactable(PICKUP, 0);
	interactables[1] = new_interactable(STEADY, 0);
	interactables[2] = new_interactable(STEADY, 1);
	
	//questa dovrebbe raccogliere l'item
	players[0]->interact(players[0], interactables[0]);
	//questa dovrebbe fare danno al player
	players[1]->interact(players[1], interactables[1]);
	//questa dovrebbe lanciare il player in alto
	players[0]->interact(players[2], interactables[2]);

}