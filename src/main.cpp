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
//int touchThreshold = THRESHOLD-10;
// Define arrays to store the calibration values (idle readings)
int rowThresholds[5];
int colThresholds[5];

// DFPlayer setup
SoftwareSerial mySoftwareSerial(16, 17); // RX, TX
DFPlayerMini_Fast myDFPlayer;

void logOutput(int row, int col, int colVal, int rowVal){

    cli();
    Serial.print("Touch detected at: ");
    Serial.print("Row ");
    Serial.print(row+1);
    Serial.print(", Col ");
    Serial.println(col+1);
    Serial.print("Pin detected ");
    Serial.println(soundmatrix[row][col]);
    Serial.println("Calib measures row col: ");
    Serial.print(rowThresholds[row]);
    Serial.print(" / ");
    Serial.println(colThresholds[col]);
    Serial.println("Current measures row col: ");
    Serial.print(rowVal);
    Serial.print(" / ");
    Serial.println(colVal);
    sei();
}
// Function to determine which pad was touched
int getTouchPin() {

    // Check each row
    for (int row = 0; row < 5; row++) {
        int rowVal = touchRead(touchRow[row]);

        // If a touch is detected on the row
        if (rowVal < (rowThresholds[row] - THREDHOLDDELTA)) {
            // Check each column
            for (int col = 0; col < 5; col++) {
                int colVal = touchRead(touchCol[col]);

                // If a touch is detected on the column
                if (colVal < (colThresholds[col] - THREDHOLDDELTA)) {
                    // Check if the touch is a new touch (finger was previously lifted)
                    if (!lastTouchState[row][col]) {                        
                        lastTouchState[row][col] = true;                // Update the last touch state to indicate it's now being touched                                            
                        if (DEBUG) {
                            logOutput(row, col, rowVal, colVal);
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

        int soundFileNumber = getTouchPin();
        if (soundFileNumber != -1 && soundFileNumber < 24 && !myDFPlayer.isPlaying()) {
            DEBUG_SERIAL.print("Playing MP3 number: ");
            DEBUG_SERIAL.println(soundFileNumber);
            myDFPlayer.playFromMP3Folder(soundFileNumber);
        }
        else if (soundFileNumber == 24) {
            if (volume <= 27)
            volume = volume +3;
            setVolume(volume);
            DEBUG_SERIAL.print("Current Volume: ");
            DEBUG_SERIAL.println(volume);
        }
        else if (soundFileNumber == 25) {
            if (volume >= 3)
            volume = volume -3;
            setVolume(volume);
            DEBUG_SERIAL.print("Current Volume: ");
            DEBUG_SERIAL.println(volume);
        }
}

void selfCalibrate() {
  // Calibrate rows
  rowThresholds[0] = touchRead(ROW1);
  rowThresholds[1] = touchRead(ROW2);
  rowThresholds[2] = touchRead(ROW3);
  rowThresholds[3] = touchRead(ROW4);
  rowThresholds[4] = touchRead(ROW5);

  // Calibrate columns
  colThresholds[0] = touchRead(COL1);
  colThresholds[1] = touchRead(COL2);
  colThresholds[2] = touchRead(COL3);
  colThresholds[3] = touchRead(COL4);
  colThresholds[4] = touchRead(COL5);
}

void outputCalibrateValues(){
// Print calibration values for debugging
  DEBUG_SERIAL.println("Row Thresholds:");
  for (int i = 0; i < 5; i++) {
    DEBUG_SERIAL.print("Row ");
    DEBUG_SERIAL.print(i + 1);
    DEBUG_SERIAL.print(": ");
    DEBUG_SERIAL.println(rowThresholds[i]);
  }

  DEBUG_SERIAL.println("Column Thresholds:");
  for (int i = 0; i < 5; i++) {
    DEBUG_SERIAL.print("Col ");
    DEBUG_SERIAL.print(i + 1);
    DEBUG_SERIAL.print(": ");
    DEBUG_SERIAL.println(colThresholds[i]);
  }
}

void attachTouchInterrupts(){
    touchAttachInterrupt(ROW1, touchDetected, rowThresholds[0] - THREDHOLDDELTA);
    touchAttachInterrupt(ROW2, touchDetected, rowThresholds[1] - THREDHOLDDELTA - 4); // Special treatment for GPIO0
    touchAttachInterrupt(ROW3, touchDetected, rowThresholds[2] - THREDHOLDDELTA);
    touchAttachInterrupt(ROW4, touchDetected, rowThresholds[3] - THREDHOLDDELTA);
    touchAttachInterrupt(ROW5, touchDetected, rowThresholds[4] - THREDHOLDDELTA);

    touchAttachInterrupt(COL1, touchDetected, colThresholds[0] - THREDHOLDDELTA);
    touchAttachInterrupt(COL2, touchDetected, colThresholds[1] - THREDHOLDDELTA);
    touchAttachInterrupt(COL3, touchDetected, colThresholds[2] - THREDHOLDDELTA);
    touchAttachInterrupt(COL4, touchDetected, colThresholds[3] - THREDHOLDDELTA);
    touchAttachInterrupt(COL5, touchDetected, colThresholds[4] - THREDHOLDDELTA);
}



void setup() {
    #if DEBUG == true
        Serial.setTxBufferSize(SERIAL_TX_BUFFER_SIZE);
        Serial.begin(115200);
    #endif
    // Set GPIO0 as INPUT to avoid bootstrap issues
    pinMode(0, INPUT_PULLDOWN);
    touchSetCycles(TOUCHCYCLES, TOUCHCYCLES);
    // Initialize the DFPlayer
    mySoftwareSerial.begin(9600);
    if (!myDFPlayer.begin(mySoftwareSerial)) {
        Serial.println("DFPlayer Mini not detected!");
        while (true);
    }
    delay(DELAY);

    setVolume(volume);  // Set volume level (0 to 30)
    selfCalibrate();
    if (DEBUG){
        outputCalibrateValues();
    }
    delay(DELAY);
    attachTouchInterrupts();
}


void loop() {

}
