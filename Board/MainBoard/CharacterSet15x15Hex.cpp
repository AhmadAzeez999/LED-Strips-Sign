#include "CharacterSet15x15Hex.h"

int getCharIndex15x15(char c)
{
    if (c >= 'A' && c <= 'Z') return c - 'A';        // Uppercase letters (0-25)
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;   // Lowercase letters (26-51)
    if (c >= '0' && c <= '9') return c - '0' + 52;   // Numbers (52-61)

    switch (c)
    {
        case '.': return 62;
        case ',': return 63;
        case '!': return 64;
        case '?': return 65;
        case '@': return 66;
        case '#': return 67;
        case '$': return 68;
        case '&': return 70;
        case '*': return 71;
        case '-': return 72;
        case '+': return 73;
        case ':': return 74;  // Colon
        default: return -1; // Return -1 for unsupported characters
    }
}