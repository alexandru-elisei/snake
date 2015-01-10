#include <sys/select.h>
#include "generic.h"
#include "flags.h"
#include "graphics.h"
#include "snakes.h"

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
	struct timeval tmp_vit;
	FILE *f;

	/* Resetting debug file */
	f = fopen(DEB_FILE, "w");
	fclose(f);
	f = fopen(DEB_FILE, "a");

	flag_init();
	error_check("INITIALIZING FLAGS");	

	gph_init();

	gph_drwmenu();
	error_check("DRAWING MENU");	
	flag_add("menu_mode", 1);

	/* Initializare select */
	nfds = 1;

	FD_ZERO(&read_descriptors);
	FD_SET(0, &read_descriptors);

	/* Viteza initiala */
	viteza.tv_sec = VI_SEC;
	viteza.tv_usec = VI_USEC;

	while (FOREVER) {

		/**************************************/
		fprintf(f, "key = %c\n", key);
		fflush(f);
		/**************************************/

		if (gph_is_quitkey(key) == 1) {
			gph_reset();
			break;
		}

		if (flag_has("menu_mode") != 0) {
			fprintf(f, "Entering menu_mode check\n");
			fflush(f);

			key = gph_getkey();

			if (gph_is_menukey(key) == 1) {
				gph_menuact(key);
				snk_init();
				error_check("INITIALIZING SNAKE");	

				/* Entering game mode */
				flag_del("menu_mode");
				flag_add("game_mode", 1);
			}
		} else {
			fprintf(f, "Entering game_mode check\n");
			fflush(f);

			sel = select(nfds, &read_descriptors, NULL, NULL, &viteza);
			if (sel == SELECT_EVENT) {
				key = gph_getkey();

				if (gph_is_quitkey(key) == 1) {
					gph_reset();
					break;
				}
			}

			while (sel != SELECT_TIMEOUT) {
				sel = select(nfds, &read_descriptors, NULL, NULL, &viteza);
				if (sel == SELECT_EVENT) {
					key = gph_getkey();

					if (gph_is_quitkey(key) == 1) {
						gph_reset();
						break;
					}
				}
			}

			if (flag_has("dead") != 0) {
				flag_del("game_mode");
				getchar();
				break;
			}

			if (snk_isdir(key) == 1)
				snk_addmv(key);

			snk_move();
		}

		FD_SET(0, &read_descriptors);

		viteza.tv_sec = VI_SEC;
		viteza.tv_usec = VI_USEC;
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
