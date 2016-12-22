#pragma once

#include "utils.h"
#include "Entities.h"

//calcola se i rettangoli si intersecano
int collides(struct Rectangle *r1, struct Rectangle *r2);

//Ricerca le entità nell'area specificata
struct Entity **search_in_area(struct Entity **list, int size, struct Rectangle *area);