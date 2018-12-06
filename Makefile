CFLAGS = -Wall -std=c++11 -pedantic -O3

.PHONY: all clean hash_util prime_util

hash_util: hash_util.hpp hash_util.cpp
	g++ $(CFLAGS) -c hash_util.cpp -o hash_util.o

prime_util: prime_util.hpp prime_util.cpp
	g++ $(CFLAGS) -c prime_util.cpp -o prime_util.o

libimproved.a: improved.cpp improved.hpp prime_util hash_util
	g++ $(CFLAGS) -c improved.cpp -o improved.o
	ar rvs libimproved.a improved.o prime_util.o hash_util.o

libimmutable.a: immutable.cpp immutable.hpp hash_util
	g++ $(CFLAGS) -c immutable.cpp -o immutable.o
	ar rvs libimmutable.a immutable.o hash_util.o

test_improved: test_improved.cpp data_store.cpp data_store.hpp libimproved.a
	g++ $(CFLAGS) test_improved.cpp data_store.cpp libimproved.a -o test_improved.o

test_immutable: test_immutable.cpp libimmutable.a
	g++ $(CFLAGS) test_immutable.cpp libimmutable.a -o test_immutable.o

test_bucketing: test_bucketing.cpp data_store.cpp data_store.hpp libimproved.a
	g++ $(CFLAGS) test_bucketing.cpp data_store.cpp libimproved.a -o test_bucketing.o

all: libimproved.a libimmutable.a test_improved test_immutable test_bucketing

clean:
	-rm -f *.o
	-rm -f *.a
