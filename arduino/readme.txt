Example implementation for Arduino MKR1000 board (48MHz ARM Cortex M0 IoT 
board)

First buy the board (approx $25), and install free Arduino IDE on your PC.
Connect the board to the PC with simple USB cable.

The test programs use the internal ECCX08 true random number generator, so 
you must first "lock" the board. (IMHO the default configuration is fine).
Also install the ECCX08 library.

https://github.com/arduino-libraries/ArduinoECCX08

Copy all files from the C++ MIRACL Core folder to a new working folder.

Copy all files from this Arduino folder to the same working folder.

Edit amcl.h in the new folder to uncomment the line

#define AMCL_ARDUINO

Then run the script

py config.py

Choose one scheme to support. Realistically the MKR1000 could only support 
one method at a time.

Note that RSA key generation can take a very long time, perhaps hours.

Now copy the working folder and drop it into the libraries subdirectory of the 
Sketchbook location which can be found in the Arduino IDE, via File->Preferences

Restart the IDE and navigate to File->Examples->Examples from custom libraries.
Your working folder should be there, so just click on the subdirectory 
containing the example code. Make sure to activate Tools->Serial Monitor to 
observe the output.



