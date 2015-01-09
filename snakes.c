/* Controloaza miscarea sarpelui pe ecran */

#include <stdlib.h>
#include <curses.h>
#include "flags.h"
#include "generic.h"
#include "graphics.h"
#include "snakes.h"

static struct Unit *snake;	/* sarpele, duh */
static char next_dir;		/* Directia de miscare a sarpelui */
static int snake_len;		/* lungimea curenta a sarpelui */ 
static int snake_mem;		/* memoria alocata curent sarpelui */

/******************************************************/
static FILE *f;
/******************************************************/

/* Antet functii locale/private */
static void aloca_mem(struct Unit **u, int *mem);
static void creeaza_sarpe();
static void muta_unitate(struct Unit *u, char dir);
static void snk_update();
static int coord_egale(struct Unit *u1, struct Unit *u2);

/* Aici trebuie sa inceapa sa se miste sarpele */
void snk_init()
{
	/* Stupidity check */
	if (START_LENGTH > MEM_INC) {
		flag_add("fatal_error", 1);
		return;
	}

	/* La pornirea jocului sarpele se misca pe orizontala spre dreapta */
	creeaza_sarpe();
	next_dir = RIGHT;

	/* Anunt programul ca incep sa ma joc */
	flag_add("game_mode", 1);

	/* Sanity check */
	flag_del("dead");

	snk_update();
}

/* Adaug miscarea ce se va executa la timer */
void snk_addmv(char dir)
{

	if (dir == UP && next_dir == DOWN);
	else if (dir == DOWN && next_dir == UP);
	else if (dir == LEFT && next_dir == RIGHT);
	else if (dir == RIGHT && next_dir == LEFT);
	else if (dir == UP || dir == DOWN || dir == LEFT || dir == RIGHT)
		next_dir = dir;

	/* Temporary */
	snk_update();
}

/* Verifica daca tasta apasata este o directie valida */
int snk_isdir(char c)
{
	if (c == UP || c == DOWN || c == LEFT || c == RIGHT)
		return 1;

	return 0;
}

/* Destructor-type function */
void snk_reset()
{
	/* Nu mai ma joc */
	flag_del("game_mode");

	/* Distrug sarpele */
	snake_len = 0;
	snake_mem = 0;
	free(snake);

	/* Nu mai ma misc */
	next_dir = 0;
}

/* Am murit */
void snk_dead()
{
	snk_reset();
	flag_add("dead", 1);
	gph_drwborder();
	gph_printcenter("** YOU ARE DEAD **");
}

/* Genereaza noua pozitie a sarpelui si apoi o deseneaza pe ecran */
static void snk_update()
{
	int i;

	for (i = 1; i < snake_len; i++)
		snake[i - 1] =  snake[i];
	muta_unitate(&snake[snake_len - 1], next_dir);

	/* Verific daca sarpele a intrat in chenar */
	if (gph_is_onborder(&snake[snake_len - 1]) == 1) {
		snk_dead();
		return;
	}

	/* Verific daca sarpele nu intra in coliziune cu el insusi */
	if (snk_is_incolision(&snake[snake_len - 1], snake) == 1) {
		snk_dead();
		return;
	}

	gph_drwsnk(snake, snake_len);
}

/* Verifica daca o unitate nu apartine sarpelui, pentru a detecta coliziunile*/
int snk_is_incolision(struct Unit *u,		/* pointer catre un element */
		struct Unit *snk)		/* vector */
{
	int i;
	int aparitii;

	aparitii = 0;
	for (i = 0; i < snake_len; i++)
		if (coord_egale(u, &snk[i]) == 1)
			++aparitii;

	/* Daca sunt doua unitati cu coordonatele egale, atunci se
	 * suprapun */
	if (aparitii == 2)
		return 1;
	else
		return 0;
}

/* Aloca memorie pentru vectori de tip Unit */
static void aloca_mem(struct Unit **u, int *mem)
{
	if (*mem == 0)
		*u = (struct Unit *) malloc((*mem + MEM_INC) *
				sizeof(struct Unit));
	else
		*u = (struct Unit *) realloc(*u,
				(*mem + MEM_INC) * sizeof(struct Unit));
		
	if (*u == NULL) {
		flag_add("fatal_error", 1);
		return;
	}

	(*mem) += MEM_INC;
}

/* Genereaza sarpele de la inceperea jocului
 * El se misca pe orizontala spre dreapta, incepand cu centrul chenarului */
static void creeaza_sarpe()
{
	int x, y;
	int i;

	snake_len = START_LENGTH;
	snake_mem = 0;
	aloca_mem(&snake, &snake_mem);

	gph_getcenter(&x, &y);
	x -= 2;

	for (i = 0; i < snake_len; i++) {
		snake[i].x = x + i;
		snake[i].y = y;
	}
}

/* Muta un punct in directia indicata */
static void muta_unitate(struct Unit *u, char dir)
{
	switch (dir) {
		case UP:
			--(u->y);
			break;
		case DOWN:
			++(u->y);
			break;
		case LEFT:
			--(u->x);
			break;
		case RIGHT:
			++(u->x);
			break;
	}
}

/* Verifica daca doua unitati, trimise ca pointer, au aceleasi coordonate */
static int coord_egale(struct Unit *u1, struct Unit *u2)
{
	if (u1->x == u2->x && u1->y == u2->y)
		return 1;

	return 0;
}
