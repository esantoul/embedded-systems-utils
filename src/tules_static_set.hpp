#pragma once

/**
 * @file tules_static_set.hpp
 * Definition of a set data structure with a capacity fixed at compile time
 * @author Etienne Santoul
 * @todo add comments
 * @todo make as constexpr as possible
 */

#include "tules_type_capacity.hpp"

namespace tules
{
  template <typename T, uint16_t cty>
  class StaticSet
  {
  public:
    StaticSet()
        : mSize{0},
          mStatus{},
          mData{},
          mForwardIndex{},
          mChild{},
          mMask{make_mask(cty)}
    {
    }

    void clear()
    {
      std::fill(mStatus, mStatus + cty, false); // TODO replace mStatus by a BoolCollection and use method clear
      mSize = 0;
    }

    class ForwardIterator
    {
    public:
      ForwardIterator(StaticSet<T, cty> *parent, uint16_t i = cty)
          : mParent(parent),
            mIdx(i)
      {
      }

      T &operator*()
      {
        return mParent->mData[mIdx];
      }

      bool operator!=(const ForwardIterator &rhs) const { return (mParent != rhs.mParent || mIdx != rhs.mIdx); }
      bool operator==(const ForwardIterator &rhs) const { return not(*this != rhs); };
      ForwardIterator &operator++()
      {
        do
          ++mIdx;
        while (mIdx < cty and mParent->mStatus[mIdx] == false);
        return *this;
      }

    protected:
      StaticSet *mParent;
      uint16_t mIdx;
    };

    ForwardIterator begin()
    {
      uint16_t idx = 0;
      while (idx < cty and mStatus[idx] == false)
        idx++;
      return ForwardIterator(this, idx);
    }

    ForwardIterator end()
    {
      return ForwardIterator(this);
    }

    uint16_t size() const
    {
      return mSize;
    }

    uint16_t capacity() const
    {
      return cty;
    }

    ForwardIterator insert(const T &el)
    {
      if (mSize < cty)
      {
        uint16_t i = hash(el);
        uint16_t parent = i;
        uint16_t root = i;
        // Most common case: the root slot is empty
        if (mStatus[i] == false)
        {
          mSize++;
          mStatus[i] = true;
          mData[i] = el;
          mChild[i] = cty;
          mForwardIndex[i] = cty;
          return ForwardIterator(this, i);
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
            return ForwardIterator(this, i);
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

        return ForwardIterator(this, i);
      }
      else
      {
        return end();
      }
    }

    ForwardIterator find(const T &el)
    {
      return ForwardIterator(this, find_element_index(el));
    }

    bool erase(const T &el)
    {
      uint16_t i = find_element_index(el);
      if (i == cty)
        return false;
      erase_cell_recursive(i);
      mSize--;
      return true;
    }

  protected:
    uint16_t find_element_index(const T &el) const
    {
      uint16_t i = hash(el);
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

    void erase_cell_recursive(uint16_t i) // TODO Extensive tests, there seems to be a bug here
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

    uint16_t hash(const T &el) const
    {
      if constexpr (__builtin_popcount(cty) == 1)
      {
        return el & mMask;
      }
      else
      {
        uint16_t h1;
        uint32_t hh;
        h1 = 0xDEBF * static_cast<uint16_t>(el);
        hh = h1 * cty;
        return hh >> 16;
      }
    }

    void increment_wrap(uint16_t &i)
    {
      i += 1;
      if (i >= cty)
        i = 0;
    }

    uint16_t mSize;
    bool mStatus[cty];
    T mData[cty];
    uint16_t mForwardIndex[cty];
    uint16_t mChild[cty];
    uint16_t mMask;
  };

} // namespace tules
