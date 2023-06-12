# Description

Namespaces are simulated to separate different curves.

To this end the BIG type is renamed to BIG_XXX, where XXX can be changed to 
describe the size and layout of the BIG variable. Similarily the FP type 
is renamed FP_YYY, where YYY reflects the modulus used. Also the ECP type 
is renamed ECP_ZZZ, where ZZZ describes the actual curve. Function names 
are also decorated in the same way.

So for example to support both Ed25519 and the NIST P256 curve on a 64-bit 
processor, we would need to create BIG_256_56, FP_25519, ECP_Ed25519 and 
BIG_256_56, FP_NIST256, ECP_NIST256. Note that both curves could be built 
on top of BIG_256_56, as both require support for 256-bit numbers using 
an internal number base of 2^56.

Separate ROM files provide the constants required for each curve. The
associated header files (big.h, fp.h and ecp.h) also specify 
certain constants that must be set for the particular curve.


# Installation and Testing

To build the library and see it in action, copy all of the files in this 
directory to a fresh directory. Then execute the python3 script config32.py 
for a 32-bit build, or config64.py for a 64-bit build, and select the curves 
that you wish to support. Note that support for 16-bit builds is currently 
somewhat limited - see config16.py. A library is built automatically 
including all of the modules that you will need.

The configuration files assume the gcc compiler. For clang edit the
config*.py files and substitute "clang" for "gcc".
Note that clang is about 10-15% faster.*

Make sure to use a 64-bit compiler on a 64-bit architecture.

NOTE: In the file config_curve.h a couple of methods with possible IP issues 
are commented out. For faster pairing code, edit this file.

---------------------------------------

To create a 32-bit library

    python3 config32.py


Then select options 1, 3, 7, 28, 30, 37, 38 and 42, which are fixed for the example 
program. (For a 16-bit build select 1,4 and 6). Select 0 then to exit.

Then compile

    gcc -O2 -std=c99 testecc.c core.a -o testecc

(if using MINGW-W64 in Windows change -o testecc to -o testecc.exe)

The test program exercises 3 different ordinary elliptic curves (for ECDH 
Key exchange, ECDSA signature and ECIES encryption), plus RSA, all in the 
one binary

Next compile

    gcc -O2  testeddsa.c core.a -o testeddsa

This test program exercises the EDDSA signature algorithm using the Edwards curves Ed25519 and Ed448


Next compile

    gcc -O2  -std=c99 testmpin.c core.a -o testmpin

This test program exercises 4 different pairing friendly curves using 
the MPIN authentication protocol.


Next compile

    gcc -O2 -std=c99 testbls.c core.a -o testbls

This program implements the pairing-based BLS signature


Next compile 

    gcc -O2 -std=c99 benchtest_all.c core.a -o benchtest_all


This program provides some timings.

Finally

    gcc -O2 -std=c99 testnhs.c core.a -o testnhs

----------------------------------------

Alternatively building and testing can be combined via

    python3 configXX.py test

where XX can be 16, 32 or 64

Note that this builds the library for ALL of the options. 
Executables of the above test programs are created, and can be run immediately.

-------------------------------------------------

NEW: support for emerging Hash To Curve standard.
See https://datatracker.ietf.org/doc/draft-irtf-cfrg-hash-to-curve/


Create 32 or 64-bit library selecting curves 1, 2, 3, 7, 17 and 31 (Ed25519, C25519, NIST256, Ed448, SECP256K1 and BLS12381)

    gcc -O2 -std=c99 testhtp.c core.a -o testhtp

Test program runs through test vectors from the draft standard.

-----------------------------------------

NEW: Experimental support for emerging HPKE (Hybrid Public Key Encryption) standard.
See https://datatracker.ietf.org/doc/draft-irtf-cfrg-hpke/

New hpke.c/.h api files

- Supports KEM_IDs for X25519, X448, P256 and P521
- Supports HDF_IDs for SHA256/512
- Supports AEAD_IDs for AES-GCM-128/256 only

Create 32 or 64-bit library selecting curves 2 and 10 (X25519 and P521)

    gcc -O2 -std=c99 testhpke.c core.a -o testhpke

Test program runs through test vectors for all modes 0-3.

---------------------------------------------------

*Using clang on Windows
Download latest clang from http://releases.llvm.org/download.html
Choose Clang for Windows (64-bit) (.sig) 
Install a free version of Microsoft Visual C++ https://www.visualstudio.com/downloads/
Now use "clang" wherever "gcc" was used before.
