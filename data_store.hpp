#ifndef DATA_STORE_H
#define DATA_STORE_H

#include <unordered_map>
#include <iostream>


class DataStore {
public:
  DataStore(int N);
  std::unordered_map<int, int> keyValueMap;
  double one_sided_processing_time;
  double false_positive_rate;
  double two_sided_processing_time;
  double S_error_rate;
  double not_S_error_rate;
  int num_hash_blocks;
  int num_tries;
};

#endif
