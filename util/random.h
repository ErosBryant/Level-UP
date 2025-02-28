#ifndef STORAGE_LEVELDB_UTIL_RANDOM_H_
#define STORAGE_LEVELDB_UTIL_RANDOM_H_

#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <stdint.h>
#include <cmath>
#include <vector>

namespace leveldb {

// A very simple random number generator.
class Random {
 private:
  uint32_t seed_;
    std::vector<double> cumulative_probabilities_; // 누적 분포 저장
    std::unordered_map<uint32_t, uint32_t> key_map; // 키와 사용 횟수 저장
std::vector<uint32_t> key_vec;    

 public:
  explicit Random(uint32_t s) : seed_(s & 0x7fffffffu) {
    if (seed_ == 0 || seed_ == 2147483647L) {
      seed_ = 1;
    }
  }

  uint32_t Next() {
    static const uint32_t M = 2147483647L;
    static const uint64_t A = 16807;
    uint64_t product = seed_ * A;
    seed_ = static_cast<uint32_t>((product >> 31) + (product & M));
    if (seed_ > M) {
      seed_ -= M;
    }
    return seed_;
  }

  uint32_t Uniform(int n) { return Next() % n; }

  bool OneIn(int n) { return (Next() % n) == 0; }

  uint32_t Skewed(int max_log) { return Uniform(1 << Uniform(max_log + 1)); }

uint32_t GenerateKey() {
    // 10% 확률로 이미 생성된 키 재사용 (키 목록은 vector에서 O(1)로 랜덤 접근)
    if (OneIn(10) && !key_vec.empty()) {
        return key_vec[Uniform(key_vec.size())];
    }

    // 새로운 키 생성 (0 ~ 99,99 9,999)
    uint32_t new_key = Uniform(100000000);

    // key_map에 새 키 추가 (이미 존재하면 사용 횟수 증가)
    auto result = key_map.insert({new_key, 1});
    if (!result.second) { // 이미 존재하는 경우
        result.first->second++;
    } else {  // 새 키인 경우, vector에도 추가
        key_vec.push_back(new_key);
    }
    return new_key;
}


    uint32_t Zipfian(int n, double skew) {
    if (cumulative_probabilities_.empty()) {
      // Precompute cumulative probabilities
      double denom = 0;
      for (int i = 1; i <= n; ++i) {
        denom += 1.0 / std::pow(i, skew);
      }

      double cumulative = 0;
      for (int i = 1; i <= n; ++i) {
        cumulative += (1.0 / std::pow(i, skew)) / denom;
        cumulative_probabilities_.push_back(cumulative);
      }
    }

    // Generate a uniform random number between 0 and 1
    double u = static_cast<double>(Next()) / 2147483647.0;

    // Perform binary search to find the corresponding Zipfian value
    auto it = std::lower_bound(cumulative_probabilities_.begin(), cumulative_probabilities_.end(), u);
    return std::distance(cumulative_probabilities_.begin(), it) + 1;
  }

};

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_UTIL_RANDOM_H_
