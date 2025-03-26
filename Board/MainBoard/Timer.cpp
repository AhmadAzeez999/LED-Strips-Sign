#include "Timer.h"
#include "Display.h"
#include <RTClib.h>

Timer* Timer::instance = nullptr;


// Singleton accessor
Timer& Timer::getInstance()
{
  if (instance == nullptr)
  {
    instance = new Timer();
  }

  return *instance;
}

Timer::Timer() :previousMillis(0), timerActive(false), timerPaused(false), targetTime(""), remainingTime(0)
{
}

void Timer::set(int minutes, int seconds)
{
    timerActive = false;

    char timeString[6];
    sprintf(timeString, "%02d+:%02d", minutes, seconds);
    Display::getInstance().displayText(timeString, "", "statc", "f");
    Serial.print("Timer Set: ");
    Serial.println(timeString);
}

void Timer::start(int minutes, int seconds) {
    remainingTime = TimeSpan(0, 0, minutes, seconds);
    targetTime = rtc.now() + remainingTime;
    timerActive = true;
    timerPaused = false;
}

void Timer::pause() {
    if (timerActive && !timerPaused) {
        remainingTime = targetTime - rtc.now();
        timerPaused = true;
    }
}

void Timer::resume() {
    if (timerActive && timerPaused) {
        targetTime = rtc.now() + remainingTime;
        timerPaused = false;
    }
}

void Timer::stop() {
    timerActive = false;
    timerPaused = false;
    Display::getInstance().displayText("STOP", "", "STATIC", "CENTER");
}


void Timer::update() {
    if (!timerActive || timerPaused) return;

    if (millis() - previousMillis >= 1000) {
        DateTime now = rtc.now();
        TimeSpan remaining = targetTime - now;
        Serial.println(targetTime.timestamp());
        Serial.println(now.timestamp());
        Serial.println(remainingTime.totalseconds());

        if (remaining.totalseconds() <= 0) {
            Display::getInstance().displayText("0:00", "", "statc", "f");
            timerActive = false;
        } else {
            char text[6];
            sprintf(text, "%d:%02d", remaining.minutes(), remaining.seconds());
            Display::getInstance().displayText(text, "", "statc", "f");
        }

        previousMillis = millis();
    }
}