#ifndef DISPLAY_H
#define DISPLAY_H

#include <Adafruit_NeoPixel.h>
#include "CharacterSet.h"
#include "CharacterSet15x15Hex.h"
#include <Arduino.h>

#define NUMPIXELS 60  
#define NUM_STRIPS 15

// Colour variable
extern uint32_t currentTopColourHex;
extern uint32_t currentBottomColourHex;
extern uint32_t currentFullColourHex;

class Display
{
private:
  // Singleton instance
  static Display* instance;
  
  // Private constructor for singleton pattern
  Display();
  
  // LED strip configuration
  const int stripPins[NUM_STRIPS] = {30, 31, 32, 33, 34, 35, 36, 37, 29, 28, 27, 26, 25, 24, 23};
  
  
  // Frame buffer for pixel data
  uint32_t frameBuffer[NUM_STRIPS][NUMPIXELS];
  
  // Character drawing helper methods
  int getCharacterWidth7x7(char c);
  int getCharacterWidth15x15(char c);
  void drawCharacter7x7(char c, int x, int y, uint32_t color);
  void drawCharacter15x15(char c, int x, int y, uint32_t color);
public:
  // Singleton accessor
  static Display& getInstance();
  Adafruit_NeoPixel strips[NUM_STRIPS];
  
  // Delete copy constructor and assignment operator
  Display(const Display&) = delete;
  void operator=(const Display&) = delete;
  
  // Display setup
  void setup(int brightness);
  
  // Buffer management
  void clearBuffer(bool bigFont);
  void updateLEDs(bool bigFont);
  void setPixel(int x, int y, uint32_t color);

  void setBrightness(int brightness);
  
  // Text display methods
  void displayText(const char* text1, const char* text2, const char* command, const char* displayType);

  // Colour management
  void setTopColour(const uint32_t colourHex);
  void setBottomColour(const uint32_t colourHex);
  void setFullColour(const uint32_t colourHex);

  // For custom drawings
  void displayCustomPixels(String text1);
};

#endif // DISPLAY_H