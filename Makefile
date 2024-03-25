animate:
	g++ animation.cpp -o run -lraylib

edit:
	g++ editor.cpp -o run -lraylib
clean:
	rm -r ./levels/level*
	rm run	