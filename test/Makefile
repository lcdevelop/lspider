all : test_util test_qtextcodec test_backtrace

test_util : test_util.o util.o
	g++ -g -o test_util test_util.o util.o -lz

test_qtextcodec : test_qtextcodec.o
	g++ -g -o test_qtextcodec test_qtextcodec.o -L /usr/lib64/ -lQt5Core

test_backtrace : test_backtrace.o
	g++ -g -o test_backtrace test_backtrace.o -rdynamic -ldl




test_util.o : test_util.cpp
	g++ -c -g -o test_util.o test_util.cpp -I ../src/

util.o : ../src/util.cpp
	g++ -c -g -o util.o ../src/util.cpp

test_qtextcodec.o : test_qtextcodec.cpp
	g++ -c -g -o test_qtextcodec.o test_qtextcodec.cpp -I /usr/include/qt5/ -fPIC

test_backtrace.o : test_backtrace.cpp
	g++ -c -g -o test_backtrace.o test_backtrace.cpp

clean :
	rm -f *.o test_util test_qtextcodec test_backtrace
