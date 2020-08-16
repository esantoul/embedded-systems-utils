#pragma once

/**
 * @file tules_bool_collection.hpp
 * Definition of an object representing an array of bool optimised in terms of space
 * @author Etienne Santoul
 * @todo add comments
 * @todo make as constexpr as possible
 */

#include "tules_commons.hpp"

namespace tules // Template Utility Library for Embedded Systems
{
  template <size_t sz>
  class BoolCollection
  {
  public:
    BoolCollection() {}

    class Bit
    {
    public:
      Bit(uint8_t &ob, const uint8_t &bp) : mOriginByte(ob), mBitPosition(bp) {}

      operator bool() const
      {
        return mOriginByte & (1 << mBitPosition);
      }

      bool operator=(const bool &val)
      {
        if (val)
          mOriginByte |= 1 << mBitPosition;
        else
          mOriginByte &= ~(1 << mBitPosition);
        return bool(*this);
      }

      bool value() const
      {
        return bool(*this);
      }

      bool operator==(const bool &other) const
      {
        return bool(*this) == other;
      }

      bool operator!() const
      {
        return !bool(*this);
      }

    private:
      uint8_t &mOriginByte;
      const uint8_t mBitPosition;
    };

    class ForwardIterator
    {
    public:
      ForwardIterator(uint8_t *data) : mIndex(sz), pDat(data) {}
      ForwardIterator(size_t index, uint8_t *data) : mIndex(index), pDat(data) {}

      bool operator==(const ForwardIterator &other) const
      {
        return (other.pDat == pDat) && (other.mIndex == mIndex);
      }

      bool operator!=(const ForwardIterator &other) const
      {
        return !(other == *this);
      }

      ForwardIterator &operator++()
      {
        ++mIndex;
        return *this;
      }

      Bit operator*()
      {
        return Bit(pDat[mIndex / 8], mIndex % 8);
      }

    private:
      size_t mIndex;
      uint8_t *pDat;
    };

    ForwardIterator begin()
    {
      return ForwardIterator(0, mData);
    }

    ForwardIterator end()
    {
      return ForwardIterator(mData);
    }

    Bit operator[](size_t index)
    {
      return Bit(mData[index / 8], index % 8);
    }

    void clear()
    {
      for (size_t i = 0; i < (sz + 7) / 8; ++i)
      {
        mData[i] = 0;
      }
    }

  private:
    uint8_t mData[(sz + 7) / 8];
  };
} // namespace tules
