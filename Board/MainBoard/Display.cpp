#include "Display.h"

// Initialize static instance to nullptr
Display* Display::instance = nullptr;

extern uint32_t currentTopColourHex = 0xFF0000;
extern uint32_t currentBottomColourHex = 0xFF0000;
extern uint32_t currentFullColourHex = 0xFF0000;

// Singleton accessor
Display& Display::getInstance()
{
  if (instance == nullptr)
  {
    instance = new Display();
  }

  return *instance;
}

// Constructor
Display::Display()
{
  // Initialize frameBuffer to zero
  for (int i = 0; i < NUM_STRIPS; i++)
  {
    for (int j = 0; j < NUMPIXELS; j++)
    {
      frameBuffer[i][j] = 0;
    }
  }
}

// Initialize LED Matrix
void Display::setup(int brightness)
{
  for (int i = 0; i < NUM_STRIPS; i++)
  {
    strips[i] = Adafruit_NeoPixel(NUMPIXELS, stripPins[i], NEO_GRB + NEO_KHZ800);
    strips[i].begin();
    strips[i].setBrightness(brightness);
    strips[i].show();
  }
}

// To set the brightness
void Display::setBrightness(int brightness)
{
  for (int i = 0; i < NUM_STRIPS; i++)
  {
    strips[i].setBrightness(brightness);
  }
}

// Clear Frame Buffer
void Display::clearBuffer(bool bigFont)
{
  int rows = bigFont ? 15 : 7;
  for (int i = 0; i < NUM_STRIPS; i++)
  {
    for (int j = 0; j < NUMPIXELS; j++)
    {
      frameBuffer[i][j] = 0;  // Reset all pixels
    }
  }
}

// Update LEDs from Buffer
void Display::updateLEDs()
{

  for (int i = 0; i < NUM_STRIPS; i++)
  {
    for (int j = 0; j < NUMPIXELS; j++)
    {
      strips[i].setPixelColor(j, frameBuffer[i][j]);
    }
    strips[i].show();
  }

}

void Display::setPixel(int x, int y, uint32_t color)
{
  if (x < 0 || x >= NUMPIXELS || y < 0 || y >= NUM_STRIPS)
    return;  // Ignore out-of-bounds pixels
  frameBuffer[y][x] = color; // Update pixel in frame buffer
}

int Display::getCharacterWidth7x7(char c)
{
  int index = getCharIndex(c);
  if (index == -1) return 0;  // Return 0 if character not found

  int minCol = 7, maxCol = 0;

  // Find the actual width of the character
  for (int row = 0; row < 7; row++)
  {
    for (int col = 0; col < 7; col++)
    {
      if (pgm_read_byte(&(charSet7x7[index][row][col])))
      {  // Check if pixel is set
        if (col < minCol) minCol = col;
        if (col > maxCol) maxCol = col;
      }
    }
  }

  return (maxCol >= minCol) ? (maxCol - minCol + 1) : 1;  // Ensure at least 1-pixel width
}

int Display::getCharacterWidth15x15(char c)
{
  int index = getCharIndex15x15(c);
  if (index == -1) return 0;  // Return 0 if character not found

  int minCol = 15, maxCol = 0;

  for (int row = 0; row < 13; row++)  // Scan only 13 rows
  {
    uint16_t rowData = pgm_read_word(&(charSet15x15Hex[index][row]));

    for (int col = 0; col < 15; col++)
    {
      if (rowData & (1 << (14 - col)))   // Check if pixel is set
      {
        if (col < minCol) minCol = col;

        if (col > maxCol) maxCol = col;
      }
    }
  }

  return (maxCol >= minCol) ? (maxCol - minCol + 1) : 0;  // Ensure at least 1-pixel width
}

void Display::drawCharacter7x7(char c, int x, int y, uint32_t color)
{
  int index = getCharIndex(c);
  if (index == -1) return;

  int minCol = 7, maxCol = 0;

  // Find the actual left and right boundaries
  for (int row = 0; row < 7; row++)
  {
    for (int col = 0; col < 7; col++)
    {
      if (pgm_read_byte(&(charSet7x7[index][row][col])))  // If pixel is set
      {
        if (col < minCol) minCol = col;

        if (col > maxCol) maxCol = col;
      }
    }
  }

  int charWidth = maxCol - minCol + 1;

  // Draw only the necessary part of the character
  for (int row = 0; row < 7; row++)
  {
    for (int col = minCol; col <= maxCol; col++)
    {
      if (pgm_read_byte(&(charSet7x7[index][row][col])))
      {
        setPixel(x + (col - minCol), y + row, color);
      }
    }
  }
}

