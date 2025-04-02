#include "Display.h"

// Static instance initialization
Display* Display::instance = nullptr;

// Singleton accessor
Display& Display::getInstance() {
    if (!instance)
        instance = new Display();
    return *instance;
}

// Private constructor
Display::Display() {
    for (int i = 0; i < NUM_STRIPS; i++) {
        strips[i] = Adafruit_NeoPixel(NUMPIXELS, stripPins[i], NEO_GRB + NEO_KHZ800);
    }
    clearBuffer(true);
}

void Display::setup(int brightness) {
    for (int i = 0; i < NUM_STRIPS; i++) {
        strips[i].begin();
        strips[i].setBrightness(brightness);
        strips[i].show();
    }
}

void Display::clearBuffer(bool bigFont) {
    for (int i = 0; i < NUM_STRIPS; i++)
        for (int j = 0; j < NUMPIXELS; j++)
            frameBuffer[i][j] = 0;
}

void Display::updateLEDs() {
    for (int i = 0; i < NUM_STRIPS; i++) {
        for (int j = 0; j < NUMPIXELS; j++) {
            strips[i].setPixelColor(j, frameBuffer[i][j]);
        }
        strips[i].show();
    }
}

void Display::setPixel(int x, int y, uint32_t color) {
    if (x < 0 || x >= NUMPIXELS || y < 0 || y >= NUM_STRIPS) return;
    frameBuffer[y][x] = color;
}

int Display::getCharacterWidth7x7(char c) {
    if (c == ' ') return 1;
    int index = getCharIndex(c);
    if (index == -1) return 0;

    int minCol = 7, maxCol = 0;
    for (int row = 0; row < 7; row++) {
        for (int col = 0; col < 7; col++) {
            if (pgm_read_byte(&(charSet7x7[index][row][col]))) {
                if (col < minCol) minCol = col;
                if (col > maxCol) maxCol = col;
            }
        }
    }
    return (maxCol >= minCol) ? (maxCol - minCol + 1) : 1;
}

int Display::getCharacterWidth15x15(char c) {
    if (c == ' ') return 1;
    int index = getCharIndex15x15(c);
    if (index == -1) return 0;

    int minCol = 15, maxCol = 0;
    for (int row = 0; row < 13; row++) {
        uint16_t rowData = pgm_read_word(&(charSet15x15Hex[index][row]));
        for (int col = 0; col < 15; col++) {
            if (rowData & (1 << (14 - col))) {
                if (col < minCol) minCol = col;
                if (col > maxCol) maxCol = col;
            }
        }
    }
    return (maxCol >= minCol) ? (maxCol - minCol + 1) : 1;
}

void Display::drawCharacter7x7(char c, int x, int y, uint32_t color) {
    int index = getCharIndex(c);
    if (index == -1) return;

    int minCol = 7, maxCol = 0;
    for (int row = 0; row < 7; row++) {
        for (int col = 0; col < 7; col++) {
            if (pgm_read_byte(&(charSet7x7[index][row][col]))) {
                if (col < minCol) minCol = col;
                if (col > maxCol) maxCol = col;
            }
        }
    }

    for (int row = 0; row < 7; row++) {
        for (int col = minCol; col <= maxCol; col++) {
            if (pgm_read_byte(&(charSet7x7[index][row][col]))) {
                setPixel(x + (col - minCol), y + row, color);
            }
        }
    }
}

void Display::drawCharacter15x15(char c, int x, int y, uint32_t color) {
    int index = getCharIndex15x15(c);
    if (index == -1) return;

    int minCol = 15, maxCol = 0;
    for (int row = 0; row < 13; row++) {
        uint16_t rowData = pgm_read_word(&(charSet15x15Hex[index][row]));
        for (int col = 0; col < 15; col++) {
            if (rowData & (1 << (14 - col))) {
                if (col < minCol) minCol = col;
                if (col > maxCol) maxCol = col;
            }
        }
    }

    for (int row = 0; row < 13; row++) {
        uint16_t rowData = pgm_read_word(&(charSet15x15Hex[index][row]));
        for (int col = minCol; col <= maxCol; col++) {
            if ((rowData >> (14 - col)) & 1) {
                setPixel(x + (col - minCol), y + row, color);
            }
        }
    }
}

int Display::calculateTextWidth(const char* text, bool useBigFont) {
    int width = 0;
    for (int i = 0; text[i] != '\0'; i++) {
        if (useBigFont)
            width += getCharacterWidth15x15(text[i]);
        else
            width += getCharacterWidth7x7(text[i]);
        width += 1;  // spacing between characters
    }
    return width;
}

void Display::scrollTextContinuous(const char* text1, const char* text2, int totalWidth, bool useBigFont) {
    int speed = 50;
    for (int shift = -NUMPIXELS; shift < totalWidth; shift++) {
        clearBuffer(useBigFont);
        int currentX = -shift;
        for (int i = 0; text1[i] != '\0'; i++) {
            int charWidth = useBigFont ? getCharacterWidth15x15(text1[i]) : getCharacterWidth7x7(text1[i]);
            if (currentX >= -charWidth && currentX < NUMPIXELS) {
                if (useBigFont)
                    drawCharacter15x15(text1[i], currentX, 1, currentFullColourHex);
                else
                    drawCharacter7x7(text1[i], currentX, 0, currentFullColourHex);
            }
            currentX += charWidth + 1;
        }
        updateLEDs();
        delay(speed);
    }
}

