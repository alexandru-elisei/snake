/* Deseneaza pe ecran grafica jocului */

#include <time.h>
#include "flags.h"
#include "snakes.h"
#include "graphics.h"
#include "generic.h"

#define CWIN_LENX	30	/* lungimea pe x a chenarului */
#define CWIN_LENY	20	/* lungimea pe y a chenarului */
#define SCRWIN_LENY	1	/* inaltimea ferestrei de scor */
#define MENUWIN_LENY	1	/* inaltimea ferestrei de menu */
#define PADDING_HORIZ	1	/* horizontal padding */	
#define PADDING_VERT	1	/* vertical padding */	
#define OBST_LEN	6	/* lungimea obstacolelor */	

struct Chenar {		
	WINDOW *wnd;		/* fereastra in care se misca sarpele */
	int startx;		/* abscisa bordajului */
	int starty;		/* coordonata bordajului */
};

static struct Chenar chenar;	/* chenarul */
static struct Unit small_food;	/* mancarea cu scorul cel mai mic */
static struct Unit obst1[OBST_LEN],	/* obstacolele */
		   obst2[OBST_LEN];	/* oamenii care incearca sa prinda sarpele */
static WINDOW *scrwin;		/* score window */
static WINDOW *menuwin;		/* menu window */

/*********************************************/
static FILE *f;
/*********************************************/

/* Antet functii locale/private */

static void destroy_window(WINDOW *win);

static void gen_small_food(struct Unit *food);

static void gen_obstacles(struct Unit *o);	/* pointer de unitati */

/* Basic construct-type function */
void gph_init()
{
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	curs_set(FALSE);
	clear();

	/* Daca pot folosit culori */
	if (has_colors() == TRUE) {
		flag_add("color", 1);
		start_color();
	}

	chenar.wnd = NULL;
	chenar.startx = -1;
	chenar.starty = -1;

	scrwin = NULL;
	menuwin = NULL;
}

/* Deseneaza chenarul in care se poate misca sarpele */
void gph_drwborder()
{
	/* Ma asigur ca am spatiu sa desenez si bara de scor si de meniu */
	if (LINES < (CWIN_LENY + SCRWIN_LENY + MENUWIN_LENY + PADDING_VERT * 2)
		       	|| COLS < (CWIN_LENX + PADDING_HORIZ * 2)) {
		flag_add("fatal_error", 1);
		return;
	}

	chenar.starty = (LINES - CWIN_LENY) / 2;
	chenar.startx = (COLS - CWIN_LENX) / 2;

	chenar.wnd = newwin(CWIN_LENY, CWIN_LENX, chenar.starty, chenar.startx);

	//init_color(COLOR_WOOD, 255, 211, 155);
	init_pair(1, COLOR_RED, COLOR_BLUE);
	init_pair(2, COLOR_BLACK, COLOR_YELLOW);

	wattron(chenar.wnd, COLOR_PAIR(1));
	wborder(chenar.wnd,  '+',  '+', '+', '+', '+', '+', '+', '+');
	wattroff(chenar.wnd, COLOR_PAIR(1));

	wrefresh(chenar.wnd);
}

/* Returneaza 1 daca un punct se afla pe bordaj */
int gph_is_onborder(struct Unit *p)
{
	if (p->x == 0 || p->y == 0 ||
			p->x == (CWIN_LENX - 1) ||
			p->y == (CWIN_LENY - 1))
		return 1;

	return 0;
}

/* Afla pozitia din centrul ferestrei */
void gph_getcenter(int *x, int *y)
{
	*x = CWIN_LENX / 2;
	*y = CWIN_LENY / 2;
}

/* Printeaza un text in mijlocul ecranului */
void gph_printcenter(char *msg)
{
	int startx, starty;

	gph_getcenter(&startx, &starty);
	startx = startx - strlen(msg) / 2;

	mvwprintw(chenar.wnd, starty, startx, "%s", msg);
	wrefresh(chenar.wnd);
}

/* Citeste o tasta in functie de modul curent */
char gph_getkey()
{
	char ret;

	if (flag_has("game_mode") != 0)
		ret = tolower(wgetch(chenar.wnd));

	return ret;
}

/* Deseneaza un sarpe */
void gph_drwsnk(struct Unit *snake, int snk_n)
{
	int i;

	gph_drwborder();

	for (i = 0; i < snk_n; i++) {
		mvwprintw(chenar.wnd, snake[i].y, snake[i].x, "%c", '*');
	}

	srand(time(NULL));

	if (flag_has("small_food") == 0) {
		gen_small_food(&small_food);
		flag_add("small_food", 1);
	}

	mvwprintw(chenar.wnd, small_food.y, small_food.x, "%c", '0');

	if (flag_has("obstacles") == 0) {
		gen_obstacles(obst1);
		flag_add("obstacles", 1);

		gen_obstacles(obst2);
		flag_add("obstacles", 2);
	}

	for (i = 0; i < OBST_LEN; i++) {
		mvwprintw(chenar.wnd, obst1[i].y, obst1[i].x, "%c", '+');
		mvwprintw(chenar.wnd, obst2[i].y, obst2[i].x, "%c", '+');
	}

	wrefresh(chenar.wnd);
}

