#pragma once

#include "Entities.h"

/********************************STRUTTURE*PER*INFO*ARMI*****************************************/
enum TypeInHand {
	MELEE,		//NOTA: tipo melee id 0 è un pugno (senza niente in mano)
	PROJECTILE,	//proiettile singolo
	SHOOTER,
	OTHER
};

struct OtherData {
	void (*on_update) (Entity *holder);
	void (*on_attack) (Entity *holder, World *world);
	void (*on_interact)(Entity *source, Entity *target);
	char *texture;
};

struct WeaponDataMelee {	//quando usata danneggia tutti gli hittable nell'area
	Rectangle hitbox; //la posizione del rettangolo rappresenta lo scostamento rispetto all'angolo basso-sx del player che attacca
	float damage;
	char *texture;
	double attack_timer;	//dopo quanto tempo posso attaccare di nuovo
};

struct WeaponDataProjectile {//un'arma da lancio (o proiettile di shooter)
	Vec2 size;
	int angle;		//l'angolo di tiro in gradi; se 0 è un tiro dritto non affetto dalla gravità
	float speed;
	char *texture;
	double timer;	//se 0 hit_func viene chiamata alla collisione con static o dinamic, altrimenti quando scade il timer (e rimbalza)
	void (*hit_func) (Entity *self, Entity *first_hit, World *world);	//nota: se è a timer first_hit sarà sempre NULL
};

struct WeaponDataShooter {	//un'arma che spara proiettili; munizioni infinite
	int projectile_id;	//il tipo di proiettile sparato
	char *texture;
	double attack_timer;
};

void spawn_random_pickupable(float pos_x, float pos_y, World *world);
void create_projectile(World *world, Entity *source, int projectile_id);

/**************************GET*INFO*****************************/
const WeaponDataMelee *get_data_melee(int id);
const WeaponDataProjectile *get_data_projectile(int id);
const WeaponDataShooter *get_data_shooter(int id);

void do_other_attack(World *world, Entity *holder);
void do_other_update(Entity *holder);