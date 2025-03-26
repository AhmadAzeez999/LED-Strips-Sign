#include "Display.h"
#include "Timer.h"
#include <Wire.h>
#include <RTClib.h>
#include "Remote.h"


RTC_DS3231 rtc;  // Global RTC instance
DateTime timerEnd;
bool useBigFont = true;  // Toggle between 7x7 and 15x15 font sizes
Timer timer;
RemoteControl remote; // Global remote variable

// Variables for parsing the received message
String command = "";
String displayType = "";
String message = "";
int messageSize = 0;

// Variable for receiving input directly
int intByte;
const uint8_t numRawChar = 90;
char msgRaw[numRawChar];
uint8_t charCount = 0;
bool dataToSend = false;

void setup()
{
    Serial.begin(9600);
    Display::getInstance().setup(4);
    remote.setupRemote();
}

void loop()
{
  if (Serial.available())
  {
    // Extracting size
    String input = Serial.readStringUntil('\n');  // Read until newline
    parseInput(input);
    
    // Debug print
    Serial.println("Parsed values:");
    Serial.println("Size: " + String(messageSize));
    Serial.println("Command: " + command);
    Serial.println("Display type: " + displayType);
    Serial.println("Message: " + message);

    Serial.println(message.c_str());
   

    Display::getInstance().displayText(message.c_str(), "", "STATIC", "", true);
  }

  remote.useRemote();

  // static bool timerActive = false;  // Track if timer has started

  // Display::getInstance().displayText("Yo", "", "STATIC", "", true);

  // timer.countdown(3, 5, true);
}

void parseInput(String input)
{
  // Check if input starts with $
  if (input.charAt(0) != '$')
  {
    Serial.println("Invalid format: must start with $");
    return;
  }
  
  // Find all $ positions
  int firstDollar = input.indexOf('$');
  int secondDollar = input.indexOf('$', firstDollar + 1);
  
  // Find brackets
  int openBracket = input.indexOf('[');
  // int closeBracket = input.indexOf(']');

  messageSize = input.length();
  
  // Extract command
  command = input.substring(firstDollar + 1, secondDollar);
  
  // Extract display type
  displayType = input.substring(secondDollar + 1, openBracket); 
  
  // Extract message
  message = input.substring(openBracket + 1, input.length() - 1);
}