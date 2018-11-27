#ifndef LIB_IMMUTABLE_H
#define LIB_IMMUTABLE_H

#include <vector>
#include <unordered_map>
#include <cstdlib>
#include <iostream>
#include <ctime>

#define PRIME 105943


// Edge struct
struct Edge {
	Edge(int target, int f_val, int h3_val) : target(target), f_val(f_val), h3_val(h3_val) {}

	int target; // target bucket
	int f_val; // Value of f(s)
	int h3_val; //value of h_3(s)
};

class HashFunctor {
public:
	HashFunctor() {}
	HashFunctor(int a, int b, int range, int prime) :
						mA(a), mB(b), mRange(range), mPrime(prime) {}

	int operator()(int x) const {
		return ((mA * x + mB) % mPrime) % mRange;
	}
private:
	int mA;
	int mB;
	int mRange;
	int mPrime;
};


class BloomierFilter {
public:
	// creates Bloomier Filter given number of buckets in hash map and modulus
	// TODO: Expand values beyond just int
	BloomierFilter(int numBuckets, int modulus, const std::unordered_map<int, int>& keyValueMap);
	int get(int x);


private:
	// Filter
	std::vector<int> mFilter;
	// number of bucketes
	int mNumBuckets;
	// modulus
	int mModulus;
	// adjacency list
	std::vector<std::vector<Edge> > mAdjList;
	// hash functions
	HashFunctor mHashFunctions[3];

	// Generate random hash functions from universal family (first 2 hash functions)
	void generateHashFunctions();

	// Generate final Bloomier Filter
	void generateBloomierFilter(const std::unordered_map<int, int>& keyValueMap);

	// Recursive helper function to fill bloomierFilter, given graph
	void computeVals(int node, const Edge& e, std::vector<bool>& computed);

	// Recursive DFS function from node, and state array (0, 1, or 2)
	// Returns true if Cycle found in undirected graph, otherwise false
	// Assumes graph is simple
	bool cyclicUtil(int prev, int node, std::vector<int>& state);

	// Checks if simple mAdjList graph is also acyclic
	// Returns true if acyclic; false otherwise
	bool checkSimpleIsAcyclic();
};

#endif
