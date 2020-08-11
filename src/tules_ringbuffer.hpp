#pragma once

/**
 * @file tules_ringbuffer.hpp
 * Definition of a simple ring buffer with fixed size
 * @author Etienne Santoul
 */

#include "tules_commons.hpp"
#include "tules_type_capacity.hpp"
#include "tules_optional.hpp"
#include "tules_algorithms.hpp"

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
   * @tparam T the type that is being contained
   * @tparam capacity the maximum number of elements that can be contained
   * @todo Check whether a destructor should be called on clear
   */
  template <typename T, size_t capacity>
  class RingBuffer
  {
  private:
    class CyclicIndex
    {
    private:
      using cty_t = typename TypeCapacity<capacity>::type;
      cty_t mIdx = 0;

    public:
      cty_t operator=(cty_t val) { return mIdx = val; }

      operator size_t() const { return mIdx; }

      size_t operator++(int)
      {
        size_t val = mIdx;
        if (++mIdx >= capacity)
          mIdx = 0;
        return val;
      }

      size_t operator++()
      {
        (*this)++;
        return *this;
      }
    };

    T mData[capacity];
    CyclicIndex mReadPos, mWritePos;

  public:
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
    size_t Capacity() const { return capacity; }

    /**
     * @return The number of elements that can be read from the RingBuffer
     */
    size_t Readable() const
    {
      if (mWritePos >= mReadPos)
        return mWritePos - mReadPos;
      else
        return mWritePos + capacity - mReadPos;
    }

    /**
     * @return The number of elements that can be written in the RingBuffer before it is full
     */
    size_t Writable() const { return capacity - Readable(); }

    /**
     * @brief Adds a single element to the RingBuffer
     * @param val The value to be written
     * @return True if the element was added, else if the RingBuffer is already full
     */
    bool Write(const T &val)
    {
      if (Writable())
      {
        mData[mWritePos++] = val;
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
        return Optional<T>{mData[mReadPos++]};
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
      if (mWritePos == mReadPos) // Overwriting
      {
        mData[mWritePos++] = val;
        mReadPos = mWritePos;
        return RINGBUFFER_STATUS::DATA_OVERWRITTEN; // Some data has been overwritten
      }
      else // Simply writing
      {
        mData[mWritePos++] = val;
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
          mData[mWritePos++] = array[i];
        mReadPos = mWritePos;
        return RINGBUFFER_STATUS::DATA_OVERWRITTEN;
      }
      else
      {
        for (size_t i = 0; i < length; ++i)
          mData[mWritePos++] = array[i];
        return RINGBUFFER_STATUS::OK;
      }
    }
  };
} // namespace tules
