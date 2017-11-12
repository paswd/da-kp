FLAGS=-pedantic -Wall -Werror -Wno-sign-compare -Wno-long-long -lm -std=c++11 -O2 -lmpg123
COMPILLER=g++

#all: lib start
all: start

#start: main.o
#	$(COMPILLER) $(FLAGS) -o da-lab4 main.o -L. lib/lib-z-search.a

start: tmpg123.o read.o main.o
	$(COMPILLER) $(FLAGS) -o prog tmpg123.o read.o main.o

main.o: main.cpp
	$(COMPILLER) -c $(FLAGS) main.cpp

read.o: read.cpp
	$(COMPILLER) -c $(FLAGS) read.cpp

tmpg123.o: tmpg123.cpp
	$(COMPILLER) -c $(FLAGS) tmpg123.cpp

clean:
	rm -f *.o *.gch *.dat prog
	#rm lib/lib-z-search.a
