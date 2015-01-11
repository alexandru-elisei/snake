#include <stdio.h>
#include <stdlib.h>
#include "generic.h"

/* Returneaza 1 in caz de succes, 0 daca s-au citit mai multe caractere
 * decat maxlen si -1 daca nu s-a citit niciun caracter (diferit de newline)
 * newline nu este considerat caracter valid */
int read_string(char **s, 	/* pointer catre sirul de citit */
		int maxlen,	/* lungimea maxima a sirului */
	   	FILE *stream)	/* fisierul de unde se citeste */
{
	/* Fortez getline sa imi aloce memorie pentru buffer */
	char *ptrlinie = NULL;	/* buffer pentru functia de citire */
	size_t len = 0;
	ssize_t caractere_citite;

	caractere_citite = getline(&ptrlinie, &len, stream);

	/* Daca am citit cel putin un caracter diferit de newline */
	if (caractere_citite > 1 ||
		(caractere_citite == 1 && ptrlinie[0] != '\n')) {

		/* Nu salvez newline */
		if (ptrlinie[caractere_citite - 1] == '\n')  {
			ptrlinie[caractere_citite - 1] = '\0';
			ptrlinie = (char *) realloc(ptrlinie, 
				caractere_citite * sizeof(char));
			--caractere_citite;
		}

		if (caractere_citite > maxlen) {
			free(ptrlinie);		/* no memory leaks for me! */
			return 0;
		}

		*s = ptrlinie;

		return 1;
	} else {
		free(ptrlinie);			/* no memory leaks for me! */
		return -1;
	}
} 

