#pragma once

#include "utils.h"

//i rettangoli si intersecano
int collides(Rectangle *r1, Rectangle *r2);

//centro del rettangolo nel cerchio
int collides(Vec2 *center, float radius, Rectangle *r);

//distanza al quadrato tra 2 posizioni
float distance2(Vec2 *pos1, Vec2 *pos2);