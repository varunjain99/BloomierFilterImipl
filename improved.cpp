#include <vector>
#include <utility>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <math.h>
#include <ctime>


// C++ program Miller-Rabin primality test
// https://www.geeksforgeeks.org/primality-test-set-3-miller-rabin/

// Utility function to do modular exponentiation.
// It returns (x^y) % p
int power(int x, unsigned int y, int p)
{
	int res = 1;	 // Initialize result
	x = x % p; // Update x if it is more than or
				// equal to p
	while (y > 0)
	{
		// If y is odd, multiply x with result
		if (y & 1)
			res = (res*x) % p;

		// y must be even now
		y = y>>1; // y = y/2
		x = (x*x) % p;
	}
	return res;
}

// This function is called for all k trials. It returns
// false if n is composite and returns false if n is
// probably prime.
// d is an odd number such that d*2<sup>r</sup> = n-1
// for some r >= 1
bool miillerTest(int d, int n)
{
	// Pick a random number in [2..n-2]
	// Corner cases make sure that n > 4
	int a = 2 + rand() % (n - 4);

	// Compute a^d % n
	int x = power(a, d, n);

	if (x == 1 || x == n-1)
	return true;

	// Keep squaring x while one of the following doesn't
	// happen
	// (i) d does not reach n-1
	// (ii) (x^2) % n is not 1
	// (iii) (x^2) % n is not n-1
	while (d != n-1)
	{
		x = (x * x) % n;
		d *= 2;

		if (x == 1)	 return false;
		if (x == n-1) return true;
	}

	// Return composite
	return false;
}

// It returns false if n is composite and returns true if n
// is probably prime. k is an input parameter that determines
// accuracy level. Higher value of k indicates more accuracy.
bool isPrime(int n, int k)
{
	// Corner cases
	if (n <= 1 || n == 4) return false;
	if (n <= 3) return true;

	// Find r such that n = 2^d * r + 1 for some r >= 1
	int d = n - 1;
	while (d % 2 == 0)
		d /= 2;

	// Iterate given nber of 'k' times
	for (int i = 0; i < k; i++)
		if (!miillerTest(d, n))
			return false;

	return true;
}


// Given number n, returns its distinct prime factors in sorted order
// Adapted from
// https://www.geeksforgeeks.org/print-all-prime-factors-of-a-given-number/
std::vector<int> getDistinctPrimeFactors(int n) {
    std::vector<int> factors;
    // Is 2 a factor?
    if (n % 2 == 0) {
      factors.push_back(2);
    }
    while (n % 2 == 0) {
        n = n / 2;
    }

    // Is odd i a factor?
    for (int i = 3; i <= sqrt(n); i = i+2) {
        if (n % i == 0) {
          factors.push_back(i);
        }
        while (n % i == 0) {
            n = n / i;
        }
    }

    // Was n != 2 a prime to begin with?
    if (n > 2) {
        factors.push_back(n);
    }

    return factors;
}



class HashFunctor {
public:
	HashFunctor() {}
  HashFunctor(int range, int prime) : mRange(range), mPrime(prime) {
    mA = rand() % mPrime;
    mB = rand() % mPrime;
  }

	HashFunctor(int a, int b, int range, int prime) :
						mA(a), mB(b), mRange(range), mPrime(prime) {}

	int operator()(int x) const {
		return ((mA * x + mB) % mPrime) % mRange;
	}
private:
	int mA;
	int mB;
	int mRange;
	int mPrime;
};


