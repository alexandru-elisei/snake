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

/* Deseneaza fereastra in care se arata highscore */
void gph_drwscore();

/* Returneaza 1 daca un punct se afla pe bordaj */
int gph_is_onborder(struct Unit *p);

/* Exits graphic mode */
void gph_reset();

/* Prints a text in the middle of the border */
void gph_printcenter(char *msg);

/* Afla pozitia din centrul ferestre */
void gph_getcenter(int *x, int *y);

/* Citeste o tasta */
char gph_getkey();

/* Citeste numele la highscore */
char *gph_highname();

/* Returneaza 1 daca am apasat quit */
int gph_is_quitkey(char key);

/* Executa o actiune inafara modului de joc */
int gph_execute(char key);

/* Deseneaza un sarpe */
void gph_draw(struct Unit *snake, int snk_n);

/* Detecteaza daca sarpele a mancat mancarea normala */
int gph_is_onsmfood(struct Unit *u);

/* Detecteaza daca sarpele a mancat mancarea bonus */
int gph_is_onbnfood(struct Unit *u);

/* Creeaza mancarea bonus */
void gph_genbonus();

/* Sterge mancarea bonus */
void gph_resetbonus();

/* Detecteaza daca ma suprapun pe un obstacol */
int gph_is_onobstacle(struct Unit *u);

#endif		/* ifndef GRAPHICS_H */
