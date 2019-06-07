This C++ version is really just C plus namespaces. Namespaces are the 
only feature of C++ that is used

Separate ROM files provide the constants required for each curve. The
associated header files (big.h, fp.h and ecp.h) also specify 
certain constants that must be set for the particular curve.

--------------------------------------

To build the library and see it in action, copy all of the files in this 
directory to a fresh directory. Then execute the python3 script config32.py 
for a 32-bit build, or config64.py for a 64-bit build, and select the curves 
that you wish to support. Note that support for 16-bit builds is currently 
somewhat limited - see config16.py. A library is built automatically 
including all of the modules that you will need.

The configuration files assume the g++ compiler. For clang++ edit the
config32.py and config64.py files and substitute "clang++" for "g++".
Note that clang++ is about 10-15% faster.*

As a quick example execute

py config32.py

or

python3 config32.py

Then select options 1, 3, 7, 19, 21, 27, 28 and 30, which are fixed for the example 
program. (For a 16-bit build select 1, 3 and 5). Select 0 then to exit.

Then compile

g++ -O2  testall.cpp amcl.a -o testall.exe

if using MINGW in Windows. Or for Linux

g++ -O2  testall.cpp amcl.a -o testall

The test program exercises 3 different ordinary elliptic curves, a 
pairing friendly curve and RSA, all in the one binary.

The correct PIN is 1234


Next compile

g++ -O2  testbls.cpp amcl.a -o testbls.exe

if using MINGW in Windows. Or for Linux

g++ -O2  testbls.cpp amcl.a -o testbls

This program implements the pairing-based BLS signature


Next compile 

g++ -O2  benchtest_all.cpp amcl.a -o benchtest_all.exe

if using MINGW in Windows. Or for Linux

g++ -O2  benchtest_all.cpp amcl.a -o benchtest_all

This program provides some timings.

Finally

g++ -O2  testnhs.cpp amcl.a -o testnhs


*Using clang on Windows
Download latest clang from http://releases.llvm.org/download.html
Choose Clang for Windows (64-bit) (.sig) 
Install a free version of Microsoft Visual C++ https://www.visualstudio.com/downloads/
Now use "clang++" wherever "g++" was used before.