class ImprovedBloomierFilter {
public:
  ImprovedBloomierFilter(const std::unordered_map<int, int>& keyValueMap, float epsilon, int M, int s, int K);
  int get(int x);

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


int ImprovedBloomierFilter::get(int x) {
  int i = 0;
  int val = 0;
  while (i < mHashBlocks.size()) {
    val = mH0(x);
    for (int j = 0; j < mS; ++j) {
      val = (val + mHashBlocks[i][j](x) * mG[mHashBlocks[i][j + mS](x) % mQ]) % mP;
    }
    if (val < (1 << mK)) {
      return val;
    }
    ++i;
  }
  return val;
}

void ImprovedBloomierFilter::generateTwoSidedFilter(const std::unordered_map<int, int>& keyValueMap,
                                               float epsilon, int M, int s, int K) {
  mN = keyValueMap.size();
  mS = s;
  mK = K;
  mM = M;
  mEpsilon = epsilon;

  srand(time(NULL)); // Seed the time
  computeQ();
  mG = std::vector<int>(mQ, 0);
  computeP();
  computePInverses();
  genTable(keyValueMap);
}

ImprovedBloomierFilter::ImprovedBloomierFilter(const std::unordered_map<int, int>& keyValueMap,
                                               float epsilon, int M, int s, int K) {
  bool oneSided;
  do  {
    oneSided = true;
    generateTwoSidedFilter(keyValueMap, epsilon, M, s, K);
    for (auto it = keyValueMap.begin(); it != keyValueMap.end(); ++it) {
      if (get(it->first) != it->second) {
        oneSided = false;
        break;
      }
    }
  } while(!oneSided);
}

void ImprovedBloomierFilter::computeG(const std::vector<std::vector<int>>& M, const std::vector<int>& v) {

  int curRow = 0;
  int curCol = 0;
  while (curRow < mN && curCol < mQ) {
    if (M[curRow][curCol] == 0) {
      mG[curCol] = 0;
    } else {
      mG[curCol] = v[curRow];
      curRow++;
    }
    curCol++;
  }


  while (curCol < mQ) {
    mG[curCol] = 0;
    curCol++;
  }
}

bool ImprovedBloomierFilter::isDependent(std::vector<std::vector<int>>& M, std::vector<int>& v, int row) {
  // Store original v value in case we have to revert
  int original_v = v[row];

  // Propagate downward: Zero out column's with leading digit above
  int firstNonzeroIndex = 0;
  for (int i = 0; i < row; ++i) {
    while (M[i][firstNonzeroIndex] == 0 && firstNonzeroIndex < mQ) {
      firstNonzeroIndex++;
    }
    if (M[row][firstNonzeroIndex] != 0) {
      int factor = (mPInverses[M[i][firstNonzeroIndex]] * M[row][firstNonzeroIndex]) % mP;
      for (int j = firstNonzeroIndex; j < mQ; ++j) {
        M[row][j] -= (factor * M[i][j]) % mP;
        if (M[row][j] < 0) {
          M[row][j] += mP;
        }
      }
      v[row] -= (factor * v[i]) % mP;
      if (v[row] < 0) {
        v[row] += mP;
      }
    }
  }

  // If row is all 0's, reset v with stored value and return true
  int startIndex = -1;
  for (int j = 0; j < mQ; ++j) {
    if (M[row][j] != 0) {
      startIndex = j;
      break;
    }
  }

  if (startIndex == -1) {
    v[row] = original_v;
    return true;
  }

  // Normalize row so that leading coefficient is 1
  int inverse = mPInverses[M[row][startIndex]];
  for (int j = startIndex; j < mQ; ++j) {
    M[row][j] = (M[row][j] * inverse) % mP;
  }
  v[row] = (v[row] * inverse) % mP;

  // Propagate leading 1 in row upwards
  for (int i = row - 1; i >= 0; --i) {
    if (M[i][startIndex] != 0) {
      int factor = M[i][startIndex];
      for (int j = startIndex; j < mQ; ++j) {
        M[i][j] -= (factor * M[row][j]) % mP;
        if (M[i][j] < 0) {
          M[i][j] += mP;
        }
      }
      v[i] -= (factor * v[row]) % mP;
      if (v[i] < 0) {
        v[i] += mP;
      }
    }
  }

  // Swap row into correct position (deletion and insert)
  int r = 0;
  int c = 0;
  while (r < row && c < startIndex) {
    if (M[r][c] == 1) {
      r++;
    }
    c++;
  }

  if (r != row) {
    std::vector<int> movingRow = M[row];
    int movingVal = v[row];
    M.erase(M.begin() + row);
    M.insert(M.begin() + r, movingRow);
    v.erase(v.begin() + row);
    v.insert(v.begin() + r, movingVal);
  }
  return false;

}


void ImprovedBloomierFilter::genTable(const std::unordered_map<int, int>& keyValueMap) {
  // Intialize mN by mQ table M to 0's
  std::vector<std::vector<int>> M;
  for (int i = 0; i < mN; ++i) {
    M.push_back(std::vector<int>(0));
    for (int j = 0; j < mQ; ++j) {
      M[i].push_back(0);
    }
  }

  std::vector<int> v = std::vector<int>(mN, 0);

  // Initialize mH0 hash function
  mH0 = HashFunctor(mP, mP);

  // Loop through key value pairs, filling M table
  auto kv_it = keyValueMap.begin();
  for (int i = 0; i < mN; ++i) {
    int target = (kv_it->second - mH0(kv_it->first)) % mP;
    if (target < 0) {
      target += mP;
    }
    v[i] = target;

    // Repeatedly try filling ith row of M, generating new hash blocks if necessary
    int j = 0;
    do {
      // Generate new hash block if needed
      if (j == mHashBlocks.size()) {
        mHashBlocks.push_back(std::vector<HashFunctor>(0));
        for (int k = 0; k < mS; ++k) {
          mHashBlocks[j].push_back(HashFunctor(mP, mP));
        }
        for (int k = mS; k < 2 * mS; ++k) {
          mHashBlocks[j].push_back(HashFunctor(mQ, mQ));
        }
      }

      // Set values from  current hash block, namely mHashBlocks[j]
      for (int k = 0; k < mS; ++k) {
        M[i][mHashBlocks[j][k + mS](kv_it->first)] = (M[i][mHashBlocks[j][k + mS](kv_it->first)] +
                                                     mHashBlocks[j][k](kv_it->first)) % mP;
      }
      ++j;
    } while(isDependent(M, v, i));
    ++kv_it;
  }
  computeG(M, v);

}


void ImprovedBloomierFilter::computeQ() {
  // Find prime q > N(1 + epsilon)
  mQ = (int) (mN * (1 + mEpsilon));

  mQ++;
  while (true) {
    if (isPrime(mQ, 20)) {
      break;
    }
    mQ++;
  }
}

void ImprovedBloomierFilter::computeP() {
  // Find distinct prime factors of q - 1
  std::vector<int> factors = getDistinctPrimeFactors(mQ - 1);

  // Find g
  int g;
  while (true) {
    g = rand() % mQ;

    bool divisible = false;
    for (size_t i = 0; i < factors.size(); ++i) {
      if (g % factors[i] == 0) {
        divisible = true;
        break;
      }
    }
    if (!divisible) {
      break;
    }
  }

  // Cache all generators
  std::unordered_set<int> generators;
  int g_val = g;
  for (int i = 1; i < mQ; ++i) {
    bool divisible = false;
    for (size_t j = 0; j < factors.size(); ++j) {
      if (i % factors[j] == 0) {
        divisible = true;
        break;
      }
    }
    if (!divisible) {
      generators.insert(i);
    }
    g_val = (g_val * g) % mQ;
  }


  // Find prime p
  do {
    mP = rand() % (1 << mM) + (1 << mM);
  } while (!isPrime(mP, 20) || generators.find(mP % mQ) != generators.end());
}

void ImprovedBloomierFilter::computePInverses() {
  for (int i = 0; i < mP; ++i) {
    if (mPInverses.find(i) == mPInverses.end()) {
      for (int j = 0; j < mP; ++j) {
        if ((i * j) % mP == 1) {
          mPInverses[i] = j;
          mPInverses[j] = i;
        }
      }
    }
  }
}


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
