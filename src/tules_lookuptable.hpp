#pragma once

/**
 * @file tules_lookuptable.hpp
 * Definition of the LookUpTable class. This class generates a lookup table at compile time.
 * @author Etienne Santoul
 */

#include "tules_commons.hpp"

namespace tules // Template Utility Library for Embedded Systems
{
  /**
   * @brief A lookup table generated at compile time
   * @tparam T the type that is being contained by the lookup table
   * @tparam capacity the number of elements in the lookup table
   */
  template <typename T, size_t capacity>
  class LookUpTable
  {
  public:
    /**
     * @brief Constructor
     * @param foo the function used to generate the lookup table values by being fed successive integers in the range [0, capacity[
     */
    constexpr LookUpTable(T (*foo)(size_t))
    {
      for (int i = 0; i < capacity; ++i)
        mData[i] = foo(i);
    }

    LookUpTable() = delete;
    LookUpTable(const LookUpTable &other) = delete;
    LookUpTable(LookUpTable &&other) = delete;
    LookUpTable &operator=(const LookUpTable &other) = delete;
    LookUpTable &operator=(LookUpTable &&other) = delete;

    /**
     * @brief Array index operator
     * @param index the index of the lookup table value
     * @return a copy of the lookup table value
     */
    constexpr T operator[](size_t index) const
    {
      return mData[index];
    }

  private:
    T mData[capacity]{};
  };
} // namespace tules

/*
#include "tules_algorithms.hpp"

int main()
{
  constexpr auto round = [](double d) {
    return int(d + 0.5);
  };

  constexpr auto myFun = [](size_t i) -> uint8_t {
    return round(63 * tules::curvegen<-127>(i / 255.0));
  };

  constexpr tules::LookUpTable<uint8_t, 256> lut{myFun};
  constexpr int vv = lut[128];
  return vv;
}
//*/