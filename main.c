#include <sys/select.h>
#include "generic.h"
#include "flags.h"
#include "graphics.h"
#include "snakes.h"

/* Antet functii */

void error_check(char *msg);

int main(void)
{
	int nfds;
	int sel;
	char key;
	struct timeval timeout;
	FILE *f;

	/* Resetting debug file */
	f = fopen(DEB_FILE, "w");
	fclose(f);

	gph_init();

	/* Initializing select */

	
	flag_init();
	error_check("INITIALIZING FLAGS");	


	gph_drwmenu();
	error_check("DRAWING MENU");	
	flag_add("menu_mode", 1);

	nfds = 0;
	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	while (FOREVER) {
		key = 0;
		key = gph_getkey();

		sel = select(nfds, NULL, NULL, NULL, &timeout);

		/**************************************/
		f = fopen(DEB_FILE, "a");
		fprintf(f, "Select fired\n");
		fflush(f);

		if (gph_is_quitkey(key) == 1) {
			gph_reset();
			break;
		}

		if (flag_has("menu_mode") != 0) {
			fprintf(f, "Entering menu_mode check\n");
			fflush(f);
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
			if (flag_has("dead") != 0) {
				flag_del("game_mode");
				getchar();
				break;
			}
			if (snk_isdir(key) == 1)
				snk_addmv(key);

			snk_move();
		}

		timeout.tv_sec = 1;
		timeout.tv_usec = 0;
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
