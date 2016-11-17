#include "Components.h"

#define MAX_ENTITIES 1000

struct World {
	int mask[MAX_ENTITIES];
	PositionComponent positionComponents[MAX_ENTITIES];
	VelocityComponent velocityComponents[MAX_ENTITIES];
	AppearanceComponent appearanceComponents[MAX_ENTITIES];
	BodyComponent bodyComponents[MAX_ENTITIES];
	CollisionComponent collisionComponents[MAX_ENTITIES];
	PlayerComponent playerComponents[MAX_ENTITIES];
};

//creazione e distruzione nuova entità generica
int createEntity(World *world);
void destroyEntity(World *world, int entity);

int createPlayer(World *world, float x, float y, char* name); //mancano parametri




