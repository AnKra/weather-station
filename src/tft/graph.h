#include <SPI.h>
#include <TFT_eSPI.h>

#include <algorithm>
#include <vector>

#include "colors.h"

namespace weather_station {
namespace tft {

class Graph {
 public:
  Graph(const int width, const int height, const String &title, const String &x_label, const String &y_label)
      : width_px_(width), height_px_(height), title_(title), x_label_(x_label), y_label_(y_label) {
    updateVariablesForGraphCalculation();
    initializeTft();
  }

  void drawAxes() {
    tft_.fillScreen(background_color_);

    drawTextLabel(title_, (width_px_ / 2), 0, TC_DATUM);

    // draw y scale
    for (auto y = y_min_; y <= y_max_; y += cell_height_) {
      auto y_px = yToPx(y);
      auto color = grid_color_;
      if (y == 0) {
        drawTextLabel(x_label_, width_px_, y_px, BR_DATUM);
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
        drawTextLabel(y_label_, x_px, 0, TL_DATUM);
        color = axes_color_;
      }
      tft_.drawLine(x_px, 0, x_px, height_px_, color);
      drawDataLabel(x, x_px, height_px_, BC_DATUM);
    }
  }

  void drawPixel(const double x, const double y, const unsigned int color) {
    x_.push_back(x);
    y_.push_back(y);

    updateVariablesForGraphCalculation();
    drawAxes();

    std::vector<double> tmp;  // unused variable
    std::transform(x_.begin(), x_.end(), y_.begin(), std::back_inserter(tmp), [&](const double &xx, const double &yy) {
      tft_.drawPixel(xToPx(xx), yToPx(yy), color);
      return 0.;  // dummy value
    });
  }

 private:
  void initializeTft() {
    tft_.begin();
    tft_.fillScreen(background_color_);
    tft_.setRotation(1);
  }

  void updateVariablesForGraphCalculation() {
    if (x_.size() > 0) {
      x_min_ = std::floor(*std::min_element(std::begin(x_), std::end(x_))) - 2;
      x_max_ = std::ceil(*std::max_element(std::begin(x_), std::end(x_))) + 1;
    }
    if (y_.size() > 0) {
      y_min_ = std::floor(*std::min_element(std::begin(y_), std::end(y_))) - 2;
      y_max_ = std::ceil(*std::max_element(std::begin(y_), std::end(y_))) + 2;
    }

    unit_height_px_ = height_px_ / (y_max_ - y_min_);
    unit_width_px_ = width_px_ / (x_max_ - x_min_);
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
  const int width_px_;
  const int height_px_;
  const String title_;
  const String x_label_;
  const String y_label_;

  const int grid_color_ = DKBLUE;
  const int axes_color_ = RED;
  const int text_color_ = WHITE;
  const int background_color_ = BLACK;
  const uint8_t font_ = 1;
  const byte dp_ = 1;

  TFT_eSPI tft_;
  double x_min_ = -2;
  double x_max_ = 20;
  double cell_width_ = 1;
  double y_min_ = -6;
  double y_max_ = 40;
  double cell_height_ = 1;
  int unit_width_px_;
  int unit_height_px_;

  std::vector<double> x_;
  std::vector<double> y_;
};

}  // namespace tft
}  // namespace weather_station