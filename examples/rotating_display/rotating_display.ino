/*
 * This file is part of POV library   by Alexander Kirillov <shurik179@gmail.com>
 * See github.com/shurik179/pov-library for details
 * Distributed under the terms of MIT license; see LICENSE file in the repository for details.
 *
 *  Requires the following libraries:
 *  FastLED
 *  Adafruit_TinyUSB
 *  Adafruit_SPIFlash
 *  Sd_Fat (adafruit fork)
 *
 *
 * This is a test of the POV library using a Hall effect sensor or a similar
 *    input as a trigger. It functions as follows:
 *
 *  - if at startup pin PIN_MODE_SELECT (defined below) is pulled low, it puts the staff in
 *    image upload mode; if the staff is connected to the computer by USB cable, it appears
 *    as an external drive so you can drag and drop your BMP images to it
 *
 *  - otherwise, the staff goes into show mode, showing just one image (to
 *    select the image edit the line #define IMAGE below. The frame rate (i.e. how many
 *    lines to show pere second) is determined by value of LINES_PER_SEC below
 *    It shows the image once and then waits until pin PIN_TRIGGER is pulled low
 *    after which it restarts the image.
 *
 *    This is common method for rotating displays: PIN_TRIGGER would be
 *    connected to a sensor (e.g. a Hall effect sensor) which is triggered as
 *    the staff rotates to a certain position.  To gurarantee that each activation
 *    of the trigger pin only is counted once, we add some blackout period for
 *    debouncing
 *
 *
 *
 * Before uploading the sketch to the staff, make sure to change the #define'd values to match your setup:
 *  NUM_PIXELS, LED_TYPE, COLOR_ORDER, PIN_MODE_SELECT, PIN_TRIGGER, LINES_PER_SEC, IMAGE
 *  Also, for M4 based boards, make sure that in your Arduino IDE you have selected
 *  Tools->USB stack: TinyUSB
 *  Finally it is assumed that you have already created the FAT filesystem on your
 *  flash memory, using SdFat_format example sketch from Sd_Fat library (Adafruit fork)
 */
#include <FastLED.h>
#include <pov.h>
//number of pixels in your strip/wand
#define NUM_PIXELS 30
// Strip type. Common options are DOTSTAR (APA102, SK9822) and NEOPIXEL (WS2812B, SK6812 and
// compatible). For other options, see FastLED documentation
#define LED_TYPE DOTSTAR
// color order. For DOTSTAR (APA102), common order is BGR
// For NeoPixel (WS2812B), most common is  GRB
#define COLOR_ORDER BGR

/*Mode selection pin
  If at startup this pin is pulled low, staff goes into image upload mode;
  otherwise, it goes into usual (show)  mode
*/
#define PIN_MODE_SELECT 5

// Trigger pin -- pull low to restart shwoing the image
#define PIN_TRIGGER 6

// Blackou time, in ms
#define BLACKOUT_TIME 150

// frame rate
#define LINES_PER_SEC 150.0f
uint32_t interval=1000000/LINES_PER_SEC; //interval between lines of image, in microseconds

#define IMAGE "rg-lines.bmp"


/* Global Variables */
CRGB leds[NUM_PIXELS];
POV staff(NUM_PIXELS, leds);
uint32_t blackoutEnd=0;   //for debouncing trigger reading; time to start checking again, in ms





void setup(){
// If using hardware SPI, use this version
    FastLED.addLeds<LED_TYPE, COLOR_ORDER>(leds, NUM_PIXELS);
//If NOT using hardware SPI, comment the previous line. Instead,
// use one of the versions below,
// replacing DATA_PIN and CLOCK_PIN by correct pin numbers
// FastLED.addLeds<DOTSTAR, DATA_PIN, CLOCK_PIN, COLOR_ORDER>(leds, NUM_PIXELS);
// FastLED.addLeds<NEOPIXEL, DATA_PIN, COLOR_ORDER>(leds, NUM_PIXELS);

    pinMode(PIN_MODE_SELECT, INPUT_PULLUP);
    pinMode(PIN_TRIGGER, INPUT_PULLUP);

    //if pin is pulled low, go into upload mode!
    //note: in this case, there should be no Serial.begin() before this, and no delay()
    if (digitalRead(PIN_MODE_SELECT)==LOW) {
        staff.begin(MODE_UPLOAD);
        //do nothing else, do not run loop() -- just let TinyUSB do its job
        while (1) yield();
    } else {
        //otherwise, regular show
        staff.begin(MODE_SHOW);
        // blink to indicate that staff is alive and working.
        // You can use any of predefined CRGB colors: https://github.com/FastLED/FastLED/wiki/Pixel-reference#predefined-colors-list
        // You can also omit the color; in this case, it will default to red.
        staff.blink(CRGB::Red);
        staff.addImage(IMAGE);
    }


}

//note that loop() will only run in MODE_SHOW
void loop(){
        checkTrigger();
        updateStaff();
}

void checkTrigger(){
    if ( (millis()>blackoutEnd) && (digitalRead(PIN_TRIGGER)== LOW)  ){
        //trigger acivated
        //start blackout, for debouncing
        blackoutEnd=millis()+BLACKOUT_TIME;
        staff.restartImage();
        staff.paused=false;
    }
}

void updateStaff(){
    int16_t lineNumber;
    if ( !staff.paused   && (staff.timeSinceUpdate()>interval)  ) {
        lineNumber = staff.showNextLine();
        if (lineNumber == 0) {
            staff.paused = true;
            staff.blank();
        }
    }
}
