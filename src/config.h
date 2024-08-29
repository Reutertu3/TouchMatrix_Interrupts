#ifndef CONFIG_H
#define CONFIG_H

#define THRESHOLD 32        // Define a threshold for detecting touch
#define DEFAULTVOLUME 10    // Set volume on boot for the DFPlayer
#define DELAY 15            // Delay for the main loop  

// Mapping pins for Rows and Columns of the Touch Matrix (start counting from top right)
#define ROW1 4      // T0
#define ROW2 0      // T1
#define ROW3 2      // T2
#define ROW4 15     // T3
#define ROW5 13     // T4
#define COL1 12     // T5
#define COL2 14     // T6
#define COL3 27     // T7
#define COL4 33     // T8
#define COL5 32     // T9

const int touchRow[5] = {4, 0, 2, 15, 13};              // Mapping rows to iterate through them
const int touchCol[5] = {12, 14, 27, 33, 32};           // Mapping columns to iterate through them

const int soundmatrix[5][5] =   { { 1, 6, 11, 16, 21 },     // Mapping sound files (00xx.mp) to the touch matrix
                                { 2, 7, 12, 17, 22 },
                                { 3, 8, 13, 18, 23 },
                                { 4, 9, 14, 19, 24 },
                                { 5, 10, 15, 20, 25 } };


#endif // CONFIG_H