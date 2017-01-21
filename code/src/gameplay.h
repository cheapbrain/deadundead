#include "utils.h"
#include "screen.h"

struct ThrowData {
	int angle;	  //l'angolo di tiro in gradi; se 0 è un tiro dritto non affetto dalla gravità
	float speed;
};

struct WeaponDataMelee {	//quando usata danneggia tutti gli hittable nell'area
	Rectangle hitbox;
	Vec2 offset; //lo scostamento rispetto all'hitbox di chi usa l'arma del rettangolo (angolo giù sx)
	float damage;
};

struct WeaponDataGrenade {	//un'arma da lancio che fa qualcosa alla scadenza del timer (tipo la granata)
	Rectangle hitbox;
	ThrowData throw_data;
	char *texture;			//il file da cui caricare la texture dell'oggetto tirato
	float timer;
	void (*on_expire) (Entity *self, Entity *thrower);	//cosa fare quando il timer scade
};

struct WeaponDataProjectile {//un'arma da lancio (o proiettile di shooter) che fa qualcosa quando collide con hittable o static (tipo piatto)
	Rectangle hitbox;
	ThrowData throw_data;
	char *texture;			//nome texture
	void (*on_collision) (Entity *self, Entity *first_hit, Entity *thrower);
};

struct WeaponDataShooter {	//un'arma che spara proiettili
	WeaponDataProjectile projectile_type;	//il tipo di proiettile sparato
	int ammo;	//quante munizioni ha; -1 vuol dire infinite
}