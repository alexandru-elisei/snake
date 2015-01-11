#include "scores.h"
#include "generic.h"

static int points;	/* scorul curent */
static int level;	/* nivelul curent */

/* Constructor */
void score_init()
{
	points = 0;
	level = 0;
}

/* Creste scorul */
void score_add(int p)
{
	points += p;
	level = points / LVL_THRESHOLD;
}

/* Returneaza scorul curent */
int score_get()
{
	return points;
}

/* Returneaza nivelul curent */
int score_lvl()
{
	return level;
}
