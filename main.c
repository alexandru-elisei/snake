#include <sys/select.h>
#include "generic.h"
#include "flags.h"
#include "graphics.h"
#include "snakes.h"
#include "scores.h"

#define SELECT_ERROR	-1	/* eroare la executia functiei select */
#define SELECT_EVENT	1	/* s-a apasat o tasta */
#define SELECT_TIMEOUT	0	/* timeout la select */

#define VI_SEC		0	/* viteza initiala, in secunda */
#define VI_USEC		600000	/* viteza initiala, in microsecunde */

/* Antet functii */

void error_check(char *msg);

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
			sel = select(nfds, &read_descriptors, NULL, NULL, &viteza);
			/* Citesc tasta pe care am apasat-o */
			if (sel == SELECT_EVENT) {
				key = gph_getkey();

				/* Ies imediat din joc */
				if (gph_is_quitkey(key) == 1)
					break;
			}

			/* Cat timp nu a expirat timpul initial */
			while (sel != SELECT_TIMEOUT) {
				sel = select(nfds, &read_descriptors, 
						NULL, NULL, &viteza);
				/* Ascult tastatura */
				if (sel == SELECT_EVENT) {
					key = gph_getkey();

					/* Ies imediat din joc */
					if (gph_is_quitkey(key) == 1)
						break;
				}
			}

			if (flag_has("dead") != 0) {
				flag_del("game_mode");
				getchar();
				break;
			}

			if (snk_isdir(key) == 1)
				snk_addmv(key);

			FD_SET(0, &read_descriptors);

			viteza.tv_sec = VI_SEC;
			viteza.tv_usec = VI_USEC;

			snk_move();
			error_check("DRAWING SNAKE (Not enough memory?)");
		} else if (flag_has("menu_mode") != 0) {
			key = gph_getkey();

			/* Ies din joc */
			if (gph_is_quitkey(key) == 1)
				break;

			if (gph_is_menukey(key) == 1) {
				gph_menuact(key);
				score_init();
				snk_init();
				error_check("INITIALIZING SNAKE (Not enough memory?)");	

				/* Entering game mode */
				flag_del("menu_mode");
				flag_add("game_mode", 1);
			}
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
