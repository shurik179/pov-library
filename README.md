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

* Images must be in bitmap (BMP) format, with 24 bit color depth. Use any
  software you like to convert images in other formats to bmp - e.g. you can use
  Microsoft Paint 3D.

* images are shown one **horizontal** line at a time. Thus, image width must
  match the number of LEDs on your strip.

* Maximal image size is 21000 pixels (e.g., it is enough for  72x288 pixel image).

* Image filenames must be at most 30 symbols long and can only contain letters,
  numbers, dashes and underscores. No spaces or special symbols!

You can create your own images or search for existing ones.  A good source for
POV image patterns is [Visual  POI Zone](https://visualpoi.zone/patterns/).
You will  need to rotate images 90 degrees.


Please note that the same color (i.e. the same RGB values) can look quite
different on the screen of your computer and on LED strip. Experiment with
colors to get some feel for it.


## Imagelist format

Library allows you to add individual images to the list of images to be shown,
or do the batch add from an image list file. This file  must contain the
 list of image files in the order you want
to use them in your show, one filename per line, including `.bmp` extension.
Optionally, you can also add how long the image should be shown, in seconds
(whole numbers only!), separated from  filename by one or more spaces
```
image1.bmp 20
image2.bmp 41
image5.bmp 10
image1.bmp
```
You can include some image file more than once, or not at all - it is your
choice.

## API

The library defines class *POVstaff* which describes your POV device (staff, poi,...).
It provides the following methods:

### Initialization

* `POVstaff(uint16_t length, CRGB * l)`: initializer. Parameter length is
  number of pixels and `l` is a pointer to a LED strip object as described in
  FastLED documentation.     

* `void begin(uint8_t mode)`: start the POV object. Mode can be one of two defined constants:
  `MODE_SHOW` or `MODE_UPLOAD`.

* `uint8_t mode()`: returns current

### Low-level pixel manipulation

* `void setPixel(uint16_t i, uint32_t c)`

* `void clear()`

* `void show()`

* `void setBrightness(uint8_t b)`

* `void blink(CRGB color=CRGB::Red)`: quickly blink each 8th LED red, for "I am alive" indication


* `void showValue(float v)`: shows a value (0 -1.0) by proportion of LEDs on staff

* `void showLine(byte * line, uint16_t size)`: Shows one line. `line` should be
  pointer to array which holds  pixel colors (3 bytes per pixel, in BGR order).
  Note: 3, not 4!!!  `size` should be size of array (number of pixels, not number of bytes).

### Working with images and image lists

The POVstaff object at all time maintains a list of images to be shown together
with durations (in seconds). You can add images to this list, move to next image,
etc. It also keeps track of current line in the image currently shown.

* `void addImage(char * filename, uint16_t duration=0)`:
  Reads from file an image and adds it to  the list, making it current
  image

* `uint8_t addImageList(char * filename)`: reads list of images from image list
  file and adds it to the end of POVstaff  image list

* `void nextImage()`: moves to the next image, making it current. If reached the end
  continues from first image


* `void restartImage()`:{currentLine = 0; lastLineUpdate=micros()}

* `int16_t showNextLine()`:  Show next line of active image.
   Returns the index of next line to be shown (not yet shown!)
   If it returns 0, it means we have completed showing the image and
    next call will start again


* `uint32_t timeSinceUpdate()`: time since strip was last updated, in micro sec


* `BMPimage * currentImage()`

* `uint16_t currentDuration()`


## Basic example
