#ifndef GENERIC_H

#define GENERIC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>
#include <ctype.h>

#define MEM_INC 20			/* incrementul de alocare dinamica */
#define FOREVER	1			/* for endless looping */
#define TITLU	"ESCAPE THE BACKYARD"	/* titlul jocului */

#define DEB_FILE	"debug"		/* for debugging */

#define ERROR(msg) printf("\n** FATAL ERROR %s **\n\n", (msg))


/*
 * Propria mea functie de citire a sirurilor de caractere
 * Conform GNU:
 * https://www.gnu.org/software/libc/manual/html_node/Line-Input.html
 *
 * Citeste un string  de dintr-un fisier folosind getline;
 * Returneaza aceleasi valori ca functia getline
 * Daca citirea nu s-a efectuat cu succes, parametrul nu este modificat
 * returneaza 1 in caz de succes, 0 daca s-au citit mai multe caractere
 * si -1 daca nu s-a citit niciun caracter (diferit de newline)
 */
int read_string(char **s, int maxlen, FILE *stream);

#endif 		/* # ifndef GENERIC_H */
