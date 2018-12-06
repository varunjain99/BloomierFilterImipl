#include "improved.hpp"
#include <unordered_map>
#include <iostream>
#include <time.h>

#include "data_store.hpp"

int main(int argc, char** argv) {
	if (argc != 7) {
		std::cerr << "Expected 6 arguments" << std::endl;
		return 1;
	}
	int numBuckets, N, M, s, K;
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
  if (sscanf (argv[6], "%d", &numBuckets) != 1) {
		fprintf(stderr, "error - not an int");
	}

	DataStore store = DataStore(N);

  // Test one-sided time
	clock_t startProcessTime = clock();
	BucketedBloomierFilter bf = BucketedBloomierFilter(numBuckets, store.keyValueMap, eps, M, s, K);
	store.one_sided_processing_time = double(clock() - startProcessTime) / (double) CLOCKS_PER_SEC;

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


	std::cout << "one_sided_processing_time, " << store.one_sided_processing_time << std::endl;
	std::cout << "false_positive_rate, " << store.false_positive_rate << std::endl;

	return 0;
}
