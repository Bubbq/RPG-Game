all:
	g++ animation.cpp tile_generation.cpp -o run -lraylib
edit:
	g++ editor.cpp tile_generation.cpp -o run -lraylib
clean:
	rm -r levels/level*
	rm run
