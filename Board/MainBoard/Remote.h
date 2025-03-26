#ifndef REMOTE_H
#define REMOTE_H




class RemoteControl {
private:
    uint8_t bright;
    bool remoteStatus;
    unsigned int enteredValue;
    bool inputState;
    int minu;
    uint32_t colorCodes[3] = {0x0000ff, 0xff0000, 0x00ff00};
    uint32_t timerCodes[10] = {0xFB040707, 0xFA050707, 0xF9060707, 0xF7080707, 0xF6090707,
                                0xF50A0707, 0xF30C0707, 0xF20D0707, 0xF10E0707, 0xEE110707};
    int getNumberFromIR(uint32_t command);

public:
    
   
    RemoteControl();

    void setupRemote();

    void displayDefaultMessage();

    void adjustBrightness(int change);

    void toggleRemote(uint32_t remoteCode);

    void handleTimerCodes(uint32_t remoteCode);

    void setDefaultMessage(uint32_t remoteCode);

    void useRemote();

    void manualTimerInput(uint32_t remoteCode);

    uint8_t getBrightness();

    void set(int minutes, int seconds);

    void updateDisplay(int time);

    void start();

    void calculateRemainingMin();
    uint32_t getRemoteCode();
/*
    void manualTimerFunction(uint32_t remoteCode)
{
    int s = 0;
    int m = 0;

    // Check if remoteCode matches any timerCodes[i]
    for (uint8_t i = 0; i < 10; i++) {
        if (remoteCode == timerCodes[i]) {
            while (true) {
                // Exit if stop command is received
                if (remoteCode == 0x97680707) {
                    remoteTimer(m, s, true);
                    return;
                }

                if (s == 0) {
                    s = i;  // Set initial seconds
                } else {
                    s = s * 10 + i; // Append digit to seconds

                    // Convert to minutes if seconds exceed 99
                    if (s >= 100) {
                        m = s / 100;  // Get minutes
                        s = s % 100;  // Get remaining seconds
                    }
                }

                countdownTimer(m, s, true);

                
                remoteCode = getRemoteInput();
            }
        }
    }
}

uint32_t getRemoteInput()
{
  uint32_t remoteCode;
  if (IrReceiver.decode()) {
    uint32_t remoteCode = IrReceiver.decodedIRData.decodedRawData;
    IrReceiver.resume();
  }
  return remoteCode;
}
*/
};

#endif

