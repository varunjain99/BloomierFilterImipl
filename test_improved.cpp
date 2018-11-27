#include "improved.hpp"
#include <unordered_map>
#include <iostream>

int main() {
	std::unordered_map<int, int> kvmap;
	kvmap[1] = 8;
	kvmap[2] = 9;
	kvmap[3] = 10;
	ImprovedBloomierFilter bf = ImprovedBloomierFilter(kvmap, 0.2, 10, 3, 4);
	for (int i = 1; i <= 3; ++i) {
		std::cout << "KEY: " << i << " , VAL: " << bf.get(i) << std::endl;
	}
	return 0;
}
