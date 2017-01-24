#pragma once

#include "Entities.h"

/********************************FUNZIONI*ALLA*PRESSIONE*DI*TASTO**********************************/
void player_interact (Entity *player, World *world);
void attack(World *world, Entity *source);

/********************************FUNZIONI*PER*INTERAZIONE******************************************/
//da mettere su entit� che rappresentano oggetti da raccogliere e tenere in mano
//source � il player che ha interagito, target � l'oggetto raccolto
//elimina il vecchio oggetto tenuto dal player e ci mette i suoi dati
void pickupable_on_interact (Entity *source, Entity *target);