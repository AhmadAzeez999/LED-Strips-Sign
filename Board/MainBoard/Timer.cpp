#include "Timer.h"
#include <Arduino.h> // Required for millis()
#include <Wire.h>
#include "Display.h"

// Constructor
Timer::Timer() : rtc() {}

void Timer::setupRTC()
{
  if (!rtc.begin())
  {
    Serial.println("Couldn't find RTC");
    while (1); // freeze the program
  }

  if (rtc.lostPower())
  {
    Serial.println("RTC lost power, setting time!");
     // syncs with PC compile time
  }
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); 
}

void Timer::startTimer(int minutes, int seconds)
{
  currentMin = minutes;
  currentSec = seconds;

  remainingTime = TimeSpan(0, 0, minutes, seconds);
  targetTime = rtc.now() + remainingTime;
  timerActive = true;
  timerPaused = false;
  Serial.println(rtc.now().timestamp());
}

void Timer::pauseTimer()
{
  if (timerActive && !timerPaused)
  {
    remainingTime = targetTime - rtc.now();
    timerPaused = true;
  }
}

void Timer::resumeTimer()
{
  if (timerActive && timerPaused)
  {
    targetTime = rtc.now() + remainingTime;
    timerPaused = false;
  }
}

void Timer::resetTimer()
{
  if (timerActive)
  {
    remainingTime = TimeSpan(0, 0, currentMin, currentSec);
    targetTime = rtc.now() + remainingTime;
    timerActive = true;
    timerPaused = false;
  }
}

void Timer::stopTimer()
{
  timerActive = false;
  timerPaused = false;
  Display::getInstance().displayText("STOP", "", "static", "yes");
}

void Timer::timeOfDays()
{
  if (!timeOfDay) return;
  
  if(millis() - lastUpdateMillis >= 1000)
  {
    DateTime now = rtc.now().unixtime();
    
    int mins = now.hour();
    int secs = now.minute();
    Serial.println(now.timestamp());
    char text[8];
    sprintf(text, "%d+:%02d", mins, secs);
    Display::getInstance().displayText(text, "", "static", "yes");
    lastUpdateMillis = millis();
  }
}

void Timer::displayTimeOfDay(bool tod)
{
  timeOfDay = tod;
}

void Timer::updateTimer()
{
  if (!timerActive || timerPaused) return;
  
  if (millis() - lastUpdateMillis >= 1000)
  {
    DateTime now = rtc.now();
    TimeSpan remaining = targetTime - now;
    Serial.println(remaining.minutes());
    Serial.println(remaining.totalseconds());
    Serial.println(targetTime.secondstime());
    Serial.println(now.timestamp());

    if (remaining.totalseconds() <= 0)
    {
      Display::getInstance().displayText("0+:00", "", "static", "yes");
      timerActive = false;
    }
    else
    {
      int mins = max(0, remaining.minutes());
      int secs = max(0, remaining.seconds());
      char text[8];
      sprintf(text, "%d+:%02d", mins, secs);
      Display::getInstance().displayText(text, "", "static", "yes");
    }

    lastUpdateMillis = millis();
  }
}

bool Timer::getTimerRunning()
{
  return timerActive;
}

bool Timer::getTimerPaused()
{
  return timerPaused;
}

void Timer::parseTimerInput(String input)
{
  // Find brackets
  int openBracket = input.indexOf('[');
  int closeBracket = input.indexOf(']');
  
  // Check if brackets are valid
  if (openBracket == -1 || closeBracket == -1 || openBracket >= closeBracket)
  {
    // Invalid input format
    Serial.println("Error: Invalid time input format");
    return;
  }
  
  // Find the comma separating minutes and seconds
  int commaIndex = input.indexOf(":", openBracket);
  
  // Check if comma is valid
  if (commaIndex == -1 || commaIndex >= closeBracket)
  {
    Serial.println("Error: Invalid time input format");
    return;
  }
  
  String minStr = input.substring(openBracket + 1, commaIndex);
  String secStr = input.substring(commaIndex + 1, closeBracket);
  
  int minutes = minStr.toInt();
  int seconds = secStr.toInt();
  
  // Start timer with parsed minutes and seconds
  startTimer(minutes, seconds);
}
