# January Watch Progress
#Watch

## Inspiration
To create a watch so simple that even my kids can use it immediately—a device that takes advantage of what I love about LLMs (natural language as an interface) but doesn’t scream “AI.” 

After three years working on the reinvention of Siri, I saw firsthand the gap between what’s technically possible and what an established product is willing to risk. This project is a way to explore that gap without the same constraints and design a product that I actualy love.

I’ve yet to find a smart watch that suits me—something always throws me off: the physical shape, the dense touch screen interactions, or the complicated UI. On the other hand, I love mechanical inputs: dials that turn and buttons that click. The interaction model for this watch is inspired by the original Nest thermostat and how it makes software feel mechanical. ~~I’m very interested in e-ink because it’s readable in sunlight, uses almost no power, and doesn’t demand attention the way glowing screens do.~~ Voice and AI are not a “feature,” but a way to keep the interface small while still letting the device do useful work.

I also want it to be fun. Cute. Inexpensive and not precious. Something well designed but at an accessible price point, like a Swatch.

## Progress
![](January%20Watch%20Progress/IMG_5498.HEIC)

## Features
**Core**
1. Independent internet connectivity (cellular modem)
2. Time, date, and glanceable system status
3. Weather and other low-attention ambient information
4. Calling and texting via a dedicated number
5. Mechanical rotary + press input as the primary navigation model
6. Push-to-talk voice interaction as the primary interface
7. Purposeful question answering and task assistance (server-side AI)

⠀**Stretch**
1. Calendar awareness: upcoming events, scheduling, reminders
2. Selective message triage and reply (email, work messaging)
3. Limited integrations with communication tools (e.g. Slack) under explicit user control

⠀**Super Stretch**
1. Location awareness and lightweight wayfinding
2. Optional health signals (e.g. heart rate), if they serve communication or safety goals

## Hardware Prototype
 [[Autodesk Fusion 360 Notes]]
### Concept
**Outer Ring** The outer ring is a continuously rotating, pressable input surface that translates rotational motion into scrolling and axial press into selection.

**Inner Ring** The inner ring constrains and guides the outer ring’s rotation, defining its axis, tolerances, and separation from the fixed watch body. Big cutouts for sensors until I figure out correct placement.

**Bearing-Spring Crown** The bearing-spring crown provides controlled resistance, centering force, and tactile feedback to the rotating ring through a combined spring and bearing mechanism.

### Fusion 360 Design
![](January%20Watch%20Progress/image.png)<!-- {"width":530} -->
![](January%20Watch%20Progress/image%204.png)<!-- {"width":205} -->![](January%20Watch%20Progress/image%203.png)<!-- {"width":219} -->![](January%20Watch%20Progress/image%202.png)<!-- {"width":215} -->



