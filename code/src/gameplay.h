#pragma once

#include "Entities.h"

/********************************FUNZIONI*ALLA*PRESSIONE*DI*TASTO**********************************/
void player_interact (Entity *player, World *world);
void attack(World *world, Entity *source);

/********************************FUNZIONI*BASE*SU*ENTITY*******************************************/
void damage_entity(Entity *e, float damage);
void stun_entity(Entity *e, double timer);
void test_hitbox(World *world, Rectangle *hitbox);