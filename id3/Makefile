outputsuf = .out.txt

all: iris

iris: main
	./main data/iris.csv data/iris-out.txt

main: main.cpp id3.cpp
	g++ -o main main.cpp id3.cpp -std=c++11 -O3

main.o: main.cpp
	g++ -c main.cpp -std=c++11 -O3

id3.o: id3.cpp
	g++ -c id3.cpp -std=c++11 -O3

clean:
	del *.exe
