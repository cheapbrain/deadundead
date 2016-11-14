#include "Components.h"

#define MAX_ENTITIES 1000;

typedef struct {
	int mask[MAX_ENTITIES];
	PositionComponent positionComponents[MAX_ENTITIES];
	VelocityComponent velocityComponents[MAX_ENTITIES];
	AppearanceComponent appearanceComponents[MAX_ENTITIES];
	MassComponent massComponents[MAX_ENTITIES];
	CollisionComponent collisionComponents[MAX_ENTITIES];
	PlayerComponent playerComponents[MAX_ENTITIES];
	BombComponent bombComponents[MAX_ENTITIES];
} World;

//creazione e distruzione nuova entità generica
int createEntity(World *world);
void destroyEntity(World *world, int entity);

int createPlayer(World *world, float x, float y, float speedX, float speedY, float mass, char* name); //mancano parametri




