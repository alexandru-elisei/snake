/*
 * Semnalizeaza diferite evenimente
 */

#ifndef FLAGS_H

#define FLAGS_H

#include <stdio.h>

/* Adaug un eveniment */
void flag_add(char *nm, int val);

/* Initializez */
void flag_init();

/* Verific daca un eveniment s-a produs, si daca da, returnez valoarea lui */
int flag_has(char *nm);

/* Sterg un eveniment din lista cu evenimente */
void flag_del(char *nm);

/* Resetez toate evenimentele */
void flag_reset();

#endif				/* ifndef FLAGS_H */
