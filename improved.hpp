#ifndef LIB_IMPROVED_H
#define LIB_IMPROVED_H

#include <vector>
#include <utility>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <math.h>
#include <ctime>

#include "prime_util.hpp"
#include "hash_util.hpp"


class ImprovedBloomierFilter {
public:
  ImprovedBloomierFilter();
  ImprovedBloomierFilter(const std::unordered_map<int, int>& keyValueMap, float epsilon, int M, int s, int K, bool oneSided=true);
  int get(int x);

	int getNumHashBlocks() const;
  int getNumTries() const;

private:
  int mN;
  float mEpsilon;
  int mP;
  int mQ;
  int mM;
  int mS;
  int mK;
  HashFunctor mH0;
  std::vector<std::vector<HashFunctor>> mHashBlocks;
  std::vector<int> mG;
  std::unordered_map<int, int> mPInverses;
  int mNumTries;


  // Sets up parameter mQ
  void computeQ();
  // Sets up parameter mP
  void computeP();
  // Sets up map from x -> x^-1 in F_p
  void computePInverses();

  void computeG(const std::vector<std::vector<int>>& M, const std::vector<int>& v);

  // Populates mG, mHashBlocks, and mH0
  void genTable(const std::unordered_map<int, int>& keyValueMap);

  // Utility function used by genTable to check if row is linearly
  // dependent of previous rows
  //
  // Precondition: row 0 to row - 1 are in reduced echelon form
  // and have rank = row
  //
  // Postcondition: If linearly dependent, function returns false and
  // M[row] is zeroed out. If independent, then function returns true
  // and M[0 ... row] will be in reduced echelon form
  bool isDependent(std::vector<std::vector<int>>& M, std::vector<int>& v, int row);

  void generateTwoSidedFilter(const std::unordered_map<int, int>& keyValueMap,
                                                 float epsilon, int M, int s, int K);
};

class BucketedBloomierFilter {
public:
  BucketedBloomierFilter(int buckets, const std::unordered_map<int, int>& keyValueMap, float epsilon, int M, int s, int K, bool oneSided=true);
  int get(int x);
private:
  std::vector<ImprovedBloomierFilter> mBuckets;
  HashFunctor mH;
};


#endif
