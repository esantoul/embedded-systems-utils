#ifndef ESUTILS_STATUS_HELPER_HPP
#define ESUTILS_STATUS_HELPER_HPP

/**
 * @file status_helper.hpp
 * Definition of a helper that is useful for return values & error codes of data structures methods
 * @author Etienne Santoul
 */

#include <cstdint>

namespace esutils
{
  struct StatusHelper
  {
    class Status_t
    {
      friend class ExampleClass;

    public:
      constexpr bool operator==(const Status_t &other) const
      {
        return mValue == other;
      }

      constexpr bool contains(const Status_t &other) const
      {
        if (other.mValue == 0)
          return !mValue;
        return (mValue & other.mValue) == other.mValue;
      }

      constexpr operator uint8_t() const { return mValue; }

    private:
      constexpr Status_t(uint8_t val) : mValue{val} {}

    private:
      uint8_t mValue;
    };
  };
} // namespace esutils

#endif // ESUTILS_STATUS_HELPER_HPP
