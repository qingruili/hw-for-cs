all: s-talk

s-talk: s-talk.c list.o
	gcc -g s-talk.c list.o -o s-talk -lpthread -w

clean:
	rm -f *.o
	rm -f s-talk