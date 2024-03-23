all:
	g++ editor2.cpp -o run -lraylib

clean:
	rm -r ./levels/level*
	rm run	