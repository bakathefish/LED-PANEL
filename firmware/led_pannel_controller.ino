/*
  LED-PANNEL — example external controller

  the panel has no mcu on it, so this is a starting-point sketch for
  an external ESP32 driving it over FastLED. untested on real hardware
  since the board isn't built yet.

  confirmed from the schematic/gerbers: 234 leds in a 13 wide x 18 tall
  grid at 20mm pitch, single-wire serial chain (LED1 DOUT -> LED2 DIN
  -> ...), serpentine row routing with LED1 in the top left corner and
  LED234 in the bottom left, 470 ohm resistor (R1) between pad H1 (data
  in) and LED1's DIN. no connector on the board, so data solders to H1
  and +5V solders to any of the 248 through-hole +5V pads.

  still worth checking once you actually have the board: whether the
  led really wants WS2812B timing and GRB order (the BOM part is a
  XINGLIGHT XL-5050RGBC-2812B, which should be WS2812B-compatible, but
  confirm it), whether a 3.3V esp32 gpio needs a level shifter to drive
  the line reliably, and real current draw (234 leds at full white is
  roughly 14A at the usual 60mA per led, don't power this off usb).

  library: FastLED (https://github.com/FastLED/FastLED)
*/

#include <FastLED.h>

// ---- Panel geometry (confirmed from schematic/gerbers) ----
#define MATRIX_WIDTH   13    // columns, X spans 10mm..250mm at 20mm pitch
#define MATRIX_HEIGHT  18    // rows,    Y spans 10mm..-330mm at 20mm pitch
#define NUM_LEDS       (MATRIX_WIDTH * MATRIX_HEIGHT)  // 234

// ---- Wiring (VERIFY before flashing) ----
#define DATA_PIN       5     // ESP32 GPIO feeding R1 -> LED1 DIN. Change to match your wiring.
#define LED_TYPE       WS2812B   // Per the BOM part number — confirm on real hardware
#define COLOR_ORDER    GRB       // Standard for WS2812B-class parts — verify, don't assume

// Matches the serpentine row routing confirmed in the gerber data:
// row 0 is LED1..LED13 left to right, row 1 is LED14..LED26 right to
// left, alternating down to LED234 in the bottom left.
const bool kMatrixSerpentine = true;
const bool kStartsAtBottom = false;   // LED1 is in the top row
const bool kStartsOnLeft   = true;    // LED1 is the leftmost LED in its row

CRGB leds[NUM_LEDS];

// Maps (x, y) matrix coordinates to the physical index in the LED chain.
uint16_t XY(uint8_t x, uint8_t y) {
  uint8_t row = kStartsAtBottom ? (MATRIX_HEIGHT - 1 - y) : y;

  bool rowReversed = kMatrixSerpentine && (row % 2 == 1);
  uint8_t col = (rowReversed == kStartsOnLeft) ? (MATRIX_WIDTH - 1 - x) : x;

  return (uint16_t)(row * MATRIX_WIDTH) + col;
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
