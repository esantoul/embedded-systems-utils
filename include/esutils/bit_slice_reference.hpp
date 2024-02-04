#ifndef ESUTILS_BIT_SLICE_REFERENCE_HPP
#define ESUTILS_BIT_SLICE_REFERENCE_HPP

#include <algorithm>
#include <cstddef>
#include <type_traits>

namespace esutils
{
  template <size_t sliceSize, typename Referenced, typename Underlying>
  class BitSliceReference
  {
    static_assert(std::is_integral_v<Referenced>, "Referenced must be an integral type");
    static_assert(std::is_unsigned_v<Underlying>, "Underlying must be an unsigned integer type");

  public:
    constexpr BitSliceReference(Underlying *ptr, ptrdiff_t offset)
      :
      mPtr(ptr),
      mOffset(offset)
    {}

    constexpr operator Referenced() const
    {
      Referenced ret{};
      for (size_t handledBits = 0; handledBits < sliceSize;)
      {
        const size_t remainingBits = sliceSize - handledBits;

        const ptrdiff_t totalBitOffset = sliceSize * mOffset + handledBits;
        const ptrdiff_t bitOffsetInCurrentUnderlying =
          totalBitOffset % std::numeric_limits<Underlying>::digits +
          (totalBitOffset < 0) * std::numeric_limits<Underlying>::digits; // to get a positive number

        const ptrdiff_t currentUnderlyingOffset =
          totalBitOffset / std::numeric_limits<Underlying>::digits -
          (totalBitOffset < 0 ? 1 : 0);

        const auto currentPtr = mPtr + currentUnderlyingOffset;

        const size_t stepBits = std::min<size_t>(
          std::numeric_limits<Underlying>::digits - bitOffsetInCurrentUnderlying,
          remainingBits
        );

        const Underlying bitMask = std::numeric_limits<Underlying>::max()
          >> (std::numeric_limits<Underlying>::digits - stepBits)
          << bitOffsetInCurrentUnderlying;

        const ptrdiff_t currentSliceOffset = handledBits - bitOffsetInCurrentUnderlying;

        if (currentSliceOffset >= 0)
        {
          ret |= static_cast<Referenced>(*currentPtr & bitMask) << currentSliceOffset;
        }
        else
        {
          ret |= static_cast<Referenced>(*currentPtr & bitMask) >> (-currentSliceOffset);
        }

        handledBits += stepBits;
      }

      return ret;
    }

    constexpr BitSliceReference &operator=(Referenced &ref)
    {
#pragma message "TODO Implement"
      return *this;
    }

  private:
    Underlying *mPtr;
    ptrdiff_t mOffset;
  };

  // template <size_t sliceSize, typename Referenced, typename Underlying>
  // BitSliceReference<sliceSize, Referenced>(Underlying ptr, ptrdiff_t offset)->BitSliceReference<sliceSize, Referenced, Underlying>;
}

#endif // ESUTILS_BIT_SLICE_REFERENCE_HPP
