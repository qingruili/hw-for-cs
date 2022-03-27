all : main.o list.o
	gcc -o all main.o list.o
test.o : main.c list.h
	gcc -c main.c
list.o : list.h list.c
	gcc -c list.c

clean :
	rm all test.o list.o

