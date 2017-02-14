#define _USE_MATH_DEFINES

#include <math.h>
#include "objects_data.h"
#include "physics.h"
#include "gameplay.h"
#include <stdlib.h>

void pozione_on_interact(Entity *source, Entity *target, World *world);
void token_on_update(Entity *holder, double delta);
void func_attack_magnete(Entity *holder, World *world);

const OtherData other_data[] = {
	{//TOKEN PUNTI VITTORIA
		&token_on_update,
		NULL,
		NULL,	//lo mette automaticamente la funzione di creazione il pickupable on interact

		{"../images/token.png", {0.3f, 0.3f}}
	},
	{//POZIONE PUNTI ISTANTANEI
		NULL,
		NULL,
		&pozione_on_interact,

		{"../images/pozione_viola.png", {0.3f, 0.3f}}
	},
	{//MAGNETE
		NULL,
		&func_attack_magnete,
		NULL,

		{"../images/magnete.png", {0.3f, 0.3f}}
	}
};

//numero di armi melee che non spawnano, da aggiornare a mano; le tengo all'inizio
#define NUM_MELEE_NON_S 3
const WeaponDataMelee wd_melee[] = {
	{//PUGNO
		{{1.0f,1.0f},{1.0f,1.0f}},
		1.0f,
		1.0,

		{NULL, {0,0}}
	},
#define ID_CANDELABRO_ACCESO 1
	{//CANDELABRO ACCESO
		{{1.0f,1.0f},{1.0f,1.0f}},
		2.0f,
		1.0,

		{"../images/candelabro_acceso.png", {0.3f, 0.3f}}
	},
#define ID_SCOPA_ACCESA 2
	{//SCOPA ACCESA
		{{1.0f,1.0f},{1.0f,1.0f}},
		1.0f,
		1.0,

		{"../images/scopa_accesa.png", {0.1f, 0.5f}}
	},
#define ID_CANDELABRO 3
	{//CANDELABRO
		{{1.0f,1.0f},{1.0f,1.0f}},
		1.0f,
		1.0,

		{"../images/candelabro.png", {0.3f, 0.3f}}
	},
#define ID_SCOPA 4
	{//SCOPA
		{{1.0f,1.0f},{1.0f,1.0f}},
		1.0f,
		1.0,

		{"../images/scopa.png", {0.1f, 0.5f}}
	}
};

void func_bomba (Entity *self, Entity *first_hit, World *world);
void func_piatto (Entity *self, Entity *first_hit, World *world);
void func_ampolla_verde (Entity *self, Entity *first_hit, World *world);
void func_p_balestra (Entity *self, Entity *first_hit, World *world);
void func_p_staffa (Entity *self, Entity *first_hit, World *world);

//numero di proiettili spawnabili (cioè non solo per shooter); da aggiornare a mano
#define NUM_PROJECTILE 3
const WeaponDataProjectile wd_projectile[] = {//i non spawnabili vanno in fondo
	{//BOMBA
		{0.2f,0.2f},
		45,
		6.0f,
		3.0,
		&func_bomba,

		{"../images/bomba.png", {0.2f, 0.2f}}
	},
	{//AMPOLLA VERDE
		{0.2f,0.2f},
		45,
		6.0f,
		-1,
		&func_ampolla_verde,

		{"../images/ampolla_verde.png", {0.2f, 0.2f}}
	},
	{//PIATTO
		{0.3f,0.3f},
		45,
		6.0f,
		-1,
		&func_piatto,

		{"../images/piatto.png", {0.3f, 0.3f}}
	},
//NON SPAWNABILI IN FONDO
#define ID_P_BALESTRA (0+NUM_PROJECTILE)
	{//PROIETTILE BALESTRA
		{0.15f,0.02f},
		0,
		4.0f,
		-1,
		&func_p_balestra,

		{"../images/balestra_p.png", {0.15f, 0.05f}}
	},
#define ID_P_STAFFA (1+NUM_PROJECTILE)
	{//PROIETTILE STAFFA
		{0.1f,0.1f},
		0,
		2.0f,
		-1,
		&func_p_staffa,

		{"../images/staffa_p.png", {0.1f, 0.1f}}
	},
};

const WeaponDataShooter wd_shooter[] = {
	{//BALESTRA
		ID_P_BALESTRA,
		1.0,

		{"../images/balestra.png", {0.3f, 0.3f}}
	},
	{//STAFFA
		ID_P_STAFFA,
		0.8,

		{"../images/staffa.png", {0.05f, 0.3f}}
	}
};

