
/* Deseneaza pe ecran grafica jocului */

#include <time.h>
#include "flags.h"
#include "snakes.h"
#include "graphics.h"
#include "generic.h"
#include "scores.h"

#define CHENAR_LENX	25	/* lungimea pe x a chenarului */
#define CHENAR_LENY	20	/* lungimea pe y a chenarului */

#define CHENAR_CHX	'+'	/* caracterul pe x pentru chenar */
#define CHENAR_CHY	'+'	/* caracterul pe y pentru chenar */

/* Do not use values lower than MENU_LENX = 40, MENU_LENY = 21 */
#define MENU_LENX	40	/* lungimea pe x a ferestrei de meniu */
#define MENU_LENY	25	/* lungimea pe y a ferestrei de meniu */

/* Do not use values lower than SCORE_LENY = 7 */
#define SCORE_LENY	7	/* lungimea pe y a ferestrei de meniu */

#define SCRBAR_LENY	1	/* inaltimea barei de scor */
#define MENUBAR_LENY	1	/* inaltimea barei de meniu */

#define PADDING_HORIZ	1	/* horizontal padding */	
#define PADDING_VERT	1	/* vertical padding */	

#define OBST_LEN	6	/* lungimea obstacolelor */	
#define SCORE_LEN	14	/* lungimea textului scor */	

#define MKEY_EASY	'1'	/* dificultate meniu usoara (baby-snake) */
#define MKEY_HARD	'2'	/* dificultatea meniu grea (man-snake) */
#define MKEY_QUIT	'q'	/* tasta de iesit din joc */
#define MKEY_HIGH	's'	/* tasta de vizualizat scorul */
#define SKEY_RETURN	'r'	/* tasta de iesit din fereastra de scor */

struct MenuWin {		
	WINDOW *win;		/* fereastra in care se afiseaza meniul */
	int startx, starty;	/* coordonatele coltului stanga-sus */
};

struct GameWin {		
	WINDOW *win;		/* fereastra in care se desfasoara jocul */
	int startx, starty;	/* coordonatele coltului stanga-sus */
	int dimx, dimy;		/* dimensiunile ferestrei */
	int chenar_startx;	/* coordonatele stanga-sus ale chenarului */
       	int chenar_starty;	
};

static struct GameWin game;	/* fereastra de joc */
static struct Unit small_food;	/* mancarea cu scorul cel mai mic */
static struct Unit obst1[OBST_LEN],	/* obstacolele */
		   obst2[OBST_LEN];	/* oamenii care incearca sa prinda 
					 * sarpele */
static struct MenuWin menu;	/* menu window */
static struct MenuWin score;	/* score window */

/*********************************************/
static FILE *f;
/*********************************************/

/* Antet functii locale/private */

static void draw_border();

static void draw_scorebar();

static void draw_menubar();

static void destroy_window(WINDOW **w);

static int check_terminal_size(int lenx, int leny);

static void gen_small_food(struct Unit *food);

static void gen_obstacles(struct Unit *o);	/* pointer de unitati */

/* Basic constructor-type function */
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

	destroy_window(&game.win);
	destroy_window(&menu.win);
	destroy_window(&score.win);


}

/* Deseneaza chenarul in care se poate misca sarpele */
void gph_drwgame()
{
	game.dimx = CHENAR_LENX;
	game.dimy = CHENAR_LENY + SCRBAR_LENY + MENUBAR_LENY;

	if (check_terminal_size(game.dimx + 2 * PADDING_HORIZ,
			       game.dimy + 2 * PADDING_VERT) == 0) {
		flag_add("fatal_error", 1);
		return;
	}

	destroy_window(&menu.win);

	game.starty = (LINES - game.dimy) / 2;
	game.startx = (COLS - game.dimx) / 2;
	game.win = newwin(game.dimy, game.dimx, game.starty, game.startx);

	game.chenar_startx = 0;
	/* Las spatiu deasupra chenarului sa scriu scorul */
	game.chenar_starty = SCRBAR_LENY;

	draw_border();	
	draw_scorebar();
	draw_menubar();

	wrefresh(game.win);
}

