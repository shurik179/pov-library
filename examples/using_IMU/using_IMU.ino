/*
 * This file is part of POV Staff project by Alexander Kirillov <shurik179@gmail.com>
 * See github.com/shurik179/pov-library for details
 * Distributed under the terms of MIT license; see LICENSE file in the repository for details.
 *
 *  Requires the following libraries:
 *  FastLED
 *  Adafruit_TinyUSB
 *  Adafruit_SPIFlash
 *  Sd_Fat (adafruit fork)
 *  Adafruit_Sensor
 *  Adafruit_MPU6050
 *
 * This is an example combining  POV library with  MPU6050 Inertial Motion Unit
 * (IMU). It assumes that you have the IMU placed on the POV staff, with x-axis
 * going along the staff, and  connected to the microcontroller uisng I2C bus.
 *
 * It functions as follows:
 *  - if at startup pin PIN_MODE_SELECT (defined below) is pulled low, it puts the staff in
 *    image upload mode; if the staff is connected to the computer by USB cable, it appears
 *    as an external drive so you can drag and drop your BMP images to it
 *
 *  - otherwise, the staff goes into show mode, showing just one image (to
 *    select the image edit the line #define IMAGE below). It uses the IMU to
 *    detect is staff is at rest (horizontal, not moving). If it is, staff is blanked.
 *    Otherwise, it will show the selected image; the frame rate (i.e. how many
 *    lines to show per second) is adjusted depending on the rotation speed,
 *    trying to keep number of lines per degree of rotation constant.
 *
 *
 * Before uploading the sketch to the staff, make sure to change the #define'd values to match your setup:
 *  NUM_PIXELS, LED_TYPE, COLOR_ORDER, PIN_MODE_SELECT, DEG_PER_LINE, IMAGE. Also
 * check the MPU6050 I2C: depending on whether you pull pin of MPU6050 high or low,
 * the address can be either 0x69 or 0x68.
 *
 *  For M4 and RP2040 based boards, make sure that in your Arduino IDE you have selected
 *  Tools->USB stack: TinyUSB
 *  Finally it is assumed that you have already created the FAT filesystem on your
 *  flash memory, using SdFat_format example sketch from Sd_Fat library (Adafruit fork)
 */
#include <FastLED.h>
#include <pov.h>
#include "IMU.h"
//number of pixels in your strip/wand
#define NUM_PIXELS 30
// Strip type. Common options are DOTSTAR (APA102, SK9822) and NEOPIXEL (WS2812B, SK6812 and
// compatible). For other options, see FastLED documentation
#define LED_TYPE DOTSTAR
// color order. For DOTSTAR (APA102), common order is BGR
// For NeoPixel (WS2812B), most common is  GRB
#define COLOR_ORDER BGR

#define IMU_ADDRESS 0x69


/*Mode selection pin
  If at startup this pin is pulled low, staff goes into image upload mode;
  otherwise, it goes into usual (show)  mode
*/
#define PIN_MODE_SELECT 5

// frame rate. Instead of using constant frame rate per second, we will adjust
// depending on rotation speed

// how many degrees of staff turn between successive lines?
#define DEG_PER_LINE 1.0f

#define IMAGE "rg-lines.bmp"


/* Global Variables */
CRGB leds[NUM_PIXELS];
POV staff(NUM_PIXELS, leds);

uint32_t lastIMUcheck = 0; //when did we last check IMU speed, in ms
float speed=0.0;           //staff rotation speed, in deg/s



void setup(){
// If using hardware SPI, use this version
    FastLED.addLeds<LED_TYPE, COLOR_ORDER>(leds, NUM_PIXELS);
//If NOT using hardware SPI, comment the previous line. Instead,
// use one of the versions below,
// replacing DATA_PIN and CLOCK_PIN by correct pin numbers
// FastLED.addLeds<DOTSTAR, DATA_PIN, CLOCK_PIN, COLOR_ORDER>(leds, NUM_PIXELS);
// FastLED.addLeds<NEOPIXEL, DATA_PIN, COLOR_ORDER>(leds, NUM_PIXELS);

    pinMode(PIN_MODE_SELECT, INPUT_PULLUP);

    //if pin is pulled low, go into upload mode!
    //note: in this case, there should be no Serial.begin() before this, and no delay()
    if (digitalRead(PIN_MODE_SELECT)==LOW) {
        staff.begin(MODE_UPLOAD);
        //do nothing else, do not run loop() -- just let TinyUSB do its job
        while (1) yield();
    } else {
        //otherwise, regular show
        staff.begin(MODE_SHOW);
        //debugging
        Serial.begin(115200);
        //Serial.println("Starting...");
        //delay(500);

        // blink to indicate that staff is alive and working.
        // You can use any of predefined CRGB colors: https://github.com/FastLED/FastLED/wiki/Pixel-reference#predefined-colors-list
        // You can also omit the color; in this case, it will default to red.
        staff.blink(CRGB::Green);
        staff.addImage(IMAGE);
        if ( IMUbegin(IMU_ADDRESS) ) {
          Serial.println("IMU STARTED");
        } else {
          Serial.println("IMU failed to start");
        }
    }


}

//note that loop() will only run in MODE_SHOW
void loop(){
    if (millis()-lastIMUcheck > 100 ) {
        //let's check if staff is at rest. To avoid overloading the MCU, we only do it 10 times/sec.
        lastIMUcheck = millis();
        staff.paused = IMUatRest();
        if (staff.paused) staff.clear();
        //also, get  rotation speed (in deg/s)
        speed = IMUrotationSpeed();
    }
    //how much has the staff turned since last update?
    float rotAngle = speed * staff.timeSinceUpdate() * 0.000001;

    if ( (!staff.paused) && (rotAngle>DEG_PER_LINE)) {
        staff.showNextLine();
    }
}
