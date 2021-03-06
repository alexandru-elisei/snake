
/* Controloaza miscarea sarpelui pe ecran */

#include <stdlib.h>
#include <curses.h>
#include "flags.h"
#include "generic.h"
#include "graphics.h"
#include "snakes.h"
#include "scores.h"

#define LENGTH_EASY	 4	/* lungimea initiala la dificultatea easy */
#define LENGTH_HARD	 8	/* lungimea initiala la dificultatea hard */

static struct Unit *snake;	/* sarpele, duh */
static char next_dir;		/* Directia de miscare a sarpelui */
static int snake_len;		/* lungimea curenta a sarpelui */ 
static int snake_mem;		/* memoria alocata curent sarpelui */

/* Antet functii locale/private */

/* Aloca memorie pentru vectori de tip Unit */
static void aloca_mem(struct Unit **u, int *mem);

/* Genereaza sarpele de la inceperea jocului
 * El se misca pe orizontala spre dreapta, incepand cu centrul chenarului */
static void creeaza_sarpe();

/* Muta un punct in directia indicata */
static void muta_unitate(struct Unit *u, char dir);

/* Adaug directia de miscare */
static void adauga_directie(char dir);

/* Aici trebuie sa inceapa sa se miste sarpele */
void snk_init()
{
	/* Initializing the game_mode */
	cbreak();
	noecho();
	curs_set(FALSE);

	/* La pornirea jocului sarpele se misca pe orizontala spre dreapta */
	creeaza_sarpe();
	next_dir = RIGHT;

	/* Sanity check */
	flag_del("dead");

	snk_move(next_dir);
}


/* Destructor-type function */
void snk_reset()
{
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
	gph_drwgame();
	gph_printcenter("** DEAD **");
}

/* Genereaza noua pozitie a sarpelui si apoi o deseneaza pe ecran */
void snk_move(char dir)
{
	struct Unit poz_viitoare;
	int i;

	adauga_directie(dir);

	if (flag_has("to_grow") == 1) {
		++snake_len;
		if (snake_len == snake_mem)
			aloca_mem(&snake, &snake_mem);
		snake[snake_len - 1] = snake[snake_len - 2];

		flag_del("to_grow");
	} else {
		for (i = 1; i < snake_len; i++)
			snake[i - 1] =  snake[i];
	}

	poz_viitoare = snake[snake_len - 1];
	muta_unitate(&poz_viitoare, next_dir);

	/* Verific daca sarpele nu va intra in chenar */
	if (gph_is_onborder(&poz_viitoare) == 1 ||
			snk_is_incolision(&poz_viitoare) == 1 ||
			gph_is_onobstacle(&poz_viitoare) == 1) {
		snk_dead();
		return;
	}

	/* Deplasez si primul element al sarpelui */
	muta_unitate(&snake[snake_len - 1], next_dir);

	/* Daca sarpele a mancat mancarea mica */
	if (gph_is_onsmfood(&snake[snake_len - 1]) == 1) {
		flag_del("small_food");
		flag_add("to_grow", 1);
		score_add(POINTS_SMALL);
	}

	/* Daca sarpele a mancat mancarea mare */
	if (gph_is_onbnfood(&snake[snake_len - 1]) == 1) {
		flag_del("draw_bonus");
		score_add(POINTS_BONUS);
	}

	gph_draw(snake, snake_len);
}

/* Verifica daca o unitate nu apartine sarpelui, pentru a detecta coliziunile*/
int snk_is_incolision(struct Unit *u)		/* pointer catre un element */
{
	int i;
	int aparitii;

	aparitii = 0;
	for (i = 0; i < snake_len; i++)
		if (EQ(*u, snake[i]) == 1)
			++aparitii;

	return aparitii;
}

/* Aloca memorie pentru vectori de tip Unit */
static void aloca_mem(struct Unit **u, int *mem)
{
	if (*mem == 0)
		*u = (struct Unit *)malloc((*mem + MEM_INC) *
				sizeof(struct Unit));
	else
		*u = (struct Unit *)realloc(*u,
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

	/* Stupidity check */
	if (flag_has("hard_difficulty") != 0)
		snake_len = LENGTH_HARD;
	else
		snake_len = LENGTH_EASY;

	if (snake_len > MEM_INC) {
		flag_add("fatal_error", 1);
		return;
	}
	
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

/* Adaug directia de miscare */
static void adauga_directie(char dir)
{

	if (dir == UP && next_dir == DOWN);
	else if (dir == DOWN && next_dir == UP);
	else if (dir == LEFT && next_dir == RIGHT);
	else if (dir == RIGHT && next_dir == LEFT);
	else if (dir == UP || dir == DOWN || dir == LEFT || dir == RIGHT)
		next_dir = dir;
}


