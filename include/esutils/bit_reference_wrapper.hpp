#ifndef ESUTILS_BIT_REFERENCE_WRAPPER_HPP
#define ESUTILS_BIT_REFERENCE_WRAPPER_HPP

#include <cstdint>
#include <limits>
#include <type_traits>

namespace esutils
{
  template <typename T>
  class BitReferenceWrapper
  {
    static_assert(std::is_unsigned_v<T>, "`T` must be an unsigned integer type");

  public:
    constexpr BitReferenceWrapper(T &data, uint8_t bitPosition)
      :
      pData(&data),
      mBitPos(bitPosition)
    {}

    constexpr operator bool() const
    {
      return (*pData >> mBitPos) & 1;
    }

    constexpr BitReferenceWrapper &operator=(bool b)
    {
      *pData &= ~(1 << mBitPos);    // Reset bit
      *pData |= (1 * b) << mBitPos; // Set bit value to b
      return *this;
    }

  private:
    T *pData;
    uint8_t mBitPos;
  };
} // namespace esutils

#endif // ESUTILS_BIT_REFERENCE_WRAPPER_HPP
