#include "Systems.h"

//esempio movimento
void MovementFunction(World *world){
	PositionComponent *p;
	VelocityComponent *v;
	for (int entity = 0; entity < MAX_ENTITIES; ++entity){
		if ((world->mask[entity] & MOVEMENT_MASK) == MOVEMENT_MASK){ //se può mouversi
			p = &(world->positionComponents[entity]);
			v = &(world->velocityComponents[entity]);
			//v-> -= 0.98f; eventuale attrito, da discutere
			p->x += v->speedX;
			p->y += v->speedY;
		}
	}
}

void GravityFunction(World *world) {
	BodyComponent m;
	for (int entity = 0; entity < MAX_ENTITIES; ++entity) {
		if ((world->mask[entity] & GRAVITY_MASK) == GRAVITY_MASK) { //se deve cadere
			//TO-DO update velocità
		}
	}
}

void CollisionFunction(World *world) {
	PositionComponent p;
	VelocityComponent v;
	BodyComponent m;
	CollisionComponent c;
	for (int entity = 0; entity < MAX_ENTITIES; ++entity) {
		if ((world->mask[entity] & COLLISION_MASK) == COLLISION_MASK) { //se deve collidere
			//TO-DO update posizione e velocità a seconda di collisioni e massa
		}
	}
}

void RenderFunction(World *world) {
	PositionComponent p;
	AppearanceComponent a;
	for (int entity = 0; entity < MAX_ENTITIES; ++entity) {
		if ((world->mask[entity] & RENDER_MASK) == RENDER_MASK) { //se deve essere disegnato
			//TO-DO disegno
		}
	}
}

void InputFunction(World *world) {
	PlayerComponent p;
	for (int entity = 0; entity < MAX_ENTITIES; ++entity) {
		if ((world->mask[entity] & INPUT_MASK) == INPUT_MASK) { //se prende input
			//TO-DO update velocità e tanto altro
		}
	}
}