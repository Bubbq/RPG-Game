all:
	g++ main.cpp Graphics.cpp -o run -lraylib

clean:
	rm run
