#pragma once

#include "tules_commons.hpp"

namespace tules
{
  class StatusHelper
  {
  public:
    class Status_t
    {
      friend class ExampleClass;

    public:
      constexpr bool operator==(const Status_t &other) const
      {
        return mValue == other;
      }

      constexpr bool Contains(const Status_t &other) const
      {
        if (other.mValue == 0)
          return !mValue;
        return (mValue & other.mValue) == other.mValue;
      }

      constexpr operator uint8_t() const { return mValue; }

    protected:
      constexpr Status_t(uint8_t val) : mValue{val} {}

    private:
      uint8_t mValue;
    };

  protected:
    StatusHelper(Status_t s) : mValue(s) {}
    Status_t mValue;
  };
} // namespace tules

// namespace tules
// {
//   class ExampleClass
//   {
//   public:
//     struct Status : StatusHelper
//     {
//     public:
//       static constexpr Status_t OK{0x00};
//       static constexpr Status_t BAD{0x01};
//       static constexpr Status_t WHAT{0x02};
//     };

//     constexpr Status::Status_t GetStatusWhat() const
//     {
//       return Status::WHAT;
//     }

//     constexpr Status::Status_t GetComplexStatus() const
//     {
//       return Status::WHAT | Status::BAD;
//     }
//   };
// } // namespace tules

// #include <cstdio>

// int main()
// {
//   using namespace tules;
//   constexpr ExampleClass rb;
//   printf("Does rb.GetComplexStatus() contains ExampleClass::Status::BAD? %d\n", rb.GetComplexStatus().Contains(ExampleClass::Status::BAD));
//   constexpr bool bb = rb.GetStatusWhat() == ExampleClass::Status::WHAT;
//   return bb;
// }