NOTE: Updated to Rust 2018

NOTE: This version of the library requires Version 1.33+ of Rust for 64-bit 
integer support, and for Rust 2018, and also for const fn support.

Namespaces are used to separate different curves.

To build the library and see it in action, copy all of the files in this 
directory to a fresh root directory. Then execute the python3 script 
config32.py or config64.py (depending on whether you want a 32 or 64-bit 
build), and select the curves that you wish to support. Libraries will be 
built automatically including all of the modules that you will need.

As a quick example execute from your root directory

py config64.py

or

python3 config64.py

Then select options 1, 3, 7, 19, 21, 27, 28 and 30 (these are fixed for 
the example program provided). Select 0 to exit.

Then copy the library from amcl/target/release/libamcl.rlib to the
root directory, and execute

rustc TestALL.rs --extern amcl=libamcl.rlib

Run this test program by executing the program TestALL

rustc TestBLS.rs --extern amcl=libamcl.rlib

Run this test program by executing the program TestBLS


rustc BenchtestALL.rs --extern amcl=libamcl.rlib

Run this test program by executing the program BenchtestALL


rustc TestNHS.rs --extern amcl=libamcl.rlib

Run this test program by executing the program TestNHS
