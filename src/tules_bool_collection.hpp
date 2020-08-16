#pragma once

/**
 * @file tules_bool_collection.hpp
 * Definition of an object representing an array of bool optimised in terms of space
 * @author Etienne Santoul
 * @todo add comments
 */

#include "tules_commons.hpp"

namespace tules // Template Utility Library for Embedded Systems
{
  template <size_t sz>
  class BoolCollection
  {
  public:
    constexpr BoolCollection() = default;

    template <typename... Args>
    constexpr BoolCollection(Args... args) : mData{static_cast<uint8_t>(args)...} {}

    class Bit
    {
    public:
      constexpr Bit(uint8_t &ob, const uint8_t &bp) : mOriginByte(ob), mBitPosition(bp) {}

      constexpr operator bool() const
      {
        return mOriginByte & (1 << mBitPosition);
      }

      constexpr bool operator=(const bool &val)
      {
        if (val)
          mOriginByte |= 1 << mBitPosition;
        else
          mOriginByte &= ~(1 << mBitPosition);
        return bool(*this);
      }

      constexpr bool value() const
      {
        return bool(*this);
      }

      constexpr bool operator==(const bool &other) const
      {
        return bool(*this) == other;
      }

      constexpr bool operator!() const
      {
        return !bool(*this);
      }

    private:
      uint8_t &mOriginByte;
      const uint8_t mBitPosition;
    };

    class CBit
    {
    public:
      constexpr CBit(const uint8_t &ob, const uint8_t &bp) : mBool{static_cast<bool>(ob & (1 << bp))} {}

      constexpr operator bool() const
      {
        return mBool;
      }

      constexpr bool value() const
      {
        return mBool;
      }

      constexpr bool operator==(const bool &other) const
      {
        return mBool == other;
      }

      constexpr bool operator!() const
      {
        return !mBool;
      }

    private:
      const bool mBool;
    };

    class ForwardIterator
    {
    public:
      constexpr ForwardIterator(uint8_t *data) : mIndex(sz), pDat(data) {}
      constexpr ForwardIterator(size_t index, uint8_t *data) : mIndex(index), pDat(data) {}

      constexpr bool operator==(const ForwardIterator &other) const
      {
        return (other.pDat == pDat) && (other.mIndex == mIndex);
      }

      constexpr bool operator!=(const ForwardIterator &other) const
      {
        return !(other == *this);
      }

      constexpr ForwardIterator &operator++()
      {
        ++mIndex;
        return *this;
      }

      constexpr Bit operator*()
      {
        return Bit(pDat[mIndex / 8], 7 - mIndex % 8);
      }

    private:
      size_t mIndex;
      uint8_t *pDat;
    };

    class CForwardIterator
    {
    public:
      constexpr CForwardIterator(const uint8_t *data) : mIndex(sz), pDat(data) {}
      constexpr CForwardIterator(size_t index, const uint8_t *data) : mIndex(index), pDat(data) {}

      constexpr bool operator==(const CForwardIterator &other) const
      {
        return (other.pDat == pDat) && (other.mIndex == mIndex);
      }

      constexpr bool operator!=(const CForwardIterator &other) const
      {
        return !(other == *this);
      }

      constexpr CForwardIterator &operator++()
      {
        ++mIndex;
        return *this;
      }

      constexpr CBit operator*() const
      {
        return {pDat[mIndex / 8], static_cast<uint8_t>(7 - mIndex % 8)};
      }

    private:
      size_t mIndex;
      const uint8_t *pDat;
    };

    constexpr ForwardIterator begin()
    {
      return {0, mData};
    }

    constexpr ForwardIterator end()
    {
      return {mData};
    }

    constexpr CForwardIterator begin() const
    {
      return {0, mData};
    }

    constexpr CForwardIterator end() const
    {
      return {mData};
    }

    constexpr CForwardIterator cbegin() const
    {
      return {0, mData};
    }

    constexpr CForwardIterator cend() const
    {
      return {mData};
    }

    constexpr CBit operator[](size_t index) const
    {
      return {mData[index / 8], static_cast<uint8_t>(7 - index % 8)};
    }

    constexpr Bit operator[](size_t index)
    {
      return {mData[index / 8], static_cast<uint8_t>(7 - index % 8)};
    }

    constexpr void clear()
    {
      for (size_t i = 0; i < (sz + 7) / 8; ++i)
      {
        mData[i] = 0;
      }
    }

  private:
    uint8_t mData[(sz + 7) / 8]{};
  };
} // namespace tules

/*

#include <cstdio>
#include <array>

constexpr std::array<uint8_t, 10> gen(const tules::BoolCollection<10> c)
{
  std::array<uint8_t, 10> a{};
  auto dat = a.data();
  for (auto el : c)
    *(dat++) = el.value();
  return a;
};

constexpr std::array<uint8_t, 10> gen2()
{
  std::array<uint8_t, 10> a{};
  auto dat = a.data();
  tules::BoolCollection<10> cc{0xF1, 0b01000000};
  for (auto el : cc)
    *(dat++) = el.value();
  *(dat - 4) = cc[7];
  return a;
};

int main()
{
  static constexpr tules::BoolCollection<10> bc{0xF1, 0b01000000};
  tules::BoolCollection<10> bc2 = bc;
  constexpr std::array<uint8_t, 10> s = gen(bc);
  constexpr std::array<uint8_t, 10> s2 = gen2();

  for (const auto &el : bc)
  {
    printf("%d ", el.value());
  }
  puts("");
  for (auto &&el : bc2)
  {
    el = true;
    printf("%d ", el.value());
  }
  puts("");
  for (const auto &el : bc2)
  {
    printf("%d ", el.value());
  }
  return s[6] + s2[7];
}

//*/