#include "utils.h"
#include "screen.h"

/********************************STRUTTURE*PER*INFO*ARMI*****************************************/
struct ThrowData {
	int angle;	  //l'angolo di tiro in gradi; se 0 è un tiro dritto non affetto dalla gravità
	float speed;
};

struct WeaponDataMelee {	//quando usata danneggia tutti gli hittable nell'area
	Rectangle hitbox; //la posizione del rettangolo rappresenta lo scostamento rispetto all'angolo basso-sx del player che attacca
	float damage;
	float attack_timer;	//dopo quanto tempo posso attaccare di nuovo
};

struct WeaponDataGrenade {	//un'arma da lancio che fa qualcosa alla scadenza del timer (tipo la granata)
	Vec2 size;
	ThrowData throw_data;
	char *texture;			//il file da cui caricare la texture dell'oggetto tirato
	float timer;
	void (*on_expire) (Entity *self, Entity *thrower);	//cosa fare quando il timer scade
};

struct WeaponDataProjectile {//un'arma da lancio (o proiettile di shooter) che fa qualcosa quando collide con hittable o static (tipo piatto)
	Vec2 size;
	ThrowData throw_data;
	char *texture;			//nome texture
	void (*on_collision) (Entity *self, Entity *first_hit, Entity *thrower);
};

struct WeaponDataShooter {	//un'arma che spara proiettili
	WeaponDataProjectile projectile_type;	//il tipo di proiettile sparato
	int ammo;		//quante munizioni ha; -1 vuol dire infinite
}


/********************************FUNZIONI*ALLA*PRESSIONE*DI*TASTO**********************************/
void player_interact (Entity *player, World *world);

/********************************FUNZIONI*PER*INTERAZIONE******************************************/
//da mettere su entità che rappresentano oggetti da raccogliere e tenere in mano
//source è il player che ha interagito, target è l'oggetto raccolto
//elimina il vecchio oggetto tenuto dal player e ci mette i suoi dati
void pickupable_on_interact (Entity *source, Entity *target);