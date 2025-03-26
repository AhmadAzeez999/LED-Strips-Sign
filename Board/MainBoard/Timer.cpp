#include "Timer.h"
#include "Display.h"

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

Timer::Timer() : isTimerRunning(false), previousMillis(0), sec(0), minu(0)
{
}



int Timer::getMin()
{
  return minu;
}

void Timer::setMin(int minutes)
{
  Serial.print("Before Assignment: ");
  Serial.println(minu);
  minu = minutes;
  Serial.print("After Assignment: ");
  Serial.println(minu);
}

int Timer::getSec()
{
  return sec;
}

void Timer::setSec(int seconds)
{
  sec = seconds;
}

void Timer::set(int minutes, int seconds)
{
    sec = minutes * 60;
    
    isTimerRunning = false;

    //DateTime now = rtc.now();
    //timerEnd = now + TimeSpan(0, 0, minutes, seconds);

    Serial.println(minu);

    char timeString[6];
    sprintf(timeString, "%02d+:%02d", minutes, seconds);
    Display::getInstance().displayText(timeString, "", "STATIC", "CENTER");
    setMin(minutes);
    Serial.print("Timer Set: ");
    Serial.println(timeString);
    Serial.println(minu);
}

void Timer::start()
{
    isTimerRunning = true;
    Serial.println("Timer Started!");
    Serial.println(minu);
}

void Timer::pause()
{
    isTimerRunning = false;
    Serial.println("Timer Paused!");
}

bool Timer::isRunning() const
{
    return isTimerRunning;
}

void Timer::update()
{
   if (!isTimerRunning) return;
   
   unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= 1000)
    {
      previousMillis = currentMillis;

      unsigned int remainingSec = (sec - 1) % 60;
      unsigned int remainingMin = minu - 1;


      sec--;
      
      if(remainingSec == 0)
      {
        remainingMin--;
        minu--;
      }

      char timeString[6];
      sprintf(timeString, "%02d+:%02d", remainingMin,remainingSec);
      Display::getInstance().displayText(timeString, "", "statc", "f");

      Serial.println(remainingMin);
      Serial.println(remainingSec);
      if(remainingSec == 0 && remainingMin == 0)
      {
        isTimerRunning = false;
      }
    }
    //Serial.println(currentMillis);

   
}

void Timer::countdown(int minutes, int seconds, bool startImmediately)
{
    set(minutes, seconds);

    if (startImmediately)
    {
        start();
    }
}