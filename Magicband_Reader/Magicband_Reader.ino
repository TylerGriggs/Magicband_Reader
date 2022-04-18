// Created by Tyler Griggs
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif
#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN         D3          // RFID Reset
#define SS_PIN          D4          // RFID PIN
#define SOUND_PIN       D1          // Sound Trigger when set to LOW
#define LEDPIN          D2          // LED Pin on controller
#define BRIGHTNESS 240
#define OUTER_RING 60
#define INNER_RING 50
#define NUM_PIXELS 110    // TOTAL of Outer Ring + Inner Ring


// Init RGB strip
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_PIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);
//Init RFID Reader
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

void setup() {
  SPI.begin();            // Init SPI bus
  mfrc522.PCD_Init();     // Init MFRC522
  delay(4);               // Optional delay. Some board do need more time after init to be ready, see Readme

  strip.begin();          // Init LED strip
  strip.setBrightness(BRIGHTNESS);
  strip.show(); 

  randomSeed(analogRead(0));  // Seed Random with a number (random Voltage reading) from unconnected pin
  
  pinMode(D0, OUTPUT);     // LED Pin
  pinMode(D1, OUTPUT);     // Soundboard Trigger Pin
  digitalWrite(D0, LOW);
  digitalWrite(D1, HIGH);  // Sound is triggered when connected to ground (low voltage)
}

void loop() {
  // Look for new cards
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  
  // This code runs when a new RFID signal is detected
  // Pick one of the Magicband Reader colors
  int red = 0;
  int green = 0;
  int blue = 0;
  int mod = random(300, 5000) % 3;
  switch(mod) {
    case 0:   // White
      red = 255;
      green = 255;
      blue = 255;
      break;
    case 1:   // Green
      red = 0;
      green = 255;
      blue = 0;
      break;
    case 2:   // Blue
      red = 0;
      green = 120;
      blue = 255;
      break;
  }
  streakSpin(red, green, blue);
  digitalWrite(D1, LOW);    // Triggers the Soundboard FX
  delay(100);
  fadeUp(red, green, blue);
  digitalWrite(D1, HIGH);
  fadeDown(red, green, blue);
  
} // End Loop

void streakSpin(int r, int g, int b) {
  uint32_t c = strip.Color(r, g, b);
  uint32_t off = strip.Color(0, 0, 0);
  uint16_t streakSize = OUTER_RING/2;                   // Number of LEDs in Outer moving streak
  uint16_t innerStreakSize = INNER_RING/10;             // Number of LEDs in Inner moving streak
  uint16_t delaySpeed = 10;                             // Speed of Streaks
  uint16_t innerLoc = 0;                                // Keep track of Inner streak location
  uint16_t pattern;
  
  for (uint16_t repeat = 0; repeat <= 4; repeat++){
    // OUTER RING Animation
    for(uint16_t i=0; i < OUTER_RING; i++) {            // OUTER Sreak Loop
      for(uint16_t s = 0; s < streakSize; s++) {        
        pattern = i + s;                                // The current LED in the OUTER streak pattern
        if (pattern > OUTER_RING-1){                    // If streak goes beyond the end of the ring
            pattern = pattern - OUTER_RING;             // Recalculate LED number ( 65 -(60) = 5 )
          }
        strip.setPixelColor(pattern, c);                // Set OUTER LED with color
        }
        
      for(uint16_t s=0; s < innerStreakSize; s++) {     // INNER Streak Loop
        if(innerLoc > INNER_RING-1) {innerLoc = 0;}     // Reset the Inner Ring loction placholder if needed
        pattern = innerLoc + s;                         // The current LED in the INNER streak pattern
        if (pattern > INNER_RING-1){                    // If streak goes beyond the end of the ring
            pattern = pattern - INNER_RING;             // Recalculate LED number ( 52 -(50) = 2 )
          }
        strip.setPixelColor(OUTER_RING + pattern, c);   // Set INNER LED with color
        }
      strip.show();                                     // Update the Strip (THIS IS WHERE THE MAGIC HAPPENS)
      delay(delaySpeed);                                // Delay to see the update with human eyes
      innerLoc++;
      
      // Reset Streak calculation by theoretically turning "off" all the pixels again (before updating again)
      for(uint16_t i=0; i<strip.numPixels(); i++) {
        strip.setPixelColor(i, off);
        }
      }
      
    delaySpeed = delaySpeed - 2;                        // Speed Up
    
    } // Repeat Streaks
  } // End streakSpin()



void fadeUp(int r, int g, int b) {
  uint32_t c;
  for (int pad = 255; pad >= 0; pad--){
    int red = r-pad; if (red < 0) {red=0;}
    int green = g-pad; if (green < 0) {green=0;}
    int blue = b-pad; if (blue < 0) {blue=0;}
    c = strip.Color(red, green, blue);
    for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
    }
    strip.show();
    delay(5);
    }
  }

void fadeDown(int r, int g, int b) {
  uint32_t c;
  for (int pad = 0; pad <= 255; pad++){ 
    int red = r-pad; if (red < 0) {red=0;}
    int green = g-pad; if (green < 0) {green=0;}
    int blue = b-pad; if (blue < 0) {blue=0;}
    c = strip.Color(red, green, blue);
    for(uint16_t i=0; i<strip.numPixels(); i++){
      strip.setPixelColor(i, c);
    }
    strip.show();
    delay(10);
    }
  }

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
    }
  }
