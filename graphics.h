/* Deseneaza pe ecran grafica jocului*/

#ifndef GRAPHICS_H

#define GRAPHICS_H

#define UP		'w'
#define DOWN		's'
#define LEFT		'a'
#define RIGHT		'd'


/* Compara doua unitati */
#define EQ(a, b) ((a).x == (b).x && (a).y == (b).y)

struct Unit {			/* need better name for a snake segment */
	int x, y;		/* coordonatele curente */
};

/* Basic construct-type function */
void gph_init();

/* Deseneaza chenarul in care se poate misca sarpele */
void gph_drwgame();

/* Deseneaza meniul */
void gph_drwmenu();

/* Deseneaza fereastra de scor */
void gph_drwscore();

/* Returneaza 1 daca un punct se afla pe bordaj */
int gph_is_onborder(struct Unit *p);

/* Exits graphic mode */
void gph_reset();

/* Prints a text in the middle of the border */
void gph_printcenter(char *msg);

/* Afla pozitia din centrul ferestre */
void gph_getcenter(int *x, int *y);

/* Citeste o directie */
char gph_getkey();

/* Returneaza 1 daca o tasta e tasta valida pentru meniu */
int gph_is_menukey(char key);

/* Returneaza 1 daca am apasat quit */
int gph_is_quitkey(char key);

/* Executa o actiune de meniu */
void gph_menuact(char key);

/* Deseneaza un sarpe */
void gph_draw(struct Unit *snake, int snk_n);

/* Detects if the snake ate the small food */
int gph_is_onsmfood(struct Unit *u);

/* Detects if the an item bumped into an obstacle */
int gph_is_onobstacle(struct Unit *u);

#endif		/* ifndef GRAPHICS_H */
