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

// TODO: Extend impl to K != int
// TODO: Potential overflow issues right now
template<class K>
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


template <class K>
class BloomierFilter {
public:
	// creates Bloomier Filter given number of buckets in hash map and modulus
	// TODO: Expand values beyond just int
	BloomierFilter(int numBuckets, int modulus, const std::unordered_map<K, int>& keyValueMap);
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
	HashFunctor<K> mHashFunctions[3];

	// Generate random hash functions from universal family (first 2 hash functions)
	void generateHashFunctions();

	// Generate final Bloomier Filter
	void generateBloomierFilter(const std::unordered_map<K, int>& keyValueMap);

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

template<class K>
BloomierFilter<K>::BloomierFilter(int numBuckets, int modulus, const std::unordered_map<K, int>& keyValueMap) {
	srand(time(NULL));
	mNumBuckets = numBuckets;
	mModulus = modulus;
	mHashFunctions[2] = HashFunctor<int>(rand() % PRIME, rand() % PRIME, mModulus, PRIME);
	mFilter.resize(mNumBuckets);
	mAdjList.resize(mNumBuckets);
	generateBloomierFilter(keyValueMap);
}

template<class K>
bool BloomierFilter<K>::cyclicUtil(int prev, int node, std::vector<int>& state) {
	state[node] = 1;
	for (int i = 0; i < mAdjList[node].size(); ++i) {
		if (state[mAdjList[node][i].target] == 0) {
			if (cyclicUtil(node, mAdjList[node][i].target, state)) {
				return true;
			}
		} else if (state[mAdjList[node][i].target] == 1 && mAdjList[node][i].target != prev) {
			return true;
		}
	}
	state[node] = 2;
	return false;
}

template<class K>
bool BloomierFilter<K>::checkSimpleIsAcyclic() {
	std::vector<int> state(mNumBuckets, 0);
	for (int i = 0; i < mNumBuckets; ++i) {
		if (state[i] == 0 && cyclicUtil(-1, i, state)) {
			return false;
		}
	}
	return true;
}


// Generate hash functions
template<class K>
void BloomierFilter<K>::generateHashFunctions() {
	for (int i = 0; i < 2; ++i) {
		mHashFunctions[i] = HashFunctor<int>(rand() % PRIME, rand() % PRIME, mNumBuckets, PRIME);
	}
}

// Recursive function to fill bloomierFilter
template<class K>
void BloomierFilter<K>::computeVals(int node, const Edge& e, std::vector<bool>& computed) {
	mFilter[e.target] = (e.f_val - mFilter[node] - e.h3_val) % mModulus;
	computed[e.target] = true;
	for (int i = 0; i < mAdjList[e.target].size(); ++i) {
		if (!computed[mAdjList[e.target][i].target]) {
			computeVals(e.target, mAdjList[e.target][i], computed);
		}
	}
}

template<class K>
void BloomierFilter<K>::generateBloomierFilter(const std::unordered_map<K, int>& keyValueMap) {
	bool simple;
	do {
		for (int i = 0; i < mAdjList.size(); ++i) {
			mAdjList[i].clear();
		}
		generateHashFunctions();
		simple = true;
		// TODO: Make this less hacky
		std::unordered_map<int, bool> edgeMapping = {};
		for (const auto& kv : keyValueMap) {
			int h1_val = mHashFunctions[0](kv.first);
			int h2_val = mHashFunctions[1](kv.first);
			int h3_val = mHashFunctions[2](kv.first);

			if (h1_val == h2_val || edgeMapping.find(h1_val * mNumBuckets + h2_val) != edgeMapping.end()) {
				simple = false;
				break;
			}

			edgeMapping[h1_val * mNumBuckets + h2_val] = true;
			edgeMapping[h2_val * mNumBuckets + h1_val] = true;

			mAdjList[h1_val].emplace_back(h2_val, kv.second, h3_val);
			mAdjList[h2_val].emplace_back(h1_val, kv.second, h3_val);

		}
	} while(!simple || !checkSimpleIsAcyclic());

	std::vector<bool> computed(mNumBuckets, 0);
	for (int i = 0; i < mNumBuckets; ++i) {
		if (!computed[i]) {
			computed[i] = true;
			for (int j = 0; j < mAdjList[i].size(); ++j) {
				if (!computed[mAdjList[i][j].target]) {
					computeVals(i, mAdjList[i][j], computed);
				}
			}
		}

	}
}

template<class K>
int BloomierFilter<K>::get(int x) {
	int h1_val = mHashFunctions[0](x);
	int h2_val = mHashFunctions[1](x);
	int h3_val = mHashFunctions[2](x);
	return mFilter[h1_val] + mFilter[h2_val] + h3_val;

}

int main() {
	std::unordered_map<int, int> kvmap;
	kvmap[1] = 8;
	kvmap[2] = 9;
	kvmap[3] = 10;
	BloomierFilter<int> bf = BloomierFilter<int>(100, 1000, kvmap);
	for (int i = 1; i <= 3; ++i) {
		std::cout << "KEY: " << i << " , VAL: " << bf.get(i) << std::endl;
	}
	return 0;
}
