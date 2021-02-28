#ifndef __DISPLAY__DISPLAY_H__
#define __DISPLAY__DISPLAY_H__

#include <string>

namespace weather_station {
namespace display {

enum class TextAlignment {
  BOTTOM_CENTER,
  BOTTOM_RIGHT,
  MIDDLE_LEFT,
  TOP_CENTER,
  TOP_LEFT,
};

class Display {
 public:
  virtual int width();

  virtual int height();

  virtual void clearScreen();

  virtual void drawGridLine(const int x0, const int y0, const int x1, const int y1);

  virtual void drawAxis(const int x0, const int y0, const int x1, const int y1, const std::string& label,
                        const int label_pos_x, const int label_pos_y, const TextAlignment label_position);

  virtual void drawTextLabel(const std::string& label, const int32_t x, const int32_t y, const TextAlignment position);

  virtual void drawDataLabel(const float label, const int32_t x, const int32_t y, const TextAlignment position);

  virtual void drawTimeLabel(const time_t time, const int32_t x, const int32_t y, const TextAlignment position);

  virtual void drawDataMeasurement(const time_t time, const float label, const int32_t x, const int32_t y,
                                   const TextAlignment position);

  virtual void drawPixel(const int32_t x, const int32_t y, const uint32_t color);

  virtual void commit() {}
};

}  // namespace display
}  // namespace weather_station

#endif