#pragma once

/**
 * Useful to get the smallest type that can fit the range 0 - N
 * Warning, this is designed for unsigned integers only
 * e.g. variables meant to store a positive array index
 */

#include "tules_commons.h"
#include <limits>

namespace tules // Template Utility Library for Embedded Systems
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

  template <uint64_t N>
  // TypeCapacity<N>::type is the smallest type that can fit the range 0 - N
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