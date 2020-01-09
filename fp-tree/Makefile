all: 0

0: main.exe
	main.exe data/T10I4D100K.txt data/T10I4D100K.out.txt 50

1: main.exe
	main.exe data/T15I7N0.5KD1K.txt data/T15I7N0.5KD1K.out.txt 5

2: main.exe
	main.exe data/T15I7N0.5KD10K.txt data/T15I7N0.5KD10K.out.txt 50

3: main.exe
	main.exe data/T15I7N0.5KD100K.txt data/T15I7N0.5KD100K.out.txt 500

4: main.exe
	main.exe data/T15I7N0.5KD1000K.txt data/T15I7N0.5KD1000K.out.txt 30000

5: main.exe
	main.exe data/T10I10N0.1KD1K.txt data/T10I10N0.1KD1K.out.txt 4

6: main.exe
	main.exe data/T20I15N1KD10K.txt data/T20I15N1KD10K.out.txt 9

7: main.exe
	main.exe data/T12I30N0.5KD100K.txt data/T12I30N0.5KD100K.out.txt 300

8: main.exe
	main.exe data/T18I20N2KD5000K.txt data/T18I20N2KD5000K.out.txt 65000

9: main.exe
	main.exe data/small.txt data/small-out.txt 2

main.exe: main.o fpgrowth.o
	g++ -o main main.o -std=c++11 -O3

main.o: main.cpp
	g++ -c main.cpp -std=c++11 -O3

fpgrowth.o: fpgrowth.cpp
	g++ -c fpgrowth.cpp -std=c++11 -O3

clean:
	del *.exe *.o
