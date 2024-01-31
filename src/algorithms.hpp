#ifndef ESUTILS_ALGORITHMS_HPP
#define ESUTILS_ALGORITHMS_HPP

/**
 * @file algorithms.hpp
 * A collection of algorithms
 * @author Etienne Santoul
 */

#include <cstdint>
#include <array>

namespace esutils
{
  template <typename Iter>
  constexpr void iter_swap(Iter lhs, Iter rhs)
  {
    auto temp = *lhs;
    *lhs = *rhs;
    *rhs = temp;
  }

  template <typename ForwardIterator>
  void rotate(ForwardIterator iter_first, ForwardIterator iter_newFirst, ForwardIterator iter_end)
  {
    if (iter_newFirst == iter_first || iter_newFirst == iter_end)
      return;
    ForwardIterator ff = iter_first;
    ForwardIterator mm = iter_newFirst;
    while (ff != iter_newFirst && mm != iter_end)
    {
      iter_swap(ff++, mm++);
    }

    if (ff == iter_newFirst)
    {
      rotate(ff, mm, iter_end);
    }
    else
    {
      rotate(ff, iter_newFirst, iter_end);
    }
  }

  /**
   * @brief Generates values of the polynomia a/2 * x^2 + (1-a/2) * x.
   * This function is meant to be used for values in the range [0, 1] to create nonlinearity
   * @tparam alpha is an int8_t that is used to compute a = alpha/127
   * @tparam T type of x and of return value
   * @param x argument value
   */
  template <int8_t alpha, typename T>
  constexpr T curvegen(T x)
  {
    T a = alpha / static_cast<T>(0x7F);
    return a / static_cast<T>(2) * x * x + (static_cast<T>(1) - a / static_cast<T>(2)) * x;
  }
} // namespace esutils

#endif // ESUTILS_ALGORITHMS_HPP
