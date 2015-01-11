#ifndef SCORES_H

#define SCORES_H

#define POINTS_SMALL	10	/* valoarea in puncte a mancarii mici */
#define POINTS_BONUS	15	/* valoarea in puncte a mancarii mari */

/* Constructor */
void score_init();

/* Creste scorul */
void score_add(int p);

/* Returneaza scorul curent */
int score_get();

#endif		/* ifndef SCORES_H */
