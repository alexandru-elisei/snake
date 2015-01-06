/*
 * Controloaza miscarea sarpelui pe ecran
 */

#include <stdlib.h>
#include <curses.h>
#include "flags.h"
#include "generic.h"
#include "graphics.h"
#include "snakes.h"

#define START_LENGTH	8	/* lungimea initiala a sarpelui */


static struct Unit *snake;	/* sarpele, duh */
static struct Unit *tp;		/* punctele de intoarcere */
/* Directia in care se va misca sarpele la urmatoarea actualizare a ecranului */
static char next_dir;
static int snk_n, tp_n;		/* lungimile curente ale sarpelui si ale
				 * vectorului de puncte de intoarcere */
static int snk_mem;		/* memoria alocata curent sarpelui */
static int tp_mem;		/* memoria alocata curent tp */

/******************************************************/
static FILE *f;
/******************************************************/


/* Antet functii locale/private */
static void aloca_mem(struct Unit **p, int *mem);
static void adauga_tp(int x, int y, char dir);
static void del_tp();
static void creeaza_sarpe();
static int get_intp(struct Unit *p);
static int coord_egale(struct Unit *u1, struct Unit *u2);
static void muta_unitate(struct Unit *u, char dir);
static void snk_update();

/* Aici trebuie sa inceapa sa se miste sarpele */
void snk_init()
{
	/* Stupidity check */
	if (START_LENGTH > MEM_INC) {
		flag_add("fatal_error", 1);
		return;
	}

	tp_n = 0;
	tp_mem = 0;
	aloca_mem(&tp, &tp_mem);

	/* La pornirea jocului sarpele se misca pe orizontala spre dreapta
	 * incepand cu mijlocul chenarului */
	creeaza_sarpe();

	/* Sarpele isi va continua miscarea */
	next_dir = 0;

	/* Anunt programul ca incep sa ma joc */
	flag_add("playing", 1);
	/* Sanity check */
	flag_del("dead");

	snk_update();
}

/* Adaug miscarea ce se va executa la timer */
void snk_addmv(char dir)
{

	if (dir == UP && snake[snk_n - 1].direction == DOWN);
	else if (dir == DOWN && snake[snk_n - 1].direction == UP);
	else if (dir == LEFT && snake[snk_n - 1].direction == RIGHT);
	else if (dir == RIGHT && snake[snk_n - 1].direction == LEFT);
	else if (dir == UP || dir == DOWN || dir == LEFT || dir == RIGHT)
		next_dir = dir;

	/******************************************************/
	/*
	f = fopen(DEB_FILE, "a");
	fprintf(f, "snk_addmv -> direction %c\n", next_dir);
	fclose(f);
	*/
	/******************************************************/

	/* Temporary */
	snk_update();
}

/* Verifica daca tasta apasata este o directie valida */
int snk_isdir(char c)
{
	/******************************************************/
	/*
	f = fopen(DEB_FILE, "a");
	fprintf(f, "snk_isdir > c = %c\n", c);
	fclose(f);
	*/
	/******************************************************/

	if (c == UP || c == DOWN || c == LEFT || c == RIGHT)
		return 1;

	return 0;
}

/* Destructor-type function */
void snk_reset()
{
	/* Nu mai ma joc */
	flag_del("playing");

	/* Distrug sarpele */
	snk_n = 0;
	snk_mem = 0;
	free(snake);

	/* Distrug punctele de intoarcere */
	tp_n = 0;
	tp_mem = 0;
	free(tp);

	/* Nu mai ma misc */
	next_dir = 0;
}

/* Am murit */
void snk_dead()
{
	/* Resetez sarpele */
	snk_reset();
	flag_add("dead", 1);
	gph_drwborder();
	gph_printcenter("** YOU ARE DEAD **");

	/*****************************************************/
	/*
	f = fopen(DEB_FILE, "a");
	fprintf(f, "\nsnk_dead > %d\n", flag_has("dead"));
	fclose(f);
	*/
	/*****************************************************/
}

