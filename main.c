#include <stdio.h>
#include <curses.h>
#include "generic.h"
#include "flags.h"
#include "graphics.h"
#include "snakes.h"

/* Antet functii */

void error_check(char *msg);

int main(void)
{
	char key;
	FILE *f;

	/* Resetting debug file */
	f = fopen(DEB_FILE, "w");
	fclose(f);

	flag_init();
	error_check("INITIALIZING FLAGS");	

	gph_init();

	gph_drwmenu();
	error_check("DRAWING MENU");	
	flag_add("menu_mode", 1);

	while (FOREVER) {
		key = gph_getkey();

		if (gph_is_quitkey(key) == 1) {
			gph_reset();
			break;
		}

		if (flag_has("menu_mode") != 0) {
			if (gph_is_menukey(key) == 1) {
				gph_menuact(key);
				snk_init();
				error_check("INITIALIZING SNAKE");	

				/* Entering game mode */
				flag_del("menu_mode");
				flag_add("game_mode", 1);
			}
		} else {
			if (flag_has("dead") != 0) {
				flag_del("game_mode");
				getchar();
				break;
			}
			if (snk_isdir(key) == 1)
				snk_addmv(key);
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
