#include <string.h>
#include <stdio.h>
#include "generic.h"
#include "flags.h"


struct Flag {
	char *name;
	int value;
};


static struct Flag *flags;	/* vectorul care va retine evenimentele */
static int n;			/* numarul de elemente din vectorul flags */
static int memaloc;		/* memoria alocata curent */
static int fatal_error;		/* pentru exception handling */

/**************************************************************************/
static FILE *f;
/**************************************************************************/

/* Antet functii locale/private */

static int flag_getpos(char *nm);

static void aloca_mem(struct Flag **f, int *mem);


/* Basic constructor-type function */
void flag_init()
{
	fatal_error = 0;
	n = 0;
	memaloc = 0;

	aloca_mem(&flags, &memaloc);
}

/* Adaug un eveniment */
void flag_add(char *nm, int val)
{
	int position;

	/* Basic exception handling - fatal_error e tratat in main.c */
	if (strcmp(nm, "fatal_error") == 0) {
		fatal_error = val;
		return;
	}

	/* Valoarea 0 inseamna ca sterg evenimentul */
	if (val == 0) {
		flag_del(nm);
		return;
	}
		
	/* Verific daca evenimentul s-a produs deja, si daca da,
	 * atunci doar actualizez valoarea lui cu cea trimisa */
	position = flag_getpos(nm);
	if (position == -1) {
		position = n;

		/* Realoc memorie pentru vectorul de evenimente */
		if (n == memaloc)
			aloca_mem(&flags, &memaloc);

		/* Creez noul element */
		flags[position].name = strdup(nm);
		++n;
	}

	flags[position].value = val;

	/************************************************************/
	/*
	f = fopen(DEB_FILE, "a");
	fprintf(f, "\nflag_add > nm = %s\n", nm);
	fprintf(f, "\n\nflag_add > flags:\n");
	for (position = 0; position < n; position++)
		fprintf(f, "flags[%d].name = %s, .val = %d\n",
				position,
				flags[position].name,
				flags[position].value);
	fclose(f);
	*/
	/************************************************************/
}

/* Verific daca un eveniment s-a produs, si daca da, returnez valoarea lui
 * Valoarea 0 inseamna ca elementul nu exista */
int flag_has(char *nm)
{
	int position;


	/* Verific existenta erorilor fatale */
	if (strcmp(nm, "fatal_error") == 0)
		return fatal_error;

	position = flag_getpos(nm);

	/************************************************************/
	/*
	f = fopen(DEB_FILE, "a");
	fprintf(f, "\nflag_has > position for %s = %d\n", nm, position);
	fclose(f);
	*/
	/************************************************************/

	if (position == -1)
		return 0;
	else
		return flags[position].value;
}

/* Sterg un eveniment din lista cu evenimente */
void flag_del(char *nm)
{
	int i;
	int position;

	position = flag_getpos(nm);
	if (flag_getpos(nm) > -1) {
		for (i = position + 1; i < n; i++)
			flags[i - 1] = flags[i];
		--n;
	}
}

/* Destructor */
void flag_reset()
{
	n = 0;
	memaloc = 0;
	free(flags);
	fatal_error = 0;
}

/* Returneaza pozitia unui eveniment */
static int flag_getpos(char *nm)
{
	int i;
	int position;

	if (n == 0)
		return -1;

	position = -1;
	for (i = 0; i < n; i++)
		if (strcmp(flags[i].name, nm) == 0) {
			position = i;
			break;
		}

	return position;
}

/* Aloca memorie pentru vectorul de evenimente */
static void aloca_mem(struct Flag **f, int *mem)
{
	if (*mem == 0)
		*f = (struct Flag *) malloc((*mem + MEM_INC) *
				sizeof(struct Flag));
	else
		*f = (struct Flag *) realloc(*f,
				(*mem + MEM_INC) * sizeof(struct Flag));
		
	if (*f == NULL) {
		flag_add("fatal_error", 1);
		return;
	}

	(*mem) += MEM_INC;
}
