# Description

NOTE: Updated to Rust 2018

NOTE: This version of the library requires Version 1.33+ of Rust for 64-bit 
integer support, and for Rust 2018, and also for const fn support.

NOTE: (18/02/2021) Package "core" changed to "mcore" to avoid clash with Rust

NOTE: (29/03/2021) Library now supports #![no_std] flag

Namespaces are used to separate different curves.

# Installation and Testing

To build the library and see it in action, copy all of the files in this 
directory to a fresh root directory. Then execute the python3 script 
config32.py or config64.py (depending on whether you want a 32 or 64-bit 
build), and select the curves that you wish to support. Libraries will be 
built automatically including all of the modules that you will need.

-----------------------------------------

As a quick example execute from your root directory

    python3 config64.py

Then select options 1, 3, 7, 28, 30, 37, 38 and 42 (these are fixed for 
the example program provided). Select 0 to exit.

Then copy the library from mcore/target/release/libmcore.rlib to the
root directory, and execute

    rustc TestECC.rs --extern mcore=libmcore.rlib

Run this test program by executing the program TestECC

    rustc TestEDDSA.rs --extern mcore=libmcore.rlib

Run this test program by executing the program TestEDDSA

    rustc TestMPIN.rs --extern mcore=libmcore.rlib

Run this test program by executing the program TestMPIN

    rustc TestBLS.rs --extern mcore=libmcore.rlib

Run this test program by executing the program TestBLS

    rustc BenchtestALL.rs --extern mcore=libmcore.rlib

Run this test program by executing the program BenchtestALL

    rustc TestNHS.rs --extern mcore=libmcore.rlib

Run this test program by executing the program TestNHS

-------------------------------------------------

Alternatively building and testing can be combined via

    python3 configXX.py test

where XX can be 32 or 64

Note that this builds the library for ALL of the options. 
Executables of the above test programs are created, and can be run immediately.

-------------------------------------------------

## Using MIRACL core with Cargo

Once you have built the library by running the python script, the contents of the rust folder will have changed, specifically all code files will now be under the folder mcore. This folder is just a rust library and can be included in any rust project.

Create a new rust project
```    cargo new mynewmiraclproject```

Copy the mcore folder into this new projects root directory

Finally, update the cargo.toml file in your new project to include the mcore library as a dependency. Here path must equal the path to the mcore folder, which if copied into the project should be just under the root.
```
[dependencies]
mcore={path="mcore", version="0.1.0"}
```

-------------------------------------------------

NEW: support for emerging Hash To Curve standard.
See https://datatracker.ietf.org/doc/draft-irtf-cfrg-hash-to-curve/


Create 32 or 64-bit library selecting curves 1, 2, 3, 7, 17 and 31 (ed25519, c25519, nist256, ed448, secp256k1 and bls12381)

    rustc TestHTP.rs --extern mcore=libmcore.rlib

Test program runs through test vectors from the draft standard.

-------------------------------------------------

NEW: Experimental support for emerging HPKE (Hybrid Public Key Encryption) standard.
See https://datatracker.ietf.org/doc/draft-irtf-cfrg-hpke/

New hpke.rs api file

- Supports KEM_IDs for X25519, X448, P256 and P521
- Supports HDF_IDs for SHA256/512
- Supports AEAD_IDs for AES-GCM-128/256 only

Create 32 or 64-bit library selecting curves 2 and 10 (c25519 and nist521)

    rustc TestHPKE.rs --extern mcore=libmcore.rlib

Test program runs through test vectors for all modes 0-3.
