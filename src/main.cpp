#include <Arduino.h>
#include <config.h>
#include <DFPlayerMini_Fast.h>
#include <SoftwareSerial.h>
//#include <esp.mac.h>

// Store the last touch state for debouncing
bool lastTouchState[5][5] = {false};

// Define the touch matrix with corresponding sound file numbers
int touchmatrix[5][5] = { { 1, 6, 11, 16, 21 },
                          { 2, 7, 12, 17, 22 },
                          { 3, 8, 13, 18, 23 },
                          { 4, 9, 14, 19, 24 },
                          { 5, 10, 15, 20, 25 } };

// DFPlayer setup
SoftwareSerial mySoftwareSerial(16, 17); // RX, TX
DFPlayerMini_Fast myDFPlayer;

// Function to handle the touch interrupt
void IRAM_ATTR onTouchDetected() {
    // Read each row
    for (int row = 0; row < 5; row++) {
        int rowVal = touchRead(ROW1 + row);

        // Read each column
        for (int col = 0; col < 5; col++) {
            int colVal = touchRead(COL1 + col);

            // Check if the touch value is below the threshold
            bool isTouched = (rowVal < THRESHOLD) && (colVal < THRESHOLD);

            if (isTouched && !lastTouchState[row][col]) {
                Serial.print("Touch detected at: ");
                Serial.print("Row ");
                Serial.print(row + 1);
                Serial.print(", Col ");
                Serial.println(col + 1);

                // Play the corresponding sound file
                int soundFileNumber = touchmatrix[row][col];
                myDFPlayer.playFromMP3Folder(soundFileNumber);

                // Update the last touch state
                lastTouchState[row][col] = true;
            } else if (!isTouched) {
                // Reset touch state when touch is lifted
                lastTouchState[row][col] = false;
            }
        }
    }
}

void setup() {

    // Set GPIO0 as INPUT to avoid bootstrap issues
    pinMode(0, INPUT_PULLDOWN);
    Serial.begin(115200);

    // Initialize the DFPlayer
    mySoftwareSerial.begin(9600);
    if (!myDFPlayer.begin(mySoftwareSerial)) {
        Serial.println("DFPlayer Mini not detected!");
        while (true);
    }
    myDFPlayer.volume(10);  // Set volume level (0 to 30)

    // Attach interrupts to the touch pins
    touchAttachInterrupt(ROW1, onTouchDetected, THRESHOLD);
    touchAttachInterrupt(ROW2, onTouchDetected, THRESHOLD);
    touchAttachInterrupt(ROW3, onTouchDetected, THRESHOLD);
    touchAttachInterrupt(ROW4, onTouchDetected, THRESHOLD);
    touchAttachInterrupt(ROW5, onTouchDetected, THRESHOLD);

    touchAttachInterrupt(COL1, onTouchDetected, THRESHOLD);
    touchAttachInterrupt(COL2, onTouchDetected, THRESHOLD);
    touchAttachInterrupt(COL3, onTouchDetected, THRESHOLD);
    touchAttachInterrupt(COL4, onTouchDetected, THRESHOLD);
    touchAttachInterrupt(COL5, onTouchDetected, THRESHOLD);
}

void loop() {
    // Nothing needed here as everything is handled by the interrupt
}
