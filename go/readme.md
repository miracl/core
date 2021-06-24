# Description

Namespaces are used to separate different curves.

So for example to support both ED25519 and the NIST P256 curves, one
could import into a particular module both "core/ED25519" and "core/NIST256"

Separate ROM files provide the constants required for each curve. Some
files (BIG.go, FP.go, ECP.go) also specify certain constants 
that must be set for the particular curve.

# Installation and Testing

Make sure that the GOPATH environmental variable is set to point
at the working directory

Then clone the MIRACL Core repository by executing

    go get github.com/miracl/core

Ignore the warning message about "no Go files"

This assumes that Git is installed on your machine - see 
https://git-scm.com/download/

Next navigate to $GOPATH/src/github.com/miracl/core/go

NOTE: If the repository address changes, then change the above, and also
change import statements at the start of some files.

To build the library and see it in action, execute the python3 
script config32.py or config64.py (depending on whether you want a 
32 or 64-bit build), and select the curves that you wish to support. 

--------------------------------------------

(Note from version 1.16 of GO, the environment variable GO111MODE should be set to auto)

To create a 64-bit library

    python3 config64.py

Then select options 1, 3, 7, 28, 30, 37, 38 and 41 (these are fixed for the example 
program provided). Select 0 to exit.

Run the ECC test program by executing

    go run TestECC.go

Run the MPIN test program by executing

    go run TestMPIN.go

The correct PIN is 1234

Run another test program by executing

    go run TestBLS.go

Next run the Benchmark program by executing

    go run BenchtestALL.go

Finally

    go run TestNHS.go

-------------------------------------------------

Alternatively building and testing can be combined via

    python3 configXX.py test

where XX can be 32 or 64

Note that this builds the library for ALL of the options. 
Executables of the above test programs are created, and can be "go run" immediately.

-------------------------------------------------

NEW: support for emerging Hash To Curve standard.
See https://datatracker.ietf.org/doc/draft-irtf-cfrg-hash-to-curve/


Create 32 or 64-bit library selecting curves 1, 2, 3, 7, 17 and 31 (ED25519, C25519, NIST256, GOLDILOCKS, SECP256K1 and BLS12381)

    go run TestHTP.go

Test program runs through test vectors from the draft standard.

-------------------------------------------------

NEW: Experimental support for emerging HPKE (Hybrid Public Key Encryption) standard.
See https://datatracker.ietf.org/doc/draft-irtf-cfrg-hpke/

New HPKE.go api file

- Supports KEM_IDs for X25519, X448, P256 and P521
- Supports HDF_IDs for SHA256/512
- Supports AEAD_IDs for AES-GCM-128/256 only

Create 32 or 64-bit library selecting curves 2 and 10 (C25519 and P521)

    go run TestHPKE.go

Test program runs through test vectors for all modes 0-3.
