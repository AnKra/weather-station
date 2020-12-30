#ifndef BLUETOOTH_LISTENER_TEST_H
#define BLUETOOTH_LISTENER_TEST_H

#include <unity.h>
#include <string>

#include "ruuvi/ruuvi_decoder.h"

void unittest_decode_positive_data() {
  const std::string raw_data = {"99040500249C40B4C003DCFF7C017091F65267AAF856BBF1D599"};

  float temperature = 0;
  float humidity = 0;
  float pressure = 0;

  weather_station::ruuvi::decodeData(raw_data, temperature, humidity, pressure);

  TEST_ASSERT_EQUAL_FLOAT(0.18, temperature);
  TEST_ASSERT_EQUAL_FLOAT(100, humidity);
  TEST_ASSERT_EQUAL_FLOAT(96272, pressure);
}

void unittest_decode_negative_data() {
  const std::string raw_data = {"990405ffbc9c40b5c6fff80014fc0092b64b650af856bbf1d599"};

  float temperature = 0;
  float humidity = 0;
  float pressure = 0;

  weather_station::ruuvi::decodeData(raw_data, temperature, humidity, pressure);

  TEST_ASSERT_EQUAL_FLOAT(-0.34, temperature);
  TEST_ASSERT_EQUAL_FLOAT(100, humidity);
  TEST_ASSERT_EQUAL_FLOAT(96534, pressure);
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(unittest_decode_positive_data);
  RUN_TEST(unittest_decode_negative_data);
  UNITY_END();

  return 0;
}
#endif