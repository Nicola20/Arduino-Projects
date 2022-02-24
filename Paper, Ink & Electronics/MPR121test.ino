/*********************************************************
This code controls a Neopixel ring with the help of a 
capacity sensor that acts as buttons for the different 
effects and colors.

Code for the Paper, Ink & Electronics modul @ Bauhaus-
Universität Weimar.

Author: Nicola Libera

**********************************************************/

#include <Wire.h>
#include "Adafruit_MPR121.h"
#include <Adafruit_NeoPixel.h>
#include "FastLED.h"
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
#define PIN 6 // On Trinket or Gemma, suggest changing this to 1

#define MAX_GLOWING_PIXELS 4
#define MIN_BRIGHTNESS 25

// Define number of NeoPixels that are attached to the Arduino
#define NUMPIXELS 24 

// When setting up the NeoPixel library, we tell it how many pixels,
// and which pin to use to send signals.
Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

uint32_t red = pixels.Color(255,0,0);
uint32_t green = pixels.Color(0,255,0);
uint32_t blue = pixels.Color(0,0,255);
uint32_t yellow = pixels.Color(255,255,0);
uint32_t white = pixels.Color(255,255,255);
uint32_t pink = pixels.Color(255,0,100);
uint32_t cyan = pixels.Color(0,255,255);
uint32_t orange = pixels.Color(230,80,0);

uint32_t fire_color = pixels.Color(80, 35, 00);
uint32_t off_color = pixels.Color(0, 0, 0);
uint32_t  colors[] = {red, green, blue, yellow, white, pink, cyan, orange};

// Variable that decides whether the pixelring should light up or not and the different colors
bool on = false;

int brightness = 0;
bool max_brightness = false;
bool run_fire_effect = true;
bool run_rainbow_fade_effect = true;
bool run_firefly_effect = true;
bool run_meteor_effect = true;
int tmp = 1;
uint32_t start_color = green;


