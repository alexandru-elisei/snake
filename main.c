#include <stdio.h>
#include <curses.h>
#include "generic.h"
#include "flags.h"
#include "graphics.h"

/* In snakes.c */
void snk_init();

/* In snakes.c */
void snk_addmv(char dir);

/* In snakes.c */
int snk_isdir(char c);

void error_check(char *msg);

int main(void)
{
	char key;
	FILE *f;

	/* Resetting debug file */
	f = fopen(DEB_FILE, "w");
	fclose(f);

	gph_init();

	gph_drwborder();
	error_check("DRAWING BORDER");	

	flag_init();
	error_check("INITIALIZING FLAGS");	

	snk_init();
	error_check("INITIALIZING SNAKE");	


	//f = fopen(DEB_FILE, "a");

	/*
	flag_add("dead", 1);
	if (flag_has("dead") != 0)
		fprintf(f, "\nRegistering dead\n");
		*/
	while (FOREVER) {
		key = gph_getkey();
		if (key == 'q' || flag_has("dead") != 0)
			break;
		if (snk_isdir(key) == 1)
			snk_addmv(key);
	}
	
	/*
	fprintf(f, "\nExited infinte loop\n");
	fclose(f);
	*/

	getchar();

	gph_reset();


	/*
	flag_init();

	flag_add("test", 1);
	printf("%d\n", flag_has("test"));

	flag_add("test", 2);
	printf("%d\n", flag_has("test"));

	flag_add("other", 2);
	printf("%d\n", flag_has("other"));

	flag_del("test");
	printf("%d\n", flag_has("test"));
	*/

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