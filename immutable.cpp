#include "immutable.hpp"

BloomierFilter::BloomierFilter(int numBuckets, int modulus, const std::unordered_map<int, int>& keyValueMap) {
	srand(time(NULL));
	mNumBuckets = numBuckets;
	mModulus = modulus;
	mHashFunctions[2] = HashFunctor(rand() % PRIME, rand() % PRIME, mModulus, PRIME);
	mFilter.resize(mNumBuckets);
	mAdjList.resize(mNumBuckets);
	generateBloomierFilter(keyValueMap);
}

bool BloomierFilter::cyclicUtil(int prev, int node, std::vector<int>& state) {
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

bool BloomierFilter::checkSimpleIsAcyclic() {
	std::vector<int> state(mNumBuckets, 0);
	for (int i = 0; i < mNumBuckets; ++i) {
		if (state[i] == 0 && cyclicUtil(-1, i, state)) {
			return false;
		}
	}
	return true;
}


// Generate hash functions
void BloomierFilter::generateHashFunctions() {
	for (int i = 0; i < 2; ++i) {
		mHashFunctions[i] = HashFunctor(rand() % PRIME, rand() % PRIME, mNumBuckets, PRIME);
	}
}

// Recursive function to fill bloomierFilter
void BloomierFilter::computeVals(int node, const Edge& e, std::vector<bool>& computed) {
	mFilter[e.target] = (e.f_val - mFilter[node] - e.h3_val) % mModulus;
	computed[e.target] = true;
	for (int i = 0; i < mAdjList[e.target].size(); ++i) {
		if (!computed[mAdjList[e.target][i].target]) {
			computeVals(e.target, mAdjList[e.target][i], computed);
		}
	}
}

void BloomierFilter::generateBloomierFilter(const std::unordered_map<int, int>& keyValueMap) {
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

int BloomierFilter::get(int x) {
	int h1_val = mHashFunctions[0](x);
	int h2_val = mHashFunctions[1](x);
	int h3_val = mHashFunctions[2](x);
	int answer = (mFilter[h1_val] + mFilter[h2_val] + h3_val) % mModulus;
	if (answer < 0) {
		return answer + mModulus;
	} else {
		return answer;
	}
}
