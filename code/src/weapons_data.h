#pragma once

#include "Entities.h"

/********************************STRUTTURE*PER*INFO*ARMI*****************************************/
enum TypeInHand {
	OTHER,
	MELEE,
	PROJECTILE,	//proiettile singolo
	SHOOTER
};

struct WeaponDataMelee {	//quando usata danneggia tutti gli hittable nell'area
	Rectangle hitbox; //la posizione del rettangolo rappresenta lo scostamento rispetto all'angolo basso-sx del player che attacca
	float damage;
	double attack_timer;	//dopo quanto tempo posso attaccare di nuovo
};

struct WeaponDataProjectile {//un'arma da lancio (o proiettile di shooter)
	Vec2 size;
	int angle;		//l'angolo di tiro in gradi; se 0 è un tiro dritto non affetto dalla gravità
	float speed;
	char *texture;	//il file da cui caricare la texture dell'oggetto tirato
	double timer;	//se 0 hit_func viene chiamata alla collisione con static o dinamic, altrimenti quando scade il timer (e rimbalza)
	void (*hit_func) (Entity *self, Entity *first_hit, World *world);	//nota: se è a timer first_hit sarà sempre NULL
};

struct WeaponDataShooter {	//un'arma che spara proiettili; munizioni infinite
	int projectile_id;	//il tipo di proiettile sparato
	double attack_timer;
};

/**************************GET*INFO*****************************/
WeaponDataMelee *get_data_melee(int id);
WeaponDataProjectile *get_data_projectile(int id);
WeaponDataShooter *get_data_shooter(int id);