/* Detects if the snake ate the small food */
int gph_is_onsmfood(struct Unit *u)
{
	/*
	f = fopen(DEB_FILE, "a");
	fprintf(f, "\n\ngph_is_onsmfood; u.x = %d, u.y = %d, sf.x = %d, sf.y = %d\n",
			u->x, u->y, small_food.x, small_food.y);
	fflush(f);
	*/
	if (flag_has("small_food") == 0)
		return 0;

	//fprintf(f, "%s\n", "I have the small food flag.");
	//fflush(f);

	if (gph_is_eq(u, &small_food) == 1)
		return 1;

	//fprintf(f, "%s\n", "u is not equal to small_food.");
	//fflush(f);

	return 0;
}

/* Detects if an item bumped into an obstacle */
int gph_is_onobstacle(struct Unit *u)
{
	int i;

	if (flag_has("obstacles") == 0)
		return 0;

	if (flag_has("obstacles") != 0) {
		for (i = 0; i < OBST_LEN; i++)
			if (gph_is_eq(u, &obst1[i]) == 1)
				return 1;
		if (flag_has("obstacles") == 2)
			for (i = 0; i < OBST_LEN; i++)
				if (gph_is_eq(u, &obst2[i]) == 1)
					return 1;
	}

	return 0;
}


void gph_reset()
{
	destroy_window(chenar.wnd);
	destroy_window(scrwin);
	destroy_window(menuwin);

	endwin();
}

/* Verifica daca doua unitati, trimise ca pointer, au aceleasi coordonate */
int gph_is_eq(struct Unit *u1, struct Unit *u2)
{
	if (u1->x == u2->x && u1->y == u2->y)
		return 1;

	return 0;
}

static void destroy_window(WINDOW *win)
{
	if (win != NULL) {
		wborder(win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
		wrefresh(win);
		delwin(win);
	}
}

/* Genereaza un punct normal pentru sarpe de mancat */
static void gen_small_food(struct Unit *food)
{
	do {
		food->x = rand() % (CWIN_LENX - 2) + 1;
		food->y = rand() % (CWIN_LENY - 2) + 1;
	} while (snk_is_incolision(food) == 1 ||
			gph_is_onborder(food) == 1);
}

/* Genereaza obstacolele pe ecran */
static void gen_obstacles(struct Unit *o)
{
	int valid;
	struct Unit centru;
	int obst_oriz;		/* daca obstacolulul va fi orizontal (==1) */
	int i;

	valid = 0;
	while (valid == 0) {
		valid = 1;

		obst_oriz = rand() % 2;

		/* Obstacolul are lungimea de OBST_LEN caractere, cu spatiu in
		 * mijloc. Daca e orizontal centrul cel mai din stanga are 
		 * x = 3 si cel mai din dreapta x = CWIN_LENX - 4 */
		if (obst_oriz == 1) {
			centru.x = rand() % (CWIN_LENX - OBST_LEN - 2) + OBST_LEN / 2 + 1;
			centru.y = rand() % (CWIN_LENY - 2) + 1;

			for (i = OBST_LEN / 2; i > 0; i--) {
				o[OBST_LEN / 2 - i].x = centru.x - i;
				o[OBST_LEN / 2 - i].y = centru.y;
			}

			/* Am spatiu intre cele doua ramuri ale obstacolului */
			for (i = OBST_LEN / 2; i < OBST_LEN; i++) {
				o[i].x = centru.x + i - 1;
				o[i].y = centru.y;
			}
		} else {
			centru.x = rand() % (CWIN_LENX - 2) + 1;
			centru.y = rand() % (CWIN_LENY - OBST_LEN - 2) + OBST_LEN / 2 + 1;

			for (i = OBST_LEN / 2; i > 0; i--) {
				o[OBST_LEN / 2 - i].y = centru.y - i;
				o[OBST_LEN / 2 - i].x = centru.x;
			}

			/* Am spatiu intre cele doua ramuri ale obstacolului */
			for (i = OBST_LEN / 2; i < OBST_LEN; i++) {
				o[i].y = centru.y + i - 1;
				o[i].x = centru.x;
			}
		}

		/* Verific validitatea obstacolului */
		for (i = 0; i < OBST_LEN; i ++) {
			if (gph_is_onborder(&o[i]) == 1 ||
					gph_is_onsmfood(&o[i]) == 1 ||
					snk_is_incolision(&o[i])) {
				valid = 0;
				break;
			}

			if (flag_has("obstacles") == 1 &&
				       gph_is_onobstacle(&o[i]) == 1) {
				valid = 0;
				break;
			}
		}
	}
}
