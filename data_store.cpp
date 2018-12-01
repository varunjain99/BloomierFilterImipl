#include "data_store.hpp"

DataStore::DataStore(int N) {
  int key, val;
  for (int i = 0; i < N; ++i) {
    std::cin >> key >> val;
    keyValueMap[key] = val;
  }
}
