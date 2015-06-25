all : test_util test_qtextcodec

test_util : test_util.o util.o
	g++ -o test_util test_util.o util.o -lz

test_qtextcodec : test_qtextcodec.o
	g++ -o test_qtextcodec test_qtextcodec.o -L /usr/lib64/ -lQt5Core

test_util.o : test_util.cpp
	g++ -c -o test_util.o test_util.cpp -I ../src/

util.o : ../src/util.cpp
	g++ -c -o util.o ../src/util.cpp

test_qtextcodec.o : test_qtextcodec.cpp
	g++ -c -o test_qtextcodec.o test_qtextcodec.cpp -I /usr/include/qt5/ -fPIC

clean :
	rm -f *.o test_util test_qtextcodec
