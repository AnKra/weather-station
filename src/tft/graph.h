#include <SPI.h>
#include <TFT_eSPI.h>

#include "colors.h"

namespace weather_station {
namespace tft {

class Graph {
 public:
  Graph(const int width, const int height, const double x_min, const double x_max, const double cell_width,
        const double y_min, const double y_max, const double cell_height)
      : width_px_(width),
        height_px_(height),
        x_min_(x_min),
        x_max_(x_max),
        cell_width_(cell_width),
        y_min_(y_min),
        y_max_(y_max),
        cell_height_(cell_height) {
    unit_height_px_ = height / (y_max - y_min);
    unit_width_px_ = width / (x_max - x_min);

    initializeTft();
  }

  void drawAxes(const String &title, const String &x_label, const String &y_label) {
    drawTextLabel(title, (width_px_ / 2), 0, TC_DATUM);

    // draw y scale
    for (auto y = y_min_; y <= y_max_; y += cell_height_) {
      auto y_px = yToPx(y);
      auto color = grid_color_;
      if (y == 0) {
        drawTextLabel(x_label, width_px_, y_px, BR_DATUM);
        color = axes_color_;
      }
      tft_.drawLine(0, y_px, width_px_, y_px, color);
      drawDataLabel(y, 0, y_px, ML_DATUM);
    }

    // draw x scale
    for (auto x = x_min_; x <= x_max_; x += cell_width_) {
      auto x_px = xToPx(x);
      auto color = grid_color_;
      if (x == 0) {
        drawTextLabel(y_label, x_px, 0, TL_DATUM);
        color = axes_color_;
      }
      tft_.drawLine(x_px, 0, x_px, height_px_, color);
      drawDataLabel(x, x_px, height_px_, BC_DATUM);
    }
  }

  void drawPixel(const double x, const double y, const unsigned int color) {
    tft_.drawPixel(xToPx(x), yToPx(y), color);
  }

 private:
  void initializeTft() {
    tft_.begin();
    tft_.fillScreen(BLACK);
    tft_.setRotation(1);
  }

  void drawTextLabel(const String &label, const int x, const int y, uint8_t position) {
    tft_.setTextDatum(position);
    tft_.setTextColor(axes_color_, background_color_);
    tft_.drawString(label, x, y, font_);
  }

  void drawDataLabel(const float label, const int x, const int y, uint8_t position) {
    tft_.setTextDatum(position);
    tft_.setTextColor(text_color_, background_color_);
    tft_.drawFloat(label, dp_, x, y, font_);
  }

  int xToPx(const double x) const { return (x - x_min_) * unit_width_px_; }

  int yToPx(const double y) const { return (height_px_ - 1) - (y - y_min_) * unit_height_px_; }

 private:
  const int grid_color_ = DKBLUE;
  const int axes_color_ = RED;
  const int text_color_ = WHITE;
  const int background_color_ = BLACK;
  const uint8_t font_ = 1;
  const byte dp_ = 1;

  TFT_eSPI tft_;
  int width_px_;
  int height_px_;
  double x_min_;
  double x_max_;
  double cell_width_;
  double y_min_;
  double y_max_;
  double cell_height_;
  int unit_width_px_;
  int unit_height_px_;
};

}  // namespace tft
}  // namespace weather_station