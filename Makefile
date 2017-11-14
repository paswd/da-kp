FLAGS=-pedantic -Wall -Werror -Wno-sign-compare -Wno-long-long -lm -std=c++11 -O2 -lmpg123
COMPILLER=g++

#all: lib start
all: start

da-kp: start

#start: main.o
#	$(COMPILLER) $(FLAGS) -o da-lab4 main.o -L. lib/lib-z-search.a

start: tmpg123.o read.o musiclib.o main.o
	$(COMPILLER) $(FLAGS) -o da-kp tmpg123.o read.o musiclib.o main.o

main.o: main.cpp
	$(COMPILLER) -c $(FLAGS) main.cpp

read.o: read.cpp
	$(COMPILLER) -c $(FLAGS) read.cpp

tmpg123.o: tmpg123.cpp
	$(COMPILLER) -c $(FLAGS) tmpg123.cpp

musiclib.o: musiclib.cpp
	$(COMPILLER) -c $(FLAGS) musiclib.cpp

clean:
	rm -f *.o *.gch *.dat da-kp
