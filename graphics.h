/* Deseneaza pe ecran grafica jocului*/

#ifndef GRAPHICS_H

#define GRAPHICS_H

#define UP		'w'
#define DOWN		's'
#define LEFT		'a'
#define RIGHT		'd'

#define CWIN_LENX	30	/* lungimea pe x a chenarului */
#define CWIN_LENY	25	/* lungimea pe y a chenarului */

/* Do not use values lower than MWIN_LENX = 40, MWIN_LENY = 20 */
#define MWIN_LENX	40	/* lungimea pe x a ferestrei de meniu */
#define MWIN_LENY	20	/* lungimea pe y a ferestrei de meniu */

#define SCRWIN_LENY	1	/* inaltimea ferestrei de scor */

#define PADDING_HORIZ	1	/* horizontal padding */	
#define PADDING_VERT	1	/* vertical padding */	

#define OBST_LEN	6	/* lungimea obstacolelor */	

#define MKEY_EASY	'1'	/* dificultate meniu usoara (baby-snake) */
#define MKEY_HARD	'2'	/* dificultatea meniu grea (man-snake) */
#define MKEY_QUIT	'q'	/* tasta de iesit din joc */

/* Compara doua unitati */
#define EQ(a, b) ((a).x == (b).x && (a).y == (b).y)

struct Unit {			/* need better name for a snake segment */
	int x, y;		/* coordonatele curente */
};

/* Basic construct-type function */
void gph_init();

/* Deseneaza chenarul in care se poate misca sarpele */
void gph_drwborder();

/* Deseneaza meniul */
void gph_drwmenu();

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
