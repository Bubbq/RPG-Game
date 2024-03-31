all:
	g++ better_editor.cpp -o run -lraylib -lm
edit:
	g++ editor.cpp tile_generation.cpp -o run -lraylib
anim:
	g++ animation.cpp tile_generation.cpp -o run -lraylib
clean:
	rm -r levels/level*
	rm run
