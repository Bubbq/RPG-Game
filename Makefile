all:
	g++ animation.cpp -o run -lraylib

clean:
	rm -r ./levels/level*
	rm run	