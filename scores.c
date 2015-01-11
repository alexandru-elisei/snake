#include "scores.h"
#include "generic.h"

static int points;	/* scorul curent */

/* Constructor */
void score_init()
{
	points = 0;
}

/* Creste scorul */
void score_add(int p)
{
	points += p;
}

/* Returneaza scorul curent */
int score_get()
{
	return points;
}
