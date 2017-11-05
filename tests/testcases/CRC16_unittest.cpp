/*
* CRC16 unit test program
* Part of the "WiLED" project, https://github.com/seanlano/WiLED
* A C++ function for generating CRC16 checksums.
* Copyright (C) 2017 Sean Lanigan.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "lib/CRC16/CRC16.h"
#include "gtest/gtest.h"

/// Test fixture for CRC16
class CRC16Test : public testing::Test {
  protected:

    //virtual void SetUp() {}

    //virtual void TearDown() {}
};


TEST_F(CRC16Test, ChecksumCompare12byte) {
  uint8_t array_1[] = {0xAA, 0x10, 0x00, 0xFF, 0xFF, 0x01,
                       0x01, 0x01, 0x49, 0x96, 0x02, 0xD2};
  const uint16_t array_1_crc = 0x19B3;

  uint8_t array_2[] = {0xAA, 0x10, 0x00, 0x11, 0x11, 0x01,
                       0x01, 0x01, 0x49, 0x96, 0x02, 0xD2};
  const uint16_t array_2_crc = 0xC37B;

  uint8_t array_3[] = {0xAA, 0x10, 0x00, 0x11, 0x11, 0x01,
                       0x01, 0x01, 0x49, 0x96, 0x02, 0xD3};
  const uint16_t array_3_crc = 0xD35A;

  uint8_t array_4[] = {0xAA, 0x1F, 0xE1, 0x11, 0x11, 0x01,
                       0x01, 0x01, 0x49, 0x96, 0x02, 0xD3};
  const uint16_t array_4_crc = 0x980F;

  EXPECT_EQ(CRC16(array_1, 0, sizeof(array_1)), array_1_crc);
  EXPECT_EQ(CRC16(array_2, 0, sizeof(array_2)), array_2_crc);
  EXPECT_EQ(CRC16(array_3, 0, sizeof(array_3)), array_3_crc);
  EXPECT_EQ(CRC16(array_4, 0, sizeof(array_4)), array_4_crc);
}


TEST_F(CRC16Test, ChecksumCompare10byte) {
  uint8_t array_1[] = {0xAA, 0x10, 0x00, 0xFF, 0xFF, 0x01,
                       0x01, 0x01, 0x49, 0x96};
  const uint16_t array_1_crc = 0x5436;

  uint8_t array_2[] = {0xAA, 0x10, 0x00, 0x11, 0x11, 0x01,
                       0x01, 0x01, 0x49, 0x96};
  const uint16_t array_2_crc = 0x75C6;

  uint8_t array_3[] = {0xAA, 0x10, 0x00, 0x11, 0x11, 0x01,
                       0x01, 0x01, 0x49, 0x97};
  const uint16_t array_3_crc = 0x65E7;

  uint8_t array_4[] = {0xAA, 0x1F, 0xE1, 0x11, 0x11, 0x01,
                       0x01, 0x01, 0x49, 0x97};
  const uint16_t array_4_crc = 0xCA59;

  EXPECT_EQ(CRC16(array_1, 0, sizeof(array_1)), array_1_crc);
  EXPECT_EQ(CRC16(array_2, 0, sizeof(array_2)), array_2_crc);
  EXPECT_EQ(CRC16(array_3, 0, sizeof(array_3)), array_3_crc);
  EXPECT_EQ(CRC16(array_4, 0, sizeof(array_4)), array_4_crc);
}
