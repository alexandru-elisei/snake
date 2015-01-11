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

int main(void)
{
	int nfds;
	int sel;
	char key;
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
		
		if (flag_has("game_mode") != 0) {
			/* Ascult tastatura */
			sel = select(nfds, &read_descriptors, NULL, NULL, 
					&viteza);
			query_select(sel, &key);

			/* Ies imediat din joc */
			if (gph_is_quitkey(key) == 1)
				break;

			error_check("DURING SELECT");

			/* Cat timp nu a expirat timpul */
			while (sel != SELECT_TIMEOUT) {
				/* Ascult tastatura */
				sel = select(nfds, &read_descriptors, NULL, NULL,
					       	&viteza);
				query_select(sel, &key);

				/* Ies imediat din joc */
				if (gph_is_quitkey(key) == 1)
					break;

				error_check("DURING SELECT");
			}

			if (flag_has("dead") != 0) {
				flag_del("game_mode");
				getchar();
				break;
			}

			snk_addmv(key);
			snk_move();
			error_check("DRAWING SNAKE (Not enough memory?)");

			if (flag_has("lvlup") != 0) {
				fprintf(f, "next level, yay!\n");
				flag_del("lvlup");
			}

			FD_SET(0, &read_descriptors);

			viteza.tv_sec = VI_SEC - score_lvl() * VINC_SEC;
			viteza.tv_usec = VI_USEC - score_lvl() * VINC_USEC;

			if (viteza.tv_sec < VMAX_SEC)
				viteza.tv_sec = VMAX_SEC;

			if (viteza.tv_usec < VMAX_USEC)
				viteza.tv_usec = VMAX_USEC;
				
		} else if (flag_has("menu_mode") != 0) {
			key = gph_getkey();

			/* Ies din joc */
			if (gph_is_quitkey(key) == 1)
				break;

			/* Intru in game_mode */
			if (gph_execute(key) == 1) {
				snk_init();
				error_check("INITIALIZING SNAKE (Not enough memory?)");	
			}
		} else if (flag_has("showhigh_mode") != 0) {
			/*
			fprintf(f, "entering showhigh mode - before gph_drwscore\n");
			fflush(f);
			*/
			gph_drwscore();

			key = gph_getkey();
			gph_execute(key);
			/*
			fprintf(f, "entering showhigh mode\n");
			fflush(f);
			*/
		}

	}

	gph_reset();

	return 0;
}

void error_check(char *msg)
{
	if (flag_has("fatal_error") != 0) {
		gph_reset();
		ERROR(msg);
		exit(1);
	}
}

void query_select(int sel, char *key)
{
	if (sel == SELECT_EVENT)
		*key = gph_getkey();
	else if (sel == SELECT_ERROR)
		flag_add("fatal_error", 1);
}
