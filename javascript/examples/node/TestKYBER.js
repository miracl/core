const CTX = require('../../index').CTX;

var ctx = new CTX();
var LOOPS = 100;

var RAW=[];
var rng=new ctx.RAND(); rng.clean();
                                
for (i=0;i<100;i++) RAW[i]=i+1;
rng.seed(100,RAW);
     
var R64 = [];
var R32 = [];
var SK = [];
var PK = [];
var SS = [];
var CT = [];

console.log("\nTesting KYBER Key Exchange\n");

for (var j=0;j<LOOPS;j++) {
    for (var i=0;i<64;i++) R64[i]=rng.getByte();
    ctx.KYBER.keypair768(R64,SK,PK);

    for (var i=0;i<32;i++) R32[i]=rng.getByte();
    ctx.KYBER.encrypt768(R32,PK,SS,CT);

    console.log("Alice key= 0x"+ctx.KYBER.bytestostring(SS));

    ctx.KYBER.decrypt768(SK,CT,SS);

    console.log("Bob's key= 0x"+ctx.KYBER.bytestostring(SS));
}
