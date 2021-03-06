#include "improved.hpp"
#include <unordered_map>
#include <iostream>
#include <time.h>

#include "data_store.hpp"

int main(int argc, char** argv) {
	if (argc != 6) {
		std::cerr << "Expected 5 arguments" << std::endl;
		return 1;
	}
	int N, M, s, K;
	float eps;
	if (sscanf (argv[1], "%d", &N) != 1) {
		fprintf(stderr, "error - not an int");
	}
	if (sscanf (argv[2], "%f", &eps) != 1) {
    fprintf(stderr, "error - not an float");
	}
	if (sscanf (argv[3], "%d", &M) != 1) {
		fprintf(stderr, "error - not an int");
	}
	if (sscanf (argv[4], "%d", &s) != 1) {
		fprintf(stderr, "error - not an int");
	}
	if (sscanf (argv[5], "%d", &K) != 1) {
		fprintf(stderr, "error - not an int");
	}

	DataStore store = DataStore(N);

  // Test one-sided time
	clock_t startProcessTime = clock();
	ImprovedBloomierFilter bf = ImprovedBloomierFilter(store.keyValueMap, eps, M, s, K);
	store.one_sided_processing_time = double(clock() - startProcessTime) / (double) CLOCKS_PER_SEC;

	// Test number of tries to get one sided error
	store.num_tries = bf.getNumTries();

  // Test for false positive rate
  int falsePositiveCounter = 0;
	int i = 0;
	while (i < 1000) {
		int key = rand() % 16000;
	  if (store.keyValueMap.find(key) == store.keyValueMap.end()) {
			int val_guess = bf.get(key);
			if (val_guess < (1 << K) && val_guess >= 0) {
				falsePositiveCounter++;
			}
			i++;
		}
	}
	store.false_positive_rate = (double) falsePositiveCounter / 1000;

  // Test Two-sided processing time
	startProcessTime = clock();
	bf = ImprovedBloomierFilter(store.keyValueMap, eps, M, s, K, false);
	store.two_sided_processing_time = double(clock() - startProcessTime) / (double) CLOCKS_PER_SEC;

	// Test number of hash blocks that are stored
	store.num_hash_blocks = bf.getNumHashBlocks();

  // Test error rate for values in S
	int numWrong = 0;
	for (auto it = store.keyValueMap.begin(); it != store.keyValueMap.end(); ++it) {
		if (bf.get(it->first) != it->second) {
			numWrong++;
		}
	}
	store.S_error_rate = (double) numWrong / N;

	//Test error rate for values not in S
  numWrong = 0;
	i = 0;
	while (i < 1000) {
		int key = rand() % 16000;
	  if (store.keyValueMap.find(key) == store.keyValueMap.end()) {
			int val_guess = bf.get(key);
			if (val_guess < (1 << K) && val_guess >= 0) {
				numWrong++;
			}
			i++;
		}
	}
	store.not_S_error_rate = (double) numWrong / 1000;

	std::cout << "one_sided_processing_time, " << store.one_sided_processing_time << std::endl;
  std::cout << "num_tries, " << store.num_tries << std::endl;
	std::cout << "false_positive_rate, " << store.false_positive_rate << std::endl;
	std::cout << "two_sided_processing_time, " << store.two_sided_processing_time << std::endl;
	std::cout << "S_error_rate, " << store.S_error_rate << std::endl;
	std::cout << "not_S_error_rate, " << store.not_S_error_rate << std::endl;
	std::cout << "num_hash_blocks, " << store.num_hash_blocks << std::endl;

	return 0;
}
