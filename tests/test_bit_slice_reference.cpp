
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>

#include <catch2/catch_test_macros.hpp>

#include "esutils/bit_slice_reference.hpp"

namespace
{
  template <size_t sliceSize>
  void checkSliceEquality_impl(const uint32_t val1, const std::array<uint8_t, 4> val2)
  {
    for (size_t i = 0; i < std::numeric_limits<uint32_t>::digits / sliceSize; ++i)
    {
      const uint32_t slice1 = esutils::BitSliceReference<sliceSize, uint32_t, const uint32_t>(&val1, i);
      const uint32_t slice2 = esutils::BitSliceReference<sliceSize, uint32_t, const uint8_t>(std::data(val2), i);
      REQUIRE(slice1 == slice2);
    }
  }

  template <size_t ... allSliceSizes>
  void checkSliceEquality(const uint32_t val1, const std::array<uint8_t, 4> val2)
  {
    (checkSliceEquality_impl<allSliceSizes>(val1, val2), ...);
  }
}

TEST_CASE("Bit Slice Reference", "[esutils]")
{
  SECTION("Basic usage")
  {
    const uint8_t value = 0xA6;

    REQUIRE(esutils::BitSliceReference<4, uint8_t, const uint8_t>(&value, 0) == 0x6);
    REQUIRE(esutils::BitSliceReference<4, uint8_t, const uint8_t>(&value, 1) == 0xA);
  }

  SECTION("Homogeneity between several underlying types")
  {
    const uint32_t value1 = 0xFFAA3300;
    const std::array<uint8_t, 4> value2 = {0x00, 0x33, 0xAA, 0xFF};

    REQUIRE(std::equal(std::begin(value2), std::end(value2), reinterpret_cast<const uint8_t *>(&value1)));

    checkSliceEquality<1, 2, 4, 8, 16, 32>(value1, value2);
  }
}
