# Description

This C++ version is really just C plus namespaces. Namespaces are the 
only feature of C++ that is used

Separate ROM files provide the constants required for each curve. The
associated header files (big.h, fp.h and ecp.h) also specify 
certain constants that must be set for the particular curve.

# Installation and Testing

To build a library and see it in action, copy all of the files in this 
directory to a fresh directory. Then execute the python3 script config32.py 
for a 32-bit build, or config64.py for a 64-bit build, and select the curves 
that you wish to support. Note that support for 16-bit builds is currently 
somewhat limited - see config16.py. A library is built automatically 
including all of the modules that you will need.

The configuration files assume the g++ compiler. For clang++ edit the
config32.py and config64.py files and substitute "clang++" for "g++".
Note that clang++ is about 10-15% faster.*

Make sure to use a 64-bit compiler on a 64-bit architecture.

---------------------------------------

To create a 32-bit library

    python3 config32.py

Then select options 1, 3, 7, 25, 27, 33, 34 and 36, which are fixed for the example 
programs. (For a 16-bit build select 1, 4 and 6). Select 0 then to exit.

Then compile

    g++ -O2  testecc.cpp core.a -o testall

(if using MINGW-W64 in Windows change -o testall to -o testall.exe)

The test program exercises 3 different ordinary elliptic curves (for ECDH 
Key exchange, ECDSA signature and ECIES encryption), plus RSA, all in the 
one binary


Next compile

    g++ -O2  testmpin.cpp core.a -o testall

This test program exercises 4 different pairing friendly curves using 
the MPIN authentication protocol.

The correct PIN is 1234


Next compile

    g++ -O2  testbls.cpp core.a -o testbls

This program implements the pairing-based BLS signature


Next compile 

    g++ -O2  benchtest_all.cpp core.a -o benchtest_all

This program provides some timings.

Finally

     g++ -O2  testnhs.cpp core.a -o testnhs

-------------------------------------------------

Alternatively building and testing can be combined via

    python3 configXX.py test

where XX can be 16, 32 or 64

-------------------------------------------------

NEW: Experimental support for emerging HPKE (Hybrid Public Key Encryption) standard.
See https://datatracker.ietf.org/doc/draft-irtf-cfrg-hpke/

New hpke.cpp/.h api files

- Supports KEM_IDs for X25519, X448, P256 and P521
- Supports HDF_IDs for SHA256/512
- Supports AEAD_IDs for AES-GCM-128/256 only

Create 32 or 64-bit library selecting curves 2 and 10 (X25519 and P521)

    g++ -O2 testhpke.cpp core.a -o testhpke

Test program runs through test vectors for all modes 0-3.

---------------------------------------------------

*Using clang on Windows.
Download latest clang from http://releases.llvm.org/download.html.
Choose Clang for Windows (64-bit) (.sig).
Install a free version of Microsoft Visual C++ https://www.visualstudio.com/downloads/.
Now use "clang++" wherever "g++" was used before.
