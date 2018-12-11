#include "immutable.hpp"
#include <unordered_map>
#include <iostream>
#include <time.h>

#include "data_store.hpp"

int main(int argc, char** argv) {
	if (argc != 5) {
		std::cerr << "Expected 4 arguments" << std::endl;
		return 1;
	}
	int N, maxVal, K;
	float c;
	if (sscanf (argv[1], "%d", &N) != 1) {
		fprintf(stderr, "error - not an int");
	}
	if (sscanf (argv[2], "%f", &c) != 1) {
	    fprintf(stderr, "error - not an float");
	}
	if (sscanf (argv[3], "%d", &maxVal) != 1) {
		fprintf(stderr, "error - not an int");
	}
	if (sscanf (argv[4], "%d", &K) != 1) {
		fprintf(stderr, "error - not an int");
	}

	DataStore store = DataStore(N);

        // Test one-sided time
	clock_t startProcessTime = clock();
	BloomierFilter bf = BloomierFilter(int(N*c), maxVal, store.keyValueMap);
	store.one_sided_processing_time = double(clock() - startProcessTime) / (double) CLOCKS_PER_SEC;

        for (auto it = store.keyValueMap.begin(); it != store.keyValueMap.end(); ++it)  {
            if (bf.get(it->first) != it->second) {
              std::cout << "ERROR " << it->first << " " << it->second << " " << bf.get(it->first) << std::endl; 
            }
        }


        // Test for false positive rate
        int falsePositiveCounter = 0;
	int i = 0;
	while (i < 1000) {
	  int key = rand() % (2 * N);
	  if (store.keyValueMap.find(key) == store.keyValueMap.end()) {
			int val_guess = bf.get(key);
			if (val_guess < (1 << K) && val_guess >= 0) {
 				falsePositiveCounter++;
			}
			i++;
		}
	}
	store.false_positive_rate = (double) falsePositiveCounter / 1000.;

	std::cout << "one_sided_processing_time, " << store.one_sided_processing_time << std::endl;
	std::cout << "false_positive_rate, " << store.false_positive_rate << std::endl;
        std::cout << "persistent_memory, " << 4 + int(N * c) * 4 + 3 * 8 << std::endl;
	return 0;
}
