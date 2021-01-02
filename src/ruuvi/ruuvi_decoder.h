#ifndef __RUUVI__RUUVI_DECODER_H__
#define __RUUVI__RUUVI_DECODER_H__

#include <algorithm>
#include <cctype>
#include <sstream>
#include <stdexcept>
#include <string>

namespace weather_station {
namespace ruuvi {

static uint16_t hexToUnsignedDec(const std::string &hex_val) {
  const auto supported_hex_length = 4;
  if (hex_val.length() != supported_hex_length) {
    std::stringstream ss;
    ss << "ERROR: Unsupported hex value length " << hex_val.length() << ". Only length == " << supported_hex_length
       << " is supported.";
    throw std::runtime_error(ss.str());
  }

  int base = 1;
  int dec_val = 0;

  for (int i = hex_val.length() - 1; i >= 0; i--) {
    if (hex_val[i] >= '0' && hex_val[i] <= '9') {
      dec_val += static_cast<uint16_t>(hex_val[i] - 48) * base;
      base = base * 16;
    } else if (hex_val[i] >= 'A' && hex_val[i] <= 'F') {
      dec_val += static_cast<uint16_t>(hex_val[i] - 55) * base;
      base = base * 16;
    }
  }

  return static_cast<uint16_t>(dec_val);
}

static void decodeData(const std::string &raw_data, float &temperature, float &humidity, float &pressure) {
  auto hex_data = raw_data;
  std::transform(hex_data.begin(), hex_data.end(), hex_data.begin(), [](unsigned char c) { return std::toupper(c); });

  // see
  // https://github.com/ruuvi/ruuvi-sensor-protocols/blob/master/broadcast_formats.md
  const auto supported_data_format = "05";
  const auto data_format = hex_data.substr(4, 2);
  if (data_format != supported_data_format) {
    throw std::runtime_error("ERROR: Unsupported Ruuvi data format " + data_format + ". Only data format " +
                             supported_data_format + " is supported.");
  }

  // see
  // https://github.com/ruuvi/ruuvi-sensor-protocols/blob/master/dataformat_05.md
  temperature = static_cast<float>(static_cast<int16_t>(hexToUnsignedDec(hex_data.substr(6, 4)))) * 0.005f;
  humidity = static_cast<float>(hexToUnsignedDec(hex_data.substr(10, 4))) * 0.0025f;
  pressure = static_cast<float>(hexToUnsignedDec(hex_data.substr(14, 4))) + 50000;
}

}  // namespace ruuvi
}  // namespace weather_station

#endif
