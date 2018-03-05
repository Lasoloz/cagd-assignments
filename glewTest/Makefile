.PHONY: clean

glewTest: glewTest.o
	g++ glewTest.o -o glewTest -lGLEW -lGLU -lGL -lglut

glewTest.o: glewTest.cpp
	g++ -c glewTest.cpp -o glewTest.o -Wall -Wpedantic

clean:
	rm -f glewTest glewTest.o
