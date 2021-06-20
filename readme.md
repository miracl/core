# The MIRACL Core Cryptographic Library

[![Build Status](https://travis-ci.org/miracl/core.svg?branch=master)](https://travis-ci.org/miracl/core)

The original contribution to the Apache Milagro Cryptographic Library (AMCL) has now been extended and is being re-released as MIRACL Core.

MIRACL Core is a multi-lingual and architecturally agnostic cryptographic library that supports elliptic curve cryptography, pairing-friendly curve cryptography, RSA, AES symmetric encryption and hash functions. It is designed from the ground up with side-channel attack resistance in mind. Multiple curves can be supported in a single application..

All code is entirely in the supported high-level languages. No assembly language and no third party code is required - these libraries are completely self-contained. MIRACL Core is currently available in the C, C++, Go, Rust, Python, Java, Javascript and Swift programming languages. Via code translators it can be easily translated to other languages (like WebAssembly and C#). It is particularly suitable for smaller devices such as those supported by Arduino infrastructure.

MIRACL Core features several extensions over the stock Milagro Cryptographic Library.

- Easy to use and flexible Threshold Secret Sharing module added (see share.c, share.cpp, SHARE.java etc.)

- Example program to track test vectors from https://datatracker.ietf.org/doc/draft-irtf-cfrg-hash-to-curve/

- Curves SM2, BN462, BLS48581, C13318, JUBJUB, X448, SECP160R1, C1174, BLS48286, TWEEDLEDUM, TWEEDLEDEE

- Support for precomputation for pairings (see BLS API)

- Performance and security improvements

- New HMAC message authentication module (KDFs, HMAC, HKDF_Extract/Expand, XOF_Expand, XMD_Expand)

- Improved side-channel resistance

- Deterministic hashing to curve points map2point (Elligator, SVDW, SSWU)

- Point compression in G2

- Demonstrated support for low-powered IoT processor nodes (ARM Cortex-M0/M4, MIPS32, RISC-V, ESP32)

- Emerging-standards-compliant implementation of BLS signature

- Emerging-standards-compliant implementation of HPKE (Hybrid Public Key Encryption)

- Emerging-standards-compliant implementation of deterministic Hash-To-Curve

Doxygen generated documentation for the C version of the library can be created by simply executing doxygen inside of the C directory. The C++ code is probably the best commented.

Install Python 3 and suitable compilers for each language of interest to you. Python 3 scripts are used to build the libraries from the command line for each of the supported languages. If using windows, set a command alias via

    DOSKEY python3 = py $*

For a quick start, try out the Python version of the library, and run our sample programs

