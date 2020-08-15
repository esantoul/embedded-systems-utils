#pragma once

/**
 * @file tules_static_stack.hpp
 * Definition of a stack with a capacity fixed at compile time
 * @author Etienne Santoul
 */

#include "tules_commons.hpp"
#include "tules_type_capacity.hpp"
#include "tules_optional.hpp"

namespace tules
{
  template <typename T, size_t cty>
  class StaticStack
  {
  public:
    constexpr StaticStack() = default;

    /**
     * @return the maximal number of elements contained in the StaticStack
     */
    size_t capacity() const
    {
      return cty;
    }

    /**
     * @return the current number of elements contained in the StaticStack
     */
    size_t size() const
    {
      return mSize;
    }

    /**
     * @brief Adds an element to the Stack
     * @param val the value to be added to the stack
     * @return true if the element was added else false (stack full)
     */
    bool push(const T &val)
    {
      if (mSize < cty)
      {
        mData[mSize++] = val;
        return true;
      }
      else
      {
        return false;
      }
    }

    /**
     * @brief Remove the top element from the stack
     * @return an Optional containing the top element if the StaticStack size was >= 1 else an empty Optional
     */
    Optional<T> pop()
    {
      if (mSize)
      {
        return mData[--mSize];
      }
      return {};
    }

    /**
     * @brief Finds an element in the stack
     * @param val the value to be found
     * @return a pointer to the data if it was found or nullptr
     */
    T *find(const T &val)
    {
      size_t i = find_idx(val);
      if (i != mSize)
      {
        return mData + i;
      }
      return nullptr;
    }

    /**
     * @brief Erases a value in the stack
     * @param val the value to be erased
     * @return true if the value was erased else false (the value did not exist in the stack)
     */
    bool erase(const T &val)
    {
      size_t i = find_idx(val);
      if (i != mSize)
      {
        tidy(i + 1);
        mSize--;
        return true;
      }
      return false;
    }

    /**
     * @return an iterator to the bottom of the stack
     */
    T *begin()
    {
      return mData;
    }

    /**
     * @return an iterator to the top of the stack
     */
    T *end()
    {
      return mData + mSize;
    }

  private:
    size_t find_idx(const T &val)
    {
      for (size_t i = 0; i < mSize; ++i)
      {
        if (mData[i] == val)
        {
          return i;
        }
      }
      return mSize;
    }

    void tidy(size_t i)
    {
      if (i < mSize)
      {
        mData[i - 1] = mData[i];
        tidy(i + 1);
      }
    }

    using cty_t = typename TypeCapacity<cty>::type;
    T mData[cty]{};
    cty_t mSize = 0;
  };

} // namespace tules

/*

int main()
{
  tules::StaticStack<uint8_t, 16> ss{};
  ss.push(12);
  ss.find(12);
  auto el = ss.pop();
  return el.value_or(0xFF);
}

//*/