# The MIRACL Core Cryptographic Library

[![Build Status](https://travis-ci.org/miracl/core.svg?branch=master)](https://travis-ci.org/miracl/core)

The original contribution to the Apache Milagro Cryptographic Library (AMCL)
has now been extended and is being re-released as MIRACL Core.


MIRACL Core is a multi-lingual and architecturally agnostic Cryptographic
library that supports elliptic curve cryptography, pairing-friendly curve
cryptography, RSA, AES symmetric encryption and hash functions. All code
is entirely in the supported high-level languages. No assembly language
and no third party code is required - these libraries are completely self-
contained. MIRACL Core was designed from the ground up with side-channel
attack resistance in mind. Multiple curves can be supported in a single
application. Read the PDF manual for more details.


MIRACL Core features several improvements over AMCL. We will still maintain
AMCL but not develop it further.

MIRACL Core features:-

- New curves (SM2, BN462, BLS48581, C13318, JUBJUB, X448, SECP160R1, C1174)
- Support for precomputation for pairings (see BLS API)
- Performance and security improvements
- New HMAC message authentication module (KDFs, HMAC, HKDF_Extract/Expand)
- Improved side-channel resistance
- Deterministic hashing to curve points (Elligator, SWU)
- Point compression in G2
- Demonstrated support for low-powered IoT processor nodes (ARM Cortex-M0/M4, MIPS32, RISC-V, ESP32)
- Emerging-standards-compliant implementation of BLS signature
- Emerging-standards-compliant implementation of HPKE (Hybrid Public Key Encryption) 
- An updated manual

Read the user manual MIRACL_Core.pdf for more details

Doxygen generated documentation for the C version of the library can be
created by simply executing doxygen inside of the C directory

Install Python 3 and suitable compilers for each language of interest to you.
Python 3 scripts are used to build the libraries from the command line for
each of the supported languages. If using windows, set a command alias via

    DOSKEY python3 = py $*

For a quick start, try out the Python version of the library, and run its
example program.

-------------------------------------------

# Contributors

The following people have contributed to the MIRACL Core library

- Mike Scott
- Kealan McCusker
- Alessandro Budroni
- Samuele Andreoli

