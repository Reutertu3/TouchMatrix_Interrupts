#include <Arduino.h>
#include <config.h>
#include <DFPlayerMini_Fast.h>
#include <SoftwareSerial.h>
//#include <esp.mac.h>

// Store the last touch state for debouncing
volatile boolean lastTouchState[5][5] = {false};
volatile boolean isTouched = false;
volatile boolean _touchDetected = false;
int soundFileNumber = 0;
int volume = DEFAULTVOLUME;

// DFPlayer setup
SoftwareSerial mySoftwareSerial(16, 17); // RX, TX
DFPlayerMini_Fast myDFPlayer;


// Function to determine which pad was touched
int getTouchPin() {

    // Check each row
    for (int row = 0; row < 5; row++) {
        int rowVal = touchRead(touchRow[row]);

        // If a touch is detected on the row
        if (rowVal < THRESHOLD) {
            // Check each column
            for (int col = 0; col < 5; col++) {
                int colVal = touchRead(touchCol[col]);

                // If a touch is detected on the column
                if (colVal < THRESHOLD) {
                    // Check if the touch is a new touch (finger was previously lifted)
                    if (!lastTouchState[row][col]) {                        
                        lastTouchState[row][col] = true;                // Update the last touch state to indicate it's now being touched                                            
                        if (SERIAL_MONITOR_ENABLED) {
                            Serial.print("Touch detected at: ");
                            Serial.print("Row ");
                            Serial.print(row+1);
                            Serial.print(", Col ");
                            Serial.println(col+1);
                            Serial.print("Pin detected ");
                            Serial.println(soundmatrix[row][col]);
                        }
                    return soundmatrix[row][col];                   // Return the corresponding sound file number from the touch matrix
                    }
                } else {
                    // If the pad is no longer being touched, reset the state
                    lastTouchState[row][col] = false;
                }
            }
        } else {
            // If no touch is detected on this row, reset the states for all columns in this row
            for (int col = 0; col < 5; col++) {
                lastTouchState[row][col] = false;
            }
        }
    }
    // Return -1 if no new touch was detected
    return -1;
}

void setVolume(int volume){
    myDFPlayer.volume(volume);
}

void touchDetected(){
  //_touchDetected = true;  

        int soundFileNumber = getTouchPin();
        if (soundFileNumber == 24) {
            if (volume <= 27)
            volume = volume +3;
            setVolume(volume);
            if (SERIAL_MONITOR_ENABLED) {
                Serial.print("Current Volume: ");
                Serial.println(volume);
            }
        }
        else if (soundFileNumber == 25) {
            if (volume >= 3)
            volume = volume -3;
            setVolume(volume);
            if (SERIAL_MONITOR_ENABLED) {
                Serial.print("Current Volume: ");
                Serial.println(volume);
            }
        }

        else if (soundFileNumber != -1 && soundFileNumber < 24 && !myDFPlayer.isPlaying()) {
            if (SERIAL_MONITOR_ENABLED) {
                Serial.print("Playing MP3 number: ");
                Serial.println(soundFileNumber);
            }
            myDFPlayer.playFromMP3Folder(soundFileNumber);
        }
    }



void setup() {
    if (SERIAL_MONITOR_ENABLED) {
        Serial.begin(9600);
    }

    // Initialize the DFPlayer
    mySoftwareSerial.begin(9600);
    if (!myDFPlayer.begin(mySoftwareSerial)) {
        Serial.println("DFPlayer Mini not detected!");
        while (true);
    }
    myDFPlayer.volume(volume);  // Set volume level (0 to 30)
    delay(DELAY);
    // Set GPIO0 as INPUT to avoid bootstrap issues
    pinMode(0, INPUT_PULLDOWN);
    // Attach interrupts to the touch pins
    touchAttachInterrupt(ROW1, touchDetected, THRESHOLD);
    touchAttachInterrupt(ROW2, touchDetected, THRESHOLD);
    touchAttachInterrupt(ROW3, touchDetected, THRESHOLD);
    touchAttachInterrupt(ROW4, touchDetected, THRESHOLD);
    touchAttachInterrupt(ROW5, touchDetected, THRESHOLD);

    touchAttachInterrupt(COL1, touchDetected, THRESHOLD);
    touchAttachInterrupt(COL2, touchDetected, THRESHOLD);
    touchAttachInterrupt(COL3, touchDetected, THRESHOLD);
    touchAttachInterrupt(COL4, touchDetected, THRESHOLD);
    touchAttachInterrupt(COL5, touchDetected, THRESHOLD);
}


void loop() {
    // Deprecated loop since we handle everything via the interrupt callback function

    // if (_touchDetected) {
    //     int soundFileNumber = getTouchPin();
    //     if (soundFileNumber == 24) {
    //         if (volume <= 28)
    //         volume = volume +2;
    //         setVolume(volume);
    //         Serial.print("Current Volume: ");
    //         Serial.println(volume);
    //     }
    //     else if (soundFileNumber == 25) {
    //         if (volume >= 2)
    //         volume = volume -2;
    //         setVolume(volume);
    //         Serial.print("Current Volume: ");
    //         Serial.println(volume);
    //     }

    //     else if (soundFileNumber != -1 && soundFileNumber < 24 && !myDFPlayer.isPlaying()) {
    //         myDFPlayer.playFromMP3Folder(soundFileNumber);
    //     }
    //     _touchDetected = false; // reset flag as we handled the touch event
    // }
    //delay(DELAY);
}
