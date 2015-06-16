all : test_util

test_util : test_util.o util.o
	g++ -o test_util test_util.o util.o -lz

test_util.o : test_util.cpp
	g++ -c -o test_util.o test_util.cpp -I ../src/

util.o : ../src/util.cpp
	g++ -c -o util.o ../src/util.cpp

clean :
	rm -f *.o test_util