/* Genereaza noua pozitie a sarpelui si apoi o deseneaza pe ecran */
static void snk_update()
{
	struct Unit new_snake[snk_n];	/* noua pozitie a sarpelui */
	int index_tp;
	int i;
	FILE *f;

	/* Copiez vechiul sarpe in noul sarpe */
	for (i = 0; i < snk_n; i ++)
		new_snake[i] = snake[i];

	/***************************************************/
	/*
	fprintf(f, "%s", "snk_update - new_snake\n");
	for (i = 0; i < snk_n; i++)
		fprintf(f, "new_snake[%d].x = %d, y = %d, dir = %c\n", i,
				new_snake[i].x, new_snake[i].y, new_snake[i].direction);
	fprintf(f, "\n");
	fflush(f);
	*/
	/***************************************************/

	if (next_dir != 0)
		muta_unitate(&new_snake[snk_n - 1], next_dir);
	else
		muta_unitate(&new_snake[snk_n - 1], snake[snk_n - 1].direction);

	/* Verific daca urmatoarea pozitie e pe chenar */
	if (gph_is_onborder(&new_snake[snk_n - 1]) == 1) {
		snk_dead();
		return;
	}

	

	/* Daca am apasat o tasta si noua directie nu corespunde cu cea veche
	 * inseamna ca am un punct de intoarcere */
	if (next_dir != 0 && snake[snk_n - 1].direction != next_dir)
		adauga_tp(snake[snk_n - 1].x, snake[snk_n - 1].y,
			       next_dir);

	/* Mut sarpele */
	for (i = snk_n - 2; i >= 0; i--) {
		/* Daca un segment al sarpelui se afla pe un punct de 
		 * intoarcere */
		index_tp = get_intp(&snake[i]);
		if (index_tp != -1)
			next_dir = tp[index_tp].direction;
		/* Daca nu se afla, nu isi va schimba directia */
		else
			next_dir = snake[i].direction;	

		/* Mut segmentul curent al sarpelui */
		muta_unitate(&new_snake[i], next_dir);	
	}

	/* Verific daca sarpele nu intra in coliziune cu el insusi */
	if (snk_is_incolision(&new_snake[snk_n - 1], new_snake) == 1) {
		snk_dead();
		return;
	}	

	/***************************************************/
	/*
	fprintf(f, "%s", "snk_update - dupa ce am mutat sarpele new_snake\n");
	for (i = 0; i < snk_n; i++)
		fprintf(f, "new_snake[%d].x = %d, y = %d, dir = %c\n", i,
				new_snake[i].x, new_snake[i].y, new_snake[i].direction);
	fprintf(f, "\n");
	fflush(f);
	*/
	/***************************************************/

	/* Sterg primul punct de intoarcere, noul sarpe deja a trecut de el
	 * complet */
	if (tp_n > 0 && snake[0].x == tp[0].x && snake[0].y == tp[0].y )
		del_tp();

	/* Copiez noul sarpe in cel vechi */
	for (i = 0; i < snk_n; i++)
		snake[i] = new_snake[i];

	/* Desenez sarpele pe ecran */
	gph_drwsnk(snake, snk_n);

	/* Astept input de la user ca sa schimb directia curenta sarpelui */
	next_dir = 0;

	//fclose(f);
}

/* Verifica daca o unitate a sarpelui este punct de intoarcere */
static int get_intp(struct Unit *p)
{
	int i;

	if (tp_n == 0)
		return -1;

	for (i = 0; i < tp_n; i++)
		if (p->x == tp[i].x && p->y == tp[i].y)
			return i;

	return -1;
}

/* Verifica daca o unitate nu apartine sarpelui, pentru a detecta coliziunile*/
int snk_is_incolision(struct Unit *u,		/* pointer catre un element */
		struct Unit *snk)		/* vector */
{
	int i;
	int aparitii;

	aparitii = 0;
	for (i = 0; i < snk_n; i++)
		if (coord_egale(u, &snk[i]) == 1)
			++aparitii;

	/* Daca sunt doua unitati cu coordonatele egale, atunci se
	 * suprapun */
	if (aparitii == 2)
		return 1;
	else
		return 0;
}

/* Adauga un punct de intoarcere, adica un punct unde sarpele isi schimba
 * directia de miscare */
static void adauga_tp(int x, int y, char dir)
{
	if (tp_n == tp_mem)
		aloca_mem(&tp, &tp_mem);

	tp[tp_n].x = x;
	tp[tp_n].y = y;
	tp[tp_n].direction = dir;
	++tp_n;
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

	snk_n = START_LENGTH;
	snk_mem = 0;
	aloca_mem(&snake, &snk_mem);

	gph_getcenter(&x, &y);
	x -= 2;
	for (i = 0; i < snk_n; i++) {
		snake[i].x = x + i;
		snake[i].y = y;
		snake[i].direction = RIGHT;
	}

	/***********************************************************/
	/*
	f = fopen(DEB_FILE, "a");
	fprintf(f, "%s", "creeaza_sarpe -> sarpele initial:\n");
	for (i = 0; i < snk_n; i++)
		fprintf(f, "snake[%d].x = %d, y = %d, dir = %c\n", i,
				snake[i].x, snake[i].y, snake[i].direction);
	fflush(f);
	fclose(f);
	*/
	/***********************************************************/
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

	u->direction = dir;
}

/* Sterge primul element al vectorului de puncte de intoarcere */
static void del_tp()
{
	int i;

	for (i = 1; i < tp_n; i++)
		tp[i - 1] = tp[i];

	--tp_n;
}

/* Verifica daca doua unitati, trimise ca pointer, au aceleasi coordonate */
static int coord_egale(struct Unit *u1, struct Unit *u2)
{
	if (u1->x == u2->x && u1->y == u2->y)
		return 1;

	return 0;
}
