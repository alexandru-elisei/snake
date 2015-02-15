# Programarea Calculatoarelor, seria CC
# Tema2 - Snake

CC = gcc
FLAGS = -Wall -lncurses -g
PROG = snake
HEADER = 	generic.h		\
	 	graphics.h		\
	 	snakes.h		\
	 	scores.h		\
		flags.h
SOURCES = 	$(HEADER:%.h=%.c)	\
		main.c			
OBJS = $(SOURCES:%.c=%.o)


.PHONY:build

build: $(PROG)

$(PROG): $(OBJS) $(HEADER)
	$(CC) $(OBJS) -o $(PROG) $(FLAGS)

%.o: %.c
	$(CC) -c $^ -o $@ $(FLAGS)


.PHONY:

clean:
	rm -f *.out $(PROG) $(OBJS)

run: $(PROG)
	./$(PROG)
