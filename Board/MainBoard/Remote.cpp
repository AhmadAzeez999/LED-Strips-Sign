#include "Display.h"
#include <IRremote.h>
#include "Remote.h"
#include "Timer.h"

Timer timers;


#define IR_PIN 10

    RemoteControl::RemoteControl() : bright(4), remoteStatus(false), enteredValue(0), inputState(false), minu(0), fIndex(0), tbIndex(0), timerInputMode(false) {}

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
      if (remoteCode == 0x97680707 && timers.getTimerRunning() == false) {
          Serial.println(minu);
                    timers.startTimer(minu, 0);
                }
                else if(remoteCode == 0xB9460707 && timers.getTimerRunning() == true)
                {
                  timers.stopTimer();
                  minu = 0;
                }
                else if(remoteCode == 0xB8470707 && timers.getTimerPaused() == true)
                {
                  timers.resumeTimer();
                }
                else if(remoteCode == 0xB54A0707 && timers.getTimerPaused() == false)
                {
                  timers.pauseTimer();
                }
                else if (remoteCode == 0x97680707 && timers.getTimerRunning() == true) {
                    timers.resetTimer(minu, 0);
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

                if(remoteCode == 0x9F600707)
                {
                  tbIndex = (tbIndex + 1) %6;
                  changeTBColourScheme();
                  
                }
                else if(remoteCode == 0x9E610707)
                {
                  tbIndex = tbIndex - 1;
                  changeTBColourScheme();
                }
                
                else if(remoteCode == 0x9A650707)
                {
                  fIndex = fIndex - 1;
                  changeFColourScheme();
                  Serial.println(fIndex);
                }
                else if(remoteCode == 0x9D620707)
                {
                  fIndex = fIndex + 1;
                  changeFColourScheme();
                }

                if(tbIndex > 5)
                {
                  tbIndex = 0;
                  changeTBColourScheme();
                } 
                else if(tbIndex < 0)
                {
                  tbIndex = 5;
                  changeTBColourScheme();
                } 

                if(fIndex > 2) 
                {
                  fIndex = 0;
                  changeFColourScheme();
                }
                else if(fIndex < 0) 
                {
                  fIndex = 2;
                  changeFColourScheme();
                }
                if (remoteCode == 0xF8070707) adjustBrightness(1);
                else if (remoteCode == 0xF40B0707) adjustBrightness(-1);

                if(remoteCode == 0x946B0707)
                {
                  Display::getInstance().displayText("00+:00", "", "static", "yes");
                  timerInputMode = true;
                  
                  manualTimerInput(); // while loop
                  delay(500);
                }
              } 
            }

            //delay(500);
            IrReceiver.resume();
        }
        
    }

    uint8_t RemoteControl::getBrightness()
    {
      return bright;
    }


void RemoteControl::manualTimerInput() {
  char num[11] = ""; // Supports up to 10 digits + null terminator
  String inputToSend = "";
  while(timerInputMode) {
    IrReceiver.resume();
    if(IrReceiver.decode())
    {
      uint32_t remoteCode = IrReceiver.decodedIRData.decodedRawData;
      Serial.println("Decoded Raw Data: ");
      Serial.println(remoteCode, HEX);

      // Check for exit code
      if (remoteCode == 0x946B0707) {
        Serial.println("Exit Command Received.");
        timerInputMode = false;
        timers.parseTimerInput(inputToSend);
      }

      // Convert IR code to a digit
      int digit = getNumberFromIR(remoteCode);
      
      if (digit == -1) {
        Serial.println("Invalid IR Code. Ignored.");
      } else {
        size_t len = strlen(num);
        if (len < 4) {
          char digitChar = '0' + digit; // Convert to char
          num[len] = digitChar;
          num[len + 1] = '\0';
          Serial.print("Current Input: ");
          Serial.println(num);
          if(len < 2)
          {
            Display::getInstance().clearBuffer(true);
            char text[5];
            sprintf(text, "%s+:00", num);
            Serial.println(num[0]);
            Serial.println(text);
            Display::getInstance().displayText(text, "", "static", "yes");
            inputToSend = "[" + String(text) + "]";
          }
          else if(len == 2)
          {
            Serial.println(num[2]);
            char num1[3] = {num[0], num[1], '\0'};
            char num2[2] = {num[2], '\0'};
            Display::getInstance().clearBuffer(true);
            char text[5];
            sprintf(text, "%s+:0%s", num1, num2);
            Serial.println(text);
            Display::getInstance().displayText(text, "", "static", "yes");
            inputToSend = "[" + String(text) + "]";
          }
          else if(len == 3)
          {
            char num1[3] = {num[0], num[1], '\0'};
            char num2[3] = {num[2], num[3], '\0'};
            Display::getInstance().clearBuffer(true);
            char text[5];
            sprintf(text, "%s+:%s", num1, num2);
            Serial.println(text);
            Display::getInstance().displayText(text, "", "static", "yes");
            inputToSend = "[" + String(text) + "]";
          }
          // Display the digit
          
        } else {
          Serial.println("Maximum digit limit reached.");
        }
      }
    }
  }
}

int RemoteControl::getNumberFromIR(uint32_t command) {
  for (uint8_t i = 0; i < 10; i++) {
    if (command == timerCodes[i]) {
      Serial.print("Detected Number: ");
      Serial.println(i);
      if(i == 9)
      {
        return 0;
      }
      else
      {
        return i+1;
      }
    }
  }
  return -1; // Invalid input
}

void RemoteControl::changeTBColourScheme()
{
  switch(tbIndex)
  {
    case 0: Display::getInstance().setTopColour(0xff0000);
            Display::getInstance().setBottomColour(0x00ff00);
            Display::getInstance().displayText("Red", "Green", "static", "no");
            break;

    case 1: Display::getInstance().setTopColour(0xff0000);
            Display::getInstance().setBottomColour(0x0000ff);
            Display::getInstance().displayText("Red", "Blue", "static", "no");
            break;

    case 2: Display::getInstance().setTopColour(0x00ff00);
            Display::getInstance().setBottomColour(0xff0000);
            Display::getInstance().displayText("Green", "Red", "static", "no");
            break;

    case 3: Display::getInstance().setTopColour(0x00ff00);
            Display::getInstance().setBottomColour(0x0000ff);
            Display::getInstance().displayText("Green", "Blue", "static", "no");
            break;

    case 4: Display::getInstance().setTopColour(0x0000ff);
            Display::getInstance().setBottomColour(0xff0000);
            Display::getInstance().displayText("Blue", "Red", "static", "no");
            break;

    case 5: Display::getInstance().setTopColour(0x0000ff);
            Display::getInstance().setBottomColour(0x00ff00);
            Display::getInstance().displayText("Blue", "Green", "static", "no");
            break;
  }
}

void RemoteControl::changeFColourScheme()
{
  switch(fIndex)
  {
    case 0: Display::getInstance().setTopColour(0xff0000);
            Display::getInstance().setBottomColour(0xff0000);
            Display::getInstance().setFullColour(0xff0000);
            Display::getInstance().displayText("Red", "", "static", "yes");
            break;

    case 1: Display::getInstance().setTopColour(0x00ff00);
            Display::getInstance().setBottomColour(0x00ff00);
            Display::getInstance().setFullColour(0x00ff00);
            Display::getInstance().displayText("Green", "", "static", "yes");
            break;

    case 2: Display::getInstance().setTopColour(0x0000ff);
            Display::getInstance().setBottomColour(0x0000ff);
            Display::getInstance().setFullColour(0x0000ff);
            Display::getInstance().displayText("Blue", "", "static", "yes");
            break;
  }
}


