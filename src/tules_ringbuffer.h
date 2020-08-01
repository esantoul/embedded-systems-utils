#pragma once

/**
 * A simple ring buffer with fixed size
 */

#include "tules_commons.h"
#include "tules_type_capacity.h"

namespace tules // Template Utility Library for Embedded Systems
{
  template <typename T, size_t capacity>
  class RingBuffer
  {
  private:
    T mArray[capacity];
    TypeCapacity<capacity>::type mReadPos;
    TypeCapacity<capacity>::type mWritePos;

    TypeCapacity<capacity>::type IncrementedWrapedValue(TypeCapacity<capacity>::type val)
    {
      if (++val == capacity)
        return 0;
      else
        return val;
    }

  public:
    RingBuffer() : mReadPos(0), mWritePos(0)
    {
    }

    size_t Capacity() { return capacity; }

    size_t Readable()
    {
      if (mWritePos >= mReadPos)
        return mWritePos - mReadPos;
      else
        return mWritePos + capacity - mReadPos;
    }

    size_t Writable() { return capacity - Readable(); }

    bool Write(const T &val)
    {
      if (Writable())
      {
        mArr[mWritePos] = val;
        mWritePos = IncrementedWrapedValue(mWritePos);
        return true;
      }
      return false;
    }

    void Overwrite(const T &val)
    {
      if (IncrementedWrapedValue(mWritePos) == mReadPos)
      {
        mReadPos = IncrementedWrapedValue(mReadPos);
        mArr[mWritePos] = val;
        mWritePos = IncrementedWrapedValue(mReadPos);
      }
      else
      {
        mArr[mWritePos] = val;
        mWritePos = IncrementedWrapedValue(mWritePos);
      }
    }
  };
} // namespace tules