void Display::scrollTextAndStop(const char* text1, const char* text2, int totalWidth, bool useBigFont) {
    int speed = 50;
    int startX = NUMPIXELS;
    int endX = 0;

    for (int xOffset = startX; xOffset >= endX; xOffset--) {
        clearBuffer(useBigFont);
        int currentX = xOffset;
        for (int i = 0; text1[i] != '\0'; i++) {
            int charWidth = useBigFont ? getCharacterWidth15x15(text1[i]) : getCharacterWidth7x7(text1[i]);
            if (currentX + charWidth >= 0 && currentX < NUMPIXELS) {
                if (useBigFont)
                    drawCharacter15x15(text1[i], currentX, 2, currentFullColourHex);
                else
                    drawCharacter7x7(text1[i], currentX, 0, currentFullColourHex);
            }
            currentX += charWidth + 1;
        }
        updateLEDs();
        delay(speed);
    }
}
void Display::fadeInText(const char* text1, const char* text2, bool useBigFont) {
    int textLen = strlen(text1);
    int totalWidth = calculateTextWidth(text1, useBigFont);
    int startX = (NUMPIXELS - totalWidth) / 2;
    int startY = useBigFont ? 2 : 0;

    for (int brightness = 0; brightness <= 255; brightness += 20) {
        for (int i = 0; i < NUM_STRIPS; i++) {
            strips[i].setBrightness(brightness);
        }

        clearBuffer(useBigFont);
        int currentX = startX;

        for (int i = 0; i < textLen; i++) {
            int charWidth = useBigFont ? getCharacterWidth15x15(text1[i]) : getCharacterWidth7x7(text1[i]);

            if (useBigFont)
                drawCharacter15x15(text1[i], currentX, startY, currentFullColourHex);
            else
                drawCharacter7x7(text1[i], currentX, startY, currentFullColourHex);

            currentX += charWidth + 1;
        }

        updateLEDs();
        delay(80);
    }

    // Set final brightness
    for (int i = 0; i < NUM_STRIPS; i++) {
        strips[i].setBrightness(255);
    }

    clearBuffer(useBigFont);
    int currentX = startX;
    for (int i = 0; i < textLen; i++) {
        int charWidth = useBigFont ? getCharacterWidth15x15(text1[i]) : getCharacterWidth7x7(text1[i]);

        if (useBigFont)
            drawCharacter15x15(text1[i], currentX, startY, currentFullColourHex);
        else
            drawCharacter7x7(text1[i], currentX, startY, currentFullColourHex);

        currentX += charWidth + 1;
    }

    updateLEDs();
}
void Display::displayStaticText(const char* text1, const char* text2, bool useBigFont) {
    bool isTimer = false;
    int colonCount = 0;
    for (int i = 0; i < strlen(text1); i++) {
        if (text1[i] == ':') colonCount++;
        if (!isdigit(text1[i]) && text1[i] != ':') {
            isTimer = false;
            break;
        }
    }
    if (colonCount == 1 || colonCount == 2) isTimer = true;

    int textLen = strlen(text1);
    int charBoxWidth = isTimer ? 9 : 0;
    int spacing = isTimer ? 2 : 1;
    int totalWidth = 0;

    for (int i = 0; i < textLen; i++) {
        if (isTimer)
            totalWidth += (text1[i] == ':' ? 4 : charBoxWidth) + spacing;
        else
            totalWidth += getCharacterWidth15x15(text1[i]) + 1;
    }

    int startX = (NUMPIXELS - totalWidth) / 2;
    int currentX = startX;
    int startY = 2;

    clearBuffer(useBigFont);

    for (int i = 0; i < textLen; i++) {
        if (isTimer) {
            int specialCharWidth = (text1[i] == ':') ? 4 : charBoxWidth;
            int specialSpacing   = (text1[i] == ':') ? 3 : spacing;
            drawCharacter15x15(text1[i], currentX, startY, currentFullColourHex);
            currentX += specialCharWidth + specialSpacing;
        } else {
            int charWidth = getCharacterWidth15x15(text1[i]);
            drawCharacter15x15(text1[i], currentX, startY, currentFullColourHex);
            currentX += charWidth + 1;
        }
    }

    updateLEDs();
}
void Display::displayCustomPixels(String input, String chunkPos) {
    if (chunkPos == "start") {
        clearBuffer(true);
    }

    char buf[input.length() + 1];
    input.toCharArray(buf, sizeof(buf));

    char* start = strchr(buf, '[');
    char* end = strchr(buf, ']');
    if (!start || !end || end <= start) return;

    start++;

    while (start < end) {
        if (*start != '(') {
            start++;
            continue;
        }
        start++;

        int x = atoi(start);
        while (*start && *start != ',') start++;
        if (*start == ',') start++;

        int y = atoi(start);
        while (*start && *start != ',') start++;
        if (*start == ',') start++;

        char colorBuf[10] = {0};
        int colorIndex = 0;
        while (*start && *start != ')') {
            if (colorIndex < 9) {
                colorBuf[colorIndex++] = *start;
            }
            start++;
        }

        if (*start == ')') start++;

        if (strncmp(colorBuf, "0x", 2) != 0) {
            char temp[10] = "0x";
            strcat(temp, colorBuf);
            strcpy(colorBuf, temp);
        }

        uint32_t color = strtoul(colorBuf, NULL, 16);
        setPixel(y, x, color);
    }

    updateLEDs();
}
void Display::setTopColour(const uint32_t colourHex) {
    currentTopColourHex = colourHex;
}

void Display::setBottomColour(const uint32_t colourHex) {
    currentBottomColourHex = colourHex;
}

void Display::setFullColour(const uint32_t colourHex) {
    currentFullColourHex = colourHex;
}