void setup() {
  Serial.begin(9600);

  while (!Serial) { // needed to keep leonardo/micro from starting too fast!
    delay(10);
  }
  
  Serial.println("Adafruit MPR121 Capacitive Touch sensor test"); 
  
  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
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

// Fire simulator
class NeoFire
{
  Adafruit_NeoPixel &pixels;
  public:

    NeoFire(Adafruit_NeoPixel&);
    void Draw();
    void Clear();
    void AddColor(uint8_t position, uint32_t color);
    void SubstractColor(uint8_t position, uint32_t color);
    uint32_t Blend(uint32_t color1, uint32_t color2);
    uint32_t Substract(uint32_t color1, uint32_t color2);
};


// Constructor
NeoFire::NeoFire(Adafruit_NeoPixel& n_pixels): pixels (n_pixels) {
}

// Set all colors
void NeoFire::Draw() {
  Clear();

  for(int i = 0; i < NUMPIXELS; i++) {
    AddColor(i, fire_color);
    int r = random(80);
    uint32_t diff_color = pixels.Color ( r, r/2, r/2);
    SubstractColor(i, diff_color);
  }
    
  pixels.show();
}


// Set color of LED
void NeoFire::AddColor(uint8_t position, uint32_t color) {
  uint32_t blended_color = Blend(pixels.getPixelColor(position), color);
  pixels.setPixelColor(position, blended_color);
}

// Set color of LED
void NeoFire::SubstractColor(uint8_t position, uint32_t color) {
  uint32_t blended_color = Substract(pixels.getPixelColor(position), color);
  pixels.setPixelColor(position, blended_color);
}

// Color blending
uint32_t NeoFire::Blend(uint32_t color1, uint32_t color2) {
  uint8_t r1,g1,b1;
  uint8_t r2,g2,b2;
  uint8_t r3,g3,b3;
  
  r1 = (uint8_t)(color1 >> 16),
  g1 = (uint8_t)(color1 >>  8),
  b1 = (uint8_t)(color1 >>  0);
  
  r2 = (uint8_t)(color2 >> 16),
  g2 = (uint8_t)(color2 >>  8),
  b2 = (uint8_t)(color2 >>  0);
  
  return pixels.Color(constrain(r1+r2, 0, 255), constrain(g1+g2, 0, 255), constrain(b1+b2, 0, 255));
}

// Color blending
uint32_t NeoFire::Substract(uint32_t color1, uint32_t color2) {
  uint8_t r1,g1,b1;
  uint8_t r2,g2,b2;
  uint8_t r3,g3,b3;
  int16_t r,g,b;
  
  r1 = (uint8_t)(color1 >> 16),
  g1 = (uint8_t)(color1 >>  8),
  b1 = (uint8_t)(color1 >>  0);
  
  r2 = (uint8_t)(color2 >> 16),
  g2 = (uint8_t)(color2 >>  8),
  b2 = (uint8_t)(color2 >>  0);
  
  r=(int16_t)r1-(int16_t)r2;
  g=(int16_t)g1-(int16_t)g2;
  b=(int16_t)b1-(int16_t)b2;
  if(r<0) r=0;
  if(g<0) g=0;
  if(b<0) b=0;
  
  return pixels.Color(r, g, b);
}

// Every LED to black
void NeoFire::Clear() {
  for(uint16_t i = 0; i < pixels.numPixels(); i++)
    pixels.setPixelColor(i, off_color);
}


/* Stuff for the Firefly effect */
class Firefly {
  private:
    float _brightness;
    float _fadeSpeed;
    float _hue;
    boolean _isGlowing;
    
  public:
    Firefly ();
  
    float hueCenter;
    float hueRange;
    float baseSpeed;
    boolean isGlowing();    
    uint32_t getColor();
    
    void ignite();
    void animate();    
};

Firefly::Firefly () {
  hueCenter = 0.80;
  hueRange = 0.2;
  baseSpeed = 0.02;
  _brightness = 0.0;
  _fadeSpeed = 0.0;
  _isGlowing = false;
}

void Firefly::ignite() {
  _hue = hueCenter + (random(100)-50)*0.01*hueRange;
  _fadeSpeed = baseSpeed*(random(100)*0.01 + 0.5);
  _isGlowing = true;
}

boolean Firefly::isGlowing() {
  return _isGlowing;
}

void Firefly::animate() {
  if(!_isGlowing) {
    return;
  }
  
  _brightness += _fadeSpeed;
    
  if(_brightness > 1.0) {
    _brightness = 1.0;
    _fadeSpeed *= -0.3;
  } else if(_brightness < 0.0) {
    _brightness = 0.0;
    _fadeSpeed = 0.0;
    _isGlowing = false;
  }
}

uint32_t Firefly::getColor() {
  uint8_t w = 255*_hue;
  if(w < 85) {
    return Adafruit_NeoPixel::Color(w*3*_brightness, (255-w*3)*_brightness, 0);
  } else if(w < 170) {
    w -= 85;
    return Adafruit_NeoPixel::Color((255-w*3)*_brightness, 0, w*3*_brightness);
  } else {
    w -= 170;
    return Adafruit_NeoPixel::Color(0, w*3*_brightness, (255-w*3)*_brightness);
  }  
}



void rainbow_fade() {
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


void fadeToBlack(int ledNo, byte fadeValue) {
  uint32_t oldColor;
  uint8_t r, g, b;
  int value;

  oldColor = pixels.getPixelColor(ledNo);
  r = (oldColor & 0x00ff0000UL) >> 16;
  g = (oldColor & 0x0000ff00UL) >> 8;
  b = (oldColor & 0x000000ffUL);

  r = (r <= 10) ? 0 : (int) r - (r * fadeValue / 250);
  g = (g <= 10) ? 0 : (int) g - (g * fadeValue / 250);
  b = (b <= 10) ? 0 : (int) b - (b * fadeValue / 250);

  pixels.setPixelColor(ledNo, r, g, b);
}


void setPixel(int Pixel, byte red, byte green, byte blue) {
  pixels.setPixelColor(Pixel, pixels.Color(red, green, blue));
}

void setAll(byte red, byte green, byte blue) {
  for (int i = 0; i < NUMPIXELS; i++) {
    setPixel(i, red, green, blue);
  }
  pixels.show();
}


void power(uint16_t currtouched, uint16_t lasttouched, uint8_t i) {
      if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) && (i == 0)) { 
      Serial.print("I was touched "); Serial.print(on); Serial.println(i);
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


void selectColor(uint32_t col) {
  if (on) {
    for (int i = 0; i < NUMPIXELS; i ++) {
      start_color = col;
      pixels.setPixelColor(i, start_color);
    }
    pixels.show();
  }
}

void uniColor(uint16_t currtouched, uint16_t lasttouched, uint8_t i) {
        if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) && (i == 1)) {
      //pixels.clear();
      Serial.print("I was touched "); Serial.println(i);
        selectColor(cyan);
    }

    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) && (i == 2)) {
      //pixels.clear();
      Serial.print("I was touched "); Serial.println(i);
       selectColor(yellow);
    }

    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) && (i == 3)) {
      //pixels.clear();
      Serial.print("I was touched "); Serial.println(i);
       selectColor(pink);
    }

    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) && (i == 4)) {
      //pixels.clear();
      Serial.print("I was touched "); Serial.println(i);
       selectColor(red);
    }

    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) && (i == 5)) {
      //pixels.clear();
      Serial.print("I was touched "); Serial.println(i);
      selectColor(blue);
    }

    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) && (i == 6)) {
      //pixels.clear();
      Serial.print("I was touched "); Serial.println(i);
        selectColor(green);
    }
}


// Initialize the classes for the effects
Firefly flies[NUMPIXELS];
NeoFire fire(pixels);


