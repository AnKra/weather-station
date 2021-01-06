#ifndef __DISPLAY__EPAPER_H__
#define __DISPLAY__EPAPER_H__

#include <algorithm>
#include <ctime>
#include <iomanip>
#include <memory>
#include <sstream>
#include <vector>

#include <Adafruit_GFX.h>
#include <Fonts/FreeSans9pt7b.h>
#include <GxEPD2_3C.h>
#include <epd3c/GxEPD2_420c.h>

#include "display/colors.h"
#include "display/display.h"

namespace weather_station {
namespace display {

class EPaper : public Display {
 public:
  EPaper() { initializeGfx(); }

  int width() {
    Serial.println(display_.width());
    Serial.println(display_.width());
    return display_.width();
  }

  int height() { return display_.height(); }

  void clearScreen() override { display_.firstPage(); }

  void drawGridLine(const int x0, const int y0, const int x1, const int y1) override {
    display_.drawLine(x0, y0, x1, y1, GxEPD_BLACK);
  }

  void drawAxis(const int x0, const int y0, const int x1, const int y1, const std::string& label, const int label_pos_x,
                const int label_pos_y, const TextAlignment label_position) override {
    display_.drawLine(x0, y0, x1, y1, GxEPD_BLACK);
    drawTextLabel(label, label_pos_x, label_pos_y, label_position);
  }

  void drawTextLabel(const std::string& label, const int32_t x, const int32_t y,
                     const TextAlignment position) override {
    display_.setTextColor(GxEPD_BLACK);
    display_.setFont(&FreeSans9pt7b);
    display_.setCursor(x, y);
    display_.print(String(label.c_str()));
  }

  void drawDataLabel(const float label, const int32_t x, const int32_t y, const TextAlignment position) override {
    display_.setTextColor(GxEPD_BLACK);
    display_.setFont(&FreeSans9pt7b);
    display_.setCursor(x, y);
    display_.print(String(label));
  }

  void drawTimeLabel(const time_t time, const int32_t x, const int32_t y, const TextAlignment position) override {
    display_.setTextColor(GxEPD_BLACK);
    display_.setFont(&FreeSans9pt7b);
    display_.setCursor(x, y);
    auto tm = *std::localtime(&time);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%H:%M");
    display_.print(String(oss.str().c_str()));
  }

  void drawPixel(const int32_t x, const int32_t y, const uint32_t /* color */) override {
    display_.drawPixel(static_cast<uint16_t>(x), static_cast<uint16_t>(y), GxEPD_RED);
    display_.drawPixel(static_cast<uint16_t>(x - 1), static_cast<uint16_t>(y - 1), GxEPD_RED);
    display_.drawPixel(static_cast<uint16_t>(x - 1), static_cast<uint16_t>(y + 1), GxEPD_RED);
    display_.drawPixel(static_cast<uint16_t>(x + 1), static_cast<uint16_t>(y - 1), GxEPD_RED);
    display_.drawPixel(static_cast<uint16_t>(x + 1), static_cast<uint16_t>(y + 1), GxEPD_RED);
  }

  void commit() override { display_.nextPage(); }

 private:
  void initializeGfx() {
    SPI.begin(EPD2_SCK, EPD2_MISO, EPD2_MOSI, EPD2_CS);
    display_.init(115200);
    display_.setRotation(0);
    display_.setFullWindow();
  }

  GxEPD2_3C<GxEPD2_420c, GxEPD2_420c::HEIGHT> display_ = GxEPD2_420c(EPD2_CS, EPD2_DC, EPD2_RST, EPD2_BUSY);
};

}  // namespace display
}  // namespace weather_station

#endif
