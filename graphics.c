/*
 * Deseneaza pe ecran grafica jocului
 */

#include <time.h>
#include "flags.h"
#include "snakes.h"
#include "graphics.h"
#include "generic.h"

#define CWIN_LENX	40	/* lungimea pe x a chenarului */
#define CWIN_LENY	20	/* lungimea pe y a chenarului */
#define SCRWIN_LENY	1	/* inaltimea ferestrei de scor */
#define MENUWIN_LENY	1	/* inaltimea ferestrei de menu */
#define PADDING_HORIZ	1	/* horizontal padding */	
#define PADDING_VERT	1	/* vertical padding */	

struct Chenar {		
	WINDOW *wnd;		/* fereastra in care se misca sarpele */
	int startx;		/* abscisa bordajului */
	int starty;		/* coordonata bordajului */
};

static struct Chenar chenar;	/* chenarul */
static struct Unit small_food;		/* mancarea cu scorul cel mai mic */
static WINDOW *scrwin;		/* score window */
static WINDOW *menuwin;		/* menu window */

/*********************************************/
static FILE *f;
/*********************************************/

/* Antet functii locale/private */

static void destroy_window(WINDOW *win);
static void gen_small_food(struct Unit *food);

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

	/* Setez culoarea verde pentru snake */
	/*
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	wattron(bwin, COLOR_PAIR(1));
	*/

	for (i = 0; i < snk_n; i++) {
		mvwprintw(chenar.wnd, snake[i].y, snake[i].x, "%c", '*');
	}

	if (flag_has("small_food") == 0) {
		gen_small_food(&small_food);
		flag_add("small_food", 1);
	}

	wattron(chenar.wnd, A_BLINK);
	mvwprintw(chenar.wnd, small_food.y, small_food.y, "%c", '0');
	wattroff(chenar.wnd, A_BLINK);


	/* O opresc */
	//wattroff(bwin, COLOR_PAIR(1));
	/* Desenez schimbarile pe ecran */
	wrefresh(chenar.wnd);
}

void gph_reset()
{
	destroy_window(chenar.wnd);
	destroy_window(scrwin);
	destroy_window(menuwin);

	endwin();
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
	srand(time(NULL));

	do {
		food->x = rand() % (CWIN_LENX - 2) + 1;
		food->y = rand() % (CWIN_LENY - 2) + 1;
	} while (snk_is_incolision(food) == 1 ||
			gph_is_onborder(food) == 1);
}
