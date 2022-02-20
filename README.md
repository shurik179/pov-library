# POV Library
Arduino library for Persistence of Vision (POV) projects.

Work in progress; not ready for public use yet.


This library is for creating Persistence of Vision displays using NeoPixel (WS2812B)
or DotStar (APA102) individually addressable LED strips.

The library  is intended for use with boards that have built-in flash memory.
It supports two modes:

 -- Upload mode. In this mode, the board, when connected to a computer via USB,
   appears as external USB drive, allowing the user to upload images in bitmap
   format. One can also upload a text file with list of images and durations.

-- Show mode. In this mode, the board reads the image(s) from the flash memory
   and shows them on LED strips one line at a time. Normally, the LED strips
   would be mounted on a rotating staff/wand/fan blade which would create
   persistence of vision illusion.

## Supported boards

The library uses Adafruit_TinyUSB library; thus, it can only be used with
boards supported by this library, such as M4 and RP2040-based boards. Full
list can be found at https://github.com/adafruit/Adafruit_TinyUSB_Arduino



## Image format
Individual images should be uploaded to the root directory of the flash drive.
Filenames are limited to 30 symbols and should only use letters a-z, digits 0-9, and underscore.

Bitmap files should be full color (24bit); image width and height are limited to.

Image list file

## API

The library defines class *POVstaff*. 

POVstaff(uint16_t length, CRGB * l);
//POVstaff(uint16_t length, uint8_t dataPin, uint8_t clockPin);
void begin(uint8_t mode);


uint8_t mode() {return _mode;}

void setPixel(uint16_t i, uint32_t c);
void clear();
void show();
void setBrightness(uint8_t b);

/* quickly blink each 8th LED red, for "I am alive" indication */
void blink(CRGB color=CRGB::Red);

/* shows a value (0 -1.0) by proportion of LEDs on staff */
void showValue(float v);

/* Shows one line. line should be pointer to array which holds  pixel colors
 * (3 bytes per pixel, in BGR order). Note: 3, not 4!!!
 *  size should be size of array (number of pixels, not number of bytes)
 */
void showLine(byte * line, uint16_t size);

/* Reads from file an image and adds it to  the list, making it current
  image */
void addImage(char * filename, uint16_t duration=0);
/*
 reads list of images from file and adds it to the staff  imageList
 */
uint8_t addImageList(char * filename);

/* moves to the next image, making it current. If reached the end
 continues from first image
*/
void nextImage();

void restartImage(){currentLine = 0; lastLineUpdate=micros();}
/* Show next line of active image
   Retunrs the index of next line to be shown (not yet shown!)
   If it retunrs 0, it means we have completed showing the image and
    next call will start again
*/
int16_t showNextLine();

//time since strip was last updated, in micro sec
uint32_t timeSinceUpdate() {return (micros()-lastLineUpdate);}


BMPimage * currentImage() {return imageList.current();}

uint16_t currentDuration() {return imageList.currentDuration();}
