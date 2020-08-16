#pragma once

/**
 * @file tules_static_set.hpp
 * Definition of a set data structure with a capacity fixed at compile time
 * @author Etienne Santoul
 * @todo add comments
 * @todo make uint32_t version (limited to uint16_t capacity for now)
 */

#include "tules_type_capacity.hpp"

namespace tules
{
  template <typename T, uint16_t cty>
  class StaticSet
  {
    using cty_t = typename TypeCapacity<cty>::type;

  public:
    constexpr StaticSet()
        : mSize{0},
          mStatus{},
          mData{},
          mForwardIndex{},
          mChild{},
          mMask{make_mask(cty)}
    {
    }

    constexpr void clear()
    {
      mStatus.clear();
      mSize = 0;
    }

    template <bool constant>
    class ForwardIterator
    {
      using set_t = std::conditional_t<constant, const StaticSet<T, cty>, StaticSet<T, cty>>;
      using unref_t = std::conditional_t<constant, const T, T>;

    public:
      constexpr ForwardIterator(set_t *parent, cty_t i = cty)
          : mParent(parent),
            mIdx(i)
      {
      }

      constexpr unref_t &operator*()
      {
        return mParent->mData[mIdx];
      }

      constexpr unref_t &operator*() const
      {
        return mParent->mData[mIdx];
      }

      constexpr bool operator!=(const ForwardIterator &rhs) const { return (mParent != rhs.mParent || mIdx != rhs.mIdx); }
      constexpr bool operator==(const ForwardIterator &rhs) const { return not(*this != rhs); };

      constexpr ForwardIterator &operator++()
      {
        do
          ++mIdx;
        while (mIdx < cty and mParent->mStatus[mIdx] == false);
        return *this;
      }

    private:
      set_t *mParent;
      cty_t mIdx;
    };

    constexpr ForwardIterator<false> begin()
    {
      cty_t idx = 0;
      while (idx < cty and mStatus[idx] == false)
        idx++;
      return {this, idx};
    }

    constexpr ForwardIterator<false> end()
    {
      return {this};
    }

    constexpr ForwardIterator<true> begin() const
    {
      cty_t idx = 0;
      while (idx < cty and mStatus[idx] == false)
        idx++;
      return {this, idx};
    }

    constexpr ForwardIterator<true> end() const
    {
      return {this};
    }

    constexpr ForwardIterator<true> cbegin() const
    {
      cty_t idx = 0;
      while (idx < cty and mStatus[idx] == false)
        idx++;
      return {this, idx};
    }

    constexpr ForwardIterator<true> cend() const
    {
      return {this};
    }

    constexpr size_t size() const
    {
      return mSize;
    }

    constexpr size_t capacity() const
    {
      return cty;
    }

    constexpr ForwardIterator<false> insert(const T &el)
    {
      if (mSize < cty)
      {
        cty_t i = hash(el);
        cty_t parent = i;
        cty_t root = i;
        // Most common case: the root slot is empty
        if (mStatus[i] == false)
        {
          mSize++;
          mStatus[i] = true;
          mData[i] = el;
          mChild[i] = cty;
          mForwardIndex[i] = cty;
          return {this, i};
        }

        // Else the root slot is populated
        bool forwarding = false;
        if (hash(mData[i]) != root) // If the root slot is populated by an element with a different index as the root index
        {
          if (mForwardIndex[i] != cty) // There are some elements in the current set with the same index as the element to be inserted
            i = mForwardIndex[i];      // So jump to the next element with same index as the element to be inserted
          else                         // There are no element in the current set with the same index as the element to be inserted
            forwarding = true;         // So the root element forwarding has to be set
        }

        // Iterate through all children
        bool remaining_chained_children = true;
        while (mStatus[i]) // Continue to iterate while slots are populated
        {
          if (mData[i] == el)
            return {this, i};
          if (remaining_chained_children == false) // No more children to iterate through
          {
            increment_wrap(i);
          }
          else // More children to iterate through
          {
            if (mChild[i] == cty) // The last child is found! Store this index and continue iterating
            {
              parent = i;
              remaining_chained_children = false;
              increment_wrap(i);
            }
            else // More children in chain so jump to next child
            {
              i = mChild[i];
            }
          }
        }
        // A free slot has been found so populate it and update all adequate data
        mSize++;
        mStatus[i] = true;
        mData[i] = el;
        mChild[i] = cty;
        mForwardIndex[i] = cty;
        if (forwarding)
          mForwardIndex[root] = i;
        else
          mChild[parent] = i;

        return {this, i};
      }
      else
      {
        return end();
      }
    }

