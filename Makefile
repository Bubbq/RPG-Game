all:
	g++ editor.cpp -o run -lraylib

clean:
	rm -r ./levels/level*
	rm run	