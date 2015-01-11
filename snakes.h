
/* Controleaza miscarea sarpelui pe ecran */

#ifndef SNAKES_H

#define SNAKES_H

#include "graphics.h"

/* Construieste sarpele */
void snk_init();

/* Adaug miscarea ce se va executa la timer */
void snk_addmv(char dir);

/* Verifica daca tasta apasata este o directie valida */
int snk_isdir(char c);

/* Destructor-type function */
void snk_reset();
 
/* Am murit */
void snk_dead();

/* Verifica daca o unitate a sarpelui este in coliziune cu el insusi */
int snk_is_incolision(struct Unit *u);

/* Misca sarpele */
void snk_move();

#endif	/* #ifndef SNAKES_H */
