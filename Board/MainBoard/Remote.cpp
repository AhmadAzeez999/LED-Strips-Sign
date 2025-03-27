#include "Display.h"
#include <IRremote.h>
#include "Remote.h"
#include "Timer.h"

Timer timers;


#define IR_PIN 10

    RemoteControl::RemoteControl() : bright(4), remoteStatus(false), enteredValue(0), inputState(false), minu(0) {}

    void RemoteControl::setupRemote() {
        timers.setupRTC();
        IrReceiver.begin(IR_PIN, ENABLE_LED_FEEDBACK);
    }

    void RemoteControl::displayDefaultMessage() {
        Display::getInstance().clearBuffer(true);
        Display::getInstance().displayText("Athletics ", "","static","no");
        Display::getInstance().updateLEDs(false);
    }

    void RemoteControl::adjustBrightness(int change) {
        if ((change > 0 && bright < 10) || (change < 0 && bright > 1)) {
            bright += change;
            Display::getInstance().clearBuffer(true);
            if(bright == 10){
              Display::getInstance().displayText("BrMax", "", "static", "yes");
            }
            else if(bright == 1)
            {
              Display::getInstance().displayText("BrMin", "", "static", "yes");
            }
            else
            {
              Display::getInstance().displayText(change > 0 ? "BrUp" : "BrDwn", "", "static", "yes");
            }
            
            Display::getInstance().updateLEDs(true);
            for (int x = 0; x < NUM_STRIPS; x++) {
                Display::getInstance().setBrightness(bright);
            }
        }
    }

    void RemoteControl::toggleRemote(uint32_t remoteCode) {
        if (remoteCode == 0xFD020707) {
            remoteStatus = !remoteStatus;
            Display::getInstance().clearBuffer(true);
            Serial.println(remoteStatus ? "RM ON" : "RM OFF");
            Display::getInstance().displayText(remoteStatus ? "RMON" : "RMOFF", "", "static", "yes");
            Display::getInstance().updateLEDs(true);
            delay(500);
        }
    }

    void RemoteControl::handleTimerCodes(uint32_t remoteCode) {
      char text[8];
      if(timers.getTimerRunning() == false)
      {
        for (uint8_t i = 0; i < 10; i++) {
            if (remoteCode == timerCodes[i]) {
                sprintf(text, "%d+:%02d", i+1, 0);
                Display::getInstance().displayText(text, "", "static", "yes");
                minu = i+1;
                break;
            }
        }
      }
      if (remoteCode == 0x97680707) {
          Serial.println(minu);
                    timers.startTimer(minu, 0);
                }
    }

    void RemoteControl::setDefaultMessage(uint32_t remoteCode)
    {
      if(remoteCode == 0x86790707)
      {
        displayDefaultMessage();
      }
    }

    void RemoteControl::useRemote() {
      timers.updateTimer();
      
        if (IrReceiver.decode()) {
            uint32_t remoteCode = IrReceiver.decodedIRData.decodedRawData;
            Serial.print("Raw Hex Code: ");
            Serial.println(remoteCode, HEX);

            toggleRemote(remoteCode);
            if (remoteStatus) {
              if(inputState == false)
              {
                setDefaultMessage(remoteCode);
              
                handleTimerCodes(remoteCode);
                if (remoteCode == 0xF8070707) adjustBrightness(1);
                else if (remoteCode == 0xF40B0707) adjustBrightness(-1);
              }
                /*
                if(remoteCode == 0xB8470707)
                {
                  inputState = !inputState;
                  Serial.println(inputState);
                  if(inputState)
                  {
                    manualTimerInput(remoteCode);
                  }
                  else
                  {
                    int minutes = enteredValue / 100;  // First two digits = minutes
                    int seconds = enteredValue % 100;
                    timers.set(minutes, seconds);
                    enteredValue = 0;
                  }
                  
                }
                */
                
                /*
                else if (remoteCode == 0xFFE21D) {  // Play/Pause button
                  if (timers.isRunning()) {
                      timers.pause();
                      Serial.println("Timer Paused.");
                  } else {
                      timers.start();
                      Serial.println("Timer Resumed.");
                  }
                }
                else if (remoteCode == 0xFF22DD) {  // Stop button → Reset timer
                  timers.set(0, 0);
                  Serial.println("Timer Stopped.");
                  Display::getInstance().displayText("00:00", "", "STATIC", "CENTER", true);
                }
                */
   
            }

            //delay(500);
            IrReceiver.resume();
        }
        
    }

    uint8_t RemoteControl::getBrightness()
    {
      return bright;
    }


// void RemoteControl::manualTimerInput(uint32_t remoteCode)
// { 
//       //Serial.println(remoteCode);
//       int digit = getNumberFromIR(remoteCode);
//       Serial.println(digit);
//       if(enteredValue < 10000)
//       {
//         if(digit != -1)
//         {
//           enteredValue = (enteredValue * 10) + digit;
//           updateDisplay(enteredValue);
//           //Serial.println(enteredValue);
//           //Serial.println(digit);
//           //digit = -1;
//         }
//         //Serial.println(digit);
//       }
// }

// int RemoteControl::getNumberFromIR(uint32_t command) {
//     for (uint8_t i = 0; i < 10; i++) {
//         if (command == timerCodes[i]) {
//             Serial.print("Detected Number: ");
//             Serial.println(i);  // Print the detected number
//             return i;  // Return the actual number (0-9)
//         }
//     }
// }

void RemoteControl::updateDisplay(int time) {
    int minutes = time / 100;
    int seconds = time % 100;

    // Ensure proper MM:SS format
    if (seconds >= 60) {
        seconds = seconds % 60;
        minutes = minutes + 1;
    }

    char timeString[6];
    sprintf(timeString, "%02d:%02d", minutes, seconds);
    Display::getInstance().displayText(timeString, "", "static", "yes");
}




