#pragma once

/**
 * @file A simple ring buffer with fixed size
 * @author Etienne Santoul
 */

#include "tules_commons.h"
#include "tules_type_capacity.h"
#include "tules_optional.h"
#include "tules_algorithms.h"

namespace tules // Template Utility Library for Embedded Systems
{
  /**
   * @brief A structure containing all the possible status code returned by RingBuffer methods
   */
  struct RINGBUFFER_STATUS
  {
    static constexpr uint8_t OK = 0x00;
    static constexpr uint8_t DATA_OVERWRITTEN = 0x01;
    static constexpr uint8_t NOT_ENOUGH_SPACE = 0x02;
    static constexpr uint8_t NO_DATA_WRITTEN = 0x80;
  };

  /**
   * @brief A contiguous chunk of data that is cycled through
   * @param T the type that is being contained
   * @param capacity the maximum number of elements that can be contained
   * @todo Check whether a destructor should be called on clear
   * @todo Add cyclic index class for mReadPos & mWritePos
   */
  template <typename T, size_t capacity>
  class RingBuffer
  {
  private:
    T mData[capacity];
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
    /**
     * @brief Default constructor
     */
    RingBuffer() : mReadPos(0), mWritePos(0)
    {
    }

    /**
     * @brief Resets the RingBuffer
     */
    void Clear()
    {
      mReadPos = mWritePos = 0;
    }

    /**
     * @return The maximal number of elements that can be contained in the RingBuffer
     */
    size_t Capacity() { return capacity; }

    /**
     * @return The number of elements that can be read from the RingBuffer
     */
    size_t Readable()
    {
      if (mWritePos >= mReadPos)
        return mWritePos - mReadPos;
      else
        return mWritePos + capacity - mReadPos;
    }

    /**
     * @return The number of elements that can be written in the RingBuffer before it is full
     */
    size_t Writable() { return capacity - Readable(); }

    /**
     * @brief Adds a single element to the RingBuffer
     * @param val The value to be written
     * @return True if the element was added, else if the RingBuffer is already full
     */
    bool Write(const T &val)
    {
      if (Writable())
      {
        mData[mWritePos] = val;
        mWritePos = IncrementedWrapedValue(mWritePos);
        return true;
      }
      return false;
    }

    /**
     * @brief Read a single element of the RingBuffer. If an element is read, it is consumed from the RingBuffer.
     * @return A tules::Optional containing the first readable value if there
     * is some value to read or nothing if there a no elements to read
     */
    Optional<T> Read()
    {
      if (Readable())
      {
        Optional<T> ret{mData[mReadPos]};
        mReadPos = IncrementedWrapedValue(mReadPos);
        return ret;
      }
      return {};
    }

    /**
     * @brief Reads multiple elements of the RingBuffer. If elements are read, they are consumed from the RingBuffer.
     * @param size The number of elements to read
     * @return A pointer to the first element of the array of read elements if the read was successful or nullptr if the read failed
     * @todo Maybe make a version that returns a custom iterator that avoids rotation of mData
     */
    T *Read(size_t size)
    {
      if (size <= Readable())
      {
        if (capacity - mReadPos > size) // Need to rotate mData in order to have a contiguous chunk of retured data
        {
          tules::rotate(mData, mData + mReadPos, mData + capacity);
          mWritePos = mReadPos > mWritePos ? mWritePos + capacity - mReadPos : mWritePos - mReadPos;
          mReadPos = 0;
          return mData;
        }
        else
        {
          return mData + mReadPos;
        }
      }
      // Cannot read that much data
      return nullptr;
    }

    /**
     * @brief Adds a single element to the RingBuffer. If the RingBuffer is full, it overwrites the first value to read.
     * @param val The value to be written
     * @return A tules::RINGBUFFER_STATUS status code
     */
    uint8_t Overwrite(const T &val)
    {
      if (IncrementedWrapedValue(mWritePos) == mReadPos) // Overwriting
      {
        mReadPos = IncrementedWrapedValue(mReadPos);
        mData[mWritePos] = val;
        mWritePos = mReadPos;
        return RINGBUFFER_STATUS::DATA_OVERWRITTEN; // Some data has been overwritten
      }
      else // Simply writing
      {
        mData[mWritePos] = val;
        mWritePos = IncrementedWrapedValue(mWritePos);
        return RINGBUFFER_STATUS::OK; // No data has been overwritten
      }
    }

    /**
     * @brief Adds multiple elements to the RingBuffer. If the RingBuffer is full, it overwrites values that are readable.
     * Fails if trying to add more elements than the RingBuffer can contain
     * @param array A pointer to the first element to be added to the RingBuffer
     * @param length The number of elements to be added to the RingBuffer
     * @return A tules::RINGBUFFER_STATUS status code
     */
    uint8_t Overwrite(const T *array, size_t length)
    {
      if (length > capacity)
      {
        return RINGBUFFER_STATUS::NO_DATA_WRITTEN | RINGBUFFER_STATUS::NOT_ENOUGH_SPACE
      }
      else if (length > Writable())
      {
        for (size_t i = 0; i < length; ++i)
        {
          mData[mWritePos] = array[i];
          mWritePos = IncrementedWrapedValue(mWritePos);
        }
        mReadPos = mWritePos;
        return RINGBUFFER_STATUS::DATA_OVERWRITTEN;
      }
      else
      {
        for (size_t i = 0; i < length; ++i)
        {
          mData[mWritePos] = array[i];
          mWritePos = IncrementedWrapedValue(mWritePos);
        }
        return RINGBUFFER_STATUS::OK;
      }
    }
  };
} // namespace tules
