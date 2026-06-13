#include <ctype.h>

#ifdef USE_DESKTOP
  #include "desktop.h"
#else
  #include <WiFi.h>
  #include <WiFiClientSecure.h>
  #include <HTTPClient.h>
  #include <ArduinoJson.h>
  #include <Print.h>
#endif

#define DISPLAY_VERSION "0.0.1.7"

#ifndef ENABLE_PISTAR_TOUCH_SSH
  #define ENABLE_PISTAR_TOUCH_SSH 1
#endif

#define DISPLAY_BACKEND_2432S028R         0 /* ESP32-2432S028R, also known as CYD / Cheap Yellow Display */
#define DISPLAY_BACKEND_JC3248W535C       1 /* ESP32-S3 JC3248W535C */
#define DISPLAY_BACKEND_WS8048S043C       2 /* Waveshare ESP32-S3-Touch-LCD-4.3C / WS8048S043C */
#define DISPLAY_BACKEND_WS8048S070C       3 /* Waveshare ESP32-S3-Touch-LCD-7 */
#define DISPLAY_BACKEND_EC8048S070C       4 /* Elecrow CrowPanel 7.0 Advance */
#define DISPLAY_BACKEND_EC8048S070B       5 /* Elecrow CrowPanel 7.0 Basic */
#define DISPLAY_BACKEND_EC8048S070CT      6 /* EC8048S070CT / ESP32-S3-Touch-LCD-7 */
#define DISPLAY_BACKEND_WSTOUCHLCD28      7 /* Waveshare ESP32-S3-Touch-LCD-2.8 */
#define DISPLAY_BACKEND_DESKTOP_SDL2      100 /* Desktop SDL2 demo, Windows/Linux */

#ifdef USE_DESKTOP
  #ifdef ENABLE_PISTAR_TOUCH_SSH
    #undef ENABLE_PISTAR_TOUCH_SSH
  #endif
  #define ENABLE_PISTAR_TOUCH_SSH 0
  #ifdef DISPLAY_BACKEND
    #undef DISPLAY_BACKEND
  #endif
  #define DISPLAY_BACKEND DISPLAY_BACKEND_DESKTOP_SDL2
#endif

#ifndef DISPLAY_BACKEND
  #define DISPLAY_BACKEND DISPLAY_BACKEND_WSTOUCHLCD28
#endif

#if ENABLE_PISTAR_TOUCH_SSH && !defined(USE_DESKTOP)
  #include "libssh_esp32.h"
  extern "C" {
    #include <libssh/libssh.h>
  }
#endif

#define DISPLAY_BACKEND_ESP32_S3_TOUCH_LCD_7 DISPLAY_BACKEND_WS8048S070C
#define DISPLAY_BACKEND_ESP32_S3_TOUCH_LCD_2_8 DISPLAY_BACKEND_WSTOUCHLCD28
#define DISPLAY_BACKEND_WS_TOUCHLCD28 DISPLAY_BACKEND_WSTOUCHLCD28
#define DISPLAY_BACKEND_ESP32_8048S070CT DISPLAY_BACKEND_EC8048S070CT
#define DISPLAY_BACKEND_ELECROW_CROWPANEL_ADVANCE_7 DISPLAY_BACKEND_EC8048S070C
#define DISPLAY_BACKEND_ELECROW_CROWPANEL_BASIC_7   DISPLAY_BACKEND_EC8048S070B
#define DISPLAY_BACKEND_IS_ELECROW_CROWPANEL_ADVANCE_7 (DISPLAY_BACKEND == DISPLAY_BACKEND_EC8048S070C)
#define DISPLAY_BACKEND_IS_ELECROW_CROWPANEL_BASIC_7   (DISPLAY_BACKEND == DISPLAY_BACKEND_EC8048S070B)
#define DISPLAY_BACKEND_IS_ELECROW_ESP32_DISPLAY_7 (DISPLAY_BACKEND_IS_ELECROW_CROWPANEL_ADVANCE_7 || DISPLAY_BACKEND_IS_ELECROW_CROWPANEL_BASIC_7)
#define DISPLAY_BACKEND_IS_EC8048S070CT (DISPLAY_BACKEND == DISPLAY_BACKEND_EC8048S070CT)
#define DISPLAY_BACKEND_IS_WS_RGB_800_480 (DISPLAY_BACKEND == DISPLAY_BACKEND_WS8048S043C || DISPLAY_BACKEND == DISPLAY_BACKEND_WS8048S070C || DISPLAY_BACKEND_IS_EC8048S070CT || DISPLAY_BACKEND_IS_ELECROW_ESP32_DISPLAY_7)
#define DISPLAY_BACKEND_IS_WS_TOUCH_LCD_2_8 (DISPLAY_BACKEND == DISPLAY_BACKEND_WSTOUCHLCD28)
#define DISPLAY_BACKEND_IS_DESKTOP (DISPLAY_BACKEND == DISPLAY_BACKEND_DESKTOP_SDL2)
#define DISPLAY_BACKEND_IS_GFX_COMPAT (DISPLAY_BACKEND == DISPLAY_BACKEND_JC3248W535C || DISPLAY_BACKEND_IS_WS_RGB_800_480 || DISPLAY_BACKEND_IS_WS_TOUCH_LCD_2_8 || DISPLAY_BACKEND_IS_DESKTOP)

#ifndef WS_RGB_LCD_INVERT_COLORS
  #define WS_RGB_LCD_INVERT_COLORS 0
#endif

#if DISPLAY_BACKEND == DISPLAY_BACKEND_2432S028R
  #define DISPLAY_NATIVE_WIDTH  240
  #define DISPLAY_NATIVE_HEIGHT 320

  #define DISPLAY_LOGICAL_WIDTH  DISPLAY_NATIVE_HEIGHT
  #define DISPLAY_LOGICAL_HEIGHT DISPLAY_NATIVE_WIDTH
  #define DISPLAY_ROTATION_NORMAL  1
  #define DISPLAY_ROTATION_FLIPPED 3

  #include <TFT_eSPI.h>
  #include <XPT2046_Touchscreen.h>

  #include "User_Setup.h"

  #if defined(ST7789_DRIVER)
    #define DISPLAY_CONTROLLER_NAME "ST7789"
  #elif defined(ILI9341_2_DRIVER)
    #define DISPLAY_CONTROLLER_NAME "ILI9341_2"
  #elif defined(ILI9341_DRIVER)
    #define DISPLAY_CONTROLLER_NAME "ILI9341"
  #else
    #define DISPLAY_CONTROLLER_NAME "TFT_eSPI"
  #endif
#elif DISPLAY_BACKEND == DISPLAY_BACKEND_JC3248W535C
  #include <Arduino_GFX_Library.h>
  #include <lvgl.h>
  #include <Wire.h>

  #define USER_SETUP_INFO "JC3248W535C"
  #define DISPLAY_CONTROLLER_NAME "AXS15231B"

  #define TFT_WIDTH  480
  #define TFT_HEIGHT 320

  #define DISPLAY_ROTATION_NORMAL  1
  #define DISPLAY_ROTATION_FLIPPED 3

  #define TFT_BL   1
  #define TFT_BACKLIGHT_ON HIGH

  #define TFT_MISO -1
  #define TFT_MOSI -1
  #define TFT_SCLK 47
  #define TFT_CS   45
  #define TFT_DC   -1
  #define TFT_RST  -1

  #define TOUCH_IRQ    3
  #define TOUCH_MOSI   4
  #define TOUCH_MISO  -1
  #define TOUCH_CLK    8L
  #define TOUCH_CS    -1

  #define SD_CS   10
  #define SD_MISO 13
  #define SD_MOSI 11
  #define SD_SCLK 12

  #define SPI_FREQUENCY       40000000
  #define SPI_READ_FREQUENCY  40000000
  #define SPI_TOUCH_FREQUENCY 400000

  #ifndef JC3248_ENABLE_LVGL
    #define JC3248_ENABLE_LVGL 0
  #endif

  static TwoWire lcdwikiTouchWire = TwoWire(1);
  static inline uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
  }

  #ifndef TFT_BLACK
    #define TFT_BLACK     color565(0, 0, 0)
    #define TFT_WHITE     color565(255, 255, 255)
    #define TFT_RED       color565(255, 0, 0)
    #define TFT_GREEN     color565(0, 255, 0)
    #define TFT_BLUE      color565(0, 0, 255)
    #define TFT_YELLOW    color565(255, 255, 0)
    #define TFT_CYAN      color565(0, 255, 255)
    #define TFT_DARKGREY  color565(64, 64, 64)
    #define TFT_LIGHTGREY color565(192, 192, 192)
  #endif

  #ifndef TL_DATUM
    #define TL_DATUM 0
    #define TC_DATUM 1
    #define TR_DATUM 2
    #define ML_DATUM 3
    #define MC_DATUM 4
    #define MR_DATUM 5
  #endif

  struct TS_Point {
    int16_t x;
    int16_t y;
    int16_t z;
    TS_Point(int16_t px = 0, int16_t py = 0, int16_t pz = 0) : x(px), y(py), z(pz) {}
  };

  class DisplayCompat {
  public:
    static const int16_t PHYS_W = 480;
    static const int16_t PHYS_H = 320;
    static const int16_t LOGICAL_W = PHYS_W;
    static const int16_t LOGICAL_H = PHYS_H;

    Arduino_DataBus *bus = nullptr;
    Arduino_AXS15231B *panel = nullptr;
    Arduino_Canvas *gfx = nullptr;
    uint8_t datum = TL_DATUM;
    uint8_t rotation = 1;
    uint8_t textSize = 1;
    uint16_t fg = TFT_WHITE;
    uint16_t bg = TFT_BLACK;

    bool initOk = false;

    float scale() const {
      float sx = (float)PHYS_W / (float)LOGICAL_W;
      float sy = (float)PHYS_H / (float)LOGICAL_H;
      return (sx < sy) ? sx : sy;
    }

    int16_t scaledW() const { return (int16_t)lroundf(LOGICAL_W * scale()); }
    int16_t scaledH() const { return (int16_t)lroundf(LOGICAL_H * scale()); }
    int16_t originX() const { return (PHYS_W - scaledW()) / 2; }
    int16_t originY() const { return (PHYS_H - scaledH()) / 2; }
    int16_t tx(int16_t x) const { return originX() + (int16_t)lroundf(x * scale()); }
    int16_t ty(int16_t y) const { return originY() + (int16_t)lroundf(y * scale()); }
    int16_t tw(int16_t w) const {
      int16_t v = (int16_t)lroundf(w * scale());
      return (v < 1) ? 1 : v;
    }
    int16_t th(int16_t h) const {
      int16_t v = (int16_t)lroundf(h * scale());
      return (v < 1) ? 1 : v;
    }
    int16_t tr(int16_t r) const {
      int16_t v = (int16_t)lroundf(r * scale());
      return (v < 1) ? 1 : v;
    }

    void init() {
      bus = new Arduino_ESP32QSPI(TFT_CS, TFT_SCLK, 21, 48, 40, 39);
      if (!bus) { initOk = false; return; }

      panel = new Arduino_AXS15231B(bus, TFT_RST, 0, false, 320, 480);
      if (!panel) { initOk = false; return; }

      gfx = new Arduino_Canvas(320, 480, panel, 0, 0, rotation);
      if (!gfx) { initOk = false; return; }

      initOk = gfx->begin();
      if (!initOk) {
        Serial.println("WARNING: RGB display begin() failed for WS8048/ESP32-S3-Touch-LCD-7 backend.");
        return;
      }

      gfx->invertDisplay(WS_RGB_LCD_INVERT_COLORS);
      Serial.printf("[INFO][LCD] ILI9341V invertDisplay(%d). If colours are negative, set LCD_INVERT_COLORS to 0.\n", (int)WS_RGB_LCD_INVERT_COLORS);
      gfx->setRotation(rotation);
      gfx->setTextWrap(false);
      gfx->setTextSize(textSize);
      gfx->setTextColor(fg, bg);
      gfx->fillScreen(TFT_BLACK);
      gfx->flush();
    }

    bool ready() const { return initOk && gfx != nullptr; }

    void setRotation(uint8_t r) {
      rotation = r;
      if (ready()) gfx->setRotation(rotation);
      flush();
    }

    int16_t width() const { return LOGICAL_W; }
    int16_t height() const { return LOGICAL_H; }

    void flush() { if (ready()) gfx->flush(); }

    void fillScreen(uint16_t c) { if (ready()) gfx->fillScreen(c); }
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t c) {
      if (ready()) gfx->fillRect(tx(x), ty(y), tw(w), th(h), c);
    }
    void drawPixel(int16_t x, int16_t y, uint16_t c) { if (ready()) gfx->drawPixel(tx(x), ty(y), c); }
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t c) {
      if (ready()) gfx->drawLine(tx(x0), ty(y0), tx(x1), ty(y1), c);
    }
    void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t c) {
      if (ready()) gfx->drawFastHLine(tx(x), ty(y), tw(w), c);
    }
    void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t c) {
      if (ready()) gfx->drawFastVLine(tx(x), ty(y), th(h), c);
    }
    void drawCircle(int16_t x, int16_t y, int16_t r, uint16_t c) {
      if (ready()) gfx->drawCircle(tx(x), ty(y), tr(r), c);
    }
    void fillCircle(int16_t x, int16_t y, int16_t r, uint16_t c) {
      if (ready()) gfx->fillCircle(tx(x), ty(y), tr(r), c);
    }
    void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t c) {
      if (ready()) gfx->drawRoundRect(tx(x), ty(y), tw(w), th(h), tr(r), c);
    }
    void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t c) {
      if (ready()) gfx->fillRoundRect(tx(x), ty(y), tw(w), th(h), tr(r), c);
    }

    void setTextColor(uint16_t fgColor, uint16_t bgColor = TFT_BLACK) {
      fg = fgColor;
      bg = bgColor;
      if (ready()) gfx->setTextColor(fg, bg);
    }

    void setTextDatum(uint8_t d) { datum = d; }

    void setTextFont(uint8_t font) {
      if (font >= 8) textSize = 10;
      else if (font >= 7) textSize = 8;
      else if (font >= 6) textSize = 6;
      else if (font >= 4) textSize = 4;
      else if (font >= 2) textSize = 2;
      else textSize = 1;

      if (gfx) {
        gfx->setFont(nullptr);
        gfx->setTextSize(textSize);
      }
    }

    int16_t textWidth(const String& s) {
      if (!gfx) return 0;

      gfx->setFont(nullptr);
      gfx->setTextSize(textSize);
      gfx->setTextWrap(false);

      int16_t bx = 0;
      int16_t by = 0;
      uint16_t bw = 0;
      uint16_t bh = 0;
      gfx->getTextBounds(s, 0, 0, &bx, &by, &bw, &bh);

      return (int16_t)lroundf((float)bw / scale());
    }

    int16_t drawString(const String& s, int32_t x, int32_t y) {
      if (!gfx) return 0;

      gfx->setFont(nullptr);
      gfx->setTextSize(textSize);
      gfx->setTextWrap(false);

      int16_t bx = 0;
      int16_t by = 0;
      uint16_t bw = 0;
      uint16_t bh = 0;
      gfx->getTextBounds(s, 0, 0, &bx, &by, &bw, &bh);

      int16_t logicalW = (int16_t)lroundf((float)bw / scale());
      int16_t logicalH = (int16_t)lroundf((float)bh / scale());

      int32_t px = tx((int16_t)x);
      int32_t py = ty((int16_t)y);

      switch (datum) {
        case TC_DATUM:
          px -= (int32_t)bw / 2;
          break;
        case TR_DATUM:
          px -= (int32_t)bw;
          break;
        case ML_DATUM:
          py -= (int32_t)bh / 2;
          break;
        case MC_DATUM:
          px -= (int32_t)bw / 2;
          py -= (int32_t)bh / 2;
          break;
        case MR_DATUM:
          px -= (int32_t)bw;
          py -= (int32_t)bh / 2;
          break;
        default:
          break;
      }

      px -= bx;
      py -= by;

      gfx->setCursor(px, py);
      gfx->print(s);
      return logicalW > 0 ? logicalW : logicalH;
    }

    bool pushImage(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t *pixels) {
      if (!ready() || !pixels || w <= 0 || h <= 0) return false;
      gfx->draw16bitRGBBitmap(tx(x), ty(y), const_cast<uint16_t*>(pixels), w, h);
      return true;
    }

    bool pushLvglArea(int32_t x, int32_t y, int32_t w, int32_t h, const uint16_t *pixels) {
      if (!panel || !pixels || w <= 0 || h <= 0) return false;
      panel->draw16bitRGBBitmap(tx((int16_t)x), ty((int16_t)y), const_cast<uint16_t*>(pixels), w, h);
      return true;
    }
  };

  class TouchCompat {
  public:
    static const uint8_t TOUCH_ADDR = 0x3B;
    static const uint8_t MAX_TOUCHES = 1;
    static const int16_t PHYS_W = 480;
    static const int16_t PHYS_H = 320;
    static const int16_t LOGICAL_W = PHYS_W;
    static const int16_t LOGICAL_H = PHYS_H;

    bool initOk = false;
    uint8_t rotation = 1;
    TS_Point lastPoint;
    bool lastTouched = false;
    uint32_t lastPollMs = 0;

    float scale() const {
      float sx = (float)PHYS_W / (float)LOGICAL_W;
      float sy = (float)PHYS_H / (float)LOGICAL_H;
      return (sx < sy) ? sx : sy;
    }

    int16_t scaledW() const { return (int16_t)lroundf(LOGICAL_W * scale()); }
    int16_t scaledH() const { return (int16_t)lroundf(LOGICAL_H * scale()); }
    int16_t originX() const { return (PHYS_W - scaledW()) / 2; }
    int16_t originY() const { return (PHYS_H - scaledH()) / 2; }

    void begin(SPIClass&) {
      Wire.begin(TOUCH_MOSI, TOUCH_CLK);
      lcdwikiTouchWire.setClock(400000);
      if (TOUCH_IRQ >= 0) pinMode(TOUCH_IRQ, INPUT_PULLUP);
      if (TOUCH_CS >= 0) {
        pinMode(TOUCH_CS, OUTPUT);
        digitalWrite(TOUCH_CS, LOW);
        delay(20);
        digitalWrite(TOUCH_CS, HIGH);
        delay(200);
      } else {
        delay(80);
      }
      initOk = true;
      lastTouched = false;
    }

    void setRotation(uint8_t r) { rotation = r; }

    bool touched() {
      poll();
      return lastTouched;
    }

    TS_Point getPoint() const { return lastPoint; }

  private:
    void poll() {
      if (!initOk) {
        lastTouched = false;
        lastPoint = TS_Point();
        return;
      }

      if ((millis() - lastPollMs) < 12) return;
      lastPollMs = millis();

      uint8_t data[MAX_TOUCHES * 6 + 2] = {0};
      const uint8_t read_cmd[11] = {
        0xB5, 0xAB, 0xA5, 0x5A, 0x00, 0x00,
        (uint8_t)(((MAX_TOUCHES * 6) + 2) >> 8),
        (uint8_t)(((MAX_TOUCHES * 6) + 2) & 0xFF),
        0x00, 0x00, 0x00
      };

      Wire.beginTransmission(TOUCH_ADDR);
      Wire.write(read_cmd, sizeof(read_cmd));
      if (Wire.endTransmission() != 0) {
        lastTouched = false;
        lastPoint = TS_Point();
        return;
      }

      if (Wire.requestFrom((int)TOUCH_ADDR, (int)sizeof(data)) != (int)sizeof(data)) {
        lastTouched = false;
        lastPoint = TS_Point();
        return;
      }

      for (size_t i = 0; i < sizeof(data); ++i) {
        data[i] = Wire.read();
      }

      if (data[1] == 0 || data[1] > MAX_TOUCHES) {
        lastTouched = false;
        lastPoint = TS_Point();
        return;
      }

      uint16_t rawX = ((data[2] & 0x0F) << 8) | data[3];
      uint16_t rawY = ((data[4] & 0x0F) << 8) | data[5];

      if ((rawX == 273 && rawY == 273) || rawX > 4000 || rawY > 4000) {
        lastTouched = false;
        lastPoint = TS_Point();
        return;
      }

      int16_t physX = constrain((int16_t)rawY, 0, PHYS_W - 1);
      int16_t physY = constrain((int16_t)map(rawX, 0, 320, PHYS_H, 0), 0, PHYS_H - 1);

      if (rotation == 3) {
        physX = (PHYS_W - 1) - physX;
        physY = (PHYS_H - 1) - physY;
      }

      if (physX < originX() || physX >= (originX() + scaledW()) || physY < originY() || physY >= (originY() + scaledH())) {
        lastTouched = false;
        lastPoint = TS_Point();
        return;
      }

      int16_t logicalX = (int16_t)((physX - originX()) / scale());
      int16_t logicalY = (int16_t)((physY - originY()) / scale());

      lastPoint = TS_Point(
        constrain(logicalX, 0, LOGICAL_W - 1),
        constrain(logicalY, 0, LOGICAL_H - 1),
        1
      );
      lastTouched = true;
    }
  };

  extern DisplayCompat tft;

  #if JC3248_ENABLE_LVGL
    static uint32_t jc3248_lv_last_tick = 0;
    static bool jc3248_lv_ready = false;

    #if LVGL_VERSION_MAJOR >= 9
      static lv_display_t *jc3248_lv_display = nullptr;
      static lv_color_t *jc3248_lv_buf_1 = nullptr;
      static lv_color_t *jc3248_lv_buf_2 = nullptr;

      static void jc3248LvglFlush(lv_display_t *disp, const lv_area_t *area, uint8_t *color_p) {
        (void)disp;
        int32_t w = area->x2 - area->x1 + 1;
        int32_t h = area->y2 - area->y1 + 1;
        tft.pushLvglArea(area->x1, area->y1, w, h, reinterpret_cast<const uint16_t *>(color_p));
        lv_display_flush_ready(disp);
      }
    #else
      static lv_disp_draw_buf_t jc3248_lv_draw_buf;
      static lv_color_t *jc3248_lv_buf_1 = nullptr;
      static lv_color_t *jc3248_lv_buf_2 = nullptr;
      static lv_disp_drv_t jc3248_lv_drv;

      static void jc3248LvglFlush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
        int32_t w = area->x2 - area->x1 + 1;
        int32_t h = area->y2 - area->y1 + 1;
        tft.pushLvglArea(area->x1, area->y1, w, h, reinterpret_cast<const uint16_t *>(color_p));
        lv_disp_flush_ready(disp);
      }
    #endif

    static void jc3248InitLvglBridge() {
      if (jc3248_lv_ready) return;

      lv_init();

      size_t buffer_pixels = TFT_WIDTH * 40;
      jc3248_lv_buf_1 = static_cast<lv_color_t*>(heap_caps_malloc(buffer_pixels * sizeof(lv_color_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
      jc3248_lv_buf_2 = static_cast<lv_color_t*>(heap_caps_malloc(buffer_pixels * sizeof(lv_color_t), MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
      if (!jc3248_lv_buf_1) jc3248_lv_buf_1 = static_cast<lv_color_t*>(heap_caps_malloc(buffer_pixels * sizeof(lv_color_t), MALLOC_CAP_8BIT));
      if (!jc3248_lv_buf_2) jc3248_lv_buf_2 = static_cast<lv_color_t*>(heap_caps_malloc(buffer_pixels * sizeof(lv_color_t), MALLOC_CAP_8BIT));
      if (!jc3248_lv_buf_1 || !jc3248_lv_buf_2) return;

      #if LVGL_VERSION_MAJOR >= 9
        jc3248_lv_display = lv_display_create(TFT_WIDTH, TFT_HEIGHT);
        if (!jc3248_lv_display) return;
        lv_display_set_flush_cb(jc3248_lv_display, jc3248LvglFlush);
        lv_display_set_buffers(
          jc3248_lv_display,
          jc3248_lv_buf_1,
          jc3248_lv_buf_2,
          buffer_pixels * sizeof(lv_color_t),
          LV_DISPLAY_RENDER_MODE_PARTIAL
        );
      #else
        lv_disp_draw_buf_init(&jc3248_lv_draw_buf, jc3248_lv_buf_1, jc3248_lv_buf_2, buffer_pixels);
        lv_disp_drv_init(&jc3248_lv_drv);
        jc3248_lv_drv.hor_res = TFT_WIDTH;
        jc3248_lv_drv.ver_res = TFT_HEIGHT;
        jc3248_lv_drv.flush_cb = jc3248LvglFlush;
        jc3248_lv_drv.draw_buf = &jc3248_lv_draw_buf;
        lv_disp_drv_register(&jc3248_lv_drv);
      #endif

      jc3248_lv_last_tick = millis();
      jc3248_lv_ready = true;
    }

    static void jc3248HandleLvgl() {
      if (!jc3248_lv_ready) return;
      uint32_t now = millis();
      uint32_t elapsed = now - jc3248_lv_last_tick;
      if (elapsed > 0) {
        #if LVGL_VERSION_MAJOR >= 9
          lv_tick_inc(elapsed);
          lv_timer_handler();
        #else
          lv_tick_inc(elapsed);
          lv_timer_handler();
        #endif
        jc3248_lv_last_tick = now;
      }
    }
  #else
    static void jc3248InitLvglBridge() {}
    static void jc3248HandleLvgl() {}
  #endif

#elif DISPLAY_BACKEND_IS_WS_TOUCH_LCD_2_8
  #include <Arduino_GFX_Library.h>
  #include <Wire.h>

  #define USER_SETUP_INFO "LCDwiki ESP32-S3 2.8 ES3C28P/ES3N28P"
  #define DISPLAY_CONTROLLER_NAME "ILI9341V / FT6336G"

  #ifndef WS_TOUCH_LCD_2_8_INVERT_COLORS
    #define WS_TOUCH_LCD_2_8_INVERT_COLORS 1
  #endif

  #ifndef LCDWIKI_FONT1_TEXTSIZE
    #define LCDWIKI_FONT1_TEXTSIZE 1
  #endif
  #ifndef LCDWIKI_FONT2_TEXTSIZE
    #define LCDWIKI_FONT2_TEXTSIZE 1
  #endif
  #ifndef LCDWIKI_FONT4_TEXTSIZE
    #define LCDWIKI_FONT4_TEXTSIZE 2
  #endif
  #ifndef LCDWIKI_FONT6_TEXTSIZE
    #define LCDWIKI_FONT6_TEXTSIZE 3
  #endif
  #ifndef LCDWIKI_FONT7_TEXTSIZE
    #define LCDWIKI_FONT7_TEXTSIZE 3
  #endif
  #ifndef LCDWIKI_FONT8_TEXTSIZE
    #define LCDWIKI_FONT8_TEXTSIZE 4
  #endif

  static inline uint8_t lcdwikiTextSizeForFont(uint8_t font) {
    if (font >= 8) return LCDWIKI_FONT8_TEXTSIZE;
    if (font >= 7) return LCDWIKI_FONT7_TEXTSIZE;
    if (font >= 6) return LCDWIKI_FONT6_TEXTSIZE;
    if (font >= 4) return LCDWIKI_FONT4_TEXTSIZE;
    if (font >= 2) return LCDWIKI_FONT2_TEXTSIZE;
    return LCDWIKI_FONT1_TEXTSIZE;
  }

  #define TFT_NATIVE_WIDTH  240
  #define TFT_NATIVE_HEIGHT 320
  #define TFT_WIDTH  320
  #define TFT_HEIGHT 240

  #define DISPLAY_ROTATION_NORMAL  1
  #define DISPLAY_ROTATION_FLIPPED 3

  #define TFT_BL   45
  #define TFT_BACKLIGHT_ON HIGH
  #define TFT_CS   10
  #define TFT_DC   46
  #define TFT_SCLK 12
  #define TFT_MOSI 11
  #define TFT_MISO 13
  #define TFT_RST  -1

  #define TOUCH_SDA  16
  #define TOUCH_SCL  15
  #define TOUCH_RST  18
  #define TOUCH_IRQ  17
  #define TOUCH_MOSI TOUCH_SDA
  #define TOUCH_MISO -1
  #define TOUCH_CLK  TOUCH_SCL
  #define TOUCH_CS   -1

  #ifndef WS28_ENABLE_TOUCH
    #define WS28_ENABLE_TOUCH 1
  #endif

  #ifndef WS28_TOUCH_I2C_SCAN
    #define WS28_TOUCH_I2C_SCAN 1
  #endif

  static TwoWire lcdwikiTouchWire = TwoWire(1);

  #ifndef WS28_USE_SD_MMC
    #define WS28_USE_SD_MMC 1
  #endif
  #define WS28_SD_CLK 38
  #define WS28_SD_CMD 40
  #define WS28_SD_D0  39
  #define WS28_SD_D1  41
  #define WS28_SD_D2  48
  #define WS28_SD_D3  47

  #define SD_CS   WS28_SD_D3
  #define SD_MISO WS28_SD_D0
  #define SD_MOSI WS28_SD_CMD
  #define SD_SCLK WS28_SD_CLK

  static inline uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
  }

  #ifndef TFT_BLACK
    #define TFT_BLACK     color565(0, 0, 0)
    #define TFT_WHITE     color565(255, 255, 255)
    #define TFT_RED       color565(255, 0, 0)
    #define TFT_GREEN     color565(0, 255, 0)
    #define TFT_BLUE      color565(0, 0, 255)
    #define TFT_YELLOW    color565(255, 255, 0)
    #define TFT_CYAN      color565(0, 255, 255)
    #define TFT_DARKGREY  color565(64, 64, 64)
    #define TFT_LIGHTGREY color565(192, 192, 192)
  #endif

  #ifndef TL_DATUM
    #define TL_DATUM 0
    #define TC_DATUM 1
    #define TR_DATUM 2
    #define ML_DATUM 3
    #define MC_DATUM 4
    #define MR_DATUM 5
  #endif

  struct TS_Point {
    int16_t x;
    int16_t y;
    int16_t z;
    TS_Point(int16_t px = 0, int16_t py = 0, int16_t pz = 0) : x(px), y(py), z(pz) {}
  };

  class DisplayCompat {
  public:
    static const int16_t PHYS_W = TFT_WIDTH;
    static const int16_t PHYS_H = TFT_HEIGHT;
    static const int16_t LOGICAL_W = PHYS_W;
    static const int16_t LOGICAL_H = PHYS_H;

    Arduino_DataBus *bus = nullptr;
    Arduino_GFX *gfx = nullptr;
    uint8_t datum = TL_DATUM;
    uint8_t rotation = DISPLAY_ROTATION_NORMAL;
    uint8_t textSize = 1;
    uint16_t fg = TFT_WHITE;
    uint16_t bg = TFT_BLACK;
    bool initOk = false;

    float scale() const { return 1.0f; }
    int16_t tx(int16_t x) const { return x; }
    int16_t ty(int16_t y) const { return y; }
    int16_t tw(int16_t w) const { return (w < 1) ? 1 : w; }
    int16_t th(int16_t h) const { return (h < 1) ? 1 : h; }
    int16_t tr(int16_t r) const { return (r < 1) ? 1 : r; }

    void init() {
      pinMode(TFT_CS, OUTPUT);
      digitalWrite(TFT_CS, HIGH);
      #if !WS28_USE_SD_MMC
        pinMode(SD_CS, OUTPUT);
        digitalWrite(SD_CS, HIGH);
      #endif
      pinMode(TFT_BL, OUTPUT);
      digitalWrite(TFT_BL, TFT_BACKLIGHT_ON);

      pinMode(TFT_DC, OUTPUT);
      if (TFT_RST >= 0) {
        pinMode(TFT_RST, OUTPUT);
        digitalWrite(TFT_RST, HIGH); delay(20);
        digitalWrite(TFT_RST, LOW);  delay(50);
        digitalWrite(TFT_RST, HIGH); delay(120);
      }

      bus = new Arduino_ESP32SPI(TFT_DC, TFT_CS, TFT_SCLK, TFT_MOSI, TFT_MISO, FSPI);
      if (!bus) { initOk = false; return; }

      gfx = new Arduino_ILI9341(bus, TFT_RST);
      if (!gfx) { initOk = false; return; }

      initOk = gfx->begin(40000000);
      if (!initOk) {
        Serial.println("WARNING: ILI9341 display begin() failed for ESP32-S3 2.8 backend.");
        return;
      }

      gfx->invertDisplay(WS_TOUCH_LCD_2_8_INVERT_COLORS);
      Serial.printf("[INFO][LCD] ILI9341V invertDisplay(%d). If colours are negative, set LCD_INVERT_COLORS to 0.\n", (int)WS_TOUCH_LCD_2_8_INVERT_COLORS);
      gfx->setRotation(rotation);
      gfx->setTextWrap(false);
      gfx->setTextSize(textSize);
      gfx->setTextColor(fg, bg);
      Serial.printf("[INFO][LCD] SMALL font map: font1=%d font2=%d font4=%d font6=%d font7=%d font8=%d.\n",
                    LCDWIKI_FONT1_TEXTSIZE, LCDWIKI_FONT2_TEXTSIZE, LCDWIKI_FONT4_TEXTSIZE,
                    LCDWIKI_FONT6_TEXTSIZE, LCDWIKI_FONT7_TEXTSIZE, LCDWIKI_FONT8_TEXTSIZE);
      gfx->fillScreen(TFT_BLACK);
    }

    bool ready() const { return initOk && gfx != nullptr; }

    void setRotation(uint8_t r) {
      rotation = r;
      if (ready()) gfx->setRotation(rotation);
      flush();
    }

    int16_t width() const { return LOGICAL_W; }
    int16_t height() const { return LOGICAL_H; }

    void flush() { if (ready()) gfx->flush(); }

    void fillScreen(uint16_t c) { if (ready()) gfx->fillScreen(c); }
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t c) { if (ready()) gfx->fillRect(tx(x), ty(y), tw(w), th(h), c); }
    void drawPixel(int16_t x, int16_t y, uint16_t c) { if (ready()) gfx->drawPixel(tx(x), ty(y), c); }
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t c) { if (ready()) gfx->drawLine(tx(x0), ty(y0), tx(x1), ty(y1), c); }
    void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t c) { if (ready()) gfx->drawFastHLine(tx(x), ty(y), tw(w), c); }
    void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t c) { if (ready()) gfx->drawFastVLine(tx(x), ty(y), th(h), c); }
    void drawCircle(int16_t x, int16_t y, int16_t r, uint16_t c) { if (ready()) gfx->drawCircle(tx(x), ty(y), tr(r), c); }
    void fillCircle(int16_t x, int16_t y, int16_t r, uint16_t c) { if (ready()) gfx->fillCircle(tx(x), ty(y), tr(r), c); }
    void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t c) { if (ready()) gfx->drawRoundRect(tx(x), ty(y), tw(w), th(h), tr(r), c); }
    void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t c) { if (ready()) gfx->fillRoundRect(tx(x), ty(y), tw(w), th(h), tr(r), c); }

    void setTextColor(uint16_t fgColor, uint16_t bgColor = TFT_BLACK) { fg = fgColor; bg = bgColor; if (ready()) gfx->setTextColor(fg, bg); }
    void setTextDatum(uint8_t d) { datum = d; }

    void setTextFont(uint8_t font) {
      textSize = lcdwikiTextSizeForFont(font);
      if (gfx) { gfx->setFont(nullptr); gfx->setTextSize(textSize); }
    }

    int16_t textWidth(const String& s) {
      if (!gfx) return 0;
      gfx->setFont(nullptr); gfx->setTextSize(textSize); gfx->setTextWrap(false);
      int16_t bx = 0, by = 0; uint16_t bw = 0, bh = 0;
      gfx->getTextBounds(s, 0, 0, &bx, &by, &bw, &bh);
      return (int16_t)bw;
    }

    int16_t drawString(const String& s, int32_t x, int32_t y) {
      if (!gfx) return 0;
      gfx->setFont(nullptr); gfx->setTextSize(textSize); gfx->setTextWrap(false);
      int16_t bx = 0, by = 0; uint16_t bw = 0, bh = 0;
      gfx->getTextBounds(s, 0, 0, &bx, &by, &bw, &bh);
      int32_t px = tx((int16_t)x), py = ty((int16_t)y);
      switch (datum) {
        case TC_DATUM: px -= (int32_t)bw / 2; break;
        case TR_DATUM: px -= (int32_t)bw; break;
        case ML_DATUM: py -= (int32_t)bh / 2; break;
        case MC_DATUM: px -= (int32_t)bw / 2; py -= (int32_t)bh / 2; break;
        case MR_DATUM: px -= (int32_t)bw; py -= (int32_t)bh / 2; break;
        default: break;
      }
      px -= bx; py -= by;
      gfx->setCursor(px, py); gfx->print(s);
      return bw > 0 ? (int16_t)bw : (int16_t)bh;
    }

    bool pushImage(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t *pixels) {
      if (!ready() || !pixels || w <= 0 || h <= 0) return false;
      gfx->draw16bitRGBBitmap(tx(x), ty(y), const_cast<uint16_t*>(pixels), w, h);
      return true;
    }

    bool pushLvglArea(int32_t x, int32_t y, int32_t w, int32_t h, const uint16_t *pixels) {
      if (!ready() || !pixels || w <= 0 || h <= 0) return false;
      gfx->draw16bitRGBBitmap(tx((int16_t)x), ty((int16_t)y), const_cast<uint16_t*>(pixels), w, h);
      return true;
    }
  };

  class TouchCompat {
  public:
    static const uint8_t TOUCH_ADDR = 0x38;
    static const uint8_t REG_TD_STATUS = 0x02;
    static const uint8_t REG_P1_XH     = 0x03;
    static const uint8_t REG_CHIP_ID   = 0xA3;
    static const uint8_t REG_VENDOR_ID = 0xA8;
    static const int16_t LOGICAL_W = TFT_WIDTH;
    static const int16_t LOGICAL_H = TFT_HEIGHT;

    bool initOk = false;
    uint8_t rotation = DISPLAY_ROTATION_NORMAL;
    TS_Point lastPoint;
    bool lastTouched = false;
    uint32_t lastPollMs = 0;

    void begin(SPIClass&) {
      #if !WS28_ENABLE_TOUCH
        initOk = false;
        lastTouched = false;
        lastPoint = TS_Point();
        Serial.println("[INFO][TOUCH] FT6336G touch probe disabled.");
        return;
      #endif

      lcdwikiTouchWire.begin(TOUCH_SDA, TOUCH_SCL, 400000);
      Wire.setClock(400000);
      lcdwikiTouchWire.setTimeOut(100);
      Serial.printf("[INFO][TOUCH] FT6336G init on I2C1 SDA=%d SCL=%d RST=%d IRQ=%d ADDR=0x%02X.\n",
                    TOUCH_SDA, TOUCH_SCL, TOUCH_RST, TOUCH_IRQ, TOUCH_ADDR);
      Serial.println("[INFO][TOUCH] The same I2C bus may also show ES8311 audio codec at 0x18.");

      if (TOUCH_IRQ >= 0) pinMode(TOUCH_IRQ, INPUT_PULLUP);
      if (TOUCH_RST >= 0) {
        pinMode(TOUCH_RST, OUTPUT);
        digitalWrite(TOUCH_RST, HIGH); delay(20);
        digitalWrite(TOUCH_RST, LOW);  delay(20);
        digitalWrite(TOUCH_RST, HIGH); delay(200);
      } else {
        delay(200);
      }

      initOk = probeController();
      if (initOk) {
        uint8_t chip = 0, vendor = 0;
        bool chipOk = read8(REG_CHIP_ID, chip);
        bool vendorOk = read8(REG_VENDOR_ID, vendor);
        Serial.print("[INFO][TOUCH] FT6336G touch controller detected");
        if (chipOk) { Serial.printf(", CHIP_ID=0x%02X", chip); }
        if (vendorOk) { Serial.printf(", VENDOR_ID=0x%02X", vendor); }
        Serial.println(".");
      } else {
        Serial.println("WARNING: FT6336G touch controller not detected on ESP32-S3 2.8 backend.");
        #if WS28_TOUCH_I2C_SCAN
          scanI2CBus();
        #endif
      }
      lastTouched = false;
    }

    void setRotation(uint8_t r) { rotation = r; }
    bool touched() { poll(); return lastTouched; }
    TS_Point getPoint() const { return lastPoint; }

  private:
    void scanI2CBus() {
      Serial.printf("[INFO][TOUCH] I2C scan on I2C1 bus SDA=%d SCL=%d...\n", TOUCH_SDA, TOUCH_SCL);
      bool found = false;
      for (uint8_t addr = 1; addr < 127; ++addr) {
        lcdwikiTouchWire.beginTransmission(addr);
        uint8_t err = lcdwikiTouchWire.endTransmission();
        if (err == 0) {
          found = true;
          Serial.printf("[INFO][TOUCH] I2C device found at 0x%02X", addr);
          if (addr == TOUCH_ADDR) Serial.print(" (expected FT6336G touch)");
          if (addr == 0x18) Serial.print(" (ES8311 audio codec on same bus)");
          Serial.println();
          delay(2);
        }
      }
      if (!found) {
        Serial.println("[INFO][TOUCH] I2C scan found no devices on the configured FT6336G touch bus.");
        Serial.println("[INFO][TOUCH] Check board model ES3C28P/E32C28P vs ES3N28P/E32N28P: N variants have no touch screen.");
      }
    }

    bool probeController() {
      lcdwikiTouchWire.beginTransmission(TOUCH_ADDR);
      return lcdwikiTouchWire.endTransmission(true) == 0;
    }

    bool readBytes(uint8_t reg, uint8_t *buf, size_t len) {
      if (!buf || !len) return false;
      lcdwikiTouchWire.beginTransmission(TOUCH_ADDR);
      lcdwikiTouchWire.write(reg);
      if (lcdwikiTouchWire.endTransmission(false) != 0) return false;
      if (lcdwikiTouchWire.requestFrom((int)TOUCH_ADDR, (int)len) != (int)len) return false;
      for (size_t i = 0; i < len; ++i) buf[i] = lcdwikiTouchWire.read();
      return true;
    }

    bool read8(uint8_t reg, uint8_t &val) {
      return readBytes(reg, &val, 1);
    }

    void poll() {
      if (!initOk) { lastTouched = false; lastPoint = TS_Point(); return; }
      if ((millis() - lastPollMs) < 12) return;
      lastPollMs = millis();

      uint8_t data[5] = {0};
      if (!readBytes(REG_TD_STATUS, data, sizeof(data))) {
        lastTouched = false;
        lastPoint = TS_Point();
        return;
      }

      uint8_t points = data[0] & 0x0F;
      if (points == 0 || points > 2) {
        lastTouched = false;
        lastPoint = TS_Point();
        return;
      }

      uint16_t rawX = ((uint16_t)(data[1] & 0x0F) << 8) | data[2];
      uint16_t rawY = ((uint16_t)(data[3] & 0x0F) << 8) | data[4];
      static uint8_t touchDebugPrints = 0;
      if (touchDebugPrints < 12) {
        Serial.printf("[INFO][TOUCH] raw point #%u: points=%u rawX=%u rawY=%u.\n",
                      (unsigned)touchDebugPrints + 1, (unsigned)points, (unsigned)rawX, (unsigned)rawY);
        touchDebugPrints++;
      }
      rawX = constrain((int)rawX, 0, TFT_NATIVE_WIDTH - 1);
      rawY = constrain((int)rawY, 0, TFT_NATIVE_HEIGHT - 1);

      int16_t x = 0, y = 0;
      if (rotation == DISPLAY_ROTATION_FLIPPED) {
        x = (int16_t)((LOGICAL_W - 1) - constrain((int)rawY, 0, LOGICAL_W - 1));
        y = (int16_t)constrain((int)rawX, 0, LOGICAL_H - 1);
      } else {
        x = (int16_t)constrain((int)rawY, 0, LOGICAL_W - 1);
        y = (int16_t)((LOGICAL_H - 1) - constrain((int)rawX, 0, LOGICAL_H - 1));
      }

      lastPoint = TS_Point(x, y, points);
      lastTouched = true;
    }
  };

  extern DisplayCompat tft;

  static void ws28InitLvglBridge() {}
  static void ws28HandleLvgl() {}

#elif DISPLAY_BACKEND_IS_WS_RGB_800_480
  #include <Arduino_GFX_Library.h>
  #include <Wire.h>

  #if DISPLAY_BACKEND_IS_ELECROW_CROWPANEL_BASIC_7
    #define USER_SETUP_INFO "ELECROW CrowPanel 7.0 Basic"
    #define DISPLAY_CONTROLLER_NAME "EK9716BD3/EK73002 RGB / GT911"
  #elif DISPLAY_BACKEND_IS_ELECROW_CROWPANEL_ADVANCE_7
    #define USER_SETUP_INFO "ELECROW CrowPanel 7.0 Advance"
    #define DISPLAY_CONTROLLER_NAME "SC7277 RGB / GT911"
  #elif DISPLAY_BACKEND_IS_EC8048S070CT
    #define USER_SETUP_INFO "EC8048S070CT / ESP32-S3-Touch-LCD-7"
    #define DISPLAY_CONTROLLER_NAME "ST7262 RGB / GT911"
  #elif DISPLAY_BACKEND == DISPLAY_BACKEND_WS8048S070C
    #define USER_SETUP_INFO "ESP32-S3-Touch-LCD-7"
    #define DISPLAY_CONTROLLER_NAME "ST7262 RGB / GT911"
  #else
    #define USER_SETUP_INFO "WS8048S043C"
    #define DISPLAY_CONTROLLER_NAME "ST7262 RGB / GT911"
  #endif

  #define TFT_WIDTH  800
  #define TFT_HEIGHT 480

  #define DISPLAY_ROTATION_NORMAL  0
  #define DISPLAY_ROTATION_FLIPPED 2

  #if DISPLAY_BACKEND_IS_ELECROW_CROWPANEL_BASIC_7
    #define TFT_BL 2
    #define TFT_BACKLIGHT_ON HIGH

    #define WS_LCD_DE    41
    #define WS_LCD_VSYNC 40
    #define WS_LCD_HSYNC 39
    #define WS_LCD_PCLK   0

    #define WS_LCD_B0 15
    #define WS_LCD_B1  7
    #define WS_LCD_B2  6
    #define WS_LCD_B3  5
    #define WS_LCD_B4  4
    #define WS_LCD_G0  9
    #define WS_LCD_G1 46
    #define WS_LCD_G2  3
    #define WS_LCD_G3  8
    #define WS_LCD_G4 16
    #define WS_LCD_G5  1
    #define WS_LCD_R0 14
    #define WS_LCD_R1 21
    #define WS_LCD_R2 47
    #define WS_LCD_R3 48
    #define WS_LCD_R4 45

    #define WS_LCD_PCLK_HZ 14000000
    #define WS_LCD_HSYNC_POLARITY   0
    #define WS_LCD_HSYNC_FRONT      40
    #define WS_LCD_HSYNC_PULSE      48
    #define WS_LCD_HSYNC_BACK       40
    #define WS_LCD_VSYNC_POLARITY   0
    #define WS_LCD_VSYNC_FRONT      1
    #define WS_LCD_VSYNC_PULSE      31
    #define WS_LCD_VSYNC_BACK       13
    #define WS_LCD_PCLK_ACTIVE_NEG  1

    #define TOUCH_IRQ   -1
    #define TOUCH_RST   38
    #define TOUCH_SDA   19
    #define TOUCH_SCL   20
    #define TOUCH_MOSI  TOUCH_SDA
    #define TOUCH_MISO  -1
    #define TOUCH_CLK   TOUCH_SCL
    #define TOUCH_CS    -1

    #define SD_CS       10
    #define SD_MISO     13
    #define SD_MOSI     11
    #define SD_SCLK     12

  #elif DISPLAY_BACKEND_IS_ELECROW_CROWPANEL_ADVANCE_7
    #define TFT_BL -1
    #define TFT_BACKLIGHT_ON HIGH

    #define WS_LCD_DE    42
    #define WS_LCD_VSYNC 41
    #define WS_LCD_HSYNC 40
    #define WS_LCD_PCLK  39

    #define WS_LCD_B0 21
    #define WS_LCD_B1 47
    #define WS_LCD_B2 48
    #define WS_LCD_B3 45
    #define WS_LCD_B4 38
    #define WS_LCD_G0  9
    #define WS_LCD_G1 10
    #define WS_LCD_G2 11
    #define WS_LCD_G3 12
    #define WS_LCD_G4 13
    #define WS_LCD_G5 14
    #define WS_LCD_R0  7
    #define WS_LCD_R1 17
    #define WS_LCD_R2 18
    #define WS_LCD_R3  3
    #define WS_LCD_R4 46

    #define WS_LCD_PCLK_HZ 18000000
    #define WS_LCD_HSYNC_POLARITY   1
    #define WS_LCD_HSYNC_FRONT      8
    #define WS_LCD_HSYNC_PULSE      4
    #define WS_LCD_HSYNC_BACK       8
    #define WS_LCD_VSYNC_POLARITY   1
    #define WS_LCD_VSYNC_FRONT      8
    #define WS_LCD_VSYNC_PULSE      4
    #define WS_LCD_VSYNC_BACK       8
    #define WS_LCD_PCLK_ACTIVE_NEG  0

    #define TOUCH_IRQ   -1
    #define TOUCH_SDA   15
    #define TOUCH_SCL   16
    #define TOUCH_MOSI  TOUCH_SDA
    #define TOUCH_MISO  -1
    #define TOUCH_CLK   TOUCH_SCL
    #define TOUCH_CS    -1

    #define SD_CS        7
    #define SD_MISO      4
    #define SD_MOSI      6
    #define SD_SCLK      5

  #else
    #define TFT_BL   -1
    #define TFT_BACKLIGHT_ON HIGH

    #define WS_LCD_DE     5
    #define WS_LCD_VSYNC  3
    #define WS_LCD_HSYNC 46
    #define WS_LCD_PCLK   7

    #define WS_LCD_B0 14
    #define WS_LCD_B1 38
    #define WS_LCD_B2 18
    #define WS_LCD_B3 17
    #define WS_LCD_B4 10
    #define WS_LCD_G0 39
    #define WS_LCD_G1  0
    #define WS_LCD_G2 45
    #define WS_LCD_G3 48
    #define WS_LCD_G4 47
    #define WS_LCD_G5 21
    #define WS_LCD_R0  1
    #define WS_LCD_R1  2
    #define WS_LCD_R2 42
    #define WS_LCD_R3 41
    #define WS_LCD_R4 40

    #define WS_LCD_PCLK_HZ 16000000
    #define WS_LCD_HSYNC_POLARITY   0
    #define WS_LCD_HSYNC_FRONT      8
    #define WS_LCD_HSYNC_PULSE      4
    #define WS_LCD_HSYNC_BACK       8
    #define WS_LCD_VSYNC_POLARITY   0
    #define WS_LCD_VSYNC_FRONT      8
    #define WS_LCD_VSYNC_PULSE      4
    #define WS_LCD_VSYNC_BACK       8
    #define WS_LCD_PCLK_ACTIVE_NEG  1

    #define TOUCH_IRQ    4
    #define TOUCH_SDA    8
    #define TOUCH_SCL    9
    #define TOUCH_MOSI   TOUCH_SDA
    #define TOUCH_MISO  -1
    #define TOUCH_CLK    TOUCH_SCL
    #define TOUCH_CS    -1

    #define SD_CS        -1
    #define SD_MISO      13
    #define SD_MOSI      11
    #define SD_SCLK      12
  #endif

  #define WS_RGB_BOUNCE_PIXELS    (TFT_WIDTH * 10)

  #if DISPLAY_BACKEND_IS_ELECROW_CROWPANEL_ADVANCE_7
    #define ELECROW_BACKLIGHT_I2C_ADDR 0x30
    #ifndef ELECROW_BACKLIGHT_V12
      #define ELECROW_BACKLIGHT_V12 0
    #endif
    #define WS_IO_TOUCH_RESET 0
    #define WS_IO_BACKLIGHT   0
    #define WS_IO_LCD_RESET   0
    #define WS_IO_SD_CS       0
    #define WS_IO_USB_SEL     0
  #elif DISPLAY_BACKEND_IS_ELECROW_CROWPANEL_BASIC_7
    #define WS_IO_TOUCH_RESET TOUCH_RST
    #define WS_IO_BACKLIGHT   TFT_BL
    #define WS_IO_LCD_RESET   -1
    #define WS_IO_SD_CS       SD_CS
    #define WS_IO_USB_SEL     -1
  #else
    #define WS_IO_EXTENSION_WR_SET_ADDR 0x24
    #define WS_IO_EXTENSION_WR_OC_ADDR  0x23
    #define WS_IO_EXTENSION_WR_IO_ADDR  0x38
    #define WS_IO_EXTENSION_RD_IO_ADDR  0x26
    #define WS_IO_EXTENSION_WR_SET_IO_OUTPUT 0x01
    #define WS_IO_EXTENSION_WR_OC_DEFAULT    0x0F
    #define WS_IO_EXTENSION_WR_IO_DEFAULT    0xFF

    #define WS_IO_TOUCH_RESET           1
    #define WS_IO_BACKLIGHT             2
    #define WS_IO_LCD_RESET             3
    #define WS_IO_SD_CS                 4
    #define WS_IO_USB_SEL               5
  #endif

  #ifndef WS8048_USE_SD_MMC
    #define WS8048_USE_SD_MMC 0
  #endif

  #if !WS8048_USE_SD_MMC
    #define WS8048_SD_SPI_CS SD_CS
  #endif

  static inline uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
  }

  #ifndef TFT_BLACK
    #define TFT_BLACK     color565(0, 0, 0)
    #define TFT_WHITE     color565(255, 255, 255)
    #define TFT_RED       color565(255, 0, 0)
    #define TFT_GREEN     color565(0, 255, 0)
    #define TFT_BLUE      color565(0, 0, 255)
    #define TFT_YELLOW    color565(255, 255, 0)
    #define TFT_CYAN      color565(0, 255, 255)
    #define TFT_DARKGREY  color565(64, 64, 64)
    #define TFT_LIGHTGREY color565(192, 192, 192)
  #endif

  #ifndef TL_DATUM
    #define TL_DATUM 0
    #define TC_DATUM 1
    #define TR_DATUM 2
    #define ML_DATUM 3
    #define MC_DATUM 4
    #define MR_DATUM 5
  #endif

  struct TS_Point {
    int16_t x;
    int16_t y;
    int16_t z;
    TS_Point(int16_t px = 0, int16_t py = 0, int16_t pz = 0) : x(px), y(py), z(pz) {}
  };

  static bool ws8048_i2c_ready = false;
  static bool ws8048_io_ready = false;
  static uint8_t ws8048_io_output = 0xFF;

  #ifndef TOUCH_SDA
    #define TOUCH_SDA TOUCH_MOSI
  #endif
  #ifndef TOUCH_SCL
    #define TOUCH_SCL TOUCH_CLK
  #endif
  // Some RGB-panel backends reset the GT911 touch controller through
  // the WS IO expander instead of a direct ESP32 GPIO.  Keep a safe
  // default so common touch logging / compatibility code can compile.
  #ifndef TOUCH_RST
    #define TOUCH_RST -1
  #endif

  static void ws8048EnsureI2C() {
    if (ws8048_i2c_ready) return;
    Wire.begin(TOUCH_SDA, TOUCH_SCL);
    Wire.setClock(100000);
    Wire.setTimeOut(100);
    ws8048_i2c_ready = true;
  }

  static bool ws8048I2cWriteBytes(uint8_t addr, const uint8_t *data, size_t len) {
    ws8048EnsureI2C();
    Wire.beginTransmission(addr);
    Wire.write(data, len);
    return Wire.endTransmission() == 0;
  }

  static bool ws8048I2cWriteByte(uint8_t addr, uint8_t value) {
    return ws8048I2cWriteBytes(addr, &value, 1);
  }

  #if DISPLAY_BACKEND_IS_ELECROW_CROWPANEL_BASIC_7
    static uint8_t ws8048_pwm_level = 0;

    static void ws8048AttachBacklightPwm() {
      static bool pwm_ready = false;
      if (pwm_ready) return;
      pinMode(TFT_BL, OUTPUT);
      #ifndef ESP_ARDUINO_VERSION_MAJOR
        #define ESP_ARDUINO_VERSION_MAJOR 2
      #endif
      #if ESP_ARDUINO_VERSION_MAJOR >= 3
        ledcAttach(TFT_BL, 300, 8);
      #else
        ledcSetup(0, 300, 8);
        ledcAttachPin(TFT_BL, 0);
      #endif
      pwm_ready = true;
    }

    static void ws8048SetBacklightPwm(uint8_t brightness) {
      ws8048AttachBacklightPwm();
      ws8048_pwm_level = brightness;
      #if ESP_ARDUINO_VERSION_MAJOR >= 3
        ledcWrite(TFT_BL, brightness);
      #else
        ledcWrite(0, brightness);
      #endif
    }

    static void ws8048IoInit() {
      if (ws8048_io_ready) return;
      ws8048AttachBacklightPwm();
      ws8048SetBacklightPwm(0);

      pinMode(TOUCH_RST, OUTPUT);
      digitalWrite(TOUCH_RST, LOW);
      delay(20);
      digitalWrite(TOUCH_RST, HIGH);
      delay(80);

      if (SD_CS >= 0) {
        pinMode(SD_CS, OUTPUT);
        digitalWrite(SD_CS, HIGH);
      }
      ws8048_io_ready = true;
    }

    static void ws8048IoOutput(uint8_t pin, bool value) {
      if ((int8_t)pin < 0) return;
      ws8048IoInit();
      pinMode(pin, OUTPUT);
      digitalWrite(pin, value ? HIGH : LOW);
    }

    static void ws8048Backlight(uint8_t brightness) {
      ws8048SetBacklightPwm(brightness);
    }

    static void ws8048SdCardEnable(bool enabled) {
      if (SD_CS >= 0) {
        pinMode(SD_CS, OUTPUT);
        digitalWrite(SD_CS, enabled ? LOW : HIGH);
        delay(2);
      }
    }

  #elif DISPLAY_BACKEND_IS_ELECROW_CROWPANEL_ADVANCE_7
    static void ws8048IoInit() {
      if (ws8048_io_ready) return;
      ws8048EnsureI2C();
      ws8048_io_ready = true;
    }

    static void ws8048IoOutput(uint8_t pin, bool value) {
      (void)pin;
      (void)value;
      ws8048IoInit();
    }

    static void ws8048Backlight(uint8_t brightness) {
      ws8048EnsureI2C();

      #if ELECROW_BACKLIGHT_V12
        if (brightness == 0) {
          ws8048I2cWriteByte(ELECROW_BACKLIGHT_I2C_ADDR, 0x05);
          return;
        }
        uint8_t level = (uint8_t)(0x06 + ((uint16_t)(brightness - 1) * (0x10 - 0x06) / 254U));
        ws8048I2cWriteByte(ELECROW_BACKLIGHT_I2C_ADDR, 0x19);
        delay(2);
        ws8048I2cWriteByte(ELECROW_BACKLIGHT_I2C_ADDR, 0x10);
        delay(2);
        ws8048I2cWriteByte(ELECROW_BACKLIGHT_I2C_ADDR, level);
      #else
        uint8_t level = (brightness == 0)
          ? 245
          : (uint8_t)(244U - ((uint16_t)(brightness - 1) * 244U / 254U));
        ws8048I2cWriteByte(ELECROW_BACKLIGHT_I2C_ADDR, 0x19);
        delay(2);
        ws8048I2cWriteByte(ELECROW_BACKLIGHT_I2C_ADDR, 0x10);
        delay(2);
        ws8048I2cWriteByte(ELECROW_BACKLIGHT_I2C_ADDR, 0x18);
        delay(2);
        ws8048I2cWriteByte(ELECROW_BACKLIGHT_I2C_ADDR, level);
      #endif
    }

    static void ws8048SdCardEnable(bool enabled) {
      (void)enabled;
    }

  #else
    static bool ws8048Ch422WriteSet(uint8_t value) {
      return ws8048I2cWriteByte(WS_IO_EXTENSION_WR_SET_ADDR, value);
    }

    static bool ws8048Ch422WriteOc(uint8_t value) {
      return ws8048I2cWriteByte(WS_IO_EXTENSION_WR_OC_ADDR, value & 0x0F);
    }

    static bool ws8048Ch422WriteIo(uint8_t value) {
      return ws8048I2cWriteByte(WS_IO_EXTENSION_WR_IO_ADDR, value);
    }

    static void ws8048IoFlush() {
      ws8048Ch422WriteIo(ws8048_io_output);
    }

    static void ws8048IoInit() {
      if (ws8048_io_ready) return;
      ws8048EnsureI2C();

      ws8048_io_output = WS_IO_EXTENSION_WR_IO_DEFAULT;
      bool ok = true;
      ok &= ws8048Ch422WriteSet(WS_IO_EXTENSION_WR_SET_IO_OUTPUT);
      ok &= ws8048Ch422WriteOc(WS_IO_EXTENSION_WR_OC_DEFAULT);
      ok &= ws8048Ch422WriteIo(ws8048_io_output);
      ws8048_io_ready = ok;

      if (!ok) {
        Serial.println("WARNING: CH422G I/O expander init failed; RGB LCD backlight/reset may stay off.");
      }
    }

    static void ws8048IoOutput(uint8_t pin, bool value) {
      ws8048IoInit();
      if (value) ws8048_io_output |= (1 << pin);
      else ws8048_io_output &= ~(1 << pin);
      ws8048IoFlush();
    }

    static void ws8048Backlight(uint8_t brightness) {
      ws8048IoOutput(WS_IO_BACKLIGHT, brightness > 0);
    }

    static void ws8048SdCardEnable(bool enabled) {
      ws8048IoOutput(WS_IO_SD_CS, !enabled);
      delay(2);
    }
  #endif

  class DisplayCompat {
  public:
    static const int16_t PHYS_W = 800;
    static const int16_t PHYS_H = 480;
    static const int16_t LOGICAL_W = PHYS_W;
    static const int16_t LOGICAL_H = PHYS_H;

    Arduino_ESP32RGBPanel *rgbpanel = nullptr;
    Arduino_RGB_Display *gfx = nullptr;
    uint8_t datum = TL_DATUM;
    uint8_t rotation = DISPLAY_ROTATION_NORMAL;
    uint8_t textSize = 1;
    uint16_t fg = TFT_WHITE;
    uint16_t bg = TFT_BLACK;
    bool initOk = false;

    float scale() const { return 1.0f; }
    int16_t tx(int16_t x) const { return x; }
    int16_t ty(int16_t y) const { return y; }
    int16_t tw(int16_t w) const { return (w < 1) ? 1 : w; }
    int16_t th(int16_t h) const { return (h < 1) ? 1 : h; }
    int16_t tr(int16_t r) const { return (r < 1) ? 1 : r; }

    void init() {
      ws8048IoInit();
      ws8048IoOutput(WS_IO_TOUCH_RESET, true);
      ws8048IoOutput(WS_IO_LCD_RESET, true);
      ws8048Backlight(255);
      delay(20);

      rgbpanel = new Arduino_ESP32RGBPanel(
        WS_LCD_DE, WS_LCD_VSYNC, WS_LCD_HSYNC, WS_LCD_PCLK,
        WS_LCD_R0, WS_LCD_R1, WS_LCD_R2, WS_LCD_R3, WS_LCD_R4,
        WS_LCD_G0, WS_LCD_G1, WS_LCD_G2, WS_LCD_G3, WS_LCD_G4, WS_LCD_G5,
        WS_LCD_B0, WS_LCD_B1, WS_LCD_B2, WS_LCD_B3, WS_LCD_B4,
        WS_LCD_HSYNC_POLARITY, WS_LCD_HSYNC_FRONT, WS_LCD_HSYNC_PULSE, WS_LCD_HSYNC_BACK,
        WS_LCD_VSYNC_POLARITY, WS_LCD_VSYNC_FRONT, WS_LCD_VSYNC_PULSE, WS_LCD_VSYNC_BACK,
        WS_LCD_PCLK_ACTIVE_NEG, WS_LCD_PCLK_HZ, false, 0, 0, WS_RGB_BOUNCE_PIXELS
      );
      if (!rgbpanel) { initOk = false; return; }

      gfx = new Arduino_RGB_Display(TFT_WIDTH, TFT_HEIGHT, rgbpanel, rotation, false);
      if (!gfx) { initOk = false; return; }

      initOk = gfx->begin();
      if (!initOk) {
        Serial.println("WARNING: RGB display begin() failed for RGB 800x480 backend.");
        return;
      }

      gfx->invertDisplay(WS_RGB_LCD_INVERT_COLORS);
      Serial.printf("[INFO][LCD] ILI9341V invertDisplay(%d). If colours are negative, set LCD_INVERT_COLORS to 0.\n", (int)WS_RGB_LCD_INVERT_COLORS);
      gfx->setRotation(rotation);
      gfx->setTextWrap(false);
      gfx->setTextSize(textSize);
      gfx->setTextColor(fg, bg);
      gfx->fillScreen(TFT_BLACK);
      gfx->flush();
    }

    bool ready() const { return initOk && gfx != nullptr; }

    void setRotation(uint8_t r) {
      rotation = r;
      if (ready()) gfx->setRotation(rotation);
      flush();
    }

    int16_t width() const { return LOGICAL_W; }
    int16_t height() const { return LOGICAL_H; }

    void flush() { if (ready()) gfx->flush(); }

    void fillScreen(uint16_t c) { if (ready()) gfx->fillScreen(c); }
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t c) {
      if (ready()) gfx->fillRect(tx(x), ty(y), tw(w), th(h), c);
    }
    void drawPixel(int16_t x, int16_t y, uint16_t c) { if (ready()) gfx->drawPixel(tx(x), ty(y), c); }
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t c) {
      if (ready()) gfx->drawLine(tx(x0), ty(y0), tx(x1), ty(y1), c);
    }
    void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t c) {
      if (ready()) gfx->drawFastHLine(tx(x), ty(y), tw(w), c);
    }
    void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t c) {
      if (ready()) gfx->drawFastVLine(tx(x), ty(y), th(h), c);
    }
    void drawCircle(int16_t x, int16_t y, int16_t r, uint16_t c) {
      if (ready()) gfx->drawCircle(tx(x), ty(y), tr(r), c);
    }
    void fillCircle(int16_t x, int16_t y, int16_t r, uint16_t c) {
      if (ready()) gfx->fillCircle(tx(x), ty(y), tr(r), c);
    }
    void drawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t c) {
      if (ready()) gfx->drawRoundRect(tx(x), ty(y), tw(w), th(h), tr(r), c);
    }
    void fillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint16_t c) {
      if (ready()) gfx->fillRoundRect(tx(x), ty(y), tw(w), th(h), tr(r), c);
    }

    void setTextColor(uint16_t fgColor, uint16_t bgColor = TFT_BLACK) {
      fg = fgColor;
      bg = bgColor;
      if (ready()) gfx->setTextColor(fg, bg);
    }

    void setTextDatum(uint8_t d) { datum = d; }

    void setTextFont(uint8_t font) {
      if (font >= 8) textSize = 10;
      else if (font >= 7) textSize = 8;
      else if (font >= 6) textSize = 6;
      else if (font >= 4) textSize = 4;
      else if (font >= 2) textSize = 2;
      else textSize = 1;

      if (gfx) {
        gfx->setFont(nullptr);
        gfx->setTextSize(textSize);
      }
    }

    int16_t textWidth(const String& s) {
      if (!gfx) return 0;

      gfx->setFont(nullptr);
      gfx->setTextSize(textSize);
      gfx->setTextWrap(false);

      int16_t bx = 0;
      int16_t by = 0;
      uint16_t bw = 0;
      uint16_t bh = 0;
      gfx->getTextBounds(s, 0, 0, &bx, &by, &bw, &bh);

      return (int16_t)bw;
    }

    int16_t drawString(const String& s, int32_t x, int32_t y) {
      if (!gfx) return 0;

      gfx->setFont(nullptr);
      gfx->setTextSize(textSize);
      gfx->setTextWrap(false);

      int16_t bx = 0;
      int16_t by = 0;
      uint16_t bw = 0;
      uint16_t bh = 0;
      gfx->getTextBounds(s, 0, 0, &bx, &by, &bw, &bh);

      int32_t px = tx((int16_t)x);
      int32_t py = ty((int16_t)y);

      switch (datum) {
        case TC_DATUM:
          px -= (int32_t)bw / 2;
          break;
        case TR_DATUM:
          px -= (int32_t)bw;
          break;
        case ML_DATUM:
          py -= (int32_t)bh / 2;
          break;
        case MC_DATUM:
          px -= (int32_t)bw / 2;
          py -= (int32_t)bh / 2;
          break;
        case MR_DATUM:
          px -= (int32_t)bw;
          py -= (int32_t)bh / 2;
          break;
        default:
          break;
      }

      px -= bx;
      py -= by;

      gfx->setCursor(px, py);
      gfx->print(s);
      return bw > 0 ? (int16_t)bw : (int16_t)bh;
    }

    bool pushImage(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t *pixels) {
      if (!ready() || !pixels || w <= 0 || h <= 0) return false;
      gfx->draw16bitRGBBitmap(tx(x), ty(y), const_cast<uint16_t*>(pixels), w, h);
      return true;
    }

    bool pushLvglArea(int32_t x, int32_t y, int32_t w, int32_t h, const uint16_t *pixels) {
      if (!ready() || !pixels || w <= 0 || h <= 0) return false;
      gfx->draw16bitRGBBitmap(tx((int16_t)x), ty((int16_t)y), const_cast<uint16_t*>(pixels), w, h);
      return true;
    }
  };

  class TouchCompat {
  public:
    static const uint8_t TOUCH_ADDR_PRIMARY = 0x5D;
    static const uint8_t TOUCH_ADDR_BACKUP  = 0x14;
    static const uint16_t REG_STATUS        = 0x814E;
    static const uint16_t REG_POINT1        = 0x8150;
    static const uint16_t REG_PRODUCT_ID    = 0x8140;
    static const int16_t PHYS_W = 800;
    static const int16_t PHYS_H = 480;

    bool initOk = false;
    uint8_t addr = TOUCH_ADDR_PRIMARY;
    uint8_t rotation = DISPLAY_ROTATION_NORMAL;
    TS_Point lastPoint;
    bool lastTouched = false;
    uint32_t lastPollMs = 0;

    void begin(SPIClass&) {
      ws8048EnsureI2C();
      if (TOUCH_IRQ >= 0) pinMode(TOUCH_IRQ, INPUT_PULLUP);
      delay(80);

      bool detected = false;
      if (probeAddress(TOUCH_ADDR_PRIMARY)) {
        addr = TOUCH_ADDR_PRIMARY;
        detected = true;
      } else if (probeAddress(TOUCH_ADDR_BACKUP)) {
        addr = TOUCH_ADDR_BACKUP;
        detected = true;
      }

      initOk = detected;
      if (initOk) {
        clearStatus();
        Serial.printf("[INFO][TOUCH] GT911 detected at 0x%02X on SDA=%d SCL=%d RST=%d.\n",
                      addr, TOUCH_SDA, TOUCH_SCL, TOUCH_RST);
      } else {
        Serial.printf("WARNING: GT911 not detected on SDA=%d SCL=%d. Tried 0x%02X and 0x%02X.\n",
                      TOUCH_SDA, TOUCH_SCL, TOUCH_ADDR_PRIMARY, TOUCH_ADDR_BACKUP);
      }
      lastTouched = false;
    }

    void setRotation(uint8_t r) { rotation = r; }

    bool touched() {
      poll();
      return lastTouched;
    }

    TS_Point getPoint() const { return lastPoint; }

  private:
    bool probeAddress(uint8_t candidate) {
      uint8_t buf[4] = {0};
      return readReg(candidate, REG_PRODUCT_ID, buf, sizeof(buf));
    }

    bool writeReg(uint8_t devAddr, uint16_t reg, uint8_t value) {
      uint8_t data[3] = {(uint8_t)(reg >> 8), (uint8_t)(reg & 0xFF), value};
      return ws8048I2cWriteBytes(devAddr, data, sizeof(data));
    }

    bool readReg(uint8_t devAddr, uint16_t reg, uint8_t *buf, size_t len) {
      if (!buf || !len) return false;
      ws8048EnsureI2C();
      Wire.beginTransmission(devAddr);
      Wire.write((uint8_t)(reg >> 8));
      Wire.write((uint8_t)(reg & 0xFF));
      if (Wire.endTransmission(false) != 0) return false;
      if (Wire.requestFrom((int)devAddr, (int)len) != (int)len) return false;
      for (size_t i = 0; i < len; ++i) buf[i] = Wire.read();
      return true;
    }

    void clearStatus() {
      writeReg(addr, REG_STATUS, 0x00);
    }

    void poll() {
      if (!initOk) {
        lastTouched = false;
        lastPoint = TS_Point();
        return;
      }

      if ((millis() - lastPollMs) < 12) return;
      lastPollMs = millis();

      uint8_t status = 0;
      if (!readReg(addr, REG_STATUS, &status, 1)) {
        lastTouched = false;
        lastPoint = TS_Point();
        return;
      }

      if ((status & 0x80) == 0) {
        lastTouched = false;
        lastPoint = TS_Point();
        return;
      }

      uint8_t touchCount = status & 0x0F;
      if (touchCount == 0 || touchCount > 5) {
        clearStatus();
        lastTouched = false;
        lastPoint = TS_Point();
        return;
      }

      uint8_t data[8] = {0};
      if (!readReg(addr, REG_POINT1, data, sizeof(data))) {
        clearStatus();
        lastTouched = false;
        lastPoint = TS_Point();
        return;
      }

      clearStatus();

      uint16_t rawX = ((uint16_t)data[1] << 8) | data[0];
      uint16_t rawY = ((uint16_t)data[3] << 8) | data[2];

      int16_t x = constrain((int)rawX, 0, PHYS_W - 1);
      int16_t y = constrain((int)rawY, 0, PHYS_H - 1);

      if (rotation == DISPLAY_ROTATION_FLIPPED) {
        x = (PHYS_W - 1) - x;
        y = (PHYS_H - 1) - y;
      }

      lastPoint = TS_Point(x, y, 1);
      lastTouched = true;
    }
  };

  extern DisplayCompat tft;

  static void ws8048InitLvglBridge() {}
  static void ws8048HandleLvgl() {}

#elif DISPLAY_BACKEND_IS_DESKTOP
  #define USER_SETUP_INFO "Desktop SDL2"
  #define DISPLAY_CONTROLLER_NAME "SDL2"
  #define TFT_WIDTH  800
  #define TFT_HEIGHT 480
  #define DISPLAY_ROTATION_NORMAL  0
  #define DISPLAY_ROTATION_FLIPPED 2
#else
  #error "Unsupported DISPLAY_BACKEND value"
#endif

#ifndef USE_DESKTOP
  #include <FS.h>
#endif
using fs::FS;
using fs::File;
#ifndef USE_DESKTOP
  #include <SD.h>
  #if (DISPLAY_BACKEND_IS_WS_RGB_800_480 && WS8048_USE_SD_MMC) || (DISPLAY_BACKEND_IS_WS_TOUCH_LCD_2_8 && WS28_USE_SD_MMC)
    #include <SD_MMC.h>
  #endif
  #include <SPI.h>
  #include <PNGdec.h>
  #include <time.h>
  #include <WebServer.h>
  #include <Update.h>
  #include <esp_system.h>
  #include <esp_heap_caps.h>
  #include <rom/rtc.h>
  #include "freertos/FreeRTOS.h"
  #include "freertos/semphr.h"
#endif

#if DISPLAY_BACKEND == DISPLAY_BACKEND_2432S028R
TFT_eSPI tft = TFT_eSPI();
XPT2046_Touchscreen ts(TOUCH_CS, TOUCH_IRQ);
#else
DisplayCompat tft;
TouchCompat ts;
#endif

#if DISPLAY_BACKEND == DISPLAY_BACKEND_2432S028R
  #if defined(ESP32) && !defined(CONFIG_IDF_TARGET_ESP32S3)
    SPIClass touchSPI(VSPI);
  #else
    SPIClass touchSPI(FSPI);
  #endif
#else
  SPIClass touchSPI(FSPI);
#endif
SPIClass sdSPI(HSPI);
WebServer server(80);

#ifndef NETWORK_LOCK_TIMEOUT_MS
  #define NETWORK_LOCK_TIMEOUT_MS 30000UL
#endif

SemaphoreHandle_t networkMutex = nullptr;

class NetworkGuard {
public:
  bool locked = false;

  explicit NetworkGuard(uint32_t timeoutMs = NETWORK_LOCK_TIMEOUT_MS) {
    if (!networkMutex) {
      locked = true;
      return;
    }
    locked = (xSemaphoreTakeRecursive(networkMutex, pdMS_TO_TICKS(timeoutMs)) == pdTRUE);
  }

  ~NetworkGuard() {
    if (locked && networkMutex) xSemaphoreGiveRecursive(networkMutex);
  }

  operator bool() const { return locked; }
};

static void handleWebClientSafely(uint32_t timeoutMs = 0) {
  NetworkGuard net(timeoutMs);
  if (net) server.handleClient();
}

PNG pngDecoder;
File pngFile;
uint16_t* pngLineBuffer = nullptr;
size_t pngLineBufferPixels = 0;
uint8_t* pngMaskBuffer = nullptr;
size_t pngMaskBufferBytes = 0;
int16_t pngDrawX = 0;
int16_t pngDrawY = 0;
int16_t pngMaxW = 0;
int16_t pngMaxH = 0;
bool pngDecodeOk = false;
uint16_t pngBlendBg565 = 0;
uint16_t pngBlendBg565Buffer = 0;
uint8_t pngBlendBgR = 0;
uint8_t pngBlendBgG = 0;
uint8_t pngBlendBgB = 0;
int pngPixelType = 0;
bool pngHasAlpha = false;

#ifndef PNG_PIXEL_TRUECOLOR_ALPHA
  #define PNG_PIXEL_TRUECOLOR_ALPHA 6
#endif

static inline void rgb565To888Components(uint16_t c, uint8_t &r, uint8_t &g, uint8_t &b) {
  r = (uint8_t)(((c >> 11) & 0x1F) * 255 / 31);
  g = (uint8_t)(((c >> 5) & 0x3F) * 255 / 63);
  b = (uint8_t)((c & 0x1F) * 255 / 31);
}

static inline uint16_t pngBufferColor565(uint16_t c) {
#if DISPLAY_BACKEND == DISPLAY_BACKEND_2432S028R
  return (uint16_t)((c << 8) | (c >> 8));
#else
  return c;
#endif
}

#ifndef CYD_TOUCH_MIN_X
  #define CYD_TOUCH_MIN_X 200
#endif
#ifndef CYD_TOUCH_MAX_X
  #define CYD_TOUCH_MAX_X 3700
#endif
#ifndef CYD_TOUCH_MIN_Y
  #define CYD_TOUCH_MIN_Y 240
#endif
#ifndef CYD_TOUCH_MAX_Y
  #define CYD_TOUCH_MAX_Y 3800
#endif

int touchMinX = CYD_TOUCH_MIN_X;
int touchMaxX = CYD_TOUCH_MAX_X;
int touchMinY = CYD_TOUCH_MIN_Y;
int touchMaxY = CYD_TOUCH_MAX_Y;

#if DISPLAY_BACKEND == DISPLAY_BACKEND_2432S028R
  const int SCREEN_W = DISPLAY_LOGICAL_WIDTH;
  const int SCREEN_H = DISPLAY_LOGICAL_HEIGHT;

  static const int UI_REF_W = 320;
  static const int UI_REF_H = 240;
#elif DISPLAY_BACKEND_IS_GFX_COMPAT
  const int SCREEN_W = TFT_WIDTH;
  const int SCREEN_H = TFT_HEIGHT;

  static const int UI_REF_W = 320;
  static const int UI_REF_H = 240;
#endif

#define UX(v) ((int)((((int32_t)(v)) * SCREEN_W + (UI_REF_W / 2)) / UI_REF_W))
#define UY(v) ((int)((((int32_t)(v)) * SCREEN_H + (UI_REF_H / 2)) / UI_REF_H))
#define UW(v) ((int)((((int32_t)(v)) * SCREEN_W + (UI_REF_W / 2)) / UI_REF_W))
#define UH(v) ((int)((((int32_t)(v)) * SCREEN_H + (UI_REF_H / 2)) / UI_REF_H))
#define UR(v) ((int)min(UW(v), UH(v)))

const char* CONFIG_FILE = "/config.json";
const char* IMPORT_CONFIG_TEMP_FILE = "/config-import.json";
const char* DEVICE_CALLSIGN = "13MAD86";

#ifndef AP_SETUP_TIMEOUT_MS
  #define AP_SETUP_TIMEOUT_MS 180000UL
#endif

#ifndef WIFI_CONNECT_TIMEOUT_MS
  #define WIFI_CONNECT_TIMEOUT_MS 15000UL
#endif

#ifndef MAX_WIFI_PROFILES
  #define MAX_WIFI_PROFILES 5
#endif

#ifndef WIFI_RETRY_INTERVAL_MS
  #define WIFI_RETRY_INTERVAL_MS 30000UL
#endif

#ifndef WIFI_RETRY_TIMEOUT_MS
  #define WIFI_RETRY_TIMEOUT_MS 3000UL
#endif

#ifndef WIFI_PROFILE_RETRY_COUNT_DEFAULT
  #define WIFI_PROFILE_RETRY_COUNT_DEFAULT 2
#endif

#ifndef WIFI_PROFILE_RETRY_COUNT_MIN
  #define WIFI_PROFILE_RETRY_COUNT_MIN 0
#endif

#ifndef WIFI_PROFILE_RETRY_COUNT_MAX
  #define WIFI_PROFILE_RETRY_COUNT_MAX 10
#endif

#ifndef HTTP_CONNECT_TIMEOUT_MS
  #define HTTP_CONNECT_TIMEOUT_MS 3000
#endif

#ifndef HTTP_READ_TIMEOUT_MS
  #define HTTP_READ_TIMEOUT_MS 3000
#endif

#ifndef ENABLE_TARGET_LIST_SD_LOOKUP
  #define ENABLE_TARGET_LIST_SD_LOOKUP 1
#endif

#ifndef ENABLE_TARGET_LIST_AUTO_DOWNLOAD
  #define ENABLE_TARGET_LIST_AUTO_DOWNLOAD 0
#endif

#ifndef TARGET_LIST_LOOKUP_CACHE_SIZE
  #if DISPLAY_BACKEND == DISPLAY_BACKEND_2432S028R
    #define TARGET_LIST_LOOKUP_CACHE_SIZE 16
  #else
    #define TARGET_LIST_LOOKUP_CACHE_SIZE 16
  #endif
#endif

#ifndef TARGET_LIST_DOWNLOAD_MIN_INTERVAL_MS
  #define TARGET_LIST_DOWNLOAD_MIN_INTERVAL_MS 10000UL
#endif

#ifndef TARGET_LIST_AUTO_RETRY_MS
  #define TARGET_LIST_AUTO_RETRY_MS (15UL * 60UL * 1000UL)
#endif

#ifndef TARGET_LIST_REFRESH_EVERY_BOOT
  #define TARGET_LIST_REFRESH_EVERY_BOOT 0
#endif

bool apModeActive = false;
String activeStorageLabel = "SD";
String apSsid = "";
String apPass = "12345678";

struct HeardRecord;
struct RadioIdLookupCacheEntry;
struct TargetNameLookupCacheEntry;

String normalizeHamCallsign(const String& input);
String trimSafe(const String& s);
String countryCodeFromCallsign(const String& cs);
String flagBmpPathFromCode(const String& code);
bool drawFlagForCallsign(int x, int y, int w, int h, const String& callsign);
bool drawBmp24FromSD(const char *filename, int16_t x, int16_t y, int16_t maxW, int16_t maxH);
bool drawPngFromSD(const char *filename, int16_t x, int16_t y, int16_t maxW, int16_t maxH, uint16_t bg565 = 0);
void *pngOpen(const char *filename, int32_t *size);
void pngClose(void *handle);
int32_t pngRead(PNGFILE *page, uint8_t *buffer, int32_t length);
int32_t pngSeek(PNGFILE *page, int32_t position);
int pngDraw(PNGDRAW *pDraw);
String csvFieldAt(const String& line, int wantedIndex);
String buildOperatorName(const String& firstName, const String& lastName);
String clipWithTilde(const String& input, size_t maxChars);
String formatCallsignName(const String& callsign, const String& name, size_t maxNameChars = 10, size_t maxCallsignChars = 10);
String cleanSvxlinkTargetValue(String value);
String stripTargetLabelPrefix(String value, const String& label);
String cleanTargetValueForMode(String value, const String& mode);
String lookupCommonTalkgroupName(const String& target);
String urlEncodeComponent(const String& value);
String buildRadioIdUsersUrl(const String& callsign);
bool radioIdLookupNeeded(const String& callsign, const String& name);
String radioIdLookupSignatureForRecord(const HeardRecord& r);
void queueRadioIdLookupForRecord(const HeardRecord& r);
void clearRadioIdLookupCache();
int radioIdLookupCacheIndexForCallsign(const String& normalizedCallsign);
struct RadioIdLookupCacheEntry* radioIdLookupCacheSlotForCallsign(const String& normalizedCallsign, bool createIfMissing);
String lookupPlainTextNameFromPayload(const String& payload, const String& normalizedCallsign);
bool populateRadioIdEntryFromJsonObject(JsonObject obj, struct RadioIdLookupCacheEntry& entry);
bool parseRadioIdUserResponse(const String& payload, struct RadioIdLookupCacheEntry& entry);
bool fetchRadioIdProfile(const String& normalizedCallsign, struct RadioIdLookupCacheEntry& outEntry);
void applyRadioIdCacheToRecord(struct HeardRecord& r, bool allowFetch = true);
String radioIdDisplayNameForRecord(const struct HeardRecord& r, bool allowFetch = true);
String radioIdDisplayCountryForRecord(const struct HeardRecord& r, bool allowFetch = true);
void applyRadioIdCacheToList(struct HeardRecord* list, int count);
bool applyRadioIdCacheToActiveData();
void processRadioIdLookups();
bool httpGetString(const String& url, String& payload, int& codeOut, const String& accept, const String& token);
String uppercaseTrimmed(String value);
String htmlEntityDecode(String value);
String stripHtmlTags(String html);
String targetDigitsOnly(const String& value);
String trimLeadingZeros(String value);
String normalizeW0chpMode(const String& rawMode);
bool targetListLookupSupported(const String& mode);
String normalizeDstarReflectorKey(String value);
String normalizeTargetLookupKey(const String& mode, const String& target);
bool targetNumericCellMatches(const String& cell, const String& key);
bool targetDstarCellMatches(const String& cell, const String& key);
String targetNameLookupCached(const String& mode, const String& target);
const char* targetListDefaultPathForMode(const String& mode);
const char* targetListFilePathForMode(const String& mode);
String targetListOverrideUrlForMode(const String& mode);
String targetListUrlForMode(const String& mode);
String targetListModeFieldName(const String& mode);
String targetListFileSizeText(const String& mode);
const char* targetListTempFilePathForMode(const String& mode);
int targetNameLookupCacheIndex(const String& mode, const String& key);
struct TargetNameLookupCacheEntry* targetNameLookupCacheSlot(const String& mode, const String& key, bool createIfMissing);
bool extractHtmlCellsFromRow(const String& row, String* cells, int maxCells, int& cellCount);
String sanitizeTargetListName(String name);
bool writeTargetListLine(Print& out, const String& mode, const String& rawKey, const String& rawName, int& writtenCount);
void writeTargetListEntriesFromCells(Print& out, const String& mode, String* cells, int cellCount, int& writtenCount);
bool writeTargetListFileFromW0chpHtml(const String& mode, const String& payload, const char* finalPath, const char* tmpPath, int& entryCount);
bool writeTargetListFileFromPlainText(const String& mode, const String& payload, const char* finalPath, const char* tmpPath, int& entryCount);
bool splitTargetListLine(const String& line, String& keyPart, String& namePart);
bool lookupTargetNameInSdFile(const String& mode, const String& key, String& outName);
bool downloadTargetListForMode(const String& mode, String* detailOut = nullptr);
void clearTargetNameLookupCache();
void processTargetNameLookups();
void handleTargetListsPage();
void handleSaveTargetLists();
void handleHamqslPage();
void handleSaveHamqsl();
void handleUpdateTargetLists();

struct ModeStatus {
  bool known = false;
  bool enabled = false;
};

struct QuickActionSlotConfig {
  bool enabled = false;
  bool useSudo = false;
  String label = "";
  String command = "";
};

static inline const char* storageOpenMode(const char* mode) {
  if (strcmp(mode, "rb") == 0) return FILE_READ;
  if (strcmp(mode, "wb") == 0) return FILE_WRITE;
  if (strcmp(mode, "ab") == 0) return FILE_APPEND;
  return mode;
}

File storageOpen(const char* path, const char* mode = FILE_READ) {
  const char* openMode = storageOpenMode(mode);
#if (DISPLAY_BACKEND_IS_WS_RGB_800_480 && WS8048_USE_SD_MMC) || (DISPLAY_BACKEND_IS_WS_TOUCH_LCD_2_8 && WS28_USE_SD_MMC)
  return SD_MMC.open(path, openMode);
#else
  return SD.open(path, openMode);
#endif
}

bool storageExists(const char* path) {
#if (DISPLAY_BACKEND_IS_WS_RGB_800_480 && WS8048_USE_SD_MMC) || (DISPLAY_BACKEND_IS_WS_TOUCH_LCD_2_8 && WS28_USE_SD_MMC)
  return SD_MMC.exists(path);
#else
  return SD.exists(path);
#endif
}

bool storageRemove(const char* path) {
#if (DISPLAY_BACKEND_IS_WS_RGB_800_480 && WS8048_USE_SD_MMC) || (DISPLAY_BACKEND_IS_WS_TOUCH_LCD_2_8 && WS28_USE_SD_MMC)
  return !SD_MMC.exists(path) || SD_MMC.remove(path);
#else
  return !SD.exists(path) || SD.remove(path);
#endif
}

bool storageMkdir(const char* path) {
#if (DISPLAY_BACKEND_IS_WS_RGB_800_480 && WS8048_USE_SD_MMC) || (DISPLAY_BACKEND_IS_WS_TOUCH_LCD_2_8 && WS28_USE_SD_MMC)
  return storageExists(path) || SD_MMC.mkdir(path);
#else
  return storageExists(path) || SD.mkdir(path);
#endif
}

bool storageRename(const char* fromPath, const char* toPath) {
#if (DISPLAY_BACKEND_IS_WS_RGB_800_480 && WS8048_USE_SD_MMC) || (DISPLAY_BACKEND_IS_WS_TOUCH_LCD_2_8 && WS28_USE_SD_MMC)
  return SD_MMC.rename(fromPath, toPath);
#else
  return SD.rename(fromPath, toPath);
#endif
}

static const char* DEFAULT_TZ_RULE = "CET-1CEST,M3.5.0/2,M10.5.0/3";
static const char* TARGET_LIST_BASE_URL_DEFAULT = "https://w0chp.radio/digital-radio-lists/";
static const char* TARGET_LIST_DMR_PATH_DEFAULT = "brandmeister-talkgroups/";
static const char* TARGET_LIST_DSTAR_PATH_DEFAULT = "xlx-reflectors/";
static const char* TARGET_LIST_YSF_PATH_DEFAULT = "ysf-reflectors/";
static const char* TARGET_LIST_P25_PATH_DEFAULT = "p25-reflectors/";
static const char* TARGET_LIST_NXDN_PATH_DEFAULT = "nxdn-reflectors/";
static const uint8_t QUICK_ACTION_SLOT_COUNT = 4;
static const uint8_t SVXLINK_ACTION_SLOT_COUNT = 4;
static const uint16_t APRS_REFRESH_MINUTES_DEFAULT = 30;
static const uint16_t APRS_REFRESH_MINUTES_MIN = 10;
static const uint16_t APRS_REFRESH_MINUTES_MAX = 1440;
static const unsigned long APRS_MIN_REQUEST_GAP_MS = 5UL * 60UL * 1000UL;
static const unsigned long APRS_MIN_RETRY_MS = 5UL * 60UL * 1000UL;
static const unsigned long APRS_MAX_RETRY_MS = 2UL * 60UL * 60UL * 1000UL;
static const unsigned long APRS_RATE_LIMIT_BACKOFF_MS = 60UL * 60UL * 1000UL;
static const uint16_t HAMQSL_REFRESH_MINUTES_DEFAULT = 60;
static const uint16_t HAMQSL_REFRESH_MINUTES_MIN = 60;
static const uint16_t HAMQSL_REFRESH_MINUTES_MAX = 1440;
static const char* HAMQSL_SOLAR_XML_URL_DEFAULT = "https://www.hamqsl.com/solarxml.php";


struct WifiProfileConfig {
  String ssid = "";
  String pass = "";
};

struct AppConfig {
  String backend;
  String wifiSsid;
  String wifiPass;
  WifiProfileConfig wifiProfiles[MAX_WIFI_PROFILES];
  uint8_t wifiRetriesPerProfile = WIFI_PROFILE_RETRY_COUNT_DEFAULT;
  String pistarHost;
  bool svxlinkEnabled = false;
  String svxlinkSource = "url";
  String svxlinkHost = "";
  String svxlinkApiPath = "/index.php";
  bool aprsEnabled = false;
  String aprsApiKey = "";
  uint16_t aprsRefreshMinutes = APRS_REFRESH_MINUTES_DEFAULT;
  String startPageAfterBoot = "mmdvm_live";
  String deviceCallsign = "";
  String themeColor = "#35C2FF";
  uint32_t refreshMs = 1500;
  uint32_t callsignHoldMs = 12000;
  bool scrollingTextEnabled = true;
  bool apiUtcToLocalTime = true;
  String timezoneTz = DEFAULT_TZ_RULE;
  uint8_t brightness = 220;
  bool flipScreen = false;
  bool screensaverEnabled = true;
  uint32_t screensaverTimeoutMs = 240000;
  uint8_t screensaverDimBrightness = 25;
  bool weatherEnabled = false;
  uint16_t weatherRefreshMinutes = 10;
  String weatherLat = "";
  String weatherLon = "";
  String weatherApiKey = "";
  bool hamqslEnabled = false;
  uint16_t hamqslRefreshMinutes = HAMQSL_REFRESH_MINUTES_DEFAULT;
  String hamqslUrl = HAMQSL_SOLAR_XML_URL_DEFAULT;
  bool radioIdLookupEnabled = true;
  String radioIdUsersApiBase = "https://radioid.net/api/users?callsign=";
  bool targetListLookupEnabled = true;
  String targetListBaseUrl = "https://w0chp.radio/digital-radio-lists/";
  String targetListDmrUrl = "";
  String targetListDstarUrl = "";
  String targetListYsfUrl = "";
  String targetListP25Url = "";
  String targetListNxdnUrl = "";
  bool touchModeSshEnabled = false;
  String pistarSshUser = "pi-star";
  String pistarSshPass = "";
  uint16_t pistarSshPort = 22;
  bool svxlinkSshEnabled = false;
  String svxlinkSshUser = "svxlink";
  String svxlinkSshPass = "";
  uint16_t svxlinkSshPort = 22;
  QuickActionSlotConfig quickActions[QUICK_ACTION_SLOT_COUNT];
  QuickActionSlotConfig svxlinkActions[SVXLINK_ACTION_SLOT_COUNT];
} cfg;

struct WeatherState {
  bool valid = false;
  bool fetchInProgress = false;
  float tempC = 0.0f;
  float feelsLikeC = 0.0f;
  float tempMinC = 0.0f;
  float tempMaxC = 0.0f;
  float windSpeedMs = 0.0f;
  int windDeg = 0;
  int humidity = 0;
  int pressureHpa = 0;
  int visibilityM = 0;
  String icon = "";
  String summary = "";
  String description = "";
  String location = "";
  unsigned long lastSuccessMillis = 0;
  unsigned long lastAttemptMillis = 0;
  int lastHttpCode = 0;
} weatherState;

struct HamqslBandCondition {
  String name = "";
  String day = "";
  String night = "";
};

struct HamqslVhfCondition {
  String name = "";
  String location = "";
  String condition = "";
};

struct HamqslState {
  bool valid = false;
  bool fetchInProgress = false;
  String updated = "";
  String solarFlux = "";
  String sunspots = "";
  String aIndex = "";
  String kIndex = "";
  String xray = "";
  String geomagField = "";
  String signalNoise = "";
  String solarWind = "";
  String magneticField = "";
  String aurora = "";
  String auroraLatitude = "";
  String muf = "";
  String fof2 = "";
  HamqslBandCondition hfBands[4];
  uint8_t hfBandCount = 0;
  HamqslVhfCondition vhfConditions[6];
  uint8_t vhfConditionCount = 0;
  String error = "";
  unsigned long lastSuccessMillis = 0;
  unsigned long lastAttemptMillis = 0;
  int lastHttpCode = 0;
} hamqslState;


struct AprsState {
  bool valid = false;
  bool fetchInProgress = false;
  String callsign = "";
  String sourceCallsign = "";
  String dstCallsign = "";
  String symbol = "";
  float lat = 0.0f;
  float lng = 0.0f;
  bool speedValid = false;
  float speedKmh = 0.0f;
  bool courseValid = false;
  int courseDeg = 0;
  bool altitudeValid = false;
  float altitudeM = 0.0f;
  String comment = "";
  String path = "";
  String status = "";
  String lastSeenText = "";
  String error = "";
  String failCode = "";
  uint8_t consecutiveFailures = 0;
  unsigned long lastSuccessMillis = 0;
  unsigned long lastAttemptMillis = 0;
  unsigned long nextAllowedAttemptMillis = 0;
  int lastHttpCode = 0;
} aprsState;

static const char* RADIOID_USERS_API_BASE_DEFAULT = "https://radioid.net/api/users?callsign=";
static const int RADIOID_LOOKUP_CACHE_SIZE = 8;
static const unsigned long RADIOID_LOOKUP_TTL_MS = 24UL * 60UL * 60UL * 1000UL;
static const unsigned long RADIOID_LOOKUP_RETRY_MS = 15UL * 60UL * 1000UL;
static const unsigned long RADIOID_LOOKUP_MIN_INTERVAL_MS = 1500UL;

struct RadioIdLookupCacheEntry {
  String callsign = "";
  String name = "";
  String country = "";
  String city = "";
  String state = "";
  bool valid = false;
  bool found = false;
  unsigned long lastFetchMillis = 0;
};

RadioIdLookupCacheEntry radioIdLookupCache[RADIOID_LOOKUP_CACHE_SIZE];
int radioIdLookupCacheWriteIndex = 0;
unsigned long lastRadioIdLookupMillis = 0;
String pendingRadioIdLookupCallsign = "";
String lastRadioIdLookupSignature = "";

struct TargetNameLookupCacheEntry {
  String mode = "";
  String target = "";
  String name = "";
  bool valid = false;
  bool found = false;
  unsigned long lastFetchMillis = 0;
};

TargetNameLookupCacheEntry targetNameLookupCache[TARGET_LIST_LOOKUP_CACHE_SIZE];
int targetNameLookupCacheWriteIndex = 0;
unsigned long lastTargetListDownloadMillis = 0;
unsigned long lastTargetListDownloadFailureMillis = 0;
uint8_t targetListDownloadIndex = 0;
bool targetListForceUpdateRequested = false;
bool targetListBootUpdateFinished = false;

struct SvxlinkModuleState {
  String name = "";
  bool active = false;
};

static const uint8_t SVXLINK_MAX_LOGICS = 6;
static const uint8_t SVXLINK_MAX_MODULES = 8;
String svxlinkCallsign = "";
String svxlinkNetworkName = "";
String svxlinkRadioStatus = "";
String svxlinkReflectorStatus = "";
String svxlinkModeText = "";
String svxlinkDefaultTg = "";
String svxlinkActiveTg = "";
String svxlinkMonitorTgs = "";
String svxlinkLastReboot = "";
String svxlinkSystemText = "";
String svxlinkLogics[SVXLINK_MAX_LOGICS];

/* DL3EL */
String  svxlinkLat = "";
String  svxlinkLon = "";
/* DL3EL */

uint8_t svxlinkLogicCount = 0;
SvxlinkModuleState svxlinkModules[SVXLINK_MAX_MODULES];
uint8_t svxlinkModuleCount = 0;
unsigned long svxlinkLastSuccessMillis = 0;

struct Theme {
  uint16_t bg;
  uint16_t panel;
  uint16_t panel2;
  uint16_t border;
  uint16_t text;
  uint16_t subtext;
  uint16_t accent;
  uint16_t good;
  uint16_t warn;
  uint16_t bad;
  uint16_t live;
  uint16_t rx;
  uint16_t tx;
  uint16_t tabIdle;
  uint16_t tabActive;
} theme;

ModeStatus modeDMR, modeDSTAR, modeYSF, modeP25, modeNXDN, modePOCSAG;

enum Page {
  PAGE_LIVE = 0,
  PAGE_LAST = 1,
  PAGE_STATUS = 2,
  PAGE_AP = 3,
  PAGE_INFO = 4,
  PAGE_ABOUT = 5
};

enum WatchSource {
  WATCH_MMDVM = 0,
  WATCH_SVXLINK = 1,
  WATCH_APRS = 2,
  WATCH_WEATHER = 3,
  WATCH_HAMQSL = 4
};

enum LastHeardFilter {
  LAST_HEARD_FILTER_ALL = 0,
  LAST_HEARD_FILTER_DMR,
  LAST_HEARD_FILTER_DSTAR,
  LAST_HEARD_FILTER_YSF,
  LAST_HEARD_FILTER_P25,
  LAST_HEARD_FILTER_NXDN,
  LAST_HEARD_FILTER_POCSAG
};

enum LastHeardSort {
  LAST_HEARD_SORT_NEWEST = 0,
  LAST_HEARD_SORT_CALLSIGN,
  LAST_HEARD_SORT_MODE
};

Page currentPage = PAGE_LIVE;
Page previousPage = PAGE_LIVE;
WatchSource currentWatchSource = WATCH_MMDVM;
bool watchSourcePopupOpen = false;
Page watchSourcePopupTargetPage = PAGE_LIVE;
unsigned long lastPoll = 0;
bool wifiOk = false;
bool configOkState = false;
bool setupRequired = false;
String footerText = "Booting...";
bool timeConfigured = false;
String lastError = "";
bool uiDirty = true;
bool parseHexColor(String s, uint16_t &out565);
bool footerDirty = true;
bool headerClockDirty = true;
bool lastLiveActiveState = false;
int lastRenderedPage = -1;
bool lastDetailPopupState = false;
bool screensaverActive = false;
unsigned long lastInteractionMillis = 0;
unsigned long lastScreensaverFrameMillis = 0;
unsigned long lastWifiRetryMillis = 0;
bool pendingConfigApply = false;
unsigned long pendingConfigApplyAt = 0;
bool wifiConnectInProgress = false;
unsigned long wifiConnectStartedAt = 0;
int8_t wifiConnectProfileIndex = -1;
uint8_t wifiConnectAttemptNumber = 1;
uint8_t wifiNextProfileIndex = 0;
bool wifiProfileTriedThisRound[MAX_WIFI_PROFILES] = { false };
unsigned long lastWifiStateCheck = 0;
int screensaverPosX = 16;
int screensaverPosY = 72;
int screensaverVelX = 4;
int screensaverVelY = 3;
String lastHeaderClock = "";
unsigned long lastClockCheck = 0;
bool nextFetchHeard = false;
unsigned long lastWeatherPollCheck = 0;
unsigned long lastHamqslPollCheck = 0;
LastHeardFilter lastHeardFilter = LAST_HEARD_FILTER_ALL;
LastHeardSort lastHeardSort = LAST_HEARD_SORT_NEWEST;
int lastHeardPageIndex = 0;
static const uint8_t LAST_HEARD_ROWS_PER_PAGE = 4;
static const unsigned long SCROLL_TEXT_STEP_MS = 230UL;
bool scrollingTextActiveFrame = false;
bool scrollingTextDirty = false;
unsigned long lastScrollingTextFrameMs = 0;

void requestRedraw() {
  uiDirty = true;
}

String getDeviceCallsign();
void noteScreenActivity(bool fromTouch = false);

String resolveTargetDisplay(const HeardRecord& rec);
String resolveTargetLabel(const HeardRecord& rec);
String prettyMetricValue(const String& value, const char* suffix = nullptr, bool keepPercent = false);
String firstNonEmpty(const String& a, const String& b, const String& c = "", const String& d = "", const String& e = "", const String& f = "");
bool svxlinkFeatureEnabled();
bool isCurrentWatchSvxlink();
bool isSvxlinkBackend();
bool aprsFeatureEnabled();
bool isCurrentWatchAprs();
bool isAprsBackend();
bool weatherFeatureEnabled();
bool isCurrentWatchWeather();
bool isWeatherBackend();
bool hamqslFeatureEnabled();
bool isCurrentWatchHamqsl();
bool isHamqslBackend();
String canonicalStartPageAfterBootValue(const String& rawValue);
bool startPageAfterBootOptionsAvailable();
bool startPageAfterBootChoiceAvailable(const String& value, bool requireRuntimeReady = false);
String normalizedStartPageAfterBoot(bool requireRuntimeReady = false);
String startPageAfterBootLabel(const String& value);
String startPageAfterBootSelectRow(bool includeSvxlinkOptions, bool includeAprsOptions, bool includeWeatherOptions, bool includeHamqslOptions);
void readStartPageAfterBootFromForm();
void applyStartPageAfterBoot();
void setCurrentWatchSource(WatchSource source);
void saveActiveHeardCache();
void loadActiveHeardCache();
String activeBackendHost();
String displayBackendHost();
String svxlinkApiUrl();
String aprsApiUrl(const String& callsign);
String aprsLookupCallsign();
String aprsFormatEpoch(const String& epochText);
String aprsIconPath(const String& symbol);
bool drawAprsIcon(const String& symbol, int x, int y, uint16_t bg565);
void resetAprsState();
bool fetchAprsData(bool force = false);
void drawAprsLivePage();
String formatApiDateTime(const struct tm& tmValue);
void resetSvxlinkState();
void parseSvxlinkStatusFromObject(JsonObject root);
bool fetchSvxlinkData();
void drawSvxlinkLivePage();
void drawSvxlinkHeardPage();
void updateSessionInfoFromRecord(const HeardRecord& rec);
void drawInfoPage();
void drawAboutPage();
String getDateText();
String getDetailedClockText();
String getLastHeardFilterLabel();
String getLastHeardSortLabel();
bool heardRecordMatchesLastHeardFilter(const HeardRecord& rec);
int buildLastHeardVisibleIndices(int* outIndices, int maxCount);
void normalizeLastHeardBrowseState();
void normalizeSvxlinkHeardBrowseState();

void setFooterText(const String& text, bool countAsScreenActivity = true) {
  if (footerText != text) {
    footerText = text;
    footerDirty = true;
    if (countAsScreenActivity) noteScreenActivity(false);
  }
}

void applyDisplaySettings();
void setScreensaverActive(bool active);
void noteUserInteraction(bool fromTouch = false);
void drawScreensaver(bool forceClear = false);
bool configLooksValid();
bool startSetupAccessPoint();
void stopSetupAccessPoint();
void beginWiFiConnectAttempt(bool stopApFirst = true);
void handleWiFiConnectProgress();
void syncTimeIfNeeded();

String weatherRequestUrl();
void clearWeatherData();

float lastAppliedLat = 0.0f;
float lastAppliedLon = 0.0f;
const float MIN_DISTANCE_THRESHOLD = 0.04f;

bool weatherConfigured();
String weatherDisplayText();
String weatherIconPath();
String weatherDetailSummary();
String weatherWindText();
String weatherVisibilityText();
String weatherLastUpdatedText();
bool fetchWeatherData();
void drawWeatherLivePage();
void clearHamqslData();
bool hamqslConfigured();
String hamqslRequestUrl();
String hamqslLastUpdatedText();
bool fetchHamqslData();
void drawHamqslLivePage();
bool handleWatchSourceSwipe(int dx);
bool persistConfigDocument(DynamicJsonDocument& doc, const char* path = CONFIG_FILE);
bool loadConfig();
String pageTop(const String& title, const String& active, const String& subtitle = "");
String pageBottom();

bool applyConfigFromFile(const char* path, String* errMsg = nullptr);
void applyConfigAfterLoad();
void sendWebMessagePage(const String& title, const String& lead, const String& detail = "", bool isError = false);
void handleExportConfig();
void handleImportConfig();
void handleImportConfigUpload();

String jsonEscape(const String& s);
String formatMillisAgoBrief(unsigned long sinceMs);
String otaErrorText();
String buildSidebarHtml(bool onRoot, const String& activeApp = "");
void noteApiFetchResult(bool ok, int statusCode, unsigned long latencyMs);
void noteWeatherFetchResult(bool ok, int statusCode);
void noteHamqslFetchResult(bool ok, int statusCode);
void handleHamqslPage();
void handleSaveHamqsl();
String buildDiagnosticsJson();
void handleDiagnosticsPage();
void handleDiagnosticsApi();
void handleOtaPage();
void handleOtaUpload();
void handleOtaUpdate();
void handleFaqPage();
void handleChangelogPage();
String buildWebLogsJson();
void handleLogsApi();
void handleLogsPage();
void handleLogsClear();
void addWebLog(const String& level, const String& tag, const String& message, bool mirrorSerial = true);
String formatLogAgeBrief(unsigned long sinceMs);

String formatUptimeBrief() {
  unsigned long totalSeconds = millis() / 1000UL;
  unsigned long days = totalSeconds / 86400UL;
  unsigned long hours = (totalSeconds % 86400UL) / 3600UL;
  unsigned long minutes = (totalSeconds % 3600UL) / 60UL;

  if (days > 0) return String(days) + "d " + String(hours) + "h";
  if (hours > 0) return String(hours) + "h " + String(minutes) + "m";
  return String(minutes) + "m";
}

String getBuildStamp() {
  return String(__DATE__) + " " + String(__TIME__);
}

String getWifiSignalText() {
  if (!wifiOk) return "Offline";
  long rssi = WiFi.RSSI();
  if (rssi == 0) return "Connected";
  return String(rssi) + " dBm";
}

void clearWeatherData() {
  weatherState.valid = false;
  weatherState.fetchInProgress = false;
  weatherState.tempC = 0.0f;
  weatherState.feelsLikeC = 0.0f;
  weatherState.tempMinC = 0.0f;
  weatherState.tempMaxC = 0.0f;
  weatherState.windSpeedMs = 0.0f;
  weatherState.windDeg = 0;
  weatherState.humidity = 0;
  weatherState.pressureHpa = 0;
  weatherState.visibilityM = 0;
  weatherState.icon = "";
  weatherState.summary = "";
  weatherState.description = "";
  weatherState.location = "";
  weatherState.lastSuccessMillis = 0;
  weatherState.lastAttemptMillis = 0;
  weatherState.lastHttpCode = 0;
}

bool weatherConfigured() {
	/* DL3EL */
	if (svxlinkLat.length() && svxlinkLon.length()) {
		float currentLat = svxlinkLat.toFloat();
		float currentLon = svxlinkLon.toFloat();

		if (lastAppliedLat == 0.0f && lastAppliedLon == 0.0f) {
			cfg.weatherLat = svxlinkLat;
			cfg.weatherLon = svxlinkLon;
			lastAppliedLat = currentLat;
			lastAppliedLon = currentLon;
		} else {
			float diffLat = abs(currentLat - lastAppliedLat);
			float diffLon = abs(currentLon - lastAppliedLon);

			if (diffLat >= MIN_DISTANCE_THRESHOLD || diffLon >= MIN_DISTANCE_THRESHOLD) {
				cfg.weatherLat = svxlinkLat;
				cfg.weatherLon = svxlinkLon;
				lastAppliedLat = currentLat;
				lastAppliedLon = currentLon;
				weatherState.valid = false; 
				Serial.printf("[Weather] new coodinates %s %s \n",svxlinkLat,svxlinkLon);
			}
		}
	}
	/* DL3EL */

	return cfg.weatherEnabled && trimSafe(cfg.weatherLat).length() && trimSafe(cfg.weatherLon).length() && trimSafe(cfg.weatherApiKey).length();
}

String weatherRequestUrl() {
  if (!weatherConfigured()) return "";
  String url = "https://api.openweathermap.org/data/2.5/weather?lat=";
  url += trimSafe(cfg.weatherLat);
  url += "&lon=";
  url += trimSafe(cfg.weatherLon);
  url += "&units=metric&appid=";
  url += trimSafe(cfg.weatherApiKey);
  return url;
}

String weatherDisplayText() {
  if (!weatherState.valid) return "";
  return String((int)lroundf(weatherState.tempC)) + " C";
}

String weatherIconPath() {
  if (!weatherState.valid) return "";
  String icon = trimSafe(weatherState.icon);
  if (icon.length() < 2) return "";

  String candidate = "/weather/" + icon + ".png";
  if (storageExists(candidate.c_str())) return candidate;

  String shortCode = icon.substring(0, 2);
  candidate = "/weather/" + shortCode + ".png";
  if (storageExists(candidate.c_str())) return candidate;

  return "";
}

String weatherIconPath32() {
  if (!weatherState.valid) return "";
  String icon = trimSafe(weatherState.icon);
  if (icon.length() < 2) return "";

  String candidate = "/weather/" + icon + "_32.png";
  if (storageExists(candidate.c_str())) return candidate;

  String shortCode = icon.substring(0, 2);
  candidate = "/weather/" + shortCode + "_32.png";
  if (storageExists(candidate.c_str())) return candidate;

  return "";
}

String weatherDetailSummary() {
  if (!weatherState.valid) return "";
  String text = trimSafe(weatherState.description.length() ? weatherState.description : weatherState.summary);
  if (!text.length()) return "Weather updated";
  if (text.length() > 0) text.setCharAt(0, (char)toupper((unsigned char)text.charAt(0)));
  return text;
}

String weatherCompassDirectionText(int deg) {
  if (deg <= 0) return "";
  static const char* dirs[] = { "N", "NE", "E", "SE", "S", "SW", "W", "NW" };
  int index = ((deg + 22) % 360) / 45;
  return String(dirs[index]);
}

String weatherWindText() {
  if (!weatherState.valid) return "-";
  float kmh = weatherState.windSpeedMs * 3.6f;
  String text = String(kmh, kmh < 10.0f ? 1 : 0) + " km/h";
  String dir = weatherCompassDirectionText(weatherState.windDeg);
  if (dir.length()) text += " " + dir;
  return text;
}

String weatherVisibilityText() {
  if (!weatherState.valid || weatherState.visibilityM <= 0) return "-";
  if (weatherState.visibilityM >= 1000) return String(weatherState.visibilityM / 1000.0f, 1) + " km";
  return String(weatherState.visibilityM) + " m";
}

String weatherLastUpdatedText() {
  if (weatherState.fetchInProgress) return "Fetching...";
  if (weatherState.lastSuccessMillis) return formatMillisAgoBrief(weatherState.lastSuccessMillis);
  if (weatherState.lastAttemptMillis) return String("HTTP ") + String(weatherState.lastHttpCode);
  return "Waiting";
}

void clearHamqslData() {
  hamqslState = HamqslState();
}

bool hamqslConfigured() {
  return cfg.hamqslEnabled && trimSafe(cfg.hamqslUrl).length() > 0;
}

String hamqslRequestUrl() {
  if (!hamqslConfigured()) return "";
  String url = trimSafe(cfg.hamqslUrl);
  if (!url.length()) url = HAMQSL_SOLAR_XML_URL_DEFAULT;
  return url;
}

String hamqslLastUpdatedText() {
  if (hamqslState.fetchInProgress) return "Fetching...";
  if (hamqslState.lastSuccessMillis) return formatMillisAgoBrief(hamqslState.lastSuccessMillis);
  if (hamqslState.lastAttemptMillis) return hamqslState.lastHttpCode ? (String("HTTP ") + String(hamqslState.lastHttpCode)) : String("Failed");
  return "Waiting";
}

bool detailPopupOpen = false;
int selectedHeardIndex = -1;
bool modeConfirmPopupOpen = false;
String modeConfirmTargetLabel = "";
bool modeConfirmEnableRequested = false;
bool quickActionPopupOpen = false;
bool activeQuickActionIsSvxlink = false;
bool parsingSvxlinkData = false;

bool aprsCommentPopupOpen = false;

TaskHandle_t modeToggleTaskHandle = nullptr;
volatile bool modeToggleTaskRunning = false;
volatile bool modeToggleTaskDone = false;
volatile bool modeToggleTaskOk = false;
volatile bool modeToggleTaskEnable = false;
char modeToggleTaskLabel[16] = {0};
char modeToggleTaskMessage[192] = {0};

struct ModeToggleTaskArgs {
  String label;
  bool enable;
};

TaskHandle_t quickActionTaskHandle = nullptr;
volatile bool quickActionTaskRunning = false;
volatile bool quickActionTaskDone = false;
volatile bool quickActionTaskOk = false;
char quickActionTaskLabel[32] = {0};
char quickActionTaskMessage[192] = {0};

struct QuickActionTaskArgs {
  uint8_t slot;
  bool svxlinkAction;
};

const int BRIGHT_SLIDER_X = UX(108);
const int BRIGHT_SLIDER_Y = UY(193);
const int BRIGHT_SLIDER_W = UW(150);
const int BRIGHT_SLIDER_H = UH(8);

struct HeardRecord {
  String timeUtc;
  String mode;
  String callsign;
  String name;
  String country;
  String grid;
  String target;
  String targetName;
  String timeslot;
  String src;
  String duration;
  String loss;
  String ber;
  String rssi;
  String rxFreq;
  String txFreq;
  bool valid = false;
};

void setModeStatusIfPresent(ModeStatus& st, JsonVariantConst v);
bool modeStatusEqual(const ModeStatus& a, const ModeStatus& b);
bool allModeStatusesEqual(const ModeStatus& dmr, const ModeStatus& dstar, const ModeStatus& ysf,
                          const ModeStatus& p25, const ModeStatus& nxdn, const ModeStatus& pocsag);
void inferModeStatusesFromHeard(const HeardRecord* list, int count);
uint16_t modeColor(const ModeStatus& st, bool active);
bool sameRecord(const HeardRecord& a, const HeardRecord& b);
String liveStatusText(bool active, const HeardRecord& rec);
String normalizeDmrTimeslot(String raw);
String dmrTimeslotLabel(const HeardRecord& rec);
const HeardRecord& dmrTimeslotRecord(uint8_t slot);
bool isDmrTimeslotLiveActive(uint8_t slot);
bool shouldShowDmrTimeslotLivePanel(bool active, const HeardRecord& rec);
void clearDmrTimeslotLiveRecords();
void updateDmrTimeslotLiveRecordsFromList(const HeardRecord* list, int count);
void clearHeard();
String normalizeHeardTimeDisplay(const String& raw);
String extractDmrTimeslotFromText(const String& text);
void drawModeButton(int x, int y, int w, int h, const String& label, const ModeStatus& st, bool active);
ModeStatus* modeStatusFromTouchLabel(const String& label);
bool startModeToggleTask(const String& label, bool enable);
void processModeToggleTaskResult();
uint8_t quickActionSlotLimit(bool svxlinkAction = false);
bool hasConfiguredQuickActions(bool svxlinkAction = false);
bool isQuickActionSlotConfigured(uint8_t slot, bool svxlinkAction = false);
String quickActionSlotLabel(uint8_t slot, bool svxlinkAction = false);
bool startQuickActionTask(uint8_t slot, bool svxlinkAction = false);
void processQuickActionTaskResult();
String renderSvxlinkActionRows();
void readSvxlinkActionRowsFromForm();
void drawSignalMeter(int x, int y, int w, int h, bool active, const HeardRecord* rec);
void drawDetailPopup(const HeardRecord& r);
void drawQuickActionsPopup();
void drawWatchSourcePopup();
String currentSvxlinkUserCallsign();
bool svxlinkAprsAlertVisible();
void openSvxlinkAprsCommentPopup();
void drawAprsCommentPopup();
void clearSvxlinkAprsCommentAlert();
void updateSvxlinkAprsCommentAlertFromState();
bool heardListsEqual(const HeardRecord* a, int aCount, const HeardRecord* b, int bCount);
bool extractWpsdStatusPill(const String& html, const String& label, ModeStatus& st);
void parseModeCellWPSD(const String& html, const String& label, ModeStatus& st);
void parseModeCellPiStar(const String& html, const String& label, ModeStatus& st);
uint16_t bmpRead16(fs::File &f);
uint32_t bmpRead32(fs::File &f);

struct SessionInfo {
  uint32_t totalQsos = 0;
  uint32_t totalLiveHits = 0;
  uint32_t modeDMR = 0;
  uint32_t modeDSTAR = 0;
  uint32_t modeYSF = 0;
  uint32_t modeP25 = 0;
  uint32_t modeNXDN = 0;
  uint32_t modePOCSAG = 0;
  int32_t bestRssi = -999;
  int32_t worstRssi = 0;
  bool hasRssi = false;
} sessionInfo;

static const int32_t DIAG_NULL_INT = -32768;
static const size_t WEB_LOG_CAPACITY = 8;

struct WebLogEntry {
  unsigned long ms = 0;
  String level = "INFO";
  String tag = "";
  String message = "";
};

WebLogEntry webLogBuffer[WEB_LOG_CAPACITY];
size_t webLogHead = 0;
size_t webLogCount = 0;

struct DiagnosticsState {
  uint32_t fetchOk = 0;
  uint32_t fetchFail = 0;
  uint32_t consecutiveFetchFailures = 0;
  uint32_t lastApiLatencyMs = 0;
  int32_t lastApiStatusCode = 0;
  unsigned long lastFetchMillis = 0;
  unsigned long lastFetchSuccessMillis = 0;
  uint32_t weatherOk = 0;
  uint32_t weatherFail = 0;
  int32_t lastWeatherHttpCode = 0;
  unsigned long lastWeatherAttemptMillis = 0;
  unsigned long lastWeatherSuccessMillis = 0;
} diagnosticsState;

bool webLogApiStateKnown = false;
bool webLogLastApiOk = false;
int32_t webLogLastApiCode = 0;
bool webLogWeatherStateKnown = false;
bool webLogLastWeatherOk = false;
int32_t webLogLastWeatherCode = 0;
bool webLogHamqslStateKnown = false;
bool webLogLastHamqslOk = false;
int32_t webLogLastHamqslCode = 0;
bool otaUpdateSuccessful = false;
unsigned long otaRestartAt = 0;
String otaLastError = "";
bool otaHeaderChecked = false;
size_t otaBytesWritten = 0;

const int MAX_HEARD = 12;
HeardRecord heardList[MAX_HEARD];
int heardCount = 0;

HeardRecord liveRecord;
unsigned long liveRecordMillis = 0;

HeardRecord dmrTs1LiveRecord;
unsigned long dmrTs1LiveRecordMillis = 0;
HeardRecord dmrTs2LiveRecord;
unsigned long dmrTs2LiveRecordMillis = 0;

HeardRecord mmdvmHeardList[MAX_HEARD];
int mmdvmHeardCount = 0;
HeardRecord mmdvmLiveRecord;
unsigned long mmdvmLiveRecordMillis = 0;
HeardRecord svxlinkHeardList[MAX_HEARD];
int svxlinkHeardCount = 0;
HeardRecord svxlinkLiveRecord;
unsigned long svxlinkLiveRecordMillis = 0;

String currentRxFreq = "438.8000";
String currentTxFreq = "430.8000";
String hotspotRxFreq = "";
String hotspotTxFreq = "";
String hotspotStatusText = "";

String trimSafe(const String& s) {
  String out = s;
  out.trim();
  return out;
}

String shellSingleQuote(const String& s) {
  String out = "'";
  for (size_t i = 0; i < s.length(); ++i) {
    char c = s.charAt(i);
    if (c == '\'') out += "'\\''";
    else out += c;
  }
  out += "'";
  return out;
}

String buildOperatorName(const String& firstName, const String& lastName) {
  String full = trimSafe(firstName);
  String last = trimSafe(lastName);
  if (last.length()) {
    if (full.length()) full += " ";
    full += last;
  }
  return trimSafe(full);
}

String clipWithTilde(const String& input, size_t maxChars) {
  String value = trimSafe(input);
  if (!maxChars || value.length() <= maxChars) return value;
  if (maxChars == 1) return String("~");
  return value.substring(0, maxChars - 1) + "~";
}

String formatCallsignName(const String& callsign, const String& name, size_t maxNameChars, size_t maxCallsignChars) {
  String shownCallsign = normalizeHamCallsign(callsign);
  if (!shownCallsign.length()) shownCallsign = "-";
  if (maxCallsignChars) shownCallsign = clipWithTilde(shownCallsign, maxCallsignChars);

  String shownName = trimSafe(name);
  if (!shownName.length()) return shownCallsign;
  if (maxNameChars) shownName = clipWithTilde(shownName, maxNameChars);

  return shownCallsign + " - " + shownName;
}

String fitLivePanelPrimaryText(const String& input, int availableWidth, int& font) {
  String value = trimSafe(input);
  if (!value.length()) value = "-";

  const uint8_t candidateFonts[] = {4, 2, 1};
  for (size_t i = 0; i < sizeof(candidateFonts) / sizeof(candidateFonts[0]); ++i) {
    font = candidateFonts[i];
    tft.setTextFont(font);
    if (tft.textWidth(value) <= availableWidth) return value;
  }

  font = 1;
  tft.setTextFont(font);
  if (tft.textWidth(value) <= availableWidth) return value;

  String clipped = value;
  while (clipped.length() > 1) {
    clipped.remove(clipped.length() - 1);
    String trial = clipped + "~";
    if (tft.textWidth(trial) <= availableWidth) return trial;
  }

  return String("~");
}

String getDeviceCallsign() {
  String callsign = trimSafe(cfg.deviceCallsign);
  return callsign.length() ? callsign : String(DEVICE_CALLSIGN);
}
String normalizeModeName(String s) {
  s = trimSafe(s);
  s.toUpperCase();
  s.replace("_", "-");
  while (s.indexOf("  ") >= 0) s.replace("  ", " ");

  if (s.startsWith("DMR")) return "DMR";
  if (s.startsWith("D-STAR") || s.startsWith("DSTAR")) return "DSTAR";
  if (s.startsWith("YSF") || s.indexOf("FUSION") >= 0 || s.startsWith("C4FM")) return "YSF";
  if (s.startsWith("P25")) return "P25";
  if (s.startsWith("NXDN")) return "NXDN";
  if (s.startsWith("POCSAG")) return "POCSAG";
  if (s.startsWith("SVXLINK") || s.startsWith("SVX-LINK") || s.startsWith("SVX")) return "SVXLINK";

  return s;
}

String extractFreqString(JsonVariantConst v) {
  if (v.isNull()) return "";

  String out;
  if (v.is<const char*>()) out = String(v.as<const char*>());
  else if (v.is<String>()) out = v.as<String>();
  else if (v.is<float>() || v.is<double>()) out = String(v.as<double>(), 4);
  else if (v.is<int>() || v.is<long>() || v.is<unsigned int>() || v.is<unsigned long>()) {
    double raw = v.as<double>();
    if (raw > 1000000.0) raw /= 1000000.0;
    out = String(raw, 4);
  } else {
    return "";
  }

  out = trimSafe(out);
  out.replace(" MHz", "");
  out.replace("MHz", "");
  out.trim();
  return out;
}

void setFreqIfPresent(String& dst, JsonVariantConst v) {
  String parsed = extractFreqString(v);
  if (parsed.length()) dst = parsed;
}

void parseFrequencyFieldsFromObject(JsonObjectConst obj) {
  setFreqIfPresent(hotspotRxFreq, obj["rxFreq"]);
  setFreqIfPresent(hotspotRxFreq, obj["rx_freq"]);
  setFreqIfPresent(hotspotRxFreq, obj["RXFrequency"]);
  setFreqIfPresent(hotspotRxFreq, obj["rx_frequency"]);
  setFreqIfPresent(hotspotRxFreq, obj["rxfrequency"]);
  setFreqIfPresent(hotspotRxFreq, obj["RX Freq"]);
  setFreqIfPresent(hotspotRxFreq, obj["rx_mhz"]);

  setFreqIfPresent(hotspotTxFreq, obj["txFreq"]);
  setFreqIfPresent(hotspotTxFreq, obj["tx_freq"]);
  setFreqIfPresent(hotspotTxFreq, obj["TXFrequency"]);
  setFreqIfPresent(hotspotTxFreq, obj["tx_frequency"]);
  setFreqIfPresent(hotspotTxFreq, obj["txfrequency"]);
  setFreqIfPresent(hotspotTxFreq, obj["TX Freq"]);
  setFreqIfPresent(hotspotTxFreq, obj["tx_mhz"]);

  JsonVariantConst modem = obj["modem"];
  if (!modem.isNull() && modem.is<JsonObjectConst>()) {
    JsonObjectConst mo = modem.as<JsonObjectConst>();
    setFreqIfPresent(hotspotRxFreq, mo["rxFreq"]);
    setFreqIfPresent(hotspotRxFreq, mo["rx_freq"]);
    setFreqIfPresent(hotspotRxFreq, mo["RXFrequency"]);
    setFreqIfPresent(hotspotRxFreq, mo["rx_frequency"]);
    setFreqIfPresent(hotspotTxFreq, mo["txFreq"]);
    setFreqIfPresent(hotspotTxFreq, mo["tx_freq"]);
    setFreqIfPresent(hotspotTxFreq, mo["TXFrequency"]);
    setFreqIfPresent(hotspotTxFreq, mo["tx_frequency"]);
  }

  JsonVariantConst frequencies = obj["frequencies"];
  if (!frequencies.isNull() && frequencies.is<JsonObjectConst>()) {
    JsonObjectConst fq = frequencies.as<JsonObjectConst>();
    setFreqIfPresent(hotspotRxFreq, fq["rx"]);
    setFreqIfPresent(hotspotRxFreq, fq["RX"]);
    setFreqIfPresent(hotspotTxFreq, fq["tx"]);
    setFreqIfPresent(hotspotTxFreq, fq["TX"]);
  }

  JsonVariantConst status = obj["status"];
  if (!status.isNull() && status.is<JsonObjectConst>()) {
    JsonObjectConst st = status.as<JsonObjectConst>();
    setFreqIfPresent(hotspotRxFreq, st["rxFreq"]);
    setFreqIfPresent(hotspotRxFreq, st["rx_frequency"]);
    setFreqIfPresent(hotspotTxFreq, st["txFreq"]);
    setFreqIfPresent(hotspotTxFreq, st["tx_frequency"]);
  }
}

bool strToBoolLoose(String v, bool &out) {
  v = trimSafe(v);
  v.toLowerCase();
  if (v == "1" || v == "true" || v == "on" || v == "yes" || v == "enabled") { out = true; return true; }
  if (v == "0" || v == "false" || v == "off" || v == "no" || v == "disabled") { out = false; return true; }
  return false;
}

bool jsonValueToBoolLoose(JsonVariantConst v, bool &out) {
  if (v.is<bool>()) { out = v.as<bool>(); return true; }
  if (v.is<int>() || v.is<long>() || v.is<unsigned int>() || v.is<unsigned long>()) { out = v.as<long>() != 0; return true; }
  if (v.is<const char*>()) return strToBoolLoose(String(v.as<const char*>()), out);
  if (v.is<String>()) return strToBoolLoose(v.as<String>(), out);
  return false;
}

void setModeStatusIfPresent(ModeStatus& st, JsonVariantConst v) {
  bool parsed = false;
  if (jsonValueToBoolLoose(v, parsed)) {
    st.known = true;
    st.enabled = parsed;
  }
}

void resetModeStatuses() {
  modeDMR = ModeStatus();
  modeDSTAR = ModeStatus();
  modeYSF = ModeStatus();
  modeP25 = ModeStatus();
  modeNXDN = ModeStatus();
  modePOCSAG = ModeStatus();
}

bool modeStatusEqual(const ModeStatus& a, const ModeStatus& b) {
  return a.known == b.known && a.enabled == b.enabled;
}

bool allModeStatusesEqual(const ModeStatus& dmr, const ModeStatus& dstar, const ModeStatus& ysf,
                          const ModeStatus& p25, const ModeStatus& nxdn, const ModeStatus& pocsag) {
  return modeStatusEqual(modeDMR, dmr) &&
         modeStatusEqual(modeDSTAR, dstar) &&
         modeStatusEqual(modeYSF, ysf) &&
         modeStatusEqual(modeP25, p25) &&
         modeStatusEqual(modeNXDN, nxdn) &&
         modeStatusEqual(modePOCSAG, pocsag);
}

void inferModeStatusesFromHeard(const HeardRecord* list, int count) {
  for (int i = 0; i < count; i++) {
    String m = normalizeModeName(list[i].mode);
    if (m == "DMR")   { modeDMR.known = true; modeDMR.enabled = true; }
    if (m == "DSTAR" || m == "D-STAR") { modeDSTAR.known = true; modeDSTAR.enabled = true; }
    if (m == "YSF" || m == "FUSION") { modeYSF.known = true; modeYSF.enabled = true; }
    if (m == "P25")   { modeP25.known = true; modeP25.enabled = true; }
    if (m == "NXDN")  { modeNXDN.known = true; modeNXDN.enabled = true; }
    if (m == "POCSAG") { modePOCSAG.known = true; modePOCSAG.enabled = true; }
  }
}

void parseModeStatusesFromObject(JsonObjectConst obj) {
  parseFrequencyFieldsFromObject(obj);

  JsonVariantConst modes = obj["modes"];
  if (!modes.isNull() && modes.is<JsonObjectConst>()) {
    JsonObjectConst mo = modes.as<JsonObjectConst>();
    setModeStatusIfPresent(modeDMR, mo["DMR"]);
    setModeStatusIfPresent(modeDMR, mo["dmr"]);
    setModeStatusIfPresent(modeDSTAR, mo["DSTAR"]);
    setModeStatusIfPresent(modeDSTAR, mo["dstar"]);
    setModeStatusIfPresent(modeDSTAR, mo["d_star"]);
    setModeStatusIfPresent(modeYSF, mo["YSF"]);
    setModeStatusIfPresent(modeYSF, mo["ysf"]);
    setModeStatusIfPresent(modeP25, mo["P25"]);
    setModeStatusIfPresent(modeP25, mo["p25"]);
    setModeStatusIfPresent(modeNXDN, mo["NXDN"]);
    setModeStatusIfPresent(modeNXDN, mo["nxdn"]);
    setModeStatusIfPresent(modePOCSAG, mo["POCSAG"]);
    setModeStatusIfPresent(modePOCSAG, mo["pocsag"]);
  }

  setModeStatusIfPresent(modeDMR, obj["dmr_enabled"]);
  setModeStatusIfPresent(modeDMR, obj["DMREnabled"]);
  setModeStatusIfPresent(modeDMR, obj["dmrEnabled"]);
  setModeStatusIfPresent(modeDMR, obj["DMR"]);

  setModeStatusIfPresent(modeDSTAR, obj["dstar_enabled"]);
  setModeStatusIfPresent(modeDSTAR, obj["DSTAREnabled"]);
  setModeStatusIfPresent(modeDSTAR, obj["dstarEnabled"]);
  setModeStatusIfPresent(modeDSTAR, obj["DSTAR"]);
  setModeStatusIfPresent(modeDSTAR, obj["D-Star"]);

  setModeStatusIfPresent(modeYSF, obj["ysf_enabled"]);
  setModeStatusIfPresent(modeYSF, obj["YSFEnabled"]);
  setModeStatusIfPresent(modeYSF, obj["ysfEnabled"]);
  setModeStatusIfPresent(modeYSF, obj["YSF"]);
  setModeStatusIfPresent(modeYSF, obj["fusion_enabled"]);

  setModeStatusIfPresent(modeP25, obj["p25_enabled"]);
  setModeStatusIfPresent(modeP25, obj["P25Enabled"]);
  setModeStatusIfPresent(modeP25, obj["p25Enabled"]);
  setModeStatusIfPresent(modeP25, obj["P25"]);

  setModeStatusIfPresent(modeNXDN, obj["nxdn_enabled"]);
  setModeStatusIfPresent(modeNXDN, obj["NXDNEnabled"]);
  setModeStatusIfPresent(modeNXDN, obj["nxdnEnabled"]);
  setModeStatusIfPresent(modeNXDN, obj["NXDN"]);

  setModeStatusIfPresent(modePOCSAG, obj["pocsag_enabled"]);
  setModeStatusIfPresent(modePOCSAG, obj["POCSAGEnabled"]);
  setModeStatusIfPresent(modePOCSAG, obj["pocsagEnabled"]);
  setModeStatusIfPresent(modePOCSAG, obj["POCSAG"]);
}

uint16_t modeColor(const ModeStatus& st, bool active) {
  if (active) return theme.warn;
  if (st.known) return st.enabled ? theme.good : theme.bad;
  return theme.border;
}

bool sameRecord(const HeardRecord& a, const HeardRecord& b) {
  return a.callsign == b.callsign &&
         a.timeUtc   == b.timeUtc &&
         a.mode      == b.mode &&
         a.target    == b.target &&
         a.timeslot  == b.timeslot &&
         a.src       == b.src &&
         a.rxFreq    == b.rxFreq &&
         a.txFreq    == b.txFreq;
}

String canonicalBackendValue(const String& rawBackend) {
  String l = trimSafe(rawBackend);
  l.toLowerCase();
  l.replace("_", "-");
  while (l.indexOf("  ") >= 0) l.replace("  ", " ");

  if (l == "wpsd") return "WPSD";
  if (l == "pi-star" || l == "pistar" || l == "pi star") return "Pi-Star";
  return "";
}

String canonicalSvxlinkSourceValue(const String& rawSource) {
  String l = trimSafe(rawSource);
  l.toLowerCase();
  l.replace("_", "-");
  while (l.indexOf("  ") >= 0) l.replace("  ", " ");
  if (l == "api" || l == "svxlink-api" || l == "svxlink api" || l == "json") return "api";
  if (l == "url" || l == "html" || l == "dashboard" || l == "svxlink-url" || l == "svxlink url") return "url";
  return "";
}

String canonicalStartPageAfterBootValue(const String& rawValue) {
  String l = trimSafe(rawValue);
  l.toLowerCase();
  l.replace("_", "-");
  while (l.indexOf("  ") >= 0) l.replace("  ", " ");

  if (!l.length() || l == "mmdvm" || l == "mmdvm-live" || l == "mmdvm-watch" || l == "live" || l == "watch") return "mmdvm_live";
  if (l == "mmdvm-last" || l == "mmdvm-heard" || l == "heard" || l == "last" || l == "last-heard") return "mmdvm_last";
  if (l == "svx" || l == "svx-live" || l == "svxlink" || l == "svxlink-live" || l == "svxlink-watch") return "svxlink_live";
  if (l == "svx-last" || l == "svx-heard" || l == "svxlink-last" || l == "svxlink-heard") return "svxlink_last";
  if (l == "aprs" || l == "aprs-live" || l == "aprs-watch") return "aprs_live";
  if (l == "wx" || l == "weather" || l == "weather-live" || l == "weather-watch") return "weather_live";
  if (l == "hamqsl" || l == "ham-qsl" || l == "hamqsl-live" || l == "hamqsl-watch" || l == "solar" || l == "propagation") return "hamqsl_live";

  return "mmdvm_live";
}

bool isValidBackendValue(const String& rawBackend) {
  return canonicalBackendValue(rawBackend).length() > 0;
}

String normalizedBackend() {
  String canonical = canonicalBackendValue(cfg.backend);
  if (canonical == "WPSD") return "wpsd";
  return "pistar";
}

String normalizedSvxlinkSource() {
  String canonical = canonicalSvxlinkSourceValue(cfg.svxlinkSource);
  return canonical.length() ? canonical : String("url");
}

bool svxlinkFeatureEnabled() {
  return cfg.svxlinkEnabled && trimSafe(cfg.svxlinkHost).length() > 0;
}

bool isCurrentWatchSvxlink() {
  return currentWatchSource == WATCH_SVXLINK && svxlinkFeatureEnabled();
}

bool isSvxlinkBackend() {
  return isCurrentWatchSvxlink();
}

bool aprsFeatureEnabled() {
  return cfg.aprsEnabled && trimSafe(cfg.aprsApiKey).length() > 0;
}

bool isCurrentWatchAprs() {
  return currentWatchSource == WATCH_APRS && aprsFeatureEnabled();
}

bool isAprsBackend() {
  return isCurrentWatchAprs();
}

bool weatherFeatureEnabled() {
  return cfg.weatherEnabled;
}

bool isCurrentWatchWeather() {
  return currentWatchSource == WATCH_WEATHER && weatherFeatureEnabled();
}

bool isWeatherBackend() {
  return isCurrentWatchWeather();
}

bool hamqslFeatureEnabled() {
  return cfg.hamqslEnabled;
}

bool isCurrentWatchHamqsl() {
  return currentWatchSource == WATCH_HAMQSL && hamqslFeatureEnabled();
}

bool isHamqslBackend() {
  return isCurrentWatchHamqsl();
}

bool startPageAfterBootOptionsAvailable() {
  return cfg.svxlinkEnabled || cfg.aprsEnabled || cfg.weatherEnabled || cfg.hamqslEnabled;
}

bool startPageAfterBootChoiceAvailable(const String& value, bool requireRuntimeReady) {
  String canonical = canonicalStartPageAfterBootValue(value);
  if (canonical == "mmdvm_live" || canonical == "mmdvm_last") return true;
  if (canonical == "svxlink_live" || canonical == "svxlink_last") return requireRuntimeReady ? svxlinkFeatureEnabled() : cfg.svxlinkEnabled;
  if (canonical == "aprs_live") return requireRuntimeReady ? aprsFeatureEnabled() : cfg.aprsEnabled;
  if (canonical == "weather_live") return requireRuntimeReady ? weatherConfigured() : cfg.weatherEnabled;
  if (canonical == "hamqsl_live") return requireRuntimeReady ? hamqslConfigured() : cfg.hamqslEnabled;
  return false;
}

String normalizedStartPageAfterBoot(bool requireRuntimeReady) {
  String canonical = canonicalStartPageAfterBootValue(cfg.startPageAfterBoot);
  if (!startPageAfterBootChoiceAvailable(canonical, requireRuntimeReady)) canonical = "mmdvm_live";
  return canonical;
}

String startPageAfterBootLabel(const String& value) {
  String canonical = canonicalStartPageAfterBootValue(value);
  if (canonical == "mmdvm_last") return "MMDVM HEARD";
  if (canonical == "svxlink_live") return "SVXLink LIVE";
  if (canonical == "svxlink_last") return "SVXLink HEARD";
  if (canonical == "aprs_live") return "APRS WATCH";
  if (canonical == "weather_live") return "Weather WATCH";
  if (canonical == "hamqsl_live") return "HAMQSL WATCH";
  return "MMDVM LIVE";
}

bool isSvxlinkApiBackend() {
  return normalizedSvxlinkSource() == "api";
}

bool isSvxlinkUrlBackend() {
  return normalizedSvxlinkSource() == "url";
}

String defaultSvxlinkPathForBackend() {
  return isSvxlinkApiBackend() ? String("/api/display-json.php") : String("/index.php");
}

String normalizedSvxlinkPathForBackend(const String& rawPath) {
  String path = trimSafe(rawPath);
  if (!path.length()) path = defaultSvxlinkPathForBackend();
  if (!path.startsWith("/")) path = "/" + path;
  if (isSvxlinkApiBackend() && path == "/index.php") path = "/api/display-json.php";
  if (isSvxlinkUrlBackend() && (path == "/api/display-json.php" || path == "/display-json.php")) path = "/index.php";
  return path;
}

String activeBackendHost() {
  if (isCurrentWatchSvxlink()) return trimSafe(cfg.svxlinkHost);
  return trimSafe(cfg.pistarHost);
}

String displayBackendHost() {
  String host = trimSafe(activeBackendHost());
  String lower = host;
  lower.toLowerCase();
  if (lower.startsWith("http://")) {
    host = host.substring(7);
  } else if (lower.startsWith("https://")) {
    host = host.substring(8);
  }
  while (host.endsWith("/") && host.length() > 1) host.remove(host.length() - 1);
  return host;
}

String heardUrl() {
  if (normalizedBackend() == "wpsd") {
    return "http://" + cfg.pistarHost + "/api/?limit=12&names=true&country=true";
  }
  return "http://" + cfg.pistarHost + "/api/last_heard.php";
}


String dashboardStatusUrl() {
  return "http://" + cfg.pistarHost + "/mmdvmhost/repeaterinfo.php";
}

String dashboardFreqUrl() {
  if (normalizedBackend() == "wpsd") {
    return "http://" + cfg.pistarHost + "/mmdvmhost/radioinfo.php";
  }
  return "http://" + cfg.pistarHost + "/mmdvmhost/repeaterinfo.php";
}

bool isAbsoluteHttpUrl(const String& rawUrl) {
  String u = trimSafe(rawUrl);
  u.toLowerCase();
  return u.startsWith("http://") || u.startsWith("https://");
}

String ensureLeadingSlash(const String& rawPath) {
  String path = trimSafe(rawPath);
  if (!path.length()) return "/";
  if (!path.startsWith("/")) path = "/" + path;
  return path;
}

String stripTrailingSlash(const String& rawUrl) {
  String u = trimSafe(rawUrl);
  while (u.endsWith("/") && u.length() > 8) u.remove(u.length() - 1);
  return u;
}

String svxlinkApiUrl() {
  String hostOrUrl = activeBackendHost();
  if (!hostOrUrl.length()) return "";

  String path = normalizedSvxlinkPathForBackend(cfg.svxlinkApiPath);

  if (isAbsoluteHttpUrl(hostOrUrl)) {
    String full = trimSafe(hostOrUrl);
    int schemeEnd = full.indexOf("://");
    int pathStart = (schemeEnd >= 0) ? full.indexOf('/', schemeEnd + 3) : -1;

    if (pathStart < 0) {
      return stripTrailingSlash(full) + path;
    }

    if (isSvxlinkApiBackend() && (path == "/api/display-json.php")) {
      String base = full.substring(0, pathStart);
      return stripTrailingSlash(base) + path;
    }

    return full;
  }

  return "http://" + hostOrUrl + path;
}


String aprsApiUrl(const String& callsign) {
  String call = normalizeHamCallsign(callsign);
  String key = trimSafe(cfg.aprsApiKey);
  if (!call.length() || !key.length()) return "";
  return String("https://api.aprs.fi/api/get?name=") + urlEncodeComponent(call) +
         "&what=loc&apikey=" + urlEncodeComponent(key) + "&format=json";
}

String aprsLookupCallsign() {
  String call = normalizeHamCallsign(liveRecord.callsign);
  if (call.length()) return call;
  if (heardCount > 0) {
    call = normalizeHamCallsign(heardList[0].callsign);
    if (call.length()) return call;
  }
  return normalizeHamCallsign(getDeviceCallsign());
}

String aprsFormatEpoch(const String& epochText) {
  String raw = trimSafe(epochText);
  if (!raw.length()) return "";
  unsigned long epochValue = strtoul(raw.c_str(), nullptr, 10);
  if (!epochValue) return raw;

  time_t t = (time_t)epochValue;
  struct tm tmValue;
  if (cfg.apiUtcToLocalTime) localtime_r(&t, &tmValue);
  else gmtime_r(&t, &tmValue);
  return formatApiDateTime(tmValue);
}

String aprsIconPath(const String& symbol) {
  String raw = trimSafe(symbol);
  if (raw.length() < 2) return "";

  char tableChar = raw.charAt(0);
  char symbolChar = raw.charAt(1);
  const char* rows[] = {
    "!\"#$%&'()*+,-./0",
    "123456789:;<=>?@",
    "ABCDEFGHIJKLMNOP",
    "QRSTUVWXYZ[\\]^_`",
    "abcdefghijklmnop",
    "qrstuvwxyz{|}~"
  };

  int row = -1;
  int col = -1;
  for (uint8_t r = 0; r < 6 && row < 0; ++r) {
    const char* found = strchr(rows[r], symbolChar);
    if (found) {
      row = r;
      col = (int)(found - rows[r]);
    }
  }
  if (row < 0 || col < 0) return "";

  if (tableChar != '/') row += 6;

  String path = "/aprs/" + String(col) + String(row) + ".png";
  return storageExists(path.c_str()) ? path : "";
}

bool drawAprsIcon(const String& symbol, int x, int y, uint16_t bg565) {
  String icon = aprsIconPath(symbol);
  if (icon.length()) return drawPngFromSD(icon.c_str(), x, y, 32, 32, bg565);

  tft.fillCircle(x + 16, y + 16, 10, bg565);
  tft.drawCircle(x + 16, y + 16, 10, theme.border);
  tft.fillCircle(x + 16, y + 16, 4, theme.accent);
  return false;
}

bool isLiveActive() {
  if (!liveRecord.valid) return false;
  return (millis() - liveRecordMillis) <= cfg.callsignHoldMs;
}

String liveStatusText(bool active, const HeardRecord& rec) {
  String status = trimSafe(hotspotStatusText);
  if (status.length()) {
    if (status.equalsIgnoreCase("Listening")) {
      String mode = normalizeModeName(rec.mode);
      if (active && mode.length()) {
        String ts = dmrTimeslotLabel(rec);
        return "Mode: " + mode + (ts.length() ? String(" ") + ts : String(""));
      }
      return "Listening";
    }
    return status;
  }

  String mode = normalizeModeName(rec.mode);
  if (active && mode.length()) {
    String ts = dmrTimeslotLabel(rec);
    return "Mode: " + mode + (ts.length() ? String(" ") + ts : String(""));
  }
  return active ? "RX ACTIVE" : "MMDVM IDLE";
}

void setDefaultTheme() {
  theme.bg        = TFT_BLACK;
  theme.panel     = 0x18C3;
  theme.panel2    = 0x2104;
  theme.border    = TFT_DARKGREY;
  theme.text      = TFT_WHITE;
  theme.subtext   = TFT_LIGHTGREY;
  theme.accent    = TFT_CYAN;
  theme.good      = TFT_GREEN;
  theme.warn      = TFT_YELLOW;
  theme.bad       = TFT_RED;
  theme.live      = 0xFBE0;
  theme.rx        = TFT_GREEN;
  theme.tx        = TFT_BLUE;
  theme.tabIdle   = 0x39C7;
  theme.tabActive = 0x059F;
}

uint16_t mix565(uint16_t a, uint16_t b, float t) {
  t = constrain(t, 0.0f, 1.0f);

  uint8_t ar = (a >> 11) & 0x1F;
  uint8_t ag = (a >> 5)  & 0x3F;
  uint8_t ab = a & 0x1F;

  uint8_t br = (b >> 11) & 0x1F;
  uint8_t bg = (b >> 5)  & 0x3F;
  uint8_t bb = b & 0x1F;

  uint8_t rr = ar + (int)((br - ar) * t);
  uint8_t rg = ag + (int)((bg - ag) * t);
  uint8_t rb = ab + (int)((bb - ab) * t);

  return (rr << 11) | (rg << 5) | rb;
}

void applyBaseThemeColor(const String& baseColorHex) {
  uint16_t base;
  if (!parseHexColor(baseColorHex, base)) return;

  theme.accent    = base;
  theme.tabActive = base;
  theme.warn      = mix565(base, TFT_YELLOW, 0.35f);
  theme.good      = mix565(base, TFT_GREEN, 0.60f);
  theme.bad       = mix565(base, TFT_RED, 0.65f);
  theme.live      = mix565(base, TFT_WHITE, 0.40f);
  theme.tx        = mix565(base, TFT_BLUE, 0.55f);
  theme.rx        = mix565(base, TFT_GREEN, 0.45f);
  theme.panel     = mix565(TFT_BLACK, base, 0.16f);
  theme.panel2    = mix565(TFT_BLACK, base, 0.10f);
  theme.border    = mix565(base, TFT_WHITE, 0.22f);
  theme.tabIdle   = mix565(TFT_BLACK, base, 0.24f);
  theme.subtext   = mix565(TFT_LIGHTGREY, base, 0.08f);
}

void fillSoftPanel(int x, int y, int w, int h, uint16_t fill, uint16_t border, int radius = 10) {
  int rr = max(1, UR(radius));
  tft.fillRoundRect(x, y, w, h, rr, fill);
  tft.drawRoundRect(x, y, w, h, rr, border);
  if (w > 2 && h > 2) {
    tft.drawRoundRect(x + 1, y + 1, w - 2, h - 2, rr, mix565(fill, TFT_WHITE, 0.10f));
  }
}

void drawInfoRow(int x, int y, int w, const String& label, const String& value,
                 uint16_t labelBg, uint16_t valueBg, uint16_t labelColor, uint16_t valueColor) {
  const int labelW = UW(76);
  const int rowH = UH(22);
  const int gap = UW(4);
  const int valueX = x + labelW + gap;

  fillSoftPanel(x, y, labelW, rowH, labelBg, theme.border, 5);
  fillSoftPanel(valueX, y, w - labelW - gap, rowH, valueBg, theme.border, 5);

  tft.setTextDatum(ML_DATUM);
  tft.setTextFont(2);

  tft.setTextColor(labelColor, labelBg);
  tft.drawString(label, x + UW(8), y + rowH / 2);

  tft.setTextColor(valueColor, valueBg);
  String clipped = value;
  int maxChars = max(4, (w - labelW - UW(16)) / 6);
  if (maxChars > 4 && clipped.length() > maxChars) clipped = clipped.substring(0, maxChars - 1) + "~";
  tft.drawString(clipped, valueX + UW(8), y + rowH / 2);
}

String fitTextInCurrentFont(const String& input, int maxWidth);
String fitTextSliceInCurrentFont(const String& input, int maxWidth);

void drawInfoRowSized(int x, int y, int w, int labelW, const String& label, const String& value,
                      uint16_t labelBg, uint16_t valueBg, uint16_t labelColor, uint16_t valueColor) {
  const int rowH = UH(22);
  const int gap = UW(4);
  labelW = constrain(labelW, UW(28), max(UW(28), w - gap - UW(36)));
  const int valueX = x + labelW + gap;
  const int valueW = max(UW(24), w - labelW - gap);

  fillSoftPanel(x, y, labelW, rowH, labelBg, theme.border, 5);
  fillSoftPanel(valueX, y, valueW, rowH, valueBg, theme.border, 5);

  tft.setTextDatum(ML_DATUM);
  tft.setTextFont(2);

  tft.setTextColor(labelColor, labelBg);
  tft.drawString(fitTextInCurrentFont(label, max(UW(8), labelW - UW(12))), x + UW(6), y + rowH / 2);

  tft.setTextColor(valueColor, valueBg);
  tft.drawString(fitTextInCurrentFont(value.length() ? value : String("-"), max(UW(8), valueW - UW(14))), valueX + UW(7), y + rowH / 2);
}

String normalizeSingleLineText(const String& input) {
  String value = trimSafe(input);
  value.replace("\r", " ");
  value.replace("\n", " ");
  while (value.indexOf("  ") >= 0) value.replace("  ", " ");
  return trimSafe(value);
}

void drawWrappedTextInCurrentFont(const String& input, int x, int y, int maxWidth, int maxLines, int lineHeight) {
  String text = normalizeSingleLineText(input);
  if (!text.length()) text = "-";
  if (maxWidth <= 0 || maxLines <= 0) return;

  int pos = 0;
  const int len = text.length();

  for (int line = 0; line < maxLines; line++) {
    while (pos < len && text.charAt(pos) == ' ') pos++;
    if (pos >= len) return;

    if (line == maxLines - 1) {
      tft.drawString(fitTextInCurrentFont(text.substring(pos), maxWidth), x, y + line * lineHeight);
      return;
    }

    String out = "";
    while (pos < len) {
      while (pos < len && text.charAt(pos) == ' ') pos++;
      if (pos >= len) break;

      int nextSpace = text.indexOf(' ', pos);
      if (nextSpace < 0) nextSpace = len;
      String word = text.substring(pos, nextSpace);
      String candidate = out.length() ? (out + " " + word) : word;

      if (tft.textWidth(candidate) <= maxWidth) {
        out = candidate;
        pos = nextSpace + 1;
        continue;
      }

      if (!out.length()) {
        out = fitTextSliceInCurrentFont(word, maxWidth);
        pos += out.length();
      }
      break;
    }

    if (!out.length()) return;
    tft.drawString(out, x, y + line * lineHeight);
  }
}

String fitTextInCurrentFont(const String& input, int maxWidth) {
  String value = trimSafe(input);
  if (!value.length()) value = "-";
  if (maxWidth <= 0 || tft.textWidth(value) <= maxWidth) return value;

  while (value.length() > 1) {
    value.remove(value.length() - 1);
    String trial = value + "~";
    if (tft.textWidth(trial) <= maxWidth) return trial;
  }

  return String("~");
}

String fitTextSliceInCurrentFont(const String& input, int maxWidth) {
  String out = "";
  if (maxWidth <= 0) return out;

  for (size_t i = 0; i < input.length(); i++) {
    String trial = out + input.charAt(i);
    if (trial.length() && tft.textWidth(trial) > maxWidth) break;
    out = trial;
  }

  while (out.length() && tft.textWidth(out) > maxWidth) {
    out.remove(out.length() - 1);
  }

  return out;
}

String scrollingTextFrameInCurrentFont(const String& input, int maxWidth) {
  String value = trimSafe(input);
  if (!value.length()) value = "-";
  if (maxWidth <= 0) return "";
  if (tft.textWidth(value) <= maxWidth) return value;
  if (!cfg.scrollingTextEnabled) return fitTextInCurrentFont(value, maxWidth);

  scrollingTextActiveFrame = true;

  const String gap = "   ";
  String loopText = value + gap;
  unsigned long cycle = (unsigned long)loopText.length();
  if (!cycle) return "";

  int offset = (int)((millis() / SCROLL_TEXT_STEP_MS) % cycle);
  String rotated = loopText.substring(offset) + loopText.substring(0, offset) + value + gap;
  return fitTextSliceInCurrentFont(rotated, maxWidth);
}

void drawScrollingTextInCurrentFont(const String& value, int x, int y, int maxWidth) {
  tft.drawString(scrollingTextFrameInCurrentFont(value, maxWidth), x, y);
}

void drawCompactValueBox(int x, int y, int w, int h,
                         const String& label, const String& value,
                         int labelFont, int valueFont, int valueXOffset,
                         uint16_t labelColor, uint16_t valueColor,
                         bool scrollValue = false) {
  fillSoftPanel(x, y, w, h, theme.panel2, theme.border, 6);

  const int centerY = y + h / 2;
  tft.setTextDatum(ML_DATUM);

  String boxLabel = trimSafe(label);
  int valueX = x + valueXOffset;
  int valueW = max(UW(12), w - valueXOffset - UW(6));

  if (boxLabel.length()) {
    tft.setTextFont(labelFont);
    tft.setTextColor(labelColor, theme.panel2);
    tft.drawString(boxLabel, x + UW(8), centerY);
  } else {
    valueX = x + UW(8);
    valueW = max(UW(12), w - UW(16));
  }

  tft.setTextFont(valueFont);
  tft.setTextColor(valueColor, theme.panel2);
  String shownValue = scrollValue ? scrollingTextFrameInCurrentFont(value, valueW) : fitTextInCurrentFont(value, valueW);
  tft.drawString(shownValue, valueX, centerY);
}

void drawLiveInfoPanel(int x, int y, int w, int h,
                       const String& callsign, const String& status, const String& target,
                       bool active) {
  fillSoftPanel(x, y, w, h, theme.panel2, theme.border, 10);

  const int flagW = UW(32);
  const int flagH = UH(32);
  const int rowY = y + UH(30);

  String caller = trimSafe(callsign);
  if (!caller.length()) caller = "-";

  String flagCallsign = caller;
  int callerSeparator = flagCallsign.indexOf(" - ");
  if (callerSeparator > 0) flagCallsign = flagCallsign.substring(0, callerSeparator);

  int textX = x + UW(58);
  int availableTextW = max(UW(36), w - (textX - x) - UW(10));
  int callerFont = 4;
  caller = fitLivePanelPrimaryText(caller, availableTextW, callerFont);

  int flagX = x + UW(14);
#if DISPLAY_BACKEND_IS_GFX_COMPAT
  int flagY = y + UH(-4);
#else
  int flagY = y + UH(4);
#endif
  drawFlagForCallsign(flagX, flagY, flagW, flagH, flagCallsign);

  tft.setTextFont(callerFont);
  tft.setTextDatum(ML_DATUM);
  tft.setTextColor(active ? theme.warn : theme.text, theme.panel2);
#if DISPLAY_BACKEND_IS_GFX_COMPAT
  int callerTextY = rowY - (callerFont >= 4 ? UH(10) : (callerFont == 2 ? UH(9) : UH(7)));
#else
  int callerTextY = rowY - (callerFont >= 4 ? UH(8) : (callerFont == 2 ? UH(7) : UH(5)));
#endif
  tft.drawString(caller, textX, callerTextY);

  int metaY = y + h - UH(28);
  int statusW = (w - UW(22)) / 2;
  int targetW = w - UW(22) - statusW;
  int statusX = x + UW(8);
  int targetX = statusX + statusW + UW(6);
  int metaH = UH(24);

  String statusText = status.length() ? status : "-";
  String targetText = target.length() ? target : "-";

  drawCompactValueBox(statusX, metaY, statusW, metaH, "", statusText,
                      1, 2, UW(8), theme.warn, active ? theme.good : theme.text, true);
  drawCompactValueBox(targetX, metaY, targetW, metaH, "", targetText,
                      1, 2, UW(8), theme.warn, theme.text, true);

  tft.setTextDatum(TL_DATUM);
}

void drawDmrTimeslotRow(int x, int y, int w, int h, uint8_t slot) {
  const HeardRecord& rec = dmrTimeslotRecord(slot);
  bool slotActive = isDmrTimeslotLiveActive(slot);
  uint16_t rowFill = slotActive ? mix565(theme.panel2, theme.live, 0.12f) : theme.panel2;
  uint16_t rowBorder = slotActive ? theme.warn : theme.border;

  fillSoftPanel(x, y, w, h, rowFill, rowBorder, 8);

  tft.setTextDatum(ML_DATUM);
  tft.setTextFont(2);
  tft.setTextColor(slotActive ? theme.warn : theme.subtext, rowFill);
  tft.drawString(String("TS") + String(slot), x + UW(8), y + h / 2);

  String caller = slotActive ? normalizeHamCallsign(rec.callsign) : String("Idle");
  String target = slotActive ? resolveTargetDisplay(rec) : String("-");
  if (!caller.length()) caller = "-";
  if (!target.length()) target = "-";

  tft.setTextColor(slotActive ? theme.text : theme.subtext, rowFill);
  tft.drawString(clipWithTilde(caller, 11), x + UW(46), y + h / 2);

  tft.setTextColor(slotActive ? theme.good : theme.subtext, rowFill);
  tft.drawString(clipWithTilde(target, 15), x + UW(142), y + h / 2);

  tft.setTextDatum(TL_DATUM);
}

void drawDmrTimeslotLivePanel(int x, int y, int w, int h) {
  fillSoftPanel(x, y, w, h, theme.panel2, theme.border, 10);

  const int gap = UH(4);
  const int rowH = (h - gap - UH(8)) / 2;
  const int rowX = x + UW(6);
  const int rowW = w - UW(12);
  const int rowY1 = y + UH(4);
  const int rowY2 = rowY1 + rowH + gap;

  drawDmrTimeslotRow(rowX, rowY1, rowW, rowH, 1);
  drawDmrTimeslotRow(rowX, rowY2, rowW, rowH, 2);
}

void drawModeButton(int x, int y, int w, int h, const String& label, const ModeStatus& st, bool active) {
  uint16_t textColor = modeColor(st, active);
  uint16_t fill = theme.panel2;
  uint16_t border = active ? mix565(textColor, TFT_WHITE, 0.20f) : theme.border;
  fillSoftPanel(x, y, w, h, fill, border, 8);
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(textColor, fill);
  tft.setTextFont(2);
  tft.drawString(label, x + w / 2, y + h / 2);
  tft.setTextDatum(TL_DATUM);
}

bool pointInRect(int px, int py, int x, int y, int w, int h) {
  return px >= x && px < (x + w) && py >= y && py < (y + h);
}

ModeStatus* modeStatusFromTouchLabel(const String& label) {
  if (label == "DMR") return &modeDMR;
  if (label == "DSTAR") return &modeDSTAR;
  if (label == "YSF") return &modeYSF;
  if (label == "P25") return &modeP25;
  if (label == "NXDN") return &modeNXDN;
  return nullptr;
}

uint8_t quickActionSlotLimit(bool svxlinkAction) {
  return svxlinkAction ? SVXLINK_ACTION_SLOT_COUNT : QUICK_ACTION_SLOT_COUNT;
}

bool isQuickActionSlotConfigured(uint8_t slot, bool svxlinkAction) {
  if (slot >= quickActionSlotLimit(svxlinkAction)) return false;
  const QuickActionSlotConfig& qa = svxlinkAction ? cfg.svxlinkActions[slot] : cfg.quickActions[slot];
  return qa.enabled && trimSafe(qa.label).length() && trimSafe(qa.command).length();
}

bool hasConfiguredQuickActions(bool svxlinkAction) {
  uint8_t limit = quickActionSlotLimit(svxlinkAction);
  for (uint8_t i = 0; i < limit; i++) {
    if (isQuickActionSlotConfigured(i, svxlinkAction)) return true;
  }
  return false;
}

String quickActionSlotLabel(uint8_t slot, bool svxlinkAction) {
  if (slot >= quickActionSlotLimit(svxlinkAction)) return String("");
  const QuickActionSlotConfig& qa = svxlinkAction ? cfg.svxlinkActions[slot] : cfg.quickActions[slot];
  String label = trimSafe(qa.label);
  if (!label.length()) label = svxlinkAction ? (String("DTMF ") + String(slot + 1)) : (String("Action ") + String(slot + 1));
  return label;
}

String buildWrappedRemoteShellCommand(const String& rawCommand, bool useSudo, const String& sshUser, bool& sendSudoPassword) {
  sendSudoPassword = false;
  String wrapped = "/bin/sh -c " + shellSingleQuote(rawCommand);
  if (useSudo && trimSafe(sshUser) != "root") {
    sendSudoPassword = true;
    return "sudo -S -p '' " + wrapped;
  }
  return wrapped;
}

String buildQuickActionRemoteCommand(uint8_t slot, bool svxlinkAction, bool& sendSudoPassword, String* errMsg = nullptr) {
  sendSudoPassword = false;
  if (slot >= quickActionSlotLimit(svxlinkAction)) {
    if (errMsg) *errMsg = "Invalid quick action slot.";
    return String("");
  }

  const QuickActionSlotConfig& qa = svxlinkAction ? cfg.svxlinkActions[slot] : cfg.quickActions[slot];
  if (!qa.enabled) {
    if (errMsg) *errMsg = "Quick action is disabled.";
    return String("");
  }

  String command = trimSafe(qa.command);
  if (!command.length()) {
    if (errMsg) *errMsg = "Quick action command is empty.";
    return String("");
  }

  return buildWrappedRemoteShellCommand(command, qa.useSudo, svxlinkAction ? cfg.svxlinkSshUser : cfg.pistarSshUser, sendSudoPassword);
}

String hostModeManagerModuleNameFromTouchLabel(const String& label) {
  if (label == "DSTAR") return "D-Star";
  return label;
}

String hostModeManagerPath() {
  return "/usr/local/sbin/pistar-mmdvmhost-module";
}

String hostModeManagerAction(bool enable) {
  return enable ? "Enable" : "Disable";
}

String wpsdGatewayServiceNameFromModuleName(const String& moduleName) {
  String m = normalizeModeName(moduleName);
  if (m == "DSTAR") return "ircddb";
  if (m == "DMR") return "dmr";
  if (m == "YSF") return "ysf";
  if (m == "P25") return "p25";
  if (m == "NXDN") return "nxdn";
  if (m == "POCSAG") return "dapnet";
  return String("");
}

String buildWpsdModeManagerShellCommand(const String& moduleName, const String& action, bool enable) {
  String moduleArg = shellSingleQuote(moduleName);
  String actionArg = shellSingleQuote(action);
  String primaryTool = "/usr/local/sbin/pistar-mmdvmhost-module";
  String fallbackTool = "/usr/local/sbin/wpsd-mode-manager";
  String service = wpsdGatewayServiceNameFromModuleName(moduleName);

  String toolCmd = "if [ -x " + primaryTool + " ]; then " + primaryTool + " " + moduleArg + " " + actionArg + "; ";
  toolCmd += "elif [ -x " + fallbackTool + " ]; then " + fallbackTool + " " + moduleArg + " " + actionArg + "; ";
  toolCmd += "else echo 'No WPSD/Pi-Star mode manager tool found'; exit 127; fi";

  String script;
  if (service.length() && !enable) {
    script += "systemctl stop " + service + "gateway.timer " + service + "gateway.service pistar-watchdog.timer pistar-watchdog.service >/dev/null 2>&1 || true; ";
  }

  script += toolCmd;

  if (service.length() && enable) {
    script += "; systemctl start " + service + "gateway.timer " + service + "gateway.service >/dev/null 2>&1 || true";
    script += "; if ! ls /etc/*_paused >/dev/null 2>&1; then systemctl start pistar-watchdog.timer pistar-watchdog.service >/dev/null 2>&1 || true; fi";
  }

  return "/bin/sh -c " + shellSingleQuote(script);
}

String buildHostModeManagerCommand(const String& label, bool enable) {
  String moduleName = hostModeManagerModuleNameFromTouchLabel(label);
  String action = hostModeManagerAction(enable);
  String baseCmd;

  if (normalizedBackend() == "wpsd") {
    baseCmd = buildWpsdModeManagerShellCommand(moduleName, action, enable);
  } else {
    baseCmd = hostModeManagerPath() + " " + shellSingleQuote(moduleName) + " " + shellSingleQuote(action);
  }

  String sshUser = trimSafe(cfg.pistarSshUser);
  if (!sshUser.length() || sshUser == "root") {
    return baseCmd;
  }

  return "sudo -S -p '' " + baseCmd;
}

bool liveModeButtonTouchHit(int x, int y, String& label) {
  struct Btn { const char* label; int x; int y; int w; int h; };
  const Btn buttons[] = {
    {"DMR",   UX(10), UY(38), UW(70), UH(20)},
    {"DSTAR", UX(86), UY(38), UW(70), UH(20)},
    {"YSF",   UX(10), UY(62), UW(70), UH(20)},
    {"P25",   UX(86), UY(62), UW(70), UH(20)},
    {"NXDN",  UX(10), UY(86), UW(70), UH(20)}
  };

  for (const auto& b : buttons) {
    if (pointInRect(x, y, b.x, b.y, b.w, b.h)) {
      label = b.label;
      return true;
    }
  }
  return false;
}

void getModeConfirmPopupLayout(int& popupX, int& popupY, int& popupW, int& popupH,
                               int& noX, int& noY, int& noW, int& noH,
                               int& yesX, int& yesY, int& yesW, int& yesH) {
  popupW = UW(240);
  popupH = UH(112);
  popupX = (SCREEN_W - popupW) / 2;
  popupY = (SCREEN_H - popupH) / 2;
  noW = UW(84);
  noH = UH(28);
  yesW = UW(84);
  yesH = UH(28);
  noX = popupX + UW(22);
  yesX = popupX + popupW - yesW - UW(22);
  noY = popupY + popupH - noH - UH(18);
  yesY = noY;
}

void openModeConfirmPopup(const String& label) {
  if (!cfg.touchModeSshEnabled) {
    setFooterText("Enable touch SSH mode control in the config page first.");
    return;
  }

  ModeStatus* st = modeStatusFromTouchLabel(label);
  if (!st || !st->known) {
    setFooterText("Cannot toggle " + label + " because its current state is unknown.");
    return;
  }

  modeConfirmTargetLabel = label;
  modeConfirmEnableRequested = !st->enabled;
  modeConfirmPopupOpen = true;
  requestRedraw();
}

#if ENABLE_PISTAR_TOUCH_SSH
static bool sshWriteAll(ssh_channel channel, const char* data, size_t len) {
  size_t sent = 0;
  while (sent < len) {
    int rc = ssh_channel_write(channel, data + sent, len - sent);
    if (rc <= 0) return false;
    sent += (size_t)rc;
    yield();
  }
  return true;
}

String sshHostFromHostOrUrl(const String& rawHost) {
  String host = trimSafe(rawHost);
  if (!host.length()) return "";
  int schemeEnd = host.indexOf("://");
  if (schemeEnd >= 0) host = host.substring(schemeEnd + 3);
  int slash = host.indexOf('/');
  if (slash >= 0) host = host.substring(0, slash);
  int colon = host.indexOf(':');
  if (colon > 0) host = host.substring(0, colon);
  return trimSafe(host);
}

bool runSshCommandWithLogin(const String& rawHost, const String& sshUser, const String& sshPass, uint16_t sshPort,
                            const String& command, bool sendSudoPassword, String* output, String* errMsg) {
  String cleanHost = sshHostFromHostOrUrl(rawHost);
  if (!cleanHost.length()) {
    if (errMsg) *errMsg = "Missing SSH host.";
    return false;
  }
  if (!trimSafe(sshUser).length()) {
    if (errMsg) *errMsg = "Missing SSH username.";
    return false;
  }
  if (!sshPass.length()) {
    if (errMsg) *errMsg = "Missing SSH password.";
    return false;
  }

  NetworkGuard net(NETWORK_LOCK_TIMEOUT_MS);
  if (!net) {
    if (errMsg) *errMsg = "Network is busy; try again.";
    return false;
  }

  ssh_session session = ssh_new();
  if (session == nullptr) {
    if (errMsg) *errMsg = "ssh_new failed.";
    return false;
  }

  const char* host = cleanHost.c_str();
  const char* user = sshUser.c_str();
  const char* pass = sshPass.c_str();
  int port = (sshPort > 0) ? sshPort : 22;
  long timeoutSec = 6;
  int strictHostKey = 0;
  int verbosity = SSH_LOG_NOLOG;
  bool needSudoPassword = sendSudoPassword && trimSafe(sshUser) != "root";

  ssh_options_set(session, SSH_OPTIONS_HOST, host);
  ssh_options_set(session, SSH_OPTIONS_USER, user);
  ssh_options_set(session, SSH_OPTIONS_PORT, &port);
  ssh_options_set(session, SSH_OPTIONS_TIMEOUT, &timeoutSec);
  ssh_options_set(session, SSH_OPTIONS_STRICTHOSTKEYCHECK, &strictHostKey);
  ssh_options_set(session, SSH_OPTIONS_LOG_VERBOSITY, &verbosity);

  int rc = ssh_connect(session);
  if (rc != SSH_OK) {
    if (errMsg) *errMsg = String("SSH connect failed: ") + ssh_get_error(session);
    ssh_free(session);
    return false;
  }

  rc = ssh_userauth_password(session, nullptr, pass);
  if (rc != SSH_AUTH_SUCCESS) {
    if (errMsg) *errMsg = String("SSH auth failed: ") + ssh_get_error(session);
    ssh_disconnect(session);
    ssh_free(session);
    return false;
  }

  ssh_channel channel = ssh_channel_new(session);
  if (channel == nullptr) {
    if (errMsg) *errMsg = "SSH channel allocation failed.";
    ssh_disconnect(session);
    ssh_free(session);
    return false;
  }

  rc = ssh_channel_open_session(channel);
  if (rc != SSH_OK) {
    if (errMsg) *errMsg = String("SSH channel open failed: ") + ssh_get_error(session);
    ssh_channel_free(channel);
    ssh_disconnect(session);
    ssh_free(session);
    return false;
  }

  rc = ssh_channel_request_exec(channel, command.c_str());
  if (rc != SSH_OK) {
    if (errMsg) *errMsg = String("SSH exec failed: ") + ssh_get_error(session);
    ssh_channel_close(channel);
    ssh_channel_free(channel);
    ssh_disconnect(session);
    ssh_free(session);
    return false;
  }

  if (needSudoPassword) {
    String passLine = sshPass + "\n";
    sshWriteAll(channel, passLine.c_str(), passLine.length());
  }
  ssh_channel_send_eof(channel);

  String out;
  char buffer[160];
  int n = 0;
  do {
    n = ssh_channel_read(channel, buffer, sizeof(buffer) - 1, 0);
    if (n > 0) {
      buffer[n] = 0;
      if (out.length() < 512) out += String(buffer);
    }
    yield();
  } while (n > 0);

  do {
    n = ssh_channel_read(channel, buffer, sizeof(buffer) - 1, 1);
    if (n > 0) {
      buffer[n] = 0;
      if (out.length() < 512) out += String(buffer);
    }
    yield();
  } while (n > 0);

  unsigned long waitStartMs = millis();
  while (!ssh_channel_is_eof(channel) && millis() - waitStartMs < 10000UL) {
    delay(5);
    yield();
  }

  int exitStatus = ssh_channel_get_exit_status(channel);
  ssh_channel_close(channel);
  ssh_channel_free(channel);
  ssh_disconnect(session);
  ssh_free(session);

  if (output) *output = out;
  if (exitStatus != 0) {
    if (errMsg) *errMsg = "Remote command exit status " + String(exitStatus) + ".";
    return false;
  }
  return true;
}

bool runHotspotSshCommand(const String& command, bool sendSudoPassword, String* output, String* errMsg) {
  return runSshCommandWithLogin(cfg.pistarHost, cfg.pistarSshUser, cfg.pistarSshPass, cfg.pistarSshPort,
                                command, sendSudoPassword, output, errMsg);
}

bool runSvxlinkSshCommand(const String& command, bool sendSudoPassword, String* output, String* errMsg) {
  return runSshCommandWithLogin(cfg.svxlinkHost, cfg.svxlinkSshUser, cfg.svxlinkSshPass, cfg.svxlinkSshPort,
                                command, sendSudoPassword, output, errMsg);
}

#endif


void modeToggleTaskEntry(void* pv) {
  ModeToggleTaskArgs* args = static_cast<ModeToggleTaskArgs*>(pv);
  String label = args ? args->label : String("");
  bool enable = args ? args->enable : false;
  delete args;

  String command = buildHostModeManagerCommand(label, enable);
  String output;
  String err;

#if ENABLE_PISTAR_TOUCH_SSH
  bool ok = runHotspotSshCommand(command, trimSafe(cfg.pistarSshUser) != "root", &output, &err);
#else
  bool ok = false;
  err = "SSH mode control is not compiled into this build.";
#endif

  String msg;
  if (!ok) {
    msg = trimSafe(err);
    if (!msg.length()) msg = "SSH command failed.";
  } else {
    msg = label + String(enable ? " enabled via SSH." : " disabled via SSH.");
  }

  strlcpy(modeToggleTaskLabel, label.c_str(), sizeof(modeToggleTaskLabel));
  strlcpy(modeToggleTaskMessage, msg.c_str(), sizeof(modeToggleTaskMessage));
  modeToggleTaskEnable = enable;
  modeToggleTaskOk = ok;
  modeToggleTaskDone = true;
  modeToggleTaskRunning = false;
  modeToggleTaskHandle = nullptr;
  vTaskDelete(nullptr);
}

bool startModeToggleTask(const String& label, bool enable) {
  if (modeToggleTaskRunning) {
    setFooterText("Wait for the current SSH action to finish.");
    return false;
  }

  ModeToggleTaskArgs* args = new ModeToggleTaskArgs{label, enable};
  if (!args) {
    setFooterText("Could not allocate SSH task memory.");
    return false;
  }

  modeToggleTaskDone = false;
  modeToggleTaskOk = false;
  modeToggleTaskEnable = enable;
  strlcpy(modeToggleTaskLabel, label.c_str(), sizeof(modeToggleTaskLabel));
  modeToggleTaskMessage[0] = 0;
  modeToggleTaskRunning = true;

  BaseType_t rc = xTaskCreatePinnedToCore(
    modeToggleTaskEntry,
    "modeToggleSSH",
    16384,
    args,
    1,
    &modeToggleTaskHandle,
    1
  );

  if (rc != pdPASS) {
    modeToggleTaskRunning = false;
    delete args;
    setFooterText("Could not start SSH task.");
    return false;
  }

  setFooterText(label + String(enable ? ": enabling..." : ": disabling..."));
  return true;
}

void processModeToggleTaskResult() {
  if (!modeToggleTaskDone) return;

  String label = String(modeToggleTaskLabel);
  bool enable = modeToggleTaskEnable;
  bool ok = modeToggleTaskOk;
  String msg = String(modeToggleTaskMessage);
  modeToggleTaskDone = false;

  if (ok) {
    ModeStatus* st = modeStatusFromTouchLabel(label);
    if (st) {
      st->known = true;
      st->enabled = enable;
    }
    nextFetchHeard = false;
    setFooterText(msg);
  } else {
    setFooterText(label + " " + (enable ? "enable" : "disable") + " failed: " + clipWithTilde(msg, 40));
  }

  requestRedraw();
}

bool runQuickActionSlotNow(uint8_t slot, String* userMsg = nullptr, bool svxlinkAction = false) {
  if (svxlinkAction) {
    if (!svxlinkFeatureEnabled()) {
      String msg = "Enable SVXLink and set its host first.";
      if (userMsg) *userMsg = msg;
      return false;
    }
    if (!cfg.svxlinkSshEnabled) {
      String msg = "Enable SVXLink SSH in the SVXLink page first.";
      if (userMsg) *userMsg = msg;
      return false;
    }
    if (!cfg.svxlinkHost.length()) {
      String msg = "Missing SVXLink host.";
      if (userMsg) *userMsg = msg;
      return false;
    }
  } else {
    if (!cfg.touchModeSshEnabled) {
      String msg = "Enable SSH remote control in the config page first.";
      if (userMsg) *userMsg = msg;
      return false;
    }
    if (!cfg.pistarHost.length()) {
      String msg = "Missing hotspot host.";
      if (userMsg) *userMsg = msg;
      return false;
    }
  }

  String buildErr;
  bool sendSudoPassword = false;
  String command = buildQuickActionRemoteCommand(slot, svxlinkAction, sendSudoPassword, &buildErr);
  String label = quickActionSlotLabel(slot, svxlinkAction);
  if (!command.length()) {
    String msg = buildErr.length() ? buildErr : String("Quick action is not configured.");
    if (userMsg) *userMsg = msg;
    return false;
  }

  String output;
  String sshErr;
#if ENABLE_PISTAR_TOUCH_SSH
  bool ok = svxlinkAction ? runSvxlinkSshCommand(command, sendSudoPassword, &output, &sshErr)
                           : runHotspotSshCommand(command, sendSudoPassword, &output, &sshErr);
#else
  bool ok = false;
  sshErr = "SSH remote control is not compiled into this build.";
#endif

  if (!ok) {
    String msg = trimSafe(sshErr);
    if (!msg.length()) msg = "SSH command failed.";
    if (userMsg) *userMsg = label + " failed: " + msg;
    return false;
  }

  if (userMsg) *userMsg = label + " completed.";
  lastPoll = 0;
  nextFetchHeard = false;
  return true;
}

void quickActionTaskEntry(void* pv) {
  QuickActionTaskArgs* args = static_cast<QuickActionTaskArgs*>(pv);
  uint8_t slot = args ? args->slot : 255;
  bool svxlinkAction = args ? args->svxlinkAction : false;
  delete args;

  String label = quickActionSlotLabel(slot, svxlinkAction);
  String msg;
  bool ok = runQuickActionSlotNow(slot, &msg, svxlinkAction);

  strlcpy(quickActionTaskLabel, label.c_str(), sizeof(quickActionTaskLabel));
  strlcpy(quickActionTaskMessage, msg.c_str(), sizeof(quickActionTaskMessage));
  quickActionTaskOk = ok;
  quickActionTaskDone = true;
  quickActionTaskRunning = false;
  quickActionTaskHandle = nullptr;
  vTaskDelete(nullptr);
}

bool startQuickActionTask(uint8_t slot, bool svxlinkAction) {
  if (quickActionTaskRunning || modeToggleTaskRunning) {
    setFooterText("Wait for the current SSH action to finish.");
    return false;
  }

  if (!isQuickActionSlotConfigured(slot, svxlinkAction)) {
    setFooterText("Preset slot is empty.");
    return false;
  }

  QuickActionTaskArgs* args = new QuickActionTaskArgs{slot, svxlinkAction};
  if (!args) {
    setFooterText("Could not allocate SSH task memory.");
    return false;
  }

  quickActionTaskDone = false;
  quickActionTaskOk = false;
  strlcpy(quickActionTaskLabel, quickActionSlotLabel(slot, svxlinkAction).c_str(), sizeof(quickActionTaskLabel));
  quickActionTaskMessage[0] = 0;
  quickActionTaskRunning = true;

  BaseType_t rc = xTaskCreatePinnedToCore(
    quickActionTaskEntry,
    "quickActionSSH",
    16384,
    args,
    1,
    &quickActionTaskHandle,
    1
  );

  if (rc != pdPASS) {
    quickActionTaskRunning = false;
    delete args;
    setFooterText("Could not start SSH task.");
    return false;
  }

  setFooterText(quickActionSlotLabel(slot, svxlinkAction) + ": running...");
  return true;
}


void processQuickActionTaskResult() {
  if (!quickActionTaskDone) return;

  String label = String(quickActionTaskLabel);
  bool ok = quickActionTaskOk;
  String msg = String(quickActionTaskMessage);
  quickActionTaskDone = false;

  if (!msg.length()) msg = ok ? (label + " completed.") : (label + " failed.");
  setFooterText(clipWithTilde(msg, 44));
  requestRedraw();
}

bool toggleHotspotModeOverSsh(const String& label, bool enable) {
  String command = buildHostModeManagerCommand(label, enable);
  String output;
  String err;

#if ENABLE_PISTAR_TOUCH_SSH
  bool ok = runHotspotSshCommand(command, trimSafe(cfg.pistarSshUser) != "root", &output, &err);
#else
  bool ok = false;
  err = "SSH mode control is not compiled into this build.";
#endif

  if (!ok) {
    String msg = trimSafe(err);
    if (!msg.length()) msg = "SSH command failed.";
    setFooterText(label + " " + (enable ? "enable" : "disable") + " failed: " + clipWithTilde(msg, 40));
    return false;
  }

  ModeStatus* st = modeStatusFromTouchLabel(label);
  if (st) {
    st->known = true;
    st->enabled = enable;
  }

  setFooterText(label + String(enable ? " enabled via SSH." : " disabled via SSH."));
  nextFetchHeard = false;
  requestRedraw();
  return true;
}

String compactRssiValue(const String& value) {
  String out = trimSafe(value);
  if (!out.length()) return "-";
  if (out.equalsIgnoreCase("n/a") || out.equalsIgnoreCase("nan") || out.equalsIgnoreCase("null")) return "-";

  int p1 = out.indexOf('(');
  int p2 = out.indexOf(')', p1 + 1);

  if (p1 >= 0 && p2 > p1) {
    String inside = trimSafe(out.substring(p1 + 1, p2));
    if (inside.indexOf("dBm") >= 0) return inside;
  }

  if (out.indexOf("dBm") >= 0) return out;

  return prettyMetricValue(out, " dBm", false);
}

void drawSignalMeter(int x, int y, int w, int h, bool active, const HeardRecord* rec) {
  fillSoftPanel(x, y, w, h, theme.panel2, theme.border, 8);

  int cx = x + UW(34);
  int cy = y + h - UH(14);
  int radius = UR(24);

  for (int i = 0; i <= 6; i++) {
    float frac = (float)i / 6.0f;
    float angle = PI - frac * (PI * 0.85f);
    int x1 = cx + cos(angle) * (radius - UR(6));
    int y1 = cy - sin(angle) * (radius - UR(6));
    int x2 = cx + cos(angle) * radius;
    int y2 = cy - sin(angle) * radius;
    uint16_t tickColor = (i >= 4) ? theme.warn : theme.subtext;
    tft.drawLine(x1, y1, x2, y2, tickColor);
  }

  int level = 1;
  if (active) {
    if (rec && rec->rssi.length()) {
      int raw = rec->rssi.toInt();
      if (raw != 0 || trimSafe(rec->rssi) == "0") level = constrain(map(raw, -130, -70, 0, 9), 0, 9);
      else level = 8;
    } else level = 8;
  }

  float needleAngle = PI - ((float)level / 9.0f) * (PI * 0.85f);
  int nx = cx + cos(needleAngle) * (radius - UR(8));
  int ny = cy - sin(needleAngle) * (radius - UR(8));
  tft.drawLine(cx, cy, nx, ny, theme.bad);
  tft.fillCircle(cx, cy, max(2, UR(3)), theme.bad);

  tft.setTextFont(1);
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(theme.text, theme.panel2);
  tft.drawString("QUALITY", cx, y + UH(8));

  int infoX = x + UW(72);
  int infoW = w - UW(80);
  int rowY = y + UH(6);
  fillSoftPanel(infoX, rowY, infoW, UH(13), theme.panel, theme.border, 4);
  fillSoftPanel(infoX, rowY + UH(16), infoW, UH(13), theme.panel, theme.border, 4);
  fillSoftPanel(infoX, rowY + UH(32), infoW, UH(13), theme.panel, theme.border, 4);

  String rssiText = active && rec ? compactRssiValue(rec->rssi) : "-";
  String berText  = active && rec ? prettyMetricValue(rec->ber, "%", false) : "-";
  String lossText = active && rec ? prettyMetricValue(rec->loss, "%", true) : "-";

  tft.setTextDatum(ML_DATUM);
  tft.setTextFont(1);
  tft.setTextColor(theme.warn, theme.panel);
  tft.drawString("RSSI", infoX + UW(5), rowY + UH(7));
  tft.drawString("BER",  infoX + UW(5), rowY + UH(23));
  tft.drawString("LOSS", infoX + UW(5), rowY + UH(39));

  tft.setTextDatum(MR_DATUM);
  tft.setTextColor(theme.text, theme.panel);
  tft.drawString(rssiText, infoX + infoW - UW(5), rowY + UH(7));
  tft.drawString(berText,  infoX + infoW - UW(5), rowY + UH(23));
  tft.drawString(lossText, infoX + infoW - UW(5), rowY + UH(39));

  tft.setTextDatum(TL_DATUM);
}

void drawQuickActionLauncherBox(int x, int y, int w) {
  bool svx = isCurrentWatchSvxlink();
  bool ready = hasConfiguredQuickActions(svx);
  uint16_t border = ready ? theme.accent : theme.border;
  uint16_t textColor = ready ? theme.accent : theme.subtext;
  fillSoftPanel(x, y, w, UH(24), theme.panel2, border, 6);
  tft.setTextDatum(MC_DATUM);
  tft.setTextFont(2);
  tft.setTextColor(textColor, theme.panel2);
  tft.drawString("ACTIONS", x + w / 2, y + UH(12));
  tft.setTextDatum(TL_DATUM);
}


void drawFrequencyBox(int x, int y, int w, const String& label, const String& value) {
  String shownValue = trimSafe(value);
  if (shownValue.length() > 8) shownValue = shownValue.substring(0, 8);
  drawCompactValueBox(x, y, w, UH(24), label, shownValue,
                      2, 2, UW(30), theme.warn, theme.good);
}

bool bootSplashReady = false;
int bootLastProgress = -1;
int bootLastScanX = -1;
String bootLastStatus = "";

void drawBootSplashFrame() {
  int cx = SCREEN_W / 2;

  tft.fillScreen(theme.bg);
  tft.drawRoundRect(UX(12), UY(16), SCREEN_W - UW(24), SCREEN_H - UH(32), UR(12), theme.border);
  tft.drawRoundRect(UX(16), UY(20), SCREEN_W - UW(32), SCREEN_H - UH(40), UR(10), theme.panel2);

  tft.fillCircle(cx, UY(72), UR(8), theme.accent);
  for (int r = UR(18); r <= UR(54); r += UR(12)) {
    tft.drawCircle(cx, UY(72), r, theme.border);
  }

  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(theme.text, theme.bg);
  tft.setTextFont(4);
  tft.drawString("MMDVM", cx, UY(106));

  tft.setTextFont(2);
  tft.setTextColor(theme.subtext, theme.bg);
  tft.drawString("BOOT SEQUENCE", cx, UY(130));

  const int barX = UX(34);
  const int barY = UY(202);
  const int barW = UW(252);
  const int barH = UH(14);
  tft.drawRoundRect(barX, barY, barW, barH, UR(7), theme.border);

  bootSplashReady = true;
  bootLastProgress = -1;
  bootLastScanX = -1;
  bootLastStatus = "";
}

void drawBootSplash(const String& status, int progress, int frame = 0) {
  progress = constrain(progress, 0, 100);

  if (!bootSplashReady) {
    drawBootSplashFrame();
  }

  const int cx = SCREEN_W / 2;
  const int barX = UX(34);
  const int barY = UY(202);
  const int barW = UW(252);
  const int barH = UH(14);
  const int innerX = barX + max(1, UW(2));
  const int innerY = barY + max(1, UH(2));
  const int innerW = max(1, barW - max(2, UW(4)));
  const int innerH = max(1, barH - max(2, UH(4)));

  int pulse = frame % 16;
  for (int r = UR(18); r <= UR(54); r += UR(12)) {
    uint16_t c = ((pulse / 2) % 3 == ((r / max(1, UR(12))) % 3)) ? theme.warn : theme.border;
    tft.drawCircle(cx, UY(72), r, c);
  }

  if (status != bootLastStatus) {
    tft.fillRect(UX(34), UY(182), UW(220), UH(16), theme.bg);
    tft.setTextDatum(TL_DATUM);
    tft.setTextColor(theme.subtext, theme.bg);
    tft.setTextFont(2);
    tft.drawString(status, UX(34), UY(182));
    bootLastStatus = status;
  }

  if (progress != bootLastProgress) {
    int oldFillW = bootLastProgress < 0 ? 0 : map(bootLastProgress, 0, 100, 0, innerW);
    int newFillW = map(progress, 0, 100, 0, innerW);

    if (newFillW > oldFillW) {
      tft.fillRoundRect(innerX, innerY, newFillW, innerH, UR(5), theme.accent);
    } else if (newFillW < oldFillW) {
      tft.fillRect(innerX + newFillW, innerY, oldFillW - newFillW, innerH, theme.bg);
    }

    tft.fillRect(UX(258), UY(182), UW(28), UH(16), theme.bg);
    tft.setTextDatum(TR_DATUM);
    tft.setTextColor(theme.text, theme.bg);
    tft.setTextFont(2);
    tft.drawString(String(progress) + "%", UX(286), UY(182));

    bootLastProgress = progress;
  }

  int scanMinX = innerX;
  int progressFillW = (int)map(progress, 0, 100, 0, innerW);
  int scanWidth = progressFillW > 1 ? progressFillW : 1;
  int scanMaxX = innerX + scanWidth - max(1, UW(2));
  int scanX = scanMinX;
  if (scanMaxX > scanMinX) {
    int scanRange = scanMaxX - scanMinX + 1;
    if (scanRange < 1) scanRange = 1;
    scanX = scanMinX + ((frame * 7) % scanRange);
  }

  if (bootLastScanX >= scanMinX && bootLastScanX <= innerX + innerW - max(1, UW(2))) {
    uint16_t restore = (bootLastScanX - innerX) < progressFillW ? theme.accent : theme.bg;
    tft.drawFastVLine(bootLastScanX, innerY, innerH, restore);
    tft.drawFastVLine(bootLastScanX + 1, innerY, innerH, restore);
  }

  tft.drawFastVLine(scanX, innerY, innerH, theme.warn);
  tft.drawFastVLine(scanX + 1, innerY, innerH, theme.warn);
  bootLastScanX = scanX;
}

void animateBootProgress(const String& status, int fromProgress, int toProgress, int frameDelay = 20) {
  if (toProgress < fromProgress) toProgress = fromProgress;
#if DISPLAY_BACKEND_IS_DESKTOP
  desktopTrace("[BOOT] %s -> %d%%\n", status.c_str(), toProgress);
  if (!bootSplashReady) drawBootSplashFrame();
  drawBootSplash(status, toProgress, 0);
  tft.flush();
  return;
#endif
  int frame = 0;
  if (!bootSplashReady) drawBootSplashFrame();
  for (int p = fromProgress; p <= toProgress; p += 2) {
    drawBootSplash(status, p, frame++);
    delay(frameDelay);
  }
  drawBootSplash(status, toProgress, frame);
#if DISPLAY_BACKEND_IS_GFX_COMPAT
  tft.flush();
#endif
}
uint16_t rgb888To565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

String color565ToHex(uint16_t c) {
  uint8_t r = ((c >> 11) & 0x1F) * 255 / 31;
  uint8_t g = ((c >> 5) & 0x3F) * 255 / 63;
  uint8_t b = (c & 0x1F) * 255 / 31;

  char buf[8];
  snprintf(buf, sizeof(buf), "#%02X%02X%02X", r, g, b);
  return String(buf);
}

bool parseHexColor(String s, uint16_t &out565) {
  s.trim();

  if (s.startsWith("#")) s.remove(0, 1);
  else if (s.startsWith("0x") || s.startsWith("0X")) s.remove(0, 2);

  if (s.length() != 6) return false;

  char buf[7];
  s.toCharArray(buf, sizeof(buf));
  char* endptr = nullptr;
  long value = strtol(buf, &endptr, 16);

  if (*endptr != '\0') return false;
  if (value < 0 || value > 0xFFFFFF) return false;

  uint8_t r = (value >> 16) & 0xFF;
  uint8_t g = (value >> 8) & 0xFF;
  uint8_t b = value & 0xFF;
  out565 = rgb888To565(r, g, b);
  return true;
}

void loadThemeColor(JsonObject themeObj, const char* key, uint16_t &target) {
  if (!themeObj.containsKey(key)) return;
  String s = String(themeObj[key] | "");
  uint16_t c;
  if (parseHexColor(s, c)) target = c;
}

void loadThemeFromJson(JsonObject themeObj) {
  loadThemeColor(themeObj, "bg", theme.bg);
  loadThemeColor(themeObj, "panel", theme.panel);
  loadThemeColor(themeObj, "panel2", theme.panel2);
  loadThemeColor(themeObj, "border", theme.border);
  loadThemeColor(themeObj, "text", theme.text);
  loadThemeColor(themeObj, "subtext", theme.subtext);
  loadThemeColor(themeObj, "accent", theme.accent);
  loadThemeColor(themeObj, "good", theme.good);
  loadThemeColor(themeObj, "warn", theme.warn);
  loadThemeColor(themeObj, "bad", theme.bad);
  loadThemeColor(themeObj, "live", theme.live);
  loadThemeColor(themeObj, "rx", theme.rx);
  loadThemeColor(themeObj, "tx", theme.tx);
  loadThemeColor(themeObj, "tabIdle", theme.tabIdle);
  loadThemeColor(themeObj, "tabActive", theme.tabActive);
}

void fillPanel(int x, int y, int w, int h, uint16_t fill, uint16_t border) {
  int rr = max(1, UR(8));
  tft.fillRoundRect(x, y, w, h, rr, fill);
  tft.drawRoundRect(x, y, w, h, rr, border);
}

void drawLabelValue(int x, int y, const String& label, const String& value,
                    uint16_t labelColor, uint16_t valueColor, uint16_t bg) {
  tft.setTextDatum(TL_DATUM);
  tft.setTextFont(2);
  tft.setTextColor(labelColor, bg);
  tft.drawString(label, x, y);
  tft.setTextColor(valueColor, bg);
  tft.drawString(value, x + UW(78), y);
}

String normalizeHamCallsign(const String& input) {
  String s = input;
  s.trim();
  s.toUpperCase();
  if (!s.length()) return s;

  int slash = s.indexOf('/');
  if (slash >= 0) {
    String left = s.substring(0, slash);
    String right = s.substring(slash + 1);
    bool leftHasDigit = false;
    bool rightHasDigit = false;
    for (size_t i = 0; i < left.length(); ++i) if (isDigit(left[i])) { leftHasDigit = true; break; }
    for (size_t i = 0; i < right.length(); ++i) if (isDigit(right[i])) { rightHasDigit = true; break; }
    if (leftHasDigit && !rightHasDigit) s = left;
    else if (!leftHasDigit && rightHasDigit) s = right;
    else s = left;
  }

  int dash = s.indexOf('-');
  if (dash > 0) {
    String base = s.substring(0, dash);
    bool baseHasDigit = false;
    bool baseHasLetter = false;
    for (size_t i = 0; i < base.length(); ++i) {
      if (isDigit(base[i])) baseHasDigit = true;
      if (isAlpha(base[i])) baseHasLetter = true;
    }
    if (baseHasDigit && baseHasLetter) s = base;
  }

  s.replace(" ", "");
  return s;
}

bool ensureLookupDirectory() {
  return storageMkdir("/lookup");
}

String urlEncodeComponent(const String& value) {
  static const char hex[] = "0123456789ABCDEF";
  String encoded;
  encoded.reserve(value.length() * 3);

  for (size_t i = 0; i < value.length(); ++i) {
    uint8_t c = (uint8_t)value[i];
    if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' || c == '~') {
      encoded += (char)c;
    } else {
      encoded += '%';
      encoded += hex[(c >> 4) & 0x0F];
      encoded += hex[c & 0x0F];
    }
  }

  return encoded;
}

String buildRadioIdUsersUrl(const String& callsign) {
  String normalized = normalizeHamCallsign(callsign);
  if (!normalized.length()) return "";

  String encodedCallsign = urlEncodeComponent(normalized);
  String base = trimSafe(cfg.radioIdUsersApiBase);
  if (!base.length()) base = String(RADIOID_USERS_API_BASE_DEFAULT);

  if (base.indexOf("%CALLSIGN%") >= 0) {
    base.replace("%CALLSIGN%", encodedCallsign);
    return base;
  }

  if (base.endsWith("=") || base.endsWith("/") || base.endsWith(":")) {
    return base + encodedCallsign;
  }

  if (base.indexOf('?') >= 0) {
    if (base.endsWith("?") || base.endsWith("&")) {
      return base + "callsign=" + encodedCallsign;
    }
    return base + "&callsign=" + encodedCallsign;
  }

  return base + encodedCallsign;
}

void clearRadioIdLookupCache() {
  for (int i = 0; i < RADIOID_LOOKUP_CACHE_SIZE; ++i) {
    radioIdLookupCache[i] = RadioIdLookupCacheEntry();
  }
  radioIdLookupCacheWriteIndex = 0;
}

int radioIdLookupCacheIndexForCallsign(const String& normalizedCallsign) {
  if (!normalizedCallsign.length()) return -1;
  for (int i = 0; i < RADIOID_LOOKUP_CACHE_SIZE; ++i) {
    if (radioIdLookupCache[i].valid && radioIdLookupCache[i].callsign == normalizedCallsign) return i;
  }
  return -1;
}

struct RadioIdLookupCacheEntry* radioIdLookupCacheSlotForCallsign(const String& normalizedCallsign, bool createIfMissing) {
  int existing = radioIdLookupCacheIndexForCallsign(normalizedCallsign);
  if (existing >= 0) return &radioIdLookupCache[existing];
  if (!createIfMissing) return nullptr;

  RadioIdLookupCacheEntry& slot = radioIdLookupCache[radioIdLookupCacheWriteIndex];
  slot = RadioIdLookupCacheEntry();
  slot.callsign = normalizedCallsign;
  slot.valid = true;
  radioIdLookupCacheWriteIndex = (radioIdLookupCacheWriteIndex + 1) % RADIOID_LOOKUP_CACHE_SIZE;
  return &slot;
}

bool radioIdLookupNeeded(const String& callsign, const String& name) {
  if (!trimSafe(callsign).length()) return false;

  String currentName = trimSafe(name);
  if (!currentName.length()) return true;

  String normalizedCurrent = normalizeHamCallsign(currentName);
  String normalizedCallsign = normalizeHamCallsign(callsign);
  if (normalizedCurrent == normalizedCallsign) return true;

  bool digitsOnly = currentName.length() > 0;
  for (size_t i = 0; i < currentName.length(); ++i) {
    if (!isDigit(currentName[i])) {
      digitsOnly = false;
      break;
    }
  }

  return digitsOnly;
}

String radioIdLookupSignatureForRecord(const HeardRecord& r) {
  String mode = trimSafe(r.mode);
  mode.toUpperCase();

  String callsign = normalizeHamCallsign(r.callsign);
  String target = trimSafe(r.target);
  target.toUpperCase();

  if (!callsign.length()) return "";
  return mode + "|" + callsign + "|" + target;
}

void queueRadioIdLookupForRecord(const HeardRecord& r) {
  if (!cfg.radioIdLookupEnabled) return;

  String normalized = normalizeHamCallsign(r.callsign);
  if (!normalized.length()) return;

  if (!radioIdLookupNeeded(r.callsign, r.name) && trimSafe(r.country).length()) return;

  String signature = radioIdLookupSignatureForRecord(r);
  if (!signature.length() || signature == lastRadioIdLookupSignature) return;

  lastRadioIdLookupSignature = signature;
  pendingRadioIdLookupCallsign = normalized;
}

String lookupPlainTextNameFromPayload(const String& payload, const String& normalizedCallsign) {
  String text = payload;
  text.replace("\r", "\n");

  int lineStart = 0;
  String firstLine = "";
  while (lineStart < (int)text.length()) {
    int lineEnd = text.indexOf('\n', lineStart);
    if (lineEnd < 0) lineEnd = text.length();
    String line = trimSafe(text.substring(lineStart, lineEnd));
    if (line.length()) {
      firstLine = line;
      break;
    }
    lineStart = lineEnd + 1;
  }
  if (firstLine.length()) text = firstLine;

  text = htmlEntityDecode(stripHtmlTags(text));
  text = normalizeSingleLineText(text);
  if (!text.length()) return "";

  String lower = text;
  lower.toLowerCase();
  if (lower == "null" || lower == "false" || lower == "none" || lower == "unknown" || lower == "not found" ||
      lower.indexOf("not found") >= 0 || lower.indexOf("no result") >= 0 || lower.indexOf("no match") >= 0) {
    return "";
  }

  String cs = normalizeHamCallsign(normalizedCallsign);
  String upper = text;
  upper.toUpperCase();
  if (cs.length() && upper == cs) return "";

  if (cs.length() && upper.startsWith(cs)) {
    int cut = cs.length();
    while (cut < (int)text.length()) {
      char c = text[cut];
      if (c == ' ' || c == '\t' || c == ':' || c == ';' || c == ',' || c == '-' || c == '=') cut++;
      else break;
    }
    if (cut > (int)cs.length() && cut < (int)text.length()) {
      text = trimSafe(text.substring(cut));
    }
  }

  if (text.length() > 80) text = clipWithTilde(text, 80);
  return text;
}

bool populateRadioIdEntryFromJsonObject(JsonObject obj, struct RadioIdLookupCacheEntry& entry) {
  if (obj.isNull()) return false;

  String apiName = firstNonEmpty(
    String(obj["name"] | ""),
    String(obj["display_name"] | ""),
    String(obj["full_name"] | ""),
    String(obj["fullname"] | ""),
    String(obj["label"] | ""),
    String(obj["username"] | "")
  );
  if (!apiName.length()) {
    apiName = buildOperatorName(
      firstNonEmpty(String(obj["fname"] | ""), String(obj["first_name"] | ""), String(obj["firstname"] | ""), String(obj["forename"] | "")),
      firstNonEmpty(String(obj["lname"] | ""), String(obj["last_name"] | ""), String(obj["lastname"] | ""), String(obj["surname"] | ""))
    );
  }

  entry.name = trimSafe(apiName);
  entry.country = firstNonEmpty(String(obj["country"] | ""), String(obj["country_name"] | ""));
  entry.city = trimSafe(String(obj["city"] | ""));
  entry.state = firstNonEmpty(String(obj["state"] | ""), String(obj["region"] | ""));
  entry.found = entry.name.length() || entry.country.length() || entry.city.length() || entry.state.length();
  return true;
}


#if DISPLAY_BACKEND_IS_DESKTOP
static size_t desktopDataJsonSkipWs(const std::string& s, size_t i) {
  while (i < s.size() && (s[i] == ' ' || s[i] == '\t' || s[i] == '\r' || s[i] == '\n')) ++i;
  return i;
}

static bool desktopDataJsonReadStringAt(const std::string& s, size_t i, String& out, size_t* endOut = nullptr) {
  if (i >= s.size() || s[i] != '"') return false;
  std::string v;
  ++i;
  while (i < s.size()) {
    char c = s[i++];
    if (c == '"') {
      out = String(v);
      if (endOut) *endOut = i;
      return true;
    }
    if (c == '\\' && i < s.size()) {
      char e = s[i++];
      switch (e) {
        case '"': v.push_back('"'); break;
        case '\\': v.push_back('\\'); break;
        case '/': v.push_back('/'); break;
        case 'b': v.push_back('\b'); break;
        case 'f': v.push_back('\f'); break;
        case 'n': v.push_back('\n'); break;
        case 'r': v.push_back('\r'); break;
        case 't': v.push_back('\t'); break;
        case 'u': if (i + 4 <= s.size()) i += 4; v.push_back('?'); break;
        default: v.push_back(e); break;
      }
    } else {
      v.push_back(c);
    }
  }
  return false;
}

static bool desktopDataJsonReadValueAt(const std::string& s, size_t i, String& out, size_t* endOut = nullptr) {
  i = desktopDataJsonSkipWs(s, i);
  if (i >= s.size()) return false;
  if (s[i] == '"') return desktopDataJsonReadStringAt(s, i, out, endOut);
  if (s[i] == '{' || s[i] == '[') return false;
  size_t start = i;
  while (i < s.size() && s[i] != ',' && s[i] != '}' && s[i] != ']' && s[i] != '\r' && s[i] != '\n') ++i;
  std::string v = s.substr(start, i - start);
  while (!v.empty() && (v.back() == ' ' || v.back() == '\t')) v.pop_back();
  out = String(v);
  if (endOut) *endOut = i;
  return v.size() > 0;
}

static size_t desktopDataJsonFindValue(const std::string& json, const char* key, size_t from = 0) {
  std::string pat = std::string("\"") + key + "\"";
  size_t p = from;
  while ((p = json.find(pat, p)) != std::string::npos) {
    size_t colon = json.find(':', p + pat.size());
    if (colon != std::string::npos) return desktopDataJsonSkipWs(json, colon + 1);
    p += pat.size();
  }
  return std::string::npos;
}

static bool desktopDataJsonGetValue(const std::string& json, const char* key, String& out) {
  size_t p = desktopDataJsonFindValue(json, key);
  if (p == std::string::npos) return false;
  return desktopDataJsonReadValueAt(json, p, out);
}

static String desktopDataJsonFirstValue(const std::string& json, const char* const* keys, size_t count) {
  for (size_t i = 0; i < count; ++i) {
    String v;
    if (desktopDataJsonGetValue(json, keys[i], v)) {
      v = trimSafe(v);
      if (v.length() && v != "null") return v;
    }
  }
  return "";
}

static bool desktopDataJsonExtractContainerAt(const std::string& json, size_t p, char openCh, char closeCh, std::string& out) {
  if (p == std::string::npos || p >= json.size() || json[p] != openCh) return false;
  size_t start = p;
  int depth = 0;
  bool inStr = false;
  bool esc = false;
  for (; p < json.size(); ++p) {
    char c = json[p];
    if (inStr) {
      if (esc) esc = false;
      else if (c == '\\') esc = true;
      else if (c == '"') inStr = false;
      continue;
    }
    if (c == '"') inStr = true;
    else if (c == openCh) ++depth;
    else if (c == closeCh) {
      --depth;
      if (depth == 0) {
        out = json.substr(start, p - start + 1);
        return true;
      }
    }
  }
  return false;
}

static size_t desktopDataJsonFindContainerValue(const std::string& json, const char* key, char openCh, size_t from = 0) {
  std::string pat = std::string("\"") + key + "\"";
  size_t p = from;
  while ((p = json.find(pat, p)) != std::string::npos) {
    size_t colon = json.find(':', p + pat.size());
    if (colon == std::string::npos) return std::string::npos;
    size_t value = desktopDataJsonSkipWs(json, colon + 1);
    if (value != std::string::npos && value < json.size() && json[value] == openCh) return value;
    p += pat.size();
  }
  return std::string::npos;
}

static bool desktopDataJsonGetArray(const std::string& json, const char* key, std::string& out) {
  size_t p = desktopDataJsonFindContainerValue(json, key, '[');
  return desktopDataJsonExtractContainerAt(json, p, '[', ']', out);
}

static bool desktopDataJsonGetObject(const std::string& json, const char* key, std::string& out) {
  size_t p = desktopDataJsonFindContainerValue(json, key, '{');
  return desktopDataJsonExtractContainerAt(json, p, '{', '}', out);
}

static bool desktopDataJsonNextObject(const std::string& json, size_t& pos, std::string& objOut) {
  size_t start = json.find('{', pos);
  if (start == std::string::npos) return false;
  if (!desktopDataJsonExtractContainerAt(json, start, '{', '}', objOut)) return false;
  pos = start + objOut.size();
  return true;
}

static bool desktopDataJsonArrayForHeardPayload(const std::string& payload, std::string& arrOut) {
  size_t p = desktopDataJsonSkipWs(payload, 0);
  if (p < payload.size() && payload[p] == '[') return desktopDataJsonExtractContainerAt(payload, p, '[', ']', arrOut);
  const char* keys[] = { "last_heard", "traffic", "data", "results", "heard", "lastHeard" };
  for (const char* key : keys) {
    if (desktopDataJsonGetArray(payload, key, arrOut)) return true;
  }
  return false;
}

static bool desktopPopulateRadioIdEntryFromRawObject(const std::string& obj, struct RadioIdLookupCacheEntry& entry) {
  const char* nameKeys[] = { "name", "display_name", "full_name", "fullname", "label", "username" };
  String apiName = desktopDataJsonFirstValue(obj, nameKeys, sizeof(nameKeys) / sizeof(nameKeys[0]));
  if (!apiName.length()) {
    const char* firstKeys[] = { "fname", "first_name", "firstname", "forename" };
    const char* lastKeys[]  = { "lname", "last_name", "lastname", "surname" };
    apiName = buildOperatorName(desktopDataJsonFirstValue(obj, firstKeys, sizeof(firstKeys) / sizeof(firstKeys[0])),
                                desktopDataJsonFirstValue(obj, lastKeys, sizeof(lastKeys) / sizeof(lastKeys[0])));
  }
  const char* countryKeys[] = { "country", "country_name" };
  const char* stateKeys[] = { "state", "region" };
  entry.name = trimSafe(apiName);
  entry.country = desktopDataJsonFirstValue(obj, countryKeys, sizeof(countryKeys) / sizeof(countryKeys[0]));
  desktopDataJsonGetValue(obj, "city", entry.city);
  entry.city = trimSafe(entry.city);
  entry.state = desktopDataJsonFirstValue(obj, stateKeys, sizeof(stateKeys) / sizeof(stateKeys[0]));
  entry.found = entry.name.length() || entry.country.length() || entry.city.length() || entry.state.length();
  return true;
}

static bool desktopParseRadioIdUserResponseRaw(const String& payload, struct RadioIdLookupCacheEntry& entry) {
  std::string p = (std::string)payload;
  std::string obj;
  std::string arr;
  if (desktopDataJsonGetArray(p, "results", arr) || desktopDataJsonGetArray(p, "data", arr)) {
    size_t pos = 0;
    if (desktopDataJsonNextObject(arr, pos, obj)) return desktopPopulateRadioIdEntryFromRawObject(obj, entry);
    String text;
    if (desktopDataJsonReadValueAt(arr, desktopDataJsonSkipWs(arr, 1), text)) {
      entry.name = lookupPlainTextNameFromPayload(text, entry.callsign);
      entry.found = entry.name.length();
      return true;
    }
    return true;
  }
  if (desktopDataJsonGetObject(p, "user", obj) || desktopDataJsonGetObject(p, "data", obj)) {
    return desktopPopulateRadioIdEntryFromRawObject(obj, entry);
  }
  size_t root = desktopDataJsonSkipWs(p, 0);
  if (root < p.size() && p[root] == '[') {
    size_t pos = root;
    if (desktopDataJsonNextObject(p, pos, obj)) return desktopPopulateRadioIdEntryFromRawObject(obj, entry);
  } else if (root < p.size() && p[root] == '{') {
    if (desktopDataJsonExtractContainerAt(p, root, '{', '}', obj)) return desktopPopulateRadioIdEntryFromRawObject(obj, entry);
  }
  entry.name = lookupPlainTextNameFromPayload(payload, entry.callsign);
  entry.found = entry.name.length();
  return true;
}

static void desktopApplyParsedHeardList(HeardRecord* newList, int newCount) {
  applyRadioIdCacheToList(newList, newCount);
  inferModeStatusesFromHeard(newList, newCount);

  bool listChanged = !heardListsEqual(heardList, heardCount, newList, newCount);
  if (!parsingSvxlinkData) updateDmrTimeslotLiveRecordsFromList(newList, newCount);

  if (newCount == 0) {
    if (!parsingSvxlinkData) clearDmrTimeslotLiveRecords();
    if (heardCount != 0) {
      clearHeard();
      requestRedraw();
    }
    setFooterText(parsingSvxlinkData ? String("SVXLink API OK, no entries") : String("API OK, no entries"), !parsingSvxlinkData);
    return;
  }

  if (listChanged) {
    clearHeard();
    for (int i = 0; i < newCount; i++) heardList[i] = newList[i];
    heardCount = newCount;
    requestRedraw();
  }

  if (newCount > 0) {
    if (newList[0].rxFreq.length()) currentRxFreq = newList[0].rxFreq;
    if (newList[0].txFreq.length()) currentTxFreq = newList[0].txFreq;
    queueRadioIdLookupForRecord(newList[0]);
    if (!sameRecord(liveRecord, newList[0])) {
      liveRecord = newList[0];
      updateSessionInfoFromRecord(liveRecord);
      liveRecordMillis = millis();
      requestRedraw();
      noteScreenActivity(false);
    } else if (!parsingSvxlinkData) {
      liveRecordMillis = millis();
    }
  }

  String okFooter = parsingSvxlinkData ? (String("SVXLink API updated from ") + displayBackendHost()) : (String("Updated from ") + displayBackendHost());
  setFooterText(okFooter, !parsingSvxlinkData);
}

static bool desktopParseHeardArrayRaw(const String& payload) {
  std::string arr;
  if (!desktopDataJsonArrayForHeardPayload((std::string)payload, arr)) {
    lastError = "Desktop JSON object missing heard array";
    return false;
  }

  HeardRecord newList[MAX_HEARD];
  int newCount = 0;
  size_t pos = 0;
  std::string obj;
  while (newCount < MAX_HEARD && desktopDataJsonNextObject(arr, pos, obj)) {
    HeardRecord r;
    const char* timeKeys[] = { "time_utc", "timestamp", "time", "time_local", "datetime" };
    const char* modeKeys[] = { "mode", "module", "source_mode" };
    const char* callKeys[] = { "callsign", "callsign_long", "call", "station" };
    const char* nameKeys[] = { "name", "display_name", "full_name", "fullname" };
    const char* firstKeys[] = { "fname", "first_name", "firstname", "forename" };
    const char* lastKeys[] = { "lname", "last_name", "lastname", "surname" };
    const char* countryKeys[] = { "country", "country_name" };
    const char* gridKeys[] = { "grid", "locator", "maidenhead", "qth_locator" };
    const char* targetKeys[] = { "target", "talkgroup", "destination", "reflector", "dst", "dst_tg" };
    const char* targetNameKeys[] = { "target_name", "talkgroup_name", "room_name", "reflector_name", "tg_name" };
    const char* tsKeys1[] = { "timeslot", "time_slot", "timeSlot", "slot", "ts" };
    const char* tsKeys2[] = { "TS", "dmr_slot", "dmrSlot", "DMRSlot", "TimeSlot" };
    const char* srcKeys[] = { "src", "source", "source_system" };
    const char* durationKeys[] = { "duration", "length" };
    const char* lossKeys[] = { "loss", "packet_loss" };
    const char* berKeys[] = { "bit_error_rate", "ber" };
    const char* rssiKeys[] = { "rssi", "signal" };
    const char* rxKeys[] = { "rxFreq", "rx_freq", "RXFrequency", "rx_frequency" };
    const char* txKeys[] = { "txFreq", "tx_freq", "TXFrequency", "tx_frequency" };

    r.timeUtc = normalizeHeardTimeDisplay(desktopDataJsonFirstValue(obj, timeKeys, sizeof(timeKeys)/sizeof(timeKeys[0])));
    r.mode = desktopDataJsonFirstValue(obj, modeKeys, sizeof(modeKeys)/sizeof(modeKeys[0]));
    r.callsign = normalizeHamCallsign(desktopDataJsonFirstValue(obj, callKeys, sizeof(callKeys)/sizeof(callKeys[0])));
    String apiName = desktopDataJsonFirstValue(obj, nameKeys, sizeof(nameKeys)/sizeof(nameKeys[0]));
    r.name = trimSafe(apiName.length() ? apiName : buildOperatorName(desktopDataJsonFirstValue(obj, firstKeys, sizeof(firstKeys)/sizeof(firstKeys[0])),
                                                                      desktopDataJsonFirstValue(obj, lastKeys, sizeof(lastKeys)/sizeof(lastKeys[0]))));
    r.country = desktopDataJsonFirstValue(obj, countryKeys, sizeof(countryKeys)/sizeof(countryKeys[0]));
    r.grid = desktopDataJsonFirstValue(obj, gridKeys, sizeof(gridKeys)/sizeof(gridKeys[0]));
    r.target = desktopDataJsonFirstValue(obj, targetKeys, sizeof(targetKeys)/sizeof(targetKeys[0]));
    r.targetName = desktopDataJsonFirstValue(obj, targetNameKeys, sizeof(targetNameKeys)/sizeof(targetNameKeys[0]));
    r.timeslot = normalizeDmrTimeslot(desktopDataJsonFirstValue(obj, tsKeys1, sizeof(tsKeys1)/sizeof(tsKeys1[0])));
    if (!r.timeslot.length()) r.timeslot = normalizeDmrTimeslot(desktopDataJsonFirstValue(obj, tsKeys2, sizeof(tsKeys2)/sizeof(tsKeys2[0])));
    if (!r.timeslot.length()) r.timeslot = extractDmrTimeslotFromText(firstNonEmpty(r.mode, r.target, r.targetName, desktopDataJsonFirstValue(obj, srcKeys, sizeof(srcKeys)/sizeof(srcKeys[0]))));
    r.src = desktopDataJsonFirstValue(obj, srcKeys, sizeof(srcKeys)/sizeof(srcKeys[0]));
    r.duration = desktopDataJsonFirstValue(obj, durationKeys, sizeof(durationKeys)/sizeof(durationKeys[0]));
    r.loss = desktopDataJsonFirstValue(obj, lossKeys, sizeof(lossKeys)/sizeof(lossKeys[0]));
    r.ber = desktopDataJsonFirstValue(obj, berKeys, sizeof(berKeys)/sizeof(berKeys[0]));
    r.rssi = desktopDataJsonFirstValue(obj, rssiKeys, sizeof(rssiKeys)/sizeof(rssiKeys[0]));
    r.rxFreq = desktopDataJsonFirstValue(obj, rxKeys, sizeof(rxKeys)/sizeof(rxKeys[0]));
    r.txFreq = desktopDataJsonFirstValue(obj, txKeys, sizeof(txKeys)/sizeof(txKeys[0]));
    if (parsingSvxlinkData && !r.mode.length()) r.mode = "SVXLink";
    if (parsingSvxlinkData && !r.src.length()) r.src = "SVXRef";
    r.valid = r.callsign.length() > 0;
    if (r.valid) newList[newCount++] = r;
  }

  desktopTrace("[DESKTOP] parsed heard JSON: %d entries.\n", newCount);
  desktopApplyParsedHeardList(newList, newCount);
  return true;
}
#endif

bool parseRadioIdUserResponse(const String& payload, struct RadioIdLookupCacheEntry& entry) {
  entry.found = false;
  entry.name = "";
  entry.country = "";
  entry.city = "";
  entry.state = "";

#if DISPLAY_BACKEND_IS_DESKTOP
  return desktopParseRadioIdUserResponseRaw(payload, entry);
#endif

  DynamicJsonDocument doc(4096);
  DeserializationError err = deserializeJson(doc, payload);
  if (!err) {
    if (doc.is<JsonObject>()) {
      JsonObject root = doc.as<JsonObject>();

      JsonArray results = root["results"].as<JsonArray>();
      if (!results.isNull()) {
        if (results.size() == 0) return true;
        JsonVariant first = results[0];
        if (first.is<JsonObject>()) return populateRadioIdEntryFromJsonObject(first.as<JsonObject>(), entry);
        if (first.is<const char*>()) {
          entry.name = lookupPlainTextNameFromPayload(String(first.as<const char*>()), entry.callsign);
          entry.found = entry.name.length();
          return true;
        }
      }

      if (root["user"].is<JsonObject>()) return populateRadioIdEntryFromJsonObject(root["user"].as<JsonObject>(), entry);
      if (root["data"].is<JsonObject>()) return populateRadioIdEntryFromJsonObject(root["data"].as<JsonObject>(), entry);
      if (root["data"].is<JsonArray>()) {
        JsonArray data = root["data"].as<JsonArray>();
        if (data.size() == 0) return true;
        JsonVariant first = data[0];
        if (first.is<JsonObject>()) return populateRadioIdEntryFromJsonObject(first.as<JsonObject>(), entry);
        if (first.is<const char*>()) {
          entry.name = lookupPlainTextNameFromPayload(String(first.as<const char*>()), entry.callsign);
          entry.found = entry.name.length();
          return true;
        }
      }

      if (populateRadioIdEntryFromJsonObject(root, entry)) return true;
      return true;
    }

    if (doc.is<JsonArray>()) {
      JsonArray arr = doc.as<JsonArray>();
      if (arr.size() == 0) return true;
      JsonVariant first = arr[0];
      if (first.is<JsonObject>()) return populateRadioIdEntryFromJsonObject(first.as<JsonObject>(), entry);
      if (first.is<const char*>()) {
        entry.name = lookupPlainTextNameFromPayload(String(first.as<const char*>()), entry.callsign);
        entry.found = entry.name.length();
        return true;
      }
      return true;
    }

    if (doc.is<const char*>()) {
      entry.name = lookupPlainTextNameFromPayload(String(doc.as<const char*>()), entry.callsign);
      entry.found = entry.name.length();
      return true;
    }
  }

  entry.name = lookupPlainTextNameFromPayload(payload, entry.callsign);
  entry.found = entry.name.length();
  return true;
}

bool fetchRadioIdProfile(const String& normalizedCallsign, struct RadioIdLookupCacheEntry& outEntry) {
  String url = buildRadioIdUsersUrl(normalizedCallsign);
  if (!url.length()) return false;

  String payload;
  int code = 0;
  bool ok = httpGetString(url, payload, code, "application/json, text/plain;q=0.9, text/html;q=0.8", "");
  if (!ok) {
    addWebLog("WARN", "LOOKUP", "Callsign lookup failed for " + normalizedCallsign + " (HTTP " + String(code) + ").");
    return false;
  }

  outEntry = RadioIdLookupCacheEntry();
  outEntry.callsign = normalizedCallsign;
  outEntry.valid = true;
  outEntry.lastFetchMillis = millis();
  ok = parseRadioIdUserResponse(payload, outEntry);
  if (!ok) {
    addWebLog("WARN", "LOOKUP", "Callsign lookup parse failed for " + normalizedCallsign + ".");
  } else if (outEntry.found) {
    addWebLog("INFO", "LOOKUP", "Callsign lookup found " + normalizedCallsign + " as " + outEntry.name + ".", false);
  }
  return ok;
}

void applyRadioIdCacheToRecord(struct HeardRecord& r, bool allowFetch) {
  if (!cfg.radioIdLookupEnabled) return;

  String normalized = normalizeHamCallsign(r.callsign);
  if (!normalized.length()) return;

  struct RadioIdLookupCacheEntry* entry = radioIdLookupCacheSlotForCallsign(normalized, false);
  if (!entry || !entry->valid || !entry->lastFetchMillis) {
    if (allowFetch && radioIdLookupNeeded(r.callsign, r.name)) { }
    return;
  }

  if (radioIdLookupNeeded(r.callsign, r.name) && entry->name.length()) r.name = entry->name;
  if (!trimSafe(r.country).length() && entry->country.length()) r.country = entry->country;
}

void applyRadioIdCacheToList(struct HeardRecord* list, int count) {
  if (count <= 0) return;
  for (int i = 0; i < count; ++i) applyRadioIdCacheToRecord(list[i], false);
}

bool applyRadioIdCacheToActiveData() {
  bool changed = false;

  for (int i = 0; i < heardCount; ++i) {
    HeardRecord before = heardList[i];
    applyRadioIdCacheToRecord(heardList[i], false);
    if (!sameRecord(before, heardList[i])) changed = true;
  }

  HeardRecord beforeLive = liveRecord;
  applyRadioIdCacheToRecord(liveRecord, false);
  if (!sameRecord(beforeLive, liveRecord)) changed = true;

  return changed;
}

String radioIdDisplayNameForRecord(const struct HeardRecord& r, bool allowFetch) {
  String displayName = trimSafe(r.name);
  if (normalizeModeName(r.mode) == "SVXLINK") displayName = cleanSvxlinkTargetValue(displayName);
  if (!radioIdLookupNeeded(r.callsign, displayName)) return displayName;

  String normalized = normalizeHamCallsign(r.callsign);
  if (!normalized.length()) return displayName;

  struct RadioIdLookupCacheEntry* entry = radioIdLookupCacheSlotForCallsign(normalized, false);
  if (entry && entry->valid && entry->name.length()) return trimSafe(entry->name);
  if (allowFetch) { }

  return displayName;
}

String radioIdDisplayCountryForRecord(const struct HeardRecord& r, bool allowFetch) {
  String currentCountry = trimSafe(r.country);
  if (currentCountry.length()) return currentCountry;

  String normalized = normalizeHamCallsign(r.callsign);
  if (!normalized.length()) return currentCountry;

  struct RadioIdLookupCacheEntry* entry = radioIdLookupCacheSlotForCallsign(normalized, false);
  if (entry && entry->valid && entry->country.length()) return trimSafe(entry->country);
  if (allowFetch) { }

  return currentCountry;
}

String nextRadioIdLookupCandidate() {
  if (!cfg.radioIdLookupEnabled || WiFi.status() != WL_CONNECTED) return "";

  auto pick = [&](const HeardRecord& rec) -> String {
    String normalized = normalizeHamCallsign(rec.callsign);
    if (!normalized.length()) return "";

    struct RadioIdLookupCacheEntry* entry = radioIdLookupCacheSlotForCallsign(normalized, false);
    unsigned long now = millis();
    if (entry && entry->valid && entry->lastFetchMillis) {
      unsigned long age = now - entry->lastFetchMillis;
      if (entry->found && age < RADIOID_LOOKUP_TTL_MS) return "";
      if (!entry->found && age < RADIOID_LOOKUP_RETRY_MS) return "";
    }

    if (!radioIdLookupNeeded(rec.callsign, rec.name) && trimSafe(rec.country).length()) return "";
    return normalized;
  };

  if (detailPopupOpen && selectedHeardIndex >= 0 && selectedHeardIndex < heardCount) {
    String candidate = pick(heardList[selectedHeardIndex]);
    if (candidate.length()) return candidate;
  }

  if (isLiveActive()) {
    String candidate = pick(liveRecord);
    if (candidate.length()) return candidate;
  }

  int rows = min(heardCount, 5);
  for (int i = 0; i < rows; ++i) {
    String candidate = pick(heardList[i]);
    if (candidate.length()) return candidate;
  }

  return "";
}

void processRadioIdLookups() {
  if (!cfg.radioIdLookupEnabled || WiFi.status() != WL_CONNECTED) return;
  if (!pendingRadioIdLookupCallsign.length()) return;

  unsigned long now = millis();
  if (now - lastRadioIdLookupMillis < RADIOID_LOOKUP_MIN_INTERVAL_MS) return;

  String normalized = pendingRadioIdLookupCallsign;
  pendingRadioIdLookupCallsign = "";

  struct RadioIdLookupCacheEntry* cached = radioIdLookupCacheSlotForCallsign(normalized, false);
  if (cached && cached->valid && cached->lastFetchMillis) {
    unsigned long age = now - cached->lastFetchMillis;
    if ((cached->found && age < RADIOID_LOOKUP_TTL_MS) || (!cached->found && age < RADIOID_LOOKUP_RETRY_MS)) {
      if (applyRadioIdCacheToActiveData()) { saveActiveHeardCache(); requestRedraw(); }
      return;
    }
  }

  lastRadioIdLookupMillis = now;

  struct RadioIdLookupCacheEntry fetched;
  if (!fetchRadioIdProfile(normalized, fetched)) {
    return;
  }

  struct RadioIdLookupCacheEntry* slot = radioIdLookupCacheSlotForCallsign(normalized, true);
  if (!slot) return;
  *slot = fetched;

  if (applyRadioIdCacheToActiveData()) { saveActiveHeardCache(); requestRedraw(); }
}

String flagBmpPathFromCode(const String& code) {
  String cc = code;
  cc.toLowerCase();
  if (cc.length()) {
    String p = "/flags/" + cc + ".bmp";
    if (storageExists(p.c_str())) return p;
    p = "/" + cc + ".bmp";
    if (storageExists(p.c_str())) return p;
  }
  if (storageExists("/flags/de.bmp")) return "/flags/de.bmp";
  return "/de.bmp";
}

bool drawFlagForCallsign(int x, int y, int w, int h, const String& callsign) {
  String path = flagBmpPathFromCode(countryCodeFromCallsign(callsign));

  const int bmpW = 32;
  const int bmpH = 32;

  if (w < bmpW || h < bmpH) return false;

  int drawX = x + (w - bmpW) / 2;
  int drawY = y + (h - bmpH) / 2;

  return drawBmp24FromSD(path.c_str(), drawX, drawY, bmpW, bmpH);
}

String getHeaderTimeText();


String getLastHeardFilterLabel() {
  switch (lastHeardFilter) {
    case LAST_HEARD_FILTER_DMR: return "DMR";
    case LAST_HEARD_FILTER_DSTAR: return "DSTAR";
    case LAST_HEARD_FILTER_YSF: return "YSF";
    case LAST_HEARD_FILTER_P25: return "P25";
    case LAST_HEARD_FILTER_NXDN: return "NXDN";
    case LAST_HEARD_FILTER_POCSAG: return "POCSAG";
    case LAST_HEARD_FILTER_ALL:
    default: return "ALL";
  }
}

String getLastHeardSortLabel() {
  switch (lastHeardSort) {
    case LAST_HEARD_SORT_CALLSIGN: return "CALL";
    case LAST_HEARD_SORT_MODE: return "MODE";
    case LAST_HEARD_SORT_NEWEST:
    default: return "NEWEST";
  }
}

bool heardRecordMatchesLastHeardFilter(const HeardRecord& rec) {
  if (lastHeardFilter == LAST_HEARD_FILTER_ALL) return true;
  String mode = normalizeModeName(rec.mode);
  if (lastHeardFilter == LAST_HEARD_FILTER_DMR) return mode == "DMR";
  if (lastHeardFilter == LAST_HEARD_FILTER_DSTAR) return mode == "DSTAR";
  if (lastHeardFilter == LAST_HEARD_FILTER_YSF) return mode == "YSF";
  if (lastHeardFilter == LAST_HEARD_FILTER_P25) return mode == "P25";
  if (lastHeardFilter == LAST_HEARD_FILTER_NXDN) return mode == "NXDN";
  if (lastHeardFilter == LAST_HEARD_FILTER_POCSAG) return mode == "POCSAG";
  return true;
}

static int compareLastHeardField(const String& a, const String& b) {
  String aa = a;
  String bb = b;
  aa.toUpperCase();
  bb.toUpperCase();
  return aa.compareTo(bb);
}

int buildLastHeardVisibleIndices(int* outIndices, int maxCount) {
  if (!outIndices || maxCount <= 0) return 0;
  int count = 0;
  for (int i = 0; i < heardCount && count < maxCount; i++) {
    if (heardRecordMatchesLastHeardFilter(heardList[i])) outIndices[count++] = i;
  }

  if (lastHeardSort == LAST_HEARD_SORT_NEWEST || count <= 1) return count;

  for (int i = 0; i < count - 1; i++) {
    for (int j = i + 1; j < count; j++) {
      bool swapNeeded = false;
      const HeardRecord& a = heardList[outIndices[i]];
      const HeardRecord& b = heardList[outIndices[j]];

      if (lastHeardSort == LAST_HEARD_SORT_CALLSIGN) {
        int cmp = compareLastHeardField(a.callsign, b.callsign);
        if (cmp == 0) cmp = compareLastHeardField(a.timeUtc, b.timeUtc);
        swapNeeded = cmp > 0;
      } else if (lastHeardSort == LAST_HEARD_SORT_MODE) {
        int cmp = compareLastHeardField(normalizeModeName(a.mode), normalizeModeName(b.mode));
        if (cmp == 0) cmp = compareLastHeardField(a.callsign, b.callsign);
        if (cmp == 0) cmp = compareLastHeardField(a.timeUtc, b.timeUtc);
        swapNeeded = cmp > 0;
      }

      if (swapNeeded) {
        int tmp = outIndices[i];
        outIndices[i] = outIndices[j];
        outIndices[j] = tmp;
      }
    }
  }

  return count;
}

void normalizeLastHeardBrowseState() {
  int indices[MAX_HEARD];
  int visibleCount = buildLastHeardVisibleIndices(indices, MAX_HEARD);
  int maxPage = visibleCount > 0 ? ((visibleCount - 1) / LAST_HEARD_ROWS_PER_PAGE) : 0;
  if (lastHeardPageIndex < 0) lastHeardPageIndex = 0;
  if (lastHeardPageIndex > maxPage) lastHeardPageIndex = maxPage;
}

void normalizeSvxlinkHeardBrowseState() {
  int maxPage = heardCount > 0 ? ((heardCount - 1) / LAST_HEARD_ROWS_PER_PAGE) : 0;
  if (lastHeardPageIndex < 0) lastHeardPageIndex = 0;
  if (lastHeardPageIndex > maxPage) lastHeardPageIndex = maxPage;
}

void drawHeaderClockBox() {
  const int clockX = UX(246);
  const int clockY = UY(6);
  const int clockW = UW(68);
  const int clockH = UH(24);

  fillSoftPanel(clockX, clockY, clockW, clockH, theme.panel2, theme.border, 8);

  tft.setTextDatum(MC_DATUM);
  tft.setTextFont(2);
  tft.setTextColor(TFT_WHITE, theme.panel2);
  tft.drawString(getHeaderTimeText(), clockX + clockW / 2, clockY + clockH / 2);
  tft.setTextDatum(TL_DATUM);
}

void drawTopTabs() {
  const int tabY = UY(6);
  const int tabH = UH(24);
  const int tabGap = UW(4);
  const int tabW = UW(54);
  const int startX = UX(6);
  const bool showApTab = setupRequired && apModeActive;

  const Page pages[4] = {PAGE_LIVE, PAGE_LAST, PAGE_STATUS, showApTab ? PAGE_AP : PAGE_INFO};
  const bool watch = svxlinkFeatureEnabled() || aprsFeatureEnabled() || weatherFeatureEnabled() || hamqslFeatureEnabled();
  const char* labels[4] = {watch ? "WATCH" : "LIVE", "HEARD", "STATUS", showApTab ? "AP" : "INFO"};

  for (int i = 0; i < 4; i++) {
    int x = startX + i * (tabW + tabGap);
    uint16_t fill = (currentPage == pages[i]) ? theme.tabActive : theme.tabIdle;
    if (pages[i] == PAGE_AP && apModeActive) fill = theme.tabActive;
    fillSoftPanel(x, tabY, tabW, tabH, fill, theme.border, 8);

    tft.setTextDatum(MC_DATUM);
    tft.setTextFont(2);
    tft.setTextColor(TFT_WHITE, fill);
    tft.drawString(labels[i], x + tabW / 2, tabY + tabH / 2);
  }

  drawHeaderClockBox();
}

void drawFooter() {
  tft.fillRect(0, UY(222), SCREEN_W, UH(18), theme.panel2);
  tft.drawFastHLine(0, UY(221), SCREEN_W, theme.border);
  tft.setTextDatum(TL_DATUM);
  tft.setTextFont(1);
  tft.setTextColor(theme.subtext, theme.panel2);
  tft.drawString(footerText, UX(4), UY(227));
}

void clearBody() {
  tft.fillRect(0, UY(34), SCREEN_W, UY(188), theme.bg);
}

String countryCodeFromCallsign(const String& cs) {
  String c = normalizeHamCallsign(cs);
  c.toUpperCase();
  if (!c.length()) return "";

  auto hasPrefix = [&](const char* p) -> bool {
    return c.startsWith(p);
  };

  auto hasAnyPrefix = [&](std::initializer_list<const char*> prefixes) -> bool {
    for (const char* p : prefixes) {
      if (c.startsWith(p)) return true;
    }
    return false;
  };

  if (hasPrefix("4U")) return "un";
  if (hasPrefix("4Y")) return "icao";
  if (hasPrefix("C7")) return "wmo";
  if (hasPrefix("3Y") || hasPrefix("JW") || hasPrefix("JX") || hasAnyPrefix({"LA","LB","LC","LD","LE","LF","LG","LH","LI","LJ","LK","LL","LM","LN"})) return "no";
  if (hasPrefix("VR")) return "hk";
  if (hasPrefix("XX")) return "mo";
  if (hasPrefix("E4")) return "ps";

  if (hasAnyPrefix({"2","G","M","VP","VQ","VS","ZB","ZC","ZD","ZE","ZF","ZG","ZH","ZI","ZJ","ZN","ZO","ZQ"})) return "uk";
  if (hasAnyPrefix({"DA","DB","DC","DD","DE","DF","DG","DH","DI","DJ","DK","DL","DM","DN","DO","DP","DQ","DR",
                    "Y2","Y3","Y4","Y5","Y6","Y7","Y8","Y9"})) return "de";
  if (hasAnyPrefix({"F","HW","HX","HY","TH","TK","TM","TO","TP","TQ","TV","TW","TX"})) return "fr";
  if (hasAnyPrefix({"EA","EB","EC","ED","EE","EF","EG","EH","AM","AN","AO"})) return "es";
  if (hasAnyPrefix({"I"})) return "it";
  if (hasAnyPrefix({"PA","PB","PC","PD","PE","PF","PG","PH","PI"})) return "nl";
  if (hasAnyPrefix({"ON","OO","OP","OQ","OR","OS","OT"})) return "be";
  if (hasAnyPrefix({"OU","OV","OW","OX","OY","OZ","5P","5Q","XP"})) return "dk";
  if (hasAnyPrefix({"OH","OF","OG","OI","OJ"})) return "fi";
  if (hasAnyPrefix({"OE"})) return "at";
  if (hasAnyPrefix({"OK","OL"})) return "cz";
  if (hasAnyPrefix({"OM"})) return "sk";
  if (hasAnyPrefix({"HB","HE"})) return "ch";
  if (hasAnyPrefix({"LX"})) return "lu";
  if (hasAnyPrefix({"LY"})) return "lt";
  if (hasAnyPrefix({"LZ"})) return "bg";
  if (hasAnyPrefix({"ER"})) return "md";
  if (hasAnyPrefix({"ES"})) return "ee";
  if (hasAnyPrefix({"EU","EV","EW"})) return "by";
  if (hasAnyPrefix({"EK"})) return "am";
  if (hasAnyPrefix({"EX"})) return "kg";
  if (hasAnyPrefix({"EY"})) return "tj";
  if (hasAnyPrefix({"EZ"})) return "tm";
  if (hasAnyPrefix({"S5"})) return "si";
  if (hasAnyPrefix({"9A"})) return "hr";
  if (hasAnyPrefix({"HA","HG"})) return "hu";
  if (hasAnyPrefix({"HF","3Z","SN","SO","SP","SQ","SR"})) return "pl";
  if (hasAnyPrefix({"YO","YP","YQ","YR"})) return "ro";
  if (hasAnyPrefix({"YT","YU"})) return "rs";
  if (hasAnyPrefix({"Z3"})) return "mk";
  if (hasAnyPrefix({"E7"})) return "ba";
  if (hasAnyPrefix({"4O"})) return "me";
  if (hasAnyPrefix({"SV","SW","SX","SY","SZ","J4"})) return "gr";
  if (hasAnyPrefix({"TA","TB","TC","YM"})) return "tr";
  if (hasAnyPrefix({"T7"})) return "sm";
  if (hasAnyPrefix({"CT","CQ","CR","CS","CU"})) return "pt";
  if (hasAnyPrefix({"EI","EJ"})) return "ie";
  if (hasAnyPrefix({"UA","UB","UC","UD","UE","UF","UG","UH","UI","RA","RB","RC","RD","RE","RF","RG","RH","RI","RJ","RK","RL","RM","RN","RO","RP","RQ","RR","RS","RT","RU","RV","RW","RX","RY","RZ"})) return "ru";
  if (hasAnyPrefix({"UR","US","UT","UU","UV","UW","UX","UY","UZ","EM","EN","EO"})) return "ua";
  if (hasAnyPrefix({"UJ","UK","UL","UM"})) return "uz";
  if (hasAnyPrefix({"UN","UO","UP","UQ"})) return "kz";

  if (hasAnyPrefix({"K","N","W","AA","AB","AC","AD","AE","AF","AG","AH","AI","AJ","AK","KA","KB","KC","KD","KE","KF","KG","KH","KI","KJ","KK","KL","KM","KN","KO","KP","KQ","KR","KS","KT","KU","KV","KW","KX","KY","KZ",
                    "NA","NB","NC","ND","NE","NF","NG","NH","NI","NJ","NK","NL","NM","NN","NO","NP","NQ","NR","NS","NT","NU","NV","NW","NX","NY","NZ",
                    "WA","WB","WC","WD","WE","WF","WG","WH","WI","WJ","WK","WL","WM","WN","WO","WP","WQ","WR","WS","WT","WU","WV","WW","WX","WY","WZ"})) return "us";
  if (hasAnyPrefix({"CF","CG","CH","CI","CJ","CK","CY","CZ","VA","VB","VC","VD","VE","VF","VG","VO","VX","VY","XJ","XK","XL","XM","XN","XO"})) return "ca";
  if (hasAnyPrefix({"XA","XB","XC","XD","XE","XF","XG","XH","XI","4A","4B","4C","6D","6E","6F","6G","6H","6I","6J"})) return "mx";
  if (hasAnyPrefix({"V2"})) return "ag";
  if (hasAnyPrefix({"V3"})) return "bz";
  if (hasAnyPrefix({"V4"})) return "kn";
  if (hasAnyPrefix({"J3"})) return "gd";
  if (hasAnyPrefix({"J6"})) return "lc";
  if (hasAnyPrefix({"J7"})) return "dm";
  if (hasAnyPrefix({"J8"})) return "vc";
  if (hasAnyPrefix({"8P"})) return "bb";
  if (hasAnyPrefix({"8R"})) return "gy";
  if (hasAnyPrefix({"9Y","9Z"})) return "tt";
  if (hasAnyPrefix({"C6"})) return "bs";
  if (hasAnyPrefix({"CO","CL","CM","T4"})) return "cu";
  if (hasAnyPrefix({"HP","HO","H3","H8","H9","3E","3F"})) return "pa";
  if (hasAnyPrefix({"P4"})) return "aw";
  if (hasAnyPrefix({"PJ"})) return "cw";
  if (hasAnyPrefix({"PZ"})) return "sr";
  if (hasAnyPrefix({"TI","TE"})) return "cr";
  if (hasAnyPrefix({"TG","TD"})) return "gt";
  if (hasAnyPrefix({"YN","H6","H7","HT"})) return "ni";
  if (hasAnyPrefix({"HR","HQ"})) return "hn";
  if (hasAnyPrefix({"HU","YS"})) return "sv";
  if (hasAnyPrefix({"HI"})) return "do";
  if (hasAnyPrefix({"HH","4V"})) return "ht";

  if (hasAnyPrefix({"AY","AZ","L2","L3","L4","L5","L6","L7","L8","L9","LO","LP","LQ","LR","LS","LT","LU","LV","LW"})) return "ar";
  if (hasAnyPrefix({"CP"})) return "bo";
  if (hasAnyPrefix({"CX","CV","CW"})) return "uy";
  if (hasAnyPrefix({"HC","HD"})) return "ec";
  if (hasAnyPrefix({"HK","HJ","5J","5K"})) return "co";
  if (hasAnyPrefix({"OA","OB","OC","4T"})) return "pe";
  if (hasAnyPrefix({"YV","YW","YX","YY","4M"})) return "ve";
  if (hasAnyPrefix({"PP","PQ","PR","PS","PT","PU","PV","PW","PX","PY","ZV","ZW","ZX","ZY","ZZ"})) return "br";
  if (hasAnyPrefix({"CA","CB","CC","CD","CE","3G","XQ","XR"})) return "cl";
  if (hasAnyPrefix({"ZP"})) return "py";

  if (hasAnyPrefix({"A2","8O"})) return "bw";
  if (hasAnyPrefix({"3B"})) return "mu";
  if (hasAnyPrefix({"3C"})) return "gq";
  if (hasPrefix("3D")) return c.startsWith("3DN") ? "fj" : "sz";
  if (hasAnyPrefix({"3X"})) return "gn";
  if (hasAnyPrefix({"5A"})) return "ly";
  if (hasAnyPrefix({"5H","5I"})) return "tz";
  if (hasAnyPrefix({"5L","5M","6Z","A8","D5","EL"})) return "lr";
  if (hasAnyPrefix({"5N","5O"})) return "ng";
  if (hasAnyPrefix({"5R","5S","6X"})) return "mg";
  if (hasAnyPrefix({"5T"})) return "mr";
  if (hasAnyPrefix({"5U"})) return "ne";
  if (hasAnyPrefix({"5V"})) return "tg";
  if (hasAnyPrefix({"5X"})) return "ug";
  if (hasAnyPrefix({"5Y","5Z"})) return "ke";
  if (hasAnyPrefix({"6V","6W"})) return "sn";
  if (hasAnyPrefix({"7P"})) return "ls";
  if (hasAnyPrefix({"7Q"})) return "mw";
  if (hasAnyPrefix({"7R","7T","7U","7V","7W","7X","7Y"})) return "dz";
  if (hasAnyPrefix({"9E","9F","ET"})) return "et";
  if (hasAnyPrefix({"9G"})) return "gh";
  if (hasAnyPrefix({"9I","9J"})) return "zm";
  if (hasAnyPrefix({"9L"})) return "sl";
  if (hasAnyPrefix({"9O","9P","9Q","9R","9S","9T"})) return "cd";
  if (hasAnyPrefix({"9U"})) return "bi";
  if (hasAnyPrefix({"9X"})) return "rw";
  if (hasAnyPrefix({"C5"})) return "gm";
  if (hasAnyPrefix({"C8","C9"})) return "mz";
  if (hasAnyPrefix({"CN","5C","5D","5E","5F","5G"})) return "ma";
  if (hasAnyPrefix({"D2","D3"})) return "ao";
  if (hasAnyPrefix({"D4"})) return "cv";
  if (hasAnyPrefix({"D6"})) return "km";
  if (hasAnyPrefix({"J2"})) return "dj";
  if (hasAnyPrefix({"S7"})) return "sc";
  if (hasAnyPrefix({"S8","ZR","ZS","ZT","ZU"})) return "za";
  if (hasAnyPrefix({"S9"})) return "st";
  if (hasAnyPrefix({"SU","SSA","SSB","SSC","SSD","SSE","SSF","SSG","SSH","SSI","SSJ","SSK","SSL","SSM"})) return "eg";
  if (hasAnyPrefix({"SSN","SSO","SSP","SSQ","SSR","SSS","SST","SSU","SSV","SSW","SSX","SSY","SSZ","ST","6T","6U"})) return "sd";
  if (hasAnyPrefix({"TJ"})) return "cm";
  if (hasAnyPrefix({"TL"})) return "cf";
  if (hasAnyPrefix({"TN"})) return "cg";
  if (hasAnyPrefix({"TR"})) return "ga";
  if (hasAnyPrefix({"TS","3V"})) return "tn";
  if (hasAnyPrefix({"TT"})) return "td";
  if (hasAnyPrefix({"TU"})) return "ci";
  if (hasAnyPrefix({"TY"})) return "bj";
  if (hasAnyPrefix({"TZ"})) return "ml";
  if (hasAnyPrefix({"XT"})) return "bf";
  if (hasAnyPrefix({"Z2"})) return "zw";
  if (hasAnyPrefix({"Z8"})) return "ss";

  if (hasAnyPrefix({"4J","4K"})) return "az";
  if (hasAnyPrefix({"4L"})) return "ge";
  if (hasAnyPrefix({"4X","4Z"})) return "il";
  if (hasAnyPrefix({"5B","C4","H2","P3"})) return "cy";
  if (hasAnyPrefix({"7O"})) return "ye";
  if (hasAnyPrefix({"7Z","8Z","HZ"})) return "sa";
  if (hasAnyPrefix({"9K"})) return "kw";
  if (hasAnyPrefix({"9N"})) return "np";
  if (hasAnyPrefix({"A4"})) return "om";
  if (hasAnyPrefix({"A5"})) return "bt";
  if (hasAnyPrefix({"A6"})) return "ae";
  if (hasAnyPrefix({"A7"})) return "qa";
  if (hasAnyPrefix({"A9"})) return "bh";
  if (hasAnyPrefix({"AP","AQ","AR","AS","6P","6Q","6R","6S"})) return "pk";
  if (hasAnyPrefix({"EP","EQ","9B","9C","9D"})) return "ir";
  if (hasAnyPrefix({"HN","YI"})) return "iq";
  if (hasAnyPrefix({"JY"})) return "jo";
  if (hasAnyPrefix({"OD"})) return "lb";
  if (hasAnyPrefix({"YK","6C"})) return "sy";
  if (hasAnyPrefix({"E4"})) return "ps";
  if (hasAnyPrefix({"T6","YA"})) return "af";

  if (hasAnyPrefix({"8Q"})) return "mv";
  if (hasAnyPrefix({"8T","8U","8V","8W","8X","8Y","AT","AU","AV","AW","VT","VU","VV","VW"})) return "in";
  if (hasAnyPrefix({"9M","9W"})) return "my";
  if (hasAnyPrefix({"9V","S6"})) return "sg";
  if (hasAnyPrefix({"B","3H","3I","3J","3K","3L","3M","3N","3O","3P","3Q","3R","3S","3T","3U","BA","BB","BC","BD","BE","BF","BG","BH","BI","BJ","BK","BL","BM","BN","BO","BP","BQ","BR","BS","BT","BU","BV","BW","BX","BY","BZ","XS"})) return "cn";
  if (hasAnyPrefix({"BV"})) return "tw";
  if (hasAnyPrefix({"DS","DT","D7","D8","D9","6K","6L","6M","6N","HL"})) return "kr";
  if (hasAnyPrefix({"HM","P5","P6","P7","P8","P9"})) return "kp";
  if (hasAnyPrefix({"E2","HS"})) return "th";
  if (hasAnyPrefix({"JA","JB","JC","JD","JE","JF","JG","JH","JI","JJ","JK","JL","JM","JN","JO","JP","JQ","JR","JS","7J","7K","7L","7M","7N","8J","8K","8L","8M","8N"})) return "jp";
  if (hasAnyPrefix({"JT","JU","JV"})) return "mn";
  if (hasAnyPrefix({"PK","PL","PM","PN","PO","7A","7B","7C","7D","7E","7F","7G","7H","7I","8A","8B","8C","8D","8E","8F","8G","8H","8I","JZ","YB","YC","YD","YE","YF","YG","YH"})) return "id";
  if (hasAnyPrefix({"V8"})) return "bn";
  if (hasAnyPrefix({"XU"})) return "kh";
  if (hasAnyPrefix({"XV","3W"})) return "vn";
  if (hasAnyPrefix({"XW"})) return "la";
  if (hasAnyPrefix({"XY","XZ"})) return "mm";
  if (hasAnyPrefix({"P2"})) return "pg";
  if (hasAnyPrefix({"A3"})) return "to";
  if (hasAnyPrefix({"C2"})) return "nr";
  if (hasAnyPrefix({"E5"})) return "ck";
  if (hasAnyPrefix({"E6"})) return "nu";
  if (hasAnyPrefix({"H4"})) return "sb";
  if (hasAnyPrefix({"T2"})) return "tv";
  if (hasAnyPrefix({"T3"})) return "ki";
  if (hasAnyPrefix({"T8"})) return "pw";
  if (hasAnyPrefix({"V6"})) return "fm";
  if (hasAnyPrefix({"V7"})) return "mh";
  if (hasAnyPrefix({"YJ"})) return "vu";
  if (hasAnyPrefix({"4W"})) return "tl";
  if (hasAnyPrefix({"AX","VH","VI","VJ","VK","VL","VM","VN","VZ"})) return "au";
  if (hasAnyPrefix({"ZK","ZL","ZM"})) return "nz";
  if (hasAnyPrefix({"5W"})) return "ws";

  return "de";
}

uint16_t bmpRead16(fs::File &f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read();
  ((uint8_t *)&result)[1] = f.read();
  return result;
}

uint32_t bmpRead32(fs::File &f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read();
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read();
  return result;
}

bool drawBmp24FromSD(const char *filename, int16_t x, int16_t y, int16_t maxW, int16_t maxH) {
  File bmpFS = storageOpen(filename, FILE_READ);
  if (!bmpFS) return false;

  if (bmpRead16(bmpFS) != 0x4D42) {
    bmpFS.close();
    return false;
  }

  bmpRead32(bmpFS);
  bmpRead32(bmpFS);
  uint32_t imageOffset = bmpRead32(bmpFS);
  uint32_t headerSize = bmpRead32(bmpFS);
  int32_t bmpWidth = (int32_t)bmpRead32(bmpFS);
  int32_t bmpHeight = (int32_t)bmpRead32(bmpFS);

  if (bmpRead16(bmpFS) != 1) {
    bmpFS.close();
    return false;
  }

  uint16_t depth = bmpRead16(bmpFS);
  uint32_t compression = bmpRead32(bmpFS);

  if (headerSize < 40 || depth != 24 || compression != 0) {
    bmpFS.close();
    return false;
  }

  bool flip = true;
  if (bmpHeight < 0) {
    bmpHeight = -bmpHeight;
    flip = false;
  }

  if (bmpWidth <= 0 || bmpHeight <= 0) {
    bmpFS.close();
    return false;
  }

  if (bmpWidth > maxW || bmpHeight > maxH) {
    bmpFS.close();
    return false;
  }

  uint32_t rowSize = (bmpWidth * 3 + 3) & ~3;
  uint8_t sdbuffer[3 * 20];
  uint8_t buffidx = sizeof(sdbuffer);

  for (int row = 0; row < bmpHeight; row++) {
    uint32_t pos = imageOffset + (flip ? (bmpHeight - 1 - row) : row) * rowSize;
    bmpFS.seek(pos);

    buffidx = sizeof(sdbuffer);

    for (int col = 0; col < bmpWidth; col++) {
      if (buffidx >= sizeof(sdbuffer)) {
        bmpFS.read(sdbuffer, sizeof(sdbuffer));
        buffidx = 0;
      }

      uint8_t b = sdbuffer[buffidx++];
      uint8_t g = sdbuffer[buffidx++];
      uint8_t r = sdbuffer[buffidx++];

#if DISPLAY_BACKEND == DISPLAY_BACKEND_2432S028R
      tft.drawPixel(x + col, y + row, rgb888To565(r, g, b));
#elif DISPLAY_BACKEND_IS_GFX_COMPAT
      tft.drawPixel(x + 8 + col, y + 8 + row, rgb888To565(r, g, b));
#else

#endif
    }
  }

  bmpFS.close();
  return true;
}

void *pngOpen(const char *filename, int32_t *size) {
  pngFile = storageOpen(filename, FILE_READ);
  if (!pngFile) {
    *size = 0;
    return nullptr;
  }

  *size = pngFile.size();
  return &pngFile;
}

void pngClose(void *handle) {
  (void)handle;
  if (pngFile) pngFile.close();
}

int32_t pngRead(PNGFILE *page, uint8_t *buffer, int32_t length) {
  (void)page;
  if (!pngFile) return 0;
  return pngFile.read(buffer, length);
}

int32_t pngSeek(PNGFILE *page, int32_t position) {
  (void)page;
  if (!pngFile) return 0;
  return pngFile.seek(position) ? position : 0;
}

int pngDraw(PNGDRAW *pDraw) {
  if (!pngLineBuffer || !pngDecodeOk) return 0;
  if (pDraw->iWidth <= 0 || pDraw->iWidth > pngMaxW || (pDraw->y + 1) > pngMaxH) {
    pngDecodeOk = false;
    return 0;
  }

  if (pngHasAlpha && pngPixelType == PNG_PIXEL_TRUECOLOR_ALPHA && pDraw->pPixels) {
    const uint8_t* s = (const uint8_t*)pDraw->pPixels;
    for (int x = 0; x < pDraw->iWidth; ++x) {
      uint8_t fr = s[0];
      uint8_t fg = s[1];
      uint8_t fb = s[2];
      uint8_t fa = s[3];
      s += 4;

      if (fa == 0) {
        pngLineBuffer[x] = pngBlendBg565Buffer;
      } else if (fa == 255) {
        pngLineBuffer[x] = pngBufferColor565(rgb888To565(fr, fg, fb));
      } else {
        uint16_t inv = 255 - fa;
        uint8_t r = (uint8_t)((fr * fa + pngBlendBgR * inv + 127) / 255);
        uint8_t g = (uint8_t)((fg * fa + pngBlendBgG * inv + 127) / 255);
        uint8_t b = (uint8_t)((fb * fa + pngBlendBgB * inv + 127) / 255);
        pngLineBuffer[x] = pngBufferColor565(rgb888To565(r, g, b));
      }
    }
  } else {
    pngDecoder.getLineAsRGB565(pDraw, pngLineBuffer, PNG_RGB565_BIG_ENDIAN, 0xffffffff);

#if DISPLAY_BACKEND == DISPLAY_BACKEND_2432S028R
    if (pngHasAlpha) {
      size_t neededMaskBytes = ((size_t)pDraw->iWidth + 7U) / 8U;
      if (pngMaskBufferBytes < neededMaskBytes) {
        uint8_t* newMask = (uint8_t*)realloc(pngMaskBuffer, neededMaskBytes);
        if (!newMask) {
          pngDecodeOk = false;
          return 0;
        }
        pngMaskBuffer = newMask;
        pngMaskBufferBytes = neededMaskBytes;
      }

      if (pngMaskBuffer && pngDecoder.getAlphaMask(pDraw, pngMaskBuffer, 255)) {
        for (int x = 0; x < pDraw->iWidth; ++x) {
          uint8_t maskByte = pngMaskBuffer[(size_t)x >> 3];
          uint8_t maskBit = 0x80 >> (x & 7);
          if ((maskByte & maskBit) == 0) {
            pngLineBuffer[x] = pngBlendBg565Buffer;
          }
        }
      } else {
        for (int x = 0; x < pDraw->iWidth; ++x) {
          pngLineBuffer[x] = pngBlendBg565Buffer;
        }
      }
    }
#endif
  }

#if DISPLAY_BACKEND == DISPLAY_BACKEND_2432S028R
  tft.pushImage(pngDrawX, pngDrawY + pDraw->y, pDraw->iWidth, 1, pngLineBuffer);
#elif DISPLAY_BACKEND_IS_GFX_COMPAT
  tft.pushImage(pngDrawX, pngDrawY + pDraw->y, pDraw->iWidth, 1, pngLineBuffer);
#endif
  return 1;
}

bool drawPngFromSD(const char *filename, int16_t x, int16_t y, int16_t maxW, int16_t maxH, uint16_t bg565) {
  if (!filename || maxW <= 0 || maxH <= 0) return false;

  pngDrawX = x;
  pngDrawY = y;
  pngMaxW = maxW;
  pngMaxH = maxH;
  pngDecodeOk = true;
  pngBlendBg565 = bg565;
  pngBlendBg565Buffer = pngBufferColor565(bg565);
  rgb565To888Components(bg565, pngBlendBgR, pngBlendBgG, pngBlendBgB);
  pngPixelType = 0;
  pngHasAlpha = false;

  int32_t rc = pngDecoder.open(filename, pngOpen, pngClose, pngRead, pngSeek, pngDraw);
  if (rc != PNG_SUCCESS) {
    pngClose(nullptr);
    return false;
  }

  pngPixelType = pngDecoder.getPixelType();
  pngHasAlpha = pngDecoder.hasAlpha();

  int32_t pngWidth = pngDecoder.getWidth();
  int32_t pngHeight = pngDecoder.getHeight();
  if (pngWidth <= 0 || pngHeight <= 0 || pngWidth > maxW || pngHeight > maxH) {
    pngDecoder.close();
    pngClose(nullptr);
    return false;
  }

  size_t neededPixels = (size_t)pngWidth;
  if (pngLineBufferPixels < neededPixels) {
    uint16_t* newBuffer = (uint16_t*)realloc(pngLineBuffer, neededPixels * sizeof(uint16_t));
    if (!newBuffer) {
      pngDecoder.close();
      pngClose(nullptr);
      return false;
    }
    pngLineBuffer = newBuffer;
    pngLineBufferPixels = neededPixels;
  }

#if DISPLAY_BACKEND == DISPLAY_BACKEND_2432S028R
  tft.startWrite();
#endif
  rc = pngDecoder.decode(nullptr, 0);
#if DISPLAY_BACKEND == DISPLAY_BACKEND_2432S028R
  tft.endWrite();
#endif

  pngDecoder.close();
  pngClose(nullptr);
  return pngDecodeOk && (rc == PNG_SUCCESS);
}

void drawFlagOrPlaceholder(int x, int y, int w, int h, const String& code) {

  fillPanel(x, y, w, h, theme.panel2, theme.border);

  if (code.length()) {
    String path = "/flags/" + code + ".bmp";
#if DISPLAY_BACKEND_IS_GFX_COMPAT
    if (drawBmp24FromSD(path.c_str(), x + UW(11), y + UH(5) - 4, w - UW(10), h - UH(10))) {
#else
    if (drawBmp24FromSD(path.c_str(), x + UW(11), y + UH(5), w - UW(10), h - UH(10))) {
#endif
      return;
    }
  }

  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(theme.accent, theme.panel2);
  tft.setTextFont(2);
  tft.drawString(code.length() ? code : "--", x + w / 2, y + h / 2);
}

void drawBrightnessSlider() {
  int rowX = UX(14);
  int rowY = UY(186);
  int rowW = UW(290);
  int rowH = UH(22);

  fillSoftPanel(rowX, rowY, UW(76), rowH, theme.panel2, theme.border, 5);
  fillSoftPanel(rowX + UW(80), rowY, rowW - UW(80), rowH, theme.panel2, theme.border, 5);

  tft.setTextDatum(ML_DATUM);
  tft.setTextFont(2);
  tft.setTextColor(theme.warn, theme.panel2);
  tft.drawString("BRIGHT", rowX + UW(8), rowY + rowH / 2);

  tft.fillRoundRect(BRIGHT_SLIDER_X, BRIGHT_SLIDER_Y, BRIGHT_SLIDER_W, BRIGHT_SLIDER_H, UR(4), theme.panel);
  tft.drawRoundRect(BRIGHT_SLIDER_X, BRIGHT_SLIDER_Y, BRIGHT_SLIDER_W, BRIGHT_SLIDER_H, UR(4), theme.border);

  int knobX = map(cfg.brightness, 0, 255,
                  BRIGHT_SLIDER_X + max(1, UW(4)),
                  BRIGHT_SLIDER_X + BRIGHT_SLIDER_W - max(1, UW(5)));
  int fillW = knobX - BRIGHT_SLIDER_X;
  if (fillW > 0) tft.fillRoundRect(BRIGHT_SLIDER_X, BRIGHT_SLIDER_Y, fillW, BRIGHT_SLIDER_H, UR(4), theme.accent);
  tft.fillCircle(knobX, BRIGHT_SLIDER_Y + BRIGHT_SLIDER_H / 2, max(2, UR(5)), theme.warn);

  tft.setTextDatum(MR_DATUM);
  tft.setTextFont(1);
  tft.setTextColor(theme.text, theme.panel2);
  tft.drawString(String(cfg.brightness), rowX + rowW - UW(8), rowY + rowH / 2);
}

void drawLivePage() {
  if (isSvxlinkBackend()) {
    drawSvxlinkLivePage();
    return;
  }
  if (isAprsBackend()) {
    drawAprsLivePage();
    return;
  }
  if (isWeatherBackend()) {
    drawWeatherLivePage();
    return;
  }
  if (isHamqslBackend()) {
    drawHamqslLivePage();
    return;
  }

  drawTopTabs();

  HeardRecord rec;
  bool active = isLiveActive();
  if (active) rec = liveRecord;

  String activeMode = active ? normalizeModeName(rec.mode) : "";
  bool dmrActive = active && activeMode == "DMR";
  bool dstarActive = active && (activeMode == "DSTAR" || activeMode == "D-STAR");
  bool ysfActive = active && (activeMode == "YSF" || activeMode == "FUSION");
  bool p25Active = active && activeMode == "P25";
  bool nxdnActive = active && activeMode == "NXDN";
  bool pocsagActive = active && activeMode == "POCSAG";

  drawModeButton(UX(10), UY(38), UW(70), UH(20), "DMR", modeDMR, dmrActive);
  drawModeButton(UX(86), UY(38), UW(70), UH(20), "DSTAR", modeDSTAR, dstarActive);
  drawModeButton(UX(10), UY(62), UW(70), UH(20), "YSF", modeYSF, ysfActive);
  drawModeButton(UX(86), UY(62), UW(70), UH(20), "P25", modeP25, p25Active);
  drawModeButton(UX(10), UY(86), UW(70), UH(20), "NXDN", modeNXDN, nxdnActive);
  drawModeButton(UX(86), UY(86), UW(70), UH(20), "POCSAG", modePOCSAG, pocsagActive);

  drawSignalMeter(UX(170), UY(44), UW(140), UH(54), active, active ? &rec : nullptr);

  String operatorName = active ? radioIdDisplayNameForRecord(rec, true) : "";
  String operatorLine = active ? formatCallsignName(rec.callsign, operatorName, 0, 0) : "-";
  String statusLine = liveStatusText(active, rec);
  String ts1Line = active ? resolveTargetDisplay(rec) : "-";
  String rxFreq = hotspotRxFreq.length() ? hotspotRxFreq : currentRxFreq;
  String txFreq = hotspotTxFreq.length() ? hotspotTxFreq : currentTxFreq;
  if (!rxFreq.length()) rxFreq = (normalizedBackend() == "wpsd") ? "Auto" : "438.8000";
  if (!txFreq.length()) txFreq = (normalizedBackend() == "wpsd") ? "Auto" : "430.8000";

  if (shouldShowDmrTimeslotLivePanel(active, rec)) {
    drawDmrTimeslotLivePanel(UX(10), UY(112), UW(300), UH(68));
  } else {
    drawLiveInfoPanel(UX(10), UY(112), UW(300), UH(68), operatorLine, statusLine, ts1Line, active);
  }

  drawFrequencyBox(UX(10), UY(190), UW(104), "TX", txFreq);
  drawQuickActionLauncherBox(UX(120), UY(190), UW(80));
  drawFrequencyBox(UX(206), UY(190), UW(104), "RX", rxFreq);

  drawFooter();
}

void drawLastPage() {
  if (isWeatherBackend()) {
    drawWeatherLivePage();
    return;
  }
  if (isHamqslBackend()) {
    drawHamqslLivePage();
    return;
  }
  if (isSvxlinkBackend()) {
    drawSvxlinkHeardPage();
    return;
  }

  drawTopTabs();
  normalizeLastHeardBrowseState();

  fillSoftPanel(UX(6), UY(36), UW(308), UH(184), theme.panel, theme.border, 12);

  tft.setTextDatum(TL_DATUM);
  tft.setTextFont(2);
  tft.setTextColor(theme.accent, theme.panel);
  tft.drawString("LAST HEARD", UX(16), UY(46));

  tft.setTextColor(theme.subtext, theme.panel);
  tft.setTextFont(1);
  tft.drawString("Tap rows / Filter, sort and browse pages", UX(16), UY(64));

  int visibleIndices[MAX_HEARD];
  int visibleCount = buildLastHeardVisibleIndices(visibleIndices, MAX_HEARD);
  int pageCount = visibleCount > 0 ? ((visibleCount + LAST_HEARD_ROWS_PER_PAGE - 1) / LAST_HEARD_ROWS_PER_PAGE) : 0;
  int pageStart = lastHeardPageIndex * LAST_HEARD_ROWS_PER_PAGE;
  int rows = max(0, min((int)LAST_HEARD_ROWS_PER_PAGE, visibleCount - pageStart));

  const int navY = UY(76);
  fillSoftPanel(UX(14), navY, UW(24), UH(20), theme.panel2, theme.border, 6);
  fillSoftPanel(UX(44), navY, UW(68), UH(20), theme.panel2, theme.border, 6);
  fillSoftPanel(UX(118), navY, UW(96), UH(20), theme.panel2, theme.border, 6);
  fillSoftPanel(UX(220), navY, UW(56), UH(20), theme.panel2, theme.border, 6);
  fillSoftPanel(UX(282), navY, UW(24), UH(20), theme.panel2, theme.border, 6);

  tft.setTextDatum(MC_DATUM);
  tft.setTextFont(2);
  tft.setTextColor(lastHeardPageIndex > 0 ? theme.text : theme.subtext, theme.panel2);
  tft.drawString("<", UX(26), navY + UH(10));
  tft.setTextColor(theme.warn, theme.panel2);
  tft.drawString(getLastHeardFilterLabel(), UX(78), navY + UH(10));
  tft.setTextColor(theme.text, theme.panel2);
  tft.drawString(getLastHeardSortLabel(), UX(166), navY + UH(10));
  tft.setTextColor(theme.subtext, theme.panel2);
  String pageText = visibleCount > 0 ? (String(lastHeardPageIndex + 1) + "/" + String(max(1, pageCount))) : String("--");
  tft.drawString(pageText, UX(248), navY + UH(10));
  tft.setTextColor((lastHeardPageIndex + 1) < pageCount ? theme.text : theme.subtext, theme.panel2);
  tft.drawString(">", UX(294), navY + UH(10));

  int y = UY(104);
  tft.setTextDatum(TL_DATUM);

  for (int row = 0; row < rows; row++) {
    HeardRecord& r = heardList[visibleIndices[pageStart + row]];
    uint16_t lineBg = (row % 2 == 0) ? theme.panel2 : mix565(theme.panel2, theme.bg, 0.18f);
    fillSoftPanel(UX(14), y, UW(292), UH(24), lineBg, theme.border, 6);

    tft.setTextFont(2);
    tft.setTextColor(theme.accent, lineBg);
    tft.drawString(clipWithTilde(normalizeHamCallsign(r.callsign), 9), UX(20), y + UY(5));

    tft.setTextColor(theme.warn, lineBg);
    tft.drawString(clipWithTilde(normalizeModeName(r.mode), 6), UX(92), y + UY(5));

    tft.setTextColor(theme.text, lineBg);
    tft.drawString(clipWithTilde(resolveTargetDisplay(r), 13), UX(136), y + UY(5));

    tft.setTextColor(theme.subtext, lineBg);
    String timeText = r.timeUtc.length() > 11 ? r.timeUtc.substring(11) : r.timeUtc;
    tft.drawString(clipWithTilde(timeText, 6), UX(250), y + UY(5));

    y += UH(28);
  }

  if (visibleCount == 0) {
    tft.setTextColor(theme.subtext, theme.panel);
    tft.setTextFont(2);
    tft.drawString(heardCount == 0 ? "No traffic received yet" : "No rows match this filter", UX(18), UY(118));
  }

  drawFooter();
}

void drawSetupPage() {
  drawTopTabs();

  fillSoftPanel(UX(6), UY(36), UW(308), UH(184), theme.panel, theme.border, 12);

  tft.setTextDatum(TL_DATUM);
  tft.setTextFont(2);
  tft.setTextColor(theme.accent, theme.panel);
  tft.drawString("CONFIG / STATUS", UX(14), UY(46));

  drawInfoRow(UX(14), UY(66), UW(290), "Wi-Fi", wifiOk ? "Connected" : "Disconnected", theme.panel2, theme.panel2,
              theme.warn, wifiOk ? theme.good : theme.bad);
  drawInfoRow(UX(14), UY(90), UW(290), "SSID", cfg.wifiSsid, theme.panel2, theme.panel2, theme.warn, theme.text);
  drawInfoRow(UX(14), UY(114), UW(290), "HOST", WiFi.localIP().toString(), theme.panel2, theme.panel2, theme.warn, theme.text);
  drawInfoRow(UX(14), UY(138), UW(290), "CALLSIGN", getDeviceCallsign(), theme.panel2, theme.panel2, theme.warn, theme.good);
  drawInfoRow(UX(14), UY(162), UW(138), "REFRESH", String(cfg.refreshMs) + " ms", theme.panel2, theme.panel2, theme.warn, theme.text);
  drawInfoRow(UX(166), UY(162), UW(138), "FLIP", cfg.flipScreen ? "On" : "Off", theme.panel2, theme.panel2, theme.warn, cfg.flipScreen ? theme.good : theme.subtext);
  drawInfoRow(UX(14), UY(186), UW(138), "SAVER", cfg.screensaverEnabled ? (String(cfg.screensaverTimeoutMs / 1000) + " s") : "Off", theme.panel2, theme.panel2, theme.warn, cfg.screensaverEnabled ? theme.good : theme.subtext);

  drawBrightnessSlider();
  drawFooter();
}

void drawApModePage() {
  drawTopTabs();

  fillSoftPanel(UX(6), UY(36), UW(308), UH(184), theme.panel, theme.border, 12);

  tft.setTextDatum(TL_DATUM);
  tft.setTextFont(2);
  tft.setTextColor(theme.accent, theme.panel);
  tft.drawString("ACCESS POINT MODE", UX(14), UY(46));

  String modeText = apModeActive ? "Active" : "Inactive";
  String ipText = apModeActive ? WiFi.softAPIP().toString() : "-";
  String stateText = setupRequired ? "Setup required before normal run" : "Optional AP status";
  String ssidText = apSsid.length() ? apSsid : "-";
  String passText = apModeActive ? apPass : "-";

  drawInfoRow(UX(14), UY(66), UW(290), "AP MODE", modeText, theme.panel2, theme.panel2,
              theme.warn, apModeActive ? theme.good : theme.subtext);
  drawInfoRow(UX(14), UY(90), UW(290), "SSID", ssidText, theme.panel2, theme.panel2, theme.warn, theme.text);
  drawInfoRow(UX(14), UY(114), UW(290), "PASSWORD", passText, theme.panel2, theme.panel2, theme.warn, theme.text);
  drawInfoRow(UX(14), UY(138), UW(290), "OPEN", ipText, theme.panel2, theme.panel2, theme.warn, theme.good);
  drawInfoRow(UX(14), UY(162), UW(290), "STORAGE", activeStorageLabel, theme.panel2, theme.panel2, theme.warn, theme.text);

  fillSoftPanel(UX(14), UY(188), UW(290), UH(22), theme.panel2, theme.border, 6);
  tft.setTextDatum(TL_DATUM);
  tft.setTextFont(1);
  tft.setTextColor(theme.subtext, theme.panel2);
  tft.drawString(stateText, UX(20), UY(195));

  drawFooter();
}

#if DISPLAY_BACKEND_IS_DESKTOP
void drawDesktopBootReadyPage() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(TL_DATUM);

  tft.fillRect(0, 0, SCREEN_W, SCREEN_H, rgb888To565(8, 12, 22));
  tft.drawRect(18, 18, SCREEN_W - 36, SCREEN_H - 36, rgb888To565(70, 92, 120));
  tft.fillRect(32, 32, SCREEN_W - 64, 96, rgb888To565(18, 28, 45));

  tft.setTextFont(2);
  tft.setTextColor(rgb888To565(70, 200, 255), rgb888To565(18, 28, 45));
  tft.drawString("MMDVM-Display - DESKTOP", 52, 56);

  tft.setTextFont(1);
  tft.setTextColor(TFT_WHITE, rgb888To565(18, 28, 45));
  tft.drawString("SDL2 backend is running", 52, 92);

  tft.setTextColor(rgb888To565(220, 230, 245), rgb888To565(8, 12, 22));
  tft.drawString("Runtime size: 800 x 480", 52, 160);
  tft.drawString(setupRequired ? "Mode: desktop setup simulation" : "Mode: desktop normal simulation", 52, 186);
  tft.drawString(String("AP/Web stub: http://") + WiFi.softAPIP().toString() + "/", 52, 212);
  tft.drawString(String("Storage: ") + activeStorageLabel, 52, 238);

  tft.setTextColor(rgb888To565(255, 190, 70), rgb888To565(8, 12, 22));
  tft.drawString("Desktop safe loop is active.", 52, 292);
  tft.drawString("No real HTTP listener is started in this desktop stub.", 52, 318);

  tft.setTextColor(rgb888To565(160, 175, 195), rgb888To565(8, 12, 22));
  tft.drawString("Close the SDL window to exit.", 52, 390);
  tft.drawString("If text is readable, the built-in bitmap font works.", 52, 416);

  tft.flush();
}
#endif

void drawInfoPage() {
  drawTopTabs();

  const int cardX = UX(6);
  const int cardY = UY(36);
  const int cardW = SCREEN_W - UW(12);
  const int cardH = UH(184);

  fillSoftPanel(cardX, cardY, cardW, cardH, theme.panel2, theme.border, 12);
  fillSoftPanel(cardX + UW(8), cardY + UH(8), cardW - UW(16), UH(42), theme.panel, theme.border, 10);

  tft.setTextDatum(TL_DATUM);
  tft.setTextFont(2);
  tft.setTextColor(theme.accent, theme.panel);
  tft.drawString("SESSION STATISTICS", cardX + UW(18), cardY + UH(14));
  tft.setTextColor(theme.subtext, theme.panel);
  tft.setTextFont(1);
  tft.drawString(getDateText() + "  " + getDetailedClockText(), cardX + UW(18), cardY + UH(32));

  drawInfoRow(cardX + UW(14), cardY + UH(58), cardW - UW(28), "DEVICE", getDeviceCallsign(), theme.panel, theme.panel2, theme.warn, theme.good);
  drawInfoRow(cardX + UW(14), cardY + UH(80), cardW - UW(28), "UPTIME", formatUptimeBrief(), theme.panel, theme.panel2, theme.warn, theme.text);
  drawInfoRow(cardX + UW(14), cardY + UH(102), cardW - UW(28), "TRAFFIC", String(sessionInfo.totalQsos) + " hits", theme.panel, theme.panel2, theme.warn, theme.text);
  String rssiSummary = sessionInfo.hasRssi ? (String(sessionInfo.bestRssi) + "/" + String(sessionInfo.worstRssi) + " dBm") : "-";
  drawInfoRow(cardX + UW(14), cardY + UH(124), cardW - UW(28), "RSSI B/W", rssiSummary, theme.panel, theme.panel2, theme.warn, theme.text);

  int miniY = cardY + UH(150);
  int miniW = UW(66);
  const char* labels[4] = {"DMR", "DSTAR", "YSF", "OTHERS"};
  uint32_t values[4] = {sessionInfo.modeDMR, sessionInfo.modeDSTAR, sessionInfo.modeYSF,
                        sessionInfo.modeP25 + sessionInfo.modeNXDN + sessionInfo.modePOCSAG};

  for (int i = 0; i < 4; ++i) {
    int x = cardX + UW(14) + i * UW(70);
    fillSoftPanel(x, miniY, miniW, UH(28), theme.panel, theme.border, 8);
    tft.setTextFont(1);
    tft.setTextColor(theme.warn, theme.panel);
    tft.drawString(labels[i], x + UW(6), miniY + UH(5));
    tft.setTextFont(2);
    tft.setTextColor(theme.text, theme.panel);
    tft.drawString(String(values[i]), x + UW(6), miniY + UH(15));
  }

  drawFooter();
}

void drawDetailPopup(const HeardRecord& r) {
  fillPanel(UX(12), UY(32), SCREEN_W - UX(24), SCREEN_H - UY(58), theme.panel2, theme.border);

  tft.setTextDatum(TL_DATUM);
  tft.setTextFont(2);
  tft.setTextColor(theme.accent, theme.panel2);
  tft.drawString("QSO DETAILS", UX(22), UY(42));

  String cc = countryCodeFromCallsign(r.callsign);

  drawFlagOrPlaceholder(SCREEN_W - UW(82), UY(42), UW(52), UH(42), cc);

  String countryText = radioIdDisplayCountryForRecord(r, true);
  if (!countryText.length()) countryText = cc;
  String heardName = radioIdDisplayNameForRecord(r, true);
  drawLabelValue(UX(22), UY(70),  "User:", normalizeHamCallsign(r.callsign), theme.warn, theme.text, theme.panel2);
  drawLabelValue(UX(22), UY(86),  "Name:", heardName.length() ? clipWithTilde(heardName, 18) : "-", theme.warn, theme.text, theme.panel2);
  String detailMode = normalizeModeName(r.mode);
  String detailTs = dmrTimeslotLabel(r);
  if (detailTs.length()) detailMode += String(" ") + detailTs;
  drawLabelValue(UX(22), UY(102), "Mode:", detailMode.length() ? detailMode : r.mode, theme.warn, theme.text, theme.panel2);
  drawLabelValue(UX(22), UY(118), "Dest:", clipWithTilde(resolveTargetDisplay(r), 18), theme.warn, theme.text, theme.panel2);
  drawLabelValue(UX(22), UY(134), "Grid:", r.grid.length() ? r.grid : "-", theme.warn, theme.good, theme.panel2);
  drawLabelValue(UX(22), UY(150), "Cntry:", countryText.length() ? countryText : "-", theme.warn, theme.text, theme.panel2);
  String quality = "RSSI " + compactRssiValue(r.rssi) + "  BER " + prettyMetricValue(r.ber, "%", false);
  drawLabelValue(UX(22), UY(166), "Qual:", clipWithTilde(quality, 18), theme.warn, theme.subtext, theme.panel2);
  drawLabelValue(UX(22), UY(182), cfg.apiUtcToLocalTime ? "Local:" : "UTC:", r.timeUtc.length() ? r.timeUtc : "-", theme.warn, theme.subtext, theme.panel2);
}

void drawModeConfirmPopup() {
  int popupX, popupY, popupW, popupH;
  int noX, noY, noW, noH;
  int yesX, yesY, yesW, yesH;
  getModeConfirmPopupLayout(popupX, popupY, popupW, popupH, noX, noY, noW, noH, yesX, yesY, yesW, yesH);

  fillSoftPanel(popupX, popupY, popupW, popupH, theme.panel2, theme.accent, 14);

  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(theme.accent, theme.panel2);
  tft.setTextFont(2);
  tft.drawString("MODE CONTROL", popupX + popupW / 2, popupY + UH(22));

  tft.setTextColor(theme.text, theme.panel2);
  tft.setTextFont(2);
  String actionText = modeConfirmEnableRequested ? "Enable " : "Disable ";
  tft.drawString(actionText + modeConfirmTargetLabel + " ?", popupX + popupW / 2, popupY + UH(50));

  fillSoftPanel(noX, noY, noW, noH, theme.panel, theme.border, 8);
  fillSoftPanel(yesX, yesY, yesW, yesH, theme.panel, theme.good, 8);

  tft.setTextColor(theme.text, theme.panel);
  tft.drawString("NO", noX + noW / 2, noY + noH / 2);
  tft.setTextColor(theme.good, theme.panel);
  tft.drawString("YES", yesX + yesW / 2, yesY + yesH / 2);

  tft.setTextDatum(TL_DATUM);
}

void drawQuickActionsPopup() {
  const bool svx = activeQuickActionIsSvxlink;
  const int popupW = UW(250);
  const int popupH = UH(128);
  const int popupX = (SCREEN_W - popupW) / 2;
  const int popupY = (SCREEN_H - popupH) / 2;
  const int btnW = UW(106);
  const int btnH = UH(24);
  const int gapX = UW(10);
  const int gapY = UH(10);
  const int startX = popupX + UW(14);
  const int startY = popupY + UH(34);
  const int closeX = popupX + UW(78);
  const int closeY = popupY + popupH - UH(30);
  const int closeW = UW(94);
  const int closeH = UH(20);

  fillSoftPanel(popupX, popupY, popupW, popupH, theme.panel2, theme.accent, 14);
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(theme.accent, theme.panel2);
  tft.setTextFont(2);
  tft.drawString("QUICK ACTIONS", popupX + popupW / 2, popupY + UH(16));
  tft.setTextFont(1);
  tft.setTextColor(theme.subtext, theme.panel2);
  tft.drawString(svx ? "Send DTMF / Preset" : "Run a saved remote preset", popupX + popupW / 2, popupY + UH(28));

  uint8_t limit = quickActionSlotLimit(svx);
  for (uint8_t i = 0; i < limit; i++) {
    int col = i % 2;
    int row = i / 2;
    int bx = startX + col * (btnW + gapX);
    int by = startY + row * (btnH + gapY);
    bool ready = isQuickActionSlotConfigured(i, svx);
    fillSoftPanel(bx, by, btnW, btnH, theme.panel, ready ? theme.good : theme.border, 8);
    tft.setTextColor(ready ? theme.good : theme.subtext, theme.panel);
    tft.setTextFont(2);
    String label = ready ? clipWithTilde(quickActionSlotLabel(i, svx), 13) : String("Empty");
    tft.drawString(label, bx + btnW / 2, by + btnH / 2);
  }

  tft.setTextDatum(TL_DATUM);
}

void drawWatchSourcePopup() {
  const bool svxEnabled = svxlinkFeatureEnabled();
  const bool aprsEnabled = aprsFeatureEnabled();
  const bool weatherEnabled = weatherFeatureEnabled();
  const bool hamqslEnabled = hamqslFeatureEnabled();
  const int optionCount = 1 + (svxEnabled ? 1 : 0) + (aprsEnabled ? 1 : 0) + (weatherEnabled ? 1 : 0) + (hamqslEnabled ? 1 : 0);
  const int cols = (optionCount >= 4) ? 2 : optionCount;
  const int rows = (optionCount + cols - 1) / cols;
  const int popupW = (optionCount >= 4) ? UW(308) : ((optionCount >= 3) ? UW(304) : UW(248));
  const int popupH = (optionCount >= 4) ? (rows >= 3 ? UH(180) : UH(148)) : UH(112);
  const int popupX = (SCREEN_W - popupW) / 2;
  const int popupY = (SCREEN_H - popupH) / 2;
  const int btnW = (optionCount >= 4) ? UW(118) : ((optionCount >= 3) ? UW(82) : UW(92));
  const int btnH = UH(30);
  const int gapX = UW(10);
  const int gapY = UH(10);
  const int totalBtnW = cols * btnW + (cols - 1) * gapX;
  const int startX = popupX + (popupW - totalBtnW) / 2;
  const int row1Y = popupY + UH(52);

  int optIndex = 0;
  auto optX = [&](int i) -> int { return startX + (i % cols) * (btnW + gapX); };
  auto optY = [&](int i) -> int { return row1Y + (i / cols) * (btnH + gapY); };

  const int mmdvmI = optIndex++;
  const int svxI = svxEnabled ? optIndex++ : -1;
  const int aprsI = aprsEnabled ? optIndex++ : -1;
  const int weatherI = weatherEnabled ? optIndex++ : -1;
  const int hamqslI = hamqslEnabled ? optIndex++ : -1;

  fillSoftPanel(popupX, popupY, popupW, popupH, theme.panel2, theme.accent, 14);
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(theme.accent, theme.panel2);
  tft.setTextFont(2);
  tft.drawString("WATCH SOURCE", popupX + popupW / 2, popupY + UH(20));
  tft.setTextFont(1);
  tft.setTextColor(theme.subtext, theme.panel2);
  tft.drawString(watchSourcePopupTargetPage == PAGE_LAST ? "Open heard list for" : "Open live page for", popupX + popupW / 2, popupY + UH(36));

  auto drawOption = [&](int i, const String& label, bool active) {
    int bx = optX(i);
    int by = optY(i);
    fillSoftPanel(bx, by, btnW, btnH, theme.panel, active ? theme.good : theme.border, 8);
    tft.setTextFont(2);
    tft.setTextColor(active ? theme.good : theme.text, theme.panel);
    tft.drawString(label, bx + btnW / 2, by + btnH / 2);
  };

  drawOption(mmdvmI, "MMDVM", currentWatchSource == WATCH_MMDVM);
  if (svxEnabled) drawOption(svxI, "SVXLINK", currentWatchSource == WATCH_SVXLINK);
  if (aprsEnabled) drawOption(aprsI, "APRS", currentWatchSource == WATCH_APRS);
  if (weatherEnabled) drawOption(weatherI, "WEATHER", currentWatchSource == WATCH_WEATHER);
  if (hamqslEnabled) drawOption(hamqslI, "HAMQSL", currentWatchSource == WATCH_HAMQSL);

  tft.setTextDatum(TL_DATUM);
}


String getScreensaverTimeText() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo, 10)) {
    char buf[8];
    strftime(buf, sizeof(buf), "%H:%M", &timeinfo);
    return String(buf);
  }
  return "--:--";
}

String getHeaderTimeText() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo, 10)) {
    char buf[8];
    strftime(buf, sizeof(buf), "%H:%M", &timeinfo);
    return String(buf);
  }
  return "--:--";
}

String getDetailedClockText() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo, 10)) {
    char buf[10];
    strftime(buf, sizeof(buf), "%H:%M", &timeinfo);
    return String(buf);
  }
  return "--:--";
}

String getDateText() {
  struct tm timeinfo;
  if (getLocalTime(&timeinfo, 10)) {
    char buf[20];
    strftime(buf, sizeof(buf), "%d %b %Y", &timeinfo);
    return String(buf);
  }
  return "-- --- ----";
}

static int64_t daysFromCivil(int year, unsigned month, unsigned day) {
  year -= (month <= 2) ? 1 : 0;
  const int era = (year >= 0 ? year : year - 399) / 400;
  const unsigned yoe = (unsigned)(year - era * 400);
  const unsigned doy = (153 * (month + (month > 2 ? -3 : 9)) + 2) / 5 + day - 1;
  const unsigned doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
  return (int64_t)era * 146097 + (int64_t)doe - 719468;
}

time_t utcTmToEpoch(const struct tm& tmValue) {
  const int year = tmValue.tm_year + 1900;
  const unsigned month = (unsigned)tmValue.tm_mon + 1;
  const unsigned day = (unsigned)tmValue.tm_mday;

  int64_t days = daysFromCivil(year, month, day);
  int64_t seconds = days * 86400LL +
                    (int64_t)tmValue.tm_hour * 3600LL +
                    (int64_t)tmValue.tm_min * 60LL +
                    (int64_t)tmValue.tm_sec;

  return (time_t)seconds;
}

bool parseApiDateTime(const String& input, struct tm& tmOut) {
  String s = trimSafe(input);
  if (s.length() < 19) return false;

  s.replace('T', ' ');

  int year = 0;
  int month = 0;
  int day = 0;
  int hour = 0;
  int minute = 0;
  int second = 0;

  if (sscanf(s.c_str(), "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second) != 6) {
    return false;
  }

  if (year < 1970 || month < 1 || month > 12 || day < 1 || day > 31 ||
      hour < 0 || hour > 23 || minute < 0 || minute > 59 || second < 0 || second > 60) {
    return false;
  }

  memset(&tmOut, 0, sizeof(tmOut));
  tmOut.tm_year = year - 1900;
  tmOut.tm_mon = month - 1;
  tmOut.tm_mday = day;
  tmOut.tm_hour = hour;
  tmOut.tm_min = minute;
  tmOut.tm_sec = second;
  tmOut.tm_isdst = -1;
  return true;
}

String formatApiDateTime(const struct tm& tmValue) {
  char buf[24];
  strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", &tmValue);
  return String(buf);
}

String normalizeHeardTimeDisplay(const String& apiTimeUtc) {
  String raw = trimSafe(apiTimeUtc);
  if (!raw.length()) return "";
  if (!cfg.apiUtcToLocalTime) return raw;

  struct tm parsedUtc;
  if (!parseApiDateTime(raw, parsedUtc)) return raw;

  time_t epochUtc = utcTmToEpoch(parsedUtc);
  if (epochUtc == (time_t)-1) return raw;

  struct tm localTm;
  localtime_r(&epochUtc, &localTm);
  return formatApiDateTime(localTm);
}

void drawScreensaver(bool forceClear) {
  static String lastFrameKey = "";

  if (forceClear) {
    lastFrameKey = "";
  }

  HeardRecord rec;
  bool active = isLiveActive();
  bool hasLastHeard = liveRecord.valid;
  if (hasLastHeard) rec = liveRecord;

  String clockText = getDetailedClockText();
  String dateText = getDateText();
  String callsignText = getDeviceCallsign();
  String shownCallsign = callsignText.length() ? normalizeHamCallsign(callsignText) : "-";
  String operatorText = rec.callsign;
  String shownOperator = operatorText.length() ? normalizeHamCallsign(operatorText) : "-";
  String screensaverName = hasLastHeard ? radioIdDisplayNameForRecord(rec, true) : "";
  String screensaverUser = hasLastHeard ? formatCallsignName(rec.callsign, screensaverName, 10, 10) : shownCallsign;
  String flagCallsign = hasLastHeard && operatorText.length() ? operatorText : callsignText;
  String targetText = hasLastHeard ? resolveTargetDisplay(rec) : "Standby";
  if (targetText.length() > 24) targetText = targetText.substring(0, 23) + "~";

  uint32_t phase = millis() / 12000UL;
  int driftX = ((int)(phase % 5) - 2) * UW(4);
  int driftY = ((int)((phase / 2) % 5) - 2) * UH(3);
  int cx = SCREEN_W / 2 + driftX;
  int weatherY = UY(40) + driftY;
  int clockY = UY(86) + driftY;
  int dateY = UY(122) + driftY;
#if DISPLAY_BACKEND_IS_GFX_COMPAT
  int CallsignY = UY(150) + driftY;
#else
  int CallsignY = UY(158) + driftY;
#endif

  int badgeY = UY(178) + driftY;

  String weatherText = weatherDisplayText();
  String weatherIcon = weatherIconPath();
  String frameKey = clockText + "|" + dateText + "|" + shownCallsign + "|" + screensaverUser + "|" + targetText + "|" + weatherText + "|" + weatherIcon + "|" + String(driftX) + "|" + String(driftY);
  if (frameKey == lastFrameKey && !forceClear) return;

  tft.fillScreen(TFT_BLACK);

#if DISPLAY_BACKEND_IS_GFX_COMPAT
  tft.drawFastHLine(UX(18), UY(10), SCREEN_W - UW(36), theme.panel2);
  tft.drawFastHLine(UX(30), UY(11), SCREEN_W - UW(60), theme.border);
  tft.drawFastHLine(UX(18), SCREEN_H - UY(10), SCREEN_W - UW(36), theme.panel2);
  tft.drawFastHLine(UX(30), SCREEN_H - UY(11), SCREEN_W - UW(60), theme.border);
#else
  tft.drawFastHLine(UX(18), UY(20), SCREEN_W - UW(36), theme.panel2);
  tft.drawFastHLine(UX(30), UY(21), SCREEN_W - UW(60), theme.border);
  tft.drawFastHLine(UX(18), SCREEN_H - UY(20), SCREEN_W - UW(36), theme.panel2);
  tft.drawFastHLine(UX(30), SCREEN_H - UY(21), SCREEN_W - UW(60), theme.border);
#endif

  if (weatherConfigured() && weatherState.valid) {
    int weatherCardW = weatherText.length() ? UW(108) : UW(56);
    int weatherCardH = UH(30);
    int weatherCardX = cx - (weatherCardW / 2);
    fillSoftPanel(weatherCardX, weatherY - (weatherCardH / 2), weatherCardW, weatherCardH, theme.panel, theme.border, 12);

    int cursorX = weatherCardX + UW(10);
    if (weatherIcon.length()) {
#if DISPLAY_BACKEND_IS_GFX_COMPAT
      drawPngFromSD(weatherIcon.c_str(), cursorX, weatherY - UH(12) + 4, 24, 24, theme.panel);
#else
      drawPngFromSD(weatherIcon.c_str(), cursorX, weatherY - UH(12), 24, 24, theme.panel);
#endif
      cursorX += UW(32);
    }

    if (weatherText.length()) {
      tft.setTextDatum(ML_DATUM);
      tft.setTextFont(2);
      tft.setTextColor(theme.text, theme.panel);
      tft.drawString(weatherText, cursorX, weatherY);
    }
  }

  tft.setTextDatum(MC_DATUM);
  tft.setTextFont(7);
  tft.setTextColor(theme.accent, TFT_BLACK);
  tft.drawString(clockText, cx, clockY);

  tft.setTextFont(2);
  tft.setTextColor(theme.subtext, TFT_BLACK);
  tft.drawString(dateText, cx, dateY);

  tft.setTextFont(4);
  tft.setTextColor(theme.warn, theme.panel);
  tft.drawString(shownCallsign, cx, CallsignY);

  int badgeW = UW(248);
  int badgeH = UH(44);
  int badgeX = (SCREEN_W - badgeW) / 2 + driftX;
  badgeX = constrain(badgeX, UX(8), SCREEN_W - badgeW - UX(8));
  fillSoftPanel(badgeX, badgeY, badgeW, badgeH, theme.panel, theme.border, 14);

  const int badgeFlagW = 32;
  const int badgeFlagH = 32;
  int badgeFlagX = badgeX + UW(10);
#if DISPLAY_BACKEND_IS_GFX_COMPAT
  int badgeFlagY = badgeY + UH(6) - 3;
#else
  int badgeFlagY = badgeY + UH(6);
#endif
  badgeFlagY = constrain(badgeFlagY, badgeY + UH(4), badgeY + badgeH - badgeFlagH - UH(4));
  drawFlagForCallsign(badgeFlagX, badgeFlagY, badgeFlagW, badgeFlagH, flagCallsign);

  int badgeTextX = badgeFlagX + badgeFlagW + UW(10);
  int badgeTextMax = max(8, (badgeW - (badgeTextX - badgeX) - UW(10)) / 6);
  String badgeUser = clipWithTilde(screensaverUser, badgeTextMax);
  String badgeTarget = clipWithTilde(targetText, badgeTextMax + 2);

  tft.setTextDatum(ML_DATUM);
  tft.setTextFont(2);
  tft.setTextColor(theme.warn, theme.panel);

#if DISPLAY_BACKEND_IS_GFX_COMPAT
  tft.drawString(badgeUser, badgeTextX, badgeY + UH(13) + 5);
#else
  tft.drawString(badgeUser, badgeTextX, badgeY + UH(13) + 3);
#endif

  tft.setTextFont(2);
  tft.setTextColor(theme.text, theme.panel);
#if DISPLAY_BACKEND_IS_GFX_COMPAT
  tft.drawString(badgeTarget, badgeTextX, badgeY + UH(27) + 5);
#else
  tft.drawString(badgeTarget, badgeTextX, badgeY + UH(27) + 3);
#endif

  lastFrameKey = frameKey;
}

void drawAboutPage() {
  drawTopTabs();

  tft.setTextDatum(TL_DATUM);

  const int cardX = UX(10);
  const int cardY = UY(10);
  const int cardW = SCREEN_W - UW(20);

  fillSoftPanel(UX(12), UY(44), SCREEN_W - UW(24), UH(164), theme.panel, theme.border, 12);

  tft.setTextColor(theme.accent, theme.panel);
  tft.setTextFont(4);
  tft.drawString("MMDVM Display", UX(24), UY(56));

  drawInfoRow(cardX + UW(16), cardY + UH(88), cardW - UW(32), "CREATOR", "13MAD86 / Martin", theme.panel, theme.panel2, theme.warn, theme.good);
  drawInfoRow(cardX + UW(16), cardY + UH(112), cardW - UW(32), "VERSION", DISPLAY_VERSION, theme.panel, theme.panel2, theme.warn, theme.text);

  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(theme.warn, theme.panel);
  tft.setTextFont(2);
  tft.drawString("Free for Public, use at own Risk!", SCREEN_W / 2, UY(176));

  drawFooter();
}

void redrawCurrentPage(bool fullRedraw = false) {
  Page pageToDraw = (setupRequired && currentPage != PAGE_INFO && currentPage != PAGE_STATUS && currentPage != PAGE_ABOUT) ? PAGE_AP : currentPage;
  bool pageChanged = (lastRenderedPage != pageToDraw);
  bool overlayPopupOpen = detailPopupOpen || modeConfirmPopupOpen || quickActionPopupOpen || watchSourcePopupOpen || aprsCommentPopupOpen;
  bool popupChanged = (lastDetailPopupState != overlayPopupOpen);

  if (screensaverActive) {
    drawScreensaver(fullRedraw || pageChanged || popupChanged);
    lastRenderedPage = pageToDraw;
    lastDetailPopupState = overlayPopupOpen;
#if DISPLAY_BACKEND_IS_GFX_COMPAT
    tft.flush();
#endif

    return;
  }

  if (fullRedraw || pageChanged || popupChanged) {
    clearBody();
  }

  scrollingTextActiveFrame = false;

  if (pageToDraw == PAGE_LIVE) drawLivePage();
  else if (pageToDraw == PAGE_LAST) drawLastPage();
  else if (pageToDraw == PAGE_STATUS) drawSetupPage();
  else if (pageToDraw == PAGE_AP) drawApModePage();
  else if (pageToDraw == PAGE_INFO) drawInfoPage();
  else if (pageToDraw == PAGE_ABOUT) drawAboutPage();

  if (detailPopupOpen && selectedHeardIndex >= 0 && selectedHeardIndex < heardCount) {
    drawDetailPopup(heardList[selectedHeardIndex]);
  }
  if (modeConfirmPopupOpen) {
    drawModeConfirmPopup();
  }
  if (quickActionPopupOpen) {
    drawQuickActionsPopup();
  }
  if (watchSourcePopupOpen) {
    drawWatchSourcePopup();
  }
  if (aprsCommentPopupOpen) {
    drawAprsCommentPopup();
  }

  lastRenderedPage = pageToDraw;
  lastDetailPopupState = overlayPopupOpen;
#if DISPLAY_BACKEND_IS_GFX_COMPAT
  tft.flush();
#endif
}


bool wifiSsidLooksUsable(String ssid) {
  String check = trimSafe(ssid);
  if (!check.length()) return false;
  String lower = check;
  lower.toLowerCase();
  return lower != "your_wifi" && lower != "wifi_ssid" && lower != "ssid";
}

void clearWifiProfiles() {
  for (uint8_t i = 0; i < MAX_WIFI_PROFILES; i++) {
    cfg.wifiProfiles[i].ssid = "";
    cfg.wifiProfiles[i].pass = "";
  }
}

bool addWifiProfile(String ssid, String pass) {
  if (!wifiSsidLooksUsable(ssid)) return false;

  for (uint8_t i = 0; i < MAX_WIFI_PROFILES; i++) {
    if (cfg.wifiProfiles[i].ssid == ssid) {
      cfg.wifiProfiles[i].pass = pass;
      return true;
    }
  }

  for (uint8_t i = 0; i < MAX_WIFI_PROFILES; i++) {
    if (!cfg.wifiProfiles[i].ssid.length()) {
      cfg.wifiProfiles[i].ssid = ssid;
      cfg.wifiProfiles[i].pass = pass;
      return true;
    }
  }

  return false;
}

uint8_t configuredWifiProfileCount() {
  uint8_t count = 0;
  for (uint8_t i = 0; i < MAX_WIFI_PROFILES; i++) {
    if (wifiSsidLooksUsable(cfg.wifiProfiles[i].ssid)) count++;
  }
  return count;
}

bool hasConfiguredWifiProfiles() {
  return configuredWifiProfileCount() > 0;
}

uint8_t effectiveWifiRetriesPerProfile() {
  int retries = cfg.wifiRetriesPerProfile;
  retries = constrain(retries, WIFI_PROFILE_RETRY_COUNT_MIN, WIFI_PROFILE_RETRY_COUNT_MAX);
  return (uint8_t)retries;
}

uint8_t wifiAttemptsPerProfile() {
  return (uint8_t)(effectiveWifiRetriesPerProfile() + 1);
}

String wifiAttemptSuffix(uint8_t attemptNumber) {
  uint8_t total = wifiAttemptsPerProfile();
  if (total <= 1) return "";
  attemptNumber = constrain(attemptNumber, (uint8_t)1, total);
  return " (" + String(attemptNumber) + "/" + String(total) + ")";
}

void resetWifiConnectRound() {
  for (uint8_t i = 0; i < MAX_WIFI_PROFILES; i++) {
    wifiProfileTriedThisRound[i] = false;
  }
}

void syncLegacyWifiFieldsFromProfiles() {
  cfg.wifiSsid = "";
  cfg.wifiPass = "";
  for (uint8_t i = 0; i < MAX_WIFI_PROFILES; i++) {
    if (wifiSsidLooksUsable(cfg.wifiProfiles[i].ssid)) {
      cfg.wifiSsid = cfg.wifiProfiles[i].ssid;
      cfg.wifiPass = cfg.wifiProfiles[i].pass;
      return;
    }
  }
}

void loadWifiProfilesFromJsonArray(JsonArray arr) {
  if (arr.isNull()) return;

  for (JsonVariant v : arr) {
    if (!v.is<JsonObject>()) continue;
    JsonObject item = v.as<JsonObject>();
    String ssid = String(item["ssid"] | "");
    String pass = String(item["password"] | "");
    if (!pass.length() && item.containsKey("pass")) pass = String(item["pass"] | "");
    addWifiProfile(ssid, pass);
  }
}

bool jsonArrayHasUsableWifiProfile(JsonArray arr) {
  if (arr.isNull()) return false;

  for (JsonVariant v : arr) {
    if (!v.is<JsonObject>()) continue;
    JsonObject item = v.as<JsonObject>();
    if (wifiSsidLooksUsable(String(item["ssid"] | ""))) return true;
  }

  return false;
}

bool configDocumentHasUsableWifiProfile(DynamicJsonDocument& doc) {
  if (wifiSsidLooksUsable(String(doc["wifi_ssid"] | ""))) return true;
  if (doc.containsKey("wifi_profiles") && jsonArrayHasUsableWifiProfile(doc["wifi_profiles"].as<JsonArray>())) return true;
  if (doc.containsKey("wifi") && jsonArrayHasUsableWifiProfile(doc["wifi"].as<JsonArray>())) return true;
  return false;
}

void writeWifiProfilesToJson(DynamicJsonDocument& doc) {
  doc["wifi_retries_per_profile"] = effectiveWifiRetriesPerProfile();
  JsonArray profiles = doc.createNestedArray("wifi_profiles");
  for (uint8_t i = 0; i < MAX_WIFI_PROFILES; i++) {
    if (!wifiSsidLooksUsable(cfg.wifiProfiles[i].ssid)) continue;
    JsonObject item = profiles.createNestedObject();
    item["ssid"] = cfg.wifiProfiles[i].ssid;
    item["password"] = cfg.wifiProfiles[i].pass;
  }
}

int8_t nextConfiguredWifiProfileAfter(int8_t currentIndex) {
  if (!hasConfiguredWifiProfiles()) return -1;

  uint8_t start = 0;
  if (currentIndex >= 0) start = ((uint8_t)currentIndex + 1) % MAX_WIFI_PROFILES;

  for (uint8_t offset = 0; offset < MAX_WIFI_PROFILES; offset++) {
    uint8_t idx = (start + offset) % MAX_WIFI_PROFILES;
    if (wifiSsidLooksUsable(cfg.wifiProfiles[idx].ssid)) return idx;
  }

  return -1;
}

bool wifiSsidVisibleInScan(const String& ssid, int networkCount) {
  if (networkCount <= 0) return false;
  for (int n = 0; n < networkCount; n++) {
    if (WiFi.SSID(n) == ssid) return true;
  }
  return false;
}

int scanWifiNetworksForProfiles() {
  WiFi.mode(apModeActive ? WIFI_AP_STA : WIFI_STA);
  WiFi.persistent(false);
  int networkCount = WiFi.scanNetworks(false, true);
  if (networkCount < 0) {
    addWebLog("WARN", "WIFI", "Wi-Fi scan failed. Will try saved profiles without scan filtering.");
  } else {
    addWebLog("INFO", "WIFI", "Wi-Fi scan found " + String(networkCount) + " network(s).");
  }
  return networkCount;
}

int8_t findNextVisibleWifiProfileIndex(uint8_t startIndex, int8_t excludeIndex = -1, bool skipTriedThisRound = false) {
  if (!hasConfiguredWifiProfiles()) return -1;

  int networkCount = scanWifiNetworksForProfiles();
  if (networkCount == 0) return -1;

  for (uint8_t offset = 0; offset < MAX_WIFI_PROFILES; offset++) {
    uint8_t idx = (startIndex + offset) % MAX_WIFI_PROFILES;
    if ((int8_t)idx == excludeIndex) continue;
    if (skipTriedThisRound && wifiProfileTriedThisRound[idx]) continue;
    if (!wifiSsidLooksUsable(cfg.wifiProfiles[idx].ssid)) continue;
    if (networkCount < 0 || wifiSsidVisibleInScan(cfg.wifiProfiles[idx].ssid, networkCount)) return idx;
  }

  return -1;
}

bool beginWifiProfileConnection(uint8_t profileIndex, const String& logPrefix, uint8_t attemptNumber = 1) {
  if (profileIndex >= MAX_WIFI_PROFILES || !wifiSsidLooksUsable(cfg.wifiProfiles[profileIndex].ssid)) return false;

  WiFi.mode(apModeActive ? WIFI_AP_STA : WIFI_STA);
  WiFi.persistent(false);
  WiFi.setAutoReconnect(true);
  delay(100);
  WiFi.begin(cfg.wifiProfiles[profileIndex].ssid.c_str(), cfg.wifiProfiles[profileIndex].pass.c_str());

  wifiConnectProfileIndex = profileIndex;
  wifiConnectAttemptNumber = constrain(attemptNumber, (uint8_t)1, wifiAttemptsPerProfile());
  wifiNextProfileIndex = (uint8_t)((profileIndex + 1) % MAX_WIFI_PROFILES);
  String msg = logPrefix + " SSID \"" + cfg.wifiProfiles[profileIndex].ssid + "\"";
  if (wifiAttemptsPerProfile() > 1) {
    msg += " (attempt " + String(wifiConnectAttemptNumber) + "/" + String(wifiAttemptsPerProfile()) + ")";
  }
  msg += ".";
  addWebLog("INFO", "WIFI", msg);
  return true;
}

bool persistConfigDocument(DynamicJsonDocument& doc, const char* path) {
  storageRemove(path);
  File f = storageOpen(path, FILE_WRITE);
  if (!f) return false;
  bool ok = serializeJsonPretty(doc, f) > 0;
  f.close();
  return ok;
}

void applyConfigAfterLoad() {
  configOkState = loadConfig();
  timeConfigured = false;
  applyDisplaySettings();
  if (!weatherConfigured()) {
    clearWeatherData();
  } else {
    fetchWeatherData();
  }
  if (!hamqslConfigured()) {
    clearHamqslData();
  } else {
    fetchHamqslData();
  }
  if (!aprsFeatureEnabled()) {
    resetAprsState();
  }
  if (cfg.radioIdLookupEnabled && heardCount > 0) {
    applyRadioIdCacheToActiveData();
    if (heardCount > 0) liveRecord = heardList[0];
    requestRedraw();
  }
  noteUserInteraction(false);
  lastLiveActiveState = isLiveActive();
  pendingConfigApply = true;
  pendingConfigApplyAt = millis() + 350;
  addWebLog("INFO", "CONFIG", "Runtime configuration applied.");
}

bool applyConfigFromFile(const char* path, String* errMsg) {
  File in = storageOpen(path, FILE_READ);
  if (!in) {
    if (errMsg) *errMsg = "Uploaded config could not be opened.";
    return false;
  }

  DynamicJsonDocument doc(15360);
  DeserializationError err = deserializeJson(doc, in);
  in.close();

  if (err) {
    if (errMsg) *errMsg = "JSON parse failed: " + String(err.c_str());
    return false;
  }

  if (!doc.is<JsonObject>()) {
    if (errMsg) *errMsg = "Config file must contain a JSON object.";
    return false;
  }

  String rawBackend = String(doc["backend"] | "");
  String backend = canonicalBackendValue(rawBackend);
  String rawBackendLower = trimSafe(rawBackend);
  rawBackendLower.toLowerCase();
  if (!backend.length() && rawBackendLower.indexOf("svxlink") >= 0) {
    backend = "Pi-Star";
    doc["svxlink_enabled"] = true;
    doc["svxlink_source"] = (rawBackendLower.indexOf("api") >= 0) ? "api" : "url";
  }
  String mmdvmHost = trimSafe(String(doc["pistar_host"] | ""));
  bool backendOk = backend.length() > 0;
  bool wifiOk = configDocumentHasUsableWifiProfile(doc);
  bool hostOk = mmdvmHost.length() > 0;
  if (backendOk) doc["backend"] = backend;
  if (!backendOk || !wifiOk || !hostOk) {
    if (errMsg) *errMsg = "Config is missing required fields (backend, wifi_ssid or wifi_profiles, and pistar_host).";

    return false;
  }

  if (!persistConfigDocument(doc, CONFIG_FILE)) {
    if (errMsg) *errMsg = "Imported config could not be saved.";
    return false;
  }

  applyConfigAfterLoad();
  return true;
}

bool saveConfigToFile() {
  DynamicJsonDocument doc(15360);

  syncLegacyWifiFieldsFromProfiles();
  doc["backend"] = cfg.backend;
  doc["wifi_ssid"] = cfg.wifiSsid;
  doc["wifi_pass"] = cfg.wifiPass;
  writeWifiProfilesToJson(doc);
  doc["pistar_host"] = cfg.pistarHost;
  doc["svxlink_enabled"] = cfg.svxlinkEnabled;
  doc["svxlink_source"] = cfg.svxlinkSource;
  doc["svxlink_host"] = cfg.svxlinkHost;
  doc["svxlink_api_path"] = cfg.svxlinkApiPath;
  doc["aprs_enabled"] = cfg.aprsEnabled;
  doc["aprs_api_key"] = cfg.aprsApiKey;
  doc["aprs_refresh_minutes"] = cfg.aprsRefreshMinutes;
  doc["start_page_after_boot"] = normalizedStartPageAfterBoot(false);
  doc["device_callsign"] = cfg.deviceCallsign;
  doc["theme_color"] = cfg.themeColor;
  doc["refresh_ms"] = cfg.refreshMs;
  doc["callsign_hold_ms"] = cfg.callsignHoldMs;
  doc["scrolling_text_enabled"] = cfg.scrollingTextEnabled;
  doc["api_utc_to_local_time"] = cfg.apiUtcToLocalTime;
  doc["timezone_tz"] = cfg.timezoneTz;
  doc["brightness"] = cfg.brightness;
  doc["flip_screen"] = cfg.flipScreen;
  doc["screensaver_enabled"] = cfg.screensaverEnabled;
  doc["screensaver_timeout_ms"] = cfg.screensaverTimeoutMs;
  doc["screensaver_dim_brightness"] = cfg.screensaverDimBrightness;
  doc["weather_enabled"] = cfg.weatherEnabled;
  doc["weather_refresh_minutes"] = cfg.weatherRefreshMinutes;
  doc["weather_lat"] = cfg.weatherLat;
  doc["weather_lon"] = cfg.weatherLon;
  doc["weather_api_key"] = cfg.weatherApiKey;
  doc["hamqsl_enabled"] = cfg.hamqslEnabled;
  doc["hamqsl_refresh_minutes"] = cfg.hamqslRefreshMinutes;
  doc["hamqsl_url"] = cfg.hamqslUrl;
  doc["radioid_lookup_enabled"] = cfg.radioIdLookupEnabled;
  doc["radioid_users_api_base"] = cfg.radioIdUsersApiBase;
  doc["target_list_lookup_enabled"] = cfg.targetListLookupEnabled;
  doc["target_list_base_url"] = cfg.targetListBaseUrl;
  doc["target_list_dmr_url"] = cfg.targetListDmrUrl;
  doc["target_list_dstar_url"] = cfg.targetListDstarUrl;
  doc["target_list_ysf_url"] = cfg.targetListYsfUrl;
  doc["target_list_p25_url"] = cfg.targetListP25Url;
  doc["target_list_nxdn_url"] = cfg.targetListNxdnUrl;
  doc["touch_mode_ssh_enabled"] = cfg.touchModeSshEnabled;
  doc["pistar_ssh_user"] = cfg.pistarSshUser;
  doc["pistar_ssh_pass"] = cfg.pistarSshPass;
  doc["pistar_ssh_port"] = cfg.pistarSshPort;
  doc["svxlink_ssh_enabled"] = cfg.svxlinkSshEnabled;
  doc["svxlink_ssh_user"] = cfg.svxlinkSshUser;
  doc["svxlink_ssh_pass"] = cfg.svxlinkSshPass;
  doc["svxlink_ssh_port"] = cfg.svxlinkSshPort;
  JsonArray qa = doc.createNestedArray("quick_actions");
  for (uint8_t i = 0; i < QUICK_ACTION_SLOT_COUNT; i++) {
    JsonObject item = qa.createNestedObject();
    item["enabled"] = cfg.quickActions[i].enabled;
    item["use_sudo"] = cfg.quickActions[i].useSudo;
    item["label"] = cfg.quickActions[i].label;
    item["command"] = cfg.quickActions[i].command;
  }
  JsonArray sva = doc.createNestedArray("svxlink_actions");
  for (uint8_t i = 0; i < SVXLINK_ACTION_SLOT_COUNT; i++) {
    JsonObject item = sva.createNestedObject();
    item["enabled"] = cfg.svxlinkActions[i].enabled;
    item["use_sudo"] = cfg.svxlinkActions[i].useSudo;
    item["label"] = cfg.svxlinkActions[i].label;
    item["command"] = cfg.svxlinkActions[i].command;
  }

  JsonObject th = doc.createNestedObject("theme");

  th["bg"] = color565ToHex(theme.bg);
  th["panel"] = color565ToHex(theme.panel);
  th["panel2"] = color565ToHex(theme.panel2);
  th["border"] = color565ToHex(theme.border);
  th["text"] = color565ToHex(theme.text);
  th["subtext"] = color565ToHex(theme.subtext);
  th["accent"] = color565ToHex(theme.accent);
  th["good"] = color565ToHex(theme.good);
  th["warn"] = color565ToHex(theme.warn);
  th["bad"] = color565ToHex(theme.bad);
  th["live"] = color565ToHex(theme.live);
  th["rx"] = color565ToHex(theme.rx);
  th["tx"] = color565ToHex(theme.tx);
  th["tabIdle"] = color565ToHex(theme.tabIdle);
  th["tabActive"] = color565ToHex(theme.tabActive);

  return persistConfigDocument(doc, CONFIG_FILE);
}

bool saveDefaultConfig() {
  DynamicJsonDocument doc(15360);

  doc["backend"] = "Pi-Star";
  doc["wifi_ssid"] = "";
  doc["wifi_pass"] = "";
  doc["wifi_retries_per_profile"] = WIFI_PROFILE_RETRY_COUNT_DEFAULT;
  doc.createNestedArray("wifi_profiles");
  doc["pistar_host"] = "192.168.1.50";
  doc["svxlink_enabled"] = false;
  doc["svxlink_source"] = "url";
  doc["svxlink_host"] = "192.168.1.60";
  doc["svxlink_api_path"] = "/index.php";
  doc["aprs_enabled"] = false;
  doc["aprs_api_key"] = "";
  doc["aprs_refresh_minutes"] = APRS_REFRESH_MINUTES_DEFAULT;
  doc["start_page_after_boot"] = "mmdvm_live";
  doc["device_callsign"] = "13MAD86";
  doc["theme_color"] = "#35C2FF";
  doc["refresh_ms"] = 1500;
  doc["callsign_hold_ms"] = 12000;
  doc["scrolling_text_enabled"] = true;
  doc["api_utc_to_local_time"] = true;
  doc["timezone_tz"] = DEFAULT_TZ_RULE;
  doc["brightness"] = 220;
  doc["flip_screen"] = false;
  doc["screensaver_enabled"] = true;
  doc["screensaver_timeout_ms"] = 60000;
  doc["screensaver_dim_brightness"] = 20;
  doc["weather_enabled"] = false;
  doc["weather_refresh_minutes"] = 10;
  doc["weather_lat"] = "";
  doc["weather_lon"] = "";
  doc["weather_api_key"] = "";
  doc["hamqsl_enabled"] = false;
  doc["hamqsl_refresh_minutes"] = HAMQSL_REFRESH_MINUTES_DEFAULT;
  doc["hamqsl_url"] = HAMQSL_SOLAR_XML_URL_DEFAULT;
  doc["radioid_lookup_enabled"] = true;
  doc["radioid_users_api_base"] = RADIOID_USERS_API_BASE_DEFAULT;
  doc["target_list_lookup_enabled"] = true;
  doc["target_list_base_url"] = TARGET_LIST_BASE_URL_DEFAULT;
  doc["target_list_dmr_url"] = "";
  doc["target_list_dstar_url"] = "";
  doc["target_list_ysf_url"] = "";
  doc["target_list_p25_url"] = "";
  doc["target_list_nxdn_url"] = "";
  doc["touch_mode_ssh_enabled"] = false;
  doc["pistar_ssh_user"] = "pi-star";
  doc["pistar_ssh_pass"] = "";
  doc["pistar_ssh_port"] = 22;
  doc["svxlink_ssh_enabled"] = false;
  doc["svxlink_ssh_user"] = "svxlink";
  doc["svxlink_ssh_pass"] = "";
  doc["svxlink_ssh_port"] = 22;
  JsonArray qa = doc.createNestedArray("quick_actions");
  for (uint8_t i = 0; i < QUICK_ACTION_SLOT_COUNT; i++) {
    JsonObject item = qa.createNestedObject();
    item["enabled"] = false;
    item["use_sudo"] = false;
    item["label"] = i == 0 ? "DMR TG91" : (i == 1 ? "DMR TG263" : (i == 2 ? "REF001C" : "Disconnect"));
    item["command"] = "";
  }
  JsonArray sva = doc.createNestedArray("svxlink_actions");
  for (uint8_t i = 0; i < SVXLINK_ACTION_SLOT_COUNT; i++) {
    JsonObject item = sva.createNestedObject();
    item["enabled"] = false;
    item["use_sudo"] = false;
    item["label"] = i == 0 ? "DTMF 1" : (i == 1 ? "DTMF 2" : (i == 2 ? "DTMF 3" : "Disconnect"));
    item["command"] = "";
  }

  JsonObject th = doc.createNestedObject("theme");
  th["bg"] = "#000000";
  th["panel"] = "#1C1C1C";
  th["panel2"] = "#2A2A2A";
  th["border"] = "#666666";
  th["text"] = "#FFFFFF";
  th["subtext"] = "#C0C0C0";
  th["accent"] = "#00FFFF";
  th["good"] = "#00FF00";
  th["warn"] = "#FFFF00";
  th["bad"] = "#FF0000";
  th["live"] = "#FFA500";
  th["rx"] = "#00FF00";
  th["tx"] = "#0080FF";
  th["tabIdle"] = "#406070";
  th["tabActive"] = "#008080";

  return persistConfigDocument(doc, CONFIG_FILE);
}

bool configLooksValid() {
  bool backendOk = isValidBackendValue(cfg.backend);
  bool hostOk = cfg.pistarHost.length() > 0;
  return hasConfiguredWifiProfiles() && hostOk && backendOk;
}


#if DISPLAY_BACKEND_IS_DESKTOP
static size_t desktopJsonSkipWs(const std::string& s, size_t i) {
  while (i < s.size() && (s[i] == ' ' || s[i] == '\t' || s[i] == '\r' || s[i] == '\n')) ++i;
  return i;
}

static bool desktopJsonReadStringAt(const std::string& s, size_t i, String& out, size_t* endOut = nullptr) {
  if (i >= s.size() || s[i] != '"') return false;
  std::string v;
  ++i;
  while (i < s.size()) {
    char c = s[i++];
    if (c == '"') {
      out = String(v);
      if (endOut) *endOut = i;
      return true;
    }
    if (c == '\\' && i < s.size()) {
      char e = s[i++];
      switch (e) {
        case '"': v.push_back('"'); break;
        case '\\': v.push_back('\\'); break;
        case '/': v.push_back('/'); break;
        case 'b': v.push_back('\b'); break;
        case 'f': v.push_back('\f'); break;
        case 'n': v.push_back('\n'); break;
        case 'r': v.push_back('\r'); break;
        case 't': v.push_back('\t'); break;
        case 'u':
          if (i + 4 <= s.size()) i += 4;
          v.push_back('?');
          break;
        default:
          v.push_back(e);
          break;
      }
    } else {
      v.push_back(c);
    }
  }
  return false;
}

static size_t desktopJsonFindValue(const std::string& json, const char* key, size_t from = 0) {
  std::string pat = std::string("\"") + key + "\"";
  size_t p = json.find(pat, from);
  if (p == std::string::npos) return std::string::npos;
  p = json.find(':', p + pat.size());
  if (p == std::string::npos) return std::string::npos;
  return desktopJsonSkipWs(json, p + 1);
}

static bool desktopJsonGetString(const std::string& json, const char* key, String& out) {
  size_t p = desktopJsonFindValue(json, key);
  if (p == std::string::npos) return false;
  return desktopJsonReadStringAt(json, p, out);
}

static bool desktopJsonGetBool(const std::string& json, const char* key, bool& out) {
  size_t p = desktopJsonFindValue(json, key);
  if (p == std::string::npos) return false;
  if (json.compare(p, 4, "true") == 0) { out = true; return true; }
  if (json.compare(p, 5, "false") == 0) { out = false; return true; }
  return false;
}

static bool desktopJsonGetLong(const std::string& json, const char* key, long& out) {
  size_t p = desktopJsonFindValue(json, key);
  if (p == std::string::npos) return false;
  char* endp = nullptr;
  long v = std::strtol(json.c_str() + p, &endp, 10);
  if (endp == json.c_str() + p) return false;
  out = v;
  return true;
}

static bool desktopJsonGetObject(const std::string& json, const char* key, std::string& out) {
  size_t p = desktopJsonFindValue(json, key);
  if (p == std::string::npos || p >= json.size() || json[p] != '{') return false;
  size_t start = p;
  int depth = 0;
  bool inStr = false;
  bool esc = false;
  for (; p < json.size(); ++p) {
    char c = json[p];
    if (inStr) {
      if (esc) esc = false;
      else if (c == '\\') esc = true;
      else if (c == '"') inStr = false;
      continue;
    }
    if (c == '"') inStr = true;
    else if (c == '{') ++depth;
    else if (c == '}') {
      --depth;
      if (depth == 0) {
        out = json.substr(start, p - start + 1);
        return true;
      }
    }
  }
  return false;
}

static bool desktopJsonGetArray(const std::string& json, const char* key, std::string& out) {
  size_t p = desktopJsonFindValue(json, key);
  if (p == std::string::npos || p >= json.size() || json[p] != '[') return false;
  size_t start = p;
  int depth = 0;
  bool inStr = false;
  bool esc = false;
  for (; p < json.size(); ++p) {
    char c = json[p];
    if (inStr) {
      if (esc) esc = false;
      else if (c == '\\') esc = true;
      else if (c == '"') inStr = false;
      continue;
    }
    if (c == '"') inStr = true;
    else if (c == '[') ++depth;
    else if (c == ']') {
      --depth;
      if (depth == 0) {
        out = json.substr(start, p - start + 1);
        return true;
      }
    }
  }
  return false;
}

static void desktopParseActionArray(const std::string& json, const char* key, QuickActionSlotConfig* slots, uint8_t count) {
  std::string arr;
  if (!desktopJsonGetArray(json, key, arr)) return;
  size_t p = 0;
  uint8_t idx = 0;
  while (idx < count && p < arr.size()) {
    size_t objStart = arr.find('{', p);
    if (objStart == std::string::npos) break;
    int depth = 0;
    bool inStr = false;
    bool esc = false;
    size_t objEnd = std::string::npos;
    for (size_t i = objStart; i < arr.size(); ++i) {
      char c = arr[i];
      if (inStr) {
        if (esc) esc = false;
        else if (c == '\\') esc = true;
        else if (c == '"') inStr = false;
        continue;
      }
      if (c == '"') inStr = true;
      else if (c == '{') ++depth;
      else if (c == '}') {
        --depth;
        if (depth == 0) { objEnd = i; break; }
      }
    }
    if (objEnd == std::string::npos) break;
    std::string obj = arr.substr(objStart, objEnd - objStart + 1);
    bool b = false;
    String v;
    if (desktopJsonGetBool(obj, "enabled", b)) slots[idx].enabled = b;
    if (desktopJsonGetBool(obj, "use_sudo", b)) slots[idx].useSudo = b;
    if (desktopJsonGetString(obj, "label", v)) slots[idx].label = v;
    if (desktopJsonGetString(obj, "command", v)) slots[idx].command = v;
    ++idx;
    p = objEnd + 1;
  }
}

static void desktopLoadThemeOverrides(const std::string& json) {
  std::string themeJson;
  if (!desktopJsonGetObject(json, "theme", themeJson)) return;
  String s;
  uint16_t c;
#define DESKTOP_THEME_COLOR(KEY, FIELD) \
  do { if (desktopJsonGetString(themeJson, KEY, s) && parseHexColor(s, c)) theme.FIELD = c; } while (0)
  DESKTOP_THEME_COLOR("bg", bg);
  DESKTOP_THEME_COLOR("panel", panel);
  DESKTOP_THEME_COLOR("panel2", panel2);
  DESKTOP_THEME_COLOR("border", border);
  DESKTOP_THEME_COLOR("text", text);
  DESKTOP_THEME_COLOR("subtext", subtext);
  DESKTOP_THEME_COLOR("accent", accent);
  DESKTOP_THEME_COLOR("good", good);
  DESKTOP_THEME_COLOR("warn", warn);
  DESKTOP_THEME_COLOR("bad", bad);
  DESKTOP_THEME_COLOR("live", live);
  DESKTOP_THEME_COLOR("rx", rx);
  DESKTOP_THEME_COLOR("tx", tx);
  DESKTOP_THEME_COLOR("tabIdle", tabIdle);
  DESKTOP_THEME_COLOR("tabActive", tabActive);
#undef DESKTOP_THEME_COLOR
}

static void desktopApplyJsonString(const std::string& json, const char* key, String& field) {
  String v;
  if (desktopJsonGetString(json, key, v)) field = v;
}
static void desktopApplyJsonBool(const std::string& json, const char* key, bool& field) {
  bool v;
  if (desktopJsonGetBool(json, key, v)) field = v;
}
static void desktopApplyJsonU8(const std::string& json, const char* key, uint8_t& field) {
  long v;
  if (desktopJsonGetLong(json, key, v)) field = (uint8_t)constrain(v, 0L, 255L);
}
static void desktopApplyJsonU16(const std::string& json, const char* key, uint16_t& field) {
  long v;
  if (desktopJsonGetLong(json, key, v)) field = (uint16_t)constrain(v, 0L, 65535L);
}
static void desktopApplyJsonU32(const std::string& json, const char* key, uint32_t& field) {
  long v;
  if (desktopJsonGetLong(json, key, v) && v >= 0) field = (uint32_t)v;
}

static void desktopFinalizeLoadedConfig() {
  syncLegacyWifiFieldsFromProfiles();
  cfg.wifiRetriesPerProfile = effectiveWifiRetriesPerProfile();

  cfg.pistarHost = trimSafe(cfg.pistarHost);
  cfg.svxlinkSource = canonicalSvxlinkSourceValue(cfg.svxlinkSource);
  if (!cfg.svxlinkSource.length()) cfg.svxlinkSource = "url";
  cfg.svxlinkHost = trimSafe(cfg.svxlinkHost);
  cfg.svxlinkApiPath = normalizedSvxlinkPathForBackend(cfg.svxlinkApiPath);
  cfg.aprsApiKey = trimSafe(cfg.aprsApiKey);
  cfg.aprsRefreshMinutes = constrain(cfg.aprsRefreshMinutes, APRS_REFRESH_MINUTES_MIN, APRS_REFRESH_MINUTES_MAX);
  cfg.startPageAfterBoot = normalizedStartPageAfterBoot(false);
  cfg.deviceCallsign = trimSafe(cfg.deviceCallsign);
  cfg.timezoneTz = trimSafe(cfg.timezoneTz);
  if (!cfg.timezoneTz.length()) cfg.timezoneTz = String(DEFAULT_TZ_RULE);
  cfg.themeColor = trimSafe(cfg.themeColor);
  cfg.weatherLat = trimSafe(cfg.weatherLat);
  cfg.weatherLon = trimSafe(cfg.weatherLon);
  cfg.weatherApiKey = trimSafe(cfg.weatherApiKey);
  cfg.hamqslUrl = trimSafe(cfg.hamqslUrl);
  if (!cfg.hamqslUrl.length()) cfg.hamqslUrl = String(HAMQSL_SOLAR_XML_URL_DEFAULT);
  cfg.radioIdUsersApiBase = trimSafe(cfg.radioIdUsersApiBase);
  cfg.pistarSshUser = trimSafe(cfg.pistarSshUser);
  cfg.svxlinkSshUser = trimSafe(cfg.svxlinkSshUser);
  if (!cfg.radioIdUsersApiBase.length()) cfg.radioIdUsersApiBase = String(RADIOID_USERS_API_BASE_DEFAULT);
  if (!cfg.pistarSshUser.length()) cfg.pistarSshUser = "pi-star";
  if (!cfg.svxlinkSshUser.length()) cfg.svxlinkSshUser = "svxlink";
  if (cfg.pistarSshPort == 0) cfg.pistarSshPort = 22;
  if (cfg.svxlinkSshPort == 0) cfg.svxlinkSshPort = 22;
  for (uint8_t i = 0; i < QUICK_ACTION_SLOT_COUNT; i++) {
    cfg.quickActions[i].label = trimSafe(cfg.quickActions[i].label);
    cfg.quickActions[i].command = trimSafe(cfg.quickActions[i].command);
    if (!cfg.quickActions[i].label.length() || !cfg.quickActions[i].command.length()) cfg.quickActions[i].enabled = false;
  }
  for (uint8_t i = 0; i < SVXLINK_ACTION_SLOT_COUNT; i++) {
    cfg.svxlinkActions[i].label = trimSafe(cfg.svxlinkActions[i].label);
    cfg.svxlinkActions[i].command = trimSafe(cfg.svxlinkActions[i].command);
    if (!cfg.svxlinkActions[i].label.length() || !cfg.svxlinkActions[i].command.length()) cfg.svxlinkActions[i].enabled = false;
  }
  if (!svxlinkFeatureEnabled() && currentWatchSource == WATCH_SVXLINK) currentWatchSource = WATCH_MMDVM;
  if (!aprsFeatureEnabled() && currentWatchSource == WATCH_APRS) currentWatchSource = WATCH_MMDVM;
  if (!weatherFeatureEnabled() && currentWatchSource == WATCH_WEATHER) currentWatchSource = WATCH_MMDVM;
  if (!hamqslFeatureEnabled() && currentWatchSource == WATCH_HAMQSL) currentWatchSource = WATCH_MMDVM;

  cfg.brightness = constrain(cfg.brightness, 0, 255);
  cfg.screensaverDimBrightness = constrain(cfg.screensaverDimBrightness, 0, 255);
  cfg.weatherRefreshMinutes = constrain(cfg.weatherRefreshMinutes, (uint16_t)1, (uint16_t)1440);
  cfg.hamqslRefreshMinutes = constrain(cfg.hamqslRefreshMinutes, HAMQSL_REFRESH_MINUTES_MIN, HAMQSL_REFRESH_MINUTES_MAX);
  if (cfg.screensaverTimeoutMs < 5000) cfg.screensaverTimeoutMs = 5000;
  if (!weatherConfigured()) clearWeatherData();
  if (!hamqslConfigured()) clearHamqslData();
}

bool loadDesktopDefaultConfig() {
  setDefaultTheme();

  cfg.backend = "Pi-Star";
  cfg.wifiSsid = "DESKTOP";
  cfg.wifiPass = "";
  clearWifiProfiles();
  addWifiProfile(cfg.wifiSsid, cfg.wifiPass);
  cfg.wifiRetriesPerProfile = 0;

  cfg.pistarHost = "127.0.0.1";
  cfg.svxlinkEnabled = false;
  cfg.svxlinkSource = "url";
  cfg.svxlinkHost = "127.0.0.1";
  cfg.svxlinkApiPath = "/index.php";
  cfg.aprsEnabled = false;
  cfg.aprsApiKey = "";
  cfg.aprsRefreshMinutes = APRS_REFRESH_MINUTES_DEFAULT;

  cfg.startPageAfterBoot = "mmdvm_live";
  cfg.deviceCallsign = "DESKTOP";
  cfg.themeColor = "#35C2FF";
  cfg.refreshMs = 1500;
  cfg.callsignHoldMs = 12000;
  cfg.scrollingTextEnabled = true;
  cfg.apiUtcToLocalTime = true;
  cfg.timezoneTz = DEFAULT_TZ_RULE;
  cfg.brightness = 220;
  cfg.flipScreen = false;
  cfg.screensaverEnabled = false;
  cfg.screensaverTimeoutMs = 60000;
  cfg.screensaverDimBrightness = 20;

  cfg.weatherEnabled = false;
  cfg.weatherRefreshMinutes = 10;
  cfg.weatherLat = "";
  cfg.weatherLon = "";
  cfg.weatherApiKey = "";

  cfg.hamqslEnabled = false;
  cfg.hamqslRefreshMinutes = HAMQSL_REFRESH_MINUTES_DEFAULT;
  cfg.hamqslUrl = HAMQSL_SOLAR_XML_URL_DEFAULT;

  cfg.radioIdLookupEnabled = false;
  cfg.radioIdUsersApiBase = RADIOID_USERS_API_BASE_DEFAULT;
  cfg.targetListLookupEnabled = false;
  cfg.targetListBaseUrl = TARGET_LIST_BASE_URL_DEFAULT;
  cfg.targetListDmrUrl = "";
  cfg.targetListDstarUrl = "";
  cfg.targetListYsfUrl = "";
  cfg.targetListP25Url = "";
  cfg.targetListNxdnUrl = "";

  cfg.touchModeSshEnabled = false;
  cfg.pistarSshUser = "pi-star";
  cfg.pistarSshPass = "";
  cfg.pistarSshPort = 22;
  cfg.svxlinkSshEnabled = false;
  cfg.svxlinkSshUser = "svxlink";
  cfg.svxlinkSshPass = "";
  cfg.svxlinkSshPort = 22;

  for (uint8_t i = 0; i < QUICK_ACTION_SLOT_COUNT; i++) {
    cfg.quickActions[i].enabled = false;
    cfg.quickActions[i].useSudo = false;
    cfg.quickActions[i].label = i == 0 ? "DMR TG91" : (i == 1 ? "DMR TG263" : (i == 2 ? "REF001C" : "Disconnect"));
    cfg.quickActions[i].command = "";
  }
  for (uint8_t i = 0; i < SVXLINK_ACTION_SLOT_COUNT; i++) {
    cfg.svxlinkActions[i].enabled = false;
    cfg.svxlinkActions[i].useSudo = false;
    cfg.svxlinkActions[i].label = i == 0 ? "DTMF 1" : (i == 1 ? "DTMF 2" : (i == 2 ? "DTMF 3" : "Disconnect"));
    cfg.svxlinkActions[i].command = "";
  }

  bool fileOk = false;
  const char* usedPath = nullptr;
  std::string json = desktopReadWholeFile("sdcard/config.json", &fileOk);
  if (fileOk) usedPath = "sdcard/config.json";
  if (!fileOk) {
    json = desktopReadWholeFile("config.json", &fileOk);
    if (fileOk) usedPath = "config.json";
  }

  if (fileOk && json.size() > 0) {
    desktopApplyJsonString(json, "backend", cfg.backend);
    String rawBackendLower = trimSafe(cfg.backend);
    rawBackendLower.toLowerCase();
    bool migratedSvxBackend = rawBackendLower.indexOf("svxlink") >= 0;
    cfg.backend = canonicalBackendValue(cfg.backend);
    if (!cfg.backend.length()) cfg.backend = "Pi-Star";

    desktopApplyJsonString(json, "wifi_ssid", cfg.wifiSsid);
    desktopApplyJsonString(json, "wifi_pass", cfg.wifiPass);
    clearWifiProfiles();
    addWifiProfile(cfg.wifiSsid, cfg.wifiPass);
    desktopApplyJsonU8(json, "wifi_retries_per_profile", cfg.wifiRetriesPerProfile);

    desktopApplyJsonString(json, "pistar_host", cfg.pistarHost);
    desktopApplyJsonBool(json, "svxlink_enabled", cfg.svxlinkEnabled);
    if (migratedSvxBackend) cfg.svxlinkEnabled = true;
    desktopApplyJsonString(json, "svxlink_source", cfg.svxlinkSource);
    desktopApplyJsonString(json, "svxlink_host", cfg.svxlinkHost);
    desktopApplyJsonString(json, "svxlink_api_path", cfg.svxlinkApiPath);
    desktopApplyJsonBool(json, "aprs_enabled", cfg.aprsEnabled);
    desktopApplyJsonString(json, "aprs_api_key", cfg.aprsApiKey);
    desktopApplyJsonU16(json, "aprs_refresh_minutes", cfg.aprsRefreshMinutes);
    desktopApplyJsonString(json, "start_page_after_boot", cfg.startPageAfterBoot);
    desktopApplyJsonString(json, "device_callsign", cfg.deviceCallsign);
    desktopApplyJsonString(json, "theme_color", cfg.themeColor);
    desktopApplyJsonU32(json, "refresh_ms", cfg.refreshMs);
    desktopApplyJsonU32(json, "callsign_hold_ms", cfg.callsignHoldMs);
    desktopApplyJsonBool(json, "scrolling_text_enabled", cfg.scrollingTextEnabled);
    desktopApplyJsonBool(json, "api_utc_to_local_time", cfg.apiUtcToLocalTime);
    desktopApplyJsonString(json, "timezone_tz", cfg.timezoneTz);
    desktopApplyJsonU8(json, "brightness", cfg.brightness);
    desktopApplyJsonBool(json, "flip_screen", cfg.flipScreen);
    desktopApplyJsonBool(json, "screensaver_enabled", cfg.screensaverEnabled);
    desktopApplyJsonU32(json, "screensaver_timeout_ms", cfg.screensaverTimeoutMs);
    desktopApplyJsonU8(json, "screensaver_dim_brightness", cfg.screensaverDimBrightness);
    desktopApplyJsonBool(json, "weather_enabled", cfg.weatherEnabled);
    desktopApplyJsonU16(json, "weather_refresh_minutes", cfg.weatherRefreshMinutes);
    desktopApplyJsonString(json, "weather_lat", cfg.weatherLat);
    desktopApplyJsonString(json, "weather_lon", cfg.weatherLon);
    desktopApplyJsonString(json, "weather_api_key", cfg.weatherApiKey);
    desktopApplyJsonBool(json, "hamqsl_enabled", cfg.hamqslEnabled);
    desktopApplyJsonU16(json, "hamqsl_refresh_minutes", cfg.hamqslRefreshMinutes);
    desktopApplyJsonString(json, "hamqsl_url", cfg.hamqslUrl);
    desktopApplyJsonBool(json, "radioid_lookup_enabled", cfg.radioIdLookupEnabled);
    desktopApplyJsonString(json, "radioid_users_api_base", cfg.radioIdUsersApiBase);
    desktopApplyJsonBool(json, "target_list_lookup_enabled", cfg.targetListLookupEnabled);
    desktopApplyJsonString(json, "target_list_base_url", cfg.targetListBaseUrl);
    desktopApplyJsonString(json, "target_list_dmr_url", cfg.targetListDmrUrl);
    desktopApplyJsonString(json, "target_list_dstar_url", cfg.targetListDstarUrl);
    desktopApplyJsonString(json, "target_list_ysf_url", cfg.targetListYsfUrl);
    desktopApplyJsonString(json, "target_list_p25_url", cfg.targetListP25Url);
    desktopApplyJsonString(json, "target_list_nxdn_url", cfg.targetListNxdnUrl);
    desktopApplyJsonBool(json, "touch_mode_ssh_enabled", cfg.touchModeSshEnabled);
    desktopApplyJsonString(json, "pistar_ssh_user", cfg.pistarSshUser);
    desktopApplyJsonString(json, "pistar_ssh_pass", cfg.pistarSshPass);
    desktopApplyJsonU16(json, "pistar_ssh_port", cfg.pistarSshPort);
    desktopApplyJsonBool(json, "svxlink_ssh_enabled", cfg.svxlinkSshEnabled);
    desktopApplyJsonString(json, "svxlink_ssh_user", cfg.svxlinkSshUser);
    desktopApplyJsonString(json, "svxlink_ssh_pass", cfg.svxlinkSshPass);
    desktopApplyJsonU16(json, "svxlink_ssh_port", cfg.svxlinkSshPort);

    desktopParseActionArray(json, "quick_actions", cfg.quickActions, QUICK_ACTION_SLOT_COUNT);
    desktopParseActionArray(json, "svxlink_actions", cfg.svxlinkActions, SVXLINK_ACTION_SLOT_COUNT);

    applyBaseThemeColor(cfg.themeColor);
    desktopLoadThemeOverrides(json);
    desktopTrace("[DESKTOP] Loaded config from %s without ArduinoJson.\n", usedPath ? usedPath : "unknown");
    desktopTrace("[DESKTOP] Config: backend=%s, pistar_host=%s, callsign=%s, quick1=%s, touch_ssh=%s.\n",
                 cfg.backend.c_str(), cfg.pistarHost.c_str(), cfg.deviceCallsign.c_str(),
                 cfg.quickActions[0].label.c_str(), cfg.touchModeSshEnabled ? "true" : "false");
    desktopTrace("[DESKTOP] Config: weather=%s lat=%s lon=%s key=%s; aprs=%s key=%s refresh=%u min; hamqsl=%s refresh=%u min.\n",
                 cfg.weatherEnabled ? "true" : "false", cfg.weatherLat.c_str(), cfg.weatherLon.c_str(),
                 cfg.weatherApiKey.length() ? "set" : "missing",
                 cfg.aprsEnabled ? "true" : "false", cfg.aprsApiKey.length() ? "set" : "missing",
                 (unsigned)cfg.aprsRefreshMinutes,
                 cfg.hamqslEnabled ? "true" : "false", (unsigned)cfg.hamqslRefreshMinutes);
  } else {
    applyBaseThemeColor(cfg.themeColor);
    desktopTrace("[DESKTOP][WARN] config.json not found; using built-in desktop defaults.\n");
  }

  desktopFinalizeLoadedConfig();
  clearRadioIdLookupCache();
  currentWatchSource = WATCH_MMDVM;
  currentPage = PAGE_LIVE;
  previousPage = PAGE_LIVE;

  return hasConfiguredWifiProfiles() && cfg.pistarHost.length() && isValidBackendValue(cfg.backend);
}
#endif

bool loadConfig() {
  setDefaultTheme();

#if DISPLAY_BACKEND_IS_DESKTOP
  return loadDesktopDefaultConfig();
#endif

  if (!storageExists(CONFIG_FILE)) {
    if (!saveDefaultConfig()) return false;
  }

  File f = storageOpen(CONFIG_FILE, FILE_READ);
  if (!f) return false;

  DynamicJsonDocument doc(15360);
  DeserializationError err = deserializeJson(doc, f);
  f.close();
  if (err) return false;

  String rawBackendConfig = String(doc["backend"] | "");
  String rawBackendLower = trimSafe(rawBackendConfig);
  rawBackendLower.toLowerCase();
  bool migratedSvxBackend = rawBackendLower.indexOf("svxlink") >= 0;
  cfg.backend        = canonicalBackendValue(rawBackendConfig);
  if (!cfg.backend.length()) cfg.backend = "Pi-Star";
  clearWifiProfiles();
  cfg.wifiSsid       = String(doc["wifi_ssid"] | "");
  cfg.wifiPass       = String(doc["wifi_pass"] | "");
  addWifiProfile(cfg.wifiSsid, cfg.wifiPass);
  loadWifiProfilesFromJsonArray(doc["wifi_profiles"].as<JsonArray>());
  loadWifiProfilesFromJsonArray(doc["wifi"].as<JsonArray>());
  syncLegacyWifiFieldsFromProfiles();
  cfg.wifiRetriesPerProfile = doc["wifi_retries_per_profile"] | WIFI_PROFILE_RETRY_COUNT_DEFAULT;
  cfg.pistarHost     = String(doc["pistar_host"] | "");
  cfg.svxlinkEnabled = doc.containsKey("svxlink_enabled") ? (bool)doc["svxlink_enabled"] : migratedSvxBackend;
  String loadedSvxSource = String(doc["svxlink_source"] | "url");
  if (!doc.containsKey("svxlink_source") && migratedSvxBackend && rawBackendLower.indexOf("api") >= 0) loadedSvxSource = "api";
  cfg.svxlinkSource  = canonicalSvxlinkSourceValue(loadedSvxSource);
  if (!cfg.svxlinkSource.length()) cfg.svxlinkSource = "url";
  cfg.svxlinkHost    = String(doc["svxlink_host"] | "");
  cfg.svxlinkApiPath = String(doc["svxlink_api_path"] | "/index.php");
  cfg.aprsEnabled = doc["aprs_enabled"] | false;
  cfg.aprsApiKey = String(doc["aprs_api_key"] | "");
  cfg.aprsRefreshMinutes = doc["aprs_refresh_minutes"] | APRS_REFRESH_MINUTES_DEFAULT;
  cfg.startPageAfterBoot = canonicalStartPageAfterBootValue(String(doc["start_page_after_boot"] | "mmdvm_live"));
  cfg.deviceCallsign = String(doc["device_callsign"] | "");
  cfg.themeColor     = String(doc["theme_color"] | "#35C2FF");
  cfg.refreshMs      = doc["refresh_ms"] | 1500;
  cfg.callsignHoldMs = doc["callsign_hold_ms"] | 12000;
  cfg.scrollingTextEnabled = doc.containsKey("scrolling_text_enabled") ? (bool)doc["scrolling_text_enabled"] : true;
  cfg.apiUtcToLocalTime = doc["api_utc_to_local_time"] | true;
  cfg.timezoneTz    = String(doc["timezone_tz"] | DEFAULT_TZ_RULE);
  cfg.brightness     = doc["brightness"] | 220;
  cfg.flipScreen     = doc["flip_screen"] | false;
  cfg.screensaverEnabled = doc["screensaver_enabled"] | true;
  cfg.screensaverTimeoutMs = doc["screensaver_timeout_ms"] | 60000;
  cfg.screensaverDimBrightness = doc["screensaver_dim_brightness"] | 20;
  cfg.weatherEnabled = doc["weather_enabled"] | false;
  cfg.weatherRefreshMinutes = doc["weather_refresh_minutes"] | 10;
  cfg.weatherLat = String(doc["weather_lat"] | "");
  cfg.weatherLon = String(doc["weather_lon"] | "");
  cfg.weatherApiKey = String(doc["weather_api_key"] | "");
  cfg.hamqslEnabled = doc["hamqsl_enabled"] | false;
  cfg.hamqslRefreshMinutes = doc["hamqsl_refresh_minutes"] | HAMQSL_REFRESH_MINUTES_DEFAULT;
  cfg.hamqslUrl = String(doc["hamqsl_url"] | HAMQSL_SOLAR_XML_URL_DEFAULT);
  cfg.radioIdLookupEnabled = doc.containsKey("radioid_lookup_enabled") ? (bool)doc["radioid_lookup_enabled"] : true;
  cfg.radioIdUsersApiBase = String(doc["radioid_users_api_base"] | RADIOID_USERS_API_BASE_DEFAULT);
  cfg.targetListLookupEnabled = doc.containsKey("target_list_lookup_enabled") ? (bool)doc["target_list_lookup_enabled"] : true;
  cfg.targetListBaseUrl = trimSafe(String(doc["target_list_base_url"] | TARGET_LIST_BASE_URL_DEFAULT));
  if (!cfg.targetListBaseUrl.length()) cfg.targetListBaseUrl = String(TARGET_LIST_BASE_URL_DEFAULT);
  cfg.targetListDmrUrl = trimSafe(String(doc["target_list_dmr_url"] | ""));
  cfg.targetListDstarUrl = trimSafe(String(doc["target_list_dstar_url"] | ""));
  cfg.targetListYsfUrl = trimSafe(String(doc["target_list_ysf_url"] | ""));
  cfg.targetListP25Url = trimSafe(String(doc["target_list_p25_url"] | ""));
  cfg.targetListNxdnUrl = trimSafe(String(doc["target_list_nxdn_url"] | ""));
  cfg.touchModeSshEnabled = doc["touch_mode_ssh_enabled"] | false;
  cfg.pistarSshUser = String(doc["pistar_ssh_user"] | "pi-star");
  cfg.pistarSshPass = String(doc["pistar_ssh_pass"] | "");
  cfg.pistarSshPort = doc["pistar_ssh_port"] | 22;
  cfg.svxlinkSshEnabled = doc["svxlink_ssh_enabled"] | false;
  cfg.svxlinkSshUser = String(doc["svxlink_ssh_user"] | "svxlink");
  cfg.svxlinkSshPass = String(doc["svxlink_ssh_pass"] | "");
  cfg.svxlinkSshPort = doc["svxlink_ssh_port"] | 22;

  for (uint8_t i = 0; i < QUICK_ACTION_SLOT_COUNT; i++) {
    cfg.quickActions[i].enabled = false;
    cfg.quickActions[i].useSudo = false;
    cfg.quickActions[i].label = "";
    cfg.quickActions[i].command = "";
  }
  for (uint8_t i = 0; i < SVXLINK_ACTION_SLOT_COUNT; i++) {
    cfg.svxlinkActions[i].enabled = false;
    cfg.svxlinkActions[i].useSudo = false;
    cfg.svxlinkActions[i].label = "";
    cfg.svxlinkActions[i].command = "";
  }

  if (doc.containsKey("quick_actions") && doc["quick_actions"].is<JsonArray>()) {
    JsonArray qa = doc["quick_actions"].as<JsonArray>();
    uint8_t i = 0;
    for (JsonObject item : qa) {
      if (i >= QUICK_ACTION_SLOT_COUNT) break;
      cfg.quickActions[i].enabled = item["enabled"] | false;
      cfg.quickActions[i].useSudo = item["use_sudo"] | false;
      cfg.quickActions[i].label = String(item["label"] | "");
      cfg.quickActions[i].command = String(item["command"] | "");
      i++;
    }
  }
  if (doc.containsKey("svxlink_actions") && doc["svxlink_actions"].is<JsonArray>()) {
    JsonArray qa = doc["svxlink_actions"].as<JsonArray>();
    uint8_t i = 0;
    for (JsonObject item : qa) {
      if (i >= SVXLINK_ACTION_SLOT_COUNT) break;
      cfg.svxlinkActions[i].enabled = item["enabled"] | false;
      cfg.svxlinkActions[i].useSudo = item["use_sudo"] | false;
      cfg.svxlinkActions[i].label = String(item["label"] | "");
      cfg.svxlinkActions[i].command = String(item["command"] | "");
      i++;
    }
  }

  syncLegacyWifiFieldsFromProfiles();
  cfg.wifiRetriesPerProfile = effectiveWifiRetriesPerProfile();

  cfg.pistarHost = trimSafe(cfg.pistarHost);
  cfg.svxlinkSource = canonicalSvxlinkSourceValue(cfg.svxlinkSource);
  if (!cfg.svxlinkSource.length()) cfg.svxlinkSource = "url";
  cfg.svxlinkHost = trimSafe(cfg.svxlinkHost);
  cfg.svxlinkApiPath = normalizedSvxlinkPathForBackend(cfg.svxlinkApiPath);
  cfg.aprsApiKey = trimSafe(cfg.aprsApiKey);
  cfg.aprsRefreshMinutes = constrain(cfg.aprsRefreshMinutes, APRS_REFRESH_MINUTES_MIN, APRS_REFRESH_MINUTES_MAX);
  cfg.startPageAfterBoot = normalizedStartPageAfterBoot(false);
  cfg.deviceCallsign = trimSafe(cfg.deviceCallsign);
  cfg.timezoneTz = trimSafe(cfg.timezoneTz);
  if (!cfg.timezoneTz.length()) cfg.timezoneTz = String(DEFAULT_TZ_RULE);
  cfg.themeColor = trimSafe(cfg.themeColor);
  cfg.weatherLat = trimSafe(cfg.weatherLat);
  cfg.weatherLon = trimSafe(cfg.weatherLon);
  cfg.weatherApiKey = trimSafe(cfg.weatherApiKey);
  cfg.hamqslUrl = trimSafe(cfg.hamqslUrl);
  if (!cfg.hamqslUrl.length()) cfg.hamqslUrl = String(HAMQSL_SOLAR_XML_URL_DEFAULT);
  cfg.radioIdUsersApiBase = trimSafe(cfg.radioIdUsersApiBase);
  cfg.pistarSshUser = trimSafe(cfg.pistarSshUser);
  cfg.svxlinkSshUser = trimSafe(cfg.svxlinkSshUser);
  if (!cfg.radioIdUsersApiBase.length()) cfg.radioIdUsersApiBase = String(RADIOID_USERS_API_BASE_DEFAULT);
  if (!cfg.pistarSshUser.length()) cfg.pistarSshUser = "pi-star";
  if (!cfg.svxlinkSshUser.length()) cfg.svxlinkSshUser = "svxlink";
  if (cfg.pistarSshPort == 0) cfg.pistarSshPort = 22;
  if (cfg.svxlinkSshPort == 0) cfg.svxlinkSshPort = 22;
  for (uint8_t i = 0; i < QUICK_ACTION_SLOT_COUNT; i++) {
    cfg.quickActions[i].label = trimSafe(cfg.quickActions[i].label);
    cfg.quickActions[i].command = trimSafe(cfg.quickActions[i].command);
    if (!cfg.quickActions[i].label.length() || !cfg.quickActions[i].command.length()) {
      cfg.quickActions[i].enabled = false;
    }
  }
  for (uint8_t i = 0; i < SVXLINK_ACTION_SLOT_COUNT; i++) {
    cfg.svxlinkActions[i].label = trimSafe(cfg.svxlinkActions[i].label);
    cfg.svxlinkActions[i].command = trimSafe(cfg.svxlinkActions[i].command);
    if (!cfg.svxlinkActions[i].label.length() || !cfg.svxlinkActions[i].command.length()) {
      cfg.svxlinkActions[i].enabled = false;
    }
  }
  if (!svxlinkFeatureEnabled() && currentWatchSource == WATCH_SVXLINK) currentWatchSource = WATCH_MMDVM;
  if (!aprsFeatureEnabled() && currentWatchSource == WATCH_APRS) currentWatchSource = WATCH_MMDVM;
  if (!weatherFeatureEnabled() && currentWatchSource == WATCH_WEATHER) currentWatchSource = WATCH_MMDVM;
  if (!hamqslFeatureEnabled() && currentWatchSource == WATCH_HAMQSL) currentWatchSource = WATCH_MMDVM;

  cfg.brightness = constrain(cfg.brightness, 0, 255);
  cfg.screensaverDimBrightness = constrain(cfg.screensaverDimBrightness, 0, 255);
  cfg.weatherRefreshMinutes = constrain(cfg.weatherRefreshMinutes, (uint16_t)1, (uint16_t)1440);
  cfg.hamqslRefreshMinutes = constrain(cfg.hamqslRefreshMinutes, HAMQSL_REFRESH_MINUTES_MIN, HAMQSL_REFRESH_MINUTES_MAX);
  if (cfg.screensaverTimeoutMs < 5000) cfg.screensaverTimeoutMs = 5000;
  if (!weatherConfigured()) clearWeatherData();
  if (!hamqslConfigured()) clearHamqslData();

  applyBaseThemeColor(cfg.themeColor);

  clearRadioIdLookupCache();

  if (doc.containsKey("theme") && doc["theme"].is<JsonObject>()) {
    JsonObject themeObj = doc["theme"].as<JsonObject>();
    loadThemeFromJson(themeObj);
  }

  return hasConfiguredWifiProfiles() && cfg.pistarHost.length() && isValidBackendValue(cfg.backend);
}

void applyBrightness() {
#if DISPLAY_BACKEND_IS_WS_RGB_800_480
  ws8048Backlight(screensaverActive ? cfg.screensaverDimBrightness : cfg.brightness);
#elif DISPLAY_BACKEND_IS_DESKTOP

#else
  if (TFT_BL >= 0) analogWrite(TFT_BL, screensaverActive ? cfg.screensaverDimBrightness : cfg.brightness);
#endif
}

static inline bool runtimeFlipScreenEnabled() {
  return cfg.flipScreen;
}

static inline uint8_t currentDisplayRotation() {
  return runtimeFlipScreenEnabled() ? DISPLAY_ROTATION_FLIPPED : DISPLAY_ROTATION_NORMAL;
}

void applyPanelOrientation() {
  tft.setRotation(currentDisplayRotation());
}

void applyDisplaySettings() {
  applyPanelOrientation();
  tft.fillScreen(theme.bg);
  applyBrightness();
  requestRedraw();
}

void setScreensaverActive(bool active) {
  if (screensaverActive == active) return;
  screensaverActive = active;
  applyBrightness();
  footerDirty = true;
  requestRedraw();
  if (active) {
    detailPopupOpen = false;
    selectedHeardIndex = -1;
    lastScreensaverFrameMillis = 0;
  }
}

void noteScreenActivity(bool fromTouch) {
  lastInteractionMillis = millis();
  if (screensaverActive) {
    setScreensaverActive(false);
    tft.fillScreen(theme.bg);
    requestRedraw();
  }
}

void noteUserInteraction(bool fromTouch) {
  noteScreenActivity(fromTouch);
}

void setBrightnessFromTouchX(int x) {
  int val = map(x, BRIGHT_SLIDER_X + max(2, UW(4)), BRIGHT_SLIDER_X + BRIGHT_SLIDER_W - max(2, UW(5)), 0, 255);
  cfg.brightness = constrain(val, 0, 255);
  applyBrightness();
}

bool connectWiFi(uint32_t timeoutMs = WIFI_CONNECT_TIMEOUT_MS) {
  (void)timeoutMs;
#if DISPLAY_BACKEND_IS_DESKTOP
  wifiOk = true;
  wifiConnectInProgress = false;
  wifiConnectProfileIndex = -1;
  WiFi.mode(WIFI_STA);
  WiFi.persistent(false);
  WiFi.begin(cfg.wifiSsid.c_str(), cfg.wifiPass.c_str());
  syncTimeIfNeeded();
  setFooterText("Desktop network: " + WiFi.localIP().toString());
  addWebLog("INFO", "WIFI", "Desktop network simulated. IP " + WiFi.localIP().toString() + ".");
  return true;
#endif

  if (!hasConfiguredWifiProfiles()) {
    wifiOk = false;
    wifiConnectInProgress = false;
    wifiConnectProfileIndex = -1;
    setFooterText("Wi-Fi profile missing");
    addWebLog("WARN", "WIFI", "No Wi-Fi profiles are configured.");
    return false;
  }

  WiFi.mode(apModeActive ? WIFI_AP_STA : WIFI_STA);
  WiFi.persistent(false);
  WiFi.setAutoReconnect(true);
  delay(300);

  int networkCount = scanWifiNetworksForProfiles();
  bool anyVisible = networkCount < 0;
  if (networkCount >= 0) {
    for (uint8_t i = 0; i < MAX_WIFI_PROFILES; i++) {
      if (wifiSsidLooksUsable(cfg.wifiProfiles[i].ssid) && wifiSsidVisibleInScan(cfg.wifiProfiles[i].ssid, networkCount)) {
        anyVisible = true;
        break;
      }
    }
  }

  if (!anyVisible) {
    wifiOk = false;
    wifiConnectInProgress = false;
    wifiConnectProfileIndex = -1;
    setFooterText("No saved Wi-Fi found");
    addWebLog("WARN", "WIFI", "None of the saved Wi-Fi profiles are visible.");
    return false;
  }

  uint8_t attemptsPerProfile = wifiAttemptsPerProfile();

  for (uint8_t i = 0; i < MAX_WIFI_PROFILES; i++) {
    if (!wifiSsidLooksUsable(cfg.wifiProfiles[i].ssid)) continue;
    if (networkCount >= 0 && !wifiSsidVisibleInScan(cfg.wifiProfiles[i].ssid, networkCount)) {
      addWebLog("INFO", "WIFI", "SSID \"" + cfg.wifiProfiles[i].ssid + "\" is not visible, skipping.");
      continue;
    }

    for (uint8_t attempt = 1; attempt <= attemptsPerProfile; attempt++) {
      setFooterText("Connecting Wi-Fi: " + cfg.wifiProfiles[i].ssid + wifiAttemptSuffix(attempt));
      requestRedraw();
      beginWifiProfileConnection(i, attempt == 1 ? "Connecting to" : "Retrying", attempt);

      unsigned long start = millis();
      wifiConnectInProgress = true;
      wifiConnectStartedAt = start;
      lastWifiRetryMillis = start;

      while (WiFi.status() != WL_CONNECTED && millis() - start < timeoutMs) {
#if DISPLAY_BACKEND_IS_DESKTOP
#else
  handleWebClientSafely();
#endif
        delay(50);
      }

      wifiConnectInProgress = false;
      wifiOk = (WiFi.status() == WL_CONNECTED);
      if (wifiOk) {
        syncTimeIfNeeded();
        if (apModeActive) stopSetupAccessPoint();
        addWebLog("INFO", "WIFI", "Connected to \"" + cfg.wifiProfiles[i].ssid + "\" on attempt " + String(attempt) + "/" + String(attemptsPerProfile) + ". IP " + WiFi.localIP().toString() + ".");
        setFooterText("Wi-Fi OK: " + WiFi.localIP().toString());
        return true;
      }

      addWebLog("WARN", "WIFI", "Connection to \"" + cfg.wifiProfiles[i].ssid + "\" failed on attempt " + String(attempt) + "/" + String(attemptsPerProfile) + ".");
      WiFi.disconnect(false, false);
      delay(200);
    }
  }

  wifiConnectProfileIndex = -1;
  wifiOk = false;
  setFooterText("Wi-Fi failed");
  return false;
}

void syncTimeIfNeeded() {
  if (!timeConfigured) {
    String tzRule = trimSafe(cfg.timezoneTz);
    if (!tzRule.length()) tzRule = String(DEFAULT_TZ_RULE);
    configTzTime(tzRule.c_str(), "pool.ntp.org", "time.nist.gov", "time.google.com");
    timeConfigured = true;
  }
}

void beginWiFiConnectAttempt(bool stopApFirst) {
  (void)stopApFirst;

  if (!hasConfiguredWifiProfiles()) {
    wifiOk = false;
    wifiConnectInProgress = false;
    wifiConnectProfileIndex = -1;
    setupRequired = true;
    currentPage = PAGE_AP;
    setFooterText("Wi-Fi profile missing");
    addWebLog("WARN", "WIFI", "No Wi-Fi profiles are configured.");
    startSetupAccessPoint();
    requestRedraw();
    return;
  }

  resetWifiConnectRound();
  int8_t profileIndex = findNextVisibleWifiProfileIndex(wifiNextProfileIndex);
  if (profileIndex < 0) {
    wifiOk = false;
    wifiConnectInProgress = false;
    wifiConnectProfileIndex = -1;
    setupRequired = true;
    currentPage = PAGE_AP;
    addWebLog("WARN", "WIFI", "No saved Wi-Fi profile is currently visible. Setup AP remains active.");
    startSetupAccessPoint();
    setFooterText(apModeActive ? ("Setup required: " + apSsid + " / http://" + WiFi.softAPIP().toString() + "/") : "No saved Wi-Fi found");
    requestRedraw();
    return;
  }

  if (!beginWifiProfileConnection((uint8_t)profileIndex, "Starting reconnect to", 1)) {
    wifiOk = false;
    wifiConnectInProgress = false;
    wifiConnectProfileIndex = -1;
    setupRequired = true;
    startSetupAccessPoint();
    requestRedraw();
    return;
  }

  wifiOk = false;
  wifiConnectInProgress = true;
  wifiConnectStartedAt = millis();
  lastWifiRetryMillis = wifiConnectStartedAt;
  setFooterText("Connecting Wi-Fi: " + cfg.wifiProfiles[profileIndex].ssid + wifiAttemptSuffix(wifiConnectAttemptNumber));
  requestRedraw();
}

void handleWiFiConnectProgress() {
  if (!wifiConnectInProgress) return;

  wl_status_t status = WiFi.status();
  if (status == WL_CONNECTED) {
    String connectedSsid = (wifiConnectProfileIndex >= 0 && wifiConnectProfileIndex < MAX_WIFI_PROFILES) ? cfg.wifiProfiles[wifiConnectProfileIndex].ssid : String("Wi-Fi");
    uint8_t connectedAttempt = wifiConnectAttemptNumber;
    wifiConnectInProgress = false;
    wifiOk = true;
    setupRequired = false;
    resetWifiConnectRound();
    syncTimeIfNeeded();
    if (apModeActive) stopSetupAccessPoint();
    setFooterText("Wi-Fi OK: " + WiFi.localIP().toString());
    addWebLog("INFO", "WIFI", "Connected to \"" + connectedSsid + "\" on attempt " + String(connectedAttempt) + "/" + String(wifiAttemptsPerProfile()) + ". IP " + WiFi.localIP().toString() + ".");
    requestRedraw();
    return;
  }

  if (millis() - wifiConnectStartedAt >= WIFI_CONNECT_TIMEOUT_MS) {
    int8_t failedProfile = wifiConnectProfileIndex;
    uint8_t failedAttempt = wifiConnectAttemptNumber;
    uint8_t attemptsPerProfile = wifiAttemptsPerProfile();
    wifiConnectInProgress = false;
    wifiOk = false;
    WiFi.disconnect(false, false);

    if (configLooksValid()) {
      if (failedProfile >= 0 && failedProfile < MAX_WIFI_PROFILES && failedAttempt < attemptsPerProfile) {
        uint8_t nextAttempt = failedAttempt + 1;
        addWebLog("WARN", "WIFI", "Wi-Fi connect failed. Retrying same saved profile.");
        if (beginWifiProfileConnection((uint8_t)failedProfile, "Retrying same saved profile", nextAttempt)) {
          wifiConnectInProgress = true;
          wifiConnectStartedAt = millis();
          lastWifiRetryMillis = wifiConnectStartedAt;
          setFooterText("Connecting Wi-Fi: " + cfg.wifiProfiles[failedProfile].ssid + wifiAttemptSuffix(nextAttempt));
          requestRedraw();
          return;
        }
      }

      if (failedProfile >= 0 && failedProfile < MAX_WIFI_PROFILES) wifiProfileTriedThisRound[failedProfile] = true;
      int8_t nextProfile = findNextVisibleWifiProfileIndex(wifiNextProfileIndex, -1, true);
      if (nextProfile >= 0) {
        addWebLog("WARN", "WIFI", "Wi-Fi connect failed after " + String(attemptsPerProfile) + " attempt(s). Trying the next visible saved profile.");
        if (beginWifiProfileConnection((uint8_t)nextProfile, "Trying next saved profile", 1)) {
          wifiConnectInProgress = true;
          wifiConnectStartedAt = millis();
          lastWifiRetryMillis = wifiConnectStartedAt;
          setFooterText("Connecting Wi-Fi: " + cfg.wifiProfiles[nextProfile].ssid + wifiAttemptSuffix(1));
          requestRedraw();
          return;
        }
      }
    }

    setupRequired = true;
    currentPage = PAGE_AP;
    addWebLog("WARN", "WIFI", "No saved Wi-Fi profile connected. Setup AP is active.");
    startSetupAccessPoint();
    setFooterText(apModeActive ? ("Setup required: " + apSsid + " / http://" + WiFi.softAPIP().toString() + "/") : "Wi-Fi failed");
    requestRedraw();
  }
}

bool startSetupAccessPoint() {
#if DISPLAY_BACKEND_IS_DESKTOP
  if (apModeActive) {
    IPAddress ip = WiFi.softAPIP();
    setFooterText("Desktop setup: http://" + ip.toString() + "/");
    addWebLog("INFO", "AP", "Desktop setup mode already active at " + ip.toString() + ".");
    return true;
  }

  apSsid = "MMDVM-Display-DESKTOP";
  apModeActive = true;
  IPAddress ip = WiFi.softAPIP();
  setFooterText("Desktop setup: http://" + ip.toString() + "/");
  desktopTrace("[DESKTOP] Setup AP simulated at %s.\n", ip.toString().c_str());
  addWebLog("INFO", "AP", "Desktop setup AP simulated at " + ip.toString() + ".");
  return true;
#else
  if (apModeActive) {
    IPAddress ip = WiFi.softAPIP();
    setFooterText("Setup AP: " + apSsid + " / " + ip.toString());
    addWebLog("INFO", "AP", "Setup AP already active at " + ip.toString() + ".");
    return true;
  }

  WiFi.disconnect(false, false);
  delay(200);
  WiFi.mode(hasConfiguredWifiProfiles() ? WIFI_AP_STA : WIFI_AP);

  String chip = String((uint32_t)(ESP.getEfuseMac() & 0xFFFFFF), HEX);
  chip.toUpperCase();
  while (chip.length() < 6) chip = "0" + chip;
  apSsid = "MMDVM-Display-" + chip;

  bool ok = WiFi.softAP(apSsid.c_str(), apPass.c_str());
  apModeActive = ok;
  if (ok) {
    IPAddress ip = WiFi.softAPIP();
    setFooterText("Setup AP: " + apSsid + " / " + ip.toString());
    addWebLog("INFO", "AP", "Setup AP started as " + apSsid + " at " + ip.toString() + ".");
  } else {
    setFooterText("Setup AP failed");
    addWebLog("ERROR", "AP", "Failed to start setup AP.");
  }
  return ok;
#endif
}

void stopSetupAccessPoint() {
  if (!apModeActive) return;
  WiFi.softAPdisconnect(true);
  apModeActive = false;
  addWebLog("INFO", "AP", "Setup AP stopped.");
}

String htmlEscape(const String& s) {
  String out = s;
  out.replace("&", "&amp;");
  out.replace("<", "&lt;");
  out.replace(">", "&gt;");
  out.replace("\"", "&quot;");
  out.replace("'", "&#39;");
  return out;
}

String formRow(const String& label, const String& name, const String& value, const String& type = "text", const String& placeholder = "") {
  String row = "<div class='form-row'>";
  row += "<label for='" + name + "'>" + label + "</label>";
  row += "<input type='" + type + "' id='" + name + "' name='" + name + "' value='" + htmlEscape(value) + "'";
  if (placeholder.length()) row += " placeholder='" + htmlEscape(placeholder) + "'";
  row += ">";
  row += "</div>";
  return row;
}

int hexNibbleValue(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  return -1;
}

String decodeHexFormValue(const String& hex) {
  if (hex.length() == 0 || (hex.length() % 2) != 0) return "";

  String out;
  out.reserve(hex.length() / 2);
  for (size_t i = 0; i < hex.length(); i += 2) {
    int hi = hexNibbleValue(hex.charAt(i));
    int lo = hexNibbleValue(hex.charAt(i + 1));
    if (hi < 0 || lo < 0) return "";
    out += (char)((hi << 4) | lo);
  }
  return out;
}

String wifiFormArg(const String& name) {
  String hexName = name + "_hex";
  if (server.hasArg(hexName)) {
    String decoded = decodeHexFormValue(server.arg(hexName));
    if (decoded.length() || server.arg(hexName).length() == 0) return decoded;
  }
  return server.arg(name);
}

String selectRow(const String& label, const String& name, const String& current) {
  String canonical = canonicalBackendValue(current);
  if (!canonical.length()) canonical = "Pi-Star";
  String row = "<label><span>" + htmlEscape(label) + "</span><select name='" + htmlEscape(name) + "'>";
  row += "<option value='Pi-Star'";
  if (canonical == "Pi-Star") row += " selected";
  row += ">Pi-Star</option>";
  row += "<option value='WPSD'";
  if (canonical == "WPSD") row += " selected";
  row += ">WPSD</option>";
  row += "</select></label>";
  return row;
}


String colorRow(const String& label, const String& name, const String& value) {
  String row = "<div class='form-row'>";
  row += "<label for='" + name + "'>" + label + "</label>";
  row += "<div class='color-inline'>";
  row += "<input class='color-picker' type='color' id='" + name + "_picker' value='" + htmlEscape(value) + "' ";
  row += "oninput=\"document.getElementById('" + name + "').value=this.value\">";
  row += "<input class='color-text' type='text' id='" + name + "' name='" + name + "' value='" + htmlEscape(value) + "'>";
  row += "</div>";
  row += "</div>";
  return row;
}

String checkboxRow(const String& label, const String& name, bool checked, const String& hint = "") {
  String row = "<div class='form-row'>";
  row += "<label for='" + name + "'>" + label + "</label>";
  row += "<div>";
  row += "<label class='toggle-row'><input type='checkbox' id='" + name + "' name='" + name + "' value='1'";
  if (checked) row += " checked";
  row += ">Enable</label>";
  if (hint.length()) row += "<div class='mini-note' style='margin:8px 0 0 0'>" + htmlEscape(hint) + "</div>";
  row += "</div></div>";
  return row;
}

String startPageAfterBootSelectRow(bool includeSvxlinkOptions, bool includeAprsOptions, bool includeWeatherOptions, bool includeHamqslOptions) {
  String current = normalizedStartPageAfterBoot(false);
  if ((current == "svxlink_live" || current == "svxlink_last") && !includeSvxlinkOptions) current = "mmdvm_live";
  if (current == "aprs_live" && !includeAprsOptions) current = "mmdvm_live";
  if (current == "weather_live" && !includeWeatherOptions) current = "mmdvm_live";
  if (current == "hamqsl_live" && !includeHamqslOptions) current = "mmdvm_live";

  String row = "<div class='form-row'>";
  row += "<label for='start_page_after_boot'>Start Page after boot</label>";
  row += "<div><select id='start_page_after_boot' name='start_page_after_boot'>";
  row += "<option value='mmdvm_live'";
  if (current == "mmdvm_live") row += " selected";
  row += ">MMDVM LIVE / WATCH</option>";
  row += "<option value='mmdvm_last'";
  if (current == "mmdvm_last") row += " selected";
  row += ">MMDVM HEARD</option>";
  if (includeSvxlinkOptions) {
    row += "<option value='svxlink_live'";
    if (current == "svxlink_live") row += " selected";
    row += ">SVXLink LIVE / WATCH</option>";
    row += "<option value='svxlink_last'";
    if (current == "svxlink_last") row += " selected";
    row += ">SVXLink HEARD</option>";
  }
  if (includeAprsOptions) {
    row += "<option value='aprs_live'";
    if (current == "aprs_live") row += " selected";
    row += ">APRS WATCH</option>";
  }
  if (includeWeatherOptions) {
    row += "<option value='weather_live'";
    if (current == "weather_live") row += " selected";
    row += ">Weather WATCH</option>";
  }
  if (includeHamqslOptions) {
    row += "<option value='hamqsl_live'";
    if (current == "hamqsl_live") row += " selected";
    row += ">HAMQSL WATCH</option>";
  }
  row += "</select><div class='mini-note' style='margin:8px 0 0 0'>Shown when APRS, SVXLink, Weather or HAMQSL is enabled. If the selected source is later disabled or incomplete, boot falls back to MMDVM LIVE.</div></div></div>";
  return row;
}

void readStartPageAfterBootFromForm() {
  if (!server.hasArg("start_page_after_boot")) return;
  cfg.startPageAfterBoot = canonicalStartPageAfterBootValue(server.arg("start_page_after_boot"));
  if (!startPageAfterBootChoiceAvailable(cfg.startPageAfterBoot, false)) cfg.startPageAfterBoot = "mmdvm_live";
}

String navItem(const String& id, const String& label, bool active = false) {
  String s = "<a class='side-link";
  if (active) s += " active";
  s += "' href='#" + id + "'>" + label + "</a>";
  return s;
}

String navHrefItem(const String& href, const String& label, bool active = false) {
  String s = "<a class='side-link";
  if (active) s += " active";
  s += "' href='" + href + "'>" + label + "</a>";
  return s;
}

String buildTopbarBrandHtml() {
  String s;
  s.reserve(240);
  s += "<div class='topbar-left'>";
  s += "<button class='sidebar-toggle' type='button' aria-label='Open navigation' onclick='toggleSidebar(true)'>&#9776;</button>";
  s += "<div><div class='brand'>MMDVM Display</div><div class='brand-sub'>Version " + String(DISPLAY_VERSION) + " by 13MAD86 / Martin</div></div>";
  s += "</div>";
  return s;
}

String buildSidebarMobileCss() {
  return R"SIDEBARCSS(*{scrollbar-width:none;-ms-overflow-style:none}*::-webkit-scrollbar{width:0;height:0;display:none}.topbar-left{display:flex;align-items:center;gap:12px;min-width:0}.sidebar-toggle,.sidebar-close{appearance:none;-webkit-appearance:none;border:1px solid var(--line);background:rgba(255,255,255,.05);color:#fff;border-radius:14px;cursor:pointer;transition:background .18s ease,border-color .18s ease,transform .18s ease}.sidebar-toggle{display:none;align-items:center;justify-content:center;width:44px;height:44px;padding:0;font-size:22px;line-height:1;box-shadow:none}.sidebar-toggle:hover,.sidebar-close:hover{background:rgba(255,255,255,.10);border-color:rgba(255,255,255,.22);transform:translateY(-1px)}.sidebar-backdrop{display:none}.sidebar-mobile-head{display:none}body.sidebar-open{overflow:hidden}@media(max-width:1080px){.topbar{padding:14px 16px}.topbar-left{width:100%;align-items:center}.brand{font-size:24px}.brand-sub{font-size:12px}.sidebar-toggle{display:inline-flex;flex:0 0 auto}.sidebar-backdrop{display:block;position:fixed;inset:0;background:rgba(2,6,23,.58);opacity:0;visibility:hidden;pointer-events:none;transition:opacity .26s ease,visibility .26s ease;z-index:90}.sidebar{position:fixed !important;top:0 !important;left:0;width:min(82vw,300px) !important;max-width:300px;height:100dvh !important;padding:16px 14px 22px !important;border-right:1px solid var(--line) !important;border-bottom:none !important;overflow:auto;transform:translateX(-104%);opacity:.35;transition:transform .28s ease,opacity .28s ease;z-index:100;box-shadow:0 24px 60px rgba(2,6,23,.48)}.sidebar-mobile-head{display:flex;align-items:center;justify-content:space-between;gap:12px;margin-bottom:8px;padding:4px 6px 12px;border-bottom:1px solid var(--line2)}.sidebar-mobile-title{font-size:12px;text-transform:uppercase;letter-spacing:.16em;color:var(--muted);font-weight:800}.sidebar-close{display:inline-flex;align-items:center;justify-content:center;width:38px;height:38px;padding:0;font-size:26px;line-height:1}.side-title{margin-left:6px}.topbar-right{width:100%;justify-content:flex-start}.header-actions{justify-content:flex-start}.btn-compact{min-height:42px}.content{padding:18px}.page-head{flex-direction:column}.stats-grid,.mini-grid,.form-grid,.theme-grid,.startup-grid,.quick-grid{grid-template-columns:1fr}body.sidebar-open .sidebar{transform:translateX(0);opacity:1}body.sidebar-open .sidebar-backdrop{opacity:1;visibility:visible;pointer-events:auto}}</style>)SIDEBARCSS";
}

String buildSidebarMobileScript() {
  return R"SIDEBARSCRIPT(<script>(function(){function mobileNav(){return window.matchMedia('(max-width: 1080px)').matches;}window.toggleSidebar=function(open){if(!mobileNav()){document.body.classList.remove('sidebar-open');return;}document.body.classList.toggle('sidebar-open',!!open);};window.addEventListener('resize',function(){if(!mobileNav())document.body.classList.remove('sidebar-open');});window.addEventListener('keydown',function(e){if(e.key==='Escape')document.body.classList.remove('sidebar-open');});document.addEventListener('click',function(e){var target=e.target;if(!target||!target.closest)return;var link=target.closest('.sidebar .side-link');if(link&&mobileNav())document.body.classList.remove('sidebar-open');});})();</script>)SIDEBARSCRIPT";
}


String jsonEscape(const String& s) {
  String out;
  out.reserve(s.length() + 8);
  for (size_t i = 0; i < s.length(); i++) {
    char c = s.charAt(i);
    switch (c) {
      case '\\': out += "\\\\"; break;
      case '"': out += "\\\""; break;
      case '\n': out += "\\n"; break;
      case '\r': out += "\\r"; break;
      case '\t': out += "\\t"; break;
      default: out += c; break;
    }
  }
  return out;
}

String formatLogAgeBrief(unsigned long sinceMs) {
  unsigned long now = millis();
  unsigned long delta = (sinceMs && now >= sinceMs) ? (now - sinceMs) : 0;
  unsigned long totalSeconds = delta / 1000UL;
  unsigned long hours = totalSeconds / 3600UL;
  unsigned long minutes = (totalSeconds % 3600UL) / 60UL;
  unsigned long seconds = totalSeconds % 60UL;

  if (hours > 0) return String(hours) + "h " + String(minutes) + "m ago";
  if (minutes > 0) return String(minutes) + "m " + String(seconds) + "s ago";
  return String(seconds) + "s ago";
}

void addWebLog(const String& level, const String& tag, const String& message, bool mirrorSerial) {
  String safeLevel = trimSafe(level);
  String safeTag = trimSafe(tag);
  String safeMessage = message;
  safeMessage.replace("\r", " ");
  safeMessage.replace("\n", " ");
  if (safeMessage.length() > 220) safeMessage = safeMessage.substring(0, 217) + "...";

  size_t idx = webLogHead;
  webLogBuffer[idx].ms = millis();
  webLogBuffer[idx].level = safeLevel.length() ? safeLevel : String("INFO");
  webLogBuffer[idx].tag = safeTag;
  webLogBuffer[idx].message = safeMessage;

  webLogHead = (webLogHead + 1) % WEB_LOG_CAPACITY;
  if (webLogCount < WEB_LOG_CAPACITY) webLogCount++;

  if (mirrorSerial) {
    String line = "[" + webLogBuffer[idx].level + "]";
    if (webLogBuffer[idx].tag.length()) line += "[" + webLogBuffer[idx].tag + "]";
    line += " " + webLogBuffer[idx].message;
    Serial.println(line);
  }
}

String buildWebLogsJson() {
  String json;
  json.reserve(24000);
  json += "{";
  json += "\"count\":" + String(webLogCount) + ",";
  json += "\"capacity\":" + String(WEB_LOG_CAPACITY) + ",";
  json += "\"items\":[";
  if (webLogCount) {
    size_t start = (webLogHead + WEB_LOG_CAPACITY - webLogCount) % WEB_LOG_CAPACITY;
    for (size_t i = 0; i < webLogCount; i++) {
      size_t idx = (start + i) % WEB_LOG_CAPACITY;
      if (i) json += ",";
      json += "{";
      json += "\"age\":\"" + jsonEscape(formatLogAgeBrief(webLogBuffer[idx].ms)) + "\",";
      json += "\"level\":\"" + jsonEscape(webLogBuffer[idx].level) + "\",";
      json += "\"tag\":\"" + jsonEscape(webLogBuffer[idx].tag) + "\",";
      json += "\"message\":\"" + jsonEscape(webLogBuffer[idx].message) + "\"";
      json += "}";
    }
  }
  json += "]}";
  return json;
}

String formatMillisAgoBrief(unsigned long sinceMs) {
  if (!sinceMs) return "Never";
  unsigned long delta = millis() - sinceMs;
  if (delta < 1000UL) return "Just now";
  if (delta < 60000UL) return String(delta / 1000UL) + "s ago";
  if (delta < 3600000UL) return String(delta / 60000UL) + "m ago";
  if (delta < 86400000UL) return String(delta / 3600000UL) + "h ago";
  return String(delta / 86400000UL) + "d ago";
}

String otaErrorText() {
  switch (Update.getError()) {
    case UPDATE_ERROR_OK: return "";
    case UPDATE_ERROR_WRITE: return "Flash write failed.";
    case UPDATE_ERROR_ERASE: return "Flash erase failed.";
    case UPDATE_ERROR_READ: return "Flash read failed.";
    case UPDATE_ERROR_SPACE: return "Not enough free space for this firmware image.";
    case UPDATE_ERROR_SIZE: return "Firmware image size is invalid.";
    case UPDATE_ERROR_STREAM: return "Firmware upload stream failed.";
    case UPDATE_ERROR_MD5: return "Firmware checksum validation failed.";
    case UPDATE_ERROR_MAGIC_BYTE: return "Uploaded file is not a valid ESP firmware image.";
    case UPDATE_ERROR_ACTIVATE: return "Updated firmware could not be activated.";
    case UPDATE_ERROR_NO_PARTITION: return "No OTA app partition available. Flash once by USB with an OTA partition table (ota_0 + ota_1).";
    case UPDATE_ERROR_BAD_ARGUMENT: return "Bad OTA argument.";
    case UPDATE_ERROR_ABORT: return "OTA update aborted.";
    default: return "OTA update failed with error " + String(Update.getError()) + ".";
  }
}

String buildSidebarHtml(bool onRoot, const String& activeApp) {
  String s;
  s.reserve(1600);
  s += "<div class='sidebar-backdrop' onclick='toggleSidebar(false)'></div>";
  s += "<aside class='sidebar'>";
  s += "<div class='sidebar-mobile-head'><div class='sidebar-mobile-title'>Navigation</div><button class='sidebar-close' type='button' aria-label='Close navigation' onclick='toggleSidebar(false)'>&times;</button></div>";
  s += "<div class='side-title' style='margin-top:18px'>Configuration</div>";
  if (onRoot) {
    s += navItem("system", "&#128223; System mode");
    s += navItem("mmdvm", "&#128246; MMDVM");
    s += navItem("quick-actions", "&#9889; Quick actions");
    s += navItem("network", "&#128655; Network");
    s += navItem("wifi", "&#128225; Wifi");
    s += navItem("display", "&#128250; Display");
    s += navItem("weather", "&#9925; Weather");
    s += navItem("hamqsl", "&#9728; HAMQSL");
    s += navItem("lookup", "&#128218; Lookup");
    s += navItem("theme", "&#127912; Theme");
  } else {
    s += navHrefItem("/#system", "&#128223; System mode");
    s += navHrefItem("/#mmdvm", "&#128246; MMDVM");
    s += navHrefItem("/#quick-actions", "&#9889; Quick actions");
    s += navHrefItem("/#network", "&#128655; Network");
    s += navHrefItem("/#wifi", "&#128225; Wifi");
    s += navHrefItem("/#display", "&#128250; Display");
    s += navHrefItem("/#weather", "&#9925; Weather");
    s += navHrefItem("/#hamqsl", "&#9728; HAMQSL");
    s += navHrefItem("/#lookup", "&#128218; Lookup");
    s += navHrefItem("/#theme", "&#127912; Theme");
  }
  s += "<div class='side-title' style='margin-top:18px'>Application</div>";
  s += navHrefItem("/app/svxlink", "&#128199; SVXLink", activeApp == "svxlink");
  s += navHrefItem("/app/aprs", "&#128205; APRS", activeApp == "aprs");
  s += navHrefItem("/app/hamqsl", "&#9728; HAMQSL", activeApp == "hamqsl");
  s += navHrefItem("/app/target-lists", "&#127915; Target lists", activeApp == "target-lists");
  s += navHrefItem("/app/ota", "&#128187; OTA firmware", activeApp == "ota");
  s += navHrefItem("/app/diagnostics", "&#128200; Diagnostics", activeApp == "diagnostics");
  s += navHrefItem("/app/logs", "&#128211; Device log", activeApp == "logs");
  s += "<div class='side-title' style='margin-top:18px'>Help</div>";
  s += navHrefItem("/app/faq", "&#10067; FAQ", activeApp == "faq");
  s += navHrefItem("/app/changelog", "&#128221; Changelog", activeApp == "changelog");
  s += "</aside>";
  return s;
}

void noteApiFetchResult(bool ok, int statusCode, unsigned long latencyMs) {
  diagnosticsState.lastApiLatencyMs = latencyMs;
  diagnosticsState.lastApiStatusCode = statusCode;
  diagnosticsState.lastFetchMillis = millis();
  if (ok) {
    diagnosticsState.fetchOk++;
    diagnosticsState.consecutiveFetchFailures = 0;
    diagnosticsState.lastFetchSuccessMillis = diagnosticsState.lastFetchMillis;
  } else {
    diagnosticsState.fetchFail++;
    diagnosticsState.consecutiveFetchFailures++;
  }

  bool shouldLog = false;
  if (!webLogApiStateKnown) shouldLog = true;
  else if (ok != webLogLastApiOk || statusCode != webLogLastApiCode) shouldLog = true;
  else if (!ok && (diagnosticsState.consecutiveFetchFailures <= 3 || (diagnosticsState.consecutiveFetchFailures % 10UL) == 0)) shouldLog = true;

  if (shouldLog) {
    if (ok) addWebLog("INFO", "API", "Hotspot API OK (HTTP " + String(statusCode) + ", " + String(latencyMs) + " ms).");
    else addWebLog("WARN", "API", "Hotspot API failed (HTTP " + String(statusCode) + ", " + String(latencyMs) + " ms, streak " + String(diagnosticsState.consecutiveFetchFailures) + ").");
  }

  webLogApiStateKnown = true;
  webLogLastApiOk = ok;
  webLogLastApiCode = statusCode;
}

void noteWeatherFetchResult(bool ok, int statusCode) {
  diagnosticsState.lastWeatherAttemptMillis = millis();
  diagnosticsState.lastWeatherHttpCode = statusCode;
  if (ok) {
    diagnosticsState.weatherOk++;
    diagnosticsState.lastWeatherSuccessMillis = diagnosticsState.lastWeatherAttemptMillis;
  } else {
    diagnosticsState.weatherFail++;
  }

  bool shouldLog = false;
  if (!webLogWeatherStateKnown) shouldLog = true;
  else if (ok != webLogLastWeatherOk || statusCode != webLogLastWeatherCode) shouldLog = true;
  else if (!ok && diagnosticsState.weatherFail <= 3) shouldLog = true;

  if (shouldLog) {
    if (ok) addWebLog("INFO", "WEATHER", "Weather refresh OK (HTTP " + String(statusCode) + ").");
    else addWebLog("WARN", "WEATHER", "Weather refresh failed (HTTP " + String(statusCode) + ").");
  }

  webLogWeatherStateKnown = true;
  webLogLastWeatherOk = ok;
  webLogLastWeatherCode = statusCode;
}

void noteHamqslFetchResult(bool ok, int statusCode) {
  if (!webLogHamqslStateKnown || ok != webLogLastHamqslOk || statusCode != webLogLastHamqslCode) {
    if (ok) addWebLog("INFO", "HAMQSL", "HAMQSL solar XML fetch OK (HTTP " + String(statusCode) + ").");
    else addWebLog("WARN", "HAMQSL", "HAMQSL solar XML fetch failed (HTTP " + String(statusCode) + ").");
  }
  webLogHamqslStateKnown = true;
  webLogLastHamqslOk = ok;
  webLogLastHamqslCode = statusCode;
}

String buildDiagnosticsJson() {
  String rx = trimSafe(hotspotRxFreq.length() ? hotspotRxFreq : currentRxFreq);
  String tx = trimSafe(hotspotTxFreq.length() ? hotspotTxFreq : currentTxFreq);
  String hotspot = trimSafe(hotspotStatusText);
  if (!hotspot.length()) hotspot = wifiOk ? "Connected" : "Offline";

  String json;
  json.reserve(4096);
  json += "{";
  json += "\"uptime\":\"" + jsonEscape(formatUptimeBrief()) + "\",";
  json += "\"wifi_text\":\"" + jsonEscape(getWifiSignalText()) + "\",";
  json += "\"wifi_ok\":" + String(wifiOk ? "true" : "false") + ",";
  json += "\"heap_free\":" + String((uint32_t)ESP.getFreeHeap()) + ",";
  json += "\"api_latency_ms\":" + String(diagnosticsState.lastApiLatencyMs) + ",";
  json += "\"api_status_code\":" + String(diagnosticsState.lastApiStatusCode) + ",";
  json += "\"fetch_ok\":" + String(diagnosticsState.fetchOk) + ",";
  json += "\"fetch_fail\":" + String(diagnosticsState.fetchFail) + ",";
  json += "\"fetch_streak\":" + String(diagnosticsState.consecutiveFetchFailures) + ",";
  json += "\"last_fetch\":\"" + jsonEscape(formatMillisAgoBrief(diagnosticsState.lastFetchMillis)) + "\",";
  json += "\"last_fetch_success\":\"" + jsonEscape(formatMillisAgoBrief(diagnosticsState.lastFetchSuccessMillis)) + "\",";
  json += "\"weather_status_code\":" + String(diagnosticsState.lastWeatherHttpCode) + ",";
  json += "\"weather_ok\":" + String(diagnosticsState.weatherOk) + ",";
  json += "\"weather_fail\":" + String(diagnosticsState.weatherFail) + ",";
  json += "\"weather_last\":\"" + jsonEscape(formatMillisAgoBrief(weatherState.lastAttemptMillis)) + "\",";
  json += "\"weather_last_success\":\"" + jsonEscape(formatMillisAgoBrief(weatherState.lastSuccessMillis)) + "\",";
  json += "\"hotspot_status\":\"" + jsonEscape(hotspot) + "\",";
  json += "\"rx_freq\":\"" + jsonEscape(rx.length() ? rx : String("-")) + "\",";
  json += "\"tx_freq\":\"" + jsonEscape(tx.length() ? tx : String("-")) + "\",";
  json += "\"total_qsos\":" + String(sessionInfo.totalQsos) + ",";
  json += "\"best_rssi\":" + String(sessionInfo.hasRssi ? sessionInfo.bestRssi : DIAG_NULL_INT) + ",";
  json += "\"worst_rssi\":" + String(sessionInfo.hasRssi ? sessionInfo.worstRssi : DIAG_NULL_INT) + ",";
  json += "\"live_callsign\":\"" + jsonEscape(trimSafe(liveRecord.callsign)) + "\",";
  json += "\"live_mode\":\"" + jsonEscape(trimSafe(liveRecord.mode)) + "\",";
  json += "\"mode_counts\":{";
  json += "\"dmr\":" + String(sessionInfo.modeDMR) + ",";
  json += "\"dstar\":" + String(sessionInfo.modeDSTAR) + ",";
  json += "\"ysf\":" + String(sessionInfo.modeYSF) + ",";
  json += "\"p25\":" + String(sessionInfo.modeP25) + ",";
  json += "\"nxdn\":" + String(sessionInfo.modeNXDN) + ",";
  json += "\"pocsag\":" + String(sessionInfo.modePOCSAG);
  json += "}";
  json += "}";
  return json;
}

void handleDiagnosticsApi() {
  server.sendHeader("Cache-Control", "no-store, no-cache, must-revalidate, max-age=0");
  server.send(200, "application/json", buildDiagnosticsJson());
}

void handleDiagnosticsPage() {
  String accentHex = cfg.themeColor.length() ? cfg.themeColor : String("#35C2FF");
  String ip = wifiOk ? WiFi.localIP().toString() : (apModeActive ? WiFi.softAPIP().toString() : "not connected");
  String backend = canonicalBackendValue(cfg.backend);
  if (!backend.length()) backend = "Pi-Star";

  String page;
  page.reserve(17000);
  page += "<!doctype html><html><head><meta charset='utf-8'>";
  page += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
  page += "<title>MMDVM Display</title>";
  page += "<style>";
  page += ":root{--bg:#0b1220;--panel:rgba(15,23,42,.82);--line:rgba(148,163,184,.20);--line2:rgba(148,163,184,.12);--text:#e5eefc;--muted:#94a3b8;--side:rgba(7,13,24,.76);--accent:" + accentHex + ";--accent-soft:rgba(53,194,255,.16);--shadow:0 18px 45px rgba(2,6,23,.34)}";
  page += R"DIAGCSS(*{box-sizing:border-box}html{scroll-behavior:smooth}body{margin:0;font-family:Inter,Segoe UI,Arial,Helvetica,sans-serif;background:#0b1220;color:var(--text)}body:before{content:'';position:fixed;inset:0;background:linear-gradient(135deg,rgba(255,255,255,.03),transparent 45%,rgba(255,255,255,.015));pointer-events:none}.topbar{position:sticky;top:0;z-index:40;backdrop-filter:blur(14px);background:rgba(11,18,32,.72);border-bottom:1px solid var(--line);padding:18px 24px;display:flex;justify-content:space-between;align-items:center;gap:16px;flex-wrap:wrap}.brand{font-size:28px;font-weight:700;letter-spacing:-.03em;color:#fff}.brand-sub{font-size:13px;color:var(--muted);margin-top:4px}.topbar-right{display:flex;align-items:center;gap:12px;flex-wrap:wrap;justify-content:flex-end}.header-actions{display:flex;gap:10px;flex-wrap:wrap;justify-content:flex-end}.btn{display:inline-flex;align-items:center;justify-content:center;padding:13px 20px;border-radius:14px;border:1px solid transparent;background:linear-gradient(135deg,var(--accent),rgba(255,255,255,.18));color:#08111d;font-weight:800;text-decoration:none;cursor:pointer;box-shadow:0 14px 30px rgba(53,194,255,.20)}.btn-secondary{background:rgba(255,255,255,.03);color:#fff;border-color:var(--line);box-shadow:none}.btn-compact{padding:10px 14px;font-size:13px}.layout{display:flex;min-height:calc(100vh - 78px)}.sidebar{width:248px;background:var(--side);border-right:1px solid var(--line);padding:24px 16px;position:sticky;top:78px;height:calc(100vh - 78px);overflow:auto;backdrop-filter:blur(16px)}.side-title{font-size:11px;text-transform:uppercase;letter-spacing:.16em;color:var(--muted);margin:0 0 12px 10px;font-weight:800}.side-link{display:block;text-decoration:none;color:#cbd5e1;padding:12px 14px;border-radius:14px;margin-bottom:8px;border:1px solid transparent;font-size:14px;font-weight:700;transition:all .18s ease}.side-link:hover{background:rgba(255,255,255,.04);border-color:var(--line);transform:translateX(2px)}.side-link.active{background:linear-gradient(135deg,var(--accent-soft),rgba(255,255,255,.02));border-color:rgba(53,194,255,.28);color:#fff;box-shadow:inset 0 0 0 1px rgba(255,255,255,.04)}.content{flex:1;min-width:0;padding:28px}.page-head{display:flex;justify-content:space-between;align-items:flex-start;gap:18px;margin-bottom:20px}.page-head h1{margin:0;font-size:34px;font-weight:800;letter-spacing:-.04em;color:#fff}.page-sub{margin-top:8px;color:var(--muted);font-size:15px;max-width:720px}.status-box,.section{background:var(--panel);border:1px solid var(--line);border-radius:22px;box-shadow:var(--shadow);backdrop-filter:blur(18px)}.status-box{padding:14px 16px;min-width:260px}.status-row{display:flex;justify-content:space-between;gap:12px;padding:6px 0;font-size:13px}.status-key{color:var(--muted);font-weight:700}.status-val{font-weight:700;color:#fff;text-align:right}.section{margin-bottom:18px;overflow:hidden}.section-head{padding:16px 20px;background:linear-gradient(180deg,rgba(255,255,255,.035),rgba(255,255,255,.01));border-bottom:1px solid var(--line2)}.section-head h2{margin:0;font-size:19px;font-weight:800;color:#fff}.section-head p{margin:6px 0 0 0;font-size:13px;color:var(--muted)}.section-body{padding:20px}.stats-grid{display:grid;grid-template-columns:repeat(4,minmax(0,1fr));gap:14px}.stat-card{padding:16px;border-radius:18px;background:rgba(255,255,255,.03);border:1px solid var(--line2)}.stat-label{font-size:12px;text-transform:uppercase;letter-spacing:.12em;color:var(--muted);font-weight:800;margin-bottom:10px}.stat-value{font-size:24px;font-weight:800;color:#fff;line-height:1.15}.stat-sub{margin-top:8px;font-size:13px;color:#cbd5e1}.mini-note{color:var(--muted);font-size:13px;line-height:1.65}.muted{color:var(--muted)}@media(max-width:1080px){.layout{display:block}.sidebar{width:auto;height:auto;position:relative;top:0;border-right:none;border-bottom:1px solid var(--line)}.content{padding:18px}.page-head{flex-direction:column}.stats-grid{grid-template-columns:1fr}.topbar{position:relative}.topbar-right{width:100%;justify-content:flex-start}.header-actions{justify-content:flex-start}})DIAGCSS";
  page += buildSidebarMobileCss();
  page += "</head><body>";

  page += "<div class='topbar'>" + buildTopbarBrandHtml();
  page += "<div class='topbar-right'>";
  page += "<div class='header-actions'>";
  page += "<a class='btn btn-secondary btn-compact' href='/export'>&#128228; Export config</a>";
  page += "<form class='inline-form' method='POST' action='/import' enctype='multipart/form-data'><input type='file' id='import_config_file' name='config_file' accept='.json,application/json' style='display:none' onchange='this.form.submit()'><label class='btn btn-secondary btn-compact' for='import_config_file'>&#128229; Import config</label></form>";
  page += "</div></div></div>";

  page += "<div class='layout'>";
  page += buildSidebarHtml(false, "diagnostics");
  page += "<main class='content'>";
  page += "<div class='page-head'><div><h1>&#128200; Diagnostics</h1><div class='page-sub'>Live runtime counters and transport health for the device and hotspot link.</div></div>";
  page += "<div class='status-box'>";
  page += "<div class='status-row'><span class='status-key'>Device IP</span><span class='status-val'>" + htmlEscape(ip) + "</span></div>";
  page += "<div class='status-row'><span class='status-key'>Backend</span><span class='status-val'>" + htmlEscape(backend) + "</span></div>";
  page += "<div class='status-row'><span class='status-key'>Hotspot</span><span class='status-val'>" + htmlEscape(cfg.pistarHost) + "</span></div>";
  page += "<div class='status-row'><span class='status-key'>Build</span><span class='status-val'>" + htmlEscape(getBuildStamp()) + "</span></div>";
  page += "</div></div>";
  page += R"DIAGHTML(<section class='section'><div class='section-head'><h2>Overview</h2><p>These cards refresh automatically while the page is open.</p></div><div class='section-body'><div id='diag-error' class='mini-note muted' style='margin-bottom:14px'></div><div class='stats-grid'><div class='stat-card'><div class='stat-label'>Uptime</div><div class='stat-value' id='metric-uptime'>-</div><div class='stat-sub' id='metric-fetch-last'>Last fetch: -</div></div><div class='stat-card'><div class='stat-label'>Wi-Fi</div><div class='stat-value' id='metric-wifi'>-</div><div class='stat-sub' id='metric-hotspot'>Hotspot: -</div></div><div class='stat-card'><div class='stat-label'>Free heap</div><div class='stat-value' id='metric-heap'>-</div><div class='stat-sub' id='metric-api'>API latency: -</div></div><div class='stat-card'><div class='stat-label'>Traffic</div><div class='stat-value' id='metric-qsos'>-</div><div class='stat-sub' id='metric-live'>Live: -</div></div><div class='stat-card'><div class='stat-label'>Fetch results</div><div class='stat-value' id='metric-fetch-ok'>-</div><div class='stat-sub' id='metric-fetch-fail'>Failures: -</div></div><div class='stat-card'><div class='stat-label'>Weather</div><div class='stat-value' id='metric-weather'>-</div><div class='stat-sub' id='metric-weather-last'>Last refresh: -</div></div><div class='stat-card'><div class='stat-label'>Frequencies</div><div class='stat-value' id='metric-rx'>-</div><div class='stat-sub' id='metric-tx'>TX: -</div></div><div class='stat-card'><div class='stat-label'>Mode totals</div><div class='stat-value' id='metric-modes'>-</div><div class='stat-sub' id='metric-rssi-range'>RSSI range: -</div></div></div></div></section>)DIAGHTML";
  page += R"DIAGSCRIPT(<script>
function setText(id, value){ const el=document.getElementById(id); if(el) el.textContent=value; }
function formatBytes(v){ if(v === null || v === undefined) return '-'; if(v >= 1048576) return (v/1048576).toFixed(2) + ' MB'; return (v/1024).toFixed(0) + ' KB'; }
async function loadDiagnostics(){ try { const res = await fetch('/api/diagnostics', { cache:'no-store' }); if(!res.ok) throw new Error('HTTP ' + res.status); const d = await res.json(); setText('diag-error', ''); setText('metric-uptime', d.uptime); setText('metric-fetch-last', 'Last fetch: ' + d.last_fetch + ' / OK: ' + d.last_fetch_success); setText('metric-wifi', d.wifi_text); setText('metric-hotspot', 'Hotspot: ' + d.hotspot_status); setText('metric-heap', formatBytes(d.heap_free)); setText('metric-api', 'API latency: ' + d.api_latency_ms + ' ms / HTTP ' + d.api_status_code); setText('metric-qsos', d.total_qsos + ' total'); setText('metric-live', 'Live: ' + ((d.live_callsign || '-') + (d.live_mode ? ' / ' + d.live_mode : ''))); setText('metric-fetch-ok', d.fetch_ok + ' ok'); setText('metric-fetch-fail', 'Failures: ' + d.fetch_fail + ' / streak ' + d.fetch_streak); setText('metric-weather', 'HTTP ' + d.weather_status_code); setText('metric-weather-last', 'Last refresh: ' + d.weather_last + ' / OK: ' + d.weather_last_success); setText('metric-rx', 'RX ' + d.rx_freq); setText('metric-tx', 'TX: ' + d.tx_freq); setText('metric-modes', 'DMR ' + d.mode_counts.dmr + ' / DSTAR ' + d.mode_counts.dstar + ' / YSF ' + d.mode_counts.ysf); const best = d.best_rssi === -32768 ? '-' : d.best_rssi + ' dBm'; const worst = d.worst_rssi === -32768 ? '-' : d.worst_rssi + ' dBm'; setText('metric-rssi-range', 'RSSI range: ' + best + ' / ' + worst); } catch (err) { setText('diag-error', 'Unable to load live diagnostics right now. ' + err.message); } }
loadDiagnostics(); setInterval(loadDiagnostics, 4000);
</script>)DIAGSCRIPT";
  page += "</main></div>";
  page += buildSidebarMobileScript();
  page += "</body></html>";
  server.send(200, "text/html", page);
}

void handleLogsApi() {
  server.sendHeader("Cache-Control", "no-store, no-cache, must-revalidate, max-age=0");
  server.send(200, "application/json", buildWebLogsJson());
}

void handleLogsClear() {
  webLogHead = 0;
  webLogCount = 0;
  addWebLog("INFO", "WEB", "Device log cleared from Web UI.");
  server.sendHeader("Cache-Control", "no-store, no-cache, must-revalidate, max-age=0");
  server.send(200, "application/json", buildWebLogsJson());
}

void handleLogsPage() {
  String accentHex = cfg.themeColor.length() ? cfg.themeColor : String("#35C2FF");
  String ip = wifiOk ? WiFi.localIP().toString() : (apModeActive ? WiFi.softAPIP().toString() : "not connected");
  String backend = canonicalBackendValue(cfg.backend);
  if (!backend.length()) backend = "Pi-Star";

  String page;
  page.reserve(20000);
  page += "<!doctype html><html><head><meta charset='utf-8'>";
  page += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
  page += "<title>MMDVM Display</title>";
  page += "<style>";
  page += ":root{--bg:#0b1220;--panel:rgba(15,23,42,.82);--line:rgba(148,163,184,.20);--line2:rgba(148,163,184,.12);--text:#e5eefc;--muted:#94a3b8;--side:rgba(7,13,24,.76);--accent:" + accentHex + ";--accent-soft:rgba(53,194,255,.16);--shadow:0 18px 45px rgba(2,6,23,.34);--warn:#fbbf24;--bad:#f87171;--good:#34d399}";
  page += R"LOGCSS(*{box-sizing:border-box}html{scroll-behavior:smooth}body{margin:0;font-family:Inter,Segoe UI,Arial,Helvetica,sans-serif;background:#0b1220;color:var(--text)}body:before{content:'';position:fixed;inset:0;background:linear-gradient(135deg,rgba(255,255,255,.03),transparent 45%,rgba(255,255,255,.015));pointer-events:none}.topbar{position:sticky;top:0;z-index:40;backdrop-filter:blur(14px);background:rgba(11,18,32,.72);border-bottom:1px solid var(--line);padding:18px 24px;display:flex;justify-content:space-between;align-items:center;gap:16px;flex-wrap:wrap}.brand{font-size:28px;font-weight:700;letter-spacing:-.03em;color:#fff}.brand-sub{font-size:13px;color:var(--muted);margin-top:4px}.topbar-right{display:flex;align-items:center;gap:12px;flex-wrap:wrap;justify-content:flex-end}.header-actions{display:flex;gap:10px;flex-wrap:wrap;justify-content:flex-end}.btn{display:inline-flex;align-items:center;justify-content:center;padding:13px 20px;border-radius:14px;border:1px solid transparent;background:linear-gradient(135deg,var(--accent),rgba(255,255,255,.18));color:#08111d;font-weight:800;text-decoration:none;cursor:pointer;box-shadow:0 14px 30px rgba(53,194,255,.20)}.btn-secondary{background:rgba(255,255,255,.03);color:#fff;border-color:var(--line);box-shadow:none}.btn-compact{padding:10px 14px;font-size:13px}.layout{display:flex;min-height:calc(100vh - 78px)}.sidebar{width:248px;background:var(--side);border-right:1px solid var(--line);padding:24px 16px;position:sticky;top:78px;height:calc(100vh - 78px);overflow:auto;backdrop-filter:blur(16px)}.side-title{font-size:11px;text-transform:uppercase;letter-spacing:.16em;color:var(--muted);margin:0 0 12px 10px;font-weight:800}.side-link{display:block;text-decoration:none;color:#cbd5e1;padding:12px 14px;border-radius:14px;margin-bottom:8px;border:1px solid transparent;font-size:14px;font-weight:700;transition:all .18s ease}.side-link:hover{background:rgba(255,255,255,.04);border-color:var(--line);transform:translateX(2px)}.side-link.active{background:linear-gradient(135deg,var(--accent-soft),rgba(255,255,255,.02));border-color:rgba(53,194,255,.28);color:#fff;box-shadow:inset 0 0 0 1px rgba(255,255,255,.04)}.content{flex:1;min-width:0;padding:28px}.page-head{display:flex;justify-content:space-between;align-items:flex-start;gap:18px;margin-bottom:20px}.page-head h1{margin:0;font-size:34px;font-weight:800;letter-spacing:-.04em;color:#fff}.page-sub{margin-top:8px;color:var(--muted);font-size:15px;max-width:760px}.status-box,.section{background:var(--panel);border:1px solid var(--line);border-radius:22px;box-shadow:var(--shadow);backdrop-filter:blur(18px)}.status-box{padding:14px 16px;min-width:260px}.status-row{display:flex;justify-content:space-between;gap:12px;padding:6px 0;font-size:13px}.status-key{color:var(--muted);font-weight:700}.status-val{font-weight:700;color:#fff;text-align:right}.section{margin-bottom:18px;overflow:hidden}.section-head{padding:16px 20px;background:linear-gradient(180deg,rgba(255,255,255,.035),rgba(255,255,255,.01));border-bottom:1px solid var(--line2)}.section-head h2{margin:0;font-size:19px;font-weight:800;color:#fff}.section-head p{margin:6px 0 0 0;font-size:13px;color:var(--muted)}.section-body{padding:20px}.toolbar{display:flex;gap:10px;flex-wrap:wrap;align-items:center;justify-content:space-between;margin-bottom:14px}.toolbar-left,.toolbar-right{display:flex;gap:10px;flex-wrap:wrap;align-items:center}.pill{display:inline-flex;align-items:center;gap:8px;padding:10px 14px;border-radius:999px;background:rgba(255,255,255,.04);border:1px solid var(--line);font-size:12px;font-weight:800;color:#cbd5e1}.console{background:#050b16;border:1px solid rgba(148,163,184,.18);border-radius:18px;padding:0;overflow:hidden}.console-head{display:grid;grid-template-columns:120px 88px 92px minmax(0,1fr);gap:12px;padding:12px 16px;border-bottom:1px solid var(--line2);background:rgba(255,255,255,.02)}.console-title{font-size:12px;text-transform:uppercase;letter-spacing:.14em;color:var(--muted);font-weight:800}.console-body{max-height:68vh;overflow:auto;font-family:ui-monospace,SFMono-Regular,Menlo,Consolas,monospace}.log-row{display:grid;grid-template-columns:120px 88px 92px minmax(0,1fr);gap:12px;padding:10px 16px;border-bottom:1px solid rgba(148,163,184,.08);font-size:13px;align-items:start}.log-row:last-child{border-bottom:none}.log-age{color:#93c5fd}.log-level{font-weight:800}.log-level.INFO{color:var(--good)}.log-level.WARN{color:var(--warn)}.log-level.ERROR{color:var(--bad)}.log-tag{color:#c4b5fd}.log-msg{color:#e2e8f0;word-break:break-word}.empty-note{padding:18px;color:var(--muted)}.switch{display:inline-flex;align-items:center;gap:8px;color:#cbd5e1;font-size:13px;font-weight:700}.muted{color:var(--muted)}@media(max-width:1080px){.layout{display:block}.sidebar{width:auto;height:auto;position:relative;top:0;border-right:none;border-bottom:1px solid var(--line)}.content{padding:18px}.page-head{flex-direction:column}.topbar{position:relative}.topbar-right{width:100%;justify-content:flex-start}.header-actions{justify-content:flex-start}.console-head,.log-row{grid-template-columns:82px 66px 72px minmax(0,1fr);font-size:12px;padding:9px 12px}})LOGCSS";
  page += buildSidebarMobileCss();
  page += "</head><body>";

  page += "<div class='topbar'>" + buildTopbarBrandHtml();
  page += "<div class='topbar-right'>";
  page += "<div class='header-actions'>";
  page += "<a class='btn btn-secondary btn-compact' href='/export'>&#128228; Export config</a>";
  page += "<form class='inline-form' method='POST' action='/import' enctype='multipart/form-data'><input type='file' id='import_config_file' name='config_file' accept='.json,application/json' style='display:none' onchange='this.form.submit()'><label class='btn btn-secondary btn-compact' for='import_config_file'>&#128229; Import config</label></form>";
  page += "</div></div></div>";

  page += "<div class='layout'>";
  page += buildSidebarHtml(false, "logs");
  page += "<main class='content'>";
  page += "<div class='page-head'><div><h1>&#128211; Device log</h1><div class='page-sub'>Live runtime messages from the ESP32 so you can see Wi-Fi retries, API failures, OTA events and other background activity without a serial cable.</div></div>";
  page += "<div class='status-box'>";
  page += "<div class='status-row'><span class='status-key'>Device IP</span><span class='status-val'>" + htmlEscape(ip) + "</span></div>";
  page += "<div class='status-row'><span class='status-key'>Backend</span><span class='status-val'>" + htmlEscape(backend) + "</span></div>";
  page += "<div class='status-row'><span class='status-key'>Buffered lines</span><span class='status-val' id='log-count'>-</span></div>";
  page += "<div class='status-row'><span class='status-key'>Capacity</span><span class='status-val'>" + String(WEB_LOG_CAPACITY) + "</span></div>";
  page += "</div></div>";
  page += R"LOGHTML(<section class='section'><div class='section-head'><h2>Runtime console</h2><p>The newest entries appear at the bottom. Auto-refresh is enabled by default.</p></div><div class='section-body'><div class='toolbar'><div class='toolbar-left'><div class='pill' id='log-meta'>Loading...</div></div><div class='toolbar-right'><label class='switch'><input type='checkbox' id='autoscroll' checked> Auto-scroll</label><button class='btn btn-secondary btn-compact' id='refresh-btn' type='button'>Refresh</button><button class='btn btn-secondary btn-compact' id='clear-btn' type='button'>Clear log</button></div></div><div class='console'><div class='console-head'><div class='console-title'>Age</div><div class='console-title'>Level</div><div class='console-title'>Tag</div><div class='console-title'>Message</div></div><div class='console-body' id='log-body'><div class='empty-note'>Loading device log...</div></div></div><div class='muted' style='margin-top:12px'>Tip: this is an in-memory ring buffer, so it survives page refreshes but not a reboot.</div></div></section>)LOGHTML";
  page += R"LOGSCRIPT(<script>
const logBody = document.getElementById('log-body');
const logMeta = document.getElementById('log-meta');
const logCount = document.getElementById('log-count');
const autoScroll = document.getElementById('autoscroll');

function esc(v){ return String(v ?? '').replace(/&/g,'&amp;').replace(/</g,'&lt;').replace(/>/g,'&gt;'); }

function renderLogs(data){
  const items = Array.isArray(data.items) ? data.items : [];
  logCount.textContent = String(data.count ?? items.length ?? 0);
  logMeta.textContent = String(items.length) + ' entries buffered';
  if(!items.length){
    logBody.innerHTML = "<div class='empty-note'>No device log entries yet.</div>";
    return;
  }
  logBody.innerHTML = items.map(item => {
    const level = esc(item.level || 'INFO');
    const tag = esc(item.tag || '-');
    const age = esc(item.age || '-');
    const msg = esc(item.message || '');
    return "<div class='log-row'><div class='log-age'>" + age + "</div><div class='log-level " + level + "'>" + level + "</div><div class='log-tag'>" + tag + "</div><div class='log-msg'>" + msg + "</div></div>";
  }).join('');
  if(autoScroll.checked) logBody.scrollTop = logBody.scrollHeight;
}

async function loadLogs(){
  try{
    const res = await fetch('/api/logs', { cache:'no-store' });
    if(!res.ok) throw new Error('HTTP ' + res.status);
    renderLogs(await res.json());
  }catch(err){
    logBody.innerHTML = "<div class='empty-note'>Unable to load device log right now. " + esc(err.message) + "</div>";
  }
}

async function clearLogs(){
  try{
    const res = await fetch('/api/logs/clear', { method:'POST', cache:'no-store' });
    if(!res.ok) throw new Error('HTTP ' + res.status);
    renderLogs(await res.json());
  }catch(err){
    alert('Unable to clear log: ' + err.message);
  }
}

document.getElementById('refresh-btn').addEventListener('click', loadLogs);
document.getElementById('clear-btn').addEventListener('click', clearLogs);
loadLogs();
setInterval(loadLogs, 2000);
</script>)LOGSCRIPT";
  page += "</main></div>";
  page += buildSidebarMobileScript();
  page += "</body></html>";
  server.send(200, "text/html", page);
}

void handleOtaPage() {
  String accentHex = cfg.themeColor.length() ? cfg.themeColor : String("#35C2FF");
  String ip = wifiOk ? WiFi.localIP().toString() : (apModeActive ? WiFi.softAPIP().toString() : "not connected");
  String backend = canonicalBackendValue(cfg.backend);
  if (!backend.length()) backend = "Pi-Star";

  String page;
  page.reserve(16000);
  page += "<!doctype html><html><head><meta charset='utf-8'>";
  page += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
  page += "<title>MMDVM Display</title>";
  page += "<style>";
  page += ":root{--bg:#0b1220;--panel:rgba(15,23,42,.82);--line:rgba(148,163,184,.20);--line2:rgba(148,163,184,.12);--text:#e5eefc;--muted:#94a3b8;--side:rgba(7,13,24,.76);--accent:" + accentHex + ";--accent-soft:rgba(53,194,255,.16);--shadow:0 18px 45px rgba(2,6,23,.34);--warn:#fdba74}";
  page += R"OTACSS(*{box-sizing:border-box}body{margin:0;font-family:Inter,Segoe UI,Arial,Helvetica,sans-serif;background:#0b1220;color:var(--text)}body:before{content:'';position:fixed;inset:0;background:linear-gradient(135deg,rgba(255,255,255,.03),transparent 45%,rgba(255,255,255,.015));pointer-events:none}.topbar{position:sticky;top:0;z-index:40;backdrop-filter:blur(14px);background:rgba(11,18,32,.72);border-bottom:1px solid var(--line);padding:18px 24px;display:flex;justify-content:space-between;align-items:center;gap:16px;flex-wrap:wrap}.brand{font-size:28px;font-weight:700;letter-spacing:-.03em;color:#fff}.brand-sub{font-size:13px;color:var(--muted);margin-top:4px}.topbar-right{display:flex;align-items:center;gap:12px;flex-wrap:wrap;justify-content:flex-end}.header-actions{display:flex;gap:10px;flex-wrap:wrap;justify-content:flex-end}.btn{display:inline-flex;align-items:center;justify-content:center;padding:13px 20px;border-radius:14px;border:1px solid transparent;background:linear-gradient(135deg,var(--accent),rgba(255,255,255,.18));color:#08111d;font-weight:800;text-decoration:none;cursor:pointer;box-shadow:0 14px 30px rgba(53,194,255,.20)}.btn-secondary{background:rgba(255,255,255,.03);color:#fff;border-color:var(--line);box-shadow:none}.btn-compact{padding:10px 14px;font-size:13px}.layout{display:flex;min-height:calc(100vh - 78px)}.sidebar{width:248px;background:var(--side);border-right:1px solid var(--line);padding:24px 16px;position:sticky;top:78px;height:calc(100vh - 78px);overflow:auto;backdrop-filter:blur(16px)}.side-title{font-size:11px;text-transform:uppercase;letter-spacing:.16em;color:var(--muted);margin:0 0 12px 10px;font-weight:800}.side-link{display:block;text-decoration:none;color:#cbd5e1;padding:12px 14px;border-radius:14px;margin-bottom:8px;border:1px solid transparent;font-size:14px;font-weight:700}.side-link:hover{background:rgba(255,255,255,.04);border-color:var(--line)}.side-link.active{background:linear-gradient(135deg,var(--accent-soft),rgba(255,255,255,.02));border-color:rgba(53,194,255,.28);color:#fff}.content{flex:1;min-width:0;padding:28px}.page-head{display:flex;justify-content:space-between;align-items:flex-start;gap:18px;margin-bottom:20px}.page-head h1{margin:0;font-size:34px;font-weight:800;letter-spacing:-.04em;color:#fff}.page-sub{margin-top:8px;color:var(--muted);font-size:15px;max-width:720px}.status-box,.section{background:var(--panel);border:1px solid var(--line);border-radius:22px;box-shadow:var(--shadow);backdrop-filter:blur(18px)}.status-box{padding:14px 16px;min-width:260px}.status-row{display:flex;justify-content:space-between;gap:12px;padding:6px 0;font-size:13px}.status-key{color:var(--muted);font-weight:700}.status-val{font-weight:700;color:#fff;text-align:right}.section{margin-bottom:18px;overflow:hidden}.section-head{padding:16px 20px;background:linear-gradient(180deg,rgba(255,255,255,.035),rgba(255,255,255,.01));border-bottom:1px solid var(--line2)}.section-head h2{margin:0;font-size:19px;font-weight:800;color:#fff}.section-head p{margin:6px 0 0 0;font-size:13px;color:var(--muted)}.section-body{padding:20px}.upload-box{padding:18px;border-radius:18px;border:1px dashed rgba(148,163,184,.28);background:rgba(255,255,255,.02)}input[type='file']{width:100%;padding:13px 14px;border:1px solid rgba(148,163,184,.26);border-radius:14px;background:rgba(15,23,42,.80);color:#fff;font-size:14px}.alert-card{padding:16px 18px;border-radius:18px;background:rgba(253,186,116,.08);border:1px solid rgba(253,186,116,.20);color:#fde68a}.mini-note{color:var(--muted);font-size:13px;line-height:1.65}.bullet{margin:8px 0 0 18px;color:#cbd5e1}.bullet li{margin:6px 0}@media(max-width:1080px){.layout{display:block}.sidebar{width:auto;height:auto;position:relative;top:0;border-right:none;border-bottom:1px solid var(--line)}.content{padding:18px}.page-head{flex-direction:column}.topbar{position:relative}.topbar-right{width:100%;justify-content:flex-start}.header-actions{justify-content:flex-start}})OTACSS";
  page += buildSidebarMobileCss();
  page += "</head><body>";

  page += "<div class='topbar'>" + buildTopbarBrandHtml();
  page += "<div class='topbar-right'>";
  page += "<div class='header-actions'>";
  page += "<a class='btn btn-secondary btn-compact' href='/export'>&#128228; Export config</a>";
  page += "<form class='inline-form' method='POST' action='/import' enctype='multipart/form-data'><input type='file' id='import_config_file' name='config_file' accept='.json,application/json' style='display:none' onchange='this.form.submit()'><label class='btn btn-secondary btn-compact' for='import_config_file'>&#128229; Import config</label></form>";
  page += "</div></div></div>";

  page += "<div class='layout'>";
  page += buildSidebarHtml(false, "ota");
  page += "<main class='content'>";
  page += "<div class='page-head'><div><h1>&#128187; OTA firmware update</h1><div class='page-sub'>Upload a compiled ESP32 firmware image directly from the browser. Successful installs reboot the device automatically.</div></div>";
  page += "<div class='status-box'>";
  page += "<div class='status-row'><span class='status-key'>Device IP</span><span class='status-val'>" + htmlEscape(ip) + "</span></div>";
  page += "<div class='status-row'><span class='status-key'>Backend</span><span class='status-val'>" + htmlEscape(backend) + "</span></div>";
  page += "<div class='status-row'><span class='status-key'>Version</span><span class='status-val'>" + htmlEscape(String(DISPLAY_VERSION)) + "</span></div>";
  page += "<div class='status-row'><span class='status-key'>Build</span><span class='status-val'>" + htmlEscape(getBuildStamp()) + "</span></div>";
  page += "</div></div>";
  page += R"OTAHTML(<section class='section'><div class='section-head'><h2>Upload firmware</h2><p>Select the .bin file produced by your ESP32 build.</p></div><div class='section-body'><div class='alert-card'><strong>Important:</strong> stay on this page until the upload finishes. The display will reboot by itself after a successful flash, so the browser may briefly lose connection.</div><div class='upload-box' style='margin-top:16px'><form id='ota-form' method='POST' action='/app/ota' enctype='multipart/form-data'><input type='file' name='firmware' id='firmware' accept='.bin,application/octet-stream' required><div style='margin-top:14px;display:flex;gap:10px;flex-wrap:wrap'><button class='btn btn-secondary' type='submit' id='ota-submit'>&#128189; Start OTA update</button></div></form><div class='mini-note' id='ota-note' style='margin-top:12px'>Use only the application .bin compiled for this exact ESP32-S3 board and partition layout. Do not upload .ino, .zip, merged-flash .bin, bootloader .bin, partitions .bin or .bin.gz files.</div></div></div></section><section class='section'><div class='section-head'><h2>Recommended flow</h2><p>A couple of small safety checks before you flash.</p></div><div class='section-body'><ul class='bullet'><li>Export your current configuration first from the main configuration page.</li><li>Confirm the board target matches this device and display backend.</li><li>Keep the browser connected to the same network until the reboot completes.</li><li>If the update changes Wi-Fi settings, reconnect using the new address or the setup AP.</li></ul></div></section><script>document.getElementById('ota-form').addEventListener('submit', function(){ const btn = document.getElementById('ota-submit'); const note = document.getElementById('ota-note'); btn.disabled = true; btn.textContent = 'Uploading...'; note.textContent = 'Firmware upload in progress. Do not close this tab.'; });</script>)OTAHTML";
  page += "</main></div>";
  page += buildSidebarMobileScript();
  page += "</body></html>";
  server.send(200, "text/html", page);
}

void handleOtaUpload() {
  HTTPUpload& upload = server.upload();

  if (upload.status == UPLOAD_FILE_START) {
    otaUpdateSuccessful = false;
    otaRestartAt = 0;
    otaLastError = "";
    otaHeaderChecked = false;
    otaBytesWritten = 0;

    String fn = upload.filename;
    fn.toLowerCase();
    addWebLog("INFO", "OTA", "OTA upload started: " + upload.filename);

    if (!fn.endsWith(".bin")) {
      otaLastError = "Invalid OTA file. Upload only the application .bin file, not .ino, .zip, .bin.gz, bootloader, partitions, or merged-flash files.";
      addWebLog("ERROR", "OTA", otaLastError);
      return;
    }

    if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
      otaLastError = otaErrorText();
      addWebLog("ERROR", "OTA", "OTA begin failed: " + otaLastError);
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (!otaLastError.length()) {
      if (!otaHeaderChecked) {
        otaHeaderChecked = true;
        if (upload.currentSize < 1 || upload.buf[0] != 0xE9) {
          otaLastError = "Invalid ESP32 application image. The first byte is not 0xE9. Do not upload source, zip, gzip, bootloader, partition-table, or merged-flash images via OTA.";
          Update.abort();
          addWebLog("ERROR", "OTA", otaLastError);
          return;
        }
      }

      size_t written = Update.write(upload.buf, upload.currentSize);
      otaBytesWritten += written;
      if (written != upload.currentSize) {
        otaLastError = otaErrorText();
        addWebLog("ERROR", "OTA", "OTA write failed: " + otaLastError);
      }
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (!otaLastError.length()) {
      if (otaBytesWritten == 0) {
        otaLastError = "OTA upload ended with zero bytes written.";
        Update.abort();
        addWebLog("ERROR", "OTA", otaLastError);
      } else if (Update.end(true)) {
        otaUpdateSuccessful = true;
        otaRestartAt = millis() + 3500UL;
        addWebLog("INFO", "OTA", "OTA image written successfully (" + String((uint32_t)otaBytesWritten) + " bytes). Reboot scheduled.");
      } else {
        otaLastError = otaErrorText();
        addWebLog("ERROR", "OTA", "OTA finalize failed: " + otaLastError);
      }
    }
  } else if (upload.status == UPLOAD_FILE_ABORTED) {
    otaLastError = "Upload aborted.";
    Update.abort();
    addWebLog("WARN", "OTA", "OTA upload aborted.");
  }
}

void handleOtaUpdate() {
  server.sendHeader("Connection", "close");
  if (otaUpdateSuccessful) {
    sendWebMessagePage("Firmware updated", "The firmware image was written successfully.", "The device will reboot automatically in a few seconds. Reload the web UI after the reboot finishes.", false);
  } else {
    sendWebMessagePage("OTA update failed", otaLastError.length() ? otaLastError : String("The firmware upload could not be applied."), "Verify that the uploaded .bin matches this board and try again.", true);
  }
}


void handleFaqPage() {
  String page = pageTop("&#10067; FAQ", "faq", "Setup help for the MMDVM Display, touchscreen Actions button and optional SVXLink monitor.");

  page += R"FAQHTML(
  <div class='hero-card'>
    <div>
      <div class='hero-eyebrow'>Help and setup</div>
      <h3>Start here when setting up the display</h3>
      <p>Use it for first boot, Pi-Star / WPSD setup, Actions buttons and SVXLink.</p>
    </div>
    <div class='hero-pill'>FAQ / Setup Guide</div>
  </div>

  <section class='section' id='faq-first-setup'>
    <div class='section-head'><h2>&#128295; How do I set up the software for the first time?</h2><p>Follow this order after flashing the firmware to the ESP32 display.</p></div>
    <div class='section-body'>
      <div class='quick-grid'>
        <div class='quick-card'><h3>1. Power the display</h3><div class='mini-note'>Power the display by USB or your normal supply. If no valid configuration exists, the display starts its setup access point.</div></div>
        <div class='quick-card'><h3>2. Join setup Wi-Fi</h3><div class='mini-note'>Connect your phone or computer to the Wi-Fi network named <span class='mono'>MMDVM-Display-xxxxxx</span>. Open the address shown on the display.</div></div>
        <div class='quick-card'><h3>3. Save Wi-Fi and backend</h3><div class='mini-note'>Open <strong>Configuration</strong>, enter your home Wi-Fi, select <strong>Pi-Star</strong> or <strong>WPSD</strong>, and enter the dashboard host or IP.</div></div>
        <div class='quick-card'><h3>4. Reconnect normally</h3><div class='mini-note'>After saving, the device joins your Wi-Fi. Open the new IP shown on the screen to continue setup from the web UI.</div></div>
      </div>
      <div class='mini-note'>Tip: use <strong>Export config</strong> after everything works. It gives you a backup JSON that can be imported again after reflashing or testing new firmware.</div>
    </div>
  </section>

  <section class='section' id='faq-mmdvm'>
    <div class='section-head'><h2>&#128246; What do I need for Pi-Star or WPSD?</h2><p>The MMDVM settings drive the normal LIVE and HEARD screens.</p></div>
    <div class='section-body'>
      <div class='quick-grid'>
        <div class='quick-card'><h3>Backend</h3><div class='mini-note'>Set <strong>Active backend</strong> to the system you use: Pi-Star or WPSD.</div></div>
        <div class='quick-card'><h3>Host/IP</h3><div class='mini-note'>Set <strong>Pi-Star / WPSD Host or IP</strong> to the dashboard host, for example <span class='mono'>pi-star.local</span> or <span class='mono'>192.168.1.50</span>.</div></div>
        <div class='quick-card'><h3>SSH remote control</h3><div class='mini-note'>Enable SSH remote control only if you want touchscreen mode changes or Quick Actions. Enter the SSH user, password and port used by your hotspot.</div></div>
        <div class='quick-card'><h3>Clock and polling</h3><div class='mini-note'>Use <strong>Refresh interval</strong> for API polling and <strong>Time zone rule</strong> if you want UTC API times converted to local time.</div></div>
      </div>
    </div>
  </section>

  <section class='section' id='faq-actions'>
    <div class='section-head'><h2>&#9889; How do I set up and use the Actions button?</h2><p>Actions are saved SSH commands that you can run from the touchscreen or from the web UI.</p></div>
    <div class='section-body'>
      <div class='quick-grid'>
        <div class='quick-card'><h3>Configure SSH first</h3><div class='mini-note'>For MMDVM actions, go to <strong>Configuration &gt; MMDVM settings</strong>, enable SSH remote control, then enter the host SSH credentials.</div></div>
        <div class='quick-card'><h3>Create presets</h3><div class='mini-note'>Go to <strong>Configuration &gt; Quick actions</strong>. For each slot, enable it, choose a short button label and enter the exact shell command to run.</div></div>
        <div class='quick-card'><h3>Use sudo only when needed</h3><div class='mini-note'>Turn on <strong>Prefix command with sudo</strong> only for commands that need elevated permissions. The display will prepend <span class='mono'>sudo</span> before running the command.</div></div>
        <div class='quick-card'><h3>Run from the display</h3><div class='mini-note'>On the LIVE screen, tap <strong>ACTIONS</strong>. Tap a configured preset. Empty slots stay disabled and show as empty in the popup.</div></div>
      </div>
      <div class='mini-note'>Good presets are short and predictable: talkgroup changes, reflector links, disconnect commands, restart scripts or your own helper scripts. The display sends exactly what you type, so test commands carefully.</div>
    </div>
  </section>

  <section class='section' id='faq-svxlink'>
    <div class='section-head'><h2>&#128199; How do I set up SVXLink?</h2><p>SVXLink is optional and lives on its own page so it can be used beside Pi-Star or WPSD.</p></div>
    <div class='section-body'>
      <div class='quick-grid'>
        <div class='quick-card'><h3>Enable the source</h3><div class='mini-note'>Open <strong>Application &gt; SVXLink</strong> and turn on <strong>Enable SVXLink</strong>. LIVE becomes WATCH and lets you choose MMDVM or SVXLink.</div></div>
        <div class='quick-card'><h3>Pick URL or API</h3><div class='mini-note'><strong>URL / dashboard HTML</strong> reads the existing SVXLink dashboard. <strong>API / JSON bridge</strong> uses the display JSON endpoint.</div></div>
        <div class='quick-card'><h3>Set host and path</h3><div class='mini-note'>Enter the SVXLink host or full URL. For API mode, use the API path such as <span class='mono'>/api/display-json.php</span>. For dashboard mode, use the dashboard path such as <span class='mono'>/index.php</span>.</div></div>
        <div class='quick-card'><h3>Save and watch</h3><div class='mini-note'>Save the SVXLink page, then use the WATCH source selector on the display to switch between MMDVM and SVXLink live/heard views. The SVXLink HEARD screen supports the same page arrows for browsing older entries.</div></div>
      </div>
    </div>
  </section>

  <section class='section' id='faq-svxlink-actions'>
    <div class='section-head'><h2>&#128241; How do SVXLink actions / DTMF buttons work?</h2><p>SVXLink actions are separate from the normal MMDVM Quick Actions.</p></div>
    <div class='section-body'>
      <div class='quick-grid'>
        <div class='quick-card'><h3>Separate SSH login</h3><div class='mini-note'>In <strong>Application &gt; SVXLink</strong>, enable <strong>SVXLink SSH actions</strong> and enter the SVXLink SSH user, password and port.</div></div>
        <div class='quick-card'><h3>Separate action slots</h3><div class='mini-note'>Fill in the SVXLink action slots with labels and commands. These are shown only when the active WATCH source is SVXLink.</div></div>
        <div class='quick-card'><h3>DTMF or scripts</h3><div class='mini-note'>Use the command field for DTMF helper commands, module commands, disconnect commands or your own scripts on the SVXLink host.</div></div>
        <div class='quick-card'><h3>On-screen use</h3><div class='mini-note'>Switch WATCH to SVXLink, then tap <strong>ACTIONS</strong>. The popup title stays the same, but the buttons come from the SVXLink slots.</div></div>
      </div>
      <div class='mini-note'>Security note: SSH passwords are stored locally on the display. Use this only on trusted radios, trusted networks and hardware you control.</div>
    </div>
  </section>

  <section class='section' id='faq-troubleshooting'>
    <div class='section-head'><h2>&#128161; Troubleshooting quick checks</h2><p>Most setup problems are host, Wi-Fi or SSH related.</p></div>
    <div class='section-body'>
      <div class='quick-grid'>
        <div class='quick-card'><h3>No data on LIVE</h3><div class='mini-note'>Check that the backend is correct and the host/IP opens from your browser on the same network.</div></div>
        <div class='quick-card'><h3>Actions do nothing</h3><div class='mini-note'>Check that SSH is enabled, credentials are correct, the slot has both label and command, and the command works when run manually over SSH.</div></div>
        <div class='quick-card'><h3>SVXLink missing</h3><div class='mini-note'>Make sure SVXLink is enabled under <strong>Application &gt; SVXLink</strong>. If it is disabled, WATCH falls back to MMDVM.</div></div>
        <div class='quick-card'><h3>Need to recover</h3><div class='mini-note'>Use the setup access point if Wi-Fi is not configured, or import a known-good JSON backup from the top bar.</div></div>
      </div>
    </div>
  </section>
  )FAQHTML";

  page += pageBottom();
  server.send(200, "text/html", page);
}


void handleChangelogPage() {
  String accentHex = cfg.themeColor.length() ? cfg.themeColor : String("#35C2FF");
  String ip = wifiOk ? WiFi.localIP().toString() : (apModeActive ? WiFi.softAPIP().toString() : "not connected");
  String backend = cfg.backend;
  if (backend != "WPSD" && backend != "Pi-Star") backend = "Pi-Star";

  String page;
  page.reserve(20000);
  page += "<!doctype html><html><head><meta charset='utf-8'>";
  page += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
  page += "<title>MMDVM Display</title>";
  page += "<style>";
  page += ":root{--bg:#0b1220;--panel:rgba(15,23,42,.82);--line:rgba(148,163,184,.20);--line2:rgba(148,163,184,.12);--text:#e5eefc;--muted:#94a3b8;--side:rgba(7,13,24,.76);--accent:" + accentHex + ";--accent-soft:rgba(53,194,255,.16);--shadow:0 18px 45px rgba(2,6,23,.34);--good:#4ade80;--fix:#fda4af;--improve:#facc15;--feature:#60a5fa}";
  page += R"CHANGELOGCSS(*{box-sizing:border-box}body{margin:0;font-family:Inter,Segoe UI,Arial,Helvetica,sans-serif;background:#0b1220;color:var(--text)}body:before{content:'';position:fixed;inset:0;background:linear-gradient(135deg,rgba(255,255,255,.03),transparent 45%,rgba(255,255,255,.015));pointer-events:none}.topbar{position:sticky;top:0;z-index:40;backdrop-filter:blur(14px);background:rgba(11,18,32,.72);border-bottom:1px solid var(--line);padding:18px 24px;display:flex;justify-content:space-between;align-items:center;gap:16px;flex-wrap:wrap}.brand{font-size:28px;font-weight:700;letter-spacing:-.03em;color:#fff}.brand-sub{font-size:13px;color:var(--muted);margin-top:4px}.topbar-right{display:flex;align-items:center;gap:12px;flex-wrap:wrap;justify-content:flex-end}.header-actions{display:flex;gap:10px;flex-wrap:wrap;justify-content:flex-end}.btn{display:inline-flex;align-items:center;justify-content:center;padding:13px 20px;border-radius:14px;border:1px solid transparent;background:linear-gradient(135deg,var(--accent),rgba(255,255,255,.18));color:#08111d;font-weight:800;text-decoration:none;cursor:pointer;box-shadow:0 14px 30px rgba(53,194,255,.20)}.btn-secondary{background:rgba(255,255,255,.03);color:#fff;border-color:var(--line);box-shadow:none}.btn-compact{padding:10px 14px;font-size:13px}.layout{display:flex;min-height:calc(100vh - 78px)}.sidebar{width:248px;background:var(--side);border-right:1px solid var(--line);padding:24px 16px;position:sticky;top:78px;height:calc(100vh - 78px);overflow:auto;backdrop-filter:blur(16px)}.side-title{font-size:11px;text-transform:uppercase;letter-spacing:.16em;color:var(--muted);margin:0 0 12px 10px;font-weight:800}.side-link{display:block;text-decoration:none;color:#cbd5e1;padding:12px 14px;border-radius:14px;margin-bottom:8px;border:1px solid transparent;font-size:14px;font-weight:700}.side-link:hover{background:rgba(255,255,255,.04);border-color:var(--line)}.side-link.active{background:linear-gradient(135deg,var(--accent-soft),rgba(255,255,255,.02));border-color:rgba(53,194,255,.28);color:#fff}.content{flex:1;min-width:0;padding:28px}.page-head{display:flex;justify-content:space-between;align-items:flex-start;gap:18px;margin-bottom:20px}.page-head h1{margin:0;font-size:34px;font-weight:800;letter-spacing:-.04em;color:#fff}.page-sub{margin-top:8px;color:var(--muted);font-size:15px;max-width:760px}.status-box,.section,.timeline-card{background:var(--panel);border:1px solid var(--line);border-radius:22px;box-shadow:var(--shadow);backdrop-filter:blur(18px)}.status-box{padding:14px 16px;min-width:260px}.status-row{display:flex;justify-content:space-between;gap:12px;padding:6px 0;font-size:13px}.status-key{color:var(--muted);font-weight:700}.status-val{font-weight:700;color:#fff;text-align:right}.section{margin-bottom:18px;overflow:hidden}.section-head{padding:16px 20px;background:linear-gradient(180deg,rgba(255,255,255,.035),rgba(255,255,255,.01));border-bottom:1px solid var(--line2)}.section-head h2{margin:0;font-size:19px;font-weight:800;color:#fff}.section-head p{margin:6px 0 0 0;font-size:13px;color:var(--muted)}.section-body{padding:20px}.timeline{display:flex;flex-direction:column;gap:16px}.timeline-card{padding:20px;position:relative;overflow:hidden}.version-head{display:flex;justify-content:space-between;gap:12px;align-items:flex-start;flex-wrap:wrap;margin-bottom:12px}.version-title{font-size:22px;font-weight:800;color:#fff}.version-meta{font-size:13px;color:var(--muted)}.change-list{margin:0;padding-left:20px;color:#cbd5e1}.change-list li{margin:10px 0;line-height:1.65}.tag{display:inline-flex;align-items:center;padding:7px 10px;border-radius:999px;font-size:11px;text-transform:uppercase;letter-spacing:.12em;font-weight:800;border:1px solid transparent;margin-right:8px}.tag-added{background:rgba(96,165,250,.14);border-color:rgba(96,165,250,.26);color:#bfdbfe}.tag-fixed{background:rgba(253,164,175,.12);border-color:rgba(253,164,175,.22);color:#fecdd3}.tag-improved{background:rgba(250,204,21,.12);border-color:rgba(250,204,21,.24);color:#fde68a}.tag-feature{background:rgba(74,222,128,.12);border-color:rgba(74,222,128,.24);color:#bbf7d0}.release-summary{margin-top:14px;color:var(--muted);font-size:13px;line-height:1.65}.release-current{box-shadow:0 18px 45px rgba(53,194,255,.18), inset 0 0 0 1px rgba(53,194,255,.10)}.mono{font-family:ui-monospace,SFMono-Regular,Menlo,Consolas,monospace}@media(max-width:1080px){.layout{display:block}.sidebar{width:auto;height:auto;position:relative;top:0;border-right:none;border-bottom:1px solid var(--line)}.content{padding:18px}.page-head{flex-direction:column}.topbar{position:relative}.topbar-right{width:100%;justify-content:flex-start}.header-actions{justify-content:flex-start}})CHANGELOGCSS";
  page += buildSidebarMobileCss();
  page += "</head><body>";

  page += "<div class='topbar'>" + buildTopbarBrandHtml();
  page += "<div class='topbar-right'>";
  page += "<div class='header-actions'>";
  page += "<a class='btn btn-secondary btn-compact' href='/export'>&#128228; Export config</a>";
  page += "<form class='inline-form' method='POST' action='/import' enctype='multipart/form-data'><input type='file' id='import_config_file' name='config_file' accept='.json,application/json' style='display:none' onchange='this.form.submit()'><label class='btn btn-secondary btn-compact' for='import_config_file'>&#128229; Import config</label></form>";
  page += "</div></div></div>";

  page += "<div class='layout'>";
  page += buildSidebarHtml(false, "changelog");
  page += "<main class='content'>";
  page += "<div class='page-head'><div><h1>&#128221; Changelog</h1><div class='page-sub'>Release notes for the Web UI, display firmware and maintenance features.</div></div>";
  page += "<div class='status-box'>";
  page += "<div class='status-row'><span class='status-key'>Device IP</span><span class='status-val'>" + htmlEscape(ip) + "</span></div>";
  page += "<div class='status-row'><span class='status-key'>Backend</span><span class='status-val'>" + htmlEscape(backend) + "</span></div>";
  page += "<div class='status-row'><span class='status-key'>Current version</span><span class='status-val'>" + htmlEscape(String(DISPLAY_VERSION)) + "</span></div>";
  page += "<div class='status-row'><span class='status-key'>Build</span><span class='status-val'>" + htmlEscape(getBuildStamp()) + "</span></div>";
  page += "</div></div>";

  page += R"CHANGELOGHTML(<section class='section'><div class='section-head'><h2>Release history</h2><p>Newest changes are shown first. This page is intended to be kept with the firmware so users can quickly review what changed.</p></div><div class='section-body'><div class='timeline'><article class='timeline-card release-current'><div class='version-head'><div><div class='version-title'>v0.0.1.7</div><div class='version-meta'>SVXLink weather GPS, Waveshare SD-card and Wi-Fi profile fixes</div></div></div><ul class='change-list'><li><span class='tag tag-added'>Added</span>Added SVXLink GPS coordinates for weather information.</li><li><span class='tag tag-fixed'>Fixed</span>Fixed the SDCard for Waveshare 7 Inch displays.</li><li><span class='tag tag-fixed'>Fixed</span>Fixed saved Wi-Fi profiles with SSIDs or passwords containing spaces.</li></ul><div class='release-summary'>Focus: SVXLink GPS-based weather location support, Waveshare 7-inch SD-card reliability, and safer Wi-Fi profile handling for network names and passwords that contain spaces.</div></article><article class='timeline-card'><div class='version-head'><div><div class='version-title'>v0.0.1.6</div><div class='version-meta'>Elecrow ESP32 Display 7.0 Basic support</div></div></div><ul class='change-list'><li><span class='tag tag-feature'>Feature</span>Added support for ELECROW CrowPanel 7&quot; Basic, 800&times;480 TN RGB panel with GT911 touch.</li><li><span class='tag tag-added'>Added</span>Added Elecrow Basic RGB pin map, GT911 I2C touch pins, SD-card SPI pins and GPIO2 PWM backlight handling.</li><li><span class='tag tag-fixed'>Fixed</span>Fixed custom callsign lookup URLs, including plain <span class='mono'>http://</span> endpoints and plain-text name responses.</li></ul><div class='release-summary'>Focus: adding the Elecrow 7-inch ESP32-S3 Basic display and making custom callsign lookup sources work while keeping the existing 800&times;480 UI layout.</div></article><article class='timeline-card'><div class='version-head'><div><div class='version-title'>v0.0.1.5</div><div class='version-meta'>Wi-Fi profiles and WS8048S070C hardware support</div></div></div><ul class='change-list'><li><span class='tag tag-added'>Added</span>Added support for up to 5 Wi-Fi profiles.</li><li><span class='tag tag-feature'>Feature</span>Initial support for WS8048S070C hardware.</li><li><span class='tag tag-added'>Added</span>APRS icons to APRS Page.</li><li><span class='tag tag-added'>Added</span>Retries per WiFi profile.</li><li><span class='tag tag-fixed'>Removed</span>Removed &ldquo;Start Page after boot&rdquo; from the APRS and SVXLink page.</li></ul><div class='release-summary'>Focus: saved Wi-Fi profile handling with retry control, WS8048S070C hardware support, APRS page icons, and cleaner APRS/SVXLink page options.</div></article><article class='timeline-card'><div class='version-head'><div><div class='version-title'>v0.0.1.4</div><div class='version-meta'>APRS.fi, SVXLink and hardware updates</div></div></div><ul class='change-list'><li><span class='tag tag-feature'>Feature</span>Initial support for WS8048S043C hardware.</li><li><span class='tag tag-added'>Added</span>APRS.fi as an optional WATCH source that can be enabled or disabled from the Web UI.</li><li><span class='tag tag-added'>Added</span>Configurable Start Page after boot for MMDVM, SVXLink and APRS when SVXLink or APRS is enabled.</li><li><span class='tag tag-added'>Added</span>Application &gt; APRS settings for the APRS.fi API key and refresh interval.</li><li><span class='tag tag-added'>Added</span>APRS lookup from the latest LIVE/last-heard callsign, including coordinates, last seen time, speed, course, altitude, comment/status and path on the display.</li><li><span class='tag tag-added'>Added</span>SVXLink WATCH APRS button. The button is shown only when aprs.fi has data for the current SVXLink callsign and opens a popup with the latest comment/status, path and position fallback.</li><li><span class='tag tag-fixed'>Fixed</span>SVXLink URL / Dashboard HTML reflector status detection so EchoLink &ldquo;Not connected&rdquo; does not override the ReflectorLogic state.</li><li><span class='tag tag-fixed'>Fixed</span>Dashboard handling where the static HTML can show &ldquo;No status&rdquo; while reflector/TG activity proves the reflector is connected.</li><li><span class='tag tag-improved'>Improved</span>SVXLink JSON bridge compatibility for reflector status when using display-json.php as the source.</li><li><span class='tag tag-improved'>Improved</span>APRS polling safety with cached results, minimum request spacing, failure backoff and a 60-minute wait after aprs.fi rate-limit responses.</li><li><span class='tag tag-fixed'>Fixed</span>Removed the unused SVXLink API token field from the Web UI and configuration import/export.</li></ul><div class='release-summary'>Focus: WS8048S043C hardware support, optional APRS.fi lookup with a SVXLink APRS popup button, safer APRS request timing, corrected SVXLink reflector detection for both Dashboard HTML and JSON bridge sources, and cleaner SVXLink settings.</div></article><article class='timeline-card'><div class='version-head'><div><div class='version-title'>v0.0.1.3</div><div class='version-meta'>SVXLink, WPSD and target-list update</div></div></div><ul class='change-list'><li><span class='tag tag-improved'>Improved</span>SVXLink monitor handling and display behavior for the optional WATCH source.</li><li><span class='tag tag-added'>Added</span>Pagination to the SVXLink HEARD screen so older SVXLink entries can be browsed from the touchscreen.</li><li><span class='tag tag-fixed'>Fixed</span>WPSD mode enable/resume actions by using the Pi-Star/WPSD-compatible mode manager command and Enable/Disable arguments.</li><li><span class='tag tag-improved'>Improved</span>WPSD mode toggles to stop/start the matching gateway services around pause/resume actions where possible.</li><li><span class='tag tag-improved'>Improved</span>Replaced the old About page with a practical FAQ for setup, Actions and SVXLink.</li><li><span class='tag tag-added'>Added</span>DMR duplex LIVE display support: TS1 and TS2 are shown side-by-side only when both timeslots have recent active traffic; otherwise the original single-panel LIVE style is used.</li><li><span class='tag tag-added'>Added</span>SD-card target-name lookup files for DMR, D-STAR, YSF, P25 and NXDN so talkgroup, reflector and room numbers can show readable names.</li><li><span class='tag tag-added'>Added</span>Target lists Web UI page for saving source URLs and manually downloading/updating lookup files on the SD card.</li></ul><div class='release-summary'>Focus: clearer help, better SVXLink browsing, more reliable WPSD mode control, smarter DMR duplex LIVE display behavior and optional SD-based target-name lists.</div></article><article class='timeline-card'><div class='version-head'><div><div class='version-title'>v0.0.1.2</div><div class='version-meta'>SVXLink monitor and scrolling polish</div></div></div><ul class='change-list'><li><span class='tag tag-added'>Added</span>SVXLink Monitor as an experimental live monitor page.</li><li><span class='tag tag-fixed'>Fixed</span>Network access is now serialized with a recursive mutex to prevent lwIP/FreeRTOS socket crashes when WebServer, HTTP/HTTPS and SSH features overlap.</li><li><span class='tag tag-fixed'>Fixed</span>HTTP/HTTPS polling now closes sockets more aggressively, including RadioID, weather and Last Heard lookups.</li><li><span class='tag tag-improved'>Improved</span>Web UI scrollbars are now hidden while pages and sidebars remain scrollable.</li><li><span class='tag tag-improved'>Improved</span>Live page drawLiveInfoPanel status and target values now scroll when too long.</li><li><span class='tag tag-fixed'>Fixed</span>Enabled WPSD Actions.</li><li><span class='tag tag-improved'>Improved</span>Increased the font size for the screensaver target display.</li><li><span class='tag tag-improved'>Improved</span>Status and target values stay inside their boxes, and longer text now scrolls.</li></ul><div class='release-summary'>Focus: experimental SVXLink monitoring, WPSD action fixes, network stability, cleaner browser UI, larger screensaver target text and long-text readability.</div></article><article class='timeline-card'><div class='version-head'><div><div class='version-title'>v0.0.1.1</div><div class='version-meta'>Live display readability and target display fixes</div></div></div><ul class='change-list'><li><span class='tag tag-improved'>Improved</span>Live Info status and target boxes now use the same boxed display style as RX/TX with larger, easier-to-read text.</li><li><span class='tag tag-improved'>Improved</span>Removed the internal STATUS and TARGET labels so the live values have more room inside their boxes.</li><li><span class='tag tag-improved'>Improved</span>Moved the flag and callsign slightly upward to give the status and target area more room.</li><li><span class='tag tag-improved'>Improved</span>RX/TX now displays up to 8 characters instead of 7 and includes a small gap between the RX/TX label and the frequency.</li><li><span class='tag tag-fixed'>Fixed</span>Fixed TG TG xxx by stripping already-present target prefixes before adding display labels.</li><li><span class='tag tag-improved'>Improved</span>Removed the ROOM prefix for YSF/Fusion target display across tabs.</li><li><span class='tag tag-improved'>Improved</span>Gave the Heard tab target/DG-ID field more horizontal space.</li><li><span class='tag tag-fixed'>Fixed</span>Restored the top tab bar on the Info tab.</li><li><span class='tag tag-fixed'>Fixed</span>Removed the old Info-tab &ldquo;tap anywhere to go back&rdquo; behavior so the visible tabs work normally.</li></ul><div class='release-summary'>Focus: easier live-screen reading, longer RX/TX labels and cleaner target/tab behavior.</div></article><article class='timeline-card'><div class='version-head'><div><div class='version-title'>v0.0.1.0</div><div class='version-meta'>CYD ST7789 touch orientation support</div></div></div><ul class='change-list'><li><span class='tag tag-added'>Added</span>Separate CYD touch transform settings for ILI9341 and ST7789 panels.</li><li><span class='tag tag-added'>Added</span>Selectable CYD TFT_eSPI controller support for ILI9341 and ST7789 panels.</li></ul><div class='release-summary'>Focus: CYD panel and touch compatibility plus easier on-device browsing of recent traffic.</div></article><article class='timeline-card'><div class='version-head'><div><div class='version-title'>v0.0.0.9</div><div class='version-meta'>Fixes and Improved</div></div></div><ul class='change-list'><li><span class='tag tag-improved'>Improved</span>Last Heard browsing with paging, filter and sort controls on the device.</li><li><span class='tag tag-fixed'>Fixed</span>Web UI minor bug fixes.</li><li><span class='tag tag-fixed'>Fixed</span>Stats fixed for DMR records.</li></ul><div class='release-summary'>Focus: CYD panel and touch compatibility plus easier on-device browsing of recent traffic.</div></article><article class='timeline-card'><div class='version-head'><div><div class='version-title'>v0.0.0.8</div><div class='version-meta'>OTA, logs and quick presets</div></div></div><ul class='change-list'><li><span class='tag tag-fixed'>Fixed</span>OTA Update.</li><li><span class='tag tag-added'>Added</span>Device log.</li><li><span class='tag tag-added'>Added</span>SSH quick action presets for TG, reflector and disconnect commands.</li></ul><div class='release-summary'>Focus: OTA update reliability, device-side logging and saved remote presets.</div></article><article class='timeline-card'><div class='version-head'><div><div class='version-title'>v0.0.0.7</div><div class='version-meta'>Remote control and maintenance tools</div></div></div><ul class='change-list'><li><span class='tag tag-added'>Added</span>SSH remote login to control DigiModes.</li><li><span class='tag tag-added'>Added</span>OTA firmware update over Web UI.</li><li><span class='tag tag-added'>Added</span>Diagnostics, About and Changelog to Web UI.</li></ul><div class='release-summary'>Focus: remote control, browser-based updates and runtime visibility.</div></article><article class='timeline-card'><div class='version-head'><div><div class='version-title'>v0.0.0.6</div><div class='version-meta'>Maintenance and UI update</div></div></div><ul class='change-list'><li><span class='tag tag-fixed'>Fixed</span>PNG transparency issues.</li><li><span class='tag tag-added'>Added</span>New set of weather icons.</li><li><span class='tag tag-improved'>Improved</span>Touchscreen responsiveness (resolved 300ms delay).</li><li><span class='tag tag-added'>Added</span>Import/Export functionality to the Web UI.</li></ul><div class='release-summary'>Focus: rendering fixes, better touch response and easier configuration backup/restore.</div></article><article class='timeline-card'><div class='version-head'><div><div class='version-title'>v0.0.0.5</div><div class='version-meta'>Weather expansion</div></div></div><ul class='change-list'><li><span class='tag tag-added'>Added</span>New weather icons.</li><li><span class='tag tag-added'>Added</span>Weather configuration settings to the Web UI.</li></ul><div class='release-summary'>Focus: weather visuals and configurable weather support from the browser.</div></article><article class='timeline-card'><div class='version-head'><div><div class='version-title'>v0.0.0.4</div><div class='version-meta'>Stability update</div></div></div><ul class='change-list'><li><span class='tag tag-fixed'>Fixed</span>RadioID polling logic.</li></ul><div class='release-summary'>Focus: more reliable online callsign lookup polling.</div></article><article class='timeline-card'><div class='version-head'><div><div class='version-title'>v0.0.0.3</div><div class='version-meta'>Display and backend polish</div></div></div><ul class='change-list'><li><span class='tag tag-fixed'>Fixed</span>WPSD fetching issues.</li><li><span class='tag tag-improved'>Improved</span>GUI refresh performance and overall UI tweaks.</li><li><span class='tag tag-added'>Added</span>New screensaver mode.</li></ul><div class='release-summary'>Focus: better WPSD communication, smoother rendering and improved idle behavior.</div></article><article class='timeline-card'><div class='version-head'><div><div class='version-title'>v0.0.0.2</div><div class='version-meta'>Hardware support</div></div></div><ul class='change-list'><li><span class='tag tag-feature'>Feature</span>Initial support for JC3248W535C hardware.</li></ul><div class='release-summary'>Focus: bringing the firmware to the JC3248W535C platform.</div></article><article class='timeline-card'><div class='version-head'><div><div class='version-title'>v0.0.0.1</div><div class='version-meta'>Project start</div></div></div><ul class='change-list'><li><span class='tag tag-added'>Added</span>Initial release.</li></ul><div class='release-summary'>The first public baseline for the project.</div></article></div></div></section>)CHANGELOGHTML";

  page += "</main></div>";
  page += buildSidebarMobileScript();
  page += "</body></html>";
  server.send(200, "text/html", page);
}

void handleRoot() {
  String ip = wifiOk ? WiFi.localIP().toString() : (apModeActive ? WiFi.softAPIP().toString() : "not connected");
  String backend = canonicalBackendValue(cfg.backend);
  if (!backend.length()) {
    backend = "Pi-Star";
  }

  String accentHex = cfg.themeColor.length() ? cfg.themeColor : String("#35C2FF");
  String startupMode = apModeActive ? String("Setup AP") : (wifiOk ? String("Wi-Fi station") : String("Offline"));
  String startupAccess = apModeActive ? ("http://" + WiFi.softAPIP().toString() + "/") : (wifiOk ? ("http://" + WiFi.localIP().toString() + "/") : String("Waiting for network"));

  String page;
  page.reserve(25000);

  page += "<!doctype html><html><head><meta charset='utf-8'>";
  page += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
  page += "<title>MMDVM Display</title>";
  page += "<style>";
  page += ":root{--bg:#0b1220;--bg2:#111827;--panel:rgba(15,23,42,.82);--panel-solid:#111827;--line:rgba(148,163,184,.20);--line2:rgba(148,163,184,.12);--text:#e5eefc;--muted:#94a3b8;--side:rgba(7,13,24,.76);--accent:" + accentHex + ";--accent-soft:rgba(53,194,255,.16);--danger:#f87171;--success:#34d399;--shadow:0 18px 45px rgba(2,6,23,.34)}";
  page += "*{box-sizing:border-box}";
  page += "html{scroll-behavior:smooth}";
  page += "body{margin:0;font-family:Inter,Segoe UI,Arial,Helvetica,sans-serif;background:var(--bg);background-attachment:fixed;color:var(--text)}";
  page += "body:before{content:'';position:fixed;inset:0;background:linear-gradient(135deg,rgba(255,255,255,.03),transparent 45%,rgba(255,255,255,.015));pointer-events:none}";
  page += ".topbar{position:sticky;top:0;z-index:40;backdrop-filter:blur(14px);background:rgba(11,18,32,.72);border-bottom:1px solid var(--line);padding:18px 24px;display:flex;justify-content:space-between;align-items:center;gap:16px;flex-wrap:wrap}";
  page += ".brand{font-size:28px;font-weight:700;letter-spacing:-.03em;color:#fff}";
  page += ".brand-sub{font-size:13px;color:var(--muted);margin-top:4px}";
  page += ".topbar-right{display:flex;align-items:center;gap:12px;flex-wrap:wrap;justify-content:flex-end}";
  page += ".header-status{display:flex;flex-direction:column;gap:4px;padding:10px 14px;border-radius:16px;border:1px solid var(--line);background:rgba(148,163,184,.08);min-width:190px}";
  page += ".header-status-label{font-size:10px;text-transform:uppercase;letter-spacing:.14em;color:var(--muted);font-weight:800}";
  page += ".header-status-value{font-size:13px;font-weight:800;color:#fff}";
  page += ".header-actions{display:flex;gap:10px;flex-wrap:wrap;justify-content:flex-end}";
  page += ".inline-form{margin:0}";
  page += ".top-chip{display:inline-flex;align-items:center;gap:10px;padding:10px 14px;border-radius:999px;border:1px solid var(--line);background:rgba(148,163,184,.08);color:#fff;font-size:13px;font-weight:700;box-shadow:var(--shadow)}";
  page += ".chip-dot{width:10px;height:10px;border-radius:999px;background:var(--accent);box-shadow:0 0 0 5px rgba(53,194,255,.14)}";
  page += ".layout{display:flex;min-height:calc(100vh - 78px)}";
  page += ".sidebar{width:248px;background:var(--side);border-right:1px solid var(--line);padding:24px 16px;position:sticky;top:78px;height:calc(100vh - 78px);overflow:auto;backdrop-filter:blur(16px)}";
  page += ".side-title{font-size:11px;text-transform:uppercase;letter-spacing:.16em;color:var(--muted);margin:0 0 12px 10px;font-weight:800}";
  page += ".side-link{display:block;text-decoration:none;color:#cbd5e1;padding:12px 14px;border-radius:14px;margin-bottom:8px;border:1px solid transparent;font-size:14px;font-weight:700;transition:all .18s ease}";
  page += ".side-link:hover{background:rgba(255,255,255,.04);border-color:var(--line);transform:translateX(2px)}";
  page += ".side-link.active{background:linear-gradient(135deg,var(--accent-soft),rgba(255,255,255,.02));border-color:rgba(53,194,255,.28);color:#fff;box-shadow:inset 0 0 0 1px rgba(255,255,255,.04)}";
  page += ".content{flex:1;min-width:0;padding:28px}";
  page += ".breadcrumb{font-size:13px;color:var(--muted);margin-bottom:14px}";
  page += ".breadcrumb a{text-decoration:none;color:var(--text)}";
  page += ".page-head{display:flex;justify-content:space-between;align-items:flex-start;gap:18px;margin-bottom:20px}";
  page += ".page-head h1{margin:0;font-size:34px;font-weight:800;letter-spacing:-.04em;color:#fff}";
  page += ".page-sub{margin-top:8px;color:var(--muted);font-size:15px;max-width:720px}";
  page += ".status-box,.startup-card,.section{background:var(--panel);border:1px solid var(--line);border-radius:22px;box-shadow:var(--shadow);backdrop-filter:blur(18px)}";
  page += ".startup-card{margin-bottom:18px;padding:18px 20px}";
  page += ".startup-grid{display:grid;grid-template-columns:repeat(3,minmax(0,1fr));gap:14px;margin-top:14px}";
  page += ".metric{padding:14px 16px;border-radius:16px;background:rgba(255,255,255,.03);border:1px solid var(--line2)}";
  page += ".metric-label{font-size:12px;text-transform:uppercase;letter-spacing:.12em;color:var(--muted);font-weight:800;margin-bottom:8px}";
  page += ".metric-value{font-size:16px;font-weight:700;color:#fff;word-break:break-word}";
  page += ".status-box{padding:14px 16px;min-width:260px}";
  page += ".status-row{display:flex;justify-content:space-between;gap:12px;padding:6px 0;font-size:13px}";
  page += ".status-key{color:var(--muted);font-weight:700}";
  page += ".status-val{font-weight:700;color:#fff;text-align:right}";
  page += ".section{margin-bottom:18px;overflow:hidden;scroll-margin-top:18px;transition:transform .18s ease,box-shadow .18s ease,border-color .18s ease}";
  page += ".section:hover{transform:translateY(-2px);border-color:rgba(255,255,255,.18)}";
  page += ".section-head{padding:16px 20px;background:linear-gradient(180deg,rgba(255,255,255,.035),rgba(255,255,255,.01));border-bottom:1px solid var(--line2)}";
  page += ".section-head h2{margin:0;font-size:19px;font-weight:800;color:#fff}";
  page += ".section-head p{margin:6px 0 0 0;font-size:13px;color:var(--muted)}";
  page += ".section-body{padding:20px}";
  page += ".form-grid{display:grid;grid-template-columns:1fr 1fr;gap:16px 18px}";
  page += ".form-row{display:flex;flex-direction:column}";
  page += ".form-row label{font-size:13px;font-weight:800;color:#dbe5f5;margin-bottom:8px}";
  page += "input,select{width:100%;padding:13px 14px;border:1px solid rgba(148,163,184,.26);border-radius:14px;background:rgba(15,23,42,.80);color:#fff;font-size:14px;transition:border-color .18s ease,box-shadow .18s ease,background .18s ease}";
  page += "input::placeholder{color:#64748b}";
  page += "input[type='checkbox']{width:auto;transform:scale(1.12);margin-right:10px;padding:0;border:none;accent-color:var(--accent)}";
  page += "input:focus,select:focus{outline:none;border-color:var(--accent);box-shadow:0 0 0 4px rgba(53,194,255,.14);background:rgba(15,23,42,.95)}";
  page += ".toggle-row{display:flex;align-items:center;min-height:48px;padding:0 2px;color:#e2e8f0;font-weight:700}";
  page += ".color-inline{display:flex;gap:10px;align-items:center}";
  page += ".color-picker{width:58px;min-width:58px;height:46px;padding:4px;border-radius:14px;border:1px solid rgba(148,163,184,.26);background:rgba(15,23,42,.8)}";
  page += ".color-text{flex:1}";
  page += ".theme-grid{display:grid;grid-template-columns:1fr 1fr;gap:16px 18px}";
  page += ".footer-actions{display:flex;justify-content:flex-end;gap:12px;padding-top:8px}";
  page += ".btn{display:inline-flex;align-items:center;justify-content:center;padding:13px 20px;border-radius:14px;border:1px solid transparent;background:linear-gradient(135deg,var(--accent),rgba(255,255,255,.18));color:#08111d;font-weight:800;text-decoration:none;cursor:pointer;box-shadow:0 14px 30px rgba(53,194,255,.20);transition:transform .16s ease,filter .16s ease,box-shadow .16s ease}";
  page += ".btn:hover{transform:translateY(-1px);filter:brightness(1.04);box-shadow:0 18px 34px rgba(53,194,255,.24)}";
  page += ".btn-secondary{background:rgba(255,255,255,.03);color:#fff;border-color:var(--line);box-shadow:none}";
  page += ".btn-compact{padding:10px 14px;font-size:13px;min-height:42px}";
  page += ".mini-note{font-size:12px;color:var(--muted);margin-top:12px;line-height:1.55}";
  page += ".quick-grid{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:14px}";
  page += ".quick-card{padding:16px;border-radius:18px;background:rgba(255,255,255,.03);border:1px solid var(--line2)}";
  page += ".quick-card h3{margin:0 0 10px 0;font-size:15px;color:#fff}";
  page += ".quick-run{display:flex;gap:10px;align-items:center;justify-content:space-between;margin-top:12px;flex-wrap:wrap}";
  page += ".hero-card{display:flex;justify-content:space-between;gap:18px;align-items:center;background:linear-gradient(135deg,rgba(15,23,42,.94),rgba(17,24,39,.82));border:1px solid rgba(148,163,184,.18);border-radius:26px;padding:24px 26px;margin:0 0 18px 0;box-shadow:var(--shadow);position:relative;overflow:hidden}";
  page += ".hero-card:after{content:'';position:absolute;right:-40px;top:-40px;width:180px;height:180px;background:radial-gradient(circle,var(--accent-soft),transparent 70%)}";
  page += ".hero-card h3{position:relative;margin:6px 0 8px 0;font-size:28px;font-weight:800;letter-spacing:-.04em;color:#fff}";
  page += ".hero-card p{position:relative;margin:0;color:#c8d4ea;max-width:640px;line-height:1.6}";
  page += ".hero-eyebrow{position:relative;font-size:11px;letter-spacing:.18em;text-transform:uppercase;color:#7dd3fc;font-weight:900}";
  page += ".hero-pill{position:relative;padding:14px 18px;border-radius:999px;background:rgba(255,255,255,.05);border:1px solid rgba(148,163,184,.18);color:#fff;font-weight:800;white-space:nowrap}";
  page += ".theme-preview{display:flex;align-items:center;gap:12px;margin-top:14px;padding:14px 16px;border-radius:16px;background:rgba(255,255,255,.03);border:1px solid var(--line2)}";
  page += ".theme-swatch{width:42px;height:42px;border-radius:14px;background:var(--accent);box-shadow:0 0 0 6px rgba(53,194,255,.10)}";
  page += ".theme-preview strong{display:block;color:#fff;margin-bottom:2px}";
  page += ".alert-card{margin:0 0 18px 0;padding:16px 18px;border-radius:18px;border:1px solid rgba(248,113,113,.34);background:rgba(127,29,29,.18);color:#fecaca;box-shadow:0 8px 22px rgba(127,29,29,.14)}";
  page += ".alert-card p{color:#fecaca}";
  page += "@media(max-width:1080px){.layout{display:block}.sidebar{width:auto;height:auto;position:relative;top:0;border-right:none;border-bottom:1px solid var(--line)}.content{padding:18px}.page-head{flex-direction:column}.form-grid,.theme-grid,.startup-grid{grid-template-columns:1fr}.hero-card{flex-direction:column;align-items:flex-start}.hero-pill{white-space:normal}.topbar{position:relative}.topbar-right{width:100%;justify-content:flex-start}.header-actions{justify-content:flex-start}}"; 
  page += buildSidebarMobileCss();
  page += "</head><body>";

  page += "<div class='topbar'>" + buildTopbarBrandHtml();
  page += "<div class='topbar-right'>";
  page += "<div class='header-actions'>";
  page += "<a class='btn btn-secondary btn-compact' href='/export'>&#128228; Export config</a>";
  page += "<form class='inline-form' method='POST' action='/import' enctype='multipart/form-data'><input type='file' id='import_config_file' name='config_file' accept='.json,application/json' style='display:none' onchange='this.form.submit()'><label class='btn btn-secondary btn-compact' for='import_config_file'>&#128229; Import config</label></form>";
  page += "</div></div></div>";

  page += "<div class='layout'>";

  page += buildSidebarHtml(true, "");

  page += "<main class='content'>";
  page += "<div class='page-head'>";
  page += "<div>";
  page += "<h1>&#9881;&#65039; Configuration</h1>";
  page += "<div class='page-sub'>To set up an MMDVM display.</div>";
  page += "</div>";

  page += "<div class='status-box'>";
  page += "<div class='status-row'><span class='status-key'>Device IP</span><span class='status-val'>" + htmlEscape(ip) + "</span></div>";
  page += "<div class='status-row'><span class='status-key'>Backend</span><span class='status-val'>" + htmlEscape(backend) + "</span></div>";
  page += "<div class='status-row'><span class='status-key'>MMDVM Host</span><span class='status-val'>" + htmlEscape(cfg.pistarHost) + "</span></div>";
  page += "<div class='status-row'><span class='status-key'>SVXLink Host</span><span class='status-val'>" + htmlEscape(cfg.svxlinkHost) + "</span></div>";
  page += "<div class='status-row'><span class='status-key'>APRS</span><span class='status-val'>" + String(aprsFeatureEnabled() ? "Enabled" : "Disabled") + "</span></div>";
  page += "<div class='status-row'><span class='status-key'>HAMQSL</span><span class='status-val'>" + String(hamqslFeatureEnabled() ? "Enabled" : "Disabled") + "</span></div>";
  page += "<div class='status-row'><span class='status-key'>Callsign</span><span class='status-val'>" + htmlEscape(getDeviceCallsign()) + "</span></div>";
  page += "</div>";
  page += "</div>";

  page += "<div class='hero-card'><div><div class='hero-eyebrow'>Personalization</div><h3>Make the display yours</h3><p>Set a custom device callsign for the boot splash and status panels. Leave it empty to keep the default callsign. The admin UI now also picks up your configured theme color as its accent.</p></div><div class='hero-pill'>Active callsign: " + htmlEscape(getDeviceCallsign()) + "</div></div>";

  if (setupRequired) {
    page += "<div class='alert-card'><strong>Setup required.</strong><p>";
    page += (configOkState ? "Wi-Fi could not connect with the saved settings." : "No valid config was found yet.");
    page += "</p><p>Please complete the setup on this page and save it before normal operation starts.</p></div>";
  }

  page += "<form id='config-form' method='POST' action='/save'>";

  page += "<section class='section' id='system'>";
  page += "<div class='section-head'><h2>&#128223; System mode</h2><p>Select the MMDVM dashboard source. SVXLink, APRS, Weather and HAMQSL can be enabled as extra WATCH sources.</p></div>";
  page += "<div class='section-body'><div class='form-grid'>";
  page += selectRow("Active backend", "backend", backend);
  page += formRow("Device callsign", "device_callsign", cfg.deviceCallsign, "text", DEVICE_CALLSIGN);
  if (startPageAfterBootOptionsAvailable()) page += startPageAfterBootSelectRow(cfg.svxlinkEnabled, cfg.aprsEnabled, cfg.weatherEnabled, cfg.hamqslEnabled);
  page += "</div><div class='mini-note'>Pi-Star and WPSD are used for the MMDVM LIVE/HEARD pages. Enable SVXLink, APRS, Weather or HAMQSL to show them under WATCH.</div>";
  page += "</div></section>";

  page += "<section class='section' id='mmdvm'>";
  page += "<div class='section-head'><h2>&#128246; MMDVM settings</h2><p>Pi-Star / WPSD host and SSH remote-control settings.</p></div>";
  page += "<div class='section-body'><div class='form-grid'>";
  page += formRow("Pi-Star / WPSD Host or IP", "pistar_host", cfg.pistarHost, "text", "192.168.1.50");
  page += checkboxRow("Enable SSH remote control (touch + quick actions)", "touch_mode_ssh_enabled", cfg.touchModeSshEnabled);
  page += formRow("SSH user", "pistar_ssh_user", cfg.pistarSshUser, "text", "pi-star");
  page += formRow("SSH password", "pistar_ssh_pass", cfg.pistarSshPass, "password");
  page += formRow("SSH port", "pistar_ssh_port", String(cfg.pistarSshPort), "number");
  page += "</div><div class='mini-note'>These settings are used only when the active backend is Pi-Star or WPSD. The password is stored locally on the display, so use it only on trusted hardware.</div>";
  page += "</div></section>";

  page += "<section class='section' id='quick-actions'>";
  page += "<div class='section-head'><h2>&#9889; Quick actions</h2><p>Save up to four preset SSH commands for talkgroup, reflector or disconnect actions. These presets are available from the touchscreen ACTIONS popup on the live page.</p></div>";
  page += "<div class='section-body'>";
  page += "<div class='quick-grid'>";
  for (uint8_t i = 0; i < QUICK_ACTION_SLOT_COUNT; i++) {
    String n = String(i + 1);
    String prefix = String("quick_action_") + n + String("_");
    page += "<div class='quick-card'>";
    page += "<h3>Preset " + n + "</h3>";
    page += checkboxRow("Enable preset", prefix + "enabled", cfg.quickActions[i].enabled);
    page += checkboxRow("Prefix command with sudo", prefix + "use_sudo", cfg.quickActions[i].useSudo);
    page += formRow("Button label", prefix + "label", cfg.quickActions[i].label, "text", i == 0 ? "DMR TG91" : "REF001C");
    page += formRow("Remote shell command", prefix + "command", cfg.quickActions[i].command, "text", "pistar-link ... or your script");
    page += "<div class='quick-run'>";
    if (isQuickActionSlotConfigured(i)) {
      page += "<span class='mini-note' style='margin:0'>Ready: " + htmlEscape(quickActionSlotLabel(i)) + "</span>";
      page += "<a class='btn btn-secondary btn-compact' href='/quick-action?slot=" + n + "'>&#9654; Run now</a>";
    } else {
      page += "<span class='mini-note' style='margin:0'>Fill in a label and command, then save the config to enable this preset.</span>";
    }
    page += "</div></div>";
  }
  page += "</div>";
  page += "<div class='mini-note'>Each preset runs the exact remote shell command you enter over SSH. This keeps the feature backend-agnostic: you can use it for Pi-Star or WPSD commands, talkgroup helpers, reflector scripts or disconnect commands. On the touchscreen, these presets are launched from the fast <span class='mono'>ACTIONS</span> popup on the live page.</div>";
  page += "</div></section>";

  page += "<section class='section' id='network'>";
  page += "<div class='section-head'><h2>&#128655; Network</h2><p>Polling and connection related settings.</p></div>";
  page += "<div class='section-body'><div class='form-grid'>";
  page += formRow("Refresh interval (ms)", "refresh_ms", String(cfg.refreshMs), "number");
  page += formRow("Callsign hold (ms)", "callsign_hold_ms", String(cfg.callsignHoldMs), "number");
  page += checkboxRow("Convert API UTC time to local device time", "api_utc_to_local_time", cfg.apiUtcToLocalTime);
  page += formRow("Time zone rule (POSIX TZ)", "timezone_tz", cfg.timezoneTz, "text", DEFAULT_TZ_RULE);
  page += "</div><div class='mini-note'>When enabled, last heard entries such as time_utc are converted from UTC to the device local time zone used by the header clock. The time zone rule below is also used by the header clock itself. Example for Germany: <span class='mono'>CET-1CEST,M3.5.0/2,M10.5.0/3</span>.</div></div></section>";

  page += "<section class='section' id='wifi'>";
  page += "<div class='section-head'><h2>&#128225; Wifi</h2><p>Add up to 5 saved Wi-Fi profiles. On boot and reconnect the device can retry the same visible profile before moving to the next saved network. If none connect, setup AP mode stays active.</p></div>";
  page += "<div class='section-body'><div class='form-grid'>";
  for (uint8_t i = 0; i < MAX_WIFI_PROFILES; i++) {
    String n = String(i + 1);
    String ssidField = String("wifi_ssid_") + n;
    String passField = String("wifi_pass_") + n;
    page += formRow(String("WiFi SSID ") + n, ssidField, cfg.wifiProfiles[i].ssid, "text", i == 0 ? "Main WiFi" : "Optional WiFi");
    page += formRow(String("WiFi Password ") + n, passField, cfg.wifiProfiles[i].pass, "password");
  }
  page += formRow("Retries per WiFi profile", "wifi_retries_per_profile", String(effectiveWifiRetriesPerProfile()), "number", String(WIFI_PROFILE_RETRY_COUNT_DEFAULT));
  page += "</div><div class='mini-note'>Leave unused rows empty. Retries per WiFi profile is limited to " + String(WIFI_PROFILE_RETRY_COUNT_MIN) + "-" + String(WIFI_PROFILE_RETRY_COUNT_MAX) + ". 0 means one connection attempt only; 2 means first try plus two retries. The first configured profile is also exported as the legacy wifi_ssid/wifi_pass fields for compatibility.</div></div></section>";

  page += "<section class='section' id='display'>";
  page += "<div class='section-head'><h2>&#128250; Display</h2><p>Local display behavior, intensity, orientation and idle mode.</p></div>";
  page += "<div class='section-body'><div class='form-grid'>";
  page += formRow("Brightness (0-255)", "brightness", String(cfg.brightness), "number");
  page += formRow("Theme base color", "theme_color", cfg.themeColor);
  page += checkboxRow("Flip screen 180&deg;", "flip_screen", cfg.flipScreen);
  page += checkboxRow("Enable scrolling text", "scrolling_text_enabled", cfg.scrollingTextEnabled, "When enabled, long values scroll instead of being shortened with a ~.");
  page += checkboxRow("Enable screensaver", "screensaver_enabled", cfg.screensaverEnabled);
  page += formRow("Screensaver timeout (ms)", "screensaver_timeout_ms", String(cfg.screensaverTimeoutMs), "number");
  page += formRow("Screensaver brightness (0-255)", "screensaver_dim_brightness", String(cfg.screensaverDimBrightness), "number");
  page += "</div><div class='mini-note'>When enabled, the screensaver dims the backlight after inactivity. Any touch wakes the display.</div></div></section>";

  page += "<section class='section' id='weather'>";
  page += "<div class='section-head'><h2>&#9925; Weather</h2><p>Optional OpenWeather source for the WATCH page and screensaver.</p></div>";
  page += "<div class='section-body'><div class='form-grid'>";
  page += checkboxRow("Enable Weather WATCH + screensaver", "weather_enabled", cfg.weatherEnabled);
  page += formRow("Refresh time (minutes)", "weather_refresh_minutes", String(cfg.weatherRefreshMinutes), "number");
  page += formRow("Latitude", "weather_lat", cfg.weatherLat, "text", "50.11552");
  page += formRow("Longitude", "weather_lon", cfg.weatherLon, "text", "8.68417");
  page += formRow("OpenWeather API key", "weather_api_key", cfg.weatherApiKey, "password");
  page += "</div><div class='mini-note'>When enabled, the device fetches current weather from OpenWeather using your latitude, longitude and API key. Weather also becomes available in the WATCH source selector.</div></div></section>";

  page += "<section class='section' id='hamqsl'>";
  page += "<div class='section-head'><h2>&#9728; HAMQSL</h2><p>Optional HAMQSL/N0NBH solar-terrestrial propagation source for the WATCH page.</p></div>";
  page += "<div class='section-body'><div class='form-grid'>";
  page += checkboxRow("Enable HAMQSL WATCH", "hamqsl_enabled", cfg.hamqslEnabled);
  page += formRow("Refresh time (minutes)", "hamqsl_refresh_minutes", String(cfg.hamqslRefreshMinutes), "number", String(HAMQSL_REFRESH_MINUTES_DEFAULT));
  page += formRow("HAMQSL XML URL", "hamqsl_url", cfg.hamqslUrl, "text", HAMQSL_SOLAR_XML_URL_DEFAULT);
  page += "</div><div class='mini-note'>Uses the HAMQSL Solar-Terrestrial XML feed. The minimum refresh is 60 minutes; the display keeps a visible HAMQSL/N0NBH credit.</div></div></section>";

  page += "<section class='section' id='lookup'>";
  page += "<div class='section-head'><h2>&#128218; Lookup</h2><p>Optional online callsign fallback with a configurable users API source.</p></div>";
  page += "<div class='section-body'><div class='form-grid'>";
  page += checkboxRow("Enable RadioID fallback", "radioid_lookup_enabled", cfg.radioIdLookupEnabled);
  page += formRow("Users API base URL", "radioid_users_api_base", cfg.radioIdUsersApiBase, "text", RADIOID_USERS_API_BASE_DEFAULT);
  page += "</div><div class='mini-note'>The callsign is appended automatically. For other APIs you can also use <span class='mono'>%CALLSIGN%</span> anywhere in the URL and it will be replaced with the encoded callsign. Custom lookup responses may be RadioID-style JSON, a JSON object/array with a name field, or plain text with the operator name.</div>";
  page += "<div class='mini-note'>Target, talkgroup, reflector and room names are resolved from SD files in <span class='mono'>/lookup</span>. Manage and manually refresh those files on the <a href='/app/target-lists' style='color:#fff;font-weight:800'>Target lists</a> page.</div>";
  page += "</div></section>";

  page += "<section class='section' id='theme'>";
  page += "<div class='section-head'><h2>&#127912; Theme</h2><p>UI colors for panels, labels and state indicators.</p></div>";
  page += "<div class='section-body'>";
  page += "<div class='theme-grid'>";
  page += colorRow("Background", "bg", color565ToHex(theme.bg));
  page += colorRow("Panel", "panel", color565ToHex(theme.panel));
  page += colorRow("Panel 2", "panel2", color565ToHex(theme.panel2));
  page += colorRow("Border", "border", color565ToHex(theme.border));
  page += colorRow("Text", "text", color565ToHex(theme.text));
  page += colorRow("Subtext", "subtext", color565ToHex(theme.subtext));
  page += colorRow("Accent", "accent", color565ToHex(theme.accent));
  page += colorRow("Good", "good", color565ToHex(theme.good));
  page += colorRow("Warn", "warn", color565ToHex(theme.warn));
  page += colorRow("Bad", "bad", color565ToHex(theme.bad));
  page += colorRow("Live", "live", color565ToHex(theme.live));
  page += colorRow("RX", "rx", color565ToHex(theme.rx));
  page += colorRow("TX", "tx", color565ToHex(theme.tx));
  page += colorRow("Tab idle", "tabIdle", color565ToHex(theme.tabIdle));
  page += colorRow("Tab active", "tabActive", color565ToHex(theme.tabActive));
  page += "</div></div>";
  page += "</section>";

  page += "<div class='footer-actions' style='margin-top:18px'>";
  page += "<button class='btn btn-secondary' type='submit' form='config-form'>&#128190; Save configuration</button>";
  page += "</div>";

  page += "</form>";
  page += R"WIFIHEX(<script>
(function(){
  const form = document.getElementById('config-form');
  if (!form) return;
  function toHex(value){
    let bytes;
    if (window.TextEncoder) {
      bytes = new TextEncoder().encode(value);
    } else {
      bytes = [];
      for (let i = 0; i < value.length; i++) bytes.push(value.charCodeAt(i) & 255);
    }
    let out = '';
    for (const b of bytes) out += b.toString(16).padStart(2, '0');
    return out;
  }
  function setHidden(name, value){
    let field = document.getElementById(name + '_hex');
    if (!field) {
      field = document.createElement('input');
      field.type = 'hidden';
      field.id = name + '_hex';
      field.name = name + '_hex';
      form.appendChild(field);
    }
    field.value = toHex(value);
  }
  form.addEventListener('submit', function(){
    for (let i = 1; i <= 5; i++) {
      const ssid = document.getElementById('wifi_ssid_' + i);
      const pass = document.getElementById('wifi_pass_' + i);
      if (ssid) setHidden('wifi_ssid_' + i, ssid.value);
      if (pass) setHidden('wifi_pass_' + i, pass.value);
    }
  });
})();
</script>)WIFIHEX";

  page += "</main></div>";
  page += buildSidebarMobileScript();
  page += "</body></html>";

  server.send(200, "text/html", page);
}

void handleSave() {
  DynamicJsonDocument doc(15360);

  cfg.backend = canonicalBackendValue(server.arg("backend"));
  if (!cfg.backend.length()) {
    cfg.backend = "Pi-Star";
  }
  clearWifiProfiles();
  if (server.hasArg("wifi_ssid_1") || server.hasArg("wifi_ssid_2")) {
    for (uint8_t i = 0; i < MAX_WIFI_PROFILES; i++) {
      String n = String(i + 1);
      String ssidField = String("wifi_ssid_") + n;
      String passField = String("wifi_pass_") + n;
      addWifiProfile(wifiFormArg(ssidField), wifiFormArg(passField));
    }
  } else {
    addWifiProfile(wifiFormArg("wifi_ssid"), wifiFormArg("wifi_pass"));
  }
  syncLegacyWifiFieldsFromProfiles();
  int wifiRetryCount = server.hasArg("wifi_retries_per_profile") ? server.arg("wifi_retries_per_profile").toInt() : cfg.wifiRetriesPerProfile;
  cfg.wifiRetriesPerProfile = constrain(wifiRetryCount, WIFI_PROFILE_RETRY_COUNT_MIN, WIFI_PROFILE_RETRY_COUNT_MAX);
  cfg.pistarHost = trimSafe(server.arg("pistar_host"));
  cfg.deviceCallsign = trimSafe(server.arg("device_callsign"));
  cfg.themeColor = server.arg("theme_color");
  cfg.refreshMs = server.arg("refresh_ms").toInt();
  cfg.callsignHoldMs = server.arg("callsign_hold_ms").toInt();
  cfg.apiUtcToLocalTime = server.hasArg("api_utc_to_local_time");
  cfg.timezoneTz = trimSafe(server.arg("timezone_tz"));
  if (!cfg.timezoneTz.length()) cfg.timezoneTz = String(DEFAULT_TZ_RULE);
  cfg.brightness = constrain(server.arg("brightness").toInt(), 0, 255);
  cfg.flipScreen = server.hasArg("flip_screen");
  cfg.scrollingTextEnabled = server.hasArg("scrolling_text_enabled");
  cfg.screensaverEnabled = server.hasArg("screensaver_enabled");
  cfg.screensaverTimeoutMs = max(5000L, server.arg("screensaver_timeout_ms").toInt());
  cfg.screensaverDimBrightness = constrain(server.arg("screensaver_dim_brightness").toInt(), 0, 255);
  cfg.weatherEnabled = server.hasArg("weather_enabled");
  cfg.weatherRefreshMinutes = constrain((uint16_t)server.arg("weather_refresh_minutes").toInt(), (uint16_t)1, (uint16_t)1440);
  cfg.weatherLat = trimSafe(server.arg("weather_lat"));
  cfg.weatherLon = trimSafe(server.arg("weather_lon"));
  cfg.weatherApiKey = trimSafe(server.arg("weather_api_key"));
  cfg.hamqslEnabled = server.hasArg("hamqsl_enabled");
  cfg.hamqslRefreshMinutes = constrain((uint16_t)server.arg("hamqsl_refresh_minutes").toInt(), HAMQSL_REFRESH_MINUTES_MIN, HAMQSL_REFRESH_MINUTES_MAX);
  cfg.hamqslUrl = trimSafe(server.arg("hamqsl_url"));
  if (!cfg.hamqslUrl.length()) cfg.hamqslUrl = String(HAMQSL_SOLAR_XML_URL_DEFAULT);
  cfg.radioIdLookupEnabled = server.hasArg("radioid_lookup_enabled");
  cfg.radioIdUsersApiBase = trimSafe(server.arg("radioid_users_api_base"));
  readStartPageAfterBootFromForm();
  cfg.touchModeSshEnabled = server.hasArg("touch_mode_ssh_enabled");
  cfg.pistarSshUser = trimSafe(server.arg("pistar_ssh_user"));
  if (!cfg.pistarSshUser.length()) cfg.pistarSshUser = "pi-star";
  cfg.pistarSshPass = server.arg("pistar_ssh_pass");
  int sshPortValue = server.arg("pistar_ssh_port").toInt();
  cfg.pistarSshPort = (sshPortValue <= 0) ? 22 : constrain(sshPortValue, 1, 65535);
  for (uint8_t i = 0; i < QUICK_ACTION_SLOT_COUNT; i++) {
    String prefix = String("quick_action_") + String(i + 1) + String("_");
    cfg.quickActions[i].enabled = server.hasArg(prefix + "enabled");
    cfg.quickActions[i].useSudo = server.hasArg(prefix + "use_sudo");
    cfg.quickActions[i].label = trimSafe(server.arg(prefix + "label"));
    cfg.quickActions[i].command = trimSafe(server.arg(prefix + "command"));
    if (!cfg.quickActions[i].label.length() || !cfg.quickActions[i].command.length()) {
      cfg.quickActions[i].enabled = false;
    }
  }
  if (!cfg.radioIdUsersApiBase.length()) cfg.radioIdUsersApiBase = String(RADIOID_USERS_API_BASE_DEFAULT);

  syncLegacyWifiFieldsFromProfiles();
  doc["backend"] = cfg.backend;
  doc["wifi_ssid"] = cfg.wifiSsid;
  doc["wifi_pass"] = cfg.wifiPass;
  writeWifiProfilesToJson(doc);
  doc["pistar_host"] = cfg.pistarHost;
  doc["svxlink_enabled"] = cfg.svxlinkEnabled;
  doc["svxlink_source"] = cfg.svxlinkSource;
  doc["svxlink_host"] = cfg.svxlinkHost;
  doc["svxlink_api_path"] = cfg.svxlinkApiPath;
  doc["aprs_enabled"] = cfg.aprsEnabled;
  doc["aprs_api_key"] = cfg.aprsApiKey;
  doc["aprs_refresh_minutes"] = cfg.aprsRefreshMinutes;
  doc["start_page_after_boot"] = normalizedStartPageAfterBoot(false);
  doc["device_callsign"] = cfg.deviceCallsign;
  doc["theme_color"] = cfg.themeColor;
  doc["refresh_ms"] = cfg.refreshMs;
  doc["callsign_hold_ms"] = cfg.callsignHoldMs;
  doc["scrolling_text_enabled"] = cfg.scrollingTextEnabled;
  doc["api_utc_to_local_time"] = cfg.apiUtcToLocalTime;
  doc["timezone_tz"] = cfg.timezoneTz;
  doc["brightness"] = cfg.brightness;
  doc["flip_screen"] = cfg.flipScreen;
  doc["screensaver_enabled"] = cfg.screensaverEnabled;
  doc["screensaver_timeout_ms"] = cfg.screensaverTimeoutMs;
  doc["screensaver_dim_brightness"] = cfg.screensaverDimBrightness;
  doc["weather_enabled"] = cfg.weatherEnabled;
  doc["weather_refresh_minutes"] = cfg.weatherRefreshMinutes;
  doc["weather_lat"] = cfg.weatherLat;
  doc["weather_lon"] = cfg.weatherLon;
  doc["weather_api_key"] = cfg.weatherApiKey;
  doc["hamqsl_enabled"] = cfg.hamqslEnabled;
  doc["hamqsl_refresh_minutes"] = cfg.hamqslRefreshMinutes;
  doc["hamqsl_url"] = cfg.hamqslUrl;
  doc["radioid_lookup_enabled"] = cfg.radioIdLookupEnabled;
  doc["radioid_users_api_base"] = cfg.radioIdUsersApiBase;
  doc["target_list_lookup_enabled"] = cfg.targetListLookupEnabled;
  doc["target_list_base_url"] = cfg.targetListBaseUrl;
  doc["target_list_dmr_url"] = cfg.targetListDmrUrl;
  doc["target_list_dstar_url"] = cfg.targetListDstarUrl;
  doc["target_list_ysf_url"] = cfg.targetListYsfUrl;
  doc["target_list_p25_url"] = cfg.targetListP25Url;
  doc["target_list_nxdn_url"] = cfg.targetListNxdnUrl;
  doc["touch_mode_ssh_enabled"] = cfg.touchModeSshEnabled;
  doc["pistar_ssh_user"] = cfg.pistarSshUser;
  doc["pistar_ssh_pass"] = cfg.pistarSshPass;
  doc["pistar_ssh_port"] = cfg.pistarSshPort;
  doc["svxlink_ssh_enabled"] = cfg.svxlinkSshEnabled;
  doc["svxlink_ssh_user"] = cfg.svxlinkSshUser;
  doc["svxlink_ssh_pass"] = cfg.svxlinkSshPass;
  doc["svxlink_ssh_port"] = cfg.svxlinkSshPort;
  JsonArray qa = doc.createNestedArray("quick_actions");
  for (uint8_t i = 0; i < QUICK_ACTION_SLOT_COUNT; i++) {
    JsonObject item = qa.createNestedObject();
    item["enabled"] = cfg.quickActions[i].enabled;
    item["use_sudo"] = cfg.quickActions[i].useSudo;
    item["label"] = cfg.quickActions[i].label;
    item["command"] = cfg.quickActions[i].command;
  }
  JsonArray sva = doc.createNestedArray("svxlink_actions");
  for (uint8_t i = 0; i < SVXLINK_ACTION_SLOT_COUNT; i++) {
    JsonObject item = sva.createNestedObject();
    item["enabled"] = cfg.svxlinkActions[i].enabled;
    item["use_sudo"] = cfg.svxlinkActions[i].useSudo;
    item["label"] = cfg.svxlinkActions[i].label;
    item["command"] = cfg.svxlinkActions[i].command;
  }

  JsonObject th = doc.createNestedObject("theme");
  th["bg"] = server.arg("bg");
  th["panel"] = server.arg("panel");
  th["panel2"] = server.arg("panel2");
  th["border"] = server.arg("border");
  th["text"] = server.arg("text");
  th["subtext"] = server.arg("subtext");
  th["accent"] = server.arg("accent");
  th["good"] = server.arg("good");
  th["warn"] = server.arg("warn");
  th["bad"] = server.arg("bad");
  th["live"] = server.arg("live");
  th["rx"] = server.arg("rx");
  th["tx"] = server.arg("tx");
  th["tabIdle"] = server.arg("tabIdle");
  th["tabActive"] = server.arg("tabActive");

  storageRemove(CONFIG_FILE);
  File f = storageOpen(CONFIG_FILE, FILE_WRITE);
  if (!f) {
    server.send(500, "text/plain", "Failed to save config");
    return;
  }

  serializeJsonPretty(doc, f);
  f.close();

  addWebLog("INFO", "CONFIG", "Configuration saved from Web UI. Backend " + cfg.backend + ", host " + cfg.pistarHost + ".");
  applyConfigAfterLoad();

  String accentHex = cfg.themeColor.length() ? cfg.themeColor : String("#35C2FF");

  String page;
  page.reserve(7600);
  page += "<!doctype html><html><head><meta charset='utf-8'>";
  page += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
  page += "<title>MMDVM Display</title>";
  page += "<style>";
  page += ":root{--accent:" + accentHex + ";--bg:#0b1220;--panel:rgba(15,23,42,.88);--line:rgba(148,163,184,.18);--text:#e5eefc;--muted:#94a3b8}";
  page += "*{box-sizing:border-box}";
  page += "body{margin:0;font-family:Inter,Segoe UI,Arial,Helvetica,sans-serif;background:#0b1220;color:var(--text)}";
  page += ".topbar{position:sticky;top:0;z-index:40;backdrop-filter:blur(14px);background:rgba(11,18,32,.72);border-bottom:1px solid var(--line);padding:18px 24px;display:flex;justify-content:space-between;align-items:center;gap:16px}";
  page += ".brand{font-size:28px;font-weight:700;letter-spacing:-.03em;color:#fff}";
  page += ".brand-sub{font-size:13px;color:var(--muted);margin-top:4px}";
  page += ".sub{font-size:13px;color:var(--muted);margin-top:4px}";
  page += ".wrap{max-width:820px;margin:34px auto;padding:0 18px}";
  page += ".card{background:var(--panel);border:1px solid var(--line);border-radius:26px;padding:28px;box-shadow:0 24px 46px rgba(2,6,23,.34)}";
  page += ".pill{display:inline-flex;align-items:center;gap:10px;padding:10px 14px;border-radius:999px;background:rgba(255,255,255,.04);border:1px solid var(--line);font-size:13px;font-weight:800}";
  page += ".dot{width:10px;height:10px;border-radius:999px;background:var(--accent);box-shadow:0 0 0 5px rgba(53,194,255,.12)}";
  page += "h2{margin:16px 0 10px 0;font-size:34px;font-weight:800;letter-spacing:-.04em;color:#fff}";
  page += "p{color:#cbd5e1;font-size:15px;line-height:1.65}";
  page += ".btn{display:inline-flex;align-items:center;justify-content:center;padding:13px 20px;border-radius:14px;border:1px solid transparent;background:linear-gradient(135deg,var(--accent),rgba(255,255,255,.18));color:#08111d;font-weight:800;text-decoration:none;cursor:pointer;box-shadow:0 14px 30px rgba(53,194,255,.20);transition:transform .16s ease,filter .16s ease,box-shadow .16s ease}";
  page += ".btn:hover{transform:translateY(-1px);filter:brightness(1.04);box-shadow:0 18px 34px rgba(53,194,255,.24)}";
  page += ".btn-secondary{background:rgba(255,255,255,.03);color:#fff;border-color:var(--line);box-shadow:none}";
  page += ".mono{font-family:monospace;background:rgba(255,255,255,.05);padding:2px 8px;border-radius:7px;color:#fff}";
  page += buildSidebarMobileCss();
  page += "</head><body>";
  page += "<div class='topbar'><div><div class='brand'>MMDVM Display</div><div class='brand-sub'>Version "+ String(DISPLAY_VERSION) +" by 13MAD86 / Martin</div></div></div>";
  page += "<div class='wrap'><div class='card'>";
  page += "<h2>&#128190; Configuration saved</h2>";
  page += "<p>The settings were saved to the device. Current last-heard entries were refreshed with the new online lookup setting, and the device is now checking the saved Wi-Fi profiles.</p>";
  page += "<p>The setup AP stays available until a saved Wi-Fi profile connects successfully. If none of the saved networks work, reconnect to the setup AP and update the Wi-Fi profiles.</p>";
  page += "<p><a class='btn btn-secondary' href='/' onclick='setTimeout(function(){location.href=\"/\";},1200);return true;'>Back to configuration</a></p>";
  page += "</div></div>";
  page += buildSidebarMobileScript();
  page += "</body></html>";
  server.send(200, "text/html", page);
}

void sendWebMessagePage(const String& title, const String& lead, const String& detail, bool isError) {
  String accentHex = cfg.themeColor.length() ? cfg.themeColor : String("#35C2FF");

  String page;
  page.reserve(7600);
  page += "<!doctype html><html><head><meta charset='utf-8'>";
  page += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
  page += "<title>MMDVM Display</title>";
  page += "<style>";
  page += ":root{--accent:" + accentHex + ";--bg:#0b1220;--panel:rgba(15,23,42,.88);--line:rgba(148,163,184,.18);--text:#e5eefc;--muted:#94a3b8;--danger:#f87171}";
  page += "*{box-sizing:border-box}";
  page += "body{margin:0;font-family:Inter,Segoe UI,Arial,Helvetica,sans-serif;background:#0b1220;color:var(--text)}";
  page += ".topbar{position:sticky;top:0;z-index:40;backdrop-filter:blur(14px);background:rgba(11,18,32,.72);border-bottom:1px solid var(--line);padding:18px 24px;display:flex;justify-content:space-between;align-items:center;gap:16px}";
  page += ".brand{font-size:28px;font-weight:700;letter-spacing:-.03em;color:#fff}";
  page += ".brand-sub{font-size:13px;color:var(--muted);margin-top:4px}";
  page += ".wrap{max-width:820px;margin:34px auto;padding:0 18px}";
  page += ".card{background:var(--panel);border:1px solid " + String(isError ? "rgba(248,113,113,.30)" : "var(--line)") + ";border-radius:26px;padding:28px;box-shadow:0 24px 46px rgba(2,6,23,.34)}";
  page += "h2{margin:0 0 12px 0;font-size:34px;font-weight:800;letter-spacing:-.04em;color:#fff}";
  page += "p{color:#cbd5e1;font-size:15px;line-height:1.65}";
  page += ".btn{display:inline-flex;align-items:center;justify-content:center;padding:13px 20px;border-radius:14px;border:1px solid rgba(148,163,184,.18);background:rgba(255,255,255,.03);color:#fff;font-weight:800;text-decoration:none;cursor:pointer}";
  page += ".btn:hover{transform:translateY(-1px)}";
  page += buildSidebarMobileCss();
  page += "</head><body>";
  page += "<div class='topbar'><div><div class='brand'>MMDVM Display</div><div class='brand-sub'>Version "+ String(DISPLAY_VERSION) +" by 13MAD86 / Martin</div></div></div>";
  page += "<div class='wrap'><div class='card'>";
  page += "<h2>&#127756; " + htmlEscape(title) + "</h2>";
  page += "<p>" + htmlEscape(lead) + "</p>";
  if (detail.length()) {
    page += "<p>" + htmlEscape(detail) + "</p>";
  }
  page += "<p><a class='btn' href='/'>Back to configuration</a></p>";
  page += "</div></div>";
  page += buildSidebarMobileScript();
  page += "</body></html>";
  server.send(isError ? 400 : 200, "text/html", page);
}

void sendNoCacheHeaders() {
  server.sendHeader("Cache-Control", "no-store, no-cache, must-revalidate, max-age=0");
  server.sendHeader("Pragma", "no-cache");
  server.sendHeader("Expires", "0");
}

String pageTop(const String& title, const String& activeNav, const String& subtitle) {
  String ip = wifiOk ? WiFi.localIP().toString() : (apModeActive ? WiFi.softAPIP().toString() : "not connected");
  String backend = canonicalBackendValue(cfg.backend);
  if (!backend.length()) backend = "Pi-Star";
  String accentHex = cfg.themeColor.length() ? cfg.themeColor : String("#35C2FF");

  String page;
  page.reserve(17000);
  page += "<!doctype html><html><head><meta charset='utf-8'>";
  page += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
  page += "<title>MMDVM Display</title>";
  page += "<style>";
  page += ":root{--bg:#0b1220;--bg2:#111827;--panel:rgba(15,23,42,.82);--panel-solid:#111827;--line:rgba(148,163,184,.20);--line2:rgba(148,163,184,.12);--text:#e5eefc;--muted:#94a3b8;--side:rgba(7,13,24,.76);--accent:" + accentHex + ";--accent-soft:rgba(53,194,255,.16);--danger:#f87171;--success:#34d399;--shadow:0 18px 45px rgba(2,6,23,.34)}";
  page += "*{box-sizing:border-box}";
  page += "html{scroll-behavior:smooth}";
  page += "body{margin:0;font-family:Inter,Segoe UI,Arial,Helvetica,sans-serif;background:var(--bg);background-attachment:fixed;color:var(--text)}";
  page += "body:before{content:'';position:fixed;inset:0;background:linear-gradient(135deg,rgba(255,255,255,.03),transparent 45%,rgba(255,255,255,.015));pointer-events:none}";
  page += ".topbar{position:sticky;top:0;z-index:40;backdrop-filter:blur(14px);background:rgba(11,18,32,.72);border-bottom:1px solid var(--line);padding:18px 24px;display:flex;justify-content:space-between;align-items:center;gap:16px;flex-wrap:wrap}";
  page += ".brand{font-size:28px;font-weight:700;letter-spacing:-.03em;color:#fff}";
  page += ".brand-sub{font-size:13px;color:var(--muted);margin-top:4px}";
  page += ".topbar-right{display:flex;align-items:center;gap:12px;flex-wrap:wrap;justify-content:flex-end}";
  page += ".header-status{display:flex;flex-direction:column;gap:4px;padding:10px 14px;border-radius:16px;border:1px solid var(--line);background:rgba(148,163,184,.08);min-width:190px}";
  page += ".header-status-label{font-size:10px;text-transform:uppercase;letter-spacing:.14em;color:var(--muted);font-weight:800}";
  page += ".header-status-value{font-size:13px;font-weight:800;color:#fff}";
  page += ".header-actions{display:flex;gap:10px;flex-wrap:wrap;justify-content:flex-end}";
  page += ".inline-form{margin:0}";
  page += ".top-chip{display:inline-flex;align-items:center;gap:10px;padding:10px 14px;border-radius:999px;border:1px solid var(--line);background:rgba(148,163,184,.08);color:#fff;font-size:13px;font-weight:700;box-shadow:var(--shadow)}";
  page += ".chip-dot{width:10px;height:10px;border-radius:999px;background:var(--accent);box-shadow:0 0 0 5px rgba(53,194,255,.14)}";
  page += ".layout{display:flex;min-height:calc(100vh - 78px)}";
  page += ".sidebar{width:248px;background:var(--side);border-right:1px solid var(--line);padding:24px 16px;position:sticky;top:78px;height:calc(100vh - 78px);overflow:auto;backdrop-filter:blur(16px)}";
  page += ".side-title{font-size:11px;text-transform:uppercase;letter-spacing:.16em;color:var(--muted);margin:0 0 12px 10px;font-weight:800}";
  page += ".side-link{display:block;text-decoration:none;color:#cbd5e1;padding:12px 14px;border-radius:14px;margin-bottom:8px;border:1px solid transparent;font-size:14px;font-weight:700;transition:all .18s ease}";
  page += ".side-link:hover{background:rgba(255,255,255,.04);border-color:var(--line);transform:translateX(2px)}";
  page += ".side-link.active{background:linear-gradient(135deg,var(--accent-soft),rgba(255,255,255,.02));border-color:rgba(53,194,255,.28);color:#fff;box-shadow:inset 0 0 0 1px rgba(255,255,255,.04)}";
  page += ".content{flex:1;min-width:0;padding:28px}";
  page += ".breadcrumb{font-size:13px;color:var(--muted);margin-bottom:14px}";
  page += ".breadcrumb a{text-decoration:none;color:var(--text)}";
  page += ".page-head{display:flex;justify-content:space-between;align-items:flex-start;gap:18px;margin-bottom:20px}";
  page += ".page-head h1{margin:0;font-size:34px;font-weight:800;letter-spacing:-.04em;color:#fff}";
  page += ".page-sub{margin-top:8px;color:var(--muted);font-size:15px;max-width:720px}";
  page += ".status-box,.startup-card,.section{background:var(--panel);border:1px solid var(--line);border-radius:22px;box-shadow:var(--shadow);backdrop-filter:blur(18px)}";
  page += ".startup-card{margin-bottom:18px;padding:18px 20px}";
  page += ".startup-grid{display:grid;grid-template-columns:repeat(3,minmax(0,1fr));gap:14px;margin-top:14px}";
  page += ".metric{padding:14px 16px;border-radius:16px;background:rgba(255,255,255,.03);border:1px solid var(--line2)}";
  page += ".metric-label{font-size:12px;text-transform:uppercase;letter-spacing:.12em;color:var(--muted);font-weight:800;margin-bottom:8px}";
  page += ".metric-value{font-size:16px;font-weight:700;color:#fff;word-break:break-word}";
  page += ".status-box{padding:14px 16px;min-width:260px}";
  page += ".status-row{display:flex;justify-content:space-between;gap:12px;padding:6px 0;font-size:13px}";
  page += ".status-key{color:var(--muted);font-weight:700}";
  page += ".status-val{font-weight:700;color:#fff;text-align:right}";
  page += ".section{margin-bottom:18px;overflow:hidden;scroll-margin-top:18px;transition:transform .18s ease,box-shadow .18s ease,border-color .18s ease}";
  page += ".section:hover{transform:translateY(-2px);border-color:rgba(255,255,255,.18)}";
  page += ".section-head{padding:16px 20px;background:linear-gradient(180deg,rgba(255,255,255,.035),rgba(255,255,255,.01));border-bottom:1px solid var(--line2)}";
  page += ".section-head h2{margin:0;font-size:19px;font-weight:800;color:#fff}";
  page += ".section-head p{margin:6px 0 0 0;font-size:13px;color:var(--muted)}";
  page += ".section-body{padding:20px}";
  page += ".form-grid{display:grid;grid-template-columns:1fr 1fr;gap:16px 18px}";
  page += ".form-row{display:flex;flex-direction:column}";
  page += ".form-row label{font-size:13px;font-weight:800;color:#dbe5f5;margin-bottom:8px}";
  page += "input,select{width:100%;padding:13px 14px;border:1px solid rgba(148,163,184,.26);border-radius:14px;background:rgba(15,23,42,.80);color:#fff;font-size:14px;transition:border-color .18s ease,box-shadow .18s ease,background .18s ease}";
  page += "input::placeholder{color:#64748b}";
  page += "input[type='checkbox']{width:auto;transform:scale(1.12);margin-right:10px;padding:0;border:none;accent-color:var(--accent)}";
  page += "input:focus,select:focus{outline:none;border-color:var(--accent);box-shadow:0 0 0 4px rgba(53,194,255,.14);background:rgba(15,23,42,.95)}";
  page += ".toggle-row{display:flex;align-items:center;min-height:48px;padding:0 2px;color:#e2e8f0;font-weight:700}";
  page += ".color-inline{display:flex;gap:10px;align-items:center}";
  page += ".color-picker{width:58px;min-width:58px;height:46px;padding:4px;border-radius:14px;border:1px solid rgba(148,163,184,.26);background:rgba(15,23,42,.8)}";
  page += ".color-text{flex:1}";
  page += ".theme-grid{display:grid;grid-template-columns:1fr 1fr;gap:16px 18px}";
  page += ".footer-actions,.sticky-actions{display:flex;justify-content:flex-end;gap:12px;padding-top:8px;flex-wrap:wrap}";
  page += ".btn{display:inline-flex;align-items:center;justify-content:center;padding:13px 20px;border-radius:14px;border:1px solid transparent;background:linear-gradient(135deg,var(--accent),rgba(255,255,255,.18));color:#08111d;font-weight:800;text-decoration:none;cursor:pointer;box-shadow:0 14px 30px rgba(53,194,255,.20);transition:transform .16s ease,filter .16s ease,box-shadow .16s ease}";
  page += ".btn:hover{transform:translateY(-1px);filter:brightness(1.04);box-shadow:0 18px 34px rgba(53,194,255,.24)}";
  page += ".btn-secondary{background:rgba(255,255,255,.03);color:#fff;border-color:var(--line);box-shadow:none}";
  page += ".btn-compact{padding:10px 14px;font-size:13px;min-height:42px}";
  page += ".mini-note{font-size:12px;color:var(--muted);margin-top:12px;line-height:1.55}";
  page += ".quick-grid{display:grid;grid-template-columns:repeat(2,minmax(0,1fr));gap:14px}";
  page += ".quick-card{padding:16px;border-radius:18px;background:rgba(255,255,255,.03);border:1px solid var(--line2)}";
  page += ".quick-card h3{margin:0 0 10px 0;font-size:15px;color:#fff}";
  page += ".quick-run{display:flex;gap:10px;align-items:center;justify-content:space-between;margin-top:12px;flex-wrap:wrap}";
  page += ".hero-card{display:flex;justify-content:space-between;gap:18px;align-items:center;background:linear-gradient(135deg,rgba(15,23,42,.94),rgba(17,24,39,.82));border:1px solid rgba(148,163,184,.18);border-radius:26px;padding:24px 26px;margin:0 0 18px 0;box-shadow:var(--shadow);position:relative;overflow:hidden}";
  page += ".hero-card:after{content:'';position:absolute;right:-40px;top:-40px;width:180px;height:180px;background:radial-gradient(circle,var(--accent-soft),transparent 70%)}";
  page += ".hero-card h3{position:relative;margin:6px 0 8px 0;font-size:28px;font-weight:800;letter-spacing:-.04em;color:#fff}";
  page += ".hero-card p{position:relative;margin:0;color:#c8d4ea;max-width:640px;line-height:1.6}";
  page += ".hero-eyebrow{position:relative;font-size:11px;letter-spacing:.18em;text-transform:uppercase;color:#7dd3fc;font-weight:900}";
  page += ".hero-pill{position:relative;padding:14px 18px;border-radius:999px;background:rgba(255,255,255,.05);border:1px solid rgba(148,163,184,.18);color:#fff;font-weight:800;white-space:nowrap}";
  page += ".theme-preview{display:flex;align-items:center;gap:12px;margin-top:14px;padding:14px 16px;border-radius:16px;background:rgba(255,255,255,.03);border:1px solid var(--line2)}";
  page += ".theme-swatch{width:42px;height:42px;border-radius:14px;border:1px solid rgba(255,255,255,.18);background:var(--accent);box-shadow:0 12px 24px rgba(0,0,0,.18)}";
  page += ".alert-card{background:rgba(248,113,113,.12);border:1px solid rgba(248,113,113,.32);border-radius:20px;padding:16px 18px;margin-bottom:18px;color:#fecaca}";
  page += ".alert-card strong{color:#fff}";
  page += ".mono{font-family:ui-monospace,SFMono-Regular,Menlo,Consolas,monospace}";
  page += "@media(max-width:1080px){.layout{display:block}.sidebar{width:auto;height:auto;position:relative;top:0;border-right:none;border-bottom:1px solid var(--line)}.content{padding:18px}.page-head{flex-direction:column}.form-grid,.quick-grid,.theme-grid,.startup-grid{grid-template-columns:1fr}.topbar{position:relative}.topbar-right{width:100%;justify-content:flex-start}.header-actions{justify-content:flex-start}.hero-card{align-items:flex-start}.hero-pill{white-space:normal}}";
  page += buildSidebarMobileCss();
  page += "</head><body>";

  page += "<div class='topbar'>" + buildTopbarBrandHtml();
  page += "<div class='topbar-right'>";
  page += "<div class='header-actions'>";
  page += "<a class='btn btn-secondary btn-compact' href='/export'>&#128228; Export config</a>";
  page += "<form class='inline-form' method='POST' action='/import' enctype='multipart/form-data'><input type='file' id='import_config_file' name='config_file' accept='.json,application/json' style='display:none' onchange='this.form.submit()'><label class='btn btn-secondary btn-compact' for='import_config_file'>&#128229; Import config</label></form>";
  page += "</div></div></div>";

  page += "<div class='layout'>";
  page += buildSidebarHtml(false, activeNav);
  page += "<main class='content'>";
  page += "<div class='page-head'><div><h1>" + title + "</h1><div class='page-sub'>" + htmlEscape(subtitle) + "</div></div>";
  page += "<div class='status-box'>";
  page += "<div class='status-row'><span class='status-key'>Device IP</span><span class='status-val'>" + htmlEscape(ip) + "</span></div>";
  page += "<div class='status-row'><span class='status-key'>Backend</span><span class='status-val'>" + htmlEscape(backend) + "</span></div>";
  page += "<div class='status-row'><span class='status-key'>Version</span><span class='status-val'>" + htmlEscape(String(DISPLAY_VERSION)) + "</span></div>";
  page += "</div></div>";
  return page;
}

String pageBottom() {
  String page;
  page += "</main></div>";
  page += buildSidebarMobileScript();
  page += "</body></html>";
  return page;
}

String svxlinkSourceSelectRow() {
  String source = normalizedSvxlinkSource();
  String row = "<div class='form-row'>";
  row += "<label for='svxlink_source'>SVXLink source</label>";
  row += "<select id='svxlink_source' name='svxlink_source'>";
  row += "<option value='url'";
  if (source == "url") row += " selected";
  row += ">URL / dashboard HTML</option>";
  row += "<option value='api'";
  if (source == "api") row += " selected";
  row += ">API / JSON bridge</option>";
  row += "</select></div>";
  return row;
}

String renderSvxlinkActionRows() {
  const char* prefix = "svxlink_action_";
  QuickActionSlotConfig* slots = cfg.svxlinkActions;
  const uint8_t count = SVXLINK_ACTION_SLOT_COUNT;
  String html;
  for (uint8_t i = 0; i < count; i++) {
    String base = String(prefix) + String(i + 1) + "_";
    html += "<div class='quick-card'><h3>Slot " + String(i + 1) + "</h3>";
    html += checkboxRow("Enabled", base + "enabled", slots[i].enabled, "Enable this saved remote action.");
    html += checkboxRow("Use sudo", base + "sudo", slots[i].useSudo, "Prefix this command with sudo on the remote system.");
    html += formRow("Button label", base + "label", slots[i].label, "text", "DTMF 1");
    html += formRow("SSH command / DTMF command", base + "command", slots[i].command, "text", "svxlink command here");
    html += "</div>";
  }
  return html;
}

void readSvxlinkActionRowsFromForm() {
  const char* prefix = "svxlink_action_";
  QuickActionSlotConfig* slots = cfg.svxlinkActions;
  const uint8_t count = SVXLINK_ACTION_SLOT_COUNT;
  for (uint8_t i = 0; i < count; i++) {
    String base = String(prefix) + String(i + 1) + "_";
    slots[i].enabled = server.hasArg(base + "enabled");
    slots[i].useSudo = server.hasArg(base + "sudo");
    slots[i].label = trimSafe(server.arg(base + "label"));
    slots[i].command = trimSafe(server.arg(base + "command"));
    if (!slots[i].label.length() || !slots[i].command.length()) slots[i].enabled = false;
  }
}

void handleSvxlinkPage() {
  sendNoCacheHeaders();
  String page = pageTop("&#128199; SVXLink", "svxlink", "Configure SVXLink as an optional WATCH/HEARD source with its own API/URL and SSH action buttons.");

  page += "<form id='svxlink-form' method='POST' action='/app/svxlink'>";
  page += "<section class='section' id='svxlink-source'>";
  page += "<div class='section-head'><h2>&#128199; SVXLink watch source</h2><p>Enable SVXLink separately from the MMDVM backend. When enabled, LIVE becomes WATCH and lets you choose MMDVM or SVXLink.</p></div>";
  page += "<div class='section-body'><div class='form-grid'>";
  page += checkboxRow("Enable SVXLink", "svxlink_enabled", cfg.svxlinkEnabled, "Adds SVXLink as a second WATCH/HEARD source.");
  page += svxlinkSourceSelectRow();
  page += formRow("SVXLink Host/IP or full URL", "svxlink_host", cfg.svxlinkHost, "text", "192.168.1.60 or https://host/index.php");
  page += formRow("SVXLink URL/API path", "svxlink_api_path", cfg.svxlinkApiPath, "text", isSvxlinkApiBackend() ? "/api/display-json.php" : "/index.php");
  page += "</div><div class='mini-note'><strong>URL</strong> reads the existing dashboard HTML. <strong>API</strong> uses the display-json bridge.</div></div></section>";

  page += "<section class='section' id='svxlink-ssh'>";
  page += "<div class='section-head'><h2>&#128272; SVXLink SSH login</h2><p>Separate SSH credentials for SVXLink actions / DTMF commands.</p></div>";
  page += "<div class='section-body'><div class='form-grid'>";
  page += checkboxRow("Enable SVXLink SSH actions", "svxlink_ssh_enabled", cfg.svxlinkSshEnabled, "Allow the display to run the SVXLink action buttons over SSH.");
  page += formRow("SVXLink SSH user", "svxlink_ssh_user", cfg.svxlinkSshUser, "text", "svxlink");
  page += formRow("SVXLink SSH password", "svxlink_ssh_pass", cfg.svxlinkSshPass, "password");
  page += formRow("SVXLink SSH port", "svxlink_ssh_port", String(cfg.svxlinkSshPort), "number", "22");
  page += "</div><div class='mini-note'>The password is stored locally on the display, so use this only on trusted hardware and networks.</div></div></section>";

  page += "<section class='section' id='svxlink-actions'>";
  page += "<div class='section-head'><h2>&#128241; SVXLink actions / DTMF</h2><p>These buttons appear on the SVXLink live page and run through the SVXLink SSH login above.</p></div>";
  page += "<div class='section-body'><div class='quick-grid'>";
  page += renderSvxlinkActionRows();
  page += "</div><div class='mini-note'>Each slot runs the exact command you enter over SSH. Fill in both a label and command before enabling a slot.</div></div></section>";

  page += "<div class='footer-actions' style='margin-top:18px'>";
  page += "<button class='btn btn-secondary' type='submit' form='svxlink-form'>&#128190; Save configuration</button>";
  page += "</div></form>";
  page += pageBottom();
  server.send(200, "text/html", page);
}

void handleSaveSvxlink() {
  cfg.svxlinkEnabled = server.hasArg("svxlink_enabled");
  cfg.svxlinkSource = canonicalSvxlinkSourceValue(server.arg("svxlink_source"));
  if (!cfg.svxlinkSource.length()) cfg.svxlinkSource = "url";
  cfg.svxlinkHost = trimSafe(server.arg("svxlink_host"));
  cfg.svxlinkApiPath = normalizedSvxlinkPathForBackend(server.arg("svxlink_api_path"));
  cfg.svxlinkSshEnabled = server.hasArg("svxlink_ssh_enabled");
  cfg.svxlinkSshUser = trimSafe(server.arg("svxlink_ssh_user"));
  if (!cfg.svxlinkSshUser.length()) cfg.svxlinkSshUser = "svxlink";
  cfg.svxlinkSshPass = server.arg("svxlink_ssh_pass");
  int svxlinkPortValue = server.arg("svxlink_ssh_port").toInt();
  cfg.svxlinkSshPort = (svxlinkPortValue <= 0) ? 22 : (uint16_t)constrain(svxlinkPortValue, 1, 65535);
  readSvxlinkActionRowsFromForm();

  if (!svxlinkFeatureEnabled() && currentWatchSource == WATCH_SVXLINK) setCurrentWatchSource(WATCH_MMDVM);
  bool ok = saveConfigToFile();
  if (ok) applyConfigAfterLoad();
  addWebLog(ok ? "INFO" : "ERROR", "CONFIG", ok ? "SVXLink settings saved from Web UI." : "SVXLink settings save failed.");

  String accentHex = cfg.themeColor.length() ? cfg.themeColor : String("#35C2FF");
  String page;
  page.reserve(7600);
  page += "<!doctype html><html><head><meta charset='utf-8'>";
  page += "<meta name='viewport' content='width=device-width,initial-scale=1'>";
  page += "<title>MMDVM Display</title>";
  page += "<style>";
  page += ":root{--accent:" + accentHex + ";--bg:#0b1220;--panel:rgba(15,23,42,.88);--line:rgba(148,163,184,.18);--text:#e5eefc;--muted:#94a3b8}";
  page += "*{box-sizing:border-box}";
  page += "body{margin:0;font-family:Inter,Segoe UI,Arial,Helvetica,sans-serif;background:#0b1220;color:var(--text)}";
  page += ".topbar{position:sticky;top:0;z-index:40;backdrop-filter:blur(14px);background:rgba(11,18,32,.72);border-bottom:1px solid var(--line);padding:18px 24px;display:flex;justify-content:space-between;align-items:center;gap:16px}";
  page += ".brand{font-size:28px;font-weight:700;letter-spacing:-.03em;color:#fff}";
  page += ".brand-sub{font-size:13px;color:var(--muted);margin-top:4px}";
  page += ".sub{font-size:13px;color:var(--muted);margin-top:4px}";
  page += ".wrap{max-width:820px;margin:34px auto;padding:0 18px}";
  page += ".card{background:var(--panel);border:1px solid var(--line);border-radius:26px;padding:28px;box-shadow:0 24px 46px rgba(2,6,23,.34)}";
  page += ".pill{display:inline-flex;align-items:center;gap:10px;padding:10px 14px;border-radius:999px;background:rgba(255,255,255,.04);border:1px solid var(--line);font-size:13px;font-weight:800}";
  page += ".dot{width:10px;height:10px;border-radius:999px;background:var(--accent);box-shadow:0 0 0 5px rgba(53,194,255,.12)}";
  page += "h2{margin:16px 0 10px 0;font-size:34px;font-weight:800;letter-spacing:-.04em;color:#fff}";
  page += "p{color:#cbd5e1;font-size:15px;line-height:1.65}";
  page += ".btn{display:inline-flex;align-items:center;justify-content:center;padding:13px 20px;border-radius:14px;border:1px solid transparent;background:linear-gradient(135deg,var(--accent),rgba(255,255,255,.18));color:#08111d;font-weight:800;text-decoration:none;cursor:pointer;box-shadow:0 14px 30px rgba(53,194,255,.20);transition:transform .16s ease,filter .16s ease,box-shadow .16s ease}";
  page += ".btn:hover{transform:translateY(-1px);filter:brightness(1.04);box-shadow:0 18px 34px rgba(53,194,255,.24)}";
  page += ".btn-secondary{background:rgba(255,255,255,.03);color:#fff;border-color:var(--line);box-shadow:none}";
  page += ".mono{font-family:monospace;background:rgba(255,255,255,.05);padding:2px 8px;border-radius:7px;color:#fff}";
  page += buildSidebarMobileCss();
  page += "</head><body>";
  page += "<div class='topbar'><div><div class='brand'>MMDVM Display</div><div class='brand-sub'>Version "+ String(DISPLAY_VERSION) +" by 13MAD86 / Martin</div></div></div>";
  page += "<div class='wrap'><div class='card'>";
  page += "<h2>&#128190; Configuration saved</h2>";
  page += "<p>The settings were saved to the device. Current last-heard entries were refreshed with the new online lookup setting, and the network is now restarting, so the current AP page may disconnect for a moment.</p>";
  page += "<p>After a few seconds, reopen the device on its new address from your router or use the setup AP again if Wi-Fi did not connect.</p>";
  page += "<p><a class='btn btn-secondary' href='/' onclick='setTimeout(function(){location.href=\"/\";},1200);return true;'>Back to configuration</a></p>";
  page += "</div></div>";
  page += buildSidebarMobileScript();
  page += "</body></html>";
  server.send(200, "text/html", page);
}

void handleRunQuickAction() {
  int slot = server.arg("slot").toInt() - 1;
  if (slot < 0 || slot >= QUICK_ACTION_SLOT_COUNT) {
    sendWebMessagePage("Quick action failed", "The selected preset slot is invalid.", "Open the configuration page and try again.", true);
    return;
  }

  String msg;
  bool ok = runQuickActionSlotNow((uint8_t)slot, &msg, false);
  addWebLog(ok ? "INFO" : "WARN", "ACTION", msg.length() ? msg : (ok ? String("Quick action completed.") : String("Quick action failed.")));
  sendWebMessagePage(ok ? String("Quick action sent") : String("Quick action failed"),
                     msg.length() ? msg : (ok ? String("The remote preset was executed.") : String("The remote preset could not be executed.")),
                     ok ? String("The live page will refresh again on its next poll.") : String("Check the SSH settings, command text and hotspot host configuration."),
                     !ok);
}

void handleExportConfig() {
  if (!saveConfigToFile()) {
    addWebLog("ERROR", "CONFIG", "Config export failed: unable to prepare export file.");
    server.send(500, "text/plain", "Failed to prepare config export");
    return;
  }

  File f = storageOpen(CONFIG_FILE, FILE_READ);
  if (!f) {
    addWebLog("ERROR", "CONFIG", "Config export failed: export file not found.");
    server.send(404, "text/plain", "Config file not found");
    return;
  }

  server.sendHeader("Cache-Control", "no-store");
  server.sendHeader("Content-Disposition", "attachment; filename=\"mmdvm-display-config.json\"");
  server.streamFile(f, "application/json");
  f.close();
  addWebLog("INFO", "CONFIG", "Configuration exported from Web UI.");
}

void handleImportConfigUpload() {
  HTTPUpload& upload = server.upload();
  static File uploadFile;

  if (upload.status == UPLOAD_FILE_START) {
    storageRemove(IMPORT_CONFIG_TEMP_FILE);
    uploadFile = storageOpen(IMPORT_CONFIG_TEMP_FILE, FILE_WRITE);
    if (!uploadFile) {
      if (uploadFile) uploadFile.close();
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (uploadFile) {
      if (uploadFile.write(upload.buf, upload.currentSize) != upload.currentSize) {
        uploadFile.close();
        storageRemove(IMPORT_CONFIG_TEMP_FILE);
      }
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (uploadFile) uploadFile.close();
  } else if (upload.status == UPLOAD_FILE_ABORTED) {
    if (uploadFile) uploadFile.close();
    storageRemove(IMPORT_CONFIG_TEMP_FILE);
  }
}

void handleImportConfig() {
  if (!storageExists(IMPORT_CONFIG_TEMP_FILE)) {
    addWebLog("WARN", "CONFIG", "Config import failed: no file uploaded.");
    sendWebMessagePage("Config import failed", "No config file was uploaded.", "Choose an exported JSON config and try again.", true);
    return;
  }

  String err;
  bool ok = applyConfigFromFile(IMPORT_CONFIG_TEMP_FILE, &err);
  storageRemove(IMPORT_CONFIG_TEMP_FILE);

  if (!ok) {
    addWebLog("WARN", "CONFIG", "Config import failed: " + (err.length() ? err : String("unknown error")) + ".");
    sendWebMessagePage("Config import failed", err.length() ? err : String("The uploaded config could not be applied."), "Use a JSON file exported by this device UI.", true);
    return;
  }

  addWebLog("INFO", "CONFIG", "Configuration imported from Web UI.");
  sendWebMessagePage("Config imported", "The uploaded configuration was applied and the device is refreshing its network and runtime settings.", "If Wi-Fi credentials changed, reconnect using the new device address or use the setup AP again if the station connection cannot be established.", false);
}

String targetListStatusTableHtml() {
  const char* modes[] = { "DMR", "DSTAR", "YSF", "P25", "NXDN" };
  String html;
  html.reserve(4200);
  html += "<table class='list-table'><thead><tr><th>Mode</th><th>SD file</th><th>Status</th><th>Source URL</th></tr></thead><tbody>";
  for (uint8_t i = 0; i < 5; ++i) {
    String mode = String(modes[i]);
    const char* path = targetListFilePathForMode(mode);
    html += "<tr><td><strong>" + htmlEscape(mode) + "</strong></td>";
    html += "<td><span class='mono'>" + htmlEscape(String(path ? path : "")) + "</span></td>";
    html += "<td>" + htmlEscape(targetListFileSizeText(mode)) + "</td>";
    html += "<td class='url-cell'>" + htmlEscape(targetListUrlForMode(mode)) + "</td></tr>";
  }
  html += "</tbody></table>";
  return html;
}

String targetListOverrideRowsHtml() {
  const char* modes[] = { "DMR", "DSTAR", "YSF", "P25", "NXDN" };
  String html;
  html.reserve(3600);
  for (uint8_t i = 0; i < 5; ++i) {
    String mode = String(modes[i]);
    String field = targetListModeFieldName(mode);
    String value = targetListOverrideUrlForMode(mode);
    String placeholder = String(TARGET_LIST_BASE_URL_DEFAULT) + String(targetListDefaultPathForMode(mode));
    html += formRow(mode + " source override URL", field, value, "text", placeholder);
  }
  return html;
}


void handleHamqslPage() {
  sendNoCacheHeaders();
  String page = pageTop("&#9728; HAMQSL", "hamqsl", "Configure HAMQSL/N0NBH solar-terrestrial propagation data as an optional WATCH screen.");

  page += "<form id='hamqsl-form' method='POST' action='/app/hamqsl'>";
  page += "<section class='section' id='hamqsl-source'>";
  page += "<div class='section-head'><h2>&#9728; HAMQSL watch source</h2><p>Enable a WATCH page showing solar flux, A/K index, X-ray level, MUF and HF/VHF band conditions.</p></div>";
  page += "<div class='section-body'><div class='form-grid'>";
  page += checkboxRow("Enable HAMQSL", "hamqsl_enabled", cfg.hamqslEnabled, "Adds HAMQSL as a selectable WATCH source.");
  page += formRow("HAMQSL XML URL", "hamqsl_url", cfg.hamqslUrl, "text", HAMQSL_SOLAR_XML_URL_DEFAULT);
  page += formRow("Refresh interval minutes", "hamqsl_refresh_minutes", String(cfg.hamqslRefreshMinutes), "number", String(HAMQSL_REFRESH_MINUTES_DEFAULT));
  page += "</div><div class='mini-note'>Default source: <span class='mono'>" + htmlEscape(String(HAMQSL_SOLAR_XML_URL_DEFAULT)) + "</span>. Keep refresh at 60 minutes or more to respect the feed update period. The display screen shows a visible HAMQSL/N0NBH credit.</div></div></section>";
  page += "<div class='footer-actions' style='margin-top:18px'>";
  page += "<button class='btn btn-secondary' type='submit' form='hamqsl-form'>&#128190; Save configuration</button>";
  page += "</div></form>";
  page += pageBottom();
  server.send(200, "text/html", page);
}

void handleSaveHamqsl() {
  cfg.hamqslEnabled = server.hasArg("hamqsl_enabled");
  cfg.hamqslUrl = trimSafe(server.arg("hamqsl_url"));
  if (!cfg.hamqslUrl.length()) cfg.hamqslUrl = String(HAMQSL_SOLAR_XML_URL_DEFAULT);
  cfg.hamqslRefreshMinutes = constrain((uint16_t)server.arg("hamqsl_refresh_minutes").toInt(), HAMQSL_REFRESH_MINUTES_MIN, HAMQSL_REFRESH_MINUTES_MAX);
  clearHamqslData();

  if (!hamqslFeatureEnabled() && currentWatchSource == WATCH_HAMQSL) setCurrentWatchSource(WATCH_MMDVM);
  bool ok = saveConfigToFile();
  if (ok) applyConfigAfterLoad();
  addWebLog(ok ? "INFO" : "ERROR", "CONFIG", ok ? "HAMQSL settings saved from Web UI." : "HAMQSL settings save failed.");

  sendWebMessagePage(ok ? "HAMQSL settings saved" : "HAMQSL save failed",
                     ok ? "HAMQSL WATCH source settings were saved." : "The HAMQSL settings could not be saved.",
                     ok ? "Tap WATCH on the display and choose HAMQSL to view propagation conditions." : "Check storage and try again.",
                     !ok);
}

void handleAprsPage() {
  sendNoCacheHeaders();
  String page = pageTop("&#128205; APRS", "aprs", "Configure APRS.fi as an optional WATCH screen using the last seen LIVE callsign.");

  page += "<form id='aprs-form' method='POST' action='/app/aprs'>";
  page += "<section class='section' id='aprs-source'>";
  page += "<div class='section-head'><h2>&#128205; APRS.fi watch source</h2><p>Enable APRS as a WATCH button beside MMDVM/SVXLink. The query callsign is taken from the current LIVE/last-heard callsign.</p></div>";
  page += "<div class='section-body'><div class='form-grid'>";
  page += checkboxRow("Enable APRS", "aprs_enabled", cfg.aprsEnabled, "Adds APRS as a selectable WATCH source.");
  page += formRow("APRS.fi API key", "aprs_api_key", cfg.aprsApiKey, "password", "Paste your aprs.fi API key");
  page += formRow("Refresh interval minutes", "aprs_refresh_minutes", String(cfg.aprsRefreshMinutes), "number", String(APRS_REFRESH_MINUTES_DEFAULT));
  page += "</div><div class='mini-note'>The display builds the API request as <span class='mono'>name=&lt;last LIVE callsign&gt;&amp;what=loc&amp;format=json</span>. APRS.fi data is requested only while the APRS WATCH screen is selected. Minimum refresh is 10 minutes, requests are spaced at least 5 minutes apart, and rate-limit responses trigger a 60-minute backoff. The screen shows a visible aprs.fi credit.</div></div></section>";
  page += "<div class='footer-actions' style='margin-top:18px'>";
  page += "<button class='btn btn-secondary' type='submit' form='aprs-form'>&#128190; Save configuration</button>";
  page += "</div></form>";
  page += pageBottom();
  server.send(200, "text/html", page);
}

void handleSaveAprs() {
  cfg.aprsEnabled = server.hasArg("aprs_enabled");
  cfg.aprsApiKey = trimSafe(server.arg("aprs_api_key"));
  cfg.aprsRefreshMinutes = constrain((uint16_t)server.arg("aprs_refresh_minutes").toInt(), APRS_REFRESH_MINUTES_MIN, APRS_REFRESH_MINUTES_MAX);
  resetAprsState();

  if (!aprsFeatureEnabled() && currentWatchSource == WATCH_APRS) setCurrentWatchSource(WATCH_MMDVM);
  bool ok = saveConfigToFile();
  if (ok) applyConfigAfterLoad();
  addWebLog(ok ? "INFO" : "ERROR", "CONFIG", ok ? "APRS settings saved from Web UI." : "APRS settings save failed.");

  sendWebMessagePage(ok ? "APRS settings saved" : "APRS save failed",
                     ok ? "APRS.fi WATCH source settings were saved." : "The APRS settings could not be saved.",
                     ok ? "Tap WATCH on the display and choose APRS after a callsign appears on LIVE." : "Check storage and try again.",
                     !ok);
}

void handleTargetListsPage() {
  sendNoCacheHeaders();
  String wifiState = WiFi.status() == WL_CONNECTED ? String("Connected") : String("Not connected");
  String lookupState = cfg.targetListLookupEnabled ? String("Enabled") : String("Disabled");

  String page = pageTop("&#127915; Target lists", "target-lists", "Manage SD-card talkgroup, reflector and room name lists. Normal display lookup reads local /lookup/*.txt files, so live display updates stay fast and work offline.");

  page += "<style>";
  page += ".list-table{width:100%;border-collapse:separate;border-spacing:0 8px}";
  page += ".list-table th{font-size:11px;text-transform:uppercase;letter-spacing:.12em;color:var(--muted);text-align:left;padding:0 10px 4px}";
  page += ".list-table td{padding:12px 10px;background:rgba(255,255,255,.03);border-top:1px solid var(--line2);border-bottom:1px solid var(--line2);font-size:13px;vertical-align:top}";
  page += ".list-table td:first-child{border-left:1px solid var(--line2);border-radius:14px 0 0 14px}";
  page += ".list-table td:last-child{border-right:1px solid var(--line2);border-radius:0 14px 14px 0}";
  page += ".url-cell{word-break:break-all;color:#cbd5e1}";
  page += ".target-info-grid{display:grid;grid-template-columns:repeat(3,minmax(0,1fr));gap:14px}";
  page += "@media(max-width:1080px){.target-info-grid{grid-template-columns:1fr}.list-table{font-size:12px}.list-table thead{display:none}.list-table tr{display:block;margin-bottom:10px}.list-table td{display:block;border:1px solid var(--line2);border-bottom:none;border-radius:0}.list-table td:first-child{border-radius:14px 14px 0 0}.list-table td:last-child{border-bottom:1px solid var(--line2);border-radius:0 0 14px 14px}}";
  page += "</style>";

  page += "<section class='section' id='target-list-overview'>";
  page += "<div class='section-head'><h2>&#128269; Lookup overview</h2><p>These settings control name resolution for DMR, D-STAR, YSF, P25 and NXDN targets.</p></div>";
  page += "<div class='section-body'><div class='target-info-grid'>";
  page += "<div class='quick-card'><h3>Lookup status</h3><div class='mini-note'><strong>" + htmlEscape(lookupState) + "</strong><br>When enabled, the display checks SD files before falling back to built-in names or raw numbers.</div></div>";
  page += "<div class='quick-card'><h3>Network status</h3><div class='mini-note'><strong>" + htmlEscape(wifiState) + "</strong><br>Network is only needed when you press the manual update button.</div></div>";
  page += "<div class='quick-card'><h3>File format</h3><div class='mini-note'><span class='mono'>262|Germany</span><br>Also accepts semicolon, comma, tab or simple whitespace-separated text.</div></div>";
  page += "</div></div></section>";

  page += "<section class='section' id='target-list-sources'><div class='section-head'><h2>&#127760; Source URLs</h2><p>Use the default W0CHP base URL, or enter mode-specific full URLs for another source.</p></div><div class='section-body'>";
  page += "<form id='target-list-config-form' method='POST' action='/app/target-lists/save'>";
  page += "<div class='form-grid'>";
  page += checkboxRow("Enable SD target-name lookup", "target_list_lookup_enabled", cfg.targetListLookupEnabled, "Resolve target numbers from SD files in /lookup.");
  page += formRow("Default base URL", "target_list_base_url", cfg.targetListBaseUrl, "text", TARGET_LIST_BASE_URL_DEFAULT);
  page += targetListOverrideRowsHtml();
  page += "</div><div class='mini-note'>Leave a mode override empty to use the base URL plus the W0CHP-style default path, for example <span class='mono'>brandmeister-talkgroups/</span> for DMR.</div>";
  page += "<div class='footer-actions' style='margin-top:18px'><button class='btn btn-secondary' type='submit'>&#128190; Save target-list settings</button></div>";
  page += "</form></div></section>";

  page += "<section class='section' id='target-list-files'><div class='section-head'><h2>&#128190; SD files</h2><p>Press update only when you choose to refresh the files. Existing files stay on the SD card and are used immediately for display lookup.</p></div><div class='section-body'>";
  page += targetListStatusTableHtml();
  page += "<form method='POST' action='/app/target-lists/update'>";
  page += "<div class='form-grid'>";
  page += checkboxRow("Update DMR", "mode_dmr", true);
  page += checkboxRow("Update D-STAR / XLX", "mode_dstar", true);
  page += checkboxRow("Update YSF", "mode_ysf", true);
  page += checkboxRow("Update P25", "mode_p25", true);
  page += checkboxRow("Update NXDN", "mode_nxdn", true);
  page += "</div>";
  page += "<div class='footer-actions' style='margin-top:18px'><button class='btn btn-secondary' type='submit'>&#11015;&#65039; Update</button></div>";
  page += "</form>";
  page += "<div class='mini-note'>After a successful update, the lookup cache is cleared so the next live/heard refresh can use the new names.</div>";
  page += "</div></section>";

  page += pageBottom();
  server.send(200, "text/html", page);
}

void handleSaveTargetLists() {
  cfg.targetListLookupEnabled = server.hasArg("target_list_lookup_enabled");
  cfg.targetListBaseUrl = trimSafe(server.arg("target_list_base_url"));
  if (!cfg.targetListBaseUrl.length()) cfg.targetListBaseUrl = String(TARGET_LIST_BASE_URL_DEFAULT);
  cfg.targetListDmrUrl = trimSafe(server.arg("target_list_dmr_url"));
  cfg.targetListDstarUrl = trimSafe(server.arg("target_list_dstar_url"));
  cfg.targetListYsfUrl = trimSafe(server.arg("target_list_ysf_url"));
  cfg.targetListP25Url = trimSafe(server.arg("target_list_p25_url"));
  cfg.targetListNxdnUrl = trimSafe(server.arg("target_list_nxdn_url"));

  if (!saveConfigToFile()) {
    sendWebMessagePage("Target-list settings not saved", "The settings could not be written to the SD card.", "Check that the SD card is mounted and writable.", true);
    return;
  }

  clearTargetNameLookupCache();
  addWebLog("INFO", "LOOKUP", "Target-list source settings saved from Web UI.");
  sendWebMessagePage("Target-list settings saved", "The source URL settings were saved.", "Open the Target lists page again to update files when you choose.", false);
}

void handleUpdateTargetLists() {
#if ENABLE_TARGET_LIST_SD_LOOKUP
  if (WiFi.status() != WL_CONNECTED) {
    sendWebMessagePage("Target-list update failed", "WiFi is not connected, so the device cannot download target lists right now.", "Connect WiFi first, then run the update again.", true);
    return;
  }

  struct UpdateChoice { const char* mode; const char* arg; } choices[] = {
    { "DMR", "mode_dmr" }, { "DSTAR", "mode_dstar" }, { "YSF", "mode_ysf" }, { "P25", "mode_p25" }, { "NXDN", "mode_nxdn" }
  };

  bool any = false;
  bool allOk = true;
  String detail = "";
  for (uint8_t i = 0; i < 5; ++i) {
    if (!server.hasArg(choices[i].arg)) continue;
    any = true;
    String msg;
    bool ok = downloadTargetListForMode(String(choices[i].mode), &msg);
    if (!ok) allOk = false;
    if (detail.length()) detail += " | ";
    detail += String(choices[i].mode) + ": " + msg;
  }

  if (!any) {
    sendWebMessagePage("No target lists selected", "Choose at least one mode to update.", "Nothing was downloaded or changed.", true);
    return;
  }

  clearTargetNameLookupCache();
  sendWebMessagePage(allOk ? "Target lists updated" : "Target-list update finished with errors",
                     allOk ? "The selected list files were downloaded and saved to the SD card." : "At least one selected list could not be downloaded or converted.",
                     detail, !allOk);
#else
  sendWebMessagePage("Target-list update unavailable", "Target-list SD lookup is disabled at compile time.", "Enable ENABLE_TARGET_LIST_SD_LOOKUP and rebuild the firmware.", true);
#endif
}

void setupWebServer() {

  server.on("/", HTTP_GET, handleRoot);
  server.on("/save", HTTP_POST, handleSave);
  server.on("/app/svxlink", HTTP_GET, handleSvxlinkPage);
  server.on("/app/svxlink", HTTP_POST, handleSaveSvxlink);
  server.on("/app/aprs", HTTP_GET, handleAprsPage);
  server.on("/app/aprs", HTTP_POST, handleSaveAprs);
  server.on("/app/hamqsl", HTTP_GET, handleHamqslPage);
  server.on("/app/hamqsl", HTTP_POST, handleSaveHamqsl);
  server.on("/app/target-lists", HTTP_GET, handleTargetListsPage);
  server.on("/app/target-lists/save", HTTP_POST, handleSaveTargetLists);
  server.on("/app/target-lists/update", HTTP_POST, handleUpdateTargetLists);
  server.on("/quick-action", HTTP_GET, handleRunQuickAction);
  server.on("/export", HTTP_GET, handleExportConfig);
  server.on("/import", HTTP_POST, handleImportConfig, handleImportConfigUpload);
  server.on("/app/diagnostics", HTTP_GET, handleDiagnosticsPage);
  server.on("/api/diagnostics", HTTP_GET, handleDiagnosticsApi);
  server.on("/app/logs", HTTP_GET, handleLogsPage);
  server.on("/api/logs", HTTP_GET, handleLogsApi);
  server.on("/api/logs/clear", HTTP_POST, handleLogsClear);
  server.on("/app/ota", HTTP_GET, handleOtaPage);
  server.on("/app/ota", HTTP_POST, handleOtaUpdate, handleOtaUpload);
  server.on("/app/faq", HTTP_GET, handleFaqPage);
  server.on("/app/about", HTTP_GET, handleFaqPage);
  server.on("/app/changelog", HTTP_GET, handleChangelogPage);
  server.begin();
  addWebLog("INFO", "WEB", "Web server started on port 80.");
}

void clearHeard() {
  heardCount = 0;
  lastHeardPageIndex = 0;
  for (int i = 0; i < MAX_HEARD; i++) heardList[i] = HeardRecord();
}

void copyCurrentHeardStateTo(HeardRecord* dstList, int& dstCount, HeardRecord& dstLive, unsigned long& dstLiveMillis) {
  dstCount = heardCount;
  for (int i = 0; i < MAX_HEARD; i++) dstList[i] = heardList[i];
  dstLive = liveRecord;
  dstLiveMillis = liveRecordMillis;
}

void restoreHeardStateFrom(HeardRecord* srcList, int srcCount, const HeardRecord& srcLive, unsigned long srcLiveMillis) {
  heardCount = constrain(srcCount, 0, MAX_HEARD);
  for (int i = 0; i < MAX_HEARD; i++) heardList[i] = (i < heardCount) ? srcList[i] : HeardRecord();
  liveRecord = srcLive;
  liveRecordMillis = srcLiveMillis;
  normalizeLastHeardBrowseState();
}

void saveActiveHeardCache() {
  if (currentWatchSource == WATCH_SVXLINK) copyCurrentHeardStateTo(svxlinkHeardList, svxlinkHeardCount, svxlinkLiveRecord, svxlinkLiveRecordMillis);
  else if (currentWatchSource == WATCH_MMDVM) copyCurrentHeardStateTo(mmdvmHeardList, mmdvmHeardCount, mmdvmLiveRecord, mmdvmLiveRecordMillis);
}

void loadActiveHeardCache() {
  if (currentWatchSource == WATCH_SVXLINK) restoreHeardStateFrom(svxlinkHeardList, svxlinkHeardCount, svxlinkLiveRecord, svxlinkLiveRecordMillis);
  else if (currentWatchSource == WATCH_MMDVM) restoreHeardStateFrom(mmdvmHeardList, mmdvmHeardCount, mmdvmLiveRecord, mmdvmLiveRecordMillis);
}

void setCurrentWatchSource(WatchSource source) {
  if (source == WATCH_SVXLINK && !svxlinkFeatureEnabled()) source = WATCH_MMDVM;
  if (source == WATCH_APRS && !aprsFeatureEnabled()) source = WATCH_MMDVM;
  if (source == WATCH_WEATHER && !weatherFeatureEnabled()) source = WATCH_MMDVM;
  if (source == WATCH_HAMQSL && !hamqslFeatureEnabled()) source = WATCH_MMDVM;
  if (source == currentWatchSource) {
    loadActiveHeardCache();
    return;
  }
  saveActiveHeardCache();
  currentWatchSource = source;
  loadActiveHeardCache();
  if (currentWatchSource == WATCH_APRS || currentWatchSource == WATCH_WEATHER || currentWatchSource == WATCH_HAMQSL) currentPage = PAGE_LIVE;
  lastPoll = 0;
  nextFetchHeard = false;
  requestRedraw();
}

void applyStartPageAfterBoot() {
  String choice = normalizedStartPageAfterBoot(true);

  if (choice == "svxlink_live") {
    setCurrentWatchSource(WATCH_SVXLINK);
    currentPage = PAGE_LIVE;
  } else if (choice == "svxlink_last") {
    setCurrentWatchSource(WATCH_SVXLINK);
    currentPage = PAGE_LAST;
  } else if (choice == "aprs_live") {
    setCurrentWatchSource(WATCH_APRS);
    currentPage = PAGE_LIVE;
  } else if (choice == "weather_live") {
    setCurrentWatchSource(WATCH_WEATHER);
    currentPage = PAGE_LIVE;
  } else if (choice == "hamqsl_live") {
    setCurrentWatchSource(WATCH_HAMQSL);
    currentPage = PAGE_LIVE;
  } else if (choice == "mmdvm_last") {
    setCurrentWatchSource(WATCH_MMDVM);
    currentPage = PAGE_LAST;
  } else {
    setCurrentWatchSource(WATCH_MMDVM);
    currentPage = PAGE_LIVE;
  }

  previousPage = currentPage;
  addWebLog("INFO", "BOOT", "Start page after boot: " + startPageAfterBootLabel(choice) + ".");
  requestRedraw();
}


bool heardListsEqual(const HeardRecord* a, int aCount, const HeardRecord* b, int bCount) {
  if (aCount != bCount) return false;
  for (int i = 0; i < aCount; i++) {
    if (!sameRecord(a[i], b[i])) return false;
  }
  return true;
}

String firstNonEmpty(const String& a, const String& b, const String& c, const String& d, const String& e, const String& f) {
  if (trimSafe(a).length()) return trimSafe(a);
  if (trimSafe(b).length()) return trimSafe(b);
  if (trimSafe(c).length()) return trimSafe(c);
  if (trimSafe(d).length()) return trimSafe(d);
  if (trimSafe(e).length()) return trimSafe(e);
  return trimSafe(f);
}

String prettyMetricValue(const String& value, const char* suffix, bool keepPercent) {
  String out = trimSafe(value);
  if (!out.length()) return "-";
  if (out.equalsIgnoreCase("n/a") || out.equalsIgnoreCase("nan") || out.equalsIgnoreCase("null")) return "-";
  if (!keepPercent) out.replace("%", "");
  if (suffix && strlen(suffix) > 0) {
    String suf = String(suffix);
    if (!out.endsWith(suf)) out += suf;
  }
  return out;
}


String uppercaseTrimmed(String value) {
  value = trimSafe(value);
  value.toUpperCase();
  return value;
}

String htmlEntityDecode(String value) {
  value.replace("&amp;", "&");
  value.replace("&quot;", "\"");
  value.replace("&#39;", "'");
  value.replace("&apos;", "'");
  value.replace("&nbsp;", " ");
  value.replace("&ndash;", "-");
  value.replace("&mdash;", "-");
  value.replace("&lt;", "<");
  value.replace("&gt;", ">");
  return trimSafe(value);
}

String stripHtmlTags(String html) {
  String out = "";
  out.reserve(html.length());
  bool inTag = false;
  bool pendingSpace = false;

  for (int i = 0; i < (int)html.length(); ++i) {
    char c = html.charAt(i);
    if (c == '<') {
      inTag = true;
      pendingSpace = true;
      continue;
    }
    if (c == '>') {
      inTag = false;
      pendingSpace = true;
      continue;
    }
    if (inTag) continue;

    if (c == '\r' || c == '\n' || c == '\t' || c == ' ') {
      pendingSpace = true;
      continue;
    }
    if (pendingSpace && out.length()) out += ' ';
    out += c;
    pendingSpace = false;
  }

  return htmlEntityDecode(out);
}

String targetDigitsOnly(const String& value) {
  String out = "";
  for (int i = 0; i < (int)value.length(); ++i) {
    char c = value.charAt(i);
    if (c >= '0' && c <= '9') out += c;
  }
  return out;
}

String trimLeadingZeros(String value) {
  value = trimSafe(value);
  int pos = 0;
  while (pos < (int)value.length() - 1 && value.charAt(pos) == '0') pos++;
  return value.substring(pos);
}

String normalizeW0chpMode(const String& rawMode) {
  String mode = normalizeModeName(rawMode);
  if (mode == "D-STAR") mode = "DSTAR";
  if (mode == "FUSION") mode = "YSF";
  if (mode == "DMR" || mode == "DSTAR" || mode == "YSF" || mode == "P25" || mode == "NXDN") return mode;
  return "";
}

bool targetListLookupSupported(const String& mode) {
#if ENABLE_TARGET_LIST_SD_LOOKUP
  return mode == "DMR" || mode == "DSTAR" || mode == "YSF" || mode == "P25" || mode == "NXDN";
#else
  (void)mode;
  return false;
#endif
}

String normalizeDstarReflectorKey(String value) {
  value = uppercaseTrimmed(value);
  if (!value.length()) return "";

  if (!value.startsWith("XLX")) return "";
  value = value.substring(3);

  String digits = targetDigitsOnly(value);
  if (!digits.length()) return "";
  while (digits.length() < 3) digits = "0" + digits;
  if (digits.length() > 3) digits = digits.substring(0, 3);
  return "XLX" + digits;
}

String normalizeTargetLookupKey(const String& mode, const String& target) {
  String m = normalizeW0chpMode(mode);
  String t = trimSafe(target);
  if (!m.length() || !t.length()) return "";

  if (m == "DSTAR") return normalizeDstarReflectorKey(t);

  String digits = targetDigitsOnly(t);
  if (!digits.length()) return "";
  return trimLeadingZeros(digits);
}

bool targetNumericCellMatches(const String& cell, const String& key) {
  String digits = trimLeadingZeros(targetDigitsOnly(cell));
  return digits.length() && key.length() && digits == key;
}

bool targetDstarCellMatches(const String& cell, const String& key) {
  return normalizeDstarReflectorKey(cell) == key;
}

const char* targetListDefaultPathForMode(const String& mode) {
  if (mode == "DMR")   return TARGET_LIST_DMR_PATH_DEFAULT;
  if (mode == "DSTAR") return TARGET_LIST_DSTAR_PATH_DEFAULT;
  if (mode == "YSF")   return TARGET_LIST_YSF_PATH_DEFAULT;
  if (mode == "P25")   return TARGET_LIST_P25_PATH_DEFAULT;
  if (mode == "NXDN")  return TARGET_LIST_NXDN_PATH_DEFAULT;
  return "";
}

String targetListOverrideUrlForMode(const String& mode) {
  if (mode == "DMR")   return trimSafe(cfg.targetListDmrUrl);
  if (mode == "DSTAR") return trimSafe(cfg.targetListDstarUrl);
  if (mode == "YSF")   return trimSafe(cfg.targetListYsfUrl);
  if (mode == "P25")   return trimSafe(cfg.targetListP25Url);
  if (mode == "NXDN")  return trimSafe(cfg.targetListNxdnUrl);
  return "";
}

String targetListUrlForMode(const String& mode) {
  String m = normalizeW0chpMode(mode);
  if (!targetListLookupSupported(m)) return "";

  String url = targetListOverrideUrlForMode(m);
  if (url.length()) return url;

  String base = trimSafe(cfg.targetListBaseUrl);
  if (!base.length()) base = String(TARGET_LIST_BASE_URL_DEFAULT);

  String path = String(targetListDefaultPathForMode(m));
  String lowerMode = m;
  lowerMode.toLowerCase();

  if (base.indexOf("%MODE%") >= 0 || base.indexOf("%PATH%") >= 0) {
    base.replace("%MODE%", lowerMode);
    base.replace("%PATH%", path);
    return base;
  }

  if (!base.endsWith("/")) base += "/";
  return base + path;
}

String targetListModeFieldName(const String& mode) {
  if (mode == "DMR") return "target_list_dmr_url";
  if (mode == "DSTAR") return "target_list_dstar_url";
  if (mode == "YSF") return "target_list_ysf_url";
  if (mode == "P25") return "target_list_p25_url";
  if (mode == "NXDN") return "target_list_nxdn_url";
  return "";
}

String targetListFileSizeText(const String& mode) {
  const char* path = targetListFilePathForMode(mode);
  if (!path || !strlen(path) || !storageExists(path)) return "Missing";
  File f = storageOpen(path, FILE_READ);
  if (!f) return "Unreadable";
  size_t bytes = f.size();
  f.close();
  if (bytes >= 1048576UL) return String((float)bytes / 1048576.0f, 2) + " MB";
  if (bytes >= 1024UL) return String((float)bytes / 1024.0f, 1) + " KB";
  return String((uint32_t)bytes) + " bytes";
}

const char* targetListFilePathForMode(const String& mode) {
  if (mode == "DMR")   return "/lookup/dmr.txt";
  if (mode == "DSTAR") return "/lookup/dstar.txt";
  if (mode == "YSF")   return "/lookup/ysf.txt";
  if (mode == "P25")   return "/lookup/p25.txt";
  if (mode == "NXDN")  return "/lookup/nxdn.txt";
  return "";
}

const char* targetListTempFilePathForMode(const String& mode) {
  if (mode == "DMR")   return "/lookup/dmr.tmp";
  if (mode == "DSTAR") return "/lookup/dstar.tmp";
  if (mode == "YSF")   return "/lookup/ysf.tmp";
  if (mode == "P25")   return "/lookup/p25.tmp";
  if (mode == "NXDN")  return "/lookup/nxdn.tmp";
  return "";
}

const char* targetListRawTempFilePathForMode(const String& mode) {
  if (mode == "DMR")   return "/lookup/dmr.raw";
  if (mode == "DSTAR") return "/lookup/dstar.raw";
  if (mode == "YSF")   return "/lookup/ysf.raw";
  if (mode == "P25")   return "/lookup/p25.raw";
  if (mode == "NXDN")  return "/lookup/nxdn.raw";
  return "";
}

int targetNameLookupCacheIndex(const String& mode, const String& key) {
  for (int i = 0; i < TARGET_LIST_LOOKUP_CACHE_SIZE; ++i) {
    if (targetNameLookupCache[i].valid && targetNameLookupCache[i].mode == mode && targetNameLookupCache[i].target == key) return i;
  }
  return -1;
}

struct TargetNameLookupCacheEntry* targetNameLookupCacheSlot(const String& mode, const String& key, bool createIfMissing) {
  int idx = targetNameLookupCacheIndex(mode, key);
  if (idx >= 0) return &targetNameLookupCache[idx];
  if (!createIfMissing) return nullptr;

  int slot = targetNameLookupCacheWriteIndex++ % TARGET_LIST_LOOKUP_CACHE_SIZE;
  targetNameLookupCache[slot] = TargetNameLookupCacheEntry();
  targetNameLookupCache[slot].mode = mode;
  targetNameLookupCache[slot].target = key;
  targetNameLookupCache[slot].valid = true;
  return &targetNameLookupCache[slot];
}

void clearTargetNameLookupCache() {
  for (int i = 0; i < TARGET_LIST_LOOKUP_CACHE_SIZE; ++i) targetNameLookupCache[i] = TargetNameLookupCacheEntry();
  targetNameLookupCacheWriteIndex = 0;
}

bool extractHtmlCellsFromRow(const String& row, String* cells, int maxCells, int& cellCount) {
  cellCount = 0;
  int pos = 0;
  while (pos >= 0 && pos < (int)row.length() && cellCount < maxCells) {
    int td = row.indexOf("<td", pos);
    int th = row.indexOf("<th", pos);
    int start = -1;
    bool header = false;

    if (td >= 0 && (th < 0 || td < th)) start = td;
    else if (th >= 0) { start = th; header = true; }
    else break;

    int gt = row.indexOf('>', start);
    if (gt < 0) break;
    String closeTag = header ? "</th>" : "</td>";
    int end = row.indexOf(closeTag, gt + 1);
    if (end < 0) break;

    String cell = stripHtmlTags(row.substring(gt + 1, end));
    if (cell.length()) cells[cellCount++] = cell;
    pos = end + closeTag.length();
  }
  return cellCount > 0;
}

String sanitizeTargetListName(String name) {
  name = htmlEntityDecode(name);
  name.replace("\r", " ");
  name.replace("\n", " ");
  name.replace("\t", " ");
  name.replace("|", "-");
  while (name.indexOf("  ") >= 0) name.replace("  ", " ");
  return trimSafe(name);
}

bool writeTargetListLine(Print& out, const String& mode, const String& rawKey, const String& rawName, int& writtenCount) {
  String key = normalizeTargetLookupKey(mode, rawKey);
  String name = sanitizeTargetListName(rawName);
  if (!key.length() || !name.length()) return false;

  out.print(key);
  out.print('|');
  out.println(name);
  writtenCount++;
  return true;
}

void writeTargetListEntriesFromCells(Print& out, const String& mode, String* cells, int cellCount, int& writtenCount) {
  if (cellCount < 2) return;

  if (mode == "DSTAR") {
    writeTargetListLine(out, mode, cells[0], cells[1], writtenCount);
    return;
  }

  if (mode == "YSF") {
    writeTargetListLine(out, mode, cells[0], cells[1], writtenCount);
    if (cellCount >= 6) writeTargetListLine(out, mode, cells[5], cells[1], writtenCount);
    return;
  }

  writeTargetListLine(out, mode, cells[0], cells[1], writtenCount);
}

bool writeTargetListFileFromW0chpHtml(const String& mode, const String& payload, const char* finalPath, const char* tmpPath, int& entryCount) {
  entryCount = 0;
  if (!ensureLookupDirectory()) return false;
  storageRemove(tmpPath);

  File out = storageOpen(tmpPath, FILE_WRITE);
  if (!out) return false;

  out.println("# MMDVM Display target-name list");
  out.println("# Format: target|name");
  out.println(String("# Mode: ") + mode);

  int pos = 0;
  while (pos >= 0 && pos < (int)payload.length()) {
    int rowStart = payload.indexOf("<tr", pos);
    if (rowStart < 0) break;
    int rowBodyStart = payload.indexOf('>', rowStart);
    if (rowBodyStart < 0) break;
    int rowEnd = payload.indexOf("</tr>", rowBodyStart + 1);
    if (rowEnd < 0) break;

    String row = payload.substring(rowBodyStart + 1, rowEnd);
    String cells[10];
    int cellCount = 0;
    if (extractHtmlCellsFromRow(row, cells, 10, cellCount)) {
      writeTargetListEntriesFromCells(out, mode, cells, cellCount, entryCount);
    }
    pos = rowEnd + 5;
  }

  out.close();

  if (entryCount <= 0) {
    storageRemove(tmpPath);
    return false;
  }

  storageRemove(finalPath);
  if (!storageRename(tmpPath, finalPath)) {
    storageRemove(tmpPath);
    return false;
  }
  return true;
}

bool writeTargetListFileFromPlainText(const String& mode, const String& payload, const char* finalPath, const char* tmpPath, int& entryCount) {
  entryCount = 0;
  if (!ensureLookupDirectory()) return false;
  storageRemove(tmpPath);

  File out = storageOpen(tmpPath, FILE_WRITE);
  if (!out) return false;

  out.println("# MMDVM Display target-name list");
  out.println("# Format: target|name");
  out.println(String("# Mode: ") + mode);

  int pos = 0;
  while (pos <= (int)payload.length()) {
    int end = payload.indexOf('\n', pos);
    if (end < 0) end = payload.length();
    String line = payload.substring(pos, end);
    String keyPart, namePart;
    if (splitTargetListLine(line, keyPart, namePart)) {
      writeTargetListLine(out, mode, keyPart, namePart, entryCount);
    }
    pos = end + 1;
    if (end >= (int)payload.length()) break;
  }

  out.close();

  if (entryCount <= 0) {
    storageRemove(tmpPath);
    return false;
  }

  storageRemove(finalPath);
  if (!storageRename(tmpPath, finalPath)) {
    storageRemove(tmpPath);
    return false;
  }
  return true;
}


bool httpDownloadToSdFile(const String& url, const char* path, int& codeOut, size_t& bytesOut, String* errorOut) {
  codeOut = 0;
  bytesOut = 0;
  if (errorOut) *errorOut = "";

  if (!path || !strlen(path)) {
    if (errorOut) *errorOut = "No temporary file path is configured.";
    return false;
  }
  if (!ensureLookupDirectory()) {
    if (errorOut) *errorOut = "Could not create /lookup on the SD card.";
    return false;
  }

  storageRemove(path);
  File out = storageOpen(path, FILE_WRITE);
  if (!out) {
    if (errorOut) *errorOut = String("Could not open ") + path + " for writing.";
    return false;
  }

  NetworkGuard net(NETWORK_LOCK_TIMEOUT_MS);
  if (!net) {
    out.close();
    storageRemove(path);
    codeOut = -98;
    if (errorOut) *errorOut = "Network is busy.";
    return false;
  }

  HTTPClient http;
  http.setConnectTimeout(HTTP_CONNECT_TIMEOUT_MS);
  http.setTimeout(HTTP_READ_TIMEOUT_MS);
  http.useHTTP10(true);
  http.setReuse(false);

  bool began = false;
  String lowerUrl = trimSafe(url);
  lowerUrl.toLowerCase();

  WiFiClient plainClient;
  WiFiClientSecure secureClient;
  if (lowerUrl.startsWith("https://")) {
    secureClient.setInsecure();
    began = http.begin(secureClient, url);
  } else {
    began = http.begin(plainClient, url);
  }

  if (!began) {
    out.close();
    storageRemove(path);
    codeOut = -1;
    plainClient.stop();
    secureClient.stop();
    if (errorOut) *errorOut = "HTTP begin failed.";
    return false;
  }

  http.setUserAgent(String("MMDVM-Display/") + DISPLAY_VERSION + " (operator=" + getDeviceCallsign() + ")");
  http.addHeader("Connection", "close");
  http.addHeader("Accept", "text/html,text/plain,*/*");

  codeOut = http.GET();
  if (codeOut != 200) {
    http.end();
    plainClient.stop();
    secureClient.stop();
    out.close();
    storageRemove(path);
    if (errorOut) *errorOut = String("HTTP ") + String(codeOut);
    return false;
  }

  Stream* stream = http.getStreamPtr();
  if (!stream) {
    http.end();
    plainClient.stop();
    secureClient.stop();
    out.close();
    storageRemove(path);
    if (errorOut) *errorOut = "HTTP stream unavailable.";
    return false;
  }
  int remaining = http.getSize();
  uint8_t buf[512];
  unsigned long lastProgress = millis();

  while (http.connected() && (remaining > 0 || remaining == -1)) {
    size_t avail = stream->available();
    if (avail) {
      int toRead = (int)((avail > sizeof(buf)) ? sizeof(buf) : avail);
      int readCount = stream->readBytes(buf, toRead);
      if (readCount > 0) {
        size_t written = out.write(buf, readCount);
        if (written != (size_t)readCount) {
          http.end();
          plainClient.stop();
          secureClient.stop();
          out.close();
          storageRemove(path);
          if (errorOut) *errorOut = "SD write failed while downloading.";
          return false;
        }
        bytesOut += (size_t)readCount;
        if (remaining > 0) remaining -= readCount;
        lastProgress = millis();
      }
    } else {
      if (millis() - lastProgress > (unsigned long)HTTP_READ_TIMEOUT_MS + 1000UL) break;
      delay(1);
    }
  }

  http.end();
  plainClient.stop();
  secureClient.stop();
  out.close();

  if (bytesOut == 0) {
    storageRemove(path);
    if (errorOut) *errorOut = "HTTP 200 but no response body was received.";
    return false;
  }
  return true;
}

void processTargetListHtmlRows(Print& out, const String& mode, String& buffer, int& entryCount, bool finalFlush) {
  while (buffer.length()) {
    String lower = buffer;
    lower.toLowerCase();

    int rowStart = lower.indexOf("<tr");
    if (rowStart < 0) {
      if (finalFlush) buffer = "";
      else if (buffer.length() > 32) buffer = buffer.substring(buffer.length() - 32);
      break;
    }

    if (rowStart > 0) {
      buffer.remove(0, rowStart);
      continue;
    }

    int rowBodyStart = lower.indexOf('>');
    if (rowBodyStart < 0) {
      if (buffer.length() > 8192) buffer = buffer.substring(buffer.length() - 32);
      break;
    }

    int rowEnd = lower.indexOf("</tr>", rowBodyStart + 1);
    if (rowEnd < 0) {
      if (buffer.length() > 16384) buffer = buffer.substring(buffer.length() - 32);
      break;
    }

    String row = buffer.substring(rowBodyStart + 1, rowEnd);
    String cells[10];
    int cellCount = 0;
    if (extractHtmlCellsFromRow(row, cells, 10, cellCount)) {
      writeTargetListEntriesFromCells(out, mode, cells, cellCount, entryCount);
    }
    buffer.remove(0, rowEnd + 5);
  }
}

bool writeTargetListFileFromW0chpHtmlFile(const String& mode, const char* rawPath, const char* finalPath, const char* tmpPath, int& entryCount) {
  entryCount = 0;
  if (!rawPath || !strlen(rawPath) || !ensureLookupDirectory()) return false;
  storageRemove(tmpPath);

  File in = storageOpen(rawPath, FILE_READ);
  if (!in) return false;

  File out = storageOpen(tmpPath, FILE_WRITE);
  if (!out) {
    in.close();
    return false;
  }

  out.println("# MMDVM Display target-name list");
  out.println("# Format: target|name");
  out.println(String("# Mode: ") + mode);

  String buffer;
  buffer.reserve(4096);
  char chunk[257];
  while (in.available()) {
    size_t n = in.readBytes(chunk, 256);
    if (n == 0) break;
    chunk[n] = 0;
    buffer += chunk;
    processTargetListHtmlRows(out, mode, buffer, entryCount, false);
  }
  processTargetListHtmlRows(out, mode, buffer, entryCount, true);

  in.close();
  out.close();

  if (entryCount <= 0) {
    storageRemove(tmpPath);
    return false;
  }

  storageRemove(finalPath);
  if (!storageRename(tmpPath, finalPath)) {
    storageRemove(tmpPath);
    return false;
  }
  return true;
}

bool writeTargetListFileFromPlainTextFile(const String& mode, const char* rawPath, const char* finalPath, const char* tmpPath, int& entryCount) {
  entryCount = 0;
  if (!rawPath || !strlen(rawPath) || !ensureLookupDirectory()) return false;
  storageRemove(tmpPath);

  File in = storageOpen(rawPath, FILE_READ);
  if (!in) return false;

  File out = storageOpen(tmpPath, FILE_WRITE);
  if (!out) {
    in.close();
    return false;
  }

  out.println("# MMDVM Display target-name list");
  out.println("# Format: target|name");
  out.println(String("# Mode: ") + mode);

  while (in.available()) {
    String line = in.readStringUntil('\n');
    String keyPart, namePart;
    if (splitTargetListLine(line, keyPart, namePart)) {
      writeTargetListLine(out, mode, keyPart, namePart, entryCount);
    }
  }

  in.close();
  out.close();

  if (entryCount <= 0) {
    storageRemove(tmpPath);
    return false;
  }

  storageRemove(finalPath);
  if (!storageRename(tmpPath, finalPath)) {
    storageRemove(tmpPath);
    return false;
  }
  return true;
}

bool downloadTargetListForMode(const String& mode, String* detailOut) {
#if ENABLE_TARGET_LIST_SD_LOOKUP
  String m = normalizeW0chpMode(mode);
  String url = targetListUrlForMode(m);
  const char* finalPath = targetListFilePathForMode(m);
  const char* tmpPath = targetListTempFilePathForMode(m);
  const char* rawPath = targetListRawTempFilePathForMode(m);
  if (detailOut) *detailOut = "";
  if (!url.length() || !finalPath || !strlen(finalPath) || !tmpPath || !strlen(tmpPath) || !rawPath || !strlen(rawPath)) {
    if (detailOut) *detailOut = "No source URL or file path is configured.";
    return false;
  }

  int code = 0;
  size_t bytes = 0;
  String downloadError;
  bool downloaded = httpDownloadToSdFile(url, rawPath, code, bytes, &downloadError);
  if (!downloaded) {
    String msg = "Target list download failed for " + m + " (HTTP " + String(code) + ")";
    if (downloadError.length()) msg += ": " + downloadError;
    msg += ".";
    addWebLog("WARN", "LOOKUP", msg);
    if (detailOut) *detailOut = msg;
    return false;
  }

  int entries = 0;
  bool wrote = writeTargetListFileFromW0chpHtmlFile(m, rawPath, finalPath, tmpPath, entries);
  if (!wrote) {
    wrote = writeTargetListFileFromPlainTextFile(m, rawPath, finalPath, tmpPath, entries);
  }
  storageRemove(rawPath);

  if (!wrote) {
    String msg = "Could not convert target list for " + m + " to SD format. Downloaded " + String((uint32_t)bytes) + " bytes. Expected W0CHP HTML or text lines like 262|Germany.";
    addWebLog("WARN", "LOOKUP", msg);
    if (detailOut) *detailOut = msg;
    return false;
  }

  clearTargetNameLookupCache();
  String msg = "Saved " + String(entries) + " " + m + " target names to " + String(finalPath) + " from " + String((uint32_t)bytes) + " downloaded bytes.";
  addWebLog("INFO", "LOOKUP", msg);
  if (detailOut) *detailOut = msg;
  return true;
#else
  (void)mode;
  if (detailOut) *detailOut = "Target-list SD lookup is disabled at compile time.";
  return false;
#endif
}

bool splitTargetListLine(const String& line, String& keyPart, String& namePart) {
  String s = trimSafe(line);
  if (!s.length() || s.startsWith("#")) return false;

  int sep = s.indexOf('|');
  if (sep < 0) sep = s.indexOf('\t');
  if (sep < 0) sep = s.indexOf(';');
  if (sep < 0) sep = s.indexOf(',');
  if (sep < 0) {
    int p = 0;
    while (p < (int)s.length() && !isSpace((unsigned char)s.charAt(p))) p++;
    if (p <= 0 || p >= (int)s.length()) return false;
    sep = p;
  }

  keyPart = trimSafe(s.substring(0, sep));
  namePart = trimSafe(s.substring(sep + 1));
  return keyPart.length() && namePart.length();
}

bool lookupTargetNameInSdFile(const String& mode, const String& key, String& outName) {
#if ENABLE_TARGET_LIST_SD_LOOKUP
  outName = "";
  const char* path = targetListFilePathForMode(mode);
  if (!path || !strlen(path) || !storageExists(path)) return false;

  File f = storageOpen(path, FILE_READ);
  if (!f) return false;

  while (f.available()) {
    String line = f.readStringUntil('\n');
    String keyPart, namePart;
    if (!splitTargetListLine(line, keyPart, namePart)) continue;

    String lineKey = normalizeTargetLookupKey(mode, keyPart);
    if (lineKey.length() && lineKey == key) {
      outName = sanitizeTargetListName(namePart);
      f.close();
      return outName.length();
    }
  }

  f.close();
  return false;
#else
  (void)mode; (void)key; (void)outName;
  return false;
#endif
}

String targetNameLookupCached(const String& mode, const String& target) {
  if (!cfg.targetListLookupEnabled) return "";
  String m = normalizeW0chpMode(mode);
  String key = normalizeTargetLookupKey(m, target);
  if (!targetListLookupSupported(m) || !key.length()) return "";

  TargetNameLookupCacheEntry* entry = targetNameLookupCacheSlot(m, key, false);
  if (entry && entry->valid) return entry->found ? trimSafe(entry->name) : String("");

  String name;
  bool found = lookupTargetNameInSdFile(m, key, name);
  TargetNameLookupCacheEntry* slot = targetNameLookupCacheSlot(m, key, true);
  if (slot) {
    slot->mode = m;
    slot->target = key;
    slot->name = found ? name : String("");
    slot->valid = true;
    slot->found = found;
    slot->lastFetchMillis = millis();
  }
  return found ? name : String("");
}

bool targetListMissingForAnyMode() {
  const char* modes[] = { "DMR", "DSTAR", "YSF", "P25", "NXDN" };
  for (uint8_t i = 0; i < 5; ++i) {
    const char* path = targetListFilePathForMode(String(modes[i]));
    if (!path || !strlen(path) || !storageExists(path)) return true;
  }
  return false;
}

void processTargetNameLookups() {
#if ENABLE_TARGET_LIST_SD_LOOKUP && ENABLE_TARGET_LIST_AUTO_DOWNLOAD
  if (WiFi.status() != WL_CONNECTED) return;

  unsigned long now = millis();
  if (lastTargetListDownloadFailureMillis && now - lastTargetListDownloadFailureMillis < TARGET_LIST_AUTO_RETRY_MS) return;
  if (now - lastTargetListDownloadMillis < TARGET_LIST_DOWNLOAD_MIN_INTERVAL_MS) return;

  bool shouldDownload = targetListForceUpdateRequested || (!targetListBootUpdateFinished && (TARGET_LIST_REFRESH_EVERY_BOOT || targetListMissingForAnyMode()));
  if (!shouldDownload) {
    targetListBootUpdateFinished = true;
    return;
  }

  const char* modes[] = { "DMR", "DSTAR", "YSF", "P25", "NXDN" };
  for (uint8_t checked = 0; checked < 5; ++checked) {
    String mode = String(modes[targetListDownloadIndex % 5]);
    targetListDownloadIndex = (targetListDownloadIndex + 1) % 5;
    const char* path = targetListFilePathForMode(mode);
    if (!targetListForceUpdateRequested && path && strlen(path) && storageExists(path)) continue;

    lastTargetListDownloadMillis = now;
    bool ok = downloadTargetListForMode(mode);
    if (!ok) lastTargetListDownloadFailureMillis = now;

    if (!targetListForceUpdateRequested && !targetListMissingForAnyMode()) targetListBootUpdateFinished = true;
    if (targetListForceUpdateRequested && targetListDownloadIndex == 0) {
      targetListForceUpdateRequested = false;
      targetListBootUpdateFinished = true;
    }
    return;
  }

  targetListForceUpdateRequested = false;
  targetListBootUpdateFinished = true;
#endif
}

String lookupCommonTalkgroupName(const String& target) {
  String tg = trimSafe(target);
  if (!tg.length()) return "";
  if (tg == "9") return "Local";
  if (tg == "91") return "World-Wide";
  if (tg == "92") return "Europe";
  if (tg == "262") return "Germany";
  if (tg == "9990") return "Parrot";
  return "";
}

String resolveTargetLabel(const HeardRecord& rec) {
  String mode = normalizeModeName(rec.mode);
  if (mode == "DMR") return "TG";
  if (mode == "DSTAR" || mode == "D-STAR") return "REF";
  if (mode == "YSF" || mode == "FUSION") return "";
  if (mode == "P25" || mode == "NXDN") return "NET";
  if (mode == "SVXLINK") return "";
  return "DST";
}

bool targetPrefixSeparator(char c) {
  return c == ' ' || c == ':' || c == '-' || c == '#' || c == '\t';
}

String stripTargetLabelPrefix(String value, const String& label) {
  value = trimSafe(value);
  String cleanLabel = trimSafe(label);
  if (!value.length() || !cleanLabel.length()) return value;

  String upperValue = value;
  String upperLabel = cleanLabel;
  upperValue.toUpperCase();
  upperLabel.toUpperCase();

  if (upperValue == upperLabel) return "";
  if (!upperValue.startsWith(upperLabel)) return value;

  int pos = cleanLabel.length();
  if (pos >= (int)value.length()) return "";

  char nextChar = value.charAt(pos);
  if (targetPrefixSeparator(nextChar)) {
    while (pos < (int)value.length() && targetPrefixSeparator(value.charAt(pos))) pos++;
  } else if (!(nextChar >= '0' && nextChar <= '9')) {
    return value;
  }

  return trimSafe(value.substring(pos));
}

String cleanSvxlinkTargetValue(String value) {
  value = trimSafe(value);
  if (!value.length()) return value;

  value = stripTargetLabelPrefix(value, "DST TG");
  value = stripTargetLabelPrefix(value, "DST");
  value = stripTargetLabelPrefix(value, "TG");
  return trimSafe(value);
}

String cleanTargetValueForMode(String value, const String& mode) {
  value = trimSafe(value);
  if (!value.length()) return value;

  if (mode == "DMR") return stripTargetLabelPrefix(value, "TG");
  if (mode == "DSTAR" || mode == "D-STAR") return stripTargetLabelPrefix(value, "REF");
  if (mode == "YSF" || mode == "FUSION") return stripTargetLabelPrefix(value, "ROOM");
  if (mode == "P25" || mode == "NXDN") return stripTargetLabelPrefix(value, "NET");
  if (mode == "SVXLINK") return cleanSvxlinkTargetValue(value);
  return stripTargetLabelPrefix(value, "DST");
}

String joinTargetDisplay(const String& label, const String& target, const String& name) {
  String cleanLabel = trimSafe(label);
  String cleanTarget = trimSafe(target);
  String cleanName = trimSafe(name);

  String upperTarget = cleanTarget;
  String upperName = cleanName;
  upperTarget.toUpperCase();
  upperName.toUpperCase();
  if (upperTarget.length() && upperName == upperTarget) cleanName = "";

  String out = "";

  if (cleanLabel.length()) out += cleanLabel;
  if (cleanTarget.length()) {
    if (out.length()) out += " ";
    out += cleanTarget;
  }
  if (cleanName.length()) {
    if (out.length()) out += " ";
    out += cleanName;
  }
  return out.length() ? out : String("-");
}

String resolveTargetDisplay(const HeardRecord& rec) {
  String mode = normalizeModeName(rec.mode);
  String label = resolveTargetLabel(rec);
  String rawTarget = cleanTargetValueForMode(rec.target, mode);
  String explicitName = cleanTargetValueForMode(rec.targetName, mode);

  if (mode == "SVXLINK") {
    if (explicitName.length()) return explicitName;
    return rawTarget.length() ? rawTarget : String("-");
  }

  if (!explicitName.length()) {
    String cachedName = targetNameLookupCached(mode, rawTarget);
    if (cachedName.length()) explicitName = cachedName;
  }

  if (mode == "DMR") {
    String mapped = lookupCommonTalkgroupName(rawTarget);
    if (!explicitName.length() && mapped.length()) explicitName = mapped;
    return joinTargetDisplay(label, rawTarget, explicitName);
  }

  return joinTargetDisplay(label, rawTarget, explicitName);
}


String normalizeDmrTimeslot(String raw) {
  raw = trimSafe(raw);
  if (!raw.length()) return "";

  String upper = raw;
  upper.toUpperCase();
  upper.replace("TIME SLOT", "");
  upper.replace("TIMESLOT", "");
  upper.replace("SLOT", "");
  upper.replace("TS", "");
  upper.replace("#", "");
  upper.replace(":", "");
  upper.replace("-", "");
  upper.trim();

  if (upper == "1" || upper == "01") return "1";
  if (upper == "2" || upper == "02") return "2";
  return "";
}

String extractDmrTimeslotFromText(const String& text) {
  String upper = trimSafe(text);
  if (!upper.length()) return "";
  upper.toUpperCase();
  upper.replace("_", " ");
  upper.replace("/", " ");
  upper.replace(":", " ");
  upper.replace("-", " ");
  while (upper.indexOf("  ") >= 0) upper.replace("  ", " ");
  upper = " " + upper + " ";

  if (upper.indexOf(" TS1 ") >= 0 || upper.indexOf(" TS 1 ") >= 0 ||
      upper.indexOf(" SLOT1 ") >= 0 || upper.indexOf(" SLOT 1 ") >= 0 ||
      upper.indexOf(" TIME SLOT 1 ") >= 0 || upper.indexOf(" TIMESLOT 1 ") >= 0) return "1";

  if (upper.indexOf(" TS2 ") >= 0 || upper.indexOf(" TS 2 ") >= 0 ||
      upper.indexOf(" SLOT2 ") >= 0 || upper.indexOf(" SLOT 2 ") >= 0 ||
      upper.indexOf(" TIME SLOT 2 ") >= 0 || upper.indexOf(" TIMESLOT 2 ") >= 0) return "2";

  return "";
}

String dmrTimeslotLabel(const HeardRecord& rec) {
  if (normalizeModeName(rec.mode) != "DMR") return "";
  String ts = normalizeDmrTimeslot(rec.timeslot);
  return ts.length() ? String("TS") + ts : String("");
}

const HeardRecord& dmrTimeslotRecord(uint8_t slot) {
  return slot == 2 ? dmrTs2LiveRecord : dmrTs1LiveRecord;
}

unsigned long dmrTimeslotRecordMillis(uint8_t slot) {
  return slot == 2 ? dmrTs2LiveRecordMillis : dmrTs1LiveRecordMillis;
}

bool isDmrTimeslotLiveActive(uint8_t slot) {
  const HeardRecord& rec = dmrTimeslotRecord(slot);
  unsigned long tsMillis = dmrTimeslotRecordMillis(slot);
  if (!rec.valid || tsMillis == 0) return false;
  return (millis() - tsMillis) <= cfg.callsignHoldMs;
}

bool shouldShowDmrTimeslotLivePanel(bool active, const HeardRecord& rec) {
  (void)active;
  (void)rec;
  if (isSvxlinkBackend()) return false;

  return isDmrTimeslotLiveActive(1) && isDmrTimeslotLiveActive(2);
}

void clearDmrTimeslotLiveRecords() {
  dmrTs1LiveRecord = HeardRecord();
  dmrTs1LiveRecordMillis = 0;
  dmrTs2LiveRecord = HeardRecord();
  dmrTs2LiveRecordMillis = 0;
}

void noteDmrTimeslotLiveRecord(uint8_t slot, const HeardRecord& rec) {
  HeardRecord& dst = (slot == 2) ? dmrTs2LiveRecord : dmrTs1LiveRecord;
  unsigned long& dstMillis = (slot == 2) ? dmrTs2LiveRecordMillis : dmrTs1LiveRecordMillis;

  if (!sameRecord(dst, rec) || dstMillis == 0) {
    dst = rec;
    dstMillis = millis();
  }
}

void updateDmrTimeslotLiveRecordsFromList(const HeardRecord* list, int count) {
  bool foundTs1 = false;
  bool foundTs2 = false;

  for (int i = 0; i < count; i++) {
    const HeardRecord& rec = list[i];
    if (!rec.valid || normalizeModeName(rec.mode) != "DMR") continue;

    String ts = normalizeDmrTimeslot(rec.timeslot);
    if (ts == "1" && !foundTs1) {
      noteDmrTimeslotLiveRecord(1, rec);
      foundTs1 = true;
    } else if (ts == "2" && !foundTs2) {
      noteDmrTimeslotLiveRecord(2, rec);
      foundTs2 = true;
    }

    if (foundTs1 && foundTs2) break;
  }
}

void updateSessionInfoFromRecord(const HeardRecord& rec) {
  if (!rec.valid) return;
  sessionInfo.totalLiveHits++;
  sessionInfo.totalQsos++;
  String mode = normalizeModeName(rec.mode);
  if (mode == "DMR") sessionInfo.modeDMR++;
  else if (mode == "DSTAR" || mode == "D-STAR") sessionInfo.modeDSTAR++;
  else if (mode == "YSF" || mode == "FUSION") sessionInfo.modeYSF++;
  else if (mode == "P25") sessionInfo.modeP25++;
  else if (mode == "NXDN") sessionInfo.modeNXDN++;
  else if (mode == "POCSAG") sessionInfo.modePOCSAG++;

  String rssiText = trimSafe(rec.rssi);
  if (rssiText.length()) {
    int rssi = rssiText.toInt();
    if (rssi != 0 || rssiText == "0") {
      if (!sessionInfo.hasRssi) {
        sessionInfo.bestRssi = rssi;
        sessionInfo.worstRssi = rssi;
        sessionInfo.hasRssi = true;
      } else {
        if (rssi > sessionInfo.bestRssi) sessionInfo.bestRssi = rssi;
        if (rssi < sessionInfo.worstRssi) sessionInfo.worstRssi = rssi;
      }
    }
  }
}

bool parseHeardArray(const String& payload) {
#if DISPLAY_BACKEND_IS_DESKTOP
  return desktopParseHeardArrayRaw(payload);
#endif

  DynamicJsonDocument doc(32768);
  auto err = deserializeJson(doc, payload);
  if (err) {
    lastError = String("JSON: ") + err.c_str();
    return false;
  }

  JsonArray arr;
  if (doc.is<JsonArray>()) {
    arr = doc.as<JsonArray>();
  } else if (doc.is<JsonObject>()) {
    JsonObject root = doc.as<JsonObject>();
    parseSvxlinkStatusFromObject(root);
    parseModeStatusesFromObject(root);

    if (root["last_heard"].is<JsonArray>()) arr = root["last_heard"].as<JsonArray>();
    else if (root["traffic"].is<JsonArray>()) arr = root["traffic"].as<JsonArray>();
    else if (root["data"].is<JsonArray>()) arr = root["data"].as<JsonArray>();
    else if (root["results"].is<JsonArray>()) arr = root["results"].as<JsonArray>();
    else {
      lastError = "JSON object missing heard array";
      return false;
    }
  } else {
    lastError = "JSON root invalid";
    return false;
  }

  HeardRecord newList[MAX_HEARD];
  int newCount = 0;

  for (JsonObject obj : arr) {
    if (newCount >= MAX_HEARD) break;

    HeardRecord r;
    r.timeUtc   = normalizeHeardTimeDisplay(firstNonEmpty(String(obj["time_utc"] | ""), String(obj["timestamp"] | ""), String(obj["time"] | ""), String(obj["time_local"] | "")));
    r.mode      = firstNonEmpty(String(obj["mode"] | ""), String(obj["module"] | ""), String(obj["source_mode"] | ""));
    r.callsign  = normalizeHamCallsign(firstNonEmpty(String(obj["callsign"] | ""), String(obj["callsign_long"] | ""), String(obj["call"] | ""), String(obj["station"] | "")));
    String apiName = firstNonEmpty(
                      String(obj["name"] | ""),
                      String(obj["display_name"] | ""),
                      String(obj["full_name"] | ""),
                      String(obj["fullname"] | "")
                    );
    String apiFirstName = firstNonEmpty(String(obj["fname"] | ""), String(obj["first_name"] | ""), String(obj["firstname"] | ""), String(obj["forename"] | ""));
    String apiLastName  = firstNonEmpty(String(obj["lname"] | ""), String(obj["last_name"] | ""), String(obj["lastname"] | ""), String(obj["surname"] | ""));
    r.name      = trimSafe(apiName.length() ? apiName : buildOperatorName(apiFirstName, apiLastName));
    r.country   = firstNonEmpty(String(obj["country"] | ""), String(obj["country_name"] | ""));
    r.grid      = firstNonEmpty(String(obj["grid"] | ""), String(obj["locator"] | ""), String(obj["maidenhead"] | ""), String(obj["qth_locator"] | ""));
    r.target    = firstNonEmpty(String(obj["target"] | ""), String(obj["talkgroup"] | ""), String(obj["destination"] | ""), String(obj["reflector"] | ""));
    r.targetName= firstNonEmpty(String(obj["target_name"] | ""), String(obj["talkgroup_name"] | ""), String(obj["room_name"] | ""), String(obj["reflector_name"] | ""), String(obj["tg_name"] | ""));
    r.timeslot  = normalizeDmrTimeslot(firstNonEmpty(String(obj["timeslot"] | ""), String(obj["time_slot"] | ""), String(obj["timeSlot"] | ""), String(obj["slot"] | ""), String(obj["ts"] | "")));
    if (!r.timeslot.length()) r.timeslot = normalizeDmrTimeslot(firstNonEmpty(String(obj["TS"] | ""), String(obj["dmr_slot"] | ""), String(obj["dmrSlot"] | ""), String(obj["DMRSlot"] | ""), String(obj["TimeSlot"] | "")));
    if (!r.timeslot.length()) r.timeslot = extractDmrTimeslotFromText(firstNonEmpty(r.mode, r.target, r.targetName, String(obj["src"] | ""), String(obj["source"] | "")));
    r.src       = firstNonEmpty(String(obj["src"] | ""), String(obj["source"] | ""), String(obj["source_system"] | ""));
    r.duration  = firstNonEmpty(String(obj["duration"] | ""), String(obj["length"] | ""));
    r.loss      = firstNonEmpty(String(obj["loss"] | ""), String(obj["packet_loss"] | ""));
    r.ber       = firstNonEmpty(String(obj["bit_error_rate"] | ""), String(obj["ber"] | ""));
    r.rssi      = firstNonEmpty(String(obj["rssi"] | ""), String(obj["signal"] | ""));
    r.rxFreq    = firstNonEmpty(String(obj["rxFreq"] | ""), String(obj["rx_freq"] | ""), String(obj["RXFrequency"] | ""), String(obj["rx_frequency"] | ""));
    r.txFreq    = firstNonEmpty(String(obj["txFreq"] | ""), String(obj["tx_freq"] | ""), String(obj["TXFrequency"] | ""), String(obj["tx_frequency"] | ""));
    if (parsingSvxlinkData && !r.mode.length()) r.mode = "SVXLink";
    if (parsingSvxlinkData && !r.src.length()) r.src = "SVXRef";
    r.valid     = r.callsign.length() > 0;

    if (r.valid) {
      newList[newCount] = r;
      newCount++;
    }
  }

  applyRadioIdCacheToList(newList, newCount);
  inferModeStatusesFromHeard(newList, newCount);

  bool listChanged = !heardListsEqual(heardList, heardCount, newList, newCount);

  if (!parsingSvxlinkData) updateDmrTimeslotLiveRecordsFromList(newList, newCount);

  if (arr.size() == 0 || newCount == 0) {
    if (!parsingSvxlinkData) clearDmrTimeslotLiveRecords();
    if (heardCount != 0) {
      clearHeard();
      requestRedraw();
    }
    setFooterText(parsingSvxlinkData ? String("SVXLink API OK, no entries") : String("API OK, no entries"), !parsingSvxlinkData);
    return true;
  }

  if (listChanged) {
    clearHeard();
    for (int i = 0; i < newCount; i++) heardList[i] = newList[i];
    heardCount = newCount;
    requestRedraw();
  }

  if (newCount > 0) {
    if (newList[0].rxFreq.length()) currentRxFreq = newList[0].rxFreq;
    if (newList[0].txFreq.length()) currentTxFreq = newList[0].txFreq;
    queueRadioIdLookupForRecord(newList[0]);
    if (!sameRecord(liveRecord, newList[0])) {
      liveRecord = newList[0];
      updateSessionInfoFromRecord(liveRecord);
      liveRecordMillis = millis();
      requestRedraw();
      noteScreenActivity(false);
    } else if (!parsingSvxlinkData) {
      liveRecordMillis = millis();
    }
  }

  String okFooter = parsingSvxlinkData ? (String("SVXLink API updated from ") + displayBackendHost()) : (String("Updated from ") + displayBackendHost());
  setFooterText(okFooter, !parsingSvxlinkData);
  return true;
}

String htmlDecodeBasic(String s) {
  s.replace("&nbsp;", " ");
  s.replace("&amp;", "&");
  s.replace("&quot;", "\"");
  s.replace("&#39;", "'");
  s.replace("&lt;", "<");
  s.replace("&gt;", ">");
  return s;
}

String hamqslXmlTagValue(const String& xml, const String& tag, int from = 0, int* endOut = nullptr) {
  String openNeedle = "<" + tag;
  int open = xml.indexOf(openNeedle, from);
  while (open >= 0) {
    int after = open + openNeedle.length();
    if (after < (int)xml.length()) {
      char c = xml.charAt(after);
      if (c != '>' && c != ' ' && c != '\t' && c != '\r' && c != '\n') {
        open = xml.indexOf(openNeedle, after);
        continue;
      }
    }
    int gt = xml.indexOf(">", open);
    if (gt < 0) return "";
    String closeNeedle = "</" + tag + ">";
    int close = xml.indexOf(closeNeedle, gt + 1);
    if (close < 0) return "";
    if (endOut) *endOut = close + closeNeedle.length();
    String v = xml.substring(gt + 1, close);
    v = htmlDecodeBasic(v);
    v.trim();
    return v;
  }
  return "";
}

String hamqslXmlAttrValue(const String& openTag, const String& attr) {
  String needle = attr + "=";
  int p = openTag.indexOf(needle);
  if (p < 0) return "";
  p += needle.length();
  if (p >= (int)openTag.length()) return "";
  char q = openTag.charAt(p);
  if (q != '\"' && q != '\'') return "";
  int e = openTag.indexOf(String(q), p + 1);
  if (e < 0) return "";
  String v = openTag.substring(p + 1, e);
  v = htmlDecodeBasic(v);
  v.trim();
  return v;
}

bool hamqslBandConditionValue(const String& xml, const String& bandName, const String& dayNight, String& out) {
  int pos = 0;
  while (true) {
    int open = xml.indexOf("<band", pos);
    if (open < 0) return false;
    int gt = xml.indexOf(">", open);
    if (gt < 0) return false;
    String openTag = xml.substring(open, gt + 1);
    String name = hamqslXmlAttrValue(openTag, "name");
    String time = hamqslXmlAttrValue(openTag, "time");
    int close = xml.indexOf("</band>", gt + 1);
    if (close < 0) return false;
    if (name == bandName && time == dayNight) {
      out = htmlDecodeBasic(xml.substring(gt + 1, close));
      out.trim();
      return out.length() > 0;
    }
    pos = close + 7;
  }
}

String hamqslFriendlyVhfName(String name) {
  name.trim();
  if (name == "vhf-aurora") return "VHF Aurora";
  if (name == "E-Skip") return "E-Skip";
  if (name == "2m-Es") return "2m Es";
  if (name == "4m-Es") return "4m Es";
  return name;
}

uint16_t hamqslConditionColor(String value) {
  value.toLowerCase();
  if (value.indexOf("good") >= 0 || value.indexOf("open") >= 0 || value.indexOf("strong") >= 0 || value.indexOf("high") >= 0) return theme.good;
  if (value.indexOf("fair") >= 0 || value.indexOf("moderate") >= 0 || value.indexOf("active") >= 0 || value.indexOf("possible") >= 0) return theme.warn;
  if (value.indexOf("poor") >= 0 || value.indexOf("closed") >= 0 || value.indexOf("low") >= 0 || value.indexOf("none") >= 0 || value.indexOf("quiet") >= 0) return theme.bad;
  return theme.text;
}

bool parseHamqslXml(const String& payload) {
  if (payload.indexOf("<solar") < 0 || payload.indexOf("<solardata") < 0) return false;

  HamqslState data;
  data.updated = hamqslXmlTagValue(payload, "updated");
  data.solarFlux = hamqslXmlTagValue(payload, "solarflux");
  data.aIndex = hamqslXmlTagValue(payload, "aindex");
  data.kIndex = hamqslXmlTagValue(payload, "kindexnt");
  if (!data.kIndex.length()) data.kIndex = hamqslXmlTagValue(payload, "kindex");
  data.xray = hamqslXmlTagValue(payload, "xray");
  data.sunspots = hamqslXmlTagValue(payload, "sunspots");
  data.solarWind = hamqslXmlTagValue(payload, "solarwind");
  data.magneticField = hamqslXmlTagValue(payload, "magneticfield");
  data.geomagField = hamqslXmlTagValue(payload, "geomagfield");
  data.signalNoise = hamqslXmlTagValue(payload, "signalnoise");
  data.aurora = hamqslXmlTagValue(payload, "aurora");
  data.auroraLatitude = hamqslXmlTagValue(payload, "latdegree");
  data.muf = hamqslXmlTagValue(payload, "muf");
  data.fof2 = hamqslXmlTagValue(payload, "fof2");

  const char* hfNames[4] = { "80m-40m", "30m-20m", "17m-15m", "12m-10m" };
  for (uint8_t i = 0; i < 4; i++) {
    HamqslBandCondition band;
    band.name = hfNames[i];
    hamqslBandConditionValue(payload, band.name, "day", band.day);
    hamqslBandConditionValue(payload, band.name, "night", band.night);
    if (band.day.length() || band.night.length()) data.hfBands[data.hfBandCount++] = band;
  }

  int pos = 0;
  while (data.vhfConditionCount < 6) {
    int open = payload.indexOf("<phenomenon", pos);
    if (open < 0) break;
    int gt = payload.indexOf(">", open);
    if (gt < 0) break;
    int close = payload.indexOf("</phenomenon>", gt + 1);
    if (close < 0) break;
    String openTag = payload.substring(open, gt + 1);
    HamqslVhfCondition item;
    item.name = hamqslFriendlyVhfName(hamqslXmlAttrValue(openTag, "name"));
    item.location = hamqslXmlAttrValue(openTag, "location");
    item.condition = htmlDecodeBasic(payload.substring(gt + 1, close));
    item.condition.trim();
    if (item.name.length() || item.condition.length()) data.vhfConditions[data.vhfConditionCount++] = item;
    pos = close + 13;
  }

  data.valid = data.solarFlux.length() || data.sunspots.length() || data.aIndex.length() || data.kIndex.length() || data.hfBandCount > 0;
  if (!data.valid) return false;
  hamqslState = data;
  return true;
}

String extractHtmlText(const String& html) {
  String out;
  out.reserve(html.length());
  bool inTag = false;
  for (size_t i = 0; i < html.length(); i++) {
    char c = html[i];
    if (c == '<') { inTag = true; continue; }
    if (c == '>') { inTag = false; continue; }
    if (!inTag) out += c;
  }
  out = htmlDecodeBasic(out);
  out.replace("\n", " ");
  out.replace("\r", " ");
  out.trim();
  while (out.indexOf("  ") >= 0) out.replace("  ", " ");
  return out;
}

bool extractTableCellAfterHeader(const String& html, const String& headerLabel, String& valueOut) {
  int h = html.indexOf(">" + headerLabel + "</th>");
  if (h < 0) return false;
  int tdStart = html.indexOf("<td", h);
  if (tdStart < 0) return false;
  int gt = html.indexOf(">", tdStart);
  if (gt < 0) return false;
  int tdEnd = html.indexOf("</td>", gt);
  if (tdEnd < 0) return false;
  valueOut = extractHtmlText(html.substring(gt + 1, tdEnd));
  return valueOut.length() > 0;
}

bool extractDivCellAfterHeader(const String& html, const String& headerLabel, String& valueOut) {
  int h = html.indexOf(">" + headerLabel + "</div>");
  if (h < 0) return false;
  int cell = html.indexOf("divTableCell", h);
  if (cell < 0) return false;
  int gt = html.indexOf(">", cell);
  if (gt < 0) return false;
  int end = html.indexOf("</div>", gt);
  if (end < 0) return false;
  valueOut = extractHtmlText(html.substring(gt + 1, end));
  return valueOut.length() > 0;
}

bool extractWpsdTableFirstCellValue(const String& html, String& valueOut) {
  int tableStart = html.indexOf("<div class=\"divTable\"");
  if (tableStart < 0) return false;
  int bodyStart = html.indexOf("<div class=\"divTableBody\"", tableStart);
  if (bodyStart < 0) return false;
  int row2 = html.indexOf("<div class=\"divTableRow", bodyStart);
  if (row2 < 0) return false;
  row2 = html.indexOf("<div class=\"divTableRow", row2 + 1);
  if (row2 < 0) return false;

  int cell = html.indexOf("divTableCell", row2);
  if (cell < 0) return false;
  int gt = html.indexOf(">", cell);
  if (gt < 0) return false;
  int end = html.indexOf("</div>", gt);
  if (end < 0) return false;

  valueOut = extractHtmlText(html.substring(gt + 1, end));
  return valueOut.length() > 0;
}

bool extractPiStarTrxValue(const String& html, String& valueOut) {
  return extractTableCellAfterHeader(html, "Trx", valueOut);
}

bool extractWpsdStatusPill(const String& html, const String& label, ModeStatus& st) {
  int searchFrom = 0;
  while (true) {
    int pill = html.indexOf("<div class='status-pill ", searchFrom);
    if (pill < 0) return false;
    int pillEnd = html.indexOf("</div>", pill);
    if (pillEnd < 0) return false;

    int labelSpan = html.indexOf("<span>", pill);
    if (labelSpan < 0 || labelSpan > pillEnd) {
      searchFrom = pill + 10;
      continue;
    }
    int labelEnd = html.indexOf("</span>", labelSpan);
    if (labelEnd < 0 || labelEnd > pillEnd) {
      searchFrom = pill + 10;
      continue;
    }

    String pillLabel = extractHtmlText(html.substring(labelSpan, labelEnd + 7));
    if (pillLabel != label) {
      searchFrom = pillEnd + 6;
      continue;
    }

    st.known = true;
    String openTag = html.substring(pill, min(pill + 80, (int)html.length()));
    if (openTag.indexOf(" paused") >= 0 || openTag.indexOf(" inactive") >= 0) st.enabled = false;
    else st.enabled = true;
    return true;
  }
}

bool extractWpsdRadioInfoValue(const String& html, const String& headerLabel, String& valueOut) {
  int tableStart = html.indexOf("<div class=\"divTable\"");
  if (tableStart < 0) return false;
  int bodyStart = html.indexOf("<div class=\"divTableBody\"", tableStart);
  if (bodyStart < 0) return false;

  int row1 = html.indexOf("<div class=\"divTableRow", bodyStart);
  if (row1 < 0) return false;
  int row1End = html.indexOf("</div>", row1);
  if (row1End < 0) return false;
  int row2 = html.indexOf("<div class=\"divTableRow", row1End);
  if (row2 < 0) return false;

  String headers[16];
  int headerCount = 0;
  int pos = row1;
  while (headerCount < 16) {
    int cell = html.indexOf("divTableHeadCell", pos);
    if (cell < 0 || cell > row2) break;
    int gt = html.indexOf(">", cell);
    if (gt < 0 || gt > row2) break;
    int end = html.indexOf("</div>", gt);
    if (end < 0 || end > row2) break;
    headers[headerCount++] = extractHtmlText(html.substring(gt + 1, end));
    pos = end + 6;
  }

  String values[16];
  int valueCount = 0;
  pos = row2;
  while (valueCount < 16) {
    int cell = html.indexOf("divTableCell", pos);
    if (cell < 0) break;
    int gt = html.indexOf(">", cell);
    if (gt < 0) break;
    int end = html.indexOf("</div>", gt);
    if (end < 0) break;
    values[valueCount++] = extractHtmlText(html.substring(gt + 1, end));
    pos = end + 6;
    if (valueCount >= headerCount) break;
  }

  for (int i = 0; i < headerCount && i < valueCount; i++) {
    if (headers[i] == headerLabel) {
      valueOut = values[i];
      return valueOut.length() > 0;
    }
  }
  return false;
}

void parseModeCellWPSD(const String& html, const String& label, ModeStatus& st) {
  int pos = html.indexOf(">" + label + "</div>");
  if (pos < 0) return;
  int divStart = html.lastIndexOf("<div", pos);
  if (divStart < 0) return;
  String snippet = html.substring(divStart, min(pos + (int)label.length() + 20, (int)html.length()));
  st.known = true;
  if (snippet.indexOf("disabled-mode-cell") >= 0) st.enabled = false;
  else st.enabled = true;
}

void parseModeCellPiStar(const String& html, const String& label, ModeStatus& st) {
  int pos = html.indexOf(">" + label + "</td>");
  if (pos < 0) return;
  int tdStart = html.lastIndexOf("<td", pos);
  if (tdStart < 0) return;
  int gt = html.indexOf(">", tdStart);
  if (gt < 0 || gt > pos) return;
  String openTag = html.substring(tdStart, gt + 1);
  st.known = true;
  if (openTag.indexOf("#606060") >= 0 || openTag.indexOf("aria-disabled") >= 0) st.enabled = false;
  else st.enabled = true;
}

bool parseDashboardStatusHtml(const String& payload) {
  ModeStatus oldDMR = modeDMR;
  ModeStatus oldDSTAR = modeDSTAR;
  ModeStatus oldYSF = modeYSF;
  ModeStatus oldP25 = modeP25;
  ModeStatus oldNXDN = modeNXDN;
  ModeStatus oldPOCSAG = modePOCSAG;
  String oldHotspotRxFreq = hotspotRxFreq;
  String oldHotspotTxFreq = hotspotTxFreq;
  String oldHotspotStatusText = hotspotStatusText;

  bool hadKnownModes = modeDMR.known || modeDSTAR.known || modeYSF.known || modeP25.known || modeNXDN.known || modePOCSAG.known;
  resetModeStatuses();

  String rx, tx, statusText;
  if (normalizedBackend() == "wpsd") {
    extractWpsdStatusPill(payload, "DMR", modeDMR);
    extractWpsdStatusPill(payload, "D-Star", modeDSTAR);
    extractWpsdStatusPill(payload, "YSF", modeYSF);
    extractWpsdStatusPill(payload, "P25", modeP25);
    extractWpsdStatusPill(payload, "NXDN", modeNXDN);
    extractWpsdStatusPill(payload, "POCSAG", modePOCSAG);

    extractWpsdRadioInfoValue(payload, "TX Freq.", tx);
    extractWpsdRadioInfoValue(payload, "RX Freq.", rx);
    if (!tx.length() && !rx.length()) {
      if (extractWpsdRadioInfoValue(payload, "TX/RX Freq.", tx) && !rx.length()) rx = tx;
    }
    extractWpsdTableFirstCellValue(payload, statusText);
  } else {
    parseModeCellPiStar(payload, "DMR", modeDMR);
    parseModeCellPiStar(payload, "D-Star", modeDSTAR);
    parseModeCellPiStar(payload, "YSF", modeYSF);
    parseModeCellPiStar(payload, "P25", modeP25);
    parseModeCellPiStar(payload, "NXDN", modeNXDN);
    parseModeCellPiStar(payload, "POCSAG", modePOCSAG);

    extractTableCellAfterHeader(payload, "Tx", tx);
    extractTableCellAfterHeader(payload, "Rx", rx);
    extractPiStarTrxValue(payload, statusText);
  }

  tx = trimSafe(tx);
  rx = trimSafe(rx);
  statusText = trimSafe(statusText);
  tx.replace(" MHz", "");
  tx.replace("MHz", "");
  rx.replace(" MHz", "");
  rx.replace("MHz", "");
  tx.trim();
  rx.trim();

  if (tx.length()) hotspotTxFreq = tx;
  if (rx.length()) hotspotRxFreq = rx;
  if (statusText.length()) hotspotStatusText = statusText;

  bool modesKnownNow = modeDMR.known || modeDSTAR.known || modeYSF.known || modeP25.known || modeNXDN.known || modePOCSAG.known;
  if (!modesKnownNow && hadKnownModes) {
    modeDMR = oldDMR;
    modeDSTAR = oldDSTAR;
    modeYSF = oldYSF;
    modeP25 = oldP25;
    modeNXDN = oldNXDN;
    modePOCSAG = oldPOCSAG;
    modesKnownNow = true;
  }

  bool modeStatusChanged = !allModeStatusesEqual(oldDMR, oldDSTAR, oldYSF, oldP25, oldNXDN, oldPOCSAG) ||
                           oldHotspotRxFreq != hotspotRxFreq || oldHotspotTxFreq != hotspotTxFreq ||
                           oldHotspotStatusText != hotspotStatusText;
  if (modeStatusChanged) {
    requestRedraw();
    if (oldHotspotStatusText != hotspotStatusText) noteScreenActivity(false);
  }

  return modesKnownNow || hotspotRxFreq.length() || hotspotTxFreq.length();
}

bool httpGetString(const String& url, String& payload, int& codeOut, const String& accept = "", const String& token = "") {
  payload = "";
  codeOut = 0;

  NetworkGuard net(NETWORK_LOCK_TIMEOUT_MS);
  if (!net) {
    codeOut = -98;
    return false;
  }

  HTTPClient http;
  http.setConnectTimeout(HTTP_CONNECT_TIMEOUT_MS);
  http.setTimeout(HTTP_READ_TIMEOUT_MS);
  http.useHTTP10(true);
  http.setReuse(false);

  bool began = false;
  String lowerUrl = trimSafe(url);
  lowerUrl.toLowerCase();

  WiFiClient plainClient;
  WiFiClientSecure secureClient;
  if (lowerUrl.startsWith("https://")) {
    secureClient.setInsecure();
    began = http.begin(secureClient, url);
  } else {
    began = http.begin(plainClient, url);
  }

  if (!began) {
    codeOut = -1;
    plainClient.stop();
    secureClient.stop();
    return false;
  }

  http.setUserAgent(String("MMDVM-Display/") + DISPLAY_VERSION + " (operator=" + getDeviceCallsign() + ")");
  http.addHeader("Connection", "close");
  if (accept.length()) http.addHeader("Accept", accept);
  if (token.length()) http.addHeader("X-Display-Token", token);

  codeOut = http.GET();
  if (codeOut == 200) payload = http.getString();

  http.end();
  plainClient.stop();
  secureClient.stop();
  return codeOut == 200;
}


#if DISPLAY_BACKEND_IS_DESKTOP
static bool desktopJsonHasKeyRaw(const std::string& json, const char* key) {
  return desktopDataJsonFindValue(json, key) != std::string::npos;
}

static bool desktopJsonGetFloatRaw(const std::string& json, const char* key, float& out) {
  String v;
  if (!desktopDataJsonGetValue(json, key, v)) return false;
  v = trimSafe(v);
  if (!v.length()) return false;
  out = v.toFloat();
  return true;
}

static bool desktopJsonGetIntRaw(const std::string& json, const char* key, int& out) {
  String v;
  if (!desktopDataJsonGetValue(json, key, v)) return false;
  v = trimSafe(v);
  if (!v.length()) return false;
  out = v.toInt();
  return true;
}

static bool desktopParseWeatherResponseRaw(const String& payload) {
  std::string root = (std::string)payload;
  std::string mainObj;
  if (!desktopDataJsonGetObject(root, "main", mainObj)) {
    setFooterText("Weather JSON: main missing");
    desktopTrace("[DESKTOP][WEATHER] JSON parse failed: main object missing. Snippet: %.220s\n", root.c_str());
    return false;
  }

  std::string weatherArr;
  std::string firstWeather;
  if (desktopDataJsonGetArray(root, "weather", weatherArr)) {
    size_t pos = 0;
    desktopDataJsonNextObject(weatherArr, pos, firstWeather);
  }

  std::string windObj;
  desktopDataJsonGetObject(root, "wind", windObj);

  float f = 0.0f;
  int i = 0;
  if (!desktopJsonGetFloatRaw(mainObj, "temp", f)) {
    setFooterText("Weather JSON: temp missing");
    desktopTrace("[DESKTOP][WEATHER] JSON parse failed: main.temp missing. Main snippet: %.180s\n", mainObj.c_str());
    return false;
  }
  weatherState.tempC = f;
  weatherState.feelsLikeC = desktopJsonGetFloatRaw(mainObj, "feels_like", f) ? f : weatherState.tempC;
  weatherState.tempMinC = desktopJsonGetFloatRaw(mainObj, "temp_min", f) ? f : weatherState.tempC;
  weatherState.tempMaxC = desktopJsonGetFloatRaw(mainObj, "temp_max", f) ? f : weatherState.tempC;
  weatherState.pressureHpa = desktopJsonGetIntRaw(mainObj, "pressure", i) ? i : 0;
  weatherState.humidity = desktopJsonGetIntRaw(mainObj, "humidity", i) ? i : 0;
  weatherState.windSpeedMs = (!windObj.empty() && desktopJsonGetFloatRaw(windObj, "speed", f)) ? f : 0.0f;
  weatherState.windDeg = (!windObj.empty() && desktopJsonGetIntRaw(windObj, "deg", i)) ? i : 0;
  weatherState.visibilityM = desktopJsonGetIntRaw(root, "visibility", i) ? i : 0;

  String text;
  weatherState.icon = (!firstWeather.empty() && desktopDataJsonGetValue(firstWeather, "icon", text)) ? trimSafe(text) : String("");
  weatherState.summary = (!firstWeather.empty() && desktopDataJsonGetValue(firstWeather, "main", text)) ? trimSafe(text) : String("");
  weatherState.description = (!firstWeather.empty() && desktopDataJsonGetValue(firstWeather, "description", text)) ? trimSafe(text) : weatherState.summary;
  weatherState.location = desktopDataJsonGetValue(root, "name", text) ? trimSafe(text) : String("");

  weatherState.valid = true;
  weatherState.lastSuccessMillis = millis();
  desktopTrace("[DESKTOP][WEATHER] parsed weather: %.1f C, %s, %s.\n",
               weatherState.tempC,
               weatherState.description.length() ? weatherState.description.c_str() : "no description",
               weatherState.location.length() ? weatherState.location.c_str() : "no location");
  return true;
}
#endif

bool fetchWeatherData() {
  if (!weatherConfigured() || WiFi.status() != WL_CONNECTED) {
#if DISPLAY_BACKEND_IS_DESKTOP
    static bool desktopWeatherConfigLogged = false;
    if (!desktopWeatherConfigLogged) {
      desktopWeatherConfigLogged = true;
      if (!weatherConfigured()) {
        desktopTrace("[DESKTOP][WEATHER] disabled or missing config. Set weather_enabled=true, weather_lat, weather_lon, and weather_api_key in config.json.\n");
      } else {
        desktopTrace("[DESKTOP][WEATHER] WiFi/network status is not connected in desktop simulation.\n");
      }
    }
#endif
    return false;
  }
  if (weatherState.fetchInProgress) return false;

  String url = weatherRequestUrl();
  if (!url.length()) return false;

  unsigned long startMs = millis();
  weatherState.fetchInProgress = true;
  weatherState.lastAttemptMillis = millis();

  String payload;
  int code = 0;
  bool httpOk = httpGetString(url, payload, code, "application/json");
  weatherState.lastHttpCode = code;
  if (!httpOk) {
    weatherState.fetchInProgress = false;
    noteWeatherFetchResult(false, code);
    diagnosticsState.lastApiLatencyMs = millis() - startMs;
#if DISPLAY_BACKEND_IS_DESKTOP
    desktopTrace("[DESKTOP][WEATHER] HTTP fetch failed, code %d.\n", code);
#endif
    return false;
  }

#if DISPLAY_BACKEND_IS_DESKTOP
  bool desktopWeatherOk = desktopParseWeatherResponseRaw(payload);
  weatherState.fetchInProgress = false;
  noteWeatherFetchResult(desktopWeatherOk, code);
  diagnosticsState.lastApiLatencyMs = millis() - startMs;
  if (desktopWeatherOk) requestRedraw();
  return desktopWeatherOk;
#endif

  StaticJsonDocument<256> filter;
  filter["main"]["temp"] = true;
  filter["main"]["feels_like"] = true;
  filter["main"]["temp_min"] = true;
  filter["main"]["temp_max"] = true;
  filter["main"]["pressure"] = true;
  filter["main"]["humidity"] = true;
  filter["wind"]["speed"] = true;
  filter["wind"]["deg"] = true;
  filter["visibility"] = true;
  filter["weather"][0]["icon"] = true;
  filter["weather"][0]["main"] = true;
  filter["weather"][0]["description"] = true;
  filter["name"] = true;

  StaticJsonDocument<1536> doc;
  DeserializationError err = deserializeJson(doc, payload, DeserializationOption::Filter(filter));
  if (err) {
    weatherState.fetchInProgress = false;
    noteWeatherFetchResult(false, code);
    diagnosticsState.lastApiLatencyMs = millis() - startMs;
    return false;
  }

  JsonObject root = doc.as<JsonObject>();
  JsonArray weatherArr = root["weather"].as<JsonArray>();
  JsonObject mainObj = root["main"].as<JsonObject>();
  if (weatherArr.isNull() || weatherArr.size() == 0 || mainObj.isNull() || !mainObj.containsKey("temp")) {
    weatherState.fetchInProgress = false;
    noteWeatherFetchResult(false, code);
    diagnosticsState.lastApiLatencyMs = millis() - startMs;
    return false;
  }

  JsonObject first = weatherArr[0];
  JsonObject windObj = root["wind"].as<JsonObject>();
  weatherState.tempC = mainObj["temp"].as<float>();
  weatherState.feelsLikeC = mainObj["feels_like"].isNull() ? weatherState.tempC : mainObj["feels_like"].as<float>();
  weatherState.tempMinC = mainObj["temp_min"].isNull() ? weatherState.tempC : mainObj["temp_min"].as<float>();
  weatherState.tempMaxC = mainObj["temp_max"].isNull() ? weatherState.tempC : mainObj["temp_max"].as<float>();
  weatherState.pressureHpa = mainObj["pressure"].isNull() ? 0 : mainObj["pressure"].as<int>();
  weatherState.humidity = mainObj["humidity"].isNull() ? 0 : mainObj["humidity"].as<int>();
  weatherState.windSpeedMs = (windObj.isNull() || windObj["speed"].isNull()) ? 0.0f : windObj["speed"].as<float>();
  weatherState.windDeg = (windObj.isNull() || windObj["deg"].isNull()) ? 0 : windObj["deg"].as<int>();
  weatherState.visibilityM = root["visibility"].isNull() ? 0 : root["visibility"].as<int>();
  weatherState.icon = trimSafe(first["icon"].isNull() ? String("") : first["icon"].as<String>());
  weatherState.summary = trimSafe(!first["main"].isNull() ? first["main"].as<String>() : (!first["description"].isNull() ? first["description"].as<String>() : String("")));
  weatherState.description = trimSafe(!first["description"].isNull() ? first["description"].as<String>() : (!first["main"].isNull() ? first["main"].as<String>() : String("")));
  weatherState.location = trimSafe(root["name"].isNull() ? String("") : root["name"].as<String>());
  weatherState.valid = true;
  weatherState.lastSuccessMillis = millis();
  weatherState.fetchInProgress = false;
  noteWeatherFetchResult(true, code);
  diagnosticsState.lastApiLatencyMs = millis() - startMs;

  requestRedraw();
  return true;
}


bool fetchHamqslData() {
  if (!hamqslConfigured() || WiFi.status() != WL_CONNECTED) {
#if DISPLAY_BACKEND_IS_DESKTOP
    static bool desktopHamqslConfigLogged = false;
    if (!desktopHamqslConfigLogged) {
      desktopHamqslConfigLogged = true;
      desktopTrace("[DESKTOP][HAMQSL] disabled/missing config or WiFi/network status is not connected.\n");
    }
#endif
    return false;
  }
  if (hamqslState.fetchInProgress) return false;

  String url = hamqslRequestUrl();
  if (!url.length()) return false;

  unsigned long startMs = millis();
  hamqslState.fetchInProgress = true;
  hamqslState.lastAttemptMillis = millis();
  requestRedraw();

  String payload;
  int code = 0;
  bool httpOk = httpGetString(url, payload, code, "application/xml, text/xml, text/plain;q=0.9, */*;q=0.5");
  hamqslState.lastHttpCode = code;
  if (!httpOk) {
    hamqslState.fetchInProgress = false;
    hamqslState.error = code == -98 ? String("Network busy") : (String("HTTP ") + String(code));
    noteHamqslFetchResult(false, code);
    diagnosticsState.lastApiLatencyMs = millis() - startMs;
    requestRedraw();
    return false;
  }

  unsigned long attemptMs = hamqslState.lastAttemptMillis;
  if (!parseHamqslXml(payload)) {
    hamqslState.fetchInProgress = false;
    hamqslState.error = "HAMQSL XML parse failed";
    noteHamqslFetchResult(false, code);
    diagnosticsState.lastApiLatencyMs = millis() - startMs;
    requestRedraw();
    return false;
  }

  hamqslState.valid = true;
  hamqslState.fetchInProgress = false;
  hamqslState.lastAttemptMillis = attemptMs;
  hamqslState.lastSuccessMillis = millis();
  hamqslState.lastHttpCode = code;
  hamqslState.error = "";
  noteHamqslFetchResult(true, code);
  diagnosticsState.lastApiLatencyMs = millis() - startMs;
  requestRedraw();
  return true;
}



String svxCleanDashboardText(String s) {
  s = htmlDecodeBasic(s);
  s.replace("\n", " ");
  s.replace("\r", " ");
  s.replace("\t", " ");
  s.trim();
  while (s.indexOf("  ") >= 0) s.replace("  ", " ");
  return s;
}

bool svxExtractModeFromText(const String& text, String& modeOut) {
  int p = text.indexOf("Mode:");
  if (p < 0) return false;
  int start = p + 5;
  while (start < (int)text.length() && text[start] == ' ') start++;
  int end = start;
  while (end < (int)text.length()) {
    char c = text[end];
    if (c == ' ' || c == '<' || c == '&') break;
    end++;
  }
  modeOut = text.substring(start, end);
  modeOut.trim();
  return modeOut.length() > 0;
}

bool svxExtractLastRebootFromHtml(const String& html, String& rebootOut) {
  int p = html.indexOf("Last Reboot");
  if (p < 0) return false;
  int end = html.indexOf("</td>", p);
  if (end < 0) end = min(p + 160, (int)html.length());
  String txt = extractHtmlText(html.substring(p, end));
  txt.replace("Last Reboot", "");
  txt.trim();
  rebootOut = svxCleanDashboardText(txt);
  return rebootOut.length() > 0;
}

bool svxExtractTableItems(const String& html, const String& title, String items[], uint8_t maxItems, uint8_t& countOut) {
  countOut = 0;
  int titlePos = html.indexOf(title);
  if (titlePos < 0) return false;
  int tableEnd = html.indexOf("</table>", titlePos);
  if (tableEnd < 0) return false;

  int pos = titlePos;
  while (countOut < maxItems) {
    int td = html.indexOf("<td", pos);
    if (td < 0 || td > tableEnd) break;
    int gt = html.indexOf(">", td);
    if (gt < 0 || gt > tableEnd) break;
    int tdEnd = html.indexOf("</td>", gt);
    if (tdEnd < 0 || tdEnd > tableEnd) break;

    String item = svxCleanDashboardText(extractHtmlText(html.substring(gt + 1, tdEnd)));
    if (item.length() && item != "No Modules") {
      items[countOut++] = item;
    }
    pos = tdEnd + 5;
  }
  return countOut > 0;
}

bool svxExtractModules(const String& html) {
  svxlinkModuleCount = 0;
  int titlePos = html.indexOf("Modules Loaded");
  if (titlePos < 0) return false;
  int tableEnd = html.indexOf("</table>", titlePos);
  if (tableEnd < 0) return false;

  int pos = titlePos;
  while (svxlinkModuleCount < SVXLINK_MAX_MODULES) {
    int td = html.indexOf("<td", pos);
    if (td < 0 || td > tableEnd) break;
    int gt = html.indexOf(">", td);
    if (gt < 0 || gt > tableEnd) break;
    int tdEnd = html.indexOf("</td>", gt);
    if (tdEnd < 0 || tdEnd > tableEnd) break;

    String item = svxCleanDashboardText(extractHtmlText(html.substring(gt + 1, tdEnd)));
    if (item.length() && item != "No Modules") {
      String openTag = html.substring(td, min(gt + 1, (int)html.length()));
      svxlinkModules[svxlinkModuleCount].name = item;
      svxlinkModules[svxlinkModuleCount].active = (openTag.indexOf("MediumSeaGreen") >= 0 || openTag.indexOf("green") >= 0);
      svxlinkModuleCount++;
    }
    pos = tdEnd + 5;
  }
  return svxlinkModuleCount > 0;
}

bool svxNormalizeReflectorStatusChunk(String chunk, String& statusOut) {
  chunk = svxCleanDashboardText(chunk);
  if (!chunk.length()) return false;

  String lower = chunk;
  lower.toLowerCase();

  if (lower.indexOf("not connected") >= 0 || lower.indexOf("disconnected") >= 0 || lower.indexOf("disconnect") >= 0) {
    statusOut = "Not connected";
    return true;
  }
  if (lower.indexOf("no status") >= 0 || lower.indexOf("unknown") >= 0) {
    statusOut = "No status";
    return true;
  }
  if (lower.indexOf("connected") >= 0 || lower.indexOf("online") >= 0 || lower.indexOf("active") >= 0) {
    statusOut = "Connected";
    return true;
  }
  return false;
}

bool svxDashboardHasReflectorConnectionEvidence(const String& text) {
  String lower = text;
  lower.toLowerCase();

  bool reflectorLogicLoaded = lower.indexOf("reflectorlogic") >= 0;
  bool reflectorHostShown = lower.indexOf("reflector-network") >= 0 || lower.indexOf("svxreflector") >= 0;
  bool talkgroupDataShown = lower.indexOf("tg default") >= 0 || lower.indexOf("tg monitor") >= 0 || lower.indexOf("tg active") >= 0;
  bool activityShown = lower.indexOf("svxreflector activity") >= 0;

  return reflectorLogicLoaded && reflectorHostShown && (talkgroupDataShown || activityShown);
}

bool svxExtractReflectorStatusFromDashboardText(const String& text, String& statusOut) {
  int label = text.indexOf("1.reflector");
  int labelLen = 11;
  if (label < 0) {
    label = text.indexOf("reflector-network");
    labelLen = 17;
  }
  if (label < 0) {
    label = text.indexOf("Reflector");
    labelLen = 9;
  }
  if (label < 0) return false;

  int start = label + labelLen;
  int end = text.length();
  const char* stopLabels[] = {
    "TG Default", "TG Monitor", "TG Active", "Repeater Status", "Radio Status",
    "EchoLink Users", "TXing", "Systeminfo", "SVXReflector Activity", "Last Reboot"
  };
  for (uint8_t i = 0; i < sizeof(stopLabels) / sizeof(stopLabels[0]); i++) {
    int p = text.indexOf(stopLabels[i], start);
    if (p >= start && p < end) end = p;
  }

  String chunk = text.substring(start, end);
  return svxNormalizeReflectorStatusChunk(chunk, statusOut);
}

uint8_t svxCollectTdTexts(const String& rowHtml, String cells[], uint8_t maxCells) {
  uint8_t count = 0;
  int pos = 0;
  while (count < maxCells) {
    int td = rowHtml.indexOf("<td", pos);
    if (td < 0) break;
    int gt = rowHtml.indexOf(">", td);
    if (gt < 0) break;
    int tdEnd = rowHtml.indexOf("</td>", gt);
    if (tdEnd < 0) break;
    cells[count++] = svxCleanDashboardText(extractHtmlText(rowHtml.substring(gt + 1, tdEnd)));
    pos = tdEnd + 5;
  }
  return count;
}

bool svxApplyParsedHeardList(HeardRecord newList[], int newCount, const String& okFooter) {
  applyRadioIdCacheToList(newList, newCount);
  inferModeStatusesFromHeard(newList, newCount);

  bool listChanged = !heardListsEqual(heardList, heardCount, newList, newCount);

  if (newCount == 0) {
    if (heardCount != 0) {
      clearHeard();
      requestRedraw();
    }
    setFooterText(okFooter.length() ? okFooter : String("SVXLink dashboard OK, no entries"), !parsingSvxlinkData);
    return true;
  }

  if (listChanged) {
    clearHeard();
    for (int i = 0; i < newCount; i++) heardList[i] = newList[i];
    heardCount = newCount;
    requestRedraw();
  }

  if (newCount > 0) {
    queueRadioIdLookupForRecord(newList[0]);
    if (!sameRecord(liveRecord, newList[0])) {
      liveRecord = newList[0];
      updateSessionInfoFromRecord(liveRecord);
      liveRecordMillis = millis();
      requestRedraw();
      noteScreenActivity(false);
    } else if (!parsingSvxlinkData) {
      liveRecordMillis = millis();
    }
  }

  setFooterText(okFooter.length() ? okFooter : String("SVXLink dashboard OK"), !parsingSvxlinkData);
  return true;
}

bool svxParseLastHeardFromHtml(const String& html) {
  int marker = html.indexOf("SVXReflector Activity");
  if (marker < 0) marker = html.indexOf("LastHeard");
  if (marker < 0) return false;

  int tableStart = html.indexOf("<table", marker);
  if (tableStart < 0) return false;
  int tableEnd = html.indexOf("</table>", tableStart);
  if (tableEnd < 0) return false;

  HeardRecord newList[MAX_HEARD];
  int newCount = 0;

  int pos = tableStart;
  while (newCount < MAX_HEARD) {
    int tr = html.indexOf("<tr", pos);
    if (tr < 0 || tr > tableEnd) break;
    int trEnd = html.indexOf("</tr>", tr);
    if (trEnd < 0 || trEnd > tableEnd) break;

    String row = html.substring(tr, trEnd + 5);
    String cells[8];
    uint8_t cellCount = svxCollectTdTexts(row, cells, 8);

    if (cellCount >= 5) {
      String timeText = cells[0];
      String callText = cells[1];
      String tgText = cells[2];
      String sourceText = (cellCount >= 5) ? cells[4] : "";
      String tgName = (cellCount >= 6) ? cells[5] : "";

      callText.trim();
      tgText.trim();
      sourceText.trim();
      tgName.trim();

      if (callText.length() && callText != "Callsign" && tgText.length() && tgText != "TG #") {
        HeardRecord r;
        r.timeUtc = normalizeHeardTimeDisplay(timeText);
        r.mode = "SVXLink";
        r.callsign = normalizeHamCallsign(callText);
        r.target = tgText;
        if (!r.target.startsWith("TG")) r.target = "TG " + r.target;
        r.targetName = tgName;
        r.src = sourceText.length() ? sourceText : String("SVXRef");
        r.valid = true;
        newList[newCount++] = r;
      }
    }

    pos = trEnd + 5;
  }

  return svxApplyParsedHeardList(newList, newCount, "SVXLink dashboard OK");
}

bool parseSvxlinkDashboardHtml(const String& payload) {
  if (payload.indexOf("SVXLink Info") < 0 && payload.indexOf("SVXReflector Activity") < 0 && payload.indexOf("Dashboard") < 0) {
    lastError = "SVXLink dashboard HTML not recognized";
    return false;
  }

  String oldRadioStatus = svxlinkRadioStatus;
  String oldReflectorStatus = svxlinkReflectorStatus;
  String oldActiveTg = svxlinkActiveTg;
  String oldMonitor = svxlinkMonitorTgs;

  String text = svxCleanDashboardText(extractHtmlText(payload));

  if (!svxlinkNetworkName.length()) svxlinkNetworkName = activeBackendHost();

  String val;
  if (extractTableCellAfterHeader(payload, "TG Default", val)) svxlinkDefaultTg = val;
  if (extractTableCellAfterHeader(payload, "TG Monitor", val)) svxlinkMonitorTgs = val;
  if (extractTableCellAfterHeader(payload, "TG Active", val)) svxlinkActiveTg = val;
  if (extractTableCellAfterHeader(payload, "Radio Status", val) || extractTableCellAfterHeader(payload, "Repeater Status", val)) svxlinkRadioStatus = val;
  if (svxExtractModeFromText(text, val)) svxlinkModeText = val;
  if (svxExtractLastRebootFromHtml(payload, val)) svxlinkLastReboot = val;

  bool reflectorEvidence = svxDashboardHasReflectorConnectionEvidence(text);
  if (svxExtractReflectorStatusFromDashboardText(text, val)) {
    if (val.equalsIgnoreCase("No status") && reflectorEvidence) val = "Connected";
    svxlinkReflectorStatus = val;
  } else if (reflectorEvidence) {
    svxlinkReflectorStatus = "Connected";
  }

  svxExtractTableItems(payload, "Active Logics", svxlinkLogics, SVXLINK_MAX_LOGICS, svxlinkLogicCount);
  svxExtractModules(payload);

  if (!svxlinkCallsign.length()) {
    int p = text.indexOf("Dashboard ");
    if (p >= 0) {
      int start = p + 10;
      int end = text.indexOf(" Host:", start);
      if (end < 0) end = text.indexOf(" Lokale Zeit:", start);
      if (end > start) {
        svxlinkCallsign = text.substring(start, end);
        svxlinkCallsign.trim();
      }
    }
  }

  svxlinkLastSuccessMillis = millis();
  bool statusChanged = oldRadioStatus != svxlinkRadioStatus || oldReflectorStatus != svxlinkReflectorStatus ||
                       oldActiveTg != svxlinkActiveTg || oldMonitor != svxlinkMonitorTgs;

  bool heardOk = svxParseLastHeardFromHtml(payload);
  if (statusChanged) {
    requestRedraw();
    noteScreenActivity(false);
  }
  return heardOk || svxlinkRadioStatus.length() || svxlinkReflectorStatus.length() || svxlinkLogicCount > 0;
}


void resetSvxlinkState() {
  svxlinkCallsign = "";
  svxlinkNetworkName = "";
  svxlinkRadioStatus = "";
  svxlinkReflectorStatus = "";
  svxlinkModeText = "";
  svxlinkDefaultTg = "";
  svxlinkActiveTg = "";
  svxlinkMonitorTgs = "";
  svxlinkLastReboot = "";
  svxlinkSystemText = "";
  svxlinkLogicCount = 0;
  svxlinkModuleCount = 0;
}


void resetAprsState() {
  aprsState = AprsState();
  clearSvxlinkAprsCommentAlert();
}

unsigned long aprsMillisUntil(unsigned long now, unsigned long deadline) {
  if (!deadline) return 0;
  return ((long)(deadline - now) > 0) ? (deadline - now) : 0;
}

String aprsFormatWait(unsigned long ms) {
  unsigned long seconds = (ms + 999UL) / 1000UL;
  if (seconds < 120UL) return String(seconds) + "s";

  unsigned long minutes = (seconds + 59UL) / 60UL;
  if (minutes < 120UL) return String(minutes) + " min";

  unsigned long hours = (minutes + 59UL) / 60UL;
  return String(hours) + " h";
}

unsigned long aprsRetryDelayMs() {
  unsigned long delayMs = APRS_MIN_RETRY_MS;
  uint8_t failures = aprsState.consecutiveFailures;
  if (failures > 8) failures = 8;

  for (uint8_t i = 1; i < failures; ++i) {
    if (delayMs >= APRS_MAX_RETRY_MS / 2UL) return APRS_MAX_RETRY_MS;
    delayMs *= 2UL;
  }
  return delayMs > APRS_MAX_RETRY_MS ? APRS_MAX_RETRY_MS : delayMs;
}

void aprsScheduleRetry(const String& failCode, const String& baseError) {
  if (aprsState.consecutiveFailures < 250) aprsState.consecutiveFailures++;

  String code = trimSafe(failCode);
  unsigned long delayMs = code.equalsIgnoreCase("ratelimit") ? APRS_RATE_LIMIT_BACKOFF_MS : aprsRetryDelayMs();
  aprsState.nextAllowedAttemptMillis = millis() + delayMs;

  if (code.equalsIgnoreCase("ratelimit")) {
    aprsState.error = String("APRS rate limited; retry in ") + aprsFormatWait(delayMs);
  } else if (trimSafe(baseError).length()) {
    aprsState.error = trimSafe(baseError) + "; retry in " + aprsFormatWait(delayMs);
  } else {
    aprsState.error = String("APRS lookup failed; retry in ") + aprsFormatWait(delayMs);
  }
}


#if DISPLAY_BACKEND_IS_DESKTOP
static bool desktopParseAprsLocationResponseRaw(const String& payload, const String& sourceCallsign) {
  std::string root = (std::string)payload;
  String result;
  desktopDataJsonGetValue(root, "result", result);
  result = trimSafe(result);
  desktopDataJsonGetValue(root, "code", aprsState.failCode);
  aprsState.failCode = trimSafe(aprsState.failCode);

  if (!result.equalsIgnoreCase("ok")) {
    String desc;
    if (!desktopDataJsonGetValue(root, "description", desc)) desc = "APRS lookup failed";
    aprsState.error = trimSafe(desc);
    if (aprsState.failCode.equalsIgnoreCase("ratelimit")) aprsState.error = "APRS rate limited";
    desktopTrace("[DESKTOP][APRS] API result not ok: %s / %s.\n", result.c_str(), aprsState.error.c_str());
    return false;
  }

  std::string entries;
  if (!desktopDataJsonGetArray(root, "entries", entries)) {
    aprsState.error = "No APRS entries array";
    desktopTrace("[DESKTOP][APRS] JSON parse failed: entries array missing.\n");
    return false;
  }

  std::string entry;
  size_t pos = 0;
  if (!desktopDataJsonNextObject(entries, pos, entry)) {
    aprsState.error = "No APRS location found";
    desktopTrace("[DESKTOP][APRS] entries array is empty.\n");
    return false;
  }

  String latText, lngText;
  desktopDataJsonGetValue(entry, "lat", latText);
  desktopDataJsonGetValue(entry, "lng", lngText);
  latText = trimSafe(latText);
  lngText = trimSafe(lngText);
  if (!latText.length() || !lngText.length()) {
    aprsState.error = "APRS entry has no coordinates";
    desktopTrace("[DESKTOP][APRS] JSON parse failed: coordinates missing.\n");
    return false;
  }

  String v;
  aprsState.valid = true;
  aprsState.error = "";
  aprsState.failCode = "";
  aprsState.consecutiveFailures = 0;
  aprsState.nextAllowedAttemptMillis = 0;
  aprsState.sourceCallsign = sourceCallsign;
  aprsState.callsign = desktopDataJsonGetValue(entry, "name", v) ? normalizeHamCallsign(v) : normalizeHamCallsign(sourceCallsign);
  aprsState.dstCallsign = desktopDataJsonGetValue(entry, "dstcall", v) ? normalizeHamCallsign(v) : String("");
  aprsState.symbol = desktopDataJsonGetValue(entry, "symbol", v) ? trimSafe(v) : String("");
  aprsState.lat = latText.toFloat();
  aprsState.lng = lngText.toFloat();
  aprsState.speedValid = desktopDataJsonGetValue(entry, "speed", v);
  aprsState.speedKmh = aprsState.speedValid ? trimSafe(v).toFloat() : 0.0f;
  aprsState.courseValid = desktopDataJsonGetValue(entry, "course", v);
  aprsState.courseDeg = aprsState.courseValid ? trimSafe(v).toInt() : 0;
  aprsState.altitudeValid = desktopDataJsonGetValue(entry, "altitude", v);
  aprsState.altitudeM = aprsState.altitudeValid ? trimSafe(v).toFloat() : 0.0f;
  aprsState.comment = desktopDataJsonGetValue(entry, "comment", v) ? trimSafe(v) : String("");
  aprsState.status = desktopDataJsonGetValue(entry, "status", v) ? trimSafe(v) : String("");
  aprsState.path = desktopDataJsonGetValue(entry, "path", v) ? trimSafe(v) : String("");
  String lastText;
  String timeText;
  desktopDataJsonGetValue(entry, "lasttime", lastText);
  desktopDataJsonGetValue(entry, "time", timeText);
  aprsState.lastSeenText = aprsFormatEpoch(firstNonEmpty(lastText, timeText));
  aprsState.lastSuccessMillis = millis();
  updateSvxlinkAprsCommentAlertFromState();
  desktopTrace("[DESKTOP][APRS] parsed APRS for %s at %.5f %.5f.\n", aprsState.callsign.c_str(), aprsState.lat, aprsState.lng);
  return true;
}
#endif

bool parseAprsLocationResponse(const String& payload, const String& sourceCallsign) {
#if DISPLAY_BACKEND_IS_DESKTOP
  return desktopParseAprsLocationResponseRaw(payload, sourceCallsign);
#endif

  DynamicJsonDocument doc(4096);
  DeserializationError err = deserializeJson(doc, payload);
  if (err) {
    aprsState.error = String("APRS JSON: ") + err.c_str();
    return false;
  }

  JsonObject root = doc.as<JsonObject>();
  String result = trimSafe(String(root["result"] | ""));
  aprsState.failCode = trimSafe(String(root["code"] | ""));
  if (!result.equalsIgnoreCase("ok")) {
    aprsState.error = trimSafe(String(root["description"] | "APRS lookup failed"));
    if (aprsState.failCode.equalsIgnoreCase("ratelimit")) aprsState.error = "APRS rate limited";
    return false;
  }

  JsonArray entries = root["entries"].as<JsonArray>();
  if (entries.isNull() || entries.size() == 0) {
    aprsState.error = "No APRS location found";
    return false;
  }

  JsonObject entry = entries[0].as<JsonObject>();
  String latText = trimSafe(String(entry["lat"] | ""));
  String lngText = trimSafe(String(entry["lng"] | ""));
  if (!latText.length() || !lngText.length()) {
    aprsState.error = "APRS entry has no coordinates";
    return false;
  }

  aprsState.valid = true;
  aprsState.error = "";
  aprsState.failCode = "";
  aprsState.consecutiveFailures = 0;
  aprsState.nextAllowedAttemptMillis = 0;
  aprsState.sourceCallsign = sourceCallsign;
  aprsState.callsign = normalizeHamCallsign(String(entry["name"] | sourceCallsign));
  aprsState.dstCallsign = normalizeHamCallsign(String(entry["dstcall"] | ""));
  aprsState.symbol = trimSafe(String(entry["symbol"] | ""));
  aprsState.lat = latText.toFloat();
  aprsState.lng = lngText.toFloat();
  aprsState.speedValid = entry.containsKey("speed");
  aprsState.speedKmh = aprsState.speedValid ? entry["speed"].as<float>() : 0.0f;
  aprsState.courseValid = entry.containsKey("course");
  aprsState.courseDeg = aprsState.courseValid ? entry["course"].as<int>() : 0;
  aprsState.altitudeValid = entry.containsKey("altitude");
  aprsState.altitudeM = aprsState.altitudeValid ? entry["altitude"].as<float>() : 0.0f;
  aprsState.comment = trimSafe(String(entry["comment"] | ""));
  aprsState.status = trimSafe(String(entry["status"] | ""));
  aprsState.path = trimSafe(String(entry["path"] | ""));
  aprsState.lastSeenText = aprsFormatEpoch(firstNonEmpty(String(entry["lasttime"] | ""), String(entry["time"] | "")));
  aprsState.lastSuccessMillis = millis();
  updateSvxlinkAprsCommentAlertFromState();
  return true;
}

bool fetchAprsData(bool force) {
  if (!aprsFeatureEnabled()) {
#if DISPLAY_BACKEND_IS_DESKTOP
    static bool desktopAprsConfigLogged = false;
    if (!desktopAprsConfigLogged) {
      desktopAprsConfigLogged = true;
      desktopTrace("[DESKTOP][APRS] disabled or missing API key. Set aprs_enabled=true and aprs_api_key in config.json.\n");
    }
#endif
    return false;
  }
  if (WiFi.status() != WL_CONNECTED) {
#if DISPLAY_BACKEND_IS_DESKTOP
    static bool desktopAprsWifiLogged = false;
    if (!desktopAprsWifiLogged) {
      desktopAprsWifiLogged = true;
      desktopTrace("[DESKTOP][APRS] WiFi/network status is not connected in desktop simulation.\n");
    }
#endif
    return false;
  }
  if (aprsState.fetchInProgress) return false;

  String call = aprsLookupCallsign();
  if (!call.length()) {
    aprsState.error = "Waiting for LIVE callsign";
    return false;
  }

  unsigned long now = millis();
  bool hasDataForCall = aprsState.valid && (aprsState.sourceCallsign == call || aprsState.callsign == call);

  unsigned long backoffWaitMs = aprsMillisUntil(now, aprsState.nextAllowedAttemptMillis);
  if (backoffWaitMs > 0) {
    aprsState.error = String("APRS backoff: ") + aprsFormatWait(backoffWaitMs);
    return hasDataForCall;
  }

  unsigned long refreshMs = (unsigned long)cfg.aprsRefreshMinutes * 60000UL;
  if (!force && hasDataForCall && aprsState.lastSuccessMillis && (now - aprsState.lastSuccessMillis) < refreshMs) return true;

  if (aprsState.lastAttemptMillis) {
    unsigned long gapMs = now - aprsState.lastAttemptMillis;
    if (gapMs < APRS_MIN_REQUEST_GAP_MS) {
      aprsState.error = String("APRS next request in ") + aprsFormatWait(APRS_MIN_REQUEST_GAP_MS - gapMs);
      return hasDataForCall;
    }
  }

  String url = aprsApiUrl(call);
  if (!url.length()) {
    aprsState.error = "APRS API key/callsign missing";
    return false;
  }

  aprsState.fetchInProgress = true;
  aprsState.lastAttemptMillis = now;
  aprsState.failCode = "";
  String payload;
  int code = 0;
  unsigned long startMs = millis();
  bool httpOk = httpGetString(url, payload, code, "application/json", "");
  aprsState.lastHttpCode = code;
  if (!httpOk) {
    aprsState.fetchInProgress = false;
    String err = code == -1 ? String("APRS HTTP begin failed") : (String("APRS HTTP: ") + String(code));
    aprsScheduleRetry("", err);
    noteApiFetchResult(false, code, millis() - startMs);
    setFooterText(aprsState.error, false);
    requestRedraw();
    return false;
  }

  bool ok = parseAprsLocationResponse(payload, call);
  aprsState.fetchInProgress = false;
  if (!ok) aprsScheduleRetry(aprsState.failCode, aprsState.error);
  noteApiFetchResult(ok, code, millis() - startMs);
  setFooterText(ok ? (String("APRS updated for ") + aprsState.callsign) : aprsState.error, ok);
  requestRedraw();
  return ok;
}

String joinJsonStringArray(JsonArray arr, const String& sep = ", ") {
  String out;
  for (JsonVariant v : arr) {
    String item = trimSafe(String(v | ""));
    if (!item.length()) continue;
    if (out.length()) out += sep;
    out += item;
  }
  return out;
}

void parseSvxlinkStatusFromObject(JsonObject root) {
  bool looksLikeSvxlink = root.containsKey("radio_status") || root.containsKey("reflector_status") ||
                          root.containsKey("tg") || root.containsKey("last_heard") ||
                          root.containsKey("api") || root.containsKey("backend") ||
                          root.containsKey("active_logics") || root.containsKey("logics");
  if (!looksLikeSvxlink) return;

  String oldRadioStatus = svxlinkRadioStatus;
  String oldReflectorStatus = svxlinkReflectorStatus;
  String oldActiveTg = svxlinkActiveTg;
  String oldMonitor = svxlinkMonitorTgs;

  JsonObject systemObj;
  if (root["system"].is<JsonObject>()) systemObj = root["system"].as<JsonObject>();
  bool hasSystemObj = !systemObj.isNull();

  svxlinkCallsign = firstNonEmpty(String(root["callsign"] | ""), String(root["node"] | ""), svxlinkCallsign);
  svxlinkNetworkName = firstNonEmpty(String(root["network_name"] | ""), String(root["network"] | ""), String(root["fmnetwork"] | ""), String(root["host"] | ""), svxlinkNetworkName);
  svxlinkRadioStatus = firstNonEmpty(String(root["radio_status"] | ""), String(root["status"] | ""), svxlinkRadioStatus);
  svxlinkReflectorStatus = firstNonEmpty(String(root["reflector_status"] | ""), String(root["svxreflector_status"] | ""), svxlinkReflectorStatus);
  svxlinkModeText = firstNonEmpty(String(root["mode"] | ""), String(root["logic_mode"] | ""), hasSystemObj ? String(systemObj["mode"] | "") : String(""), svxlinkModeText);
  svxlinkLastReboot = firstNonEmpty(String(root["last_reboot"] | ""), String(root["uptime_since"] | ""), hasSystemObj ? String(systemObj["reboot"] | "") : String(""), svxlinkLastReboot);
  svxlinkSystemText = firstNonEmpty(String(root["system_text"] | ""), hasSystemObj ? String(systemObj["process"] | "") : String(""), svxlinkSystemText);

  /* DL3EL */
  svxlinkLat = firstNonEmpty(String(root["hs_lat"] | ""), String(root["lat"] | ""), svxlinkLat);
  svxlinkLon = firstNonEmpty(String(root["hs_lon"] | ""), String(root["lon"] | ""), svxlinkLon);
  /* DL3EL */

  if (root["tg"].is<JsonObject>()) {
    JsonObject tg = root["tg"].as<JsonObject>();
    svxlinkDefaultTg = firstNonEmpty(String(tg["default"] | ""), svxlinkDefaultTg);
    svxlinkActiveTg = firstNonEmpty(String(tg["active"] | ""), svxlinkActiveTg);
    if (tg["monitor"].is<JsonArray>()) {
      svxlinkMonitorTgs = joinJsonStringArray(tg["monitor"].as<JsonArray>(), " ");
    } else {
      svxlinkMonitorTgs = firstNonEmpty(String(tg["monitor"] | ""), svxlinkMonitorTgs);
    }
  } else {
    svxlinkDefaultTg = firstNonEmpty(String(root["tg_default"] | ""), String(root["default_tg"] | ""), svxlinkDefaultTg);
    svxlinkActiveTg = firstNonEmpty(String(root["tg_active"] | ""), String(root["active_tg"] | ""), svxlinkActiveTg);
    if (root["tg_monitor"].is<JsonArray>()) {
      svxlinkMonitorTgs = joinJsonStringArray(root["tg_monitor"].as<JsonArray>(), " ");
    } else {
      svxlinkMonitorTgs = firstNonEmpty(String(root["tg_monitor"] | ""), String(root["monitor_tgs"] | ""), svxlinkMonitorTgs);
    }
  }

  svxlinkLogicCount = 0;
  JsonArray logicArray;
  if (root["active_logics"].is<JsonArray>()) logicArray = root["active_logics"].as<JsonArray>();
  else if (root["logics"].is<JsonArray>()) logicArray = root["logics"].as<JsonArray>();
  if (!logicArray.isNull()) {
    for (JsonVariant v : logicArray) {
      if (svxlinkLogicCount >= SVXLINK_MAX_LOGICS) break;
      String item = trimSafe(String(v | ""));
      if (!item.length()) continue;
      svxlinkLogics[svxlinkLogicCount++] = item;
    }
  }

  svxlinkModuleCount = 0;
  if (root["modules"].is<JsonArray>()) {
    for (JsonObject mod : root["modules"].as<JsonArray>()) {
      if (svxlinkModuleCount >= SVXLINK_MAX_MODULES) break;
      String name = trimSafe(String(mod["name"] | ""));
      if (!name.length()) continue;
      svxlinkModules[svxlinkModuleCount].name = name;
      svxlinkModules[svxlinkModuleCount].active = mod["active"] | false;
      svxlinkModuleCount++;
    }
  } else if (root["modules_loaded"].is<JsonArray>()) {
    for (JsonVariant v : root["modules_loaded"].as<JsonArray>()) {
      if (svxlinkModuleCount >= SVXLINK_MAX_MODULES) break;
      String item = trimSafe(String(v | ""));
      if (!item.length()) continue;
      svxlinkModules[svxlinkModuleCount].name = item;
      svxlinkModules[svxlinkModuleCount].active = true;
      svxlinkModuleCount++;
    }
  }

  svxlinkLastSuccessMillis = millis();
  if (oldRadioStatus != svxlinkRadioStatus || oldReflectorStatus != svxlinkReflectorStatus ||
      oldActiveTg != svxlinkActiveTg || oldMonitor != svxlinkMonitorTgs) {
    requestRedraw();
    noteScreenActivity(false);
  }
}

bool fetchSvxlinkData() {
  if (WiFi.status() != WL_CONNECTED) {
    wifiOk = false;
    setFooterText("Wi-Fi disconnected", false);
    requestRedraw();
    noteApiFetchResult(false, -1, 0);
    return false;
  }

  String url = svxlinkApiUrl();
  if (!url.length()) {
    setFooterText("SVXLink host missing", false);
    requestRedraw();
    noteApiFetchResult(false, -1, 0);
    return false;
  }

  unsigned long startMs = millis();
  String payload;
  int code = 0;
  String accept = isSvxlinkApiBackend() ? String("application/json") : String("text/html");
  bool ok = httpGetString(url, payload, code, accept, "");
  if (!ok) {
    if (code == -1) setFooterText("SVXLink HTTP begin failed", false);
    else setFooterText("SVXLink HTTP: " + String(code), false);
    requestRedraw();
    noteApiFetchResult(false, code, millis() - startMs);
    return false;
  }

  parsingSvxlinkData = true;
  if (isSvxlinkApiBackend()) {
    ok = parseHeardArray(payload);
  } else {
    ok = parseSvxlinkDashboardHtml(payload);
  }
  parsingSvxlinkData = false;
  if (ok) saveActiveHeardCache();

  noteApiFetchResult(ok, code, millis() - startMs);
  return ok;
}


String currentSvxlinkUserCallsign() {
  if (svxlinkHeardCount > 0 && svxlinkHeardList[0].valid) {
    String call = normalizeHamCallsign(svxlinkHeardList[0].callsign);
    if (call.length()) return call;
  }
  if (heardCount > 0 && heardList[0].valid) {
    String call = normalizeHamCallsign(heardList[0].callsign);
    if (call.length()) return call;
  }
  return "";
}

void clearSvxlinkAprsCommentAlert() {
  aprsCommentPopupOpen = false;
}

void updateSvxlinkAprsCommentAlertFromState() {
  if (isCurrentWatchSvxlink() && svxlinkAprsAlertVisible()) {
    requestRedraw();
  }
}

bool svxlinkAprsAlertVisible() {
  if (!aprsFeatureEnabled() || !isCurrentWatchSvxlink() || !aprsState.valid) return false;
  String currentCall = currentSvxlinkUserCallsign();
  if (!currentCall.length()) return false;
  String aprsCall = normalizeHamCallsign(firstNonEmpty(aprsState.callsign, aprsState.sourceCallsign));
  return aprsCall.length() && currentCall == aprsCall;
}

void openSvxlinkAprsCommentPopup() {
  if (!svxlinkAprsAlertVisible()) return;
  aprsCommentPopupOpen = true;
  requestRedraw();
  noteScreenActivity(false);
}

void drawAprsCommentPopup() {
  const int popupW = UW(270);
  const int popupH = UH(116);
  const int popupX = (SCREEN_W - popupW) / 2;
  const int popupY = (SCREEN_H - popupH) / 2;

  String popupCall = normalizeHamCallsign(firstNonEmpty(aprsState.callsign, aprsState.sourceCallsign));
  String popupText = normalizeSingleLineText(firstNonEmpty(aprsState.comment, aprsState.status, aprsState.path, ""));
  if (!popupText.length()) {
    popupText = String("Location: ") + String(aprsState.lat, 5) + ", " + String(aprsState.lng, 5);
  }
  String popupTime = aprsState.lastSeenText.length() ? aprsState.lastSeenText : String("APRS data available");

  fillSoftPanel(popupX, popupY, popupW, popupH, theme.panel2, theme.accent, 14);

  tft.setTextDatum(MC_DATUM);
  tft.setTextFont(2);
  tft.setTextColor(theme.accent, theme.panel2);
  tft.drawString("APRS COMMENT", popupX + popupW / 2, popupY + UH(17));

  tft.setTextDatum(TL_DATUM);
  tft.setTextFont(2);
  tft.setTextColor(theme.warn, theme.panel2);
  tft.drawString(clipWithTilde(popupCall, 10), popupX + UW(16), popupY + UH(36));

  tft.setTextFont(1);
  tft.setTextColor(theme.subtext, theme.panel2);
  tft.drawString(clipWithTilde(popupTime, 26), popupX + UW(104), popupY + UH(40));

  tft.setTextFont(2);
  tft.setTextColor(theme.text, theme.panel2);
  drawWrappedTextInCurrentFont(popupText, popupX + UW(16), popupY + UH(62), popupW - UW(32), 2, UH(17));

  tft.setTextDatum(MC_DATUM);
  tft.setTextFont(1);
  tft.setTextColor(theme.subtext, theme.panel2);
  tft.drawString("tap to close", popupX + popupW / 2, popupY + popupH - UH(12));
  tft.setTextDatum(TL_DATUM);
}

void drawSvxlinkLastHeardCard(int x, int y, int w, int h) {
  fillSoftPanel(x, y, w, h, theme.panel2, theme.border, 10);

  HeardRecord rec;
  bool hasLastHeard = false;
  if (liveRecord.valid && normalizeHamCallsign(liveRecord.callsign).length()) {
    rec = liveRecord;
    hasLastHeard = true;
  } else if (heardCount > 0 && heardList[0].valid && normalizeHamCallsign(heardList[0].callsign).length()) {
    rec = heardList[0];
    hasLastHeard = true;
  }

  String operatorName = hasLastHeard ? radioIdDisplayNameForRecord(rec, true) : String("");
  String operatorLine = hasLastHeard ? formatCallsignName(rec.callsign, operatorName, 0, 0) : String("No last heard yet");
  String targetLine = hasLastHeard ? resolveTargetDisplay(rec) : String("Waiting for SVXLink activity");
  String timeText = "";
  if (hasLastHeard && rec.timeUtc.length()) {
    timeText = rec.timeUtc.length() > 11 ? rec.timeUtc.substring(11) : rec.timeUtc;
  }

  const int flagW = 32;
  const int flagH = 32;
  int flagX = x + UW(12);
#if DISPLAY_BACKEND_IS_GFX_COMPAT
  int flagY = y + UH(11) - 3;
#else
  int flagY = y + UH(11);
#endif
  flagY = constrain(flagY, y + UH(5), y + h - flagH - UH(5));
  drawFlagForCallsign(flagX, flagY, flagW, flagH, hasLastHeard ? rec.callsign : getDeviceCallsign());

  int textX = flagX + flagW + UW(10);
  int textW = max(UW(40), w - (textX - x) - UW(12));
  int callFont = 4;
  String fittedOperator = fitLivePanelPrimaryText(operatorLine, textW, callFont);

  tft.setTextDatum(ML_DATUM);
  tft.setTextColor(hasLastHeard ? theme.warn : theme.text, theme.panel2);
  tft.setTextFont(callFont);
#if DISPLAY_BACKEND_IS_GFX_COMPAT
  int callY = y + UH(18) + (callFont >= 4 ? UH(1) : UH(0));
#else
  int callY = y + UH(18);
#endif
  tft.drawString(fittedOperator, textX, callY);

  String detailLine = targetLine;
  if (timeText.length()) detailLine += "  " + timeText;
  tft.setTextFont(1);
  tft.setTextColor(theme.subtext, theme.panel2);
  if (!hasLastHeard) tft.drawString(scrollingTextFrameInCurrentFont(detailLine, textW), textX, y + h - UH(10));

  tft.setTextDatum(TL_DATUM);
}

void drawSvxlinkLivePage() {
  drawTopTabs();

  bool transmitting = svxlinkRadioStatus.equalsIgnoreCase("TX") || svxlinkRadioStatus.indexOf("TX") >= 0;
  bool receiving = svxlinkRadioStatus.equalsIgnoreCase("RX") || svxlinkRadioStatus.indexOf("RX") >= 0;
  uint16_t stateColor = transmitting ? theme.tx : (receiving ? theme.rx : theme.good);

  fillSoftPanel(UX(8), UY(38), UW(304), UH(34), theme.panel, theme.border, 12);
  tft.setTextDatum(TL_DATUM);
  tft.setTextFont(2);
  tft.setTextColor(theme.subtext, theme.panel);
  bool aprsButtonReady = svxlinkAprsAlertVisible();
  tft.drawString("Host: " + clipWithTilde(displayBackendHost(), aprsButtonReady ? 14 : 20), UX(18), UY(49));

  if (aprsButtonReady) {
    fillSoftPanel(UX(172), UY(44), UW(48), UH(22), theme.panel2, theme.accent, 8);
    tft.setTextDatum(MC_DATUM);
    tft.setTextFont(2);
    tft.setTextColor(theme.accent, theme.panel2);
    tft.drawString("APRS", UX(196), UY(55));
  }

  bool actionReady = hasConfiguredQuickActions(true);
  fillSoftPanel(UX(226), UY(44), UW(76), UH(22), theme.panel2, actionReady ? theme.good : theme.border, 8);
  tft.setTextDatum(MC_DATUM);
  tft.setTextFont(2);
  tft.setTextColor(actionReady ? theme.good : theme.subtext, theme.panel2);
  tft.drawString("ACTIONS", UX(264), UY(55));

  fillSoftPanel(UX(8), UY(78), UW(146), UH(34), theme.panel2, theme.border, 10);
  tft.setTextDatum(MC_DATUM);
  tft.setTextFont(2);
  tft.setTextColor(stateColor, theme.panel2);
  tft.drawString(clipWithTilde(svxlinkRadioStatus.length() ? svxlinkRadioStatus : "LISTEN", 12), UX(81), UY(90));
  tft.setTextFont(1);
  tft.setTextColor(theme.subtext, theme.panel2);
  tft.drawString("Radio status", UX(81), UY(105));

  fillSoftPanel(UX(166), UY(78), UW(146), UH(34), theme.panel2, theme.border, 10);
  tft.setTextFont(2);
  tft.setTextColor(theme.good, theme.panel2);
  tft.drawString(clipWithTilde(svxlinkReflectorStatus.length() ? svxlinkReflectorStatus : "Unknown", 14), UX(239), UY(90));
  tft.setTextFont(1);
  tft.setTextColor(theme.subtext, theme.panel2);
  tft.drawString("Reflector", UX(239), UY(105));

  drawSvxlinkLastHeardCard(UX(8), UY(118), UW(304), UH(50));

  fillSoftPanel(UX(8), UY(174), UW(304), UH(44), theme.panel, theme.border, 10);
  tft.setTextDatum(TL_DATUM);
  tft.setTextFont(1);
  tft.setTextColor(theme.subtext, theme.panel);
  tft.drawString("TG default", UX(18), UY(181));
  tft.drawString("TG monitor", UX(112), UY(181));
  tft.drawString("TG active", UX(230), UY(181));
  String defaultTgText = svxlinkDefaultTg.length() ? svxlinkDefaultTg : "-";
  String monitorTgsText = svxlinkMonitorTgs.length() ? svxlinkMonitorTgs : "-";
  String activeTgText = svxlinkActiveTg.length() ? svxlinkActiveTg : "-";
  tft.setTextFont(2);
  tft.setTextColor(theme.text, theme.panel);
  tft.drawString(defaultTgText, UX(18), UY(199));
  tft.setTextColor(theme.warn, theme.panel);
  drawScrollingTextInCurrentFont(monitorTgsText, UX(112), UY(199), UW(108));
  tft.setTextColor(theme.accent, theme.panel);
  tft.drawString(activeTgText, UX(230), UY(199));

  drawFooter();
}

void drawSvxlinkHeardPage() {
  drawTopTabs();
  normalizeSvxlinkHeardBrowseState();

  fillSoftPanel(UX(6), UY(36), UW(308), UH(184), theme.panel, theme.border, 12);
  tft.setTextDatum(TL_DATUM);
  tft.setTextFont(2);
  tft.setTextColor(theme.accent, theme.panel);
  tft.drawString("SVXLINK HEARD", UX(16), UY(46));

  tft.setTextFont(1);
  tft.setTextColor(theme.subtext, theme.panel);
  String sub = svxlinkReflectorStatus.length() ? ("Reflector: " + svxlinkReflectorStatus) : "Tap rows for details";
  tft.drawString(clipWithTilde(sub, 42), UX(16), UY(64));

  int pageCount = heardCount > 0 ? ((heardCount + LAST_HEARD_ROWS_PER_PAGE - 1) / LAST_HEARD_ROWS_PER_PAGE) : 0;
  int pageStart = lastHeardPageIndex * LAST_HEARD_ROWS_PER_PAGE;
  int rows = max(0, min((int)LAST_HEARD_ROWS_PER_PAGE, heardCount - pageStart));

  const int navY = UY(76);
  fillSoftPanel(UX(14), navY, UW(24), UH(20), theme.panel2, theme.border, 6);
  fillSoftPanel(UX(44), navY, UW(170), UH(20), theme.panel2, theme.border, 6);
  fillSoftPanel(UX(220), navY, UW(56), UH(20), theme.panel2, theme.border, 6);
  fillSoftPanel(UX(282), navY, UW(24), UH(20), theme.panel2, theme.border, 6);

  tft.setTextDatum(MC_DATUM);
  tft.setTextFont(2);
  tft.setTextColor(lastHeardPageIndex > 0 ? theme.text : theme.subtext, theme.panel2);
  tft.drawString("<", UX(26), navY + UH(10));
  tft.setTextColor(theme.warn, theme.panel2);
  String statusText = svxlinkReflectorStatus.length() ? svxlinkReflectorStatus : String("SVXLINK");
  tft.drawString(clipWithTilde(statusText, 22), UX(129), navY + UH(10));
  tft.setTextColor(theme.subtext, theme.panel2);
  String pageText = heardCount > 0 ? (String(lastHeardPageIndex + 1) + "/" + String(max(1, pageCount))) : String("--");
  tft.drawString(pageText, UX(248), navY + UH(10));
  tft.setTextColor((lastHeardPageIndex + 1) < pageCount ? theme.text : theme.subtext, theme.panel2);
  tft.drawString(">", UX(294), navY + UH(10));

  int y = UY(104);
  for (int row = 0; row < rows; row++) {
    HeardRecord& r = heardList[pageStart + row];
    uint16_t lineBg = (row % 2 == 0) ? theme.panel2 : mix565(theme.panel2, theme.bg, 0.18f);
    fillSoftPanel(UX(14), y, UW(292), UH(24), lineBg, theme.border, 6);

    tft.setTextDatum(TL_DATUM);
    tft.setTextFont(2);
    tft.setTextColor(theme.accent, lineBg);
    tft.drawString(clipWithTilde(normalizeHamCallsign(r.callsign), 9), UX(20), y + UY(5));
    tft.setTextColor(theme.warn, lineBg);
    drawScrollingTextInCurrentFont(resolveTargetDisplay(r), UX(98), y + UY(5), UW(132));
    tft.setTextColor(theme.subtext, lineBg);
    String timeText = r.timeUtc.length() > 11 ? r.timeUtc.substring(11) : r.timeUtc;
    tft.drawString(clipWithTilde(timeText, 8), UX(238), y + UY(5));
    y += UH(28);
  }

  if (heardCount == 0) {
    tft.setTextColor(theme.subtext, theme.panel);
    tft.setTextFont(2);
    tft.drawString("No SVXLink traffic yet", UX(18), UY(118));
  }

  drawFooter();
}

void drawAprsLivePage() {
  drawTopTabs();

  fillSoftPanel(UX(8), UY(36), UW(304), UH(184), theme.panel, theme.border, 12);
  tft.setTextDatum(TL_DATUM);
  tft.setTextFont(2);
  tft.setTextColor(theme.accent, theme.panel);
  tft.drawString("APRS LOCATION", UX(18), UY(46));

  fillSoftPanel(UX(222), UY(48), UW(78), UH(22), theme.panel2, theme.border, 7);
  tft.setTextDatum(MC_DATUM);
  tft.setTextFont(1);
  tft.setTextColor(theme.text, theme.panel2);
  tft.drawString("REFRESH", UX(261), UY(59));
  tft.setTextDatum(TL_DATUM);

  tft.setTextFont(1);
  tft.setTextColor(theme.subtext, theme.panel);
  tft.drawString("Data source: aprs.fi", UX(18), UY(65));

  if (!aprsFeatureEnabled()) {
    tft.setTextFont(2);
    tft.setTextColor(theme.warn, theme.panel);
    tft.drawString("Enable APRS in Web UI", UX(18), UY(96));
    tft.setTextFont(1);
    tft.setTextColor(theme.subtext, theme.panel);
    tft.drawString("Application > APRS", UX(18), UY(116));
    drawFooter();
    return;
  }

  String lookupCall = aprsLookupCallsign();
  if (!lookupCall.length()) lookupCall = "WAITING";
  bool hasDataForCall = aprsState.valid && aprsState.callsign.length() && (aprsState.sourceCallsign == lookupCall || aprsState.callsign == lookupCall);

  const int aprsBoxX = UX(16);
  const int aprsBoxY = UY(82);
  const int aprsBoxW = UW(288);
  const int aprsBoxH = UH(42);
  fillSoftPanel(aprsBoxX, aprsBoxY, aprsBoxW, aprsBoxH, theme.panel2, hasDataForCall ? theme.good : theme.warn, 10);

  tft.setTextFont(4);
  tft.setTextColor(hasDataForCall ? theme.good : theme.warn, theme.panel2);
  String shownAprsCall = hasDataForCall ? clipWithTilde(aprsState.callsign, 10) : clipWithTilde(lookupCall, 10);
  const int aprsCenterY = aprsBoxY + aprsBoxH / 2;

  if (hasDataForCall) {
    const int iconW = 32;
    const int iconH = 32;
    const int iconGap = UW(10);
    int callW = tft.textWidth(shownAprsCall);
    int groupW = iconW + iconGap + callW;
    int groupX = aprsBoxX + (aprsBoxW - groupW) / 2;
    if (groupX < aprsBoxX + UX(6)) groupX = aprsBoxX + UX(6);
    int aprsIconX = groupX;
    int aprsIconY = aprsBoxY + (aprsBoxH - iconH) / 2;
    drawAprsIcon(aprsState.symbol, aprsIconX, aprsIconY, theme.panel2);

    tft.setTextDatum(ML_DATUM);
    tft.drawString(shownAprsCall, aprsIconX + iconW + iconGap, aprsCenterY);
  } else {
    tft.setTextDatum(MC_DATUM);
    tft.drawString(shownAprsCall, aprsBoxX + aprsBoxW / 2, aprsCenterY);
  }
  tft.setTextDatum(TL_DATUM);

  if (hasDataForCall) {
    String coord = String(aprsState.lat, 5) + ", " + String(aprsState.lng, 5);
    drawInfoRow(UX(16), UY(132), UW(288), "COORD", coord, theme.panel, theme.panel2, theme.accent, theme.text);

    String moveSpeed = aprsState.speedValid ? (String(aprsState.speedKmh, 0) + " Km/h") : String("-");
    String moveCourse = aprsState.courseValid ? (String(aprsState.courseDeg) + " deg") : String("-");
    String moveAlt = aprsState.altitudeValid ? (String(aprsState.altitudeM, 0) + " m") : String("-");
    String move = moveSpeed + " / " + moveCourse + " / ALT: " + moveAlt;
    drawInfoRowSized(UX(16), UY(156), UW(288), UW(50), "MOVE", move, theme.panel, theme.panel2, theme.warn, theme.text);

    String detail = firstNonEmpty(aprsState.comment, aprsState.status, aprsState.path, "No APRS comment");
    fillSoftPanel(UX(16), UY(182), UW(288), UH(36), theme.panel2, theme.border, 8);
    tft.setTextDatum(TL_DATUM);
    tft.setTextFont(1);
    tft.setTextColor(theme.accent, theme.panel2);
    tft.drawString("COMMENT", UX(24), UY(188));
    tft.setTextFont(2);
    tft.setTextColor(theme.text, theme.panel2);
    drawWrappedTextInCurrentFont(detail, UX(80), UY(185), UW(216), 2, UH(15));
  } else {
    tft.setTextFont(2);
    tft.setTextColor(theme.subtext, theme.panel);
    tft.drawString(aprsState.fetchInProgress ? "Fetching APRS.fi ..." : "Waiting for next APRS poll", UX(18), UY(140));
    if (aprsState.error.length()) {
      tft.setTextFont(1);
      tft.setTextColor(theme.bad, theme.panel);
      tft.drawString(clipWithTilde(aprsState.error, 42), UX(18), UY(164));
    }
  }

  drawFooter();
}


void redrawScrollingTextFieldPartial(const String& value, int x, int y, int maxWidth, int clearHeight,
                                     uint8_t font, uint8_t datum, uint16_t fg, uint16_t bg) {
  if (maxWidth <= 0 || clearHeight <= 0) return;

  int clearY = y;
  if (datum == ML_DATUM || datum == MC_DATUM || datum == MR_DATUM) {
    clearY = y - clearHeight / 2;
  }

  tft.fillRect(x, clearY, maxWidth, clearHeight, bg);
  tft.setTextDatum(datum);
  tft.setTextFont(font);
  tft.setTextColor(fg, bg);
  tft.drawString(scrollingTextFrameInCurrentFont(value, maxWidth), x, y);
  tft.setTextDatum(TL_DATUM);
}

void redrawCompactScrollingValueBoxPartial(int x, int y, int w, int h,
                                           const String& value, uint16_t valueColor) {
  int valueX = x + UW(8);
  int valueW = max(UW(12), w - UW(16));
  int clearH = max(1, h - max(2, UH(4)));

  redrawScrollingTextFieldPartial(value.length() ? value : String("-"), valueX, y + h / 2,
                                  valueW, clearH, 2, ML_DATUM, valueColor, theme.panel2);
}

void drawWeatherMetricCard(int x, int y, int w, int h, const String& label, const String& value, uint16_t valueColor) {
  uint16_t cardFill = mix565(theme.panel2, theme.accent, 0.05f);
  uint16_t cardBorder = mix565(theme.border, theme.accent, 0.22f);
  fillSoftPanel(x, y, w, h, cardFill, cardBorder, 9);
  tft.fillRoundRect(x + UW(5), y + UH(5), max(2, w - UW(10)), max(1, UH(3)), UR(2), mix565(theme.accent, TFT_WHITE, 0.10f));

  tft.setTextDatum(TL_DATUM);
  tft.setTextFont(1);
  tft.setTextColor(theme.subtext, cardFill);
  tft.drawString(fitTextInCurrentFont(label, w - UW(14)), x + UW(8), y + UH(11));

  tft.setTextFont(2);
  tft.setTextColor(valueColor, cardFill);
  tft.drawString(fitTextInCurrentFont(value.length() ? value : String("-"), w - UW(14)), x + UW(8), y + UH(26));
}

void drawHamqslConditionPill(int x, int y, int w, int h, const String& label, const String& value) {
  uint16_t fill = mix565(theme.panel2, hamqslConditionColor(value), 0.08f);
  uint16_t border = mix565(theme.border, hamqslConditionColor(value), 0.35f);
  fillSoftPanel(x, y, w, h, fill, border, 7);
  tft.setTextDatum(TL_DATUM);
  tft.setTextFont(1);
  tft.setTextColor(theme.subtext, fill);
  tft.drawString(fitTextInCurrentFont(label, w - UW(8)), x + UW(5), y + UH(4));
  tft.setTextFont(2);
  tft.setTextColor(hamqslConditionColor(value), fill);
  tft.drawString(fitTextInCurrentFont(value.length() ? value : String("-"), w - UW(8)), x + UW(5), y + UH(16));
}

void drawHamqslLivePage() {
  drawTopTabs();

  const int pageX = UX(8);
  const int pageY = UY(36);
  const int pageW = UW(304);
  const int pageH = UH(184);
  uint16_t softPanel = mix565(theme.panel, theme.accent, 0.04f);
  fillSoftPanel(pageX, pageY, pageW, pageH, softPanel, theme.border, 13);

  tft.setTextDatum(TL_DATUM);
  tft.setTextFont(2);
  tft.setTextColor(theme.accent, softPanel);
  tft.drawString("HAMQSL WATCH", UX(18), UY(45));

  const int refreshX = UX(222);
  const int refreshY = UY(44);
  const int refreshW = UW(78);
  const int refreshH = UH(22);
  fillSoftPanel(refreshX, refreshY, refreshW, refreshH, mix565(theme.panel2, theme.accent, 0.10f), mix565(theme.border, theme.accent, 0.35f), 7);
  tft.setTextDatum(MC_DATUM);
  tft.setTextFont(1);
  tft.setTextColor(theme.text, mix565(theme.panel2, theme.accent, 0.10f));
  tft.drawString("REFRESH", refreshX + refreshW / 2, refreshY + refreshH / 2);
  tft.setTextDatum(TL_DATUM);

  tft.setTextFont(1);
  tft.setTextColor(theme.subtext, softPanel);
  String meta = hamqslConfigured() ? (String("HAMQSL/N0NBH - ") + hamqslLastUpdatedText()) : String("Configure HAMQSL in Web UI");
  tft.drawString(fitTextInCurrentFont(meta, UW(196)), UX(18), UY(64));

  if (!hamqslFeatureEnabled()) {
    fillSoftPanel(UX(16), UY(86), UW(288), UH(100), theme.panel2, mix565(theme.warn, theme.border, 0.50f), 12);
    tft.setTextDatum(MC_DATUM);
    tft.setTextFont(4);
    tft.setTextColor(theme.warn, theme.panel2);
    tft.drawString("SOL", UX(160), UY(112));
    tft.setTextFont(2);
    tft.setTextColor(theme.text, theme.panel2);
    tft.drawString("HAMQSL is disabled", UX(160), UY(145));
    tft.setTextFont(1);
    tft.setTextColor(theme.subtext, theme.panel2);
    tft.drawString("Enable it in Web UI > HAMQSL", UX(160), UY(166));
    tft.setTextDatum(TL_DATUM);
    drawFooter();
    return;
  }

  if (!hamqslConfigured()) {
    fillSoftPanel(UX(16), UY(82), UW(288), UH(72), theme.panel2, mix565(theme.warn, theme.border, 0.45f), 12);
    tft.setTextDatum(TL_DATUM);
    tft.setTextFont(2);
    tft.setTextColor(theme.warn, theme.panel2);
    tft.drawString("HAMQSL setup incomplete", UX(28), UY(96));
    tft.setTextFont(1);
    tft.setTextColor(theme.subtext, theme.panel2);
    tft.drawString("Add the solar XML URL in Web UI.", UX(28), UY(120));
    drawInfoRowSized(UX(16), UY(170), UW(288), UW(66), "SOURCE", trimSafe(cfg.hamqslUrl).length() ? "Ready" : "Missing", softPanel, theme.panel2, theme.warn, theme.text);
    drawFooter();
    return;
  }

  if (!hamqslState.valid) {
    fillSoftPanel(UX(16), UY(82), UW(288), UH(86), theme.panel2, hamqslState.fetchInProgress ? theme.good : mix565(theme.warn, theme.border, 0.35f), 12);
    tft.setTextDatum(MC_DATUM);
    tft.setTextFont(2);
    tft.setTextColor(hamqslState.fetchInProgress ? theme.good : theme.warn, theme.panel2);
    tft.drawString(hamqslState.fetchInProgress ? "Fetching HAMQSL..." : "No HAMQSL data yet", UX(160), UY(108));
    tft.setTextFont(1);
    tft.setTextColor(theme.subtext, theme.panel2);
    String msg = hamqslState.error.length() ? hamqslState.error : (hamqslState.lastHttpCode ? (String("Last HTTP: ") + String(hamqslState.lastHttpCode)) : String("Tap REFRESH or wait for auto update"));
    tft.drawString(fitTextInCurrentFont(msg, UW(250)), UX(160), UY(132));
    tft.setTextDatum(TL_DATUM);
    drawInfoRowSized(UX(16), UY(176), UW(288), UW(68), "SOURCE", "HAMQSL XML", softPanel, theme.panel2, theme.warn, theme.text);
    drawFooter();
    return;
  }

  drawWeatherMetricCard(UX(16), UY(76), UW(68), UH(46), "SFI", hamqslState.solarFlux, theme.text);
  drawWeatherMetricCard(UX(90), UY(76), UW(68), UH(46), "SUN", hamqslState.sunspots, theme.text);
  String ak = String("A") + (hamqslState.aIndex.length() ? hamqslState.aIndex : String("-")) + " K" + (hamqslState.kIndex.length() ? hamqslState.kIndex : String("-"));
  drawWeatherMetricCard(UX(164), UY(76), UW(68), UH(46), "A/K", ak, theme.text);
  drawWeatherMetricCard(UX(238), UY(76), UW(66), UH(46), "NOISE", hamqslState.signalNoise.length() ? hamqslState.signalNoise : hamqslState.geomagField, hamqslConditionColor(hamqslState.signalNoise.length() ? hamqslState.signalNoise : hamqslState.geomagField));

  fillSoftPanel(UX(16), UY(128), UW(288), UH(70), mix565(theme.panel2, theme.accent, 0.06f), mix565(theme.border, theme.accent, 0.30f), 10);
  tft.setTextDatum(TL_DATUM);
  tft.setTextFont(1);
  tft.setTextColor(theme.accent, mix565(theme.panel2, theme.accent, 0.06f));
  tft.drawString("HF BAND CONDITIONS", UX(24), UY(135));
  tft.setTextColor(theme.subtext, mix565(theme.panel2, theme.accent, 0.06f));
  tft.drawString("DAY", UX(158), UY(135));
  tft.drawString("NIGHT", UX(222), UY(135));

  for (uint8_t i = 0; i < hamqslState.hfBandCount && i < 4; i++) {
    int y = UY(150) + i * UH(11);
    const HamqslBandCondition& b = hamqslState.hfBands[i];
    tft.setTextFont(1);
    tft.setTextColor(theme.text, mix565(theme.panel2, theme.accent, 0.06f));
    tft.drawString(b.name, UX(24), y);
    tft.setTextColor(hamqslConditionColor(b.day), mix565(theme.panel2, theme.accent, 0.06f));
    tft.drawString(fitTextInCurrentFont(b.day.length() ? b.day : String("-"), UW(58)), UX(158), y);
    tft.setTextColor(hamqslConditionColor(b.night), mix565(theme.panel2, theme.accent, 0.06f));
    tft.drawString(fitTextInCurrentFont(b.night.length() ? b.night : String("-"), UW(58)), UX(222), y);
  }

  String lower = String("X ") + (hamqslState.xray.length() ? hamqslState.xray : String("-"));
  if (hamqslState.muf.length()) lower += "  MUF " + hamqslState.muf;
  else if (hamqslState.fof2.length()) lower += "  foF2 " + hamqslState.fof2;
  if (hamqslState.vhfConditionCount > 0) {
    const HamqslVhfCondition& v = hamqslState.vhfConditions[0];
    lower += "  " + v.name + ": " + v.condition;
  }
  tft.setTextFont(1);
  tft.setTextColor(theme.subtext, softPanel);
  tft.drawString(fitTextInCurrentFont(lower, UW(286)), UX(18), UY(204));

  drawFooter();
}

void drawWeatherLivePage() {
  drawTopTabs();

  const int pageX = UX(8);
  const int pageY = UY(36);
  const int pageW = UW(304);
  const int pageH = UH(184);
  const int pageR = 13;
  uint16_t softPanel = mix565(theme.panel, theme.accent, 0.04f);
  fillSoftPanel(pageX, pageY, pageW, pageH, softPanel, theme.border, pageR);

  tft.setTextDatum(TL_DATUM);
  tft.setTextFont(2);
  tft.setTextColor(theme.accent, softPanel);
  tft.drawString("WEATHER WATCH", UX(18), UY(45));

  const int refreshX = UX(222);
  const int refreshY = UY(44);
  const int refreshW = UW(78);
  const int refreshH = UH(22);
  fillSoftPanel(refreshX, refreshY, refreshW, refreshH, mix565(theme.panel2, theme.accent, 0.10f), mix565(theme.border, theme.accent, 0.35f), 7);
  tft.setTextDatum(MC_DATUM);
  tft.setTextFont(1);
  tft.setTextColor(theme.text, mix565(theme.panel2, theme.accent, 0.10f));
  tft.drawString("REFRESH", refreshX + refreshW / 2, refreshY + refreshH / 2);
  tft.setTextDatum(TL_DATUM);

  tft.setTextFont(1);
  tft.setTextColor(theme.subtext, softPanel);
  String headerMeta = weatherConfigured() ? ("Updated " + weatherLastUpdatedText()) : String("Configure OpenWeather in Web UI");
  tft.drawString(fitTextInCurrentFont(headerMeta, UW(196)), UX(18), UY(64));

  if (!weatherFeatureEnabled()) {
    fillSoftPanel(UX(16), UY(86), UW(288), UH(100), theme.panel2, mix565(theme.warn, theme.border, 0.50f), 12);
    tft.setTextDatum(MC_DATUM);
    tft.setTextFont(4);
    tft.setTextColor(theme.warn, theme.panel2);
    tft.drawString("WX", UX(160), UY(112));
    tft.setTextFont(2);
    tft.setTextColor(theme.text, theme.panel2);
    tft.drawString("Weather is disabled", UX(160), UY(145));
    tft.setTextFont(1);
    tft.setTextColor(theme.subtext, theme.panel2);
    tft.drawString("Enable it in Web UI > Settings > Weather", UX(160), UY(166));
    tft.setTextDatum(TL_DATUM);
    drawFooter();
    return;
  }

  if (!weatherConfigured()) {
    fillSoftPanel(UX(16), UY(82), UW(288), UH(58), theme.panel2, mix565(theme.warn, theme.border, 0.45f), 12);
    tft.setTextDatum(TL_DATUM);
    tft.setTextFont(2);
    tft.setTextColor(theme.warn, theme.panel2);
    tft.drawString("Weather setup incomplete", UX(28), UY(94));
    tft.setTextFont(1);
    tft.setTextColor(theme.subtext, theme.panel2);
    tft.drawString("Add latitude, longitude and API key.", UX(28), UY(116));

    drawInfoRowSized(UX(16), UY(150), UW(288), UW(66), "LAT/LON", trimSafe(cfg.weatherLat).length() && trimSafe(cfg.weatherLon).length() ? "Ready" : "Missing", softPanel, theme.panel2, theme.warn, theme.text);
    drawInfoRowSized(UX(16), UY(176), UW(288), UW(66), "API KEY", trimSafe(cfg.weatherApiKey).length() ? "Ready" : "Missing", softPanel, theme.panel2, theme.warn, theme.text);
    drawFooter();
    return;
  }

  if (!weatherState.valid) {
    fillSoftPanel(UX(16), UY(82), UW(288), UH(80), theme.panel2, weatherState.fetchInProgress ? theme.good : mix565(theme.warn, theme.border, 0.35f), 12);
    tft.setTextDatum(MC_DATUM);
    tft.setTextFont(2);
    tft.setTextColor(weatherState.fetchInProgress ? theme.good : theme.warn, theme.panel2);
    tft.drawString(weatherState.fetchInProgress ? "Fetching weather..." : "No weather data yet", UX(160), UY(108));
    tft.setTextFont(1);
    tft.setTextColor(theme.subtext, theme.panel2);
    String msg = weatherState.lastHttpCode ? ("Last HTTP: " + String(weatherState.lastHttpCode)) : String("Tap REFRESH or wait for auto update");
    tft.drawString(msg, UX(160), UY(132));
    tft.setTextDatum(TL_DATUM);

    drawInfoRowSized(UX(16), UY(176), UW(288), UW(68), "LOCATION", trimSafe(cfg.weatherLat) + ", " + trimSafe(cfg.weatherLon), softPanel, theme.panel2, theme.warn, theme.text);
    drawFooter();
    return;
  }

  const int heroX = UX(16);
  const int heroY = UY(78);
  const int heroW = UW(288);
  const int heroH = UH(84);
  uint16_t heroFill = mix565(theme.panel2, theme.accent, 0.08f);
  uint16_t heroBorder = mix565(theme.border, theme.accent, 0.38f);
  fillSoftPanel(heroX, heroY, heroW, heroH, heroFill, heroBorder, 13);

  const int iconTileX = heroX + UW(8);
  const int iconTileY = heroY + UH(10);
  const int iconTileW = UW(66);
  const int iconTileH = UH(62);
  uint16_t iconFill = mix565(theme.panel, theme.accent, 0.10f);
  fillSoftPanel(iconTileX, iconTileY, iconTileW, iconTileH, iconFill, mix565(theme.border, theme.accent, 0.30f), 12);

  String iconPath = weatherIconPath32();
  if (iconPath.length()) {
#if DISPLAY_BACKEND_IS_GFX_COMPAT
    drawPngFromSD(iconPath.c_str(), iconTileX + (iconTileW - 32) / 2, iconTileY + 4 + (iconTileH - 48) / 2 + 4, 32, 32, iconFill);
#else
    drawPngFromSD(iconPath.c_str(), iconTileX + 2 + (iconTileW - 32) / 2, iconTileY + 8 + (iconTileH - 48) / 2, 32, 32, iconFill);
#endif
  } else {
    tft.setTextDatum(MC_DATUM);
    tft.setTextFont(4);
    tft.setTextColor(theme.accent, iconFill);
    tft.drawString("WX", iconTileX + iconTileW / 2, iconTileY + iconTileH / 2);
    tft.setTextDatum(TL_DATUM);
  }

  const int textX = iconTileX + iconTileW + UW(12);
  const int textW = heroW - (textX - heroX) - UW(12);
  String place = weatherState.location.length() ? weatherState.location : String("Weather");

  tft.setTextDatum(TL_DATUM);
  tft.setTextFont(1);
  tft.setTextColor(theme.subtext, heroFill);
  tft.drawString(fitTextInCurrentFont(place, textW), textX, heroY + UH(8));

  tft.setTextFont(4);
  tft.setTextColor(theme.good, heroFill);
  String tempText = String((int)lroundf(weatherState.tempC)) + " C";
  tft.drawString(fitTextInCurrentFont(tempText, textW), textX, heroY + UH(23));

  tft.setTextFont(2);
  tft.setTextColor(theme.text, heroFill);
#if DISPLAY_BACKEND_IS_GFX_COMPAT
  tft.drawString(fitTextInCurrentFont(weatherDetailSummary(), textW), textX, heroY + UH(50));
#else
  tft.drawString(fitTextInCurrentFont(weatherDetailSummary(), textW), textX, heroY + UH(46));
#endif

  tft.setTextFont(1);
  tft.setTextColor(theme.subtext, heroFill);
  String windAndRange = "Wind " + weatherWindText() + "  L/H " + String((int)lroundf(weatherState.tempMinC)) + "/" + String((int)lroundf(weatherState.tempMaxC)) + " C";
  tft.drawString(fitTextInCurrentFont(windAndRange, textW), textX, heroY + UH(70));

  drawWeatherMetricCard(UX(16), UY(168), UW(68), UH(48), "FEELS", String((int)lroundf(weatherState.feelsLikeC)) + " C", theme.text);
  drawWeatherMetricCard(UX(90), UY(168), UW(68), UH(48), "HUMID", String(weatherState.humidity) + "%", theme.text);
  drawWeatherMetricCard(UX(164), UY(168), UW(68), UH(48), "HPA", weatherState.pressureHpa > 0 ? String(weatherState.pressureHpa) : String("-"), theme.text);
  drawWeatherMetricCard(UX(238), UY(168), UW(66), UH(48), "VIS", weatherVisibilityText(), theme.text);

  drawFooter();
}

bool redrawMmdvmLiveScrollingTextAreas() {
  if (currentPage != PAGE_LIVE || isSvxlinkBackend() || isAprsBackend() || isWeatherBackend() || isHamqslBackend()) return false;
  if (detailPopupOpen || modeConfirmPopupOpen || quickActionPopupOpen || watchSourcePopupOpen || aprsCommentPopupOpen) return false;

  HeardRecord rec;
  bool active = isLiveActive();
  if (active) rec = liveRecord;

  if (shouldShowDmrTimeslotLivePanel(active, rec)) {
    drawDmrTimeslotLivePanel(UX(10), UY(112), UW(300), UH(68));
    return true;
  }

  String statusLine = liveStatusText(active, rec);
  String targetLine = active ? resolveTargetDisplay(rec) : String("-");

  int panelX = UX(10);
  int panelY = UY(112);
  int panelW = UW(300);
  int panelH = UH(68);
  int metaY = panelY + panelH - UH(28);
  int statusW = (panelW - UW(22)) / 2;
  int targetW = panelW - UW(22) - statusW;
  int statusX = panelX + UW(8);
  int targetX = statusX + statusW + UW(6);
  int metaH = UH(24);

  redrawCompactScrollingValueBoxPartial(statusX, metaY, statusW, metaH,
                                        statusLine, active ? theme.good : theme.text);
  redrawCompactScrollingValueBoxPartial(targetX, metaY, targetW, metaH,
                                        targetLine, theme.text);
  return true;
}

void redrawSvxlinkLiveLastHeardScrollerPartial() {
  int x = UX(8);
  int y = UY(118);
  int w = UW(304);
  int h = UH(50);

  bool hasLastHeard = heardCount > 0;
  HeardRecord rec;
  if (hasLastHeard) rec = heardList[0];

  String targetLine = hasLastHeard ? resolveTargetDisplay(rec) : String("Waiting for SVXLink activity");
  String timeText = "";
  if (hasLastHeard && rec.timeUtc.length()) {
    timeText = rec.timeUtc.length() > 11 ? rec.timeUtc.substring(11) : rec.timeUtc;
  }

  String detailLine = targetLine;
  if (timeText.length()) detailLine += "  " + timeText;

  const int flagW = 32;
  int flagX = x + UW(12);
  int textX = flagX + flagW + UW(10);
  int textW = max(UW(40), w - (textX - x) - UW(12));

  redrawScrollingTextFieldPartial(detailLine, textX, y + h - UH(14),
                                  textW, max(8, UH(12)), 1, TL_DATUM,
                                  theme.subtext, theme.panel2);
}

bool redrawSvxlinkLiveScrollingTextAreas() {
  if (currentPage != PAGE_LIVE || !isSvxlinkBackend()) return false;
  if (detailPopupOpen || modeConfirmPopupOpen || quickActionPopupOpen || watchSourcePopupOpen || aprsCommentPopupOpen) return false;

  redrawSvxlinkLiveLastHeardScrollerPartial();

  String defaultTgText = svxlinkDefaultTg.length() ? svxlinkDefaultTg : String("-");
  String monitorTgsText = svxlinkMonitorTgs.length() ? svxlinkMonitorTgs : String("-");
  String activeTgText = svxlinkActiveTg.length() ? svxlinkActiveTg : String("-");

  redrawScrollingTextFieldPartial(defaultTgText, UX(18), UY(199), UW(84),
                                  max(12, UH(17)), 2, TL_DATUM, theme.text, theme.panel);
  redrawScrollingTextFieldPartial(monitorTgsText, UX(112), UY(199), UW(108),
                                  max(12, UH(17)), 2, TL_DATUM, theme.warn, theme.panel);
  redrawScrollingTextFieldPartial(activeTgText, UX(230), UY(199), UW(72),
                                  max(12, UH(17)), 2, TL_DATUM, theme.accent, theme.panel);
  return true;
}

bool redrawSvxlinkHeardScrollingTextAreas() {
  if (currentPage != PAGE_LAST || !isSvxlinkBackend()) return false;
  if (detailPopupOpen || modeConfirmPopupOpen || quickActionPopupOpen || watchSourcePopupOpen || aprsCommentPopupOpen) return false;

  normalizeSvxlinkHeardBrowseState();
  int pageStart = lastHeardPageIndex * LAST_HEARD_ROWS_PER_PAGE;
  int rows = max(0, min((int)LAST_HEARD_ROWS_PER_PAGE, heardCount - pageStart));
  int y = UY(104);

  for (int row = 0; row < rows; row++) {
    HeardRecord& r = heardList[pageStart + row];
    uint16_t lineBg = (row % 2 == 0) ? theme.panel2 : mix565(theme.panel2, theme.bg, 0.18f);
    redrawScrollingTextFieldPartial(resolveTargetDisplay(r), UX(98), y + UY(5), UW(132),
                                    max(12, UH(17)), 2, TL_DATUM, theme.warn, lineBg);
    y += UH(28);
  }

  return true;
}

bool redrawScrollingTextAreasForCurrentPage() {
  if (screensaverActive) return false;
  if (currentPage == PAGE_LIVE) {
    if (isSvxlinkBackend()) return redrawSvxlinkLiveScrollingTextAreas();
    if (isAprsBackend() || isWeatherBackend() || isHamqslBackend()) return false;
    return redrawMmdvmLiveScrollingTextAreas();
  }
  if (currentPage == PAGE_LAST && isSvxlinkBackend()) {
    return redrawSvxlinkHeardScrollingTextAreas();
  }
  return false;
}


bool fetchModeStatus() {
  if (WiFi.status() != WL_CONNECTED) {
    wifiOk = false;
    setFooterText("Wi-Fi disconnected");
    requestRedraw();
    noteApiFetchResult(false, -1, 0);
    return false;
  }

  unsigned long startMs = millis();
  bool ok = false;
  String payload;
  int code = 0;

  if (httpGetString(dashboardStatusUrl(), payload, code)) {
    ok = parseDashboardStatusHtml(payload) || ok;
  }

  String freqUrl = dashboardFreqUrl();
  if (freqUrl != dashboardStatusUrl()) {
    payload = "";
    code = 0;
    if (httpGetString(freqUrl, payload, code)) {
      ok = parseDashboardStatusHtml(payload) || ok;
    }
  }

  if (!ok) {
    if (code == -1) setFooterText("Dashboard begin failed");
    else if (code != 0 && code != 200) setFooterText("Dashboard HTTP: " + String(code));
    else setFooterText("Dashboard parse failed");
  }

  noteApiFetchResult(ok, code, millis() - startMs);
  return ok;
}

bool fetchLastHeard() {
  if (WiFi.status() != WL_CONNECTED) {
    wifiOk = false;
    setFooterText("Wi-Fi disconnected");
    requestRedraw();
    noteApiFetchResult(false, -1, 0);
    return false;
  }

  unsigned long startMs = millis();
  int code = 0;
  String url = heardUrl();
  if (!url.length()) {
    setFooterText("Backend host missing");
    requestRedraw();
    noteApiFetchResult(false, -1, millis() - startMs);
    return false;
  }

  String payload;
  bool httpOk = httpGetString(url, payload, code, "application/json");
  if (!httpOk) {
    if (code == -1) setFooterText("HTTP begin failed");
    else if (code == -98) setFooterText("Network busy");
    else setFooterText("HTTP error: " + String(code));
    requestRedraw();
    noteApiFetchResult(false, code, millis() - startMs);
    return false;
  }

  bool ok = parseHeardArray(payload);
  noteApiFetchResult(ok, code, millis() - startMs);
  return ok;
}

bool fetchAllData() {
  if (isCurrentWatchWeather()) {
    unsigned long weatherRefreshMs = max(60000UL, (unsigned long)cfg.weatherRefreshMinutes * 60000UL);
    if (!weatherState.lastAttemptMillis || millis() - weatherState.lastAttemptMillis >= weatherRefreshMs) {
      return fetchWeatherData();
    }
    return false;
  }

  if (isCurrentWatchHamqsl()) {
    unsigned long hamqslRefreshMs = max((unsigned long)HAMQSL_REFRESH_MINUTES_MIN * 60000UL, (unsigned long)cfg.hamqslRefreshMinutes * 60000UL);
    if (!hamqslState.lastAttemptMillis || millis() - hamqslState.lastAttemptMillis >= hamqslRefreshMs) {
      return fetchHamqslData();
    }
    return false;
  }

  if (isCurrentWatchSvxlink()) {
    bool svxOk = fetchSvxlinkData();
    bool aprsOk = aprsFeatureEnabled() ? fetchAprsData(false) : false;
    return svxOk || aprsOk;
  }

  bool ok = false;

  if (nextFetchHeard) {
    ok = fetchLastHeard();
  } else {
    ok = fetchModeStatus();
  }

  if (ok) saveActiveHeardCache();
  nextFetchHeard = !nextFetchHeard;

  if (isCurrentWatchAprs()) {
    bool aprsOk = fetchAprsData(false);
    return ok || aprsOk;
  }

  return ok;
}


#ifndef CYD_TOUCH_SWAP_XY
  #define CYD_TOUCH_SWAP_XY 1
#endif
#ifndef CYD_TOUCH_FLIP_X
  #define CYD_TOUCH_FLIP_X 0
#endif
#ifndef CYD_TOUCH_FLIP_Y
  #define CYD_TOUCH_FLIP_Y 1
#endif

static const bool TOUCH_SWAP_XY = (CYD_TOUCH_SWAP_XY != 0);
static const bool TOUCH_FLIP_X  = (CYD_TOUCH_FLIP_X != 0);
static const bool TOUCH_FLIP_Y  = (CYD_TOUCH_FLIP_Y != 0);
static const unsigned long TOUCH_DEBOUNCE_MS = 220;
static const unsigned long TOUCH_POLL_GRACE_MS = 700;

bool getTouchPoint(int& x, int& y) {
  if (!ts.touched()) return false;

  TS_Point p = ts.getPoint();

#if DISPLAY_BACKEND_IS_GFX_COMPAT
  x = constrain((int)p.x, 0, SCREEN_W - 1);
  y = constrain((int)p.y, 0, SCREEN_H - 1);
  if (cfg.flipScreen) {
    x = (SCREEN_W - 1) - x;
    y = (SCREEN_H - 1) - y;
  }
  return true;
#else
  int rawX = p.x;
  int rawY = p.y;

  if (TOUCH_SWAP_XY) {
    x = map(rawY, touchMinY, touchMaxY, 0, SCREEN_W - 1);
    y = map(rawX, touchMinX, touchMaxX, 0, SCREEN_H - 1);
  } else {
    x = map(rawX, touchMinX, touchMaxX, 0, SCREEN_W - 1);
    y = map(rawY, touchMinY, touchMaxY, 0, SCREEN_H - 1);
  }

  if (TOUCH_FLIP_X) x = (SCREEN_W - 1) - x;
  if (TOUCH_FLIP_Y) y = (SCREEN_H - 1) - y;
  if (cfg.flipScreen) {
    x = (SCREEN_W - 1) - x;
    y = (SCREEN_H - 1) - y;
  }

  x = constrain(x, 0, SCREEN_W - 1);
  y = constrain(y, 0, SCREEN_H - 1);

  return true;
#endif
}

bool watchSourceAvailable(int source) {
  if (source == WATCH_MMDVM) return true;
  if (source == WATCH_SVXLINK) return svxlinkFeatureEnabled();
  if (source == WATCH_APRS) return aprsFeatureEnabled();
  if (source == WATCH_WEATHER) return weatherFeatureEnabled();
  if (source == WATCH_HAMQSL) return hamqslFeatureEnabled();
  return false;
}

bool handleWatchSourceSwipe(int dx) {
  if (screensaverActive || setupRequired) return false;
  if (detailPopupOpen || modeConfirmPopupOpen || quickActionPopupOpen || watchSourcePopupOpen || aprsCommentPopupOpen) return false;
  if (currentPage != PAGE_LIVE && currentPage != PAGE_LAST) return false;

  const int sources[5] = { WATCH_MMDVM, WATCH_SVXLINK, WATCH_APRS, WATCH_WEATHER, WATCH_HAMQSL };
  int currentIndex = 0;
  for (int i = 0; i < 5; ++i) {
    if (sources[i] == currentWatchSource) {
      currentIndex = i;
      break;
    }
  }

  const int step = (dx < 0) ? 1 : -1;
  for (int offset = 1; offset <= 5; ++offset) {
    int idx = (currentIndex + step * offset + 10) % 5;
    int candidate = sources[idx];
    if (!watchSourceAvailable(candidate)) continue;
    if (candidate == currentWatchSource) return false;
    setCurrentWatchSource((WatchSource)candidate);
    currentPage = PAGE_LIVE;
    if (candidate == WATCH_WEATHER && weatherConfigured()) fetchWeatherData();
    if (candidate == WATCH_HAMQSL && hamqslConfigured()) fetchHamqslData();
    if (candidate == WATCH_MMDVM) setFooterText("WATCH: MMDVM", true);
    else if (candidate == WATCH_SVXLINK) setFooterText("WATCH: SVXLink", true);
    else if (candidate == WATCH_APRS) setFooterText("WATCH: APRS", true);
    else if (candidate == WATCH_WEATHER) setFooterText("WATCH: Weather", true);
    else if (candidate == WATCH_HAMQSL) setFooterText("WATCH: HAMQSL", true);
    requestRedraw();
    return true;
  }
  return false;
}

void handleTouch() {
  static unsigned long lastTouch = 0;
  static unsigned long touchHeldSince = 0;
  static bool touchHeld = false;
  static int touchStartX = 0;
  static int touchStartY = 0;
  static bool swipeConsumed = false;

  int x, y;
  bool touched = getTouchPoint(x, y);
  unsigned long now = millis();

  if (!touched) {
    touchHeld = false;
    touchHeldSince = 0;
    swipeConsumed = false;
    return;
  }

  if (!touchHeld) {
    touchHeldSince = now;
    touchStartX = x;
    touchStartY = y;
    swipeConsumed = false;
  } else if (!swipeConsumed) {
    int dx = x - touchStartX;
    int dy = y - touchStartY;
    int threshold = max(UW(54), SCREEN_W / 5);
    if (abs(dx) >= threshold && abs(dx) > abs(dy) * 2 && touchStartY > UY(34)) {
      swipeConsumed = handleWatchSourceSwipe(dx);
      if (swipeConsumed) {
        lastTouch = now;
        lastInteractionMillis = now;
        return;
      }
    }
  }

  if (screensaverActive) {
    touchHeld = true;
    lastTouch = now;
    lastInteractionMillis = now;
    setScreensaverActive(false);
    tft.fillScreen(theme.bg);
    requestRedraw();
    return;
  }

  if (touchHeld && touchHeldSince && (now - touchHeldSince > 1500UL)) {
    touchHeld = false;
    touchHeldSince = now;
  }

  if (touchHeld || swipeConsumed) return;
  if (now - lastTouch < TOUCH_DEBOUNCE_MS) return;

  touchHeld = true;
  touchHeldSince = now;
  lastTouch = now;
  lastInteractionMillis = now;
  noteUserInteraction(true);

  if (aprsCommentPopupOpen) {
    aprsCommentPopupOpen = false;
    requestRedraw();
    return;
  }

  if (watchSourcePopupOpen) {
    const bool svxEnabled = svxlinkFeatureEnabled();
    const bool aprsEnabled = aprsFeatureEnabled();
    const bool weatherEnabled = weatherFeatureEnabled();
    const bool hamqslEnabled = hamqslFeatureEnabled();
    const int optionCount = 1 + (svxEnabled ? 1 : 0) + (aprsEnabled ? 1 : 0) + (weatherEnabled ? 1 : 0) + (hamqslEnabled ? 1 : 0);
    const int cols = (optionCount >= 4) ? 2 : optionCount;
    const int rows = (optionCount + cols - 1) / cols;
    const int popupW = (optionCount >= 4) ? UW(308) : ((optionCount >= 3) ? UW(304) : UW(248));
    const int popupH = (optionCount >= 4) ? (rows >= 3 ? UH(180) : UH(148)) : UH(112);
    const int popupX = (SCREEN_W - popupW) / 2;
    const int popupY = (SCREEN_H - popupH) / 2;
    const int btnW = (optionCount >= 4) ? UW(118) : ((optionCount >= 3) ? UW(82) : UW(92));
    const int btnH = UH(30);
    const int gapX = UW(10);
    const int gapY = UH(10);
    const int totalBtnW = cols * btnW + (cols - 1) * gapX;
    const int startX = popupX + (popupW - totalBtnW) / 2;
    const int row1Y = popupY + UH(52);

    auto optX = [&](int i) -> int { return startX + (i % cols) * (btnW + gapX); };
    auto optY = [&](int i) -> int { return row1Y + (i / cols) * (btnH + gapY); };
    auto hitOption = [&](int i) -> bool { return i >= 0 && pointInRect(x, y, optX(i), optY(i), btnW, btnH); };

    int optIndex = 0;
    const int mmdvmI = optIndex++;
    const int svxI = svxEnabled ? optIndex++ : -1;
    const int aprsI = aprsEnabled ? optIndex++ : -1;
    const int weatherI = weatherEnabled ? optIndex++ : -1;
    const int hamqslI = hamqslEnabled ? optIndex++ : -1;

    if (hitOption(mmdvmI)) {
      watchSourcePopupOpen = false;
      setCurrentWatchSource(WATCH_MMDVM);
      currentPage = watchSourcePopupTargetPage;
      requestRedraw();
      return;
    }
    if (hitOption(svxI)) {
      watchSourcePopupOpen = false;
      setCurrentWatchSource(WATCH_SVXLINK);
      currentPage = watchSourcePopupTargetPage;
      requestRedraw();
      return;
    }
    if (hitOption(aprsI)) {
      watchSourcePopupOpen = false;
      setCurrentWatchSource(WATCH_APRS);
      currentPage = PAGE_LIVE;
      requestRedraw();
      return;
    }
    if (hitOption(weatherI)) {
      watchSourcePopupOpen = false;
      setCurrentWatchSource(WATCH_WEATHER);
      currentPage = PAGE_LIVE;
      if (weatherConfigured()) fetchWeatherData();
      requestRedraw();
      return;
    }
    if (hitOption(hamqslI)) {
      watchSourcePopupOpen = false;
      setCurrentWatchSource(WATCH_HAMQSL);
      currentPage = PAGE_LIVE;
      if (hamqslConfigured()) fetchHamqslData();
      requestRedraw();
      return;
    }
    if (!pointInRect(x, y, popupX, popupY, popupW, popupH)) {
      watchSourcePopupOpen = false;
      requestRedraw();
      return;
    }
    return;
  }

  if (modeConfirmPopupOpen) {
    int popupX, popupY, popupW, popupH;
    int noX, noY, noW, noH;
    int yesX, yesY, yesW, yesH;
    getModeConfirmPopupLayout(popupX, popupY, popupW, popupH, noX, noY, noW, noH, yesX, yesY, yesW, yesH);

    if (pointInRect(x, y, yesX, yesY, yesW, yesH)) {
      String label = modeConfirmTargetLabel;
      bool enable = modeConfirmEnableRequested;
      modeConfirmPopupOpen = false;
      requestRedraw();
      startModeToggleTask(label, enable);
    } else {
      modeConfirmPopupOpen = false;
      requestRedraw();
    }
    return;
  }

  if (quickActionPopupOpen) {
    const int popupW = UW(250);
    const int popupH = UH(128);
    const int popupX = (SCREEN_W - popupW) / 2;
    const int popupY = (SCREEN_H - popupH) / 2;
    const int btnW = UW(106);
    const int btnH = UH(24);
    const int gapX = UW(10);
    const int gapY = UH(10);
    const int startX = popupX + UW(14);
    const int startY = popupY + UH(34);
    const int closeX = popupX + UW(78);
    const int closeY = popupY + popupH - UH(30);
    const int closeW = UW(94);
    const int closeH = UH(20);

    if (pointInRect(x, y, closeX, closeY, closeW, closeH)) {
      quickActionPopupOpen = false;
      requestRedraw();
      return;
    }

    uint8_t limit = quickActionSlotLimit(activeQuickActionIsSvxlink);
    for (uint8_t i = 0; i < limit; i++) {
      int col = i % 2;
      int row = i / 2;
      int bx = startX + col * (btnW + gapX);
      int by = startY + row * (btnH + gapY);
      if (pointInRect(x, y, bx, by, btnW, btnH)) {
        if (!isQuickActionSlotConfigured(i, activeQuickActionIsSvxlink)) {
          setFooterText("Preset slot is empty.");
          return;
        }
        bool svx = activeQuickActionIsSvxlink;
        quickActionPopupOpen = false;
        requestRedraw();
        startQuickActionTask(i, svx);
        return;
      }
    }

    quickActionPopupOpen = false;

    requestRedraw();
    return;
  }

  if (detailPopupOpen) {
    detailPopupOpen = false;
    selectedHeardIndex = -1;
    requestRedraw();
    return;
  }


  if (y <= UY(32)) {
    const int tabGap = UW(4);
    const int tabW = UW(54);
    const int startX = UX(6);
    const bool showApTab = setupRequired && apModeActive;
    const bool showStatTab = !showApTab;

    const int clockW = UW(68);
    const int clockH = UH(24);
    const int clockX = UX(246);
    const int clockY = UY(6);

    if (x >= clockX && x < clockX + clockW && y >= clockY && y < clockY + clockH) {
      previousPage = currentPage;
      currentPage = PAGE_ABOUT;
      requestRedraw();
      return;
    }

    int tabIndex = -1;
    for (int i = 0; i < 4; i++) {
      int tabX = startX + i * (tabW + tabGap);
      if (x >= tabX && x < tabX + tabW) {
        tabIndex = i;
        break;
      }
    }

    if (tabIndex == 0) {
      if (svxlinkFeatureEnabled() || aprsFeatureEnabled() || weatherFeatureEnabled() || hamqslFeatureEnabled()) {
        watchSourcePopupTargetPage = PAGE_LIVE;
        watchSourcePopupOpen = true;
        requestRedraw();
        return;
      }
      currentPage = PAGE_LIVE;
    }
    else if (tabIndex == 1) {
      currentPage = PAGE_LAST;
    }
    else if (tabIndex == 2) currentPage = PAGE_STATUS;
    else if (tabIndex == 3 && showApTab) currentPage = PAGE_AP;
    else if (tabIndex == 3 && showStatTab) currentPage = PAGE_INFO;
    else return;

    requestRedraw();
    return;
  }

  if (!setupRequired && currentPage == PAGE_LIVE) {
    if (isCurrentWatchWeather()) {
      if (pointInRect(x, y, UX(222), UY(44), UW(78), UH(22))) {
        weatherState.lastAttemptMillis = 0;
        bool refreshed = fetchWeatherData();
        if (refreshed) setFooterText("Weather refresh requested", true);
        else if (weatherState.lastHttpCode) setFooterText("Weather HTTP: " + String(weatherState.lastHttpCode), false);
        requestRedraw();
        return;
      }
    } else if (isCurrentWatchAprs()) {
    } else if (isCurrentWatchHamqsl()) {
      if (pointInRect(x, y, UX(222), UY(44), UW(78), UH(22))) {
        hamqslState.lastAttemptMillis = 0;
        bool refreshed = fetchHamqslData();
        if (refreshed) setFooterText("HAMQSL refresh requested", true);
        else if (hamqslState.error.length()) setFooterText(hamqslState.error, false);
        else if (hamqslState.lastHttpCode) setFooterText("HAMQSL HTTP: " + String(hamqslState.lastHttpCode), false);
        requestRedraw();
        return;
      }
    } else if (isCurrentWatchAprs()) {
      if (pointInRect(x, y, UX(222), UY(44), UW(78), UH(22))) {
        aprsState.lastSuccessMillis = 0;
        lastPoll = 0;
        bool refreshed = fetchAprsData(true);
        if (refreshed) setFooterText("APRS refresh requested", true);
        else if (aprsState.error.length()) setFooterText(aprsState.error, false);
        requestRedraw();
        return;
      }
    } else if (isCurrentWatchSvxlink()) {
      if (svxlinkAprsAlertVisible() && pointInRect(x, y, UX(172), UY(44), UW(48), UH(22))) {
        openSvxlinkAprsCommentPopup();
        return;
      }
      if (pointInRect(x, y, UX(226), UY(44), UW(76), UH(22))) {
        if (!hasConfiguredQuickActions(true)) {
          setFooterText("No SVXLink actions are configured yet.");
          return;
        }
        activeQuickActionIsSvxlink = true;
        quickActionPopupOpen = true;
        requestRedraw();
        return;
      }
    } else {
      String modeLabel;
      if (liveModeButtonTouchHit(x, y, modeLabel)) {
        openModeConfirmPopup(modeLabel);
        return;
      }

      if (pointInRect(x, y, UX(112), UY(190), UW(96), UH(24))) {
        if (!hasConfiguredQuickActions(false)) {
          setFooterText("No quick actions are configured yet.");
          return;
        }
        activeQuickActionIsSvxlink = false;
        quickActionPopupOpen = true;
        requestRedraw();
        return;
      }
    }
  }

  if (currentPage == PAGE_LAST)
 {
    if (isSvxlinkBackend()) {
      normalizeSvxlinkHeardBrowseState();

      int pageCount = heardCount > 0 ? ((heardCount + LAST_HEARD_ROWS_PER_PAGE - 1) / LAST_HEARD_ROWS_PER_PAGE) : 0;
      if (pointInRect(x, y, UX(14), UY(76), UW(24), UH(20))) {
        if (lastHeardPageIndex > 0) {
          lastHeardPageIndex--;
          requestRedraw();
        }
        return;
      }
      if (pointInRect(x, y, UX(282), UY(76), UW(24), UH(20))) {
        if ((lastHeardPageIndex + 1) < pageCount) {
          lastHeardPageIndex++;
          requestRedraw();
        }
        return;
      }

      int pageStart = lastHeardPageIndex * LAST_HEARD_ROWS_PER_PAGE;
      int rows = max(0, min((int)LAST_HEARD_ROWS_PER_PAGE, heardCount - pageStart));
      for (int row = 0; row < rows; row++) {
        int rowY = UY(104) + (row * UH(28));
        if (x >= UX(14) && x <= SCREEN_W - UX(14) && y >= rowY && y <= rowY + UH(24)) {
          selectedHeardIndex = pageStart + row;
          detailPopupOpen = true;
          requestRedraw();
          return;
        }
      }
      return;
    }

    normalizeLastHeardBrowseState();

    if (pointInRect(x, y, UX(14), UY(76), UW(24), UH(20))) {
      if (lastHeardPageIndex > 0) {
        lastHeardPageIndex--;
        requestRedraw();
      }
      return;
    }
    if (pointInRect(x, y, UX(44), UY(76), UW(68), UH(20))) {
      lastHeardFilter = (LastHeardFilter)(((int)lastHeardFilter + 1) % 7);
      lastHeardPageIndex = 0;
      requestRedraw();
      return;
    }
    if (pointInRect(x, y, UX(118), UY(76), UW(96), UH(20))) {
      lastHeardSort = (LastHeardSort)(((int)lastHeardSort + 1) % 3);
      lastHeardPageIndex = 0;
      requestRedraw();
      return;
    }

    int visibleIndices[MAX_HEARD];
    int visibleCount = buildLastHeardVisibleIndices(visibleIndices, MAX_HEARD);
    int pageCount = visibleCount > 0 ? ((visibleCount + LAST_HEARD_ROWS_PER_PAGE - 1) / LAST_HEARD_ROWS_PER_PAGE) : 0;
    if (pointInRect(x, y, UX(282), UY(76), UW(24), UH(20))) {
      if ((lastHeardPageIndex + 1) < pageCount) {
        lastHeardPageIndex++;
        requestRedraw();
      }
      return;
    }

    int pageStart = lastHeardPageIndex * LAST_HEARD_ROWS_PER_PAGE;
    int rows = max(0, min((int)LAST_HEARD_ROWS_PER_PAGE, visibleCount - pageStart));
    for (int row = 0; row < rows; row++) {
      int rowY = UY(104) + (row * UH(28));
      if (x >= UX(14) && x <= SCREEN_W - UX(14) && y >= rowY && y <= rowY + UH(24)) {
        selectedHeardIndex = visibleIndices[pageStart + row];
        detailPopupOpen = true;
        requestRedraw();
        return;
      }
    }
  }

  if (currentPage == PAGE_STATUS) {
    if (x >= BRIGHT_SLIDER_X && x <= BRIGHT_SLIDER_X + BRIGHT_SLIDER_W &&
        y >= BRIGHT_SLIDER_Y - UH(8) && y <= BRIGHT_SLIDER_Y + BRIGHT_SLIDER_H + UH(8)) {
      setBrightnessFromTouchX(x);
      applyBrightness();
      saveConfigToFile();
      requestRedraw();
      return;
    }
  }
}

void initBacklight() {
#if DISPLAY_BACKEND_IS_DESKTOP

#elif DISPLAY_BACKEND_IS_WS_RGB_800_480
  ws8048Backlight(255);
#else
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, TFT_BACKLIGHT_ON);
#endif
}

static void deselectSpiDevices() {
#if DISPLAY_BACKEND == DISPLAY_BACKEND_2432S028R
  pinMode(TFT_CS, OUTPUT);
  pinMode(TOUCH_CS, OUTPUT);
  pinMode(SD_CS, OUTPUT);
  digitalWrite(TFT_CS, HIGH);
  digitalWrite(TOUCH_CS, HIGH);
  digitalWrite(SD_CS, HIGH);
#elif DISPLAY_BACKEND_IS_WS_TOUCH_LCD_2_8
  pinMode(TFT_CS, OUTPUT);
  digitalWrite(TFT_CS, HIGH);
  #if !WS28_USE_SD_MMC
    pinMode(SD_CS, OUTPUT);
    digitalWrite(SD_CS, HIGH);
  #endif
#elif DISPLAY_BACKEND_IS_WS_RGB_800_480 && !WS8048_USE_SD_MMC
  #if WS8048_SD_SPI_CS >= 0
    pinMode(WS8048_SD_SPI_CS, OUTPUT);
    digitalWrite(WS8048_SD_SPI_CS, HIGH);
  #endif
#endif
}

void setup() {
#if DISPLAY_BACKEND_IS_DESKTOP
  desktopTrace("[DESKTOP] setup: entered.\n");
#endif
  Serial.begin(115200);
#if DISPLAY_BACKEND_IS_DESKTOP
  desktopTrace("[DESKTOP] setup: Serial.begin done.\n");
#else
  delay(300);
#endif
  Serial.println();
  Serial.println("Boot start");
  Serial.printf("Reset reason CPU0: %d\n", (int)rtc_get_reset_reason(0));
#if DISPLAY_BACKEND_IS_DESKTOP
  desktopTrace("[DESKTOP] setup: first Serial output done.\n");
#endif
  addWebLog("INFO", "BOOT", "Boot start.");
  addWebLog("INFO", "BOOT", "Reset reason CPU0: " + String((int)rtc_get_reset_reason(0)) + ".");
#if DISPLAY_BACKEND_IS_DESKTOP
  desktopTrace("[DESKTOP] setup: first web-log output done.\n");
#endif

  if (!networkMutex) networkMutex = xSemaphoreCreateRecursiveMutex();
  if (!networkMutex) addWebLog("WARN", "BOOT", "Network mutex allocation failed; network calls are not serialized.");

#if ENABLE_PISTAR_TOUCH_SSH
  libssh_begin();
#endif

  initBacklight();
  deselectSpiDevices();

#if DISPLAY_BACKEND == DISPLAY_BACKEND_2432S028R || DISPLAY_BACKEND == DISPLAY_BACKEND_JC3248W535C
  sdSPI.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);
#elif DISPLAY_BACKEND_IS_WS_TOUCH_LCD_2_8 && !WS28_USE_SD_MMC
  sdSPI.begin(SD_SCLK, SD_MISO, SD_MOSI, SD_CS);
#elif DISPLAY_BACKEND_IS_WS_RGB_800_480 && !WS8048_USE_SD_MMC
  sdSPI.setHwCs(false);
  sdSPI.begin(SD_SCLK, SD_MISO, SD_MOSI, WS8048_SD_SPI_CS);
#endif

  setDefaultTheme();

#if DISPLAY_BACKEND_IS_DESKTOP
  desktopTrace("[DESKTOP] setup: calling tft.init()/SDL_Init.\n");
#endif
  tft.init();
#if DISPLAY_BACKEND_IS_DESKTOP
  desktopTrace("[DESKTOP] setup: tft.init()/SDL_Init complete.\n");
#endif
  tft.setRotation(DISPLAY_ROTATION_NORMAL);

#if DISPLAY_BACKEND == DISPLAY_BACKEND_2432S028R
  Serial.printf("CYD TFT controller: %s\n", DISPLAY_CONTROLLER_NAME);
  Serial.printf("CYD touch transform: swap_xy=%d flip_x=%d flip_y=%d raw_x=%d..%d raw_y=%d..%d\n",
                TOUCH_SWAP_XY, TOUCH_FLIP_X, TOUCH_FLIP_Y,
                touchMinX, touchMaxX, touchMinY, touchMaxY);
  Serial.printf("TFT_eSPI runtime size after rotation: %d x %d, expected: %d x %d\n",
                tft.width(), tft.height(), SCREEN_W, SCREEN_H);
  if (tft.width() != SCREEN_W || tft.height() != SCREEN_H) {
    Serial.println("WARNING: TFT_eSPI User_Setup.h controller/geometry does not match this sketch. Select ILI9341 or ST7789 in the companion User_Setup.h and keep TFT_WIDTH/TFT_HEIGHT at 240x320 for this CYD layout.");
  }
#endif

#if DISPLAY_BACKEND_IS_WS_RGB_800_480 || DISPLAY_BACKEND_IS_WS_TOUCH_LCD_2_8
  Serial.printf("%s controller: %s, runtime size: %d x %d\n", USER_SETUP_INFO, DISPLAY_CONTROLLER_NAME, tft.width(), tft.height());
#endif

#if DISPLAY_BACKEND_IS_DESKTOP
  Serial.printf("Desktop controller: %s, runtime size: %d x %d\n", DISPLAY_CONTROLLER_NAME, tft.width(), tft.height());
#endif

#if DISPLAY_BACKEND == DISPLAY_BACKEND_JC3248W535C
  jc3248InitLvglBridge();
#elif DISPLAY_BACKEND_IS_WS_RGB_800_480
  ws8048InitLvglBridge();
#elif DISPLAY_BACKEND_IS_WS_TOUCH_LCD_2_8
  ws28InitLvglBridge();
#endif
  animateBootProgress("POWERING DISPLAY", 0, 14, 24);

#if DISPLAY_BACKEND == DISPLAY_BACKEND_2432S028R || DISPLAY_BACKEND_IS_WS_TOUCH_LCD_2_8
  deselectSpiDevices();
#endif
#if DISPLAY_BACKEND_IS_WS_RGB_800_480
  ws8048SdCardEnable(true);
#endif
#if DISPLAY_BACKEND_IS_DESKTOP
  bool sdMounted = SD.begin();
#elif DISPLAY_BACKEND_IS_WS_RGB_800_480 && WS8048_USE_SD_MMC
  bool sdMounted = SD_MMC.setPins(SD_SCLK, SD_MOSI, SD_MISO) && SD_MMC.begin("/sdcard", true, false);
#elif DISPLAY_BACKEND_IS_WS_TOUCH_LCD_2_8 && WS28_USE_SD_MMC
  delay(50);
  Serial.printf("[INFO][SD] SDIO pins CLK=%d CMD=%d D0=%d D1=%d D2=%d D3=%d.\n",
                WS28_SD_CLK, WS28_SD_CMD, WS28_SD_D0, WS28_SD_D1, WS28_SD_D2, WS28_SD_D3);
  bool sdMounted = false;
  #if defined(WS28_SD_D1) && defined(WS28_SD_D2) && defined(WS28_SD_D3)
    Serial.println("[INFO][SD] Trying SD_MMC 4-bit mode.");
    sdMounted = SD_MMC.setPins(WS28_SD_CLK, WS28_SD_CMD, WS28_SD_D0, WS28_SD_D1, WS28_SD_D2, WS28_SD_D3) &&
                SD_MMC.begin("/sdcard", false, false);
    if (!sdMounted) {
      Serial.println("[WARN][SD] SD_MMC 4-bit mount failed; retrying 1-bit mode on CLK/CMD/D0.");
      SD_MMC.end();
      delay(100);
      sdMounted = SD_MMC.setPins(WS28_SD_CLK, WS28_SD_CMD, WS28_SD_D0, -1, -1, -1) &&
                  SD_MMC.begin("/sdcard", true, false);
    }
  #else
    Serial.println("[INFO][SD] Trying SD_MMC 1-bit mode.");
    sdMounted = SD_MMC.setPins(WS28_SD_CLK, WS28_SD_CMD, WS28_SD_D0, -1, -1, -1) && SD_MMC.begin("/sdcard", true, false);
  #endif
#elif DISPLAY_BACKEND_IS_WS_RGB_800_480
  bool sdMounted = SD.begin(WS8048_SD_SPI_CS, sdSPI, 8000000);
#else
  bool sdMounted = SD.begin(SD_CS, sdSPI, 10000000);
#endif
  if (!sdMounted) {
    activeStorageLabel = "FLASH";
    animateBootProgress("SD mount failed / using FLASH", 14, 38, 18);
    addWebLog("WARN", "BOOT", "SD mount failed. Falling back to FLASH storage.");
  } else {
    activeStorageLabel = "SD";
    animateBootProgress("SD CARD READY", 14, 38, 18);
    addWebLog("INFO", "BOOT", "SD storage mounted.");
  }

  bool configOk = loadConfig();
  configOkState = configOk;
  setupRequired = !configOk;
  if (!configOk) {
    animateBootProgress("Config missing / invalid", 38, 62, 18);
    addWebLog("WARN", "BOOT", "Configuration missing or invalid.");
  } else {
    animateBootProgress("CONFIG LOADED", 38, 62, 18);
    addWebLog("INFO", "BOOT", "Configuration loaded.");
  }

  tft.fillRoundRect(UX(92), UY(142), UW(136), UH(28), UR(8), theme.panel);
  tft.drawRoundRect(UX(92), UY(142), UW(136), UH(28), UR(8), theme.accent);
  tft.setTextFont(2);
  tft.setTextColor(theme.accent, theme.panel);
  tft.setTextDatum(MC_DATUM);
  tft.drawString(getDeviceCallsign(), SCREEN_W / 2, UY(156));
  tft.setTextDatum(TL_DATUM);

  lastLiveActiveState = isLiveActive();
  lastInteractionMillis = millis();

#if DISPLAY_BACKEND == DISPLAY_BACKEND_2432S028R
  touchSPI.begin(TOUCH_CLK, TOUCH_MISO, TOUCH_MOSI, TOUCH_CS);
  ts.begin(touchSPI);
  tft.setRotation(DISPLAY_ROTATION_NORMAL);
  animateBootProgress("TOUCH ONLINE", 62, 78, 18);
#elif DISPLAY_BACKEND == DISPLAY_BACKEND_JC3248W535C
  ts.begin(touchSPI);
  ts.setRotation(DISPLAY_ROTATION_NORMAL);
  animateBootProgress("TOUCH ONLINE", 62, 78, 18);
#elif DISPLAY_BACKEND_IS_WS_RGB_800_480
  ts.begin(touchSPI);
  ts.setRotation(DISPLAY_ROTATION_NORMAL);
  animateBootProgress("TOUCH ONLINE", 62, 78, 18);
#elif DISPLAY_BACKEND_IS_WS_TOUCH_LCD_2_8
  ts.begin(touchSPI);
  ts.setRotation(DISPLAY_ROTATION_NORMAL);
  #if WS28_ENABLE_TOUCH
    animateBootProgress("TOUCH ONLINE", 62, 78, 18);
  #else
    animateBootProgress("TOUCH DISABLED", 62, 78, 18);
  #endif
#elif DISPLAY_BACKEND_IS_DESKTOP
  ts.begin(touchSPI);
  ts.setRotation(DISPLAY_ROTATION_NORMAL);
  animateBootProgress("MOUSE TOUCH ONLINE", 62, 78, 18);
#endif

  #if DISPLAY_BACKEND_IS_WS_TOUCH_LCD_2_8
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, TFT_BACKLIGHT_ON);
  #endif

  animateBootProgress("LINKING WIFI", 78, 88, 18);
  if (configOk && configLooksValid()) {
    connectWiFi(WIFI_CONNECT_TIMEOUT_MS);
  } else {
    wifiOk = false;
  }

  if (!configOk || !wifiOk) {
    setupRequired = true;
    animateBootProgress("STARTING SETUP AP", 88, 94, 18);
    startSetupAccessPoint();
    currentPage = PAGE_AP;
    previousPage = PAGE_AP;
  } else {
    setupRequired = false;
  }
  setupWebServer();

  animateBootProgress(wifiOk ? "BOOT COMPLETE" : (apModeActive ? "SETUP AP READY" : "BOOT COMPLETE / OFFLINE"), 94, 100, 18);
  addWebLog("INFO", "BOOT", wifiOk ? ("Boot complete. Web UI at http://" + WiFi.localIP().toString() + "/") : (apModeActive ? ("Boot complete with setup AP at http://" + WiFi.softAPIP().toString() + "/") : String("Boot complete without network connection.")));

#if DISPLAY_BACKEND_IS_DESKTOP
  Serial.println("[DESKTOP] post-boot: footer");
  setFooterText(wifiOk ? ("Desktop: http://" + WiFi.localIP().toString() + "/") : (apModeActive ? ("Desktop setup: http://" + WiFi.softAPIP().toString() + "/") : "Desktop offline"));

  Serial.println("[DESKTOP] post-boot: display settings");
  if (!setupRequired) applyStartPageAfterBoot();
  applyDisplaySettings();

  if (setupRequired || apModeActive) {
    Serial.println("[DESKTOP] post-boot: safe setup/AP page");
    drawDesktopBootReadyPage();
    lastRenderedPage = PAGE_AP;
  } else {
    Serial.println("[DESKTOP] post-boot: normal first page");
    tft.fillScreen(theme.bg);
    requestRedraw();
    redrawCurrentPage(true);
  }

  lastHeaderClock = getHeaderTimeText();
  uiDirty = false;
  footerDirty = false;
  headerClockDirty = false;
  scrollingTextDirty = false;
  Serial.println("[DESKTOP] post-boot: complete");
#else
  delay(350);

  setFooterText(wifiOk ? ("Web UI: http://" + WiFi.localIP().toString() + "/") : (apModeActive ? ("Setup required: " + apSsid + " / http://" + WiFi.softAPIP().toString() + "/") : "Wi-Fi failed, retrying..."));

  if (!setupRequired) applyStartPageAfterBoot();

  applyDisplaySettings();
  tft.fillScreen(theme.bg);

  requestRedraw();
  redrawCurrentPage(true);
  lastHeaderClock = getHeaderTimeText();
  uiDirty = false;
#endif
}

void loop() {
#if DISPLAY_BACKEND_IS_DESKTOP
  static unsigned long lastDesktopSafeDraw = 0;
  static unsigned long lastDesktopSafeLog = 0;
  desktopPollEventsOnly();
  if (setupRequired || apModeActive) {
    if (millis() - lastDesktopSafeDraw >= 250UL || uiDirty || footerDirty || headerClockDirty || scrollingTextDirty) {
      lastDesktopSafeDraw = millis();
      drawDesktopBootReadyPage();
      uiDirty = false;
      footerDirty = false;
      headerClockDirty = false;
      scrollingTextDirty = false;
    }
    if (millis() - lastDesktopSafeLog >= 5000UL) {
      lastDesktopSafeLog = millis();
      Serial.println("[DESKTOP] safe setup/AP loop active");
    }
    return;
  }
#endif

#if DISPLAY_BACKEND_IS_DESKTOP
  static bool desktopLoopModeLogged = false;
  const char* desktopNetworkEnv = std::getenv("MMDVM_DESKTOP_NETWORK");
  const bool desktopNetworkEnabled = !(desktopNetworkEnv && std::strcmp(desktopNetworkEnv, "0") == 0);
  if (!desktopLoopModeLogged) {
    desktopLoopModeLogged = true;
    if (desktopNetworkEnabled) Serial.println("[DESKTOP] firmware network polling enabled; set MMDVM_DESKTOP_NETWORK=0 for UI-only mode");
    else Serial.println("[DESKTOP] interactive UI loop active; network/WebServer/SSH polling is skipped");
  }
  if (!desktopNetworkEnabled) {
    static bool desktopUiHandleTouchTraced = false;
    if (!desktopUiHandleTouchTraced) desktopTrace("[DESKTOP] UI loop: before handleTouch.\n");
    handleTouch();
    if (!desktopUiHandleTouchTraced) {
      desktopTrace("[DESKTOP] UI loop: handleTouch returned.\n");
      desktopUiHandleTouchTraced = true;
    }

    if (cfg.screensaverEnabled && !screensaverActive && millis() - lastInteractionMillis >= cfg.screensaverTimeoutMs) {
      setScreensaverActive(true);
    }

    if (screensaverActive && millis() - lastScreensaverFrameMillis >= 60000) {
      lastScreensaverFrameMillis = millis();
      requestRedraw();
    }

    bool liveActiveNow = isLiveActive();
    if (liveActiveNow != lastLiveActiveState) {
      lastLiveActiveState = liveActiveNow;
      if (currentPage == PAGE_LIVE) requestRedraw();
    }

    if (!screensaverActive && millis() - lastClockCheck >= 5000) {
      lastClockCheck = millis();
      String nowClock = getHeaderTimeText();
      if (nowClock != lastHeaderClock) {
        lastHeaderClock = nowClock;
        headerClockDirty = true;
      }
    }

    if (!screensaverActive && scrollingTextActiveFrame &&
        !detailPopupOpen && !modeConfirmPopupOpen && !quickActionPopupOpen && !watchSourcePopupOpen && !aprsCommentPopupOpen &&
        (currentPage == PAGE_LIVE || currentPage == PAGE_LAST) &&
        millis() - lastScrollingTextFrameMs >= SCROLL_TEXT_STEP_MS) {
      lastScrollingTextFrameMs = millis();
      scrollingTextDirty = true;
    }

    if (!uiDirty && !screensaverActive && (headerClockDirty || footerDirty || scrollingTextDirty)) {
      bool partialDone = false;

      if (headerClockDirty) {
        drawHeaderClockBox();
        headerClockDirty = false;
        partialDone = true;
      }

      if (footerDirty) {
        drawFooter();
        footerDirty = false;
        partialDone = true;
      }

      if (scrollingTextDirty) {
        scrollingTextActiveFrame = false;
        if (redrawScrollingTextAreasForCurrentPage()) {
          scrollingTextDirty = false;
          partialDone = true;
        } else {
          requestRedraw();
        }
      }

      if (partialDone) tft.flush();
    }

    if (uiDirty || footerDirty || headerClockDirty || scrollingTextDirty) {
      redrawCurrentPage(uiDirty);
      footerDirty = false;
      headerClockDirty = false;
      scrollingTextDirty = false;
      uiDirty = false;
    }
    return;
  }
#endif

  handleTouch();
#if DISPLAY_BACKEND == DISPLAY_BACKEND_JC3248W535C
  jc3248HandleLvgl();
#elif DISPLAY_BACKEND_IS_WS_RGB_800_480
  ws8048HandleLvgl();
#elif DISPLAY_BACKEND_IS_WS_TOUCH_LCD_2_8
  ws28HandleLvgl();
#endif
  handleWebClientSafely();

  if (otaUpdateSuccessful && otaRestartAt && millis() >= otaRestartAt) {
    addWebLog("INFO", "OTA", "Rebooting into updated firmware.");
    delay(150);
#ifndef USE_DESKTOP
    ESP.restart();
#else
    gDesktopRunning = false;
#endif
  }

  if (pendingConfigApply && millis() >= pendingConfigApplyAt) {
    pendingConfigApply = false;

    if (configOkState && configLooksValid()) {
      setupRequired = false;
      beginWiFiConnectAttempt(true);
    } else {
      wifiConnectInProgress = false;
      wifiOk = false;
      setupRequired = true;
      currentPage = PAGE_AP;
      startSetupAccessPoint();
      setFooterText(apModeActive ? ("Setup required: " + apSsid + " / http://" + WiFi.softAPIP().toString() + "/") : "Config saved from web");
      requestRedraw();
    }
  }

  if (millis() - lastWifiStateCheck >= 250) {
    lastWifiStateCheck = millis();

    bool configValidNow = configLooksValid();
    bool stationConnected = (WiFi.status() == WL_CONNECTED);

    if (!configValidNow) {
      wifiConnectInProgress = false;
      wifiOk = false;
      setupRequired = true;
      currentPage = PAGE_AP;
      if (!apModeActive) startSetupAccessPoint();
    } else {
      if (wifiConnectInProgress) {
        handleWiFiConnectProgress();
        stationConnected = (WiFi.status() == WL_CONNECTED);
      }

      if (stationConnected) {
        wifiOk = true;
        setupRequired = false;
        syncTimeIfNeeded();
        if (apModeActive) stopSetupAccessPoint();
      } else {
        wifiOk = false;
        if (setupRequired) {
          if (!apModeActive) startSetupAccessPoint();
        } else if (!wifiConnectInProgress && millis() - lastWifiRetryMillis >= WIFI_RETRY_INTERVAL_MS) {
          beginWiFiConnectAttempt(false);
        }
      }
    }
  }

  bool pageNeedsNetworkPolling = (currentPage == PAGE_LIVE || currentPage == PAGE_LAST);
  bool touchGraceActive = (millis() - lastInteractionMillis) < TOUCH_POLL_GRACE_MS;
  bool canPollNetwork = !setupRequired && wifiOk && !apModeActive && !wifiConnectInProgress;
  unsigned long normalPollInterval = isCurrentWatchSvxlink() ? max(3000UL, (unsigned long)cfg.refreshMs) : (unsigned long)cfg.refreshMs;
  unsigned long screensaverPollInterval = isCurrentWatchSvxlink() ? max(10000UL, (unsigned long)cfg.refreshMs) : (unsigned long)cfg.refreshMs;
  bool normalUiPoll = !screensaverActive && canPollNetwork && pageNeedsNetworkPolling && !touchGraceActive && millis() - lastPoll >= normalPollInterval;
  bool screensaverWakePoll = screensaverActive && canPollNetwork && millis() - lastPoll >= screensaverPollInterval;

  if (normalUiPoll) {
    lastPoll = millis();
    fetchAllData();
  } else if (screensaverWakePoll) {
    lastPoll = millis();
    if (isCurrentWatchWeather()) fetchWeatherData();
    else if (isCurrentWatchHamqsl()) fetchHamqslData();
    else if (isCurrentWatchSvxlink()) fetchSvxlinkData();
    else fetchLastHeard();
  }

  processRadioIdLookups();
  processTargetNameLookups();

  if (millis() - lastWeatherPollCheck >= 1000UL) {
    lastWeatherPollCheck = millis();
    if (wifiOk && !setupRequired && !apModeActive && weatherConfigured()) {
      unsigned long weatherRefreshMs = max(60000UL, (unsigned long)cfg.weatherRefreshMinutes * 60000UL);
      if (!weatherState.lastAttemptMillis || millis() - weatherState.lastAttemptMillis >= weatherRefreshMs) {
        fetchWeatherData();
      }
    }
  }

  if (millis() - lastHamqslPollCheck >= 1000UL) {
    lastHamqslPollCheck = millis();
    if (wifiOk && !setupRequired && !apModeActive && hamqslConfigured()) {
      unsigned long hamqslRefreshMs = max((unsigned long)HAMQSL_REFRESH_MINUTES_MIN * 60000UL, (unsigned long)cfg.hamqslRefreshMinutes * 60000UL);
      if (!hamqslState.lastAttemptMillis || millis() - hamqslState.lastAttemptMillis >= hamqslRefreshMs) {
        fetchHamqslData();
      }
    }
  }

  if (cfg.screensaverEnabled && !screensaverActive && millis() - lastInteractionMillis >= cfg.screensaverTimeoutMs) {
    setScreensaverActive(true);
  }

  if (screensaverActive && millis() - lastScreensaverFrameMillis >= 60000) {
    lastScreensaverFrameMillis = millis();
	requestRedraw();
  }

  bool liveActiveNow = isLiveActive();
  if (liveActiveNow != lastLiveActiveState) {
    lastLiveActiveState = liveActiveNow;
    if (currentPage == PAGE_LIVE) requestRedraw();
  }

  processModeToggleTaskResult();
  processQuickActionTaskResult();

  if (!screensaverActive && millis() - lastClockCheck >= 5000) {
    lastClockCheck = millis();
    String nowClock = getHeaderTimeText();
    if (nowClock != lastHeaderClock) {
      lastHeaderClock = nowClock;
      headerClockDirty = true;
    }
  }

  if (!screensaverActive && scrollingTextActiveFrame &&
      !detailPopupOpen && !modeConfirmPopupOpen && !quickActionPopupOpen && !watchSourcePopupOpen && !aprsCommentPopupOpen &&
      (currentPage == PAGE_LIVE || currentPage == PAGE_LAST) &&
      millis() - lastScrollingTextFrameMs >= SCROLL_TEXT_STEP_MS) {
    lastScrollingTextFrameMs = millis();
    scrollingTextDirty = true;
  }

  if (!uiDirty && !screensaverActive && (headerClockDirty || footerDirty || scrollingTextDirty)) {
    bool partialDone = false;

    if (headerClockDirty) {
      drawHeaderClockBox();
      headerClockDirty = false;
      partialDone = true;
    }

    if (footerDirty) {
      drawFooter();
      footerDirty = false;
      partialDone = true;
    }

    if (scrollingTextDirty) {
      scrollingTextActiveFrame = false;
      if (redrawScrollingTextAreasForCurrentPage()) {
        scrollingTextDirty = false;
        partialDone = true;
      } else {
        requestRedraw();
      }
    }

#if DISPLAY_BACKEND_IS_GFX_COMPAT
    if (partialDone) tft.flush();
#endif
  }

  if (uiDirty || footerDirty || headerClockDirty || scrollingTextDirty) {
    redrawCurrentPage(uiDirty);
    footerDirty = false;
    headerClockDirty = false;
    scrollingTextDirty = false;
    uiDirty = false;
  }
}
