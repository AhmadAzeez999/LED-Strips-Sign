#include "Timer.h"
#include <Arduino.h> // Required for millis()
#include <Wire.h>
#include "Display.h"

// Constructor
Timer::Timer() : rtc() {
 
}

void Timer::setupRTC()
{
  if (!rtc.begin()) {
        Serial.println("Couldn't find RTC");
        while (1); // freeze the program
    }

    if (rtc.lostPower()) {
        Serial.println("RTC lost power, setting time!");
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // syncs with PC compile time
    }
}

void Timer::startTimer(int minutes, int seconds) {
    remainingTime = TimeSpan(0, 0, minutes, seconds);
    targetTime = rtc.now() + remainingTime;
    timerActive = true;
    timerPaused = false;
}

void Timer::pauseTimer() {
    if (timerActive && !timerPaused) {
        remainingTime = targetTime - rtc.now();
        timerPaused = true;
    }
}

void Timer::resumeTimer() {
    if (timerActive && timerPaused) {
        targetTime = rtc.now() + remainingTime;
        timerPaused = false;
    }
}

void Timer::stopTimer() {
    timerActive = false;
    timerPaused = false;
    Display::getInstance().displayText("STOP", "", "statc", "yes");
}

void Timer::updateTimer() {
    if (!timerActive || timerPaused) return;
    

    if (millis() - lastUpdateMillis >= 1000) {
        DateTime now = rtc.now();
        TimeSpan remaining = targetTime - now;
        Serial.println(remaining.minutes());
        Serial.println(remaining.totalseconds());
        Serial.println(targetTime.secondstime());
        Serial.println(now.timestamp());
        

        if (remaining.totalseconds() <= 0) {
            Display::getInstance().displayText("0:00", "", "statc", "yes");
            timerActive = false;
        } else {
          int mins = max(0, remaining.minutes());
          int secs = max(0, remaining.seconds());
          char text[8];
          sprintf(text, "%d:%02d", mins, secs);
          timeText = text;
        }

        lastUpdateMillis = millis();
    }
}

char* Timer::getTimeText()
{
  return timeText;
}