void Display::drawCharacter15x15(char c, int x, int y, uint32_t color)
{
  int index = getCharIndex15x15(c);

  if (index == -1)
    return;

  int minCol = 15, maxCol = 0;

  // Find the actual left and right boundaries
  for (int row = 0; row < 13; row++)
  {
    uint16_t rowData = pgm_read_word(&(charSet15x15Hex[index][row]));

    for (int col = 0; col < 15; col++)
    {
      if (rowData & (1 << (14 - col))) // Pixel is set
      {
        if (col < minCol) minCol = col;
        if (col > maxCol) maxCol = col;
      }
    }
  }

  int charWidth = maxCol - minCol;

  // Draw only the necessary part of the character
  for (int row = 0; row < 13; row++)
  {
    uint16_t rowData = pgm_read_word(&(charSet15x15Hex[index][row]));

    for (int col = minCol; col <= maxCol; col++)
    {
      if ((rowData >> (14 - col)) & 1)
      {
        setPixel(x + (col - minCol), y + row, color);
      }
    }
  }
}

void Display::displayText(const char* text1, const char* text2, const char* command, const char* displayType)
{
  bool useBigFont = (strcmp(displayType, "yes") == 0);
  clearBuffer(useBigFont);

  int text1Len = strlen(text1);
  int text2Len = strlen(text2);

  // Calculate width based on the longer text
  int totalWidth = calculateTextWidth((text1Len > text2Len) ? text1 : text2, useBigFont);
  
  if (strcmp(command, "scrolC") == 0)
  {
    // Continuous scrolling implementation
    scrollTextContinuous(text1, text2, totalWidth, useBigFont);
  }
  else if (strcmp(command, "scrolS") == 0)
  {
    // Scroll from right then stop at the left
    scrollTextAndStop(text1, text2, totalWidth, useBigFont);
  }
  else if (strcmp(command, "fadeIn") == 0)
  {
    // Fade in text effect
    fadeInText(text1, text2, useBigFont);
  }
  else if (strcmp(command, "static") == 0)
  {
    // Static display implementation
    displayStaticText(text1, text2, useBigFont);
  }
}

// Helper function to calculate total text width
int Display::calculateTextWidth(const char* text, bool useBigFont)
{
  int totalWidth = 0;
  int textLen = strlen(text);
  
  for (int i = 0; i < textLen; i++)
  {
    totalWidth += useBigFont ? 
      getCharacterWidth15x15(text[i]) : 
      getCharacterWidth7x7(text[i]);
    totalWidth += 1;  // Add spacing between characters
  }
  
  return totalWidth;
}

// Continuous scrolling implementation
void Display::scrollTextContinuous(const char* text1, const char* text2, int totalWidth, bool useBigFont)
{
  int scrollSpeed = 100;  // Milliseconds between shifts
  int text1Len = strlen(text1);
  int text2Len = strlen(text2);
  int longerTextLen = (text1Len > text2Len) ? text1Len : text2Len;
  int shift = 0;

  scrollInterrupt = false;
  
  // We'll need to make a copy of the text to ensure it remains valid
  // even if the original gets overwritten by a new command
  char* text1Copy = new char[text1Len + 1];
  char* text2Copy = new char[text2Len + 1];
  strcpy(text1Copy, text1);
  strcpy(text2Copy, text2);
  
  unsigned long previousMillis = 0;
  unsigned long currentMillis;
  
  // Create a separate task or thread for scrolling
  // This will run until interrupted
  while (!scrollInterrupt)
  {
    currentMillis = millis();
    
    // Check if it's time to update the scroll position
    if (currentMillis - previousMillis >= scrollSpeed)
    {
      previousMillis = currentMillis;
      
      // Clear the buffer for the new frame
      clearBuffer(useBigFont);
      
      // Calculate the x position with wrapping for infinite scroll
      int effectiveShift = shift % (totalWidth + NUMPIXELS);
      int currentX = NUMPIXELS - effectiveShift;
      
      // If we're approaching the end of text, start drawing a copy at the beginning
      for (int i = 0; i < longerTextLen; i++)
      {
        int charWidth = useBigFont ? 
          getCharacterWidth15x15(text1Copy[i]) : 
          getCharacterWidth7x7((text1Len > text2Len) ? text1Copy[i] : text2Copy[i]);
          
        if (currentX >= -charWidth && currentX < NUMPIXELS)
        {
          if (useBigFont)
          {
            drawCharacter15x15(text1Copy[i], currentX, 1, currentFullColourHex);
          }
          else
          {
            if (i < text1Len)
              drawCharacter7x7(text1Copy[i], currentX, 0, currentTopColourHex);
            if (i < text2Len)
              drawCharacter7x7(text2Copy[i], currentX, 8, currentBottomColourHex);
          }
        }
        
        // Also draw the character again after a full text width to create seamless loop
        if (currentX + totalWidth + NUMPIXELS >= 0 && currentX + totalWidth + NUMPIXELS < NUMPIXELS * 2)
        {
          if (useBigFont)
          {
            drawCharacter15x15(text1Copy[i], currentX + totalWidth + NUMPIXELS, 1, currentFullColourHex);
          }
          else
          {
            if (i < text1Len)
              drawCharacter7x7(text1Copy[i], currentX + totalWidth + NUMPIXELS, 0, currentTopColourHex);
            if (i < text2Len)
              drawCharacter7x7(text2Copy[i], currentX + totalWidth + NUMPIXELS, 8, currentBottomColourHex);
          }
        }
        
        currentX += charWidth + 1;
      }
      
      // Update the display
      updateLEDs();
      
      // Increment the shift for the next frame
      shift++;
      
      // Check for data available on Serial port or other interrupt condition
      if (Serial.available() > 0)
      {
        clearBuffer(useBigFont);
        scrollInterrupt = true;
      }
    }
    
    // Small delay to prevent hogging the CPU
    // This also gives other parts of the code a chance to run
    delay(1);
  }
  
  // Clean up the text copy
  delete[] text1Copy;
  delete[] text2Copy;
}

