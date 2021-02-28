#ifndef __DISPLAY__GRAPH_H__
#define __DISPLAY__GRAPH_H__

#include <algorithm>
#include <memory>
#include <vector>

#include "display/colors.h"
#include "display/display.h"

namespace weather_station {
namespace display {

class Graph {
 public:
  using degrees = float;
  using seconds = time_t;
  using pixel = int32_t;

  Graph(std::unique_ptr<Display> display, const std::string &title, const std::string &x_label,
        const std::string &y_label)
      : display_(std::move(display)),
        width_px_(display_->width()),
        height_px_(display_->height()),
        title_(title),
        x_label_(x_label),
        y_label_(y_label) {
    updateVariablesForGraphCalculation();
  }

  void drawAxes() {
    display_->clearScreen();

    // draw y scale
    for (auto y = y_min_; y <= y_max_; y += k_cell_height_deg_) {
      auto y_px = degreesToPx(y);
      if (y == 0) {
        display_->drawAxis(0, y_px, width_px_, y_px, x_label_, width_px_, y_px, TextAlignment::BOTTOM_RIGHT);
      } else {
        display_->drawGridLine(0, y_px, width_px_, y_px);
      }
      display_->drawDataLabel(y, 0, y_px, TextAlignment::MIDDLE_LEFT);
    }

    // draw x scale
    for (auto x = x_min_; x <= x_max_; x += k_cell_width_s_) {
      auto x_px = secondsToPx(x);
      if (x == 0) {
        display_->drawAxis(x_px, 0, x_px, height_px_, y_label_, x_px, 0, TextAlignment::TOP_LEFT);
      } else {
        display_->drawGridLine(x_px, 0, x_px, height_px_);
      }
      display_->drawTimeLabel(x, x_px, height_px_ - 17, TextAlignment::TOP_CENTER);
    }

    // draw title
    display_->drawTextLabel(title_, (width_px_ / 2), 0, TextAlignment::TOP_CENTER);
  }

  void addDataPoint(const seconds new_x, const degrees new_y, const unsigned int color) {
    auto seconds_since_midnight = new_x % (24 * 60 * 60);
    data_x_.push_back(seconds_since_midnight);
    data_y_.push_back(new_y);

    updateVariablesForGraphCalculation();
    drawAxes();

    auto y = data_y_.begin();
    for (auto x = data_x_.begin(); x != data_x_.end() && y != data_y_.end(); ++y, ++x) {
      display_->drawPixel(secondsToPx(*x), degreesToPx(*y), color);
    }

    display_->drawDataMeasurement(*data_x_.rbegin(), static_cast<double>(*data_y_.rbegin()), (width_px_ / 2) * 1.5, 10,
                                  TextAlignment::MIDDLE_LEFT);

    display_->commit();
  }

 private:
  void updateVariablesForGraphCalculation() {
    if (!data_y_.empty()) {
      y_min_ = std::floor(*std::min_element(std::begin(data_y_), std::end(data_y_))) - 2 * k_y_padding_deg_;
      y_max_ = std::ceil(*std::max_element(std::begin(data_y_), std::end(data_y_))) + 2 * k_y_padding_deg_;
    }

    pixel_per_degree_ = static_cast<degrees>(height_px_) / (y_max_ - y_min_);
  }

  int secondsToPx(const seconds x) const {
    return static_cast<int>(static_cast<degrees>(x - x_min_) * pixel_per_second_);
  }

  int degreesToPx(const degrees y) const {
    return static_cast<int>((height_px_ - 1) - (y - y_min_) * pixel_per_degree_);
  }

 private:
  std::unique_ptr<Display> display_;

  const int width_px_;
  const int height_px_;
  const std::string title_;
  const std::string x_label_;
  const std::string y_label_;

  const seconds x_min_ = 0;
  const seconds x_max_ = 60 * 60 * 24;                     // 60s * 60min * 24h
  constexpr static seconds k_cell_width_s_ = 60 * 60 * 2;  // 60s * 60min * 2h
  degrees y_min_ = -6;
  degrees y_max_ = 40;
  constexpr static degrees k_cell_height_deg_ = 1;
  const float pixel_per_second_ = static_cast<float>(width_px_) / static_cast<float>(x_max_ - x_min_);
  float pixel_per_degree_;
  constexpr static auto k_x_padding_s_ = 0;
  constexpr static auto k_y_padding_deg_ = k_cell_height_deg_;

  std::vector<seconds> data_x_;
  std::vector<degrees> data_y_;
};

}  // namespace display
}  // namespace weather_station

#endif
