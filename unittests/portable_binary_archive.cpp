/*
  Copyright (c) 2013, Randolph Voorhies, Shane Grant
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
      * Neither the name of cereal nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL RANDOLPH VOORHIES AND SHANE GRANT BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "common.hpp"
#include <boost/test/unit_test.hpp>

namespace mynamespace { struct MyCustomClass {}; }

#ifdef _MSC_VER
BOOST_AUTO_TEST_CASE( util )
{
  BOOST_CHECK_EQUAL( cereal::util::demangledName<mynamespace::MyCustomClass>(), "struct mynamespace::MyCustomClass" );
}
#else
BOOST_AUTO_TEST_CASE( util )
{
  BOOST_CHECK_EQUAL( cereal::util::demangledName<mynamespace::MyCustomClass>(), "mynamespace::MyCustomClass" );
}
#endif

template <class T>
inline void swapBytes( T & t )
{
  cereal::portable_binary_detail::swap_bytes<sizeof(T)>( reinterpret_cast<std::uint8_t*>(&t) );
}

BOOST_AUTO_TEST_CASE( portable_binary_archive )
{
  std::random_device rd;
  std::mt19937 gen(rd());

  for(size_t i=0; i<100; ++i)
  {
    bool     o_bool   = random_value<uint8_t>(gen) % 2 ? true : false;
    uint8_t  o_uint8  = random_value<uint8_t>(gen);
    int8_t   o_int8   = random_value<int8_t>(gen);
    uint16_t o_uint16 = random_value<uint16_t>(gen);
    int16_t  o_int16  = random_value<int16_t>(gen);
    uint32_t o_uint32 = random_value<uint32_t>(gen);
    int32_t  o_int32  = random_value<int32_t>(gen);
    uint64_t o_uint64 = random_value<uint64_t>(gen);
    int64_t  o_int64  = random_value<int64_t>(gen);
    float    o_float  = random_value<float>(gen);
    double   o_double = random_value<double>(gen);

    // swap the bytes on all of the data
    swapBytes(o_bool);
    swapBytes(o_uint8);
    swapBytes(o_int8);
    swapBytes(o_uint16);
    swapBytes(o_int16);
    swapBytes(o_uint32);
    swapBytes(o_int32);
    swapBytes(o_uint64);
    swapBytes(o_int64);
    swapBytes(o_float);
    swapBytes(o_double);

    std::ostringstream os;
    {
      cereal::BinaryOutputArchive oar(os);
      // manually insert incorrect endian encoding
      oar(!cereal::portable_binary_detail::is_little_endian());

      oar(o_bool);
      oar(o_uint8);
      oar(o_int8);
      oar(o_uint16);
      oar(o_int16);
      oar(o_uint32);
      oar(o_int32);
      oar(o_uint64);
      oar(o_int64);
      oar(o_float);
      oar(o_double);
    }

    // swap back to original values
    swapBytes(o_bool);
    swapBytes(o_uint8);
    swapBytes(o_int8);
    swapBytes(o_uint16);
    swapBytes(o_int16);
    swapBytes(o_uint32);
    swapBytes(o_int32);
    swapBytes(o_uint64);
    swapBytes(o_int64);
    swapBytes(o_float);
    swapBytes(o_double);

    bool     i_bool   = false;
    uint8_t  i_uint8  = 0;
    int8_t   i_int8   = 0;
    uint16_t i_uint16 = 0;
    int16_t  i_int16  = 0;
    uint32_t i_uint32 = 0;
    int32_t  i_int32  = 0;
    uint64_t i_uint64 = 0;
    int64_t  i_int64  = 0;
    float    i_float  = 0;
    double   i_double = 0;

    std::istringstream is(os.str());
    {
      cereal::PortableBinaryInputArchive iar(is);
      iar(i_bool);
      iar(i_uint8);
      iar(i_int8);
      iar(i_uint16);
      iar(i_int16);
      iar(i_uint32);
      iar(i_int32);
      iar(i_uint64);
      iar(i_int64);
      iar(i_float);
      iar(i_double);
    }

    BOOST_CHECK_EQUAL(i_bool   , o_bool);
    BOOST_CHECK_EQUAL(i_uint8  , o_uint8);
    BOOST_CHECK_EQUAL(i_int8   , o_int8);
    BOOST_CHECK_EQUAL(i_uint16 , o_uint16);
    BOOST_CHECK_EQUAL(i_int16  , o_int16);
    BOOST_CHECK_EQUAL(i_uint32 , o_uint32);
    BOOST_CHECK_EQUAL(i_int32  , o_int32);
    BOOST_CHECK_EQUAL(i_uint64 , o_uint64);
    BOOST_CHECK_EQUAL(i_int64  , o_int64);
    BOOST_CHECK_CLOSE(i_float  , o_float,  (float)1e-5);
    BOOST_CHECK_CLOSE(i_double , o_double, 1e-5);
  }
}