// Scroll and stop implementation
void Display::scrollTextAndStop(const char* text1, const char* text2, int totalWidth, bool useBigFont) {
  int speed = 50;
  int text1Len = strlen(text1);
  int text2Len = strlen(text2);
  int longerTextLen = (text1Len > text2Len) ? text1Len : text2Len;
  int stopPosition = 0; // Stop position (left edge)
  
  // Scroll from right edge to stop position
  for (int shift = NUMPIXELS; shift >= stopPosition; shift--)
  {
    clearBuffer(useBigFont);
    int currentX = shift;
    
    for (int i = 0; i < longerTextLen; i++)
    {
      int charWidth = useBigFont ? 
        getCharacterWidth15x15(text1[i]) : 
        getCharacterWidth7x7((text1Len > text2Len) ? text1[i] : text2[i]);
        
      if (currentX >= -charWidth && currentX < NUMPIXELS)
      {
        if (useBigFont)
        {
          drawCharacter15x15(text1[i], currentX, 1, currentFullColourHex);
        }
        else
        {
          if (i < text1Len)
            drawCharacter7x7(text1[i], currentX, 0, currentTopColourHex);
          if (i < text2Len)
            drawCharacter7x7(text2[i], currentX, 8, currentBottomColourHex);
        }
      }
      currentX += charWidth + 1;
    }
    
    updateLEDs();
    delay(speed);
  }
}

void Display::fadeInText(const char* text1, const char* text2, bool useBigFont)
{
  int steps = 20;  // Number of fade steps
  int delay_ms = 50;  // Delay between steps
  int currentBrightness;
  
  // Store the original brightness value
  int originalBrightness = 0;
  for (int i = 0; i < NUM_STRIPS; i++)
  {
    originalBrightness = strips[i].getBrightness();
    break;  // All strips should have the same brightness
  }
  
  // Draw the text at full color but with brightness at 0
  setBrightness(0);
  displayStaticText(text1, text2, useBigFont);
  
  // Gradually increase brightness
  for (int step = 1; step <= steps; step++)
  {
    currentBrightness = (originalBrightness * step) / steps;
    setBrightness(currentBrightness);
    updateLEDs();
    delay(delay_ms);
  }
  
  // Restore original brightness
  setBrightness(originalBrightness);
}

