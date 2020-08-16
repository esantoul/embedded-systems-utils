#pragma once

/**
 * @file tules_bool_collection.hpp
 * Definition of an object representing an array of bool optimised in terms of space
 * @author Etienne Santoul
 */

#include "tules_commons.hpp"

namespace tules // Template Utility Library for Embedded Systems
{
  template <size_t sz>
  class BoolCollection
  {
  public:
    /**
     * @brief Default constructor
     */
    constexpr BoolCollection() = default;

    /**
     * @brief List initializer constructor
     */
    template <typename... Args>
    constexpr BoolCollection(Args... args) : mData{static_cast<uint8_t>(args)...} {}

    /**
     * @brief Represents a bit an enable interaction with it
     */
    class Bit
    {
    public:
      /**
       * @brief Constructor
       * @param ob Origin byte
       * @param bp Bit position in the byte (must be in the range [0, 7])
       */
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

    /**
     * @brief Represents a constant bit an enable interaction with it
     */
    class CBit
    {
    public:
      /**
       * @brief Constructor
       * @param ob Origin byte
       * @param bp Bit position in the byte (must be in the range [0, 7])
       */
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

    /**
     * @brief Forward iterator object.
     * Returns a Bit when dereferenced
     */
    class ForwardIterator
    {
    public:
      /**
       * @brief Constructor for an iterator to the begin of the BoolCollection
       * @param data the BoolCollection values uint8_t array pointer
       */
      constexpr ForwardIterator(uint8_t *data) : mIndex(sz), pDat(data) {}

      /**
       * @brief Constructor for an iterator at a given bit of the BoolCollection
       * @param index the index of the bit to which the iterator should point to
       * @param data the BoolCollection values uint8_t array pointer
       */
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

    /**
     * @brief Constant Forward iterator object.
     * Returns a CBit when dereferenced
     */
    class CForwardIterator
    {
    public:
      /**
       * @brief Constructor for an iterator to the begin of the BoolCollection
       * @param data the BoolCollection values uint8_t array pointer
       */
      constexpr CForwardIterator(const uint8_t *data) : mIndex(sz), pDat(data) {}

      /**
       * @brief Constructor for an iterator at a given bit of the BoolCollection
       * @param index the index of the bit to which the iterator should point to
       * @param data the BoolCollection values uint8_t array pointer
       */
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

    /**
     * @return a ForwardIterator to the begin of the collection
     */
    constexpr ForwardIterator begin()
    {
      return {0, mData};
    }

    /**
     * @return a ForwardIterator to the end of the collection
     */
    constexpr ForwardIterator end()
    {
      return {mData};
    }

    /**
     * @return a CForwardIterator to the begin of the collection
     */
    constexpr CForwardIterator begin() const
    {
      return {0, mData};
    }

    /**
     * @return a CForwardIterator to the end of the collection
     */
    constexpr CForwardIterator end() const
    {
      return {mData};
    }

    /**
     * @return a CForwardIterator to the begin of the collection
     */
    constexpr CForwardIterator cbegin() const
    {
      return {0, mData};
    }

    /**
     * @return a CForwardIterator to the end of the collection
     */
    constexpr CForwardIterator cend() const
    {
      return {mData};
    }

    /**
     * @brief Array subscript operator
     * @param index the index of the desired bit
     */
    constexpr Bit operator[](size_t index)
    {
      return {mData[index / 8], static_cast<uint8_t>(7 - index % 8)};
    }

    /**
     * @brief Array subscript operator
     * @param index the index of the desired bit
     */
    constexpr CBit operator[](size_t index) const
    {
      return {mData[index / 8], static_cast<uint8_t>(7 - index % 8)};
    }

    /**
     * @brief Fills the bool collection with false
     */
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