//quantità spawnabili
#define NUM_OTHER (sizeof(other_data)/sizeof(OtherData))
#define NUM_MELEE (sizeof(wd_melee)/sizeof(WeaponDataMelee) - NUM_MELEE_NON_S)
#define NUM_SHOOTER (sizeof(wd_shooter)/sizeof(WeaponDataShooter))


/*getter*e*vari*/
void pickupable_on_interact (Entity *source, Entity *target, World *world);
void spawn_pickupable(float pos_x, float pos_y, int type, int id, World *world) {
	Entity *e = world_new_entity(world, RENDER | ACTIVE_EVENT);
	const ObjectDrawInfo *di = get_object_draw_info(type, id);
	e->tag = e->tag | ITEM;
	e->texture = load_texture(di->texture);
	e->width = di->size.x;
	e->height = di->size.y;

	e->type_in_hand = type;
	e->id_in_hand = id;
	e->x = pos_x+e->width/2; e->y = pos_y+e->height/2;
	e->render = &wall_render;
	if (type == OTHER && other_data[id].on_interact != NULL) {
		e->on_interact = other_data[id].on_interact;
	} else {
		e->on_interact = &pickupable_on_interact;
	}
}

void spawn_random_pickupable(float pos_x, float pos_y, World *world) {
	spawn_pickupable(pos_x, pos_y, PROJECTILE, 1, world);
	return;
	int val = rand()%(NUM_OTHER + NUM_PROJECTILE + NUM_MELEE + NUM_SHOOTER);
	if (val < NUM_OTHER) {
		spawn_pickupable(pos_x, pos_y, OTHER, val, world);
	} else if (val < NUM_OTHER+NUM_MELEE) {
		spawn_pickupable(pos_x, pos_y, MELEE, val - NUM_OTHER + NUM_MELEE_NON_S, world);
	} else if (val < NUM_OTHER+NUM_MELEE+NUM_SHOOTER) {
		spawn_pickupable(pos_x, pos_y, SHOOTER, val - (NUM_OTHER+NUM_MELEE), world);
	} else {
		spawn_pickupable(pos_x, pos_y, PROJECTILE, val - (NUM_OTHER+NUM_MELEE+NUM_SHOOTER), world);
	}
}

const WeaponDataMelee *get_data_melee(int id) {return &(wd_melee[id]);}
const WeaponDataProjectile *get_data_projectile(int id){return &(wd_projectile[id]);}
const WeaponDataShooter *get_data_shooter(int id){return &(wd_shooter[id]);}

const ObjectDrawInfo *get_object_draw_info(int type, int id) {
	switch (type) {
	case OTHER:		return &(other_data[id].draw_info); break;
	case MELEE:		return &(get_data_melee(id)->draw_info); break;
	case SHOOTER:	return &(get_data_shooter(id)->draw_info); break;
	case PROJECTILE:return &(get_data_projectile(id)->draw_info); break;
	default:		return NULL;
	}
}

void do_other_attack(World *world, Entity *holder) {
	if (other_data[holder->id_in_hand].on_attack != NULL) {
		other_data[holder->id_in_hand].on_attack(holder, world);
	}
}
void do_other_update(Entity *holder, double delta) {
	if (other_data[holder->id_in_hand].on_update != NULL) {
		other_data[holder->id_in_hand].on_update(holder, delta);
	}
}















/************FUNZIONI*BASE*PER*ARMI*************/
//applica hit_func a tutte le entità hittable nel cerchio attorno al proiettile
static void hit_in_area(World *world, float radius, Entity *projectile, void (*hit_func) (Entity *e), int exclude_thrower) {
	Vec2 p_center = get_entity_center(projectile);
	EntityList *hittable_list = &world->lists[HITTABLE_LIST];

	for (int i = 0; i < hittable_list->count; i++) {
		Entity *e = world->entities + hittable_list->entity_indexes[i];
		Rectangle er = get_entity_rectangle(e);
		if ((!exclude_thrower || e->id != projectile->thrower_id) && collides(&p_center, radius, &er)) {
			hit_func(e);
		}
	}
}

