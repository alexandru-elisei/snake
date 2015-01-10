#ifndef SCORES_H

#define SCORES_H

#define POINTS_SMALL	10	/* points for normal food */
#define POINTS_BONUS	15	/* points for bonus food */

/* Creste scorul */
void scr_add(int p);

/* Returneaza scorul curent */
int scr_get();

#endif		/* ifndef SCORES_H */
