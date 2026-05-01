#include <WiFi.h>
#include <time.h>

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>

static const int TFT_CS   = 10;
static const int TFT_DC   = 9;
static const int TFT_RST  = 8;
static const int TFT_SCK  = 12;
static const int TFT_MOSI = 11;

Adafruit_GC9A01A tft(TFT_CS, TFT_DC, TFT_RST);

// ---- put your Wi-Fi creds here ----
const char* WIFI_SSID = "Zoras Palace";
const char* WIFI_PASS = "Odincat131!";

// Alameda is Pacific Time. PST/PDT handling:
const char* TZ_INFO = "PST8PDT,M3.2.0,M11.1.0"; 
// (US daylight saving rules)
const char* MONTHS[] = {
  "JAN", "FEB", "MAR", "APR", "MAY", "JUN",
  "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
};

uint16_t bg = tft.color565(0xBE, 0x32, 0x2A);

void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.print("WiFi connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("WiFi connected, IP: ");
  Serial.println(WiFi.localIP());
}

bool syncTime() {
  // NTP servers (you can swap in time.google.com etc)
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  setenv("TZ", TZ_INFO, 1);
  tzset();

  // Wait until time is set
  struct tm timeinfo;
  Serial.print("Syncing time");
  for (int i = 0; i < 40; i++) { // ~10s max
    if (getLocalTime(&timeinfo)) {
      Serial.println("\nTime synced.");
      return true;
    }
    Serial.print(".");
    delay(250);
  }
  Serial.println("\nTime sync FAILED.");
  return false;
}

void drawClock(const struct tm& t) {
  char timeBuf[6];   // "H:MM" or "HH:MM"
  char dateBuf[8];

  // ---- TIME ----
  int hour12 = t.tm_hour % 12;
  if (hour12 == 0) hour12 = 12;

  snprintf(timeBuf, sizeof(timeBuf), "%d:%02d", hour12, t.tm_min);

  int timeX = (hour12 < 10) ? 51 : 33;

  tft.fillRect(33, 99, 174, 42, bg);
  tft.setTextColor(GC9A01A_WHITE);
  tft.setTextSize(6);
  tft.setCursor(timeX, 99);
  tft.print(timeBuf);

  // ---- DATE ----
  snprintf(dateBuf, sizeof(dateBuf), "%s %d", MONTHS[t.tm_mon], t.tm_mday);

  tft.fillRect(85, 68, 70, 14, bg);
  tft.setTextSize(2);
  tft.setCursor(85, 68);
  tft.print(dateBuf);
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("boot");

  SPI.begin(TFT_SCK, -1, TFT_MOSI, TFT_CS);
  tft.begin();
  tft.fillScreen(bg);

  connectWiFi();
  syncTime();

  // Optional: once time is synced, you can turn Wi-Fi off to save power
  // WiFi.disconnect(true);
  // WiFi.mode(WIFI_OFF);
}

void loop() {
  static unsigned long last = 0;
  if (millis() - last >= 60000) {
    last = millis();

    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
      drawClock(timeinfo);
    } else {
      // If time somehow isn't available, you could re-sync or show an error
      Serial.println("getLocalTime failed");
    }
  }
}
