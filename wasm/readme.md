
# Description

MIRACL Core in Webassembly 

See https://webassembly.org/getting-started/developers-guide/


The MIRACL Core library already has a Javascript version, but can also run 
up to 20 times faster in a browser that supports Webassembly. And thats
most of the popular browsers in use today (Firefox, Safari, Edge, Chrome)

The C, C++ or Rust version of the MIRACL Core library can be compiled to 
a bitcode that runs directly in the browser, by-passing Javascript 
entirely. Which is good for our type of application, as the way
in which Javascript handles integer arithmetic is very slow.

To install the Emscripten C/C++ compiler follow the instructions
above. 


# Installation and Testing using C

First make sure the environment is set up correctly for your command window 
via something like

    source ./emsdk_env.sh --build=Release

Copy the MIRACL Core C code into a new directory, and then copy in the 
config32.py and config64.py files from this wasm directory. For a 32-bit 
build in the new directory execute

    python3 config32.py

Then select options 1, 3, 7, 28, 30, 36, 37 and 40, which are fixed for 
the example programs.

Build some test programs with

    emcc -O2 benchtest_all.c core.a -s WASM=1 -o benchtest_all.html
    emcc -O2 testecc.c core.a -s WASM=1 -o testecc.html
    emcc -O2 testbls.c core.a -s WASM=1 -o testbls.html

Then run a local HTML server (as described in the link above) 

    emrun --no_browser --port 8080 .

and then for example point your browser at http://localhost:8080/testbls.html 

Wait for programs to complete (which may take a while).

Alternatively run the examples in Node.js

    node testecc.js
    node testbls.js
    node benchtest_all.js

# Installation and Testing using Rust

Webassembly can also be generated from the Rust code. First the Rust compiler 
must be updated to target wasm, by

    rustup target add wasm32-unknown-emscripten

The Emscripten toolchain is also required, as above

Copy the MIRACL Core Rust code into a new directory. 

Edit config32.py and replace the line

    os.system("cargo rustc --manifest-path core"+slashtext+"Cargo.toml --release --lib")

with 

    os.system("cargo rustc --target wasm32-unknown-emscripten --manifest-path core"+slashtext+"Cargo.toml --release --lib")

Build the Rust library by executing

    python3 config32.py

Then select options 1, 3, 7, 28, 30, 36, 37 and 40, which are fixed for 
the example programs.

Copy the MIRACL Core library to the current directory

    cp core/target/wasm32-unknown-emscripten/release/libcore.rlib .

Finally build one of the test programs by, for example

    rustc -O --target=wasm32-unknown-emscripten TestBLS.rs --extern core=libcore.rlib -o TestBLS.html

This will create a .html file which can be loaded into a browser as described above, and a .js file which can be run via Node.js