void loop() {
  currtouched = cap.touched();

  for (uint8_t i=0; i<12; i++) {    
    // it if *is* touched and *wasnt* touched before, alert!
    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) ) {
      Serial.print(i); Serial.println(" touched");
    }
    
    power(currtouched, lasttouched, i);
    uniColor(currtouched, lasttouched, i);


      if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) && (i == 8)) {
      //pixels.clear();
      Serial.print("I was touched "); Serial.print(on); Serial.println(i);
        while (run_meteor_effect && on) {
          if (tmp != 0) {
            //meteorRain(0xaa, 0x00, 0xff, 1, 160, true, 120); 
              for (int k = 0; k < NUMPIXELS; k++) {

                // fade brightness all LEDs one step
                for (int p = 0; p < NUMPIXELS; p++) {
                  if ((!true) || (random(10) > 5)) {
                    fadeToBlack(p, 160);
                  }
      
                  currtouched = cap.touched();
                  for (uint8_t j = 0; j < 12; j++){
                    if ((currtouched & _BV(j)) && !(lasttouched & _BV(j)) && (i != j)) {
                      Serial.print(j); Serial.println(" touched in meteor");
                      run_meteor_effect = false;
                      tmp = 0;
                      if (j == 0) {
                        Serial.println("Power off was called in meteor rain");
                        power(currtouched, lasttouched, j);
                      } else {
                          uniColor(currtouched, lasttouched, j);
                      }
                    }
                  }
                }

                if (tmp != 0) {
                  // draw meteor
                  for (int p = 0; p < 1; p++) {
                    if ((k - p < NUMPIXELS) && (k - p >= 0)) {
                      setPixel(k - p, 0xaa, 0x00, 0xff);
                    }
                  }
              
                  pixels.show();
                  delay(120);
                }
              }
          }
        }
        run_meteor_effect = true;
        tmp = 1;
    }
    

    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) && (i == 9)) {
      //pixels.clear();
      Serial.print("I was touched "); Serial.print(on); Serial.println(i);
        while (run_rainbow_fade_effect && on) {
          if (tmp != 0) {
                for (int j = 0; j < (sizeof(colors)/sizeof(colors[0])); j++) {
                    for (int h = 0; h < 102; h++) {
                      currtouched = cap.touched();
                      for (uint8_t s = 0; s < 12; s++){
                        if ((currtouched & _BV(s)) && !(lasttouched & _BV(s)) && (i != s)) {
                          Serial.print(j); Serial.println(" touched");
                          run_rainbow_fade_effect = false;
                          tmp = 0;
                          if (s == 0) {
                            power(currtouched, lasttouched, s);
                          } else {
                                uniColor(currtouched,lasttouched, s);
                            }
                        }
                      }
                      if (tmp != 0) {
                      for (int s = 0; s < NUMPIXELS; s ++) {
                        pixels.setPixelColor(s, colors[j]);
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
                      delay(50);
                    }
                    }
                  }
          }
        }
        run_rainbow_fade_effect = true;
        tmp = 1;
        brightness = 0;
    }

    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) && (i == 10)) {
      //pixels.clear();
      Serial.print("I was touched "); Serial.print(on); Serial.println(i);
        while (run_fire_effect && on) {
            currtouched = cap.touched();
            for (uint8_t j = 0; j < 12; j++){
              if ((currtouched & _BV(j)) && !(lasttouched & _BV(j)) && (i != j)) {
                Serial.print(j); Serial.println(" touched");
                run_fire_effect = false;
                tmp = 0;
                if (j == 0) {
                  power(currtouched, lasttouched, j);
                } else {
                    uniColor(currtouched, lasttouched, j);
                }
              }
            }
          if (tmp != 0) {
            fire.Draw();
            delay(random(50,150)); 
          }
        }
        // next 2 lines not really needed when other taps are initialized
        //pixels.clear();
        //pixels.show();
        run_fire_effect = true;
        tmp = 1;
    }

    if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) && (i == 11)) {
      //pixels.clear();
      Serial.print("I was touched "); Serial.print(on); Serial.println(i);
        while (run_firefly_effect && on) {
            currtouched = cap.touched();
            for (uint8_t j = 0; j < 12; j++){
              if ((currtouched & _BV(j)) && !(lasttouched & _BV(j)) && (i != j)) {
                Serial.print(j); Serial.println(" touched");
                run_firefly_effect = false;
                tmp = 0;
                 if (j == 0) {
                  power(currtouched, lasttouched, j);
                } else {
                    uniColor(currtouched, lasttouched, j);
                }
              }
            }
            if (tmp != 0) {
              int glowingPixelCount = 0;
              // do we have enough lit pixels?
              for(int i = 0; i < NUMPIXELS; i++) {
                if(flies[i].isGlowing()) {
                  glowingPixelCount++;
                }
              }
              
              for(int i = 0; i < NUMPIXELS; i++) {
                if(!flies[i].isGlowing() && glowingPixelCount < MAX_GLOWING_PIXELS && random(100) < 5) {
                  flies[i].ignite();
                  glowingPixelCount++;
                }
                flies[i].animate();
                pixels.setPixelColor(i, flies[i].getColor());
              }
              pixels.show();
              delay(30); 
          }
        }
        // next 2 lines not really needed when other taps are initialized
        //pixels.clear();
        //pixels.show();
        run_firefly_effect = true;
        tmp = 1;
    }
  }


  // reset our state
  lasttouched = currtouched;

  // comment out this line for detailed data from the sensor!
  return;
  
  // put a delay so it isn't overwhelming
  delay(100);
}
