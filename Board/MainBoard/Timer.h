#ifndef TIMER_H
#define TIMER_H

#include <Arduino.h>
#include <RTClib.h>

extern RTC_DS3231 rtc;  // Reference to global RTC instance


class Timer
{
private:
    DateTime targetTime;
    TimeSpan remainingTime;
    bool timerActive = false;
    bool timerPaused = false;
    unsigned long previousMillis;
    static Timer* instance;
    
public:
    static Timer& getInstance();

    Timer();
    
    void set(int minutes, int seconds);
    void start(int minutes, int seconds);
    void resume();
    void stop();
    void pause();
    void update();
    bool isRunning() const;
    void countdown(int minutes, int seconds, bool startImmediately);
};



#endif // TIMER_H