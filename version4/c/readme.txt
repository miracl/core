Namespaces are sinulated to separate different curves.

To this end the BIG type is renamed to BIG_XXX, where XXX can be changed to 
describe the size and layout of the BIG variable. Similarily the FP type 
is renamed FP_YYY, where YYY reflects the modulus used. Also the ECP type 
is renamed ECP_ZZZ, where ZZZ describes the actual curve. Function names 
are also decorated in the same way.

So for example to support both ED25519 and the NIST P256 curve on a 64-bit 
processor, we would need to create BIG_256_56, FP_25519, ECP_ED25519 and 
BIG_256_56, FP_NIST256, ECP_NIST256. Note that both curves could be built 
on top of BIG_256_56, as both require support for 256-bit numbers using 
an internal number base of 2^56.

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

The configuration files assume the gcc compiler. For clang edit the
config32.py and config64.py files and substitute "clang" for "gcc".
Note that clang is about 10-15% faster.*

NOTE: In the file config_curve.h a couple of methods with possible IP issues 
are commented out. For faster pairing code, edit this file.

As a quick example execute

py config32.py

or

python3 config32.py

Then select options 1, 3, 7, 19, 21, 27, 28 and 30, which are fixed for the example 
program. (For a 16-bit build select 1,3 and 5). Select 0 then to exit.

Then compile

gcc -O2 -std=c99 testall.c amcl.a -o testall.exe

if using MINGW in Windows. Or for Linux

gcc -O2 -std=c99 testall.c amcl.a -o testall

The test program exercises 3 different ordinary elliptic curves, a 
pairing friendly curve and RSA, all in the one binary.

The correct PIN is 1234


Next compile

gcc -O2 -std=c99 testbls.c amcl.a -o testbls.exe

if using MINGW in Windows. Or for Linux

gcc -O2 -std=c99 testbls.c amcl.a -o testbls

This program implements the pairing-based BLS signature


Next compile 

gcc -O2 -std=c99 benchtest_all.c amcl.a -o benchtest_all.exe

if using MINGW in Windows. Or for Linux

gcc -O2 -std=c99 benchtest_all.c amcl.a -o benchtest_all

This program provides some timings.

Finally

gcc -O2 -std=c99 testnhs.c amcl.a -o testnhs



*Using clang on Windows
Download latest clang from http://releases.llvm.org/download.html
Choose Clang for Windows (64-bit) (.sig) 
Install a free version of Microsoft Visual C++ https://www.visualstudio.com/downloads/
Now use "clang" wherever "gcc" was used before.