static void apply_physics(Entity *self, float downward_accelleration, float downward_max_speed, float x_friction, double delta) {
	self -> old_x = self -> x; self -> old_y = self -> y;
	self -> x += (float) ((self->speed_x)*delta); self -> y += (float) ((self->speed_y)*delta);
	self -> speed_y -= (downward_accelleration*(float)delta);
	if (self->is_on_floor) {
		self->is_on_floor = 0;
		self -> speed_x -= (self->speed_x)*x_friction*(float) delta;
	}
	if (self -> speed_y < downward_max_speed) {
		self -> speed_y = downward_max_speed;
	}
}

static void projectile_update_function(Entity *self, World *world, double delta) {
	//aggiorno posizione (collisioni gestite nel world update)
	if (self->type_in_hand == 1) {	//lo uso per sapere se applicare la fisica o no
		self -> old_x = self -> x;
		self -> x += (float) ((self->speed_x)*delta);
	} else {
#define DOWNWARD_ACCELLERATION	7.f//TODO
#define DOWNWARD_MAX_SPEED		-4.f//TODO
#define X_FRICTION				7.f
		/*
		self -> old_x = self -> x; self -> old_y = self -> y;
		self -> x += (float) ((self->speed_x)*delta); self -> y += (float) ((self->speed_y)*delta);
		self -> speed_y -= (DOWNWARD_ACCELLERATION*(float)delta);
		if (self->is_on_floor) {
			self->is_on_floor = 0;
			self -> speed_x -= (self->speed_x)*X_FRICTION*(float) delta;
		}
		if (self -> speed_y < DOWNWARD_MAX_SPEED) {
			self -> speed_y = DOWNWARD_MAX_SPEED;
		}
		*/
		apply_physics(self, DOWNWARD_ACCELLERATION, DOWNWARD_MAX_SPEED, X_FRICTION, delta);
#undef X_FRICTION
#undef DOWNWARD_ACCELLERATION
#undef DOWNWARD_MAX_SPEED
	}

	//controllo se attivarmi
	if (self -> timer < 0) {
		//alla collisione
		Rectangle rect_self = {{self->x, self->y},{self->width, self->height}};
		EntityList *hittable_list = &world->lists[HITTABLE_LIST];

		for (int i = 0; i < hittable_list->count; i++) {
			Entity *e = world->entities + hittable_list->entity_indexes[i];
			Rectangle rect_e = {{e->x, e->y},{e->width, e->height}};
			if (e -> id != self -> thrower_id && collides(&rect_self, &rect_e)) {
				self -> on_collide(self, e, world);
				break;
			}
		}
	} else {
		//alla scadenza del timer
		self -> timer -= delta;
		if (self -> timer <= 0) {
			get_data_projectile(self->id_in_hand) -> hit_func(self, NULL, world);/*poi qui va rimosso*/
		}
	}
}

void create_projectile(World *world, Entity *source, int projectile_id) {
	const WeaponDataProjectile *data = get_data_projectile(projectile_id);
	Entity *p = world_new_entity(world, UPDATE | RENDER | DYNAMIC_COLLIDE);

	p->x = source->x + source->width/2; p->y = source->y + source->height/2;
	p->width = data->size.x; p->height = data->size.y;
	p->old_x = p->x; p->old_y = p->y;
	if (data->angle == 0) {
		p->speed_x = data->speed; p->speed_y = 0;
		p->type_in_hand = 1;
	} else {
		p->speed_x = (data->speed) * (float) cos(M_PI*(data->angle)/180.0);
		p->speed_y = (data->speed) * (float) sin(M_PI*(data->angle)/180.0);
		p->type_in_hand = 0;
	}
	if (!source->is_facing_right) {
		p->speed_x = -p->speed_x;
	}
	p->texture = load_texture(data->draw_info.texture);
	//int is_on_floor;
	p->is_facing_right = source->is_facing_right;
	//int player_id;
	p->bounce_coeff = 0.9f;

	//PlayerStatus status;
	//double score;
	//float skill_bar;
	p->timer = data->timer;
	p->id_in_hand = projectile_id;
	//SpriterInstance *animation; //per settare quale animazione fare

	p->thrower_id = source->id;

	p->update = &projectile_update_function;
	p->render = &wall_render; //TODO
	if (data->timer < 0) {
		p->on_collide = data->hit_func;
	} else {
		p->on_collide = NULL;
	}
	p->on_hit = NULL;
	p->on_enter = NULL;
	p->on_interact = NULL;
}

