#pragma once

/**
 * @file tules_ring_buffer.hpp
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
   * @tparam cty the maximum number of elements that can be contained
   * @todo Check whether a destructor should be called on clear
   */
  template <typename T, size_t cty>
  class RingBuffer
  {
  public:
    constexpr RingBuffer() = default;

    /**
     * @brief Resets the RingBuffer
     */
    void clear()
    {
      mReadPos = mWritePos = 0;
    }

    /**
     * @return The maximal number of elements that can be contained in the RingBuffer
     */
    size_t capacity() const { return cty; }

    /**
     * @return The number of elements that can be read from the RingBuffer
     */
    size_t readable() const
    {
      if (mWritePos >= mReadPos)
        return mWritePos - mReadPos;
      else
        return mWritePos + cty - mReadPos;
    }

    /**
     * @return The number of elements that can be written in the RingBuffer before it is full
     */
    size_t writable() const { return cty - readable(); }

    /**
     * @brief Adds a single element to the RingBuffer
     * @param val The value to be written
     * @return True if the element was added, else if the RingBuffer is already full
     */
    bool write(const T &val)
    {
      if (writable())
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
    Optional<T> read()
    {
      if (readable())
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
    T *read(size_t size)
    {
      if (size <= readable())
      {
        if (cty - mReadPos > size) // Need to rotate mData in order to have a contiguous chunk of retured data
        {
          tules::rotate(mData, mData + mReadPos, mData + cty);
          mWritePos = mReadPos > mWritePos ? mWritePos + cty - mReadPos : mWritePos - mReadPos;
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
    uint8_t overwrite(const T &val)
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
    uint8_t overwrite(const T *array, size_t length)
    {
      if (length > cty)
      {
        return RINGBUFFER_STATUS::NO_DATA_WRITTEN | RINGBUFFER_STATUS::NOT_ENOUGH_SPACE
      }
      else if (length > writable())
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

    /**
     * @brief A function to take a look or modify inplace the value of the first readable element
     * @return A pointer to the first readable element if there is one or nullptr
     */
    T *peek()
    {
      return readable() ? mData + mReadPos : nullptr;
    }

  private:
    /**
     * @brief A forward index that goes back to 0 when reaching cty
     */
    class CyclicIndex
    {
    private:
      using cty_t = typename TypeCapacity<cty>::type;
      cty_t mIdx = 0;

    public:
      cty_t operator=(cty_t val) { return mIdx = val; }

      operator size_t() const { return mIdx; }

      size_t operator++(int)
      {
        size_t val = mIdx;
        if (++mIdx >= cty)
          mIdx = 0;
        return val;
      }

      size_t operator++()
      {
        (*this)++;
        return *this;
      }
    };

    T mData[cty]{};
    CyclicIndex mReadPos, mWritePos;
  };
} // namespace tules

/*

int main()
{
  tules::RingBuffer<uint8_t, 10> rb{};
  rb.write(17);
  return rb.readable();
}
//*/