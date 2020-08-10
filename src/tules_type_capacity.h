#pragma once

/**
 * @file Header file for TypeCapacity, a type that is the smallest unsigned int that can represent any value in the range [0, N]
 * @author Etienne Santoul
 */

#include "tules_commons.h"
#include <limits>

/**
 * @brief Template Utility Library for Embedded Systems
 */
namespace tules
{
  template <uint8_t twoPwrNBytes>
  struct TypeCapacityHelper;

  template <>
  struct TypeCapacityHelper<3>
  {
    typedef uint64_t type;
  };

  template <>
  struct TypeCapacityHelper<2>
  {
    typedef uint32_t type;
  };

  template <>
  struct TypeCapacityHelper<1>
  {
    typedef uint16_t type;
  };

  template <>
  struct TypeCapacityHelper<0>
  {
    typedef uint8_t type;
  };

  /**
   * @brief TypeCapacity<N>::type is the smallest unsigned type that can fit the range [0, N]
   * @param N the maximal unsigned value that is going to be stored in the variable (included)
   */
  template <uint64_t N>
  class TypeCapacity
  {
  public:
    typedef typename TypeCapacityHelper<(N > static_cast<uint64_t>(std::numeric_limits<uint32_t>::max())) +
                                        (N > static_cast<uint64_t>(std::numeric_limits<uint16_t>::max())) +
                                        (N > static_cast<uint64_t>(std::numeric_limits<uint8_t>::max()))>::type type;
  };
} // namespace tules

/*
#include <typeinfo>
int main()
{
  using test_type = tules::TypeCapacity<10000>::type;
  uint8_t nBytes = sizeof(test_type);
  return (typeid(test_type) == typeid(uint16_t)) * nBytes;
}
//*/