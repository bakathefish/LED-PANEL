/*
  LED-PANNEL — example external controller

  the panel has no mcu on it, so this is a starting-point sketch for
  an external ESP32 driving it over FastLED. untested on real hardware
  since the board isn't built yet.

  confirmed from the schematic/gerbers: 234 leds, 18x13, single-wire
  serial chain (LED1 DOUT -> LED2 DIN -> ...), serpentine row routing,
  470 ohm resistor (R1) between pad H1 (data in) and LED1's DIN, no
  connector on the board so power/data get soldered on directly.

  still worth checking once you actually have the board: exact led
  part/protocol (assuming WS2812B timing below), whether a 3.3V esp32
  gpio needs a level shifter to drive the line reliably, real current
  draw (234 leds at full white pulls a lot, don't power this off usb),
  and whether the serpentine direction assumed below actually matches
  the silkscreen once you can see it in person.

  library: FastLED (https://github.com/FastLED/FastLED)
*/

#include <FastLED.h>

// ---- Panel geometry (confirmed from schematic/gerbers) ----
#define MATRIX_WIDTH   18
#define MATRIX_HEIGHT  13
#define NUM_LEDS       (MATRIX_WIDTH * MATRIX_HEIGHT)  // 234

// ---- Wiring (VERIFY before flashing) ----
#define DATA_PIN       5     // ESP32 GPIO feeding R1 -> LED1 DIN. Change to match your wiring.
#define LED_TYPE       WS2812B   // UNVERIFIED — confirm actual LED part/protocol
#define COLOR_ORDER    GRB       // Common default for WS2812B-class parts — verify, don't assume

// Matches the serpentine row routing seen in the gerber data.
// If your first LED (LED1) is bottom-left instead of top-left, or the
// panel is mirrored, switch these constants accordingly.
const bool kMatrixSerpentine = true;
const bool kStartsAtBottom = false;   // LED1 was in the top row in the gerber data
const bool kStartsOnLeft   = true;    // LED1 was the leftmost LED in its row

CRGB leds[NUM_LEDS];

// Maps (x, y) matrix coordinates to the physical index in the LED chain.
uint16_t XY(uint8_t x, uint8_t y) {
  uint16_t i;
  uint8_t row = kStartsAtBottom ? (MATRIX_HEIGHT - 1 - y) : y;

  bool rowReversed = kMatrixSerpentine && (row % 2 == 1);
  uint8_t col = (rowReversed == kStartsOnLeft) ? (MATRIX_WIDTH - 1 - x) : x;

  i = (row * MATRIX_WIDTH) + col;
  return i;
}

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("LED-PANNEL example controller — UNTESTED, verify wiring before use");

  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);

  // Keep this low until you've measured real current draw with a meter.
  // Raise it only once you have a supply sized for the result.
  FastLED.setBrightness(40);
  FastLED.clear();
  FastLED.show();
}

void loop() {
  // Minimal sanity-check pattern: sweep a single pixel across the
  // matrix so you can visually confirm the XY mapping is correct
  // before trying anything more ambitious.
  static uint8_t x = 0;
  static uint8_t y = 0;

  FastLED.clear();
  leds[XY(x, y)] = CRGB::White;
  FastLED.show();

  x++;
  if (x >= MATRIX_WIDTH) {
    x = 0;
    y++;
    if (y >= MATRIX_HEIGHT) y = 0;
  }

  delay(40);
}