### Parts
[Chip](https://www.amazon.com/gp/product/B0DWWZH5Y4/ref=ox_sc_saved_title_1?smid=A1WZRPJ0MN58A9&th=1)<!-- {"preview":"true"} -->[Round LCD](https://www.amazon.com/dp/B0G25K22DG?ref=ppx_yo2ov_dt_b_fed_asin_title)<!-- {"preview":"true"} -->[Optical Sensor](https://www.digikey.com/en/products/detail/vishay-semiconductor-opto-division/TCND5000/1681395)<!-- {"preview":"true"} -->


### Next Steps
- [x] Send to [Uproar](https://uproar.art/) for print quote
- [ ] Schedule print and pickup
- [ ] Assemble and test mechanical parts
- [ ] Assemble and test sensors
- [ ] Order cellular modem

## Software Prototype
[[Programming MCU Notes]]

### Wifi-Connected Clock

```
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

```

### Scrolling Text

```
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>

static const int TFT_CS   = 10;
static const int TFT_DC   = 9;
static const int TFT_RST  = 8;
static const int TFT_SCK  = 12;
static const int TFT_MOSI = 11;

Adafruit_GC9A01A tft(TFT_CS, TFT_DC, TFT_RST);

// ----- Colors -----
uint16_t bg;

// ----- Wheel layout -----
static const int SCREEN_W = 240;
static const int SCREEN_H = 240;

static const int LINE_LEN = 12;   // fixed 12 characters per line
static const int VISIBLE  = 11;
static const uint8_t SIZES[VISIBLE] = { 1, 1, 2, 2, 3, 3, 3, 2, 2, 1, 1 };

// Default Adafruit_GFX classic font metrics:
static const int FONT_W = 6; // 5px glyph + 1px spacing
static const int FONT_H = 7; // 7px tall

// ----- Content -----
static const char* POEM =
  "’Twas brillig, and the slithy toves "
  "Did gyre and gimble in the wabe; "
  "All mimsy were the borogoves, "
  "And the mome raths outgrabe. "
  "Beware the Jabberwock, my son! "
  "The jaws that bite, the claws that catch! "
  "Beware the Jubjub bird, and shun "
  "The frumious Bandersnatch! ";

// Store wrapped lines here
static const int MAX_LINES = 400;
String lines[MAX_LINES];
int lineCount = 0;

// Center selection index (the “focused” line)
int indexCenter = 0;

// -------------------- Helpers --------------------
// Count visible characters by trimming trailing spaces (so centering is based on “ink”, not padding)
int visibleLenRightTrim(const String& s) {
  int n = s.length();
  while (n > 0 && s[n - 1] == ' ') n--;
  return n;
}

// -------------------- Wrapping --------------------
// Packs words into fixed 12-character lines.
// - Prefers breaking on spaces
// - Hard-splits words longer than 12
// - Pads every stored line to exactly 12 chars
void wrapToFixedLines(const char* text) {
  lineCount = 0;

  String cur = "";
  String word = "";

  auto flushLine = [&]() {
    if (lineCount >= MAX_LINES) return;
    while (cur.length() < LINE_LEN) cur += " ";
    if (cur.length() > LINE_LEN) cur = cur.substring(0, LINE_LEN);
    lines[lineCount++] = cur;
    cur = "";
  };

  auto pushWord = [&](const String& w) {
    if (w.length() == 0) return;

    // If the word is longer than the line length, split it.
    if ((int)w.length() > LINE_LEN) {
      if (cur.length() > 0) flushLine();

      int i = 0;
      while (i < (int)w.length() && lineCount < MAX_LINES) {
        cur = w.substring(i, min(i + LINE_LEN, (int)w.length()));
        flushLine();
        i += LINE_LEN;
      }
      return;
    }

    int needed = (cur.length() == 0) ? w.length() : (1 + w.length());
    if ((int)cur.length() + needed <= LINE_LEN) {
      if (cur.length() > 0) cur += " ";
      cur += w;
    } else {
      if (cur.length() > 0) flushLine();
      cur = w;
    }
  };

  for (const char* p = text;; p++) {
    char c = *p;
    bool end = (c == '\0');
    bool isSpace = (c == ' ' || c == '\n' || c == '\r' || c == '\t');

    if (!end && !isSpace) {
      word += c;
    } else {
      if (word.length() > 0) {
        pushWord(word);
        word = "";
      }

      if (c == '\n') {
        if (cur.length() > 0) flushLine();
      }

      if (end) break;
    }
  }

  if (cur.length() > 0) flushLine();
  if (lineCount == 0) {
    lines[lineCount++] = "            ";
  }
}

// -------------------- Drawing --------------------
void drawWheel() {
  // Full clear (simple + reliable for now)
  tft.fillRect(0, 0, SCREEN_W, SCREEN_H, bg);
  tft.setTextColor(GC9A01A_WHITE);

  const int centerRow = VISIBLE / 2; // 11 -> 5
  const int CENTER_Y  = 120;         // vertical center of screen
  const int PADDING   = 6;           // spacing between line boxes

  // Compute y positions so the wheel stays centered automatically
  int yPos[VISIBLE];
  yPos[centerRow] = CENTER_Y - (FONT_H * SIZES[centerRow]) / 2;

  // Above center
  for (int i = centerRow - 1; i >= 0; i--) {
    int thisH = FONT_H * SIZES[i];
    yPos[i] = yPos[i + 1] - thisH - PADDING;
  }

  // Below center
  for (int i = centerRow + 1; i < VISIBLE; i++) {
    int prevH = FONT_H * SIZES[i - 1];
    yPos[i] = yPos[i - 1] + prevH + PADDING;
  }

  // Draw each visible row
  for (int row = 0; row < VISIBLE; row++) {
    int size = SIZES[row];
    int idx  = indexCenter + (row - centerRow);

    // Clamp indices so we don't read out of bounds
    if (idx < 0) idx = 0;
    if (idx >= lineCount) idx = lineCount - 1;

    String s = lines[idx]; // padded to 12 chars

    // Center based on visible (non-trailing-space) characters
    int visLen = visibleLenRightTrim(s);
    int w = visLen * FONT_W * size;
    int x = (SCREEN_W - w) / 2;

    tft.setTextSize(size);
    tft.setCursor(x, yPos[row]);
    tft.print(s); // printing padded string is fine; trailing spaces don't draw ink
  }
}

// -------------------- Setup / Loop --------------------
void setup() {
  Serial.begin(115200);
  delay(200);

  SPI.begin(TFT_SCK, -1, TFT_MOSI, TFT_CS);
  tft.begin();

  bg = tft.color565(0xBE, 0x32, 0x2A); // #BE322A
  tft.fillScreen(bg);

  wrapToFixedLines(POEM);
  indexCenter = 0;

  drawWheel();
}

void loop() {
  static unsigned long last = 0;
  const unsigned long intervalMs = 1000;

  if (millis() - last >= intervalMs) {
    last = millis();

    // Auto-scroll. Later replace this with encoder delta.
    indexCenter++;
    if (indexCenter >= lineCount) indexCenter = 0;

    drawWheel();
  }
}
```

### Hello world
```
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
```

### Next Steps
- [x] Hello world interface
- [x] Simple wifi-connected clock interface
- [ ] Simple wifi-connected clock interface with Twilio messaging
- [ ] Modem-connected clock interface with Twilio messaging

## 