//applica hit a e se è un hittable non player, hit_player se è un player, altrimenti non fa niente
void hit_if_hittable(Entity *e, void (*hit) (Entity *), void (*hit_player) (Entity *)) {
	if ((e->tag & HITTABLE) != 0) {
		if ((e->tag & PLAYER) != 0) {
			if (hit_player != NULL) {
				hit_player(e);
			}
		} else if (hit != NULL) {
			hit(e);
		}
	}
}

//danneggia l'entità se hittable poi si distrugge
void simple_projectile(Entity *e, float damage, Entity *self, World *world) {
	if ((e->tag & HITTABLE) != 0) {
		damage_entity(e, damage);
	}
	world_remove_entity(world, self->index);
}









/************FUNZIONI*SPECIFICHE*ARMI*********************/
void func_bomba_danni(Entity *e) {damage_entity(e, 3.0f);}
void func_bomba (Entity *self, Entity *first_hit, World *world) {
	hit_in_area(world, 5.0f, self, &func_bomba_danni, 0);
	world_remove_entity(world, self->index);
}

void func_piatto_hit(Entity *e) {damage_entity(e, 1.5f);}
void func_piatto_hit_player(Entity *e) {func_piatto_hit(e); stun_entity(e, 1.0);}
void func_piatto (Entity *self, Entity *first_hit, World *world) {
	hit_if_hittable(first_hit, &func_piatto_hit, &func_piatto_hit_player);
	world_remove_entity(world, self->index);
}

void func_p_balestra(Entity *self, Entity *first_hit, World *world) {simple_projectile(first_hit, 1.0f, self, world);}

void func_p_staffa(Entity *self, Entity *first_hit, World *world) {simple_projectile(first_hit, 1.5f, self, world);}

/***funzioni attacco other***/
struct IntFloatPair {
	int i;
	float f;
};

int compareDist (const void * a, const void * b) {
	if (((struct IntFloatPair *) a)->f == ((struct IntFloatPair *) b)->f) {
		return 0;
	} else if (((struct IntFloatPair *) a)->f > ((struct IntFloatPair *) b)->f) {
		return 1;
	}
	return -1;
}
void func_attack_magnete(Entity *holder, World *world) {
	IntSet set;
	Vec2 holder_pos = {holder->x, holder->y};
	IntFloatPair *dists;
	int dists_count = 0;

	int_set_init(&set, 10);
#define RANGE 6.0f
	get_entities_in_area(&holder_pos, RANGE, world, RENDER_LIST, &set);
#undef RANGE

	if (set.count > 0) {
		dists = (IntFloatPair *) malloc(set.count*sizeof(IntFloatPair));
		
		for (int i = 0; i < set.count; i++) {
			Entity *e = world_get_entity(world, set.elements[i]);

			if (e != holder && ((e->tag & (PLAYER | ITEM)) != 0)) {
				Vec2 e_pos = {e->x, e->y};
				dists[dists_count].i = e->id;
				dists[dists_count].f = distance2(&e_pos, &holder_pos);
				dists_count++;
			}
		}
		if (dists_count > 0) {
			qsort(dists, dists_count, sizeof(IntFloatPair), &compareDist);

			for (int i = 0; i < dists_count; i++) {
				Entity *e = world_get_entity(world, dists[i].i);
				if (e -> type_in_hand != MELEE || e->id_in_hand != 0) {
					if ((e->tag & ITEM) != 0) {
						e->on_interact(holder, e, world);
					} else {
						holder->type_in_hand = e->type_in_hand;
						holder->id_in_hand = e->id_in_hand;
						e->type_in_hand = 0;
						e->id_in_hand = 0;
					}
					break;
				}
			}
		}
		free(dists);
	}

	int_set_destroy(&set);

	holder->status = ATTACKING;
	holder->timer = 1.0;
}

