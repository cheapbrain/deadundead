#pragma once

#include "Entities.h"

/********************************STRUTTURE*PER*INFO*ARMI*****************************************/
enum TypeInHand {
	MELEE,		//NOTA: tipo melee id 0 è un pugno (senza niente in mano)
	PROJECTILE,	//proiettile singolo
	SHOOTER,
	OTHER
};

struct ObjectDrawInfo {
	char *texture;
	Vec2 size;
};

struct OtherData {
	void (*on_update) (Entity *holder);
	void (*on_attack) (Entity *holder, World *world);
	void (*on_interact)(Entity *source, Entity *target, World *world);

	ObjectDrawInfo draw_info;
};

struct WeaponDataMelee {	//quando usata danneggia tutti gli hittable nell'area
	Rectangle hitbox; //la posizione del rettangolo rappresenta lo scostamento rispetto all'angolo basso-sx del player che attacca
	float damage;
	double attack_timer;	//dopo quanto tempo posso attaccare di nuovo

	ObjectDrawInfo draw_info;
};

struct WeaponDataProjectile {//un'arma da lancio (o proiettile di shooter)
	Vec2 size;
	int angle;		//l'angolo di tiro in gradi; se 0 è un tiro dritto non affetto dalla gravità
	float speed;
	double timer;	//se <0 hit_func viene chiamata alla collisione con static o dynamic, altrimenti quando scade il timer (e rimbalza)
	void (*hit_func) (Entity *self, Entity *first_hit, World *world);	//nota: se è a timer first_hit sarà sempre NULL

	ObjectDrawInfo draw_info;
};

struct WeaponDataShooter {	//un'arma che spara proiettili; munizioni infinite
	int projectile_id;	//il tipo di proiettile sparato
	double attack_timer;

	ObjectDrawInfo draw_info;
};

void spawn_pickupable(float pos_x, float pos_y, int type, int id, World *world);
void spawn_random_pickupable(float pos_x, float pos_y, World *world);
void create_projectile(World *world, Entity *source, int projectile_id);

/**************************GET*INFO*****************************/
const ObjectDrawInfo *get_object_draw_info(int type, int id);
const WeaponDataMelee *get_data_melee(int id);
const WeaponDataProjectile *get_data_projectile(int id);
const WeaponDataShooter *get_data_shooter(int id);

void do_other_attack(World *world, Entity *holder);
void do_other_update(Entity *holder);