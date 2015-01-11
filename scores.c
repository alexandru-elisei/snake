
/* Tine evidenta scorului curent si al scorului maxim */

#include <stdlib.h>
#include <stdlib.h>
#include "scores.h"
#include "flags.h"
#include "generic.h"

#define SCORE_FILE	"data.bin"

struct HighScore {
	char *nume;
	int score;
};

static int points;	/* scorul curent */
static int level;	/* nivelul curent */
static HighScore high;

/* Constructor */
void score_init()
{
	points = 0;
	level = 0;
}

/* Creste scorul */
void score_add(int p)
{
	int tmp;

	points += p;
	tmp = points / LVL_THRESHOLD;

	if (tmp > level)
		flag_add("lvlup", 1);

	level = tmp;
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
