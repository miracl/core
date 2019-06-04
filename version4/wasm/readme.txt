
AMCL in Webassembly 

See https://webassembly.org/getting-started/developers-guide/


The AMCL library already has a Javascript version, but can also run up 
to 20 times faster in a browser that supports Webassembly. And thats
most of the popular browsers in use today (Firefox, Safari, Edge, Chrome)

The C, C++ or Rust version of the AMCL library can be compiled to a 
bitcode that runs directly in the browser, by-passing Javascript 
entirely. Which is good for our type of application, as the way
in which Javascript handles integer arithmetic is very slow.

To install the Emscripten C/C++ compiler follow the instructions
above. 

First make sure the environment is set up via something like

$ source ./emsdk_env.sh --build=Release

--------------------------------------------------

C installation



Copy the AMCL C code into a new directory, along with the config32.py file 
from this directory. For a 32-bit build in the new directory execute

python3 config32.py

Then select options 1, 3, 7, 19, 21, 27, 28 and 30, which are fixed for 
the example programs.

Build the test programs with

emcc -O2 benchtest_all.c amcl.a -s WASM=1 -o benchtest_all.html

emcc -O2 testall.c amcl.a -s WASM=1 -o testall.html

emcc -O2 testbls.c amcl.a -s WASM=1 -o testbls.html

Then run a local HTML server (as described in the link above) and load the 
HTML file.

Wait for programs to complete (which will take a while).


-------------------------------

Rust Installation

Webassembly can also be generated from the Rust code. First the Rust compiler 
must be updated to target wasm, by

rustup target add wasm32-unknown-emscripten

The Emscripten toolchain is also required, as above

Copy the AMCL Rust code into a new directory. 

Edit config32.py and replace the line

os.system("cargo rustc --manifest-path amcl"+slashtext+"Cargo.toml --release --lib")

with 

os.system("cargo rustc --target wasm32-unknown-emscripten --manifest-path amcl"+slashtext+"Cargo.toml --release --lib")

Build the Rust library by executing

python3 config32.py

Then select options 1, 3, 7, 19, 21, 27, 28 and 30, which are fixed for 
the example programs.

Copy the AMCL library to the current directory

cp amcl/target/wasm32-unknown-emscripten/release/libamcl.rlib .

Finally build one of the test programs by, for example

rustc -O --target=wasm32-unknown-emscripten TestBLS.rs --extern amcl=libamcl.rlib -o TestBLS.html

This will create a HTML file, which can be loaded into a browser as 
described above.


