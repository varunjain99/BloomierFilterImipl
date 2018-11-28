#ifndef HASH_UTIL_H
#define HASH_UTIL_H

#include <stdlib.h>

class HashFunctor {
public:
	HashFunctor();
  HashFunctor(int range, int prime);
	HashFunctor(int a, int b, int range, int prime);
  int operator()(int x) const;

private:
	int mA;
	int mB;
	int mRange;
	int mPrime;
};

#endif
