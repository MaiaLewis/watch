#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>

static const int TFT_CS   = 10;
static const int TFT_DC   = 9;
static const int TFT_RST  = 8;
static const int TFT_SCK  = 12;
static const int TFT_MOSI = 11;

Adafruit_GC9A01A tft(TFT_CS, TFT_DC, TFT_RST);

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("boot");

  SPI.begin(TFT_SCK, -1, TFT_MOSI, TFT_CS);
  Serial.println("SPI.begin ok");

  Serial.println("about to tft.begin()");
  tft.begin();
  Serial.println("tft.begin ok");

  tft.fillScreen(GC9A01A_BLACK);
  tft.setTextColor(GC9A01A_WHITE);
  tft.setTextSize(3);
  tft.setCursor(60, 120);
  tft.print("HELLO");

  Serial.println("done");
}

void loop() {}
