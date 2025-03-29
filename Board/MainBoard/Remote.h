#ifndef REMOTE_H
#define REMOTE_H




class RemoteControl {
private:
    uint8_t bright;
    bool remoteStatus;
    unsigned int enteredValue;
    bool inputState;
    int minu;
    int tbIndex;
    int fIndex;
    bool timerInputMode;
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


    uint8_t getBrightness();

    void set(int minutes, int seconds);

    void updateDisplay(int time);

    void start();

    void calculateRemainingMin();
    uint32_t getRemoteCode();

    void manualTimerInput();
    void changeTBColourScheme();
    void changeFColourScheme();
    
};

#endif

