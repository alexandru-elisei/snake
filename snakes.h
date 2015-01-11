
/* Controleaza miscarea sarpelui pe ecran */

#ifndef SNAKES_H

#define SNAKES_H

#include "graphics.h"

/* Construieste sarpele */
void snk_init();

/* Destructor-type function */
void snk_reset();
 
/* Am murit */
void snk_dead();

/* Verifica daca o unitate a sarpelui este in coliziune cu el insusi */
int snk_is_incolision(struct Unit *u);

/* Misca sarpele */
void snk_move(char dir);

#endif	/* #ifndef SNAKES_H */
