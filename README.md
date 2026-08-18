# LED-PANNEL

an 18x13 addressable RGB LED panel, built to be driven pixel by pixel from an external controller

the project zine is in [zine.pdf](zine.pdf).

![PCB top view](images/pcb-top.png)

## What it does

LED-PANNEL is a big PCB carrying 234 individually addressable RGB LEDs wired up in an 18 column by 13 row grid. every LED sits on one continuous single-wire data chain, so one controller pin can drive all 234 pixels independently, full RGB, each one on its own. the board itself is just the panel, it has no brain on it, you bring the controller.

the name comes from the actual goal here: watching a movie the way it's meant to be seen, at real framerate, on a panel dense enough to actually output video and not just slow ambient patterns. 100+fps is the target for the finished system, not a measured result yet, since the board hasn't been built.

## Why i made it

i wanted a real reason to learn pcb design past a single blinky led board, so i picked something with an actual constraint to solve: pack as many individually addressable pixels as i could onto one board that's still manufacturable, route the data chain sanely across all of them, and get the power distribution right so it doesn't sag under load. an 18x13 matrix made routing and power actually hard, and gives me something genuinely useful afterward instead of just a demo.

## How it works

each led footprint has 4 pads: +5V, DIN, DOUT, GND. LED1's DOUT feeds LED2's DIN, LED2's DOUT feeds LED3's DIN, and so on, all 234 LEDs wired as one chain. the routing between rows snakes back and forth (serpentine) so the trace between the last led in a row and the first led in the next row stays short instead of running all the way back across the board every time.

there's a single 470Ω resistor (R1) between the data-in pad (H1) and LED1's DIN, which protects the first led in the chain from signal reflections, pretty standard for this kind of setup. every led also gets its own decoupling cap right next to it.

there's no connector on this board, no mcu either. H1 is a single solder pad for your controller's data wire (through R1, into LED1). power gets soldered straight onto the shared +5V/GND rails, there's no dedicated pad for it. H2 is the board's one mounting hole.

## What's on the board

- 234x addressable RGB LEDs, 20mm pitch, 18 columns x 13 rows
- 234x decoupling caps, one per led
- 1x 470Ω resistor (R1) on the data line
- 1x data-in pad (H1)
- 1x mounting hole (H2)
- 2-layer board, 260mm x 360mm

## Hardware

### Schematic
![Schematic](images/schematic.png)

### PCB top
![PCB top](images/pcb-top.png)

### PCB bottom
![PCB bottom](images/pcb-bottom.png)

### 3D view
![3D view](images/pcb-3d.png)

board files (gerbers) are in `PCB_FILES/LED_PANNEL_PCB.zip`.

## Firmware

there's no firmware on the panel itself since there's no mcu on it, `firmware/led_pannel_controller.ino` is a starting-point sketch for an external ESP32 controller using FastLED, mapped to the real serpentine chain order. it hasn't been tested on real hardware yet since the board isn't built, and a couple things are still worth double checking once it is: whether the led protocol is really WS2812B-style timing, and whether a 3.3V esp32 gpio needs a level shifter to drive the data line reliably.

### flashing it

open the sketch in arduino ide, install the **esp32 by Espressif** board package and the **FastLED** library, pick your esp32 board, plug it in over usb, select its com port and hit upload. check `DATA_PIN` in the sketch matches whichever gpio you actually wired to the panel's data line first.

## How to use it

1. wire +5V, GND, and DATA (through R1, into H1) from your controller/power supply to the panel.
2. flash `firmware/led_pannel_controller.ino` to your controller.
3. power the panel from a 5V supply sized for real measured draw, not your controller board's onboard regulator, 234 LEDs at full white pulls a lot more current than usb can give.
4. run the sketch's sweep pattern first to check every led lights up in the right spot before doing anything fancier.

## BOM

full bom is in `BOM.csv`. led is a XINGLIGHT XL-5050RGBC-2812B (LCSC C2843785), decoupling cap is a Samsung CL10B104KB8NNNC 100nF 0603 (LCSC C1591), at 234 qty each that's $11.37 for the leds and $3.51 for the caps, about $14.88 total for parts. that doesn't include the pcb itself, this is a 260 x 360mm board so get a real quote from your fab house since it's bigger than the usual cheap prototyping size.

## Assembly

1. order the pcb using the gerber zip in `PCB_FILES/LED_PANNEL_PCB.zip` (2-layer, 260 x 360mm, confirm your fab house can do this size).
2. solder the 234 leds and their decoupling caps, plus R1, using the silkscreen as a guide.
3. check the full data chain and the +5V/GND rails with a multimeter before powering up.
4. wire +5V, GND, and DATA to your external controller.
5. power up at low brightness first and check every led addresses correctly before running it at full brightness.

made by Rudra Puri, 17, India, for Hack Club.
