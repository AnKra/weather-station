#ifndef __DISPLAY__EPAPER_H__
#define __DISPLAY__EPAPER_H__

#include <algorithm>
#include <ctime>
#include <iomanip>
#include <memory>
#include <sstream>
#include <vector>

#include <EPD_WaveShare_42.h>
#include <MiniGrafx.h>

#include "display/colors.h"
#include "display/display.h"

namespace weather_station {
namespace display {

class EPaper : public Display {
 public:
  EPaper() : epd_(GFX_CS, GFX_RST, GFX_DC, GFX_BUSY) {
    uint16_t palette[] = {BLACK, WHITE};
    gfx_ = std::make_unique<MiniGrafx>(MiniGrafx(&epd_, BITS_PER_PIXEL, palette));

    initializeGfx();
  }

  int width() { return gfx_->getWidth(); }

  int height() { return gfx_->getHeight(); }

  void clearScreen() override { gfx_->fillBuffer(1); }

  void drawGridLine(const int x0, const int y0, const int x1, const int y1) override {
    gfx_->setColor(BLACK);
    gfx_->drawLine(x0, y0, x1, y1);
  }

  void drawAxis(const int x0, const int y0, const int x1, const int y1, const std::string& label, const int label_pos_x,
                const int label_pos_y, const TextAlignment label_position) override {
    gfx_->setColor(BLACK);
    gfx_->drawLine(x0, y0, x1, y1);
    drawTextLabel(label, label_pos_x, label_pos_y, label_position);
  }

  void drawTextLabel(const std::string& label, const int32_t x, const int32_t y,
                     const TextAlignment position) override {
    gfx_->setColor(BLACK);
    gfx_->setFont(ArialMT_Plain_10);
    gfx_->setTextAlignment(toGfxPosition(position));
    gfx_->drawString(x, y, String(label.c_str()));
  }

  void drawDataLabel(const float label, const int32_t x, const int32_t y, const TextAlignment position) override {
    gfx_->setColor(BLACK);
    gfx_->setFont(ArialMT_Plain_10);
    gfx_->setTextAlignment(toGfxPosition(position));
    gfx_->drawString(x, y, String(label));
  }

  void drawTimeLabel(const time_t time, const int32_t x, const int32_t y, const TextAlignment position) override {
    gfx_->setColor(BLACK);
    gfx_->setFont(ArialMT_Plain_10);
    gfx_->setTextAlignment(toGfxPosition(position));
    auto tm = *std::localtime(&time);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%H:%M");
    gfx_->drawString(x, y, String(oss.str().c_str()));
  }

  void drawPixel(const int32_t x, const int32_t y, const uint32_t /* color */) override {
    gfx_->setColor(BLACK);
    gfx_->setPixel(static_cast<uint16_t>(x), static_cast<uint16_t>(y));
  }

  void commit() override { gfx_->commit(); }

 private:
  void initializeGfx() {
    gfx_->init();
    gfx_->setRotation(0);
    gfx_->setFastRefresh(false);
    gfx_->fillBuffer(1);
    gfx_->setColor(BLACK);
    gfx_->setFont(ArialMT_Plain_10);
    gfx_->drawString(10, 10, "Hello World!");
    gfx_->setColor(BLACK);
    gfx_->setFont(ArialMT_Plain_10);
    gfx_->drawString(10, 10 + 1 * 50, "Hello World!");
    gfx_->setColor(BLACK);
    gfx_->setFont(ArialMT_Plain_10);
    gfx_->drawString(10, 10 + 2 * 50, "Hello World!");
    gfx_->commit();
  }

  TEXT_ALIGNMENT toGfxPosition(TextAlignment position) {
    switch (position) {
      case TextAlignment::BOTTOM_CENTER:
        return TEXT_ALIGN_CENTER;
      case TextAlignment::BOTTOM_RIGHT:
        return TEXT_ALIGN_RIGHT;
      case TextAlignment::MIDDLE_LEFT:
        return TEXT_ALIGN_LEFT;
      case TextAlignment::TOP_CENTER:
        return TEXT_ALIGN_CENTER;
      case TextAlignment::TOP_LEFT:
        return TEXT_ALIGN_LEFT;
      default:
        return TEXT_ALIGN_LEFT;
    }
  }

  EPD_WaveShare42 epd_;
  std::unique_ptr<MiniGrafx> gfx_;
};

}  // namespace display
}  // namespace weather_station

#endif
