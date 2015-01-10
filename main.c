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

	while (FOREVER) {
		key = gph_getkey();

		if (flag_has("menu_mode") != 0) {
			curs_set(FALSE);
			snk_init();
			error_check("INITIALIZING SNAKE");	
		} else {
			if (key == 'q' || flag_has("dead") != 0)
				break;
			if (snk_isdir(key) == 1)
				snk_addmv(key);
		}
	}

	getchar();
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
