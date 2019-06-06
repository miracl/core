What was the Apache Milagro Cryptographic Library (AMCL) is now renamed
as MIRACL Core. It also has new licensing conditions, which should be read 
carefully.

Version 2 is now deprecated.

Version 4 features several improvements over version 3. Version 3 will still 
be maintained, but not developed further.

MIRACL Core v4.0 features:- 

New curves (SM2, BN462, BLS48_581)
Support for precomputation for pairings (see BLS API)
Performance improvements
Demonstrated support for low-powered IoT processor nodes
Updated manual

-----------------------------------

AMCL v3.2 incorporated many minor improvements

Python version
Web Assembly support
Improved side channel resistance
Faster Swift code
Better Rust build system
Improved modular inversion algorithm
General speed optimizations
Improved Javascript testbed
More curves supported
New BLS signature API
Post quantum New Hope Implementation

-----------------------------------

AMCL v3.1 

Version 3.1 is a major "under the hood" upgrade. Field arithmetic is 
performed using ideas from http://eprint.iacr.org/2017/437 to ensure 
that critical calculations are performed in constant time. This strongly 
mitigates against side-channel attacks. Exception-free formulae are 
now used for Weierstrass elliptic curves. 

----------------------------------

AMCL V3.0

The main improvement is that AMCL v3 can optionally simultaneously support 
multiple elliptic curves and RSA key sizes within a single appliction.

Note that AMCL is largely configured at compile time. In version 3 this
configuration is handled by the Python script.

AMCL is available in 32-bit and 64-bit versions in most languages. Limited 
support for 16-bit processors is provided by the C version.

Languages like to remain "standard" irrespective of the underlying 
hardware. However when it comes to optimal performance, it is impossible 
to remain architecture-agnostic. If a processor supports 64-bit 
instructions that operate on 64-bit registers, it will be a waste not to
use them. Therefore the 64-bit language versions should always be used
on 64-bit processors. A standard Python 3 script to build libraries in all
supported languages.


---------------------------------------------
