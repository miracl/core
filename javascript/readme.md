
Note that this code does NOT use the Javascript BigInt type, which is not constant time, and hence deprecated for use in cryptography. 

-------------------------------------------

# Installation and Testing

Suppose you want to implement ECDH with NIST256 elliptic curve. First you need to initialize the context:

    var ctx = new CTX("NIST256");

then you can call the functions as follows:

    ctx.ECDH.KEY_PAIR_GENERATE(...);
    ctx.ECDH.ECPSVDP_DH(...);

If you need to use more than one elliptic curve in the same program you only need to initialize two different contexts, for example

    var ctx1 = new CTX("NIST256");
    var ctx2 = new CTX("C25519");

The following is the list of all elliptic curves supported by MIRACL Core JS

    ['ED25519', 'C25519', 'C41417', 'GOLDILOCKS', 'X448', 'NIST256', 'SECP160R1', 'C1174', 'C1665', 'MDC', 'NIST384','NIST521', 'BRAINPOOL', 'ANSSI', 'HIFIVE', 'JUBJUB', 'NUMS256W', 'SECP256K1','NUMS256E', 'NUMS384W', 'NUMS384E', 'NUMS512W', 'NUMS512E', 'TWEEDLEDUM', 'TWEEDLEDEE', 'BN254', 'BN254CX', 'BLS12383', 'BLS12381', 'FP256BN', 'FP512BN', 'BLS12461', 'BN462', 'BLS24479', 'BLS48556', 'BLS48581', 'BLS48286'];


This library supports also RSA encryption/decryption and RSA signature. The following is a quick example on how to use RSA. First initialize the context

    var ctx = new CTX("RSA2048");

then you can call the RSA functions as follows:

    ctx.RSA.ENCRYPT(...);
    ctx.RSA.DECRYPT(...);

The following is the list of all the RSA security level supported by *MIRACL Core JS*

    ['RSA2048','RSA3072','RSA4096'];


MIRACL Core JS supports also SHA256, SHA384, SHA512, SHA3, AES-GCM encryption and a secure random number generator. Those functions are contained in every context initialized with RSA 
or with an elliptic curve. If you want to create a context supporting only those general functions then initialize it with no parameter as follows:


    var ctx = new CTX();

-------------------------------------------

To use this library in Node.js import it as follows:

```javascript
var CTX = require('./index.js');
```

It is also possible to use it as a npm package. Install it locally into your project with:

    npm install --save <path-to-this-library>

As an example in React it is possible to install the library with npm and import it with

```javascript
const ctx = require('miracl-core-js').CTX;
```
After the import the library is used as described above.

--------------------------------------


To see some running examples using your favourite browser, navigate to the examples/browser folder, and load for example TestECC.html

Alternatively if using node.js navigate to the examples/node folder, and execute for example

	node BenchBN254.js

Note that the optimizer does not like context changes, so the individual benchmarks run faster.

You might have to wait a few seconds for the benchmarking scripts to complete.

For TestMPIN the correct PIN number is 1234.

