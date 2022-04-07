const CTX = require('../../index').CTX;

// BN254 context
function BN254() {
	var ctx = new CTX('BN254');
	

	console.log("BN254 Pairing-Friendly Curve "+  "");

	var i,res;

	var S=[];
	var W=[];
	var SIG=[];

	var RAW=[];
	var rng=new ctx.RAND();

	rng.clean();
	for (i=0;i<100;i++) RAW[i]=i;

	rng.seed(100,RAW);

	var IKM=[];
	for (i=0;i<32;i++)
		//IKM[i]=i+1;
		IKM[i]=rng.getByte();

	var res = ctx.BLS.init();
	if (res!=0)
	{
		console.log("Failed to initialize" );
		return;
	}

	console.log ("Testing BLS code ");
	var mess="This is a test message";
	res=ctx.BLS.KeyPairGenerate(IKM,S,W);
	if (res!=0)
	{
		console.log("Failed to Generate Keys" );
		return;
	}
	console.log("Private key : 0x"+ctx.BLS.bytestostring(S) );
	console.log("Public  key : 0x"+ctx.BLS.bytestostring(W) );

	ctx.BLS.core_sign(SIG,ctx.BLS.asciitobytes(mess),S);
	console.log("Signature : 0x"+ctx.BLS.bytestostring(SIG) );

	res=ctx.BLS.core_verify(SIG,ctx.BLS.asciitobytes(mess),W);

	if (res==0)
		console.log("Signature is OK" );
	else
		console.log("Signature is *NOT* OK"  );

}

// BLS383 context
function BLS12383() {
	var ctx = new CTX('BLS12383');
	

	console.log("BLS12 Pairing-Friendly Curve "+  "");

	var i,res;

	var S=[];
	var W=[];
	var SIG=[];

	var RAW=[];
	var rng=new ctx.RAND();

	rng.clean();
	for (i=0;i<100;i++) RAW[i]=i;

	rng.seed(100,RAW);

	var IKM=[];
	for (i=0;i<32;i++)
		//IKM[i]=i+1;
		IKM[i]=rng.getByte();

	var res=ctx.BLS.init();
	if (res!=0)
	{
		console.log("Failed to initialize" );
		return;
	}

	console.log ("Testing BLS code ");
	var mess="This is a test message";

	res=ctx.BLS.KeyPairGenerate(IKM,S,W);
	if (res!=0)
	{
		console.log("Failed to Generate Keys" );
		return;
	}
	console.log("Private key : 0x"+ctx.BLS.bytestostring(S) );
	console.log("Public  key : 0x"+ctx.BLS.bytestostring(W) );

	ctx.BLS.core_sign(SIG,ctx.BLS.asciitobytes(mess),S);
	console.log("Signature : 0x"+ctx.BLS.bytestostring(SIG) );

	res=ctx.BLS.core_verify(SIG,ctx.BLS.asciitobytes(mess),W);

	if (res==0)
		console.log("Signature is OK" );
	else
		console.log("Signature is *NOT* OK"  );
 
}

// BLS24 context
function BLS24479() {
	var ctx = new CTX('BLS24479');
	

	console.log("BLS24 Pairing-Friendly Curve "+  "");

	var i,res;

	var S=[];
	var W=[];
	var SIG=[];

	var RAW=[];
	var rng=new ctx.RAND();

	rng.clean();
	for (i=0;i<100;i++) RAW[i]=i;

	rng.seed(100,RAW);

	var IKM=[];
	for (i=0;i<48;i++)
		//IKM[i]=i+1;
		IKM[i]=rng.getByte();

	var res=ctx.BLS192.init();
	if (res!=0)
	{
		console.log("Failed to initialize" );
		return;
	}

	console.log ("Testing BLS code ");
	var mess="This is a test message";

	res=ctx.BLS192.KeyPairGenerate(IKM,S,W);
	if (res!=0)
	{
		console.log("Failed to Generate Keys" );
		return;
	}
	console.log("Private key : 0x"+ctx.BLS192.bytestostring(S) );
	console.log("Public  key : 0x"+ctx.BLS192.bytestostring(W) );

	ctx.BLS192.core_sign(SIG,ctx.BLS192.asciitobytes(mess),S);
	console.log("Signature : 0x"+ctx.BLS192.bytestostring(SIG) );

	res=ctx.BLS192.core_verify(SIG,ctx.BLS192.asciitobytes(mess),W);

	if (res==0)
		console.log("Signature is OK" );
	else
		console.log("Signature is *NOT* OK"  );
 
}

// BLS48 context
function BLS48556() {
	var ctx = new CTX('BLS48556');
	

	console.log("BLS48 Pairing-Friendly Curve "+  "");

	var i,res;

	var S=[];
	var W=[];
	var SIG=[];

	var RAW=[];
	var rng=new ctx.RAND();

	rng.clean();
	for (i=0;i<100;i++) RAW[i]=i;

	rng.seed(100,RAW);

	var IKM=[];
	for (i=0;i<64;i++)
		//IKM[i]=i+1;
		IKM[i]=rng.getByte();

	var res=ctx.BLS256.init();
	if (res!=0)
	{
		console.log("Failed to initialize" );
		return;
	}

	console.log ("Testing BLS code ");
	var mess="This is a test message";

	res=ctx.BLS256.KeyPairGenerate(IKM,S,W);
	if (res!=0)
	{
		console.log("Failed to Generate Keys" );
		return;
	}
	console.log("Private key : 0x"+ctx.BLS256.bytestostring(S) );
	console.log("Public  key : 0x"+ctx.BLS256.bytestostring(W) );

	ctx.BLS256.core_sign(SIG,ctx.BLS256.asciitobytes(mess),S);
	console.log("Signature : 0x"+ctx.BLS256.bytestostring(SIG) );

	res=ctx.BLS256.core_verify(SIG,ctx.BLS256.asciitobytes(mess),W);

	if (res==0)
		console.log("Signature is OK" );
	else
		console.log("Signature is *NOT* OK"  );
 
}

BN254();
console.log("\n");
BLS12383();
console.log("\n");
BLS24479();
console.log("\n");
BLS48556();
console.log("\n");
