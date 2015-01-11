#ifndef SCORES_H

#define SCORES_H

#define POINTS_SMALL	10	/* valoarea in puncte a mancarii mici */
#define POINTS_BONUS	15	/* valoarea in puncte a mancarii mari */
#define LVL_THRESHOLD	40	/* la cate puncte trec la urmatorul nivel */

/* Constructor */
void score_init();

/* Creste scorul */
void score_add(int p);

/* Returneaza scorul curent */
int score_get();

/* Returneaza nivelul curent */
int score_lvl();

#endif		/* ifndef SCORES_H */
