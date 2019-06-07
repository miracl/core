Suppose you want to implement ECDH with NIST256 elliptic curve. First you need to initialize the context:

```
var ctx = new CTX("NIST256");
```
then you can call the functions as follows:
```
ctx.ECDH.KEY_PAIR_GENERATE(...);
ctx.ECDH.ECPSVDP_DH(...);
```
If you need to use more than one elliptic curve in the same script you only need to initialize two different contexts, for example
```
var ctx1 = new CTX("NIST256");
var ctx2 = new CTX("C25519");
```
The following is the list of all elliptic curves supported by MCJS
```
['ED25519', 'C25519', 'C41417', 'GOLDILOCKS', 'NIST256', 'NIST384','NIST521', 'BRAINPOOL', 'ANSSI', 'HIFIVE', 'NUMS256W', 'SECP256K1'
'NUMS256E', 'NUMS384W', 'NUMS384E', 'NUMS512W', 'NUMS512E', 'BN254', 'BN254CX', 'BLS383', 'BLS381', 'FP256BN', 'FP512BN', 'BLS461', 'BN462', 'BLS24', 'BLS48', 'BLS48_581'];
```

This library supports also RSA encryption/decryption and RSA signature. The following is a quick example on how to use RSA. First initialize the context
```
var ctx = new CTX("RSA2048");
```
then you can call the RSA functions as follows:
```
ctx.RSA.ENCRYPT(...);
ctx.RSA.DECRYPT(...);
```
The following is the list of all the RSA security level supported by *MCJS*
```
['RSA2048','RSA3072','RSA4096'];
```

MCJS supports also SHA256, SHA384, SHA512, AES-GCM encryption and Marsaglia & Zaman random number generator. Those functions are contained in every context initialized with RSA or with an elliptic curve. 
If you want to create a context supporting only those general functions then initialize it with no parameter as follows:
```
var ctx = new CTX();
```

--------------------------------------


To see some running examples, load TestALL.html or TestBLS.html or TestNHS.html or BenchtestALL.html into your favourite browser.
You might have to wait a few seconds for the benchmarking scripts to complete.

For TestALL.html the correct PIN number is 1234.

