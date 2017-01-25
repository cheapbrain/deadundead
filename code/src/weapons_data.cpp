/*
const WeaponDataMelee wd_melee[] = { //indice<->id_in_hand (con type_in_hand=melee)

};
*/
#include "weapons_data.h"
#include "physics.h"
#include <stdlib.h>

/************FUNZIONI*BASE*PER*PROIETTILI*************/
static void damage_entity(Entity *e, float damage) {
	e->health -= damage;
}
static void stun_entity(Entity *e, double timer) {
	e->status = STUNNED;
	e->timer = timer;
}
//applica hit_func a tutte le entità hittable nel cerchio attorno al proiettile escluso chi lo ha lanciato
static void hit_in_area(World *world, float radius, Entity *projectile, void (*hit_func) (Entity *e)) {
	Vec2 p_center = get_entity_center(projectile);
	EntityList *hittable_list = &world->lists[HITTABLE_LIST];

	for (int i = 0; i < hittable_list->count; i++) {
		Entity *e = world_get_entity(world, hittable_list->entity_id[i]);
		Rectangle er = get_entity_rectangle(e);
		if (e != projectile->thrower && collides(&p_center, radius, &er)) {
			hit_func(e);
		}
	}
}

WeaponDataMelee *get_data_melee(int id) {return NULL;}//TODO
WeaponDataProjectile *get_data_projectile(int id){return NULL;}//TODO
WeaponDataShooter *get_data_shooter(int id){return NULL;}//TODO