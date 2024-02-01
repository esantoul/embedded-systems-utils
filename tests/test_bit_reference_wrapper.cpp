
#include <catch2/catch_test_macros.hpp>

#include "esutils/bit_reference_wrapper.hpp"

TEST_CASE("Bit Reference Wrapper", "[esutils]")
{
  SECTION("Reference to const bit")
  {
    // Taking a const bit reference of an uint8_t
    uint8_t val = 0;
    const uint8_t &rval = val;
    esutils::BitReferenceWrapper cbitref(rval, 0);

    // Value must be correctly read
    REQUIRE(cbitref == false);
    val = 0xFF;
    REQUIRE(cbitref == true);

    // This must be the same for all possible bit position in the underlying type
    for (int i = 0; i < std::numeric_limits<uint8_t>::digits; ++i)
    {
      const uint8_t v = 1 << i;

      for (int j = 0; j < std::numeric_limits<uint8_t>::digits; ++j)
      {
        esutils::BitReferenceWrapper ref(v, j);
        REQUIRE(ref == (i == j));
      }
    }
  }

  SECTION("Reference to mutable bit")
  {
    uint8_t val = 0;
    esutils::BitReferenceWrapper bitref(val, 0);

    REQUIRE(static_cast<bool>(bitref) == false);

    // Setting the value must set the correct bit in val
    bitref = true;
    REQUIRE(val == 0x01);

    val = 0xFF;

    bitref = false;
    REQUIRE(val == 0xFE);

    // This should be the same for all the possible bit positions in the underlying type
    for (int i = 0; i < std::numeric_limits<uint8_t>::digits; ++i)
    {
      uint8_t v = 0;
      esutils::BitReferenceWrapper ref(v, i);

      ref = true;

      REQUIRE(v == 1 << i);

      v = 0xFF;

      ref = false;

      REQUIRE(v == (0xFF & ~(1 << i)));
    }
  }
}
