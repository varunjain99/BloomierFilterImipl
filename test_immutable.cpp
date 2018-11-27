#include "immutable.hpp"
#include <unordered_map>
#include <iostream>

int main() {
	std::unordered_map<int, int> kvmap;
	kvmap[1] = 8;
	kvmap[2] = 9;
	kvmap[3] = 10;
	BloomierFilter bf = BloomierFilter(100, 1000, kvmap);
	for (int i = 1; i <= 3; ++i) {
		std::cout << "KEY: " << i << " , VAL: " << bf.get(i) << std::endl;
	}
	return 0;
}
