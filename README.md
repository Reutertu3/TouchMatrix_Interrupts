FH Dortmund ESP32 based 5x5 Touch Matrix Soundboard.

Touch inputs are registered via interrupts. All rows and columns perform a self-calibration upon startup
to accomodate different idle values depending on PCB design and other interferences.

Parameters for touch sensitivy and default volume can be adjusted in config.h