static void pozza_update(Entity *e, World *world, double delta) {
	e -> timer -= delta;
	if (e->timer <= 0) {
		world_remove_entity(world, e->index);
	}
}
static void goccia_on_collide (Entity *self, Entity *other, World *world) {
	if (self -> is_on_floor) {

		//crea pozza
		Entity *p = world_new_entity(world, RENDER | UPDATE | PASSIVE_EVENT);
		p->width = 0.5f; p->height = 0.06f;
		p->x = self->x - p->width/2; p->y = self->y - p->height/2;
		p->timer = 4.0;
		p->type_in_hand = POZZA_AMPOLLA_VERDE;
		p->texture = load_texture("../images/pozza_ampolla_verde.png"); //TODO

		/*trim a pavimento*/
		/*
		if (p->x < other->x) {
			p->width -= (other->x - p->x);
			p->x = other->x;
		}
		if ((p->x + p->width) > (other->x + other->width)) {
			p->width -= (p->x+p->width - other->x-other->width);
		}
		*/
		/*unione con altre pozze*/
		/*
		int has_changed = 1;
		Rectangle self_rect = {{p->x, p->y},{p->width, p->height}};
		EntityList *passive_list = &world->lists[PASSIVE_EVENT_LIST];
		while (has_changed) {
			has_changed = 0;
			for (int i = 0; i < passive_list->count; i++) {
				Entity *e = world->entities + passive_list->entity_indexes[i];
				if (!int_set_contains(&(world->to_be_removed), e->index) && (e -> tag | PASSIVE_EVENT) != 0 && e -> type_in_hand == POZZA_AMPOLLA_VERDE) {
					Rectangle other_rect = {{e->x, e->y}, {e->width, e->height}};
					if (collides(&self_rect, &other_rect)) {
						if (p->x < e->x) {
							p->width += (e->width - (p->x+p->width - e->x));
						} else {
							p->width += (e->width - (e->x+e->width - p->x));
							p->x = e->x;
						}
						has_changed = 1;
						world_remove_entity(world, e->index);
					}
				}
			}
		}
		*/

		p->update = &pozza_update;
		p->render = &wall_render; //TODO
		p->on_collide = NULL;
		p->on_hit = NULL;
		p->on_enter = NULL;
		p->on_interact = NULL;

		world_remove_entity(world, self->index);
	}
}
static void goccia_update(Entity *e, World *world, double delta) {
	apply_physics(e, 7, -4, 7, delta);
}
static void crea_goccia(World *world, float x, float y, double angle) {
#define SIZE 0.1f
#define SPEED 1.5f
	Entity *p = world_new_entity(world, UPDATE | RENDER | DYNAMIC_COLLIDE);

	p->x = x; p->y = y;
	p->width = SIZE; p->height = SIZE;
	p->old_x = p->x; p->old_y = p->y;
	p->speed_x = SPEED * (float) cos(angle);
	p->speed_y = SPEED * (float) sin(angle);
	p->bounce_coeff = 0.9f;
	p->texture = load_texture("../images/goccia_ampolla_verde.png");

	p->update = &goccia_update;
	p->render = &wall_render; //TODO
	p->on_collide = &goccia_on_collide;
	p->on_hit = NULL;
	p->on_enter = NULL;
	p->on_interact = NULL;
#undef SIZE
#undef SPEED
}
void func_ampolla_verde (Entity *self, Entity *first_hit, World *world) {
#define NUM_GOCCE 16
	for (int i = 0; i < NUM_GOCCE; i++) {
		crea_goccia(world, self->x, self->y, M_PI*2*i/NUM_GOCCE);
	}
	world_remove_entity(world, self->index);
#undef NUM_GOCCE
}







/********************************FUNZIONI*SPECIFICHE*ALTRO*****************************************/
//da mettere su entità che rappresentano oggetti da raccogliere e tenere in mano
//source è il player che ha interagito, target è l'oggetto raccolto
//elimina il vecchio oggetto tenuto dal player e ci mette i suoi dati
static void pickupable_on_interact (Entity *source, Entity *target, World *world) {
	if (source->status == ATTACKING) {
		source->timer = 0;
	}
	source->type_in_hand = target->type_in_hand;
	source->id_in_hand = target->id_in_hand;
	world_remove_entity(world, target->index);
}

void camino_on_interact(Entity *source, Entity *target, World *world) {
	if (source->type_in_hand == MELEE) {
		if (source -> id_in_hand == ID_CANDELABRO) {
			source -> id_in_hand = ID_CANDELABRO_ACCESO;
		} else if (source -> id_in_hand == ID_SCOPA) {
			source -> id_in_hand = ID_SCOPA_ACCESA;
		}
	}
}

void pozione_on_interact(Entity *source, Entity *target, World *world) {
	source->score += 5.0;
	world_remove_entity(world, target->index);
}

void token_on_update(Entity *holder, double delta) {
	holder->score += delta;
}


