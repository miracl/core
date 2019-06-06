Namespaces are used to separate different curves.

To build the library and see it in action, execute the python3 scripts 
config32.py or config64.py (depending on whether you want a 32 or 
64-bit build), and select the curves that you wish to support. The 
configured library can be built using maven. 

Tests will take a while to  run.

As a quick example copy to a working directory and execute

py config64.py

or perhaps

python3 config64.py

Choose options 1, 3, 7, 19, 21, 27, 28 and 30, for example.

Once the library is configured, you can compile and install with maven:

cd core
mvn clean install

Testing will be carried out during the installation process.

Elliptic curve key exchange, signature and encryption (ECDH, ECDSA and ECCSI) will be tested.
Also MPIN and BLS (Boneh-Lynn-Shacham) signature (using pairings)

