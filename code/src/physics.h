#pragma once

#include "utils.h"
#include "Entities.h"

//calcola se i rettangoli si intersecano
int collides(struct Rectangle *r1, struct Rectangle *r2);

//nota: restituisce sempre lo stesso puntatore, l'array restituito viene allocato solo una volta.
//Quando viene richiamata la funzione quindi i risultati precedenti vengono sostituiti con quelli nuovi
//Nel caso si cambia in un attimo.
struct Entity **search_in_area(struct Entity **list, int size, struct Rectangle *area);