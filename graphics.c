/*
 * Deseneaza pe ecran grafica jocului
 */

#include <curses.h>
#include <string.h>
#include <ctype.h>
#include "flags.h"
#include "graphics.h"
#include "generic.h"

#define BWIN_LENX	40	/* lungimea pe x a chenarului */
#define BWIN_LENY	20	/* lungimea pe y a chenarului */
#define SCRWIN_LENY	1	/* inaltimea ferestrei de scor */
#define MENUWIN_LENY	1	/* inaltimea ferestrei de menu */
#define PADDING_HORIZ	1	/* horizontal padding */	
#define PADDING_VERT	1	/* vertical padding */	

static WINDOW *bwin;		/* border window */
static WINDOW *scrwin;		/* score window */
static WINDOW *menuwin;		/* menu window */
static int bwin_startx;		/* abscisa bordajului */
static int bwin_starty;		/* coordonata bordajului */

/*********************************************/
static FILE *f;
/*********************************************/

/* Antet functii locale/private */

static void destroy_window(WINDOW *win);

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

	/* Initializez pointerii catre ferestre */
	bwin = NULL;
	scrwin = NULL;
	menuwin = NULL;

	/* Initializez coordonatele bordajului */
	bwin_startx = -1;
	bwin_starty = -1;
}

/* Deseneaza chenarul in care se poate misca sarpele */
void gph_drwborder()
{
	/* Ma asigur ca am spatiu sa desenez si bara de scor si de meniu */
	if (LINES < (BWIN_LENY + SCRWIN_LENY + MENUWIN_LENY + PADDING_VERT * 2)
		       	|| COLS < (BWIN_LENX + PADDING_HORIZ * 2)) {
		flag_add("fatal_error", 1);
		return;
	}

	bwin_starty = (LINES - BWIN_LENY) / 2;
	bwin_startx = (COLS - BWIN_LENX) / 2;

	bwin = newwin(BWIN_LENY, BWIN_LENX, bwin_starty, bwin_startx);

	//init_color(COLOR_WOOD, 255, 211, 155);
	init_pair(1, COLOR_RED, COLOR_BLUE);
	init_pair(2, COLOR_BLACK, COLOR_YELLOW);

	wattron(bwin, COLOR_PAIR(1));
	wborder(bwin,  '+',  '+', '+', '+', '+', '+', '+', '+');
	wattroff(bwin, COLOR_PAIR(1));

	wrefresh(bwin);
}

/* Returneaza 1 daca un punct se afla pe bordaj */
int gph_is_onborder(struct Unit *p)
{
	if (p->x == 0 || p->y == 0 ||
			p->x == (BWIN_LENX - 1) ||
			p->y == (BWIN_LENY - 1))
		return 1;

	return 0;
}

/* Afla pozitia din centrul ferestrei */
void gph_getcenter(int *x, int *y)
{
	*x = BWIN_LENX / 2;
	*y = BWIN_LENY / 2;
}

/* Printeaza un text in mijlocul ecranului */
void gph_printcenter(char *msg)
{
	int startx, starty;

	gph_getcenter(&startx, &starty);
	startx = startx - strlen(msg) / 2;

	mvwprintw(bwin, starty, startx, "%s", msg);
	wrefresh(bwin);
}

/* Citeste o directie */
char gph_getkey()
{
	char ret;

	if (flag_has("playing") != 0)
		ret = tolower(wgetch(bwin));

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
		mvwprintw(bwin,
			snake[i].y,
			snake[i].x,
			"%c", '*');
	}

	/* O opresc */
	//wattroff(bwin, COLOR_PAIR(1));
	/* Desenez schimbarile pe ecran */
	wrefresh(bwin);
}

void gph_reset()
{
	destroy_window(bwin);
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

