#ifndef TIMER_H
#define TIMER_H

#include <Arduino.h>
#include <RTClib.h>

extern RTC_DS3231 rtc;  // Reference to global RTC instance
extern DateTime timerEnd;

class Timer
{
private:
    volatile bool isTimerRunning;
    unsigned long previousMillis;
    static Timer* instance;
    unsigned int sec;
    unsigned int minu;
    
public:
    static Timer& getInstance();

    Timer();

    int getMin();

void setMin(int minutes);

int getSec();

void setSec(int seconds);
    
    void set(int minutes, int seconds);
    void start();
    void pause();
    void update();
    bool isRunning() const;
    void countdown(int minutes, int seconds, bool startImmediately);
};



#endif // TIMER_H