/* Deseneaza meniul */
void gph_drwmenu()
{
	int centru_x;

	if (check_terminal_size(MENU_LENX, MENU_LENY) == 0) {
		flag_add("fatal_error", 1);
		return;
	}

	destroy_window(&game.win);
	destroy_window(&score.win);

	menu.starty = (LINES - MENU_LENY) / 2;
	menu.startx = (COLS - MENU_LENX) / 2;
	menu.win = newwin(MENU_LENY, MENU_LENX, menu.starty, menu.startx);

	centru_x = (MENU_LENX - strlen(TITLU)) / 2;
	wattron(menu.win, A_BOLD); 
	mvwprintw(menu.win, 0, centru_x, "%s\n", TITLU);
	wattroff(menu.win, A_BOLD);

	mvwprintw(menu.win, 2, 0, "Life has been kind to you. You have your");
	mvwprintw(menu.win, 3, 0, "own backyard, the mice are plentiful,");
	mvwprintw(menu.win, 4, 0, "and sometimes, if you are lucky, you");
	mvwprintw(menu.win, 5, 0, "catch a rabbit. A pink, fluffly rabbit.");
	mvwprintw(menu.win, 6, 0, "A very tasy, pink, fluffy rabbit.");

	mvwprintw(menu.win, 8, 0, "...Until one day. When the pesky, nasty,");
	mvwprintw(menu.win, 9, 0, "bipedal humans saw you in your yard and");
	mvwprintw(menu.win, 10, 0, "they tried to stop you.");

	mvwprintw(menu.win, 12, 0, "Will you manage to ESCAPE THE BACKYARD?");

	mvwprintw(menu.win, 14, 0, "Choose your game mode:");
	mvwprintw(menu.win, 15, 0, "(%c) Baby-snake mode.", MKEY_EASY);
	mvwprintw(menu.win, 16, 0, "(%c) Man-snake mode.", MKEY_HARD);
	mvwprintw(menu.win, 17, 0, "(%c) to view highscore.", MKEY_HIGH);
	mvwprintw(menu.win, 18, 0, "(%c) to quit game.", MKEY_QUIT);

	mvwprintw(menu.win, 20, 0, "");

	nocbreak();
	echo();
	curs_set(TRUE);

	wrefresh(menu.win);
}

/* Deseneaza fereastra de scor */
void gph_drwscore()
{
	char titlu[] = "** HIGH SCORE **";
	
	if (check_terminal_size(CHENAR_LENX, SCORE_LENY) == 0) {
		flag_add("fatal_error", 1);
		return;
	}

	score.startx = (COLS - CHENAR_LENX) / 2;
	score.starty = (LINES - SCORE_LENY) / 2;
	score.win = newwin(SCORE_LENY, CHENAR_LENX, score.starty, score.startx);

	wclear(score.win);

	if (flag_has("color") != 0) {
		init_pair(1, COLOR_RED, COLOR_BLACK);
		wattron(score.win, COLOR_PAIR(1)|A_BOLD);
	}

	mvwprintw(score.win, 1, (CHENAR_LENX - strlen(titlu)) / 2, "%s", titlu);
	mvwprintw(score.win, 3, 2, "%s: %d", score_highname(), score_high()); 

	if (flag_has("color") != 0)
		wattroff(score.win, COLOR_PAIR(1)|A_BOLD);

	mvwprintw(score.win, 5, 2, "(%c) to return to menu", SKEY_RETURN);

	cbreak();
	noecho();
	curs_set(FALSE);

	wrefresh(score.win);
}

/* Returneaza 1 daca un punct se afla pe bordaj */
int gph_is_onborder(struct Unit *p)
{
	if (p->x == game.chenar_startx ||
		      	p->y == game.chenar_starty ||
			p->x == (game.chenar_startx + CHENAR_LENX - 1) ||
			p->y == (game.chenar_starty + CHENAR_LENY - 1))
		return 1;

	return 0;
}

/* Afla pozitia din centrul ferestrei */
void gph_getcenter(int *x, int *y)
{
	*x = (game.chenar_startx + CHENAR_LENX) / 2;
	*y = (game.chenar_starty + CHENAR_LENY) / 2;
}

/* Printeaza un text in mijlocul ecranului */
void gph_printcenter(char *msg)
{
	int startx, starty;

	gph_getcenter(&startx, &starty);
	startx = startx - strlen(msg) / 2;

	mvwprintw(game.win, starty, startx, "%s", msg);
	wrefresh(game.win);
}

/* Citeste o tasta in functie de modul curent */
char gph_getkey()
{
	char ret;
	char buffer[2];
	int y, x;
	
	if (flag_has("game_mode") != 0)
		ret = tolower(wgetch(game.win));
	else if (flag_has("menu_mode") != 0) {
		wgetnstr(menu.win, buffer, 1);
		ret = tolower(buffer[0]);
		getyx(menu.win, y, x);
		mvwdelch(menu.win, y -1, 0);
	} else if (flag_has("showhigh_mode") != 0) {
		ret = tolower(wgetch(score.win));
	}

	return ret;
} 


/* Returneaza 1 daca am apasat quit */
int gph_is_quitkey(char key)
{
	key = tolower(key);
	if (key == MKEY_QUIT)
		return 1;

	return 0;
}

/* Executa o actiune inafara modului de joc, astfel daca returneaza 1 intru in
 * modul de joc, daca returneaza 2 intru in modul showhigh */
int gph_execute(char key)
{
	if (flag_has("menu_mode") != 0) {
		if (key == MKEY_EASY) {
			flag_del("hard_difficulty");
			flag_add("easy_difficulty", 1);

			/* Entering game mode */
			flag_del("menu_mode");
			flag_add("game_mode", 1);
		} else if (key == MKEY_HARD) {
			flag_del("easy_difficulty");
			flag_add("hard_difficulty", 1);

			/* Entering game mode */
			flag_del("menu_mode");
			flag_add("game_mode", 1);
		} else if (key == MKEY_HIGH) {
			flag_del("menu_mode");
			flag_add("showhigh_mode", 1);
		}

		return 1;
	} 
	
	if (flag_has("showhigh_mode") != 0) {
		if (key == SKEY_RETURN)
			flag_del("showhigh_mode");

		return 1;
	}

	return 0;
}

