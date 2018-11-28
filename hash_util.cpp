#include "hash_util.hpp"

HashFunctor::HashFunctor() {}
HashFunctor::HashFunctor(int range, int prime) : mRange(range), mPrime(prime) {
  mA = rand() % mPrime;
  mB = rand() % mPrime;
}

HashFunctor::HashFunctor(int a, int b, int range, int prime) :
					mA(a), mB(b), mRange(range), mPrime(prime) {}

int HashFunctor::operator()(int x) const {
	return ((mA * x + mB) % mPrime) % mRange;
}
