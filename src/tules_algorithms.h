#pragma once

/**
 * Algorithm functions collection
 */

#include "tules_commons.h"
#include <array>

namespace tules // Template Utility Library for Embedded Systems
{
  template <typename Iter>
  constexpr void iter_swap(Iter lhs, Iter rhs)
  {
    auto temp = *lhs;
    *lhs = *rhs;
    *rhs = temp;
  }

  template <typename Iter>
  void rotate(Iter iter_first, Iter iter_newFirst, Iter iter_end)
  {
    if (iter_newFirst == iter_first || iter_newFirst == iter_end)
      return;
    Iter ff = iter_first;
    Iter mm = iter_newFirst;
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
} // namespace tules
