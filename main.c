#include <sys/select.h>
#include "generic.h"
#include "flags.h"
#include "graphics.h"
#include "snakes.h"
#include "scores.h"

#define SELECT_ERROR	-1	/* eroare la executia functiei select */
#define SELECT_EVENT	1	/* s-a apasat o tasta */
#define SELECT_TIMEOUT	0	/* timeout la select */
#define SELECT_ERROR	-1	/* fatal error */

#define VI_SEC		0	/* viteza initiala, in secunda */
#define VI_USEC		500000	/* viteza initiala, in microsecunde */

#define VINC_SEC	0	/* cu cat creste viteza de afisaj pe nivel */
#define VINC_USEC	100000	/* cu cat creste viteza de afisaj pe nivel */

#define VMAX_SEC	0	/* viteza de afisaj maxima (in secunde) */
#define VMAX_USEC	100000	/* viteza de afisaj maxima (in microsecunde) */

/* Antet functii */

void error_check(char *msg);

void query_select(int sel, char *key);

void calculate_speed(struct timeval *v);

void return_to_menu();

int main(void)
{
	int nfds;
	int sel;
	char key;
	char *high_name;
	fd_set read_descriptors;
	struct timeval viteza;
	FILE *f;

	/* Resetting debug file */
	f = fopen(DEB_FILE, "w");
	fclose(f);
	f = fopen(DEB_FILE, "a");

	flag_init();
	error_check("INITIALIZING FLAGS (Not enough memory?)");	

	gph_init();
	score_init();

	gph_drwmenu();
	error_check("DRAWING MENU (Terminal too small?)");	
	flag_add("menu_mode", 1);

	/* Initializare select */
	nfds = 1;

	FD_ZERO(&read_descriptors);
	FD_SET(0, &read_descriptors);

	/* Viteza initiala */
	viteza.tv_sec = VI_SEC;
	viteza.tv_usec = VI_USEC;

	while (FOREVER) {
		/* Daca sunt in modul de joc */	
		if (flag_has("game_mode") != 0) {
			/* Ascult tastatura */
			sel = select(nfds, &read_descriptors, NULL, NULL, 
					&viteza);
			query_select(sel, &key);
			error_check("DURING SELECT");

			/* Ies imediat din joc in meniu */
			if (gph_is_quitkey(key) == 1) {
				snk_reset();
				return_to_menu();
			}

			/* Daca nu am apasat q */
			if (flag_has("game_mode") != 0) {
				/* Cat timp nu a expirat timpul */
				while (sel != SELECT_TIMEOUT) {
					/* Ascult tastatura */
					sel = select(nfds, &read_descriptors, 
							NULL, NULL, &viteza);
					query_select(sel, &key);

					/* Ies imediat din joc */
					if (gph_is_quitkey(key) == 1)
						break;

					error_check("DURING SELECT");
				}

				snk_move(key);
				error_check("DRAWING SNAKE (Not enough memory?)");

				if (flag_has("lvlup") != 0)
					flag_del("lvlup");

				FD_SET(0, &read_descriptors);

				calculate_speed(&viteza);	

				if (flag_has("dead") != 0) {
					if (score_get() > score_high()) {
						flag_del("game_mode");
						flag_add("newhigh_mode", 1);
						gph_drwscore();
					} else {
						gph_getkey();
						return_to_menu();
					}
				}
			}
				
		} else if (flag_has("menu_mode") != 0) {
			key = gph_getkey();

			/* Ies din joc */
			if (gph_is_quitkey(key) == 1)
				break;

			if (gph_execute(key) == 1) {

				/* Intru in game_mode */
			       	if (flag_has("showhigh_mode") == 0) {
				       snk_init();
				       error_check("INITIALIZING SNAKE (Not enough memory?)");	
				       flag_add("game_mode", 1);

				/* Sau in showhigh_mode */
			    	} else {
				       gph_drwscore();
			       	}
			}

		} else if (flag_has("showhigh_mode") != 0) {
			key = gph_getkey();

			/* Ies din showhigh_mode si intru in menu_mode */
			if (gph_execute(key) == 1 && flag_has("showhigh_mode") == 0) {
				gph_drwmenu();
				error_check("DRAWING MENU (Terminal too small?)");	
				flag_add("menu_mode", 1);
			}

		} else if (flag_has("newhigh_mode") != 0) {
			high_name = gph_highname();
			score_addhigh(high_name, score_get());

			flag_del("newhigh_mode");
			score_init();
			return_to_menu();
		}

	}

	gph_reset();

	return 0;
}

/* Verifica erorile erorile */
void error_check(char *msg)
{
	if (flag_has("fatal_error") != 0) {
		gph_reset();
		ERROR(msg);
		exit(1);
	}
}

/* Analizeaza rezultatul lui select */
void query_select(int sel, char *key)
{
	if (sel == SELECT_EVENT)
		*key = gph_getkey();
	else if (sel == SELECT_ERROR)
		flag_add("fatal_error", 1);
}

/* Recalculeaza viteza dupa un eveniment select */
void calculate_speed(struct timeval *v)
{
	v->tv_sec = VI_SEC - score_lvl() * VINC_SEC;
	v->tv_usec = VI_USEC - score_lvl() * VINC_USEC;

	if (v->tv_sec < VMAX_SEC)
		v->tv_sec = VMAX_SEC;

	if (v->tv_usec < VMAX_USEC)
		v->tv_usec = VMAX_USEC;
}

/* Obviously */
void return_to_menu()
{
	gph_drwmenu();
	error_check("DRAWING MENU (Terminal too small?)");	

	flag_del("game_mode");
	flag_add("menu_mode", 1);
}
