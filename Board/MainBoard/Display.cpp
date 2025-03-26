#include "Display.h"

// Initialize static instance to nullptr
Display* Display::instance = nullptr;

uint32_t currentColourHex = 0xFF0000;

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
void Display::updateLEDs(bool bigFont)
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

void Display::displayText(const char* text1, const char* text2, const char* mode, const char* position, bool useBigFont) {
  uint32_t color = currentColourHex;  // Default text color
  clearBuffer(useBigFont);

  if (strcmp(mode, "SCROLL") == 0)
  {
    int speed = 50;
    int textLen = strlen(text1);
    int totalWidth = 0;

    // Calculate total text width dynamically
    for (int i = 0; i < textLen; i++)
    {
      totalWidth += useBigFont ? getCharacterWidth15x15(text1[i]) : getCharacterWidth7x7(text1[i]);
      totalWidth += 1;  // Add small spacing between characters
    }

    for (int shift = -NUMPIXELS; shift < totalWidth; shift++)
    {
      clearBuffer(useBigFont);

      int currentX = -shift;
      for (int i = 0; i < textLen; i++)
      {
        int charWidth = useBigFont ? getCharacterWidth15x15(text1[i]) : getCharacterWidth7x7(text1[i]);
        if (currentX >= -charWidth && currentX < NUMPIXELS)
          useBigFont ? drawCharacter15x15(text1[i], currentX, 1, color)
                     : drawCharacter7x7(text1[i], currentX, 0, color);
        currentX += charWidth + 1;
      }

      updateLEDs(useBigFont);
      delay(speed);
    }
  }
  else if (strcmp(mode, "STATIC") == 0)
  {
    if (useBigFont)
    {
      // Big Font Mode (Single Row)
      int textLen = strlen(text1);
      int totalWidth = 0;

      // Calculate total text width dynamically
      for (int i = 0; i < textLen; i++)
      {
        totalWidth += getCharacterWidth15x15(text1[i]) + 1;
      }

      int startX = (NUMPIXELS - totalWidth) / 2;  // Centering dynamically
      int startY = 2;                             // Big font starts at row 2

      int currentX = startX;
      for (int i = 0; i < textLen; i++)
      {
        int charWidth = getCharacterWidth15x15(text1[i]);
        drawCharacter15x15(text1[i], currentX, startY, color);
        currentX += charWidth + 1;
      }
    }
    else
    {
      // Small Font Mode (Two Rows)
      int topLen = strlen(text1);
      int bottomLen = strlen(text2);
      int topWidth = 0, bottomWidth = 0;

      // Calculate width dynamically for both rows
      for (int i = 0; i < topLen; i++)
      {
        topWidth += getCharacterWidth7x7(text1[i]) + 1;
      }

      for (int i = 0; i < bottomLen; i++)
      {
        bottomWidth += getCharacterWidth7x7(text2[i]) + 1;
      }

      int topX = (NUMPIXELS - topWidth) / 2;
      int bottomX = (NUMPIXELS - bottomWidth) / 2;

      // Render Top Row (Upper 7 Strips)
      int currentX = topX;
      for (int i = 0; i < topLen; i++)
      {
        int charWidth = getCharacterWidth7x7(text1[i]);
        drawCharacter7x7(text1[i], currentX, 0, color);  // Y = 0 for top row
        currentX += charWidth + 1;
      }

      // Render Bottom Row (Lower 7 Strips)
      currentX = bottomX;
      for (int i = 0; i < bottomLen; i++)
      {
        int charWidth = getCharacterWidth7x7(text2[i]);
        drawCharacter7x7(text2[i], currentX, 8, color);  // Y = 8 for bottom row
        currentX += charWidth + 1;
      }
    }

    updateLEDs(useBigFont);
  }
}

void Display::changeColour(const int pattern)
{
  if (pattern == 0)
  {
    // Display::getInstance().displayText("Green", "Purple", "STATIC", "", false);
    currentColourHex = 0x00FF00;
  }
  else if (pattern == 1)
  {
    // Display::getInstance().displayText("Purple", "Green", "STATIC", "", false);
    currentColourHex = 0x800080; // Purple
  }
  else if (pattern == 2)
  {
    // Display::getInstance().displayText("Blue", "Purple", "STATIC", "", false);
    currentColourHex = 0x0000FF; // Blue
  }
  else if (pattern == 3)
  {
    // Display::getInstance().displayText("Green", "Blue", "STATIC", "", false);
    currentColourHex = 0xFF0000; // Red
  }
}