// Static text display implementation
void Display::displayStaticText(const char* text1, const char* text2, bool useBigFont)
{
  bool isTimer = false;
  int colonCount = 0;
  for (int i = 0; i < strlen(text1); i++)
  {
    if (text1[i] == ':')
    {
      isTimer = true;
      break;
    } 
    // if (!isdigit(text1[i]) && text1[i] != ':')
    // {
    //     isTimer = false;
    //     break;
    // }
  }

  // if (colonCount == 1 || colonCount == 2)
  //   isTimer = true;
  
  // if (isTimer)
  // {
  //   Serial.println("Is a timer");
  //   int textLen = strlen(text1);
  //   int charBoxWidth = isTimer ? 9 : 0;
  //   int spacing = isTimer ? 2 : 1;
  //   int totalWidth = 0;

  //   for (int i = 0; i < textLen; i++)
  //   {
  //       if (isTimer)
  //           totalWidth += (text1[i] == ':' ? 4 : charBoxWidth) + spacing;
  //       else
  //           totalWidth += getCharacterWidth15x15(text1[i]) + 1;
  //   }

  //   int startX = (NUMPIXELS - totalWidth) / 2;
  //   int currentX = startX;
  //   int startY = 2;

  //   clearBuffer(useBigFont);

  //   for (int i = 0; i < textLen; i++)
  //   {
  //     int specialCharWidth = (text1[i] == ':') ? 4 : charBoxWidth;
  //     int specialSpacing   = (text1[i] == ':') ? 3 : spacing;
  //     drawCharacter15x15(text1[i], currentX, startY, currentFullColourHex);
  //     currentX += specialCharWidth + specialSpacing;
      
  //   }
  // }
  
  if (useBigFont)
  {
    // Big Font Mode (Single Row)
    int textLen = strlen(text1);
    int totalWidth = calculateTextWidth(text1, true);
    
    int startX = (NUMPIXELS - totalWidth) / 2;  // Centering dynamically
    int startY = 2;                             // Big font starts at row 2
    
    int currentX = startX;
    for (int i = 0; i < textLen; i++)
    {
      int charWidth = getCharacterWidth15x15(text1[i]);
      drawCharacter15x15(text1[i], currentX, startY, currentFullColourHex);
      currentX += charWidth + 1;
    }
  }
  else
  {
    // Small Font Mode (Two Rows)
    int topLen = strlen(text1);
    int bottomLen = strlen(text2);
    
    int topWidth = calculateTextWidth(text1, false);
    int bottomWidth = calculateTextWidth(text2, false);
    
    int topX = (NUMPIXELS - topWidth) / 2;
    int bottomX = (NUMPIXELS - bottomWidth) / 2;
    
    // Render Top Row
    int currentX = topX;
    for (int i = 0; i < topLen; i++)
    {
      int charWidth = getCharacterWidth7x7(text1[i]);
      drawCharacter7x7(text1[i], currentX, 0, currentTopColourHex);
      currentX += charWidth + 1;
    }
    
    // Render Bottom Row
    currentX = bottomX;
    for (int i = 0; i < bottomLen; i++)
    {
      int charWidth = getCharacterWidth7x7(text2[i]);
      drawCharacter7x7(text2[i], currentX, 8, currentBottomColourHex);
      currentX += charWidth + 1;
    }
  }
  
  updateLEDs();
}

void Display::setTopColour(const uint32_t colourHex)
{
  currentTopColourHex = colourHex;
}
void Display::setBottomColour(const uint32_t colourHex)
{
  currentBottomColourHex = colourHex;
}

void Display::setFullColour(const uint32_t colourHex)
{
  currentFullColourHex = colourHex;
}

void Display::displayCustomPixels(const char* input, const char* chunkPos)
{
  // Clear the display if this is the first chunk
  if (strcmp(chunkPos, "start") == 0)
  {
    clearBuffer(true);
  }
  
  // Print chunk position for debugging
  Serial.println(chunkPos);
  
  // Find the opening bracket
  const char* start = strchr(input, '[');
  if (!start) return; // Exit if no opening bracket found
  start++; // Move past the opening bracket
  
  // Process each coordinate pair
  char coordBuffer[30]; // Buffer for holding one coordinate set
  int bufferIndex = 0;
  bool inCoordPair = false;
  
  for (const char* p = start; *p && *p != ']'; p++)
  {
    if (*p == '(')
    {
      inCoordPair = true;
      bufferIndex = 0;
    }
    else if (*p == ')')
    {
      inCoordPair = false;
      coordBuffer[bufferIndex] = '\0'; // Null terminate
      
      // Parse the coordinate pair
      char* xStr = coordBuffer;
      char* yStr = NULL;
      char* colorStr = NULL;
      
      // Find first comma
      char* firstComma = strchr(coordBuffer, ',');
      if (firstComma)
      {
        *firstComma = '\0'; // Split string
        yStr = firstComma + 1;
        
        // Find second comma
        char* secondComma = strchr(yStr, ',');
        if (secondComma)
        {
          *secondComma = '\0'; // Split string
          colorStr = secondComma + 1;
        }
      }
      
      // If we have all three components
      if (xStr && yStr && colorStr)
      {
        int x = atoi(xStr);
        int y = atoi(yStr);
        
        // Handle hex color with or without # prefix
        unsigned long color = 0;
        if (strncmp(colorStr, "#", 1) == 0)
        {
          color = strtoul(colorStr + 1, NULL, 16);
        }
        else
        {
          color = strtoul(colorStr, NULL, 16);
        }
        
        // Set the pixel with parsed color (bounds checking recommended)
        if (x >= 0 && y >= 0)
        {
          setPixel(y, x, color);
        }
      }
    }
    else if (inCoordPair && bufferIndex < sizeof(coordBuffer) - 1)
    {
      coordBuffer[bufferIndex++] = *p;
    }
  }
  
  // Update LEDs
  updateLEDs();
}