#include "Entities.h"

int createEntity(World *world) {
	for (int entity = 0; entity < MAX_ENTITIES; entity++) {
		if (world->mask[entity] == COMPONENT_NONE) { //la prima entity che trova senza componenti non è un entity, è un buco
			return(entity);
		}
	}
	printf("Error!  No more entities left!\n");
	return(MAX_ENTITIES);
}

void destroyEntity(World *world, int entity) {
	world->mask[entity] = COMPONENT_NONE;
}


int createPlayer(World *world, float x, float y, float speedX, float speedY, float mass, char* name){
	int entity = createEntity(world);
	world->mask[entity] = COMPONENT_POSITION | COMPONENT_VELOCITY | COMPONENT_APPEARANCE | COMPONENT_PHYSICS | COMPONENT_PLAYER;
	world->positionComponents[entity].x = x;
	world->positionComponents[entity].y = y;
	world->velocityComponents[entity].speedX = speedX;
	world->velocityComponents[entity].speedY = speedY;
	//manca appearance
	world->physicsComponents[entity].mass = mass;
	world->playerComponents[entity].name = name;
	return(entity);
}