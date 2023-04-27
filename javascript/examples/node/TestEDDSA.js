const CTX = require('../../index').CTX;

function prehashSHA2(M)  {
    var sh = new ctx.HASH512();
    for (var i=0;i<M.length;i++ )
        sh.process(M[i]);
    return sh.hash();
}

function prehashSHA3(M)  {
    var digest = [];
    var sh=new ctx.SHA3(ctx.SHA3.SHAKE256);
    for (var i=0;i<M.length;i++ )
        sh.process(M[i]);
    sh.shake(digest,64);
    return digest;
}

// ED25519 context
function ED25519() {
	var ctx = new CTX('ED25519');
	console.log("ED25519 Curve");

	var RAW=[];
	var rng=new ctx.RAND();
	rng.clean();
	for (i=0;i<100;i++) RAW[i]=i;
	rng.seed(100,RAW);

	var D=ctx.EDDSA.stringtobytes("c5aa8df43f9f837bedb7442f31dcb7b166d38535076f094b85ce3a2e0b4458f7");
	var Q=[];
	var prehash=false;

// Generate Key pair S/W 
	ctx.EDDSA.KEY_PAIR_GENERATE(null,D,Q); 

	console.log("Private key= 0x"+ctx.EDDSA.bytestostring(D));
	console.log("Public key= 0x"+ctx.EDDSA.bytestostring(Q));

	var M=[0xAF,0x82];
	var SIG=[];

    if (prehash) {
        var HM = prehashSHA2(M);
        ctx.EDDSA.SIGNATURE(true,D, null,HM, SIG);
    } else {
        ctx.EDDSA.SIGNATURE(false,D, null,M, SIG);
    }
    console.log("Signature= 0x"+ctx.EDDSA.bytestostring(SIG));

    var verify;
    if (prehash) {
        var HM = prehashEDDSA(M);
        verify=ctx.EDDSA.VERIFY(true,Q, null ,HM, SIG);
    } else {
        verify=ctx.EDDSA.VERIFY(false,Q, null ,M, SIG);
    }
    if (verify)
        console.log("Signature is verified");
    else
        console.log("Signature is NOT verified");
}

// ED448 context
function ED448() {
	var ctx = new CTX('ED448');
	console.log("ED448 Curve");

	var RAW=[];
	var rng=new ctx.RAND();
	rng.clean();
	for (i=0;i<100;i++) RAW[i]=i;
	rng.seed(100,RAW);

	var D=ctx.EDDSA.stringtobytes("c4eab05d357007c632f3dbb48489924d552b08fe0c353a0d4a1f00acda2c463afbea67c5e8d2877c5e3bc397a659949ef8021e954e0a12274e");
	var Q=[];
	var prehash=false;

// Generate Key pair S/W 
	ctx.EDDSA.KEY_PAIR_GENERATE(null,D,Q); 

	console.log("Private key= 0x"+ctx.EDDSA.bytestostring(D));
	console.log("Public key= 0x"+ctx.EDDSA.bytestostring(Q));

	var M=[0x03];
	var SIG=[];
	var CONTEXT=[];

    if (prehash) {
        var HM = prehashSHA3(M);
        ctx.EDDSA.SIGNATURE(true,D, CONTEXT,HM, SIG);
    } else {
        ctx.EDDSA.SIGNATURE(false,D, CONTEXT,M, SIG);
    }
    console.log("Signature= 0x"+ctx.EDDSA.bytestostring(SIG));

    var verify;
    if (prehash) {
        var HM = prehashEDDSA(M);
        verify=ctx.EDDSA.VERIFY(true,Q, CONTEXT ,HM, SIG);
    } else {
        verify=ctx.EDDSA.VERIFY(false,Q, CONTEXT ,M, SIG);
    }
    if (verify)
        console.log("Signature is verified");
    else
        console.log("Signature is NOT verified");
}

ED25519();
console.log("\n");
ED448();
console.log("\n");

