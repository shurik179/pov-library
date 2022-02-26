#include "pov.h"

POV::POV(uint16_t n, CRGB * l)
    :numPixels(n), paused(false) {
        leds=l;
    }

void POV::begin(uint8_t mode){
    //Serial.println("Starting...")
    _mode=mode;

    //init external flash
    flash.begin();
    // Open file system on the flash
    if ( !fatfs.begin(&flash) ) {
        Serial.begin(9600);
        Serial.println("Error: filesystem doesn't not exist. Please try SdFat_format example to make one.");
        while(1) yield();
    }
    if (_mode==MODE_UPLOAD) {
        msc_init();
        Serial.begin(9600);
        delay(1000);
        Serial.println("Mass storage device started ");
        Serial.print("JEDEC ID: "); Serial.println(flash.getJEDECID(), HEX);
        Serial.print("Flash size: "); Serial.println(flash.size());
    } else {
        FastLED.setCorrection( TypicalSMD5050 );
    }
    FastLED.show();
}

void POV::showValue(float v){
    uint16_t level=0;
    uint8_t i, value;
    uint32_t c;
    FastLED.clear();
    if (v<0.0) {
      v=0.0;
    } else if (v>1.0) {
      v=1.0;
    }
    level = v*numPixels;
    if (level>numPixels) level=numPixels;
    //do gradient fill of the hole leds array
    fill_gradient_RGB(leds, numPixels, CRGB::Red, CRGB::Green);
    //now, black out everything above level
    if (level<numPixels){
        fill_solid(&(leds[level]), numPixels-level, CRGB::Black );
    }
    FastLED.show();
    lastLineUpdate=micros();
}

void POV::blank(){
    FastLED.clear(true);
    FastLED.show();
}



void POV::show(){
    FastLED.show();
    lastLineUpdate=micros();
}

void POV::setBrightness(uint8_t b){
    FastLED.setBrightness(b);
}

void POV::setPixel(uint16_t i, uint32_t c){
    leds[i]=c;
}

void POV::showLine(byte * line, uint16_t size){
    uint16_t i,pos;
    uint8_t r,g,b;
    for (i=0; i<numPixels; i++) {
        if (i<size) {
            pos=3*i;
            //using bgr order
            b=line[pos++];
            g=line[pos++];
            r=line[pos];
            leds[i].setRGB(r,g,b);
        } else {
           leds[i]=0x00;
        }
    }
    FastLED.show();
    lastLineUpdate=micros();
}

void POV::blink(CRGB color){
    uint16_t i;

    //repeat twice
    for (uint8_t j=0; j<2; j++){
        FastLED.clear(true);
        delay(500);
        for(i=0; 8*i<numPixels; i++) {
            leds[8*i]=color;
        }
        FastLED.show();
        delay(500);
    }
    FastLED.clear(true);
}

void POV::addImage(char * filename, uint16_t duration){
    BMPimage * ptr;
    ptr=imageList.addImage(filename,duration);
    imageList.current()->load();
    currentLine=0;
}

uint8_t POV::addImageList(char * filename){
    currentLine=0;
    uint8_t count=imageList.addFromFile(filename);
    if (count) {
        //load first image in buffer
        imageList.current()->load();
    }
    return count;

}

void POV::clearImageList(){
    imageList.reset();
    currentLine=0;
}

void POV::firstImage(){
    imageList.current()->unload();
    imageList.first();
    imageList.current()->load();
    currentLine=0;
}

void POV::nextImage(){
    if (paused) return;
    imageList.current()->unload();
    imageList.next();
    imageList.current()->load();
    currentLine=0;
}

int16_t POV::showNextLine(){
    BMPimage * currentImg=imageList.current();
    if (currentImg==NULL) return 0;
    if (paused) return currentLine;
    //move to next line
    showLine(currentImg->line(currentLine), currentImg->width());
    currentLine++;
    if (currentLine == currentImg->height()) {currentLine=0;}
    return currentLine;
}
