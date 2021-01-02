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
  using degrees = float;
  using seconds = time_t;
  using pixel = int32_t;

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

    // draw y scale
    for (auto y = y_min_; y <= y_max_; y += k_cell_height_deg_) {
      auto y_px = degreesToPx(y);
      if (y == 0) {
        tft_->drawAxis(0, y_px, width_px_, y_px, x_label_, width_px_, y_px, BR_DATUM);
      } else {
        tft_->drawGridLine(0, y_px, width_px_, y_px);
      }
      tft_->drawDataLabel(y, 0, y_px, ML_DATUM);
    }

    // draw x scale
    for (auto x = x_min_; x <= x_max_; x += k_cell_width_s_) {
      auto x_px = secondsToPx(x);
      if (x == 0) {
        tft_->drawAxis(x_px, 0, x_px, height_px_, y_label_, x_px, 0, TL_DATUM);
      } else {
        tft_->drawGridLine(x_px, 0, x_px, height_px_);
      }
      tft_->drawTimeLabel(x, x_px, height_px_, BC_DATUM);
    }

    // draw title
    tft_->drawTextLabel(title_, (width_px_ / 2), 0, TC_DATUM);
  }

  void addDataPoint(const seconds new_x, const degrees new_y, const unsigned int color) {
    data_x_.push_back(new_x);
    data_y_.push_back(new_y);

    updateVariablesForGraphCalculation();
    drawAxes();

    auto y = data_y_.begin();
    for (auto x = data_x_.begin(); x != data_x_.end() && y != data_y_.end(); ++y, ++x) {
      tft_->drawPixel(secondsToPx(*x), degreesToPx(*y), color);
    }
  }

 private:
  void updateVariablesForGraphCalculation() {
    if (!data_x_.empty()) {
      x_min_ = *std::min_element(std::begin(data_x_), std::end(data_x_)) - k_x_padding_s_;
      x_max_ = *std::max_element(std::begin(data_x_), std::end(data_x_)) + k_x_padding_s_;
    }
    if (!data_y_.empty()) {
      y_min_ = std::floor(*std::min_element(std::begin(data_y_), std::end(data_y_))) - 2 * k_y_padding_deg_;
      y_max_ = std::ceil(*std::max_element(std::begin(data_y_), std::end(data_y_))) + 2 * k_y_padding_deg_;
    }

    pixel_per_degree_ = static_cast<degrees>(height_px_) / (y_max_ - y_min_);
    pixel_per_second_ = static_cast<degrees>(width_px_) / static_cast<degrees>(x_max_ - x_min_);
  }

  int secondsToPx(const seconds x) const {
    return static_cast<int>(static_cast<degrees>(x - x_min_) * pixel_per_second_);
  }

  int degreesToPx(const degrees y) const {
    return static_cast<int>((height_px_ - 1) - (y - y_min_) * pixel_per_degree_);
  }

 private:
  std::unique_ptr<Tft> tft_;

  const int width_px_;
  const int height_px_;
  const String title_;
  const String x_label_;
  const String y_label_;

  seconds x_min_ = 0;
  seconds x_max_ = 240;
  constexpr static seconds k_cell_width_s_ = 120;
  degrees y_min_ = -6;
  degrees y_max_ = 40;
  constexpr static degrees k_cell_height_deg_ = 1;
  float pixel_per_second_;
  float pixel_per_degree_;
  constexpr static auto k_x_padding_s_ = k_cell_width_s_;
  constexpr static auto k_y_padding_deg_ = k_cell_height_deg_;

  std::vector<seconds> data_x_;
  std::vector<degrees> data_y_;
};

}  // namespace display
}  // namespace weather_station

#endif