    constexpr ForwardIterator<false> find(const T &el)
    {
      return {this, find_element_index(el)};
    }

    constexpr ForwardIterator<true> find(const T &el) const
    {
      return {this, find_element_index(el)};
    }

    constexpr bool erase(const T &el)
    {
      cty_t i = find_element_index(el);
      if (i == cty)
        return false;
      erase_cell_recursive(i);
      mSize--;
      return true;
    }

  private:
    constexpr cty_t find_element_index(const T &el) const
    {
      cty_t i = hash(el);
      if (mStatus[i])
      {
        if (mData[i] == el)
          return i;
        if (mForwardIndex[i] != cty)
          i = mForwardIndex[i];
        do
        {
          if (mData[i] == el)
            return i;
          i = mChild[i];
        } while (i != cty);
      }
      return cty;
    }

    constexpr void erase_cell_recursive(cty_t i) // TODO Extensive tests, there seems to be a bug here
    {
      if (mChild[i] == cty) // Last child in children chain
      {
        if (mForwardIndex[i] == cty) // No forwarding from this cell
        {
          mStatus[i] = false;
        }
        else // Copy forwarded cell and recursive erase forwarded cell
        {
          mData[i] = mData[mForwardIndex[i]];
          erase_cell_recursive(mForwardIndex[i]);
          if ((mStatus[mForwardIndex[i]] == false) || (hash(mData[mForwardIndex[i]]) != hash(mData[i])))
            mChild[i] = cty;
          else
            mChild[i] = mForwardIndex[i];
          mForwardIndex[i] = cty;
        }
      }
      else
      {
        mData[i] = mData[mChild[i]];
        erase_cell_recursive(mChild[i]);
        if ((mStatus[mChild[i]] == false) || (hash(mData[mChild[i]]) != hash(mData[i])))
          mChild[i] = cty;
      }
    }

    constexpr uint16_t make_mask(const uint16_t x) const
    {
      if (x == 0)
        return 0;
      return static_cast<uint32_t>(-1) >> (__builtin_clz(x) + 1);
    }

    constexpr uint16_t hash(const T &el) const
    {
      if constexpr (__builtin_popcount(cty) == 1)
      {
        return el & mMask;
      }
      else
      {
        uint16_t h1 = 0xDEBF * static_cast<uint16_t>(el);
        uint32_t hh = h1 * cty;
        return hh >> 16;
      }
    }

    constexpr void increment_wrap(cty_t &i)
    {
      i += 1;
      if (i >= cty)
        i = 0;
    }

    cty_t mSize;
    bool mStatus[cty];
    T mData[cty];
    cty_t mForwardIndex[cty];
    cty_t mChild[cty];
    uint16_t mMask;
  };

} // namespace tules

/*

constexpr auto gen = []() {
  tules::StaticSet<uint8_t, 10> ss;
  ss.insert(5);
  ss.insert(97);
  auto f = ss.find(77);
  return f == ss.end() ? 2 : *f;
};

template <size_t N>
uint8_t findValueOr(const tules::StaticSet<uint8_t, N> &set, uint8_t valuetofind, uint8_t alternative)
{
  auto f = set.find(valuetofind);
  return f == set.end() ? alternative : *f;
}

template <typename T>
constexpr uint8_t CUI8(const T &v)
{
  return static_cast<uint8_t>(v);
}

int main()
{
  constexpr auto val = gen();
  tules::StaticSet<uint8_t, 12> s1;
  s1.insert(24);
  return findValueOr<12>(s1, CUI8(24), CUI8(3)) * val;
}

constexpr size_t sss = sizeof(tules::StaticSet<int, 128>);

//*/
