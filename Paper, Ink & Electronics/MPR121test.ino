/*********************************************************
This is a library for the MPR121 12-channel Capacitive touch sensor

Designed specifically to work with the MPR121 Breakout in the Adafruit shop 
  ----> https://www.adafruit.com/products/

These sensors use I2C communicate, at least 2 pins are required 
to interface

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada for Adafruit Industries.  
BSD license, all text above must be included in any redistribution
**********************************************************/

#include <Wire.h>
#include "Adafruit_MPR121.h"
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif

// You can have up to 4 on one i2c bus but one is enough for testing!
Adafruit_MPR121 cap = Adafruit_MPR121();

// Keeps track of the last pins touched
// so we know when buttons are 'released'
uint16_t lasttouched = 0;
uint16_t currtouched = 0;

/*Define stuff for the Neopixel Ring*/
// Which pin on the Arduino is connected to the NeoPixels?
#define PIN        6 // On Trinket or Gemma, suggest changing this to 1

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS 24 // Popular NeoPixel ring size

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals. Note that for older NeoPixel
// strips you might need to change the third parameter -- see the
// strandtest example for more information on possible values.
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

uint32_t red = pixels.Color(255,0,0);
uint32_t green = pixels.Color(0,255,0);
uint32_t blue = pixels.Color(0,0,255);
uint32_t yellow = pixels.Color(255,255,0);
uint32_t white = pixels.Color(255,255,255);
uint32_t pink = pixels.Color(255,0,100);
uint32_t cyan = pixels.Color(0,255,255);
uint32_t orange = pixels.Color(230,80,0);
uint32_t  colors[] = {red, green, blue, yellow, white, pink, cyan, orange};

// Variable that decides whether the pixelring should light up or not and the different colors
bool on = false;

int brightness = 0;
bool max_brightness = false;

uint32_t start_color = green;

#define DELAYVAL 500 // Time (in milliseconds) to pause between pixels


void setup() {
  Serial.begin(9600);

  while (!Serial) { // needed to keep leonardo/micro from starting too fast!
    delay(10);
  }
  Serial.println("I came this far");
  Serial.println("Adafruit MPR121 Capacitive Touch sensor test"); 
  Serial.println("I came this far");
  
  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  Serial.println("I came this far");
  if (!cap.begin(0x5A)) {
     Serial.println("I came this far");
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
    Serial.println("I came this far");
  Serial.println("MPR121 found!");

  // Stuff for Neopixel init
  #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
  #endif
  // END of Trinket-specific code.

  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
  //pixels.setBrightness(brightness);
  pixels.show(); // Turn all the LEDs off
  Serial.println("Initialized Neopixels");
  
}

void rainbox_fade() {
    for (int j = 0; j < (sizeof(colors)/sizeof(colors[0])); j++) {
    for (int h = 0; h < 102; h++) {
      for (int i = 0; i < NUMPIXELS; i ++) {
        pixels.setPixelColor(i, colors[j]);
      }
      if (max_brightness == false) {
        if (brightness != 255) {
          brightness = brightness + 5;
        } 
        if (brightness == 255) {
          max_brightness = true;
          
        }
      } else {
        if (brightness != 0) {
          brightness = brightness - 5;
        } 
        if (brightness == 0) {
          max_brightness = false;
        }
      }
      pixels.show();
      pixels.setBrightness(brightness);
      delay(70);
    }
  }
}

void loop() {

  // Get the currently touched pads
  currtouched = cap.touched();

  for (uint8_t i=0; i<12; i++) {
    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) && (i == 0)) { 
      if (on == false) {
          on = true;
          for (int i = 0; i < NUMPIXELS; i ++) {
            pixels.setPixelColor(i, start_color);
          }
          pixels.show();
          pixels.setBrightness(255);
      } else {
        on = false;
        pixels.clear();
        pixels.show();
      }
    }
  }

  
  //Serial.println(currtouched);
 /* for (uint8_t i=0; i<12; i++) {
    // it if *is* touched and *wasnt* touched before, alert!
    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) ) {
      Serial.print(i); Serial.println(" touched");
    }

    
    // if it *was* touched and now *isnt*, alert!
    if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)) ) {
      Serial.print(i); Serial.println(" released");
    }
  }*/

  // reset our state
  lasttouched = currtouched;

  // comment out this line for detailed data from the sensor!
  return;
  
  // debugging info, what
  Serial.print("\t\t\t\t\t\t\t\t\t\t\t\t\t 0x"); Serial.println(cap.touched(), HEX);
  Serial.print("Filt: ");
  for (uint8_t i=0; i<12; i++) {
    Serial.print(cap.filteredData(i)); Serial.print("\t");
  }
  Serial.println();
  Serial.print("Base: ");
  for (uint8_t i=0; i<12; i++) {
    Serial.print(cap.baselineData(i)); Serial.print("\t");
  }
  Serial.println();
  
  // put a delay so it isn't overwhelming
  delay(100);
}
