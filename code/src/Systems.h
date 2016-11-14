#include "Entities.h"
#define MOVEMENT_MASK (COMPONENT_POSITION | COMPONENT_VELOCITY)
#define GRAVITY_MASK (COMPONENT_VELOCITY | COMPONENT_MASS)
#define COLLISION_MASK (COMPONENT_POSITION | COMPONENT_VELOCITY | COMPONENT_MASS | COMPONENT_COLLISION)
#define RENDER_MASK (COMPONENT_POSITION | COMPONENT_APPEARANCE)
#define INPUT_MASK (COMPONENT_PLAYER)

//esempio movement
void MovementFunction(World *world);
void GravityFunction(World *world);
void CollisionFunction(World *world);
void RenderFunction(World *world);
void InputFunction(World *world);