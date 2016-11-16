#include "Entities.h"

int createEntity(World *world) {
	for (int entity = 0; entity < MAX_ENTITIES; entity++) {
		if (world->mask[entity] == COMPONENT_NONE) { //la prima entity che trova senza componenti non è un entity, è un buco
			return(entity);
		}
	}
	//printf("Error!  No more entities left!\n");
	return(MAX_ENTITIES);
}

void destroyEntity(World *world, int entity) {
	world->mask[entity] = COMPONENT_NONE;
}

int createPlatform(World *world, float x, float y, float mass, float elasticity) {
}


int createPlayer(World *world, float x, float y, char* name){
	int entity = createEntity(world);
	world->mask[entity] = COMPONENT_POSITION | COMPONENT_VELOCITY | COMPONENT_APPEARANCE | COMPONENT_BODY | COMPONENT_COLLISION | COMPONENT_PLAYER;
	world->positionComponents[entity].x = x;
	world->positionComponents[entity].y = y;
	world->velocityComponents[entity].speedX = 0;
	world->velocityComponents[entity].speedY = 0;
	//manca appearance e collision
	world->bodyComponents[entity].mass = 10;
	world->bodyComponents[entity].elasticity = .7f;
	world->playerComponents[entity].name = name;
	return(entity);
}