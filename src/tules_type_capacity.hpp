#pragma once

/**
 * @file Header file for TypeCapacity, a type that is the smallest unsigned int that can represent any value in the range [0, N]
 * @author Etienne Santoul
 */

#include "tules_commons.hpp"
#include <limits>

namespace tules // Template Utility Library for Embedded Systems
{
  template <uint8_t twoPwrNBytes>
  struct TypeCapacityHelper;

  template <>
  struct TypeCapacityHelper<3>
  {
    using type = uint64_t;
  };

  template <>
  struct TypeCapacityHelper<2>
  {
    using type = uint32_t;
  };

  template <>
  struct TypeCapacityHelper<1>
  {
    using type = uint16_t;
  };

  template <>
  struct TypeCapacityHelper<0>
  {
    using type = uint8_t;
  };

  /**
   * @brief TypeCapacity<N>::type is the smallest unsigned type that can fit the range [0, N]
   * @param N the maximal unsigned value that is going to be stored in the variable (included)
   */
  template <uint64_t N>
  class TypeCapacity
  {
  public:
    using type = typename TypeCapacityHelper<(N > static_cast<uint64_t>(std::numeric_limits<uint32_t>::max())) +
                                             (N > static_cast<uint64_t>(std::numeric_limits<uint16_t>::max())) +
                                             (N > static_cast<uint64_t>(std::numeric_limits<uint8_t>::max()))>::type;
  };
} // namespace tules

/*
#include <typeinfo>
int main()
{
  using test_type = tules::TypeCapacity<10000>::type;
  constexpr uint8_t nBytes = sizeof(test_type);
  return (typeid(test_type) == typeid(uint16_t)) * nBytes;
}
//*/