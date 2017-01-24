#pragma once

#include "utils.h"
#include "Entities.h"

//i rettangoli si intersecano
int collides(Rectangle *r1, Rectangle *r2);

//centro del rettangolo nel cerchio
int collides(Vec2 *center, float radius, Rectangle *r);

//distanza al quadrato tra 2 posizioni
float distance2(Vec2 *pos1, Vec2 *pos2);

//l'entità più vicina (centro) nella lista specificata alla posizione (solo se entro la distanza specificata)
Entity *closest(Vec2 *pos, float max_dist, World *world, EntityListType list);

//il centro dell'entità
Vec2 get_entity_center(Entity *e);
Rectangle get_entity_rectangle(Entity *e);