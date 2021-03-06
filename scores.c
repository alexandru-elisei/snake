
/* Tine evidenta scorului curent si al scorului maxim */

#include "scores.h"
#include "flags.h"
#include "generic.h"

#define SCORE_FILE	"data.bin"

struct HighScore {
	char *name;
	int score;
};

static int points;		/* scorul curent */
static int level;		/* nivelul curent */
static struct HighScore high;	/* scorul maxim */

/*************************************************/
FILE *deb;
/*************************************************/

/* Antet metode private */

/* Salveaza scorul high in fisier */
void serializeaza();

/* Citeste scorul high din fisier */
int deserializeaza();

/* Constructor */
void score_init()
{
	points = 0;
	level = 0;

	/* Daca fisierul nu exista, il creez */
	if (deserializeaza() == 0) {
		high.name = strdup("Darth Sidious");
		high.score = 0;

		serializeaza();
	}
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

/* Returneaza un pointer catre numele cu scorul cel mai mare */
char *score_highname()
{
	return strdup(high.name);
}

/* Returneaza valoarea scorului cel mai mare */
int score_high()
{
	return high.score;
}

/* Salveaza noul highscore */
void score_addhigh(char *name, int s)
{
	high.name = strdup(name);
	high.score = s;
	serializeaza();
}

/* Salveaza scorul high in fisier */
void serializeaza()
{
	FILE *f;
	int lg_nume;

	f = fopen(SCORE_FILE, "wb");

	lg_nume = strlen(high.name) + 1;
	fwrite(&lg_nume, sizeof(int), 1, f);
	fwrite(high.name, lg_nume * sizeof(char), 1, f);
	fwrite(&high.score, sizeof(int), 1, f);

	fclose(f);
}

/* Citeste scorul high din fisier */
int deserializeaza()
{
	FILE *f;
	int lg_nume;

	f = fopen(SCORE_FILE, "rb");

	if (f == NULL)
		return 0;

	fread(&lg_nume, sizeof(int), 1, f);
	high.name = (char *) malloc(lg_nume * sizeof(char));
	fread(high.name, lg_nume * sizeof(char), 1, f);
	fread(&high.score, sizeof(int), 1, f);

	fclose(f);

	return 1;
}


