TOOLS= arguments.o main.o main_function.o queue_function.o FirstComeFirstServed.o
CFLAGS=-pthread -Wall -ggdb3 -pedantic -O3
CC=gcc


all: simulator

simulator: arguments.o main_function.o main.o queue_function.o FirstComeFirstServed.o
	$(CC) $(CFLAGS) -o $@ $(TOOLS)

arguments.o: arguments.c main.h
	$(CC) $(CFLAGS) -g -c arguments.c

main.o: main.c structures.h main_function.h
	$(CC) $(CFLAGS) -g -c main.c

main_function.o: main_function.c structures.h main_function.h
	$(CC) $(CFLAGS) -g -c main_function.c

queue_function.o: queue_function.c structures.h queue_function.h
	$(CC) $(FLAGS) -g -c queue_function.c

FirstComeFirstServed.o: FirstComeFirstServed.c structures.h queue_function.h
	$(CC) $(CFLAGS) -g -c FirstComeFirstServed.c queue_function.h -lpthread

clean: cleano cleanb

cleanb:
	find . -maxdepth 1 -type f -executable -delete
cleano:
	rm -f *.o