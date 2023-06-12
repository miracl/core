# Description

Namespaces are used to separate different curves.

# Installation and Testing

To build the library and see it in action, copy all of the files in this 
directory to a fresh directory. Then execute the python3 scripts config32.py 
or config64.py (depending on whether you want a 32 or 64-bit build), and 
select the curves that you wish to support. The configured library can be 
installed using maven. 

Tests will take a while to  run.

--------------------------------------------

To create a 64-bit library

    python3 config64.py

Choose options 1, 3, 7, 28, 30, 37, 38 and 42, for example.

Once the library is configured, you can compile and install with maven:

    cd core
    mvn clean install

Testing will be carried out during the installation process.

Elliptic curve key exchange, signature and encryption (ECDH, ECDSA and ECCSI) will be tested.
Also MPIN and BLS (Boneh-Lynn-Shacham) signature (using pairings)

-------------------------------------------------

Alternatively building and testing can be combined via

    python3 configXX.py test

where XX can be 32 or 64

-------------------------------------------------

NEW: support for emerging Hash To Curve standard.
See https://datatracker.ietf.org/doc/draft-irtf-cfrg-hash-to-curve/


Create 32 or 64-bit library selecting curves 1, 2, 3, 7, 17 and 31 (ED25519, C25519, NIST256, ED448, SECP256K1 and BLS12381)

    cd core
    mvn clean install

Test program runs through test vectors from the draft standard.

-------------------------------------------------

NEW: Experimental support for emerging HPKE (Hybrid Public Key Encryption) standard.
See https://datatracker.ietf.org/doc/draft-irtf-cfrg-hpke/

New HPKE.java api file

- Supports KEM_IDs for X25519, X448, P256 and P521
- Supports HDF_IDs for SHA256/512
- Supports AEAD_IDs for AES-GCM-128/256 only

Create 32 or 64-bit library selecting curves 2 and 10 (C25519 and P521)

Then compile and install with maven as above

    cd core
    mvn clean install

Test program TestHPKE.java runs through test vectors for all modes 0-3.
