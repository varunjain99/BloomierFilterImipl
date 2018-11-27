CFLAGS = -Wall -std=c++11 -pedantic

.PHONY: all clean

libimproved.a: improved.cpp improved.hpp prime_util.cpp prime_util.hpp
	g++ $(CFLAGS) -c prime_util.cpp -o prime_util.o
	g++ $(CFLAGS) -c improved.cpp -o improved.o
	ar rvs libimproved.a improved.o prime_util.o

libimmutable.a: immutable.cpp immutable.hpp
	g++ $(CFLAGS) -c immutable.cpp -o immutable.o
	ar rvs libimmutable.a immutable.o

test_improved: test_improved.cpp libimproved.a
	g++ $(CFLAGS) test_improved.cpp libimproved.a -o test_improved.o

test_immutable: test_immutable.cpp libimmutable.a
	g++ $(CFLAGS) test_immutable.cpp libimmutable.a -o test_immutable.o

all: libimproved.a libimmutable.a test_improved test_immutable

clean:
	-rm -f *.o
	-rm -f *.a
