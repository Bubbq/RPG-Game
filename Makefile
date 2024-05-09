all:
	gcc game.c -o run -lraylib -lm -Wall
clean:
	rm run
