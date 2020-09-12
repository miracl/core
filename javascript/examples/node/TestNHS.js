const CTX = require('../../index').CTX;

var ctx = new CTX();

var RAW=[];
var srng=new ctx.RAND(); srng.clean();
var crng=new ctx.RAND(); crng.clean();
for (i=0;i<100;i++) RAW[i]=i+1;
srng.seed(100,RAW);

for (i=0;i<100;i++) RAW[i]=i+2;
crng.seed(100,RAW);
var S=[];
var SB=[];
var UC=[];
var KEYA=[];
var KEYB=[];

ctx.NHS.SERVER_1(srng,SB,S);

ctx.NHS.CLIENT(crng,SB,UC,KEYB);

ctx.NHS.SERVER_2(S,UC,KEYA);

console.log("Alice Key= 0x"+ctx.NHS.bytestostring(KEYA));
console.log("Bob's Key= 0x"+ctx.NHS.bytestostring(KEYB));
