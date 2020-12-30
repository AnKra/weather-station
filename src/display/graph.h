#ifndef __DISPLAY__GRAPH_H__
#define __DISPLAY__GRAPH_H__

#include <SPI.h>
#include <TFT_eSPI.h>

#include <algorithm>
#include <memory>
#include <vector>

#include "display/colors.h"
#include "display/tft.h"

namespace weather_station {
namespace display {

class Graph {
 public:
  Graph(std::unique_ptr<Tft> tft, const int width, const int height, const String &title, const String &x_label,
        const String &y_label)
      : tft_(std::move(tft)),
        width_px_(width),
        height_px_(height),
        title_(title),
        x_label_(x_label),
        y_label_(y_label) {
    updateVariablesForGraphCalculation();
  }

  void drawAxes() {
    tft_->clearScreen();

    tft_->drawTextLabel(title_, (width_px_ / 2), 0, TC_DATUM);

    // draw y scale
    for (auto y = y_min_; y <= y_max_; y += cell_height_) {
      auto y_px = yToPx(y);
      if (y == 0) {
        tft_->drawAxis(0, y_px, width_px_, y_px, x_label_, width_px_, y_px, BR_DATUM);
      } else {
        tft_->drawGridLine(0, y_px, width_px_, y_px);
      }
      tft_->drawDataLabel(y, 0, y_px, ML_DATUM);
    }

    // draw x scale
    for (auto x = x_min_; x <= x_max_; x += cell_width_) {
      auto x_px = xToPx(x);
      if (x == 0) {
        tft_->drawAxis(x_px, 0, x_px, height_px_, y_label_, x_px, 0, TL_DATUM);
      } else {
        tft_->drawGridLine(x_px, 0, x_px, height_px_);
      }
      tft_->drawTimeLabel(x, x_px, height_px_, BC_DATUM);
    }
  }

  void addDataPoint(const time_t x, const float y, const unsigned int color) {
    x_.push_back(x);
    y_.push_back(y);

    updateVariablesForGraphCalculation();
    drawAxes();

    std::vector<double> tmp;  // unused variable
    std::transform(x_.begin(), x_.end(), y_.begin(), std::back_inserter(tmp), [&](const time_t &xx, const double &yy) {
      tft_->drawPixel(xToPx(xx), yToPx(yy), color);
      return 0.;  // dummy value
    });
  }

 private:
  void updateVariablesForGraphCalculation() {
    if (x_.size() > 0) {
      x_min_ = *std::min_element(std::begin(x_), std::end(x_)) - cell_width_;
      x_max_ = *std::max_element(std::begin(x_), std::end(x_)) + cell_width_;
    }
    if (y_.size() > 0) {
      y_min_ = std::floor(*std::min_element(std::begin(y_), std::end(y_))) - 2 * cell_height_;
      y_max_ = std::ceil(*std::max_element(std::begin(y_), std::end(y_))) + 2 * cell_height_;
    }

    unit_height_px_ = static_cast<float>(height_px_) / (y_max_ - y_min_);
    unit_width_px_ = static_cast<float>(width_px_) / static_cast<float>(x_max_ - x_min_);
  }

  int xToPx(const time_t x) const { return static_cast<int>(static_cast<float>(x - x_min_) * unit_width_px_); }

  int yToPx(const double y) const { return static_cast<int>((height_px_ - 1) - (y - y_min_) * unit_height_px_); }

 private:
  std::unique_ptr<Tft> tft_;

  const int width_px_;
  const int height_px_;
  const String title_;
  const String x_label_;
  const String y_label_;

  time_t x_min_ = 0;
  time_t x_max_ = 240;
  const int cell_width_ = 120;
  float y_min_ = -6;
  float y_max_ = 40;
  const float cell_height_ = 1;
  float unit_width_px_;
  float unit_height_px_;

  std::vector<time_t> x_;
  std::vector<float> y_;
};

}  // namespace display
}  // namespace weather_station

#endif
