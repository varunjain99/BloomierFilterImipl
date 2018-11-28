#include "data_store.hpp"

DataStore::DataStore(int N) {
  int i = 0;
  int val;
  while (i < N) {
    std::cin >> val;
    keyValueMap[i] = val;
    i++;
  }
}