/* Deseneaza un sarpe */
void gph_draw(struct Unit *snake, int snk_n)
{
	int i;

	gph_drwgame();

	for (i = 0; i < snk_n; i++) {
		mvwprintw(game.win, snake[i].y, snake[i].x, "%c", '*');
	}

	srand(time(NULL));

	if (flag_has("small_food") == 0) {
		gen_small_food(&small_food);
		flag_add("small_food", 1);
	}

	mvwprintw(game.win, small_food.y, small_food.x, "%c", '0');

	if (flag_has("hard_difficulty") != 0) {
		if (flag_has("obstacles") == 0 && flag_has("hard_difficulty") != 0) {
			gen_obstacles(obst1);
			flag_add("obstacles", 1);

			gen_obstacles(obst2);
			flag_add("obstacles", 2);
		}

		for (i = 0; i < OBST_LEN; i++) {
			mvwprintw(game.win, obst1[i].y, obst1[i].x, "%c", '+');
			mvwprintw(game.win, obst2[i].y, obst2[i].x, "%c", '+');
		}
	}

	wrefresh(game.win);
}

/* Detects if the snake ate the small food */
int gph_is_onsmfood(struct Unit *u)
{
	if (flag_has("small_food") == 0)
		return 0;

	if (EQ(*u, small_food) == 1)
		return 1;

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
			if (EQ(*u, obst1[i]) == 1)
				return 1;
		if (flag_has("obstacles") == 2)
			for (i = 0; i < OBST_LEN; i++)
				if (EQ(*u, obst2[i]) == 1)
					return 1;
	}

	return 0;
}


void gph_reset()
{
	destroy_window(&game.win);
	destroy_window(&menu.win);

	endwin();
}

static void destroy_window(WINDOW **w)
{
	if (*w != NULL) {
		wclear(*w);
		wrefresh(*w);
		delwin(*w);
		*w = NULL;
	}
}

/* Genereaza un punct normal pentru sarpe de mancat */
static void gen_small_food(struct Unit *food)
{
	do {
		food->x = rand() % (CHENAR_LENX - 2) + 1;
		food->y = rand() % (CHENAR_LENY - 2) + 1;
	} while (snk_is_incolision(food) == 1 ||
			gph_is_onborder(food) == 1 ||
			gph_is_onobstacle(food) == 1);
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
		 * x = 3 si cel mai din dreapta x = CHENAR_LENX - 4 */
		if (obst_oriz == 1) {
			centru.x = rand() % (CHENAR_LENX - OBST_LEN - 2) + OBST_LEN / 2 + 1;
			centru.y = rand() % (CHENAR_LENY - 2) + 1;

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
			centru.x = rand() % (CHENAR_LENX - 2) + 1;
			centru.y = rand() % (CHENAR_LENY - OBST_LEN - 2) + OBST_LEN / 2 + 1;

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

/* Verific daca terminalul este indeajuns de mare incat sa desenez pe el */
static int check_terminal_size(int lenx, int leny)
{

	/* Ma asigur ca am spatiu sa desenez si bara de scor si de meniu */
	if (LINES < leny || COLS < lenx)
		return 0;

	return 1;
}

/* Deseneaza chenarul */
static void draw_border()
{
	if (flag_has("color") != 0) {
		init_pair(1, COLOR_RED, COLOR_CYAN);
		wattron(game.win, COLOR_PAIR(1));
	}

	/* Desenez liniile orizontale */
	mvwhline(game.win, game.chenar_starty, game.chenar_startx,
		       	CHENAR_CHX, CHENAR_LENX);
	mvwhline(game.win, game.chenar_starty + CHENAR_LENY - 1, game.chenar_startx,
			CHENAR_CHX, CHENAR_LENX);

	/* Desenez liniile verticale */
	mvwvline(game.win, game.chenar_starty + 1, game.chenar_startx,
			CHENAR_CHY, CHENAR_LENY - 2);
	mvwvline(game.win, game.chenar_starty + 1, game.chenar_startx + CHENAR_LENX - 1,
			CHENAR_CHY, CHENAR_LENY - 2);

	if (flag_has("color") != 0) 
		wattroff(game.win, COLOR_PAIR(1));
}

/* Printeaza scorul curent */
static void draw_scorebar()
{
	mvwprintw(game.win, 0, 0, "LEVEL: %d", score_lvl());
	mvwprintw(game.win, 0, game.dimx - SCORE_LEN, "SCORE: %d", score_get());
}

static void draw_menubar(struct MenuWin m)
{
	mvwprintw(game.win, game.dimy - 1, 0, "Q: EXIT GAME");
}

