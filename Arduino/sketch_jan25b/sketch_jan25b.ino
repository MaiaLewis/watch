#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>

static const int TFT_CS   = 10;
static const int TFT_DC   = 9;
static const int TFT_RST  = 8;
static const int TFT_SCK  = 12;
static const int TFT_MOSI = 11;

Adafruit_GC9A01A tft(TFT_CS, TFT_DC, TFT_RST);

// ----- Screen -----
static const int SCREEN_W = 240;
static const int SCREEN_H = 240;

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

static const int LINE_LEN = 18;
static const int MAX_LINES = 400;
static const int VISIBLE = 7;   // fewer lines looks cleaner with big fonts

String lines[MAX_LINES];
int lineCount = 0;
int indexCenter = 0;

uint16_t bg;

// -------------------- Wrapping --------------------
// Max text width in pixels (leave margins on 240px screen)
static const int MAX_LINE_PX = 220;

// Measure pixel width of a String using a specific GFX font
int textWidthPx(const String& s, const GFXfont* font) {
  int16_t x1, y1;
  uint16_t w, h;
  tft.setFont(font);
  tft.getTextBounds(s.c_str(), 0, 0, &x1, &y1, &w, &h);
  return (int)w;
}

// Splits a single "word" that is too wide into chunks that fit MAX_LINE_PX
void pushWordSplitByPixels(const String& word, const GFXfont* font) {
  if (lineCount >= MAX_LINES) return;

  int start = 0;
  while (start < (int)word.length() && lineCount < MAX_LINES) {
    int end = start + 1;

    // Grow the chunk until adding one more char would exceed width
    while (end <= (int)word.length()) {
      String chunk = word.substring(start, end);
      if (textWidthPx(chunk, font) > MAX_LINE_PX) {
        break;
      }
      end++;
    }

    // If even a single character doesn't fit (shouldn't happen), force progress
    if (end == start + 1 && textWidthPx(word.substring(start, end), font) > MAX_LINE_PX) {
      lines[lineCount++] = word.substring(start, end);
      start = end;
      continue;
    }

    // Step back to last-fitting end
    int lastFitEnd = end - 1;
    if (lastFitEnd <= start) lastFitEnd = start + 1;

    lines[lineCount++] = word.substring(start, lastFitEnd);
    start = lastFitEnd;
  }
}

// Pixel-based wrapper: fits lines to MAX_LINE_PX using the largest font
void wrapToPixelLines(const char* text, const GFXfont* wrapFont) {
  lineCount = 0;

  String cur = "";
  String word = "";

  auto flushLine = [&]() {
    if (lineCount >= MAX_LINES) return;
    // Trim right
    while (cur.length() > 0 && cur[cur.length() - 1] == ' ') cur.remove(cur.length() - 1);
    if (cur.length() == 0) return;
    lines[lineCount++] = cur;
    cur = "";
  };

  auto pushWord = [&](const String& w) {
    if (w.length() == 0 || lineCount >= MAX_LINES) return;

    // If current line is empty, try to place word directly
    if (cur.length() == 0) {
      if (textWidthPx(w, wrapFont) <= MAX_LINE_PX) {
        cur = w;
      } else {
        // Word alone too wide: split it
        pushWordSplitByPixels(w, wrapFont);
      }
      return;
    }

    // Try adding with a preceding space
    String candidate = cur + " " + w;
    if (textWidthPx(candidate, wrapFont) <= MAX_LINE_PX) {
      cur = candidate;
    } else {
      // Current line is full; flush it, then place word on new line
      flushLine();
      if (lineCount >= MAX_LINES) return;

      if (textWidthPx(w, wrapFont) <= MAX_LINE_PX) {
        cur = w;
      } else {
        pushWordSplitByPixels(w, wrapFont);
      }
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

      // If there's an explicit newline, force a line break
      if (c == '\n') {
        flushLine();
      }

      if (end) break;
    }
  }

  flushLine();

  if (lineCount == 0) {
    lines[lineCount++] = "";
  }
}


// -------------------- Drawing --------------------
void drawWheel() {
  tft.fillRect(0, 0, SCREEN_W, SCREEN_H, bg);
  tft.setTextColor(GC9A01A_WHITE);
  tft.setTextSize(1);

  const int centerRow = VISIBLE / 2;
  const int CENTER_Y  = SCREEN_H / 2;
  const int PADDING   = 10;

  int yPos[VISIBLE];

  // Precompute heights
  int outerHeight;
  int centerHeight;

  // Measure representative text height
  int16_t x1, y1;
  uint16_t w, h;

  tft.setFont(&FreeSans9pt7b);
  tft.getTextBounds("Test", 0, 0, &x1, &y1, &w, &h);
  outerHeight = h;

  tft.setFont(&FreeSans12pt7b);
  tft.getTextBounds("Test", 0, 0, &x1, &y1, &w, &h);
  centerHeight = h;

  // Center row position
  yPos[centerRow] = CENTER_Y + centerHeight / 2;

  // Above center
  for (int i = centerRow - 1; i >= 0; i--) {
    yPos[i] = yPos[i + 1] - outerHeight - PADDING;
  }

  // Below center
  for (int i = centerRow + 1; i < VISIBLE; i++) {
    yPos[i] = yPos[i - 1] + outerHeight + PADDING;
  }

  // Draw rows
  for (int row = 0; row < VISIBLE; row++) {

    int idx = indexCenter + (row - centerRow);
    if (idx < 0) idx = 0;
    if (idx >= lineCount) idx = lineCount - 1;

    String s = lines[idx];

    // Choose font
    if (row == centerRow) {
      tft.setFont(&FreeSans12pt7b);
    } else {
      tft.setFont(&FreeSans9pt7b);
    }

    // Measure text
    tft.getTextBounds(s, 0, 0, &x1, &y1, &w, &h);

    int x = (SCREEN_W - w) / 2;

    tft.setCursor(x, yPos[row]);
    tft.print(s);
  }
}

// -------------------- Setup / Loop --------------------
void setup() {
  Serial.begin(115200);
  delay(200);

  SPI.begin(TFT_SCK, -1, TFT_MOSI, TFT_CS);
  tft.begin();

  bg = tft.color565(0xBE, 0x32, 0x2A);
  tft.fillScreen(bg);

  wrapToPixelLines(POEM, &FreeSans12pt7b);  // wrap using the largest font
  drawWheel();
}

void loop() {
  static unsigned long last = 0;
  const unsigned long intervalMs = 1500;

  if (millis() - last >= intervalMs) {
    last = millis();

    indexCenter++;
    if (indexCenter >= lineCount) indexCenter = 0;

    drawWheel();
  }
}
