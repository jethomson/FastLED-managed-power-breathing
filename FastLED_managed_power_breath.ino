/*
  This code is copyright 2018 Jonathan Thomson, jethomson.wordpress.com

  Permission to use, copy, modify, and distribute this software
  and its documentation for any purpose and without fee is hereby
  granted, provided that the above copyright notice appear in all
  copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/*
  Example code that shows how to get a breathing effect when using FastLED to manage power.

  If using a FastLED function (e.g. FastLED.setMaxPowerInVoltsAndMilliamps()) to manage how
  much power your LEDs use FastLED limits the maximum brightness that can be set to a value
  that does not exceed the requested power draw. So if FastLED determines your power limited
  maximum brightness is M then calling setBrightness() with a value greater than M will
  result in FastLED setting the brightness to exactly M.

  A breath effect ramps the brightness up until it reaches the maximum brightness then ramps
  the brightness back down to a minimum brightness then starts the whole cycle over again. A
  naive approach would be to use a triangle wave that goes from 0 to 255 and back down to 0
  as the input for setBrightness(), but if the power limited maximum brightness M is less
  than 255 then the brightness will plateau at M, and the brightness will remain fixed until
  the triangle wave input falls below M again. My breath() function achieves a continually
  changing brightness by setting the peak of the triangle wave to M. However, M changes
  based on the number of LEDs currently lit and their colors, so breath() recalculates M
  before setting a new brightness value.
  
  https://github.com/FastLED/FastLED/wiki/Power-notes
  http://fastled.io/docs/3.1/group___power.html
*/

#include <FastLED.h>
#include <stdint.h>

#define NUM_LEDS 30
#define DATA_PIN 2
#define LED_STRIP_VOLTAGE 5
#define LED_STRIP_MILLIAMPS 90  // you can increase/decrease this to get brighter/dimmer LEDs, or if you add/remove more LEDs

CRGB leds[NUM_LEDS];

void breath(uint16_t interval) {
    const uint8_t min_brightness = 2;
    static uint8_t delta = 0; // goes up to 255 then overflows back to 0

    static uint32_t pm = 0; // previous millis
    if ( (millis() - pm) > interval ) {
        pm = millis();

        // for the LEDs in the current state setting the brightness higher than max_brightness will not actually increase the brightness displayed
        uint8_t max_brightness = calculate_max_brightness_for_power_vmA(leds, NUM_LEDS, 255, LED_STRIP_VOLTAGE, LED_STRIP_MILLIAMPS);
        uint8_t b = scale8(triwave8(delta), max_brightness-min_brightness)+min_brightness;

        FastLED.setBrightness(b);

        delta++;
    }
}


void setup() {
    FastLED.setMaxPowerInVoltsAndMilliamps(LED_STRIP_VOLTAGE, LED_STRIP_MILLIAMPS);
    FastLED.setCorrection(TypicalSMD5050);
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
}


void loop() {
    static uint8_t hue = 0;

    fill_solid(leds, NUM_LEDS, CHSV(hue, 255, 255));
    breath(10);

    FastLED.show();  
    EVERY_N_MILLISECONDS(250) {hue++;}
}
