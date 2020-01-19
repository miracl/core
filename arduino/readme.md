# Description

Example implementation for Arduino MKR1000 board (48MHz ARM Cortex M0 IoT 
board). Also works with other compatible Arduino boards, for example Fishino
Piranha (5-120MHz MIPS32 core).

First buy the board (approx $30), and install free Arduino IDE on your PC.
Connect the board to the PC with simple USB cable.

# Installation and Testing

The test programs can optionally be used with an internal ECCX08 true random 
number generator (the MKR1000 has one, others may not). 

If it is supported, install the ECCX08 library.

https://github.com/arduino-libraries/ArduinoECCX08

Download as a ZIP file. Then in Arduino IDE, Sketch &rightarrow; Include Library &rightarrow; Add .ZIP library

Finally you must "lock" the board to enable the random number generator, 
using for example the SelfSignedCert Tool. (IMHO the default configuration is 
fine).


Copy all files from the C++ MIRACL Core folder to a new working folder.

Copy all files from this Arduino folder to the same working folder.

If using windows, set a command alias via

    DOSKEY python3 = py $*

Edit core.h in the new folder to uncomment the line

    #define CORE_ARDUINO

If the particular board does NOT support an ECCX08 random number generator, 
edit the test*.ino programs and comment out

    #define HAVE_ECCX08

Then run the script

    python3 config.py

Choose one scheme to support. Realistically these small devices could probably 
only support one method at a time. Example programs only support BN and BLS12
pairing-friendly curves.

Note that RSA key generation can take a very long time, perhaps hours.

Now copy the working folder and drop it into the libraries subdirectory of the 
Sketchbook location which can be found in the Arduino IDE, via File->Preferences

Restart the IDE and navigate to File &rightarrow; Examples &rightarrow; Examples from custom libraries.
Your working folder should be there, so just click on the subdirectory 
containing the example code. Make sure to activate Tools &rightarrow; Serial Monitor to 
observe the output.


