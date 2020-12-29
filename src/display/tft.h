#ifndef __DISPLAY__TFT_H__
#define __DISPLAY__TFT_H__

#include <SPI.h>
#include <TFT_eSPI.h>

#include <algorithm>
#include <vector>

#include "display/colors.h"

namespace weather_station {
namespace display {

class Tft {
 public:
  Tft() { initializeTft(); }

  void clearScreen() { tft_.fillScreen(background_color_); }

  void drawGridLine(const int x0, const int y0, const int x1, const int y1) {
    tft_.drawLine(x0, y0, x1, y1, grid_color_);
  }

  void drawAxis(const int x0, const int y0, const int x1, const int y1, const String& label, const int label_pos_x,
                const int label_pos_y, const uint8_t label_position) {
    tft_.drawLine(x0, y0, x1, y1, axes_color_);
    drawTextLabel(label, label_pos_x, label_pos_y, label_position);
  }

  void drawTextLabel(const String& label, const int x, const int y, uint8_t position) {
    tft_.setTextDatum(position);
    tft_.setTextColor(axes_color_, background_color_);
    tft_.drawString(label, x, y, font_);
  }

  void drawDataLabel(const float label, const int x, const int y, uint8_t position) {
    tft_.setTextDatum(position);
    tft_.setTextColor(text_color_, background_color_);
    tft_.drawFloat(label, dp_, x, y, font_);
  }

  void drawPixel(const double x, const double y, const unsigned int color) { tft_.drawPixel(x, y, color); }

 private:
  void initializeTft() {
    tft_.begin();
    tft_.fillScreen(background_color_);
    tft_.setRotation(1);
  }

  const int grid_color_ = DKBLUE;
  const int axes_color_ = RED;
  const int text_color_ = WHITE;
  const int background_color_ = BLACK;
  const uint8_t font_ = 1;
  const byte dp_ = 1;

  TFT_eSPI tft_;
};

}  // namespace display
}  // namespace weather_station

#endif