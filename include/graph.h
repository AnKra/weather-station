#include <SPI.h>
#include <TFT_eSPI.h>

#include "colors.h"

void Graph(TFT_eSPI &tft, byte dp, int width, int height, double x_min,
           double x_max, double cell_width, double y_min, double y_max,
           double cell_height, const String &title, const String &x_label,
           const String &y_label) {
  auto grid_color = DKBLUE;
  auto axes_color = RED;
  auto text_color = WHITE;
  auto background_color = BLACK;
  uint8_t font = 1;

  // draw y scale
  auto unit_height_px = height / (y_max - y_min);
  for (auto y = y_min; y <= y_max; y += cell_height) {
    auto y_px = height - 1 - (y - y_min) * unit_height_px;
    if (y == 0) {
      tft.drawLine(0, y_px, width, y_px, axes_color);

      // axis label
      tft.setTextColor(axes_color, background_color);
      tft.setTextDatum(MR_DATUM);
      tft.drawString(x_label, (int)width, (int)y_px, font);
    } else {
      tft.drawLine(0, y_px, width, y_px, grid_color);
    }

    // axis labels
    tft.setTextDatum(ML_DATUM);
    tft.setTextColor(text_color, background_color);
    tft.drawFloat(y, dp, 0, y_px, font);
  }

  // draw x scale
  auto unit_width_px = width / (x_max - x_min);
  for (auto x = x_min; x <= x_max; x += cell_width) {
    auto x_px = (x - x_min) * unit_width_px;
    if (x == 0) {
      tft.drawLine(x_px, 0, x_px, height, axes_color);

      // axis label
      tft.setTextColor(axes_color, background_color);
      tft.setTextDatum(TL_DATUM);
      tft.drawString(y_label, (int)x_px, 0, font);
    } else {
      tft.drawLine(x_px, 0, x_px, height, grid_color);
    }

    // axis labels
    tft.setTextDatum(BC_DATUM);
    tft.setTextColor(text_color, background_color);
    tft.drawFloat(x, dp, (int)x_px, height, font);
  }

  // now draw the graph labels
  tft.setTextDatum(TC_DATUM);
  tft.setTextColor(text_color, background_color);
  tft.drawString(title, (int)(width / 2), 0, 1);
}

void drawPixel(TFT_eSPI &tft, double x, double y, int width, int height,
               double x_min, double x_max, double cell_width, double y_min,
               double y_max, double cell_height, unsigned int color) {
  auto data_color = color;

  auto unit_width_px = width / (x_max - x_min);
  auto unit_height_px = height / (y_max - y_min);

  auto x_abs = x - x_min;
  auto y_abs = y - y_min;

  auto x_pixel = x_abs * unit_width_px;
  auto y_pixel = height - 1 - y_abs * unit_height_px;

  tft.drawPixel(x_pixel, y_pixel, data_color);
  // tft.setTextDatum(MC_DATUM);
  // tft.drawChar(x_pixel, y_pixel, 'x', data_color, 1, 1);
  // tft.drawLine(ox, oy, x, y, data_color);
}
