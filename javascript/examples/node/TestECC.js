const CTX = require('../../index').CTX;

// ED25519 context
function ED25519() {
	var ctx = new CTX('ED25519');
	console.log("ED25519 Curve "+  "");

	var i,res;
	var pp="M0ng00se";

	var EGS=ctx.ECDH.EGS;
	var sha=ctx.ECP.HASH_TYPE;

	var S1=[];
	var W0=[];
	var W1=[];
	var Z0=[];
	var Z1=[];
	var RAW=[];
	var SALT=[];
	var P1=[];
	var P2=[];
	var V=[];
	var M=[];
	var T=new Array(12);  // must specify required length
	var CS=[];
	var DS=[];

	var rng=new ctx.RAND();

	rng.clean();
	for (i=0;i<100;i++) RAW[i]=i;

	rng.seed(100,RAW);

	for (i=0;i<8;i++) SALT[i]=(i+1);  // set Salt

	console.log("Alice's Passphrase= " + pp );

	var PW=ctx.ECDH.asciitobytes(pp);
// private key S0 of size EGS bytes derived from Password and Salt
	var S0=ctx.HMAC.PBKDF2(ctx.HMAC.MC_SHA2, sha,PW,SALT,1000,EGS);

	console.log("Alice's private key= 0x"+ctx.ECDH.bytestostring(S0));
// Generate Key pair S/W
	ctx.ECDH.KEY_PAIR_GENERATE(null,S0,W0);

	console.log("Alice's public key= 0x"+ctx.ECDH.bytestostring(W0));

	res=ctx.ECDH.PUBLIC_KEY_VALIDATE(W0);
	if (res!=0)
		alert("ECP Public Key is invalid!");
// Random private key for other party
	ctx.ECDH.KEY_PAIR_GENERATE(rng,S1,W1);

	console.log("Servers private key= 0x"+ctx.ECDH.bytestostring(S1));
	console.log("Servers public key= 0x"+ctx.ECDH.bytestostring(W1));

	res=ctx.ECDH.PUBLIC_KEY_VALIDATE(W1);
	if (res!=0)
		alert("ECP Public Key is invalid!");


// Calculate common key using DH - IEEE 1363 method

	ctx.ECDH.ECPSVDP_DH(S0,W1,Z0,0);
	ctx.ECDH.ECPSVDP_DH(S1,W0,Z1,0);

	var same=true;
	for (i=0;i<ctx.ECDH.EFS;i++)
		if (Z0[i]!=Z1[i]) same=false;

	if (!same)
		alert("*** ECPSVDP-DH Failed");

	var KEY=ctx.HMAC.KDF2(ctx.HMAC.MC_SHA2, sha,Z0,null,ctx.ECP.AESKEY);

	console.log("Alice's ECDH Key=  0x"+ctx.ECDH.bytestostring(KEY));
	console.log("Servers ECDH Key=  0x"+ctx.ECDH.bytestostring(KEY));

	if (ctx.ECP.CURVETYPE!=ctx.ECP.MONTGOMERY)
	{
		console.log("Testing ECIES");

		P1[0]=0x0; P1[1]=0x1; P1[2]=0x2;
		P2[0]=0x0; P2[1]=0x1; P2[2]=0x2; P2[3]=0x3;

		for (i=0;i<=16;i++) M[i]=i;

		var C=ctx.ECDH.ECIES_ENCRYPT(sha,P1,P2,rng,W1,M,V,T);

		console.log("Ciphertext= ");
		console.log("V= 0x"+ctx.ECDH.bytestostring(V));
		console.log("C= 0x"+ctx.ECDH.bytestostring(C));
		console.log("T= 0x"+ctx.ECDH.bytestostring(T));


		M=ctx.ECDH.ECIES_DECRYPT(sha,P1,P2,V,C,T,S1);
		if (M.length==0)
			alert("*** ECIES Decryption Failed ");
		else console.log("Decryption succeeded");

		console.log("Message is 0x"+ctx.ECDH.bytestostring(M));

		console.log("Testing ECDSA");

		if (ctx.ECDH.ECPSP_DSA(sha,rng,S0,M,CS,DS)!=0)
			alert("***ECDSA Signature Failed");

		console.log("Signature= ");
		console.log("C= 0x"+ctx.ECDH.bytestostring(CS));
		console.log("D= 0x"+ctx.ECDH.bytestostring(DS));

		if (ctx.ECDH.ECPVP_DSA(sha,W0,M,CS,DS)!=0)
			alert("***ECDSA Verification Failed");
		else console.log("ECDSA Signature/Verification succeeded "+  "");
	}
}
// NIST256 context

function NIST256() {
	var ctx = new CTX('NIST256');
	

	console.log("NIST256 Curve "+  "");
	var i,res;
	var pp="M0ng00se";

	var EGS=ctx.ECDH.EGS;
	var sha=ctx.ECP.HASH_TYPE;

	var S1=[];
	var W0=[];
	var W1=[];
	var Z0=[];
	var Z1=[];
	var RAW=[];
	var SALT=[];
	var P1=[];
	var P2=[];
	var V=[];
	var M=[];
	var T=new Array(12);  // must specify required length
	var CS=[];
	var DS=[];

	var rng=new ctx.RAND();

	rng.clean();
	for (i=0;i<100;i++) RAW[i]=i;

	rng.seed(100,RAW);


	for (i=0;i<8;i++) SALT[i]=(i+1);  // set Salt

	console.log("Alice's Passphrase= " + pp );

	var PW=ctx.ECDH.asciitobytes(pp);
// private key S0 of size EGS bytes derived from Password and Salt
	var S0=ctx.HMAC.PBKDF2(ctx.HMAC.MC_SHA2, sha,PW,SALT,1000,EGS);

	console.log("Alice's private key= 0x"+ctx.ECDH.bytestostring(S0));
// Generate Key pair S/W
	ctx.ECDH.KEY_PAIR_GENERATE(null,S0,W0);

	console.log("Alice's public key= 0x"+ctx.ECDH.bytestostring(W0));

	res=ctx.ECDH.PUBLIC_KEY_VALIDATE(W0);
	if (res!=0)
		alert("ECP Public Key is invalid!");
// Random private key for other party
	ctx.ECDH.KEY_PAIR_GENERATE(rng,S1,W1);

	console.log("Servers private key= 0x"+ctx.ECDH.bytestostring(S1));
	console.log("Servers public key= 0x"+ctx.ECDH.bytestostring(W1));

	res=ctx.ECDH.PUBLIC_KEY_VALIDATE(W1);
	if (res!=0)
		alert("ECP Public Key is invalid!");


// Calculate common key using DH - IEEE 1363 method

	ctx.ECDH.ECPSVDP_DH(S0,W1,Z0,0);
	ctx.ECDH.ECPSVDP_DH(S1,W0,Z1,0);

	var same=true;
	for (i=0;i<ctx.ECDH.EFS;i++)
		if (Z0[i]!=Z1[i]) same=false;

	if (!same)
		alert("*** ECPSVDP-DH Failed");

	var KEY=ctx.HMAC.KDF2(ctx.HMAC.MC_SHA2, sha,Z0,null,ctx.ECP.AESKEY);

	console.log("Alice's ECDH Key=  0x"+ctx.ECDH.bytestostring(KEY));
	console.log("Servers ECDH Key=  0x"+ctx.ECDH.bytestostring(KEY));

	if (ctx.ECP.CURVETYPE!=ctx.ECP.MONTGOMERY)
	{
		console.log("Testing ECIES");

		P1[0]=0x0; P1[1]=0x1; P1[2]=0x2;
		P2[0]=0x0; P2[1]=0x1; P2[2]=0x2; P2[3]=0x3;

		for (i=0;i<=16;i++) M[i]=i;

		var C=ctx.ECDH.ECIES_ENCRYPT(sha,P1,P2,rng,W1,M,V,T);

		console.log("Ciphertext= ");
		console.log("V= 0x"+ctx.ECDH.bytestostring(V));
		console.log("C= 0x"+ctx.ECDH.bytestostring(C));
		console.log("T= 0x"+ctx.ECDH.bytestostring(T));


		M=ctx.ECDH.ECIES_DECRYPT(sha,P1,P2,V,C,T,S1);
		if (M.length==0)
			alert("*** ECIES Decryption Failed ");
		else console.log("Decryption succeeded");

		console.log("Message is 0x"+ctx.ECDH.bytestostring(M));

		console.log("Testing ECDSA");

		if (ctx.ECDH.ECPSP_DSA(sha,rng,S0,M,CS,DS)!=0)
			alert("***ECDSA Signature Failed");

		console.log("Signature= ");
		console.log("C= 0x"+ctx.ECDH.bytestostring(CS));
		console.log("D= 0x"+ctx.ECDH.bytestostring(DS));

		if (ctx.ECDH.ECPVP_DSA(sha,W0,M,CS,DS)!=0)
			alert("***ECDSA Verification Failed");
		else console.log("ECDSA Signature/Verification succeeded "+  "");
	}

}

// GOLDILOCKS context
function GOLDILOCKS() {
	var ctx = new CTX('GOLDILOCKS');
	

	console.log("GOLDILOCKS Curve "+  "");

	var i,res;
	var pp="M0ng00se";

	var EGS=ctx.ECDH.EGS;
	var sha=ctx.ECP.HASH_TYPE;

	var S1=[];
	var W0=[];
	var W1=[];
	var Z0=[];
	var Z1=[];

	var SALT=[];
	var P1=[];
	var P2=[];
	var V=[];
	var M=[];
	var T=new Array(12);  // must specify required length
	var CS=[];
	var DS=[];

	var RAW=[];
	var rng=new ctx.RAND();

	rng.clean();
	for (i=0;i<100;i++) RAW[i]=i;

	rng.seed(100,RAW);


	for (i=0;i<8;i++) SALT[i]=(i+1);  // set Salt

	console.log("Alice's Passphrase= " + pp );

	var PW=ctx.ECDH.asciitobytes(pp);
// private key S0 of size EGS bytes derived from Password and Salt
	var S0=ctx.HMAC.PBKDF2(ctx.HMAC.MC_SHA2, sha,PW,SALT,1000,EGS);

	console.log("Alice's private key= 0x"+ctx.ECDH.bytestostring(S0));
// Generate Key pair S/W
	ctx.ECDH.KEY_PAIR_GENERATE(null,S0,W0);

	console.log("Alice's public key= 0x"+ctx.ECDH.bytestostring(W0));

	res=ctx.ECDH.PUBLIC_KEY_VALIDATE(W0);
	if (res!=0)
		alert("ECP Public Key is invalid!");
// Random private key for other party
	ctx.ECDH.KEY_PAIR_GENERATE(rng,S1,W1);

	console.log("Servers private key= 0x"+ctx.ECDH.bytestostring(S1));
	console.log("Servers public key= 0x"+ctx.ECDH.bytestostring(W1));

	res=ctx.ECDH.PUBLIC_KEY_VALIDATE(W1);
	if (res!=0)
		alert("ECP Public Key is invalid!");


// Calculate common key using DH - IEEE 1363 method

	ctx.ECDH.ECPSVDP_DH(S0,W1,Z0,0);
	ctx.ECDH.ECPSVDP_DH(S1,W0,Z1,0);

	var same=true;
	for (i=0;i<ctx.ECDH.EFS;i++)
		if (Z0[i]!=Z1[i]) same=false;

	if (!same)
		alert("*** ECPSVDP-DH Failed");

	var KEY=ctx.HMAC.KDF2(ctx.HMAC.MC_SHA2, sha,Z0,null,ctx.ECP.AESKEY);

	console.log("Alice's ECDH Key=  0x"+ctx.ECDH.bytestostring(KEY));
	console.log("Servers ECDH Key=  0x"+ctx.ECDH.bytestostring(KEY));

	if (ctx.ECP.CURVETYPE!=ctx.ECP.MONTGOMERY)
	{
		console.log("Testing ECIES");

		P1[0]=0x0; P1[1]=0x1; P1[2]=0x2;
		P2[0]=0x0; P2[1]=0x1; P2[2]=0x2; P2[3]=0x3;

		for (i=0;i<=16;i++) M[i]=i;

		var C=ctx.ECDH.ECIES_ENCRYPT(sha,P1,P2,rng,W1,M,V,T);

		console.log("Ciphertext= ");
		console.log("V= 0x"+ctx.ECDH.bytestostring(V));
		console.log("C= 0x"+ctx.ECDH.bytestostring(C));
		console.log("T= 0x"+ctx.ECDH.bytestostring(T));


		M=ctx.ECDH.ECIES_DECRYPT(sha,P1,P2,V,C,T,S1);
		if (M.length==0)
			alert("*** ECIES Decryption Failed ");
		else console.log("Decryption succeeded");

		console.log("Message is 0x"+ctx.ECDH.bytestostring(M));

		console.log("Testing ECDSA");

		if (ctx.ECDH.ECPSP_DSA(sha,rng,S0,M,CS,DS)!=0)
			alert("***ECDSA Signature Failed");

		console.log("Signature= ");
		console.log("C= 0x"+ctx.ECDH.bytestostring(CS));
		console.log("D= 0x"+ctx.ECDH.bytestostring(DS));

		if (ctx.ECDH.ECPVP_DSA(sha,W0,M,CS,DS)!=0)
			alert("***ECDSA Verification Failed");
		else console.log("ECDSA Signature/Verification succeeded "+  "");
	}
}


// Test RSA
// test driver and function exerciser for RSA API Functions


// RSA2048 context
function RSA2048() {
	var ctx = new CTX('RSA2048');

	var i,j=0;

	var sha=ctx.RSA.HASH_TYPE;

	var message="Hello World\n";

	var pub=new ctx.rsa_public_key(ctx.FF.FFLEN);
	var priv=new ctx.rsa_private_key(ctx.FF.HFLEN);

	var ML=[];
	var C=[];
	var S=[];

	var RAW=[];
	var rng=new ctx.RAND();

	rng.clean();
	for (i=0;i<100;i++) RAW[i]=i;

	rng.seed(100,RAW);

	

	var start,end,time;
	start=new Date().getTime();
	console.log("Generating RSA public/private key pair (slow!)  ");
	ctx.RSA.KEY_PAIR(rng,65537,priv,pub);
	end=new Date().getTime();
	time=end-start;
	console.log("Time in ms= "+time+"");

	var M=ctx.RSA.stringtobytes(message);
	console.log("Encrypting test string ");

	var E=ctx.HMAC.OAEP_ENCODE(sha,M,rng,null,ctx.RSA.RFS); // OAEP encode message m to e
	console.log("Encoding= 0x" + ctx.RSA.bytestohex(E) );

	console.log("Public key= 0x"+pub.n.toString() );

	start=new Date().getTime();
	ctx.RSA.ENCRYPT(pub,E,C);     // encrypt encoded message
	end=new Date().getTime();
	time=end-start;
	console.log("Time in ms= "+time+"");

	console.log("Ciphertext= 0x" + ctx.RSA.bytestohex(C) );

	console.log("Decrypting test string ");
	start=new Date().getTime();
	ctx.RSA.DECRYPT(priv,C,ML);
	end=new Date().getTime();
	time=end-start;
	console.log("Time in ms= "+time+"");

	var cmp=true;
	if (E.length!=ML.length) cmp=false;
	else
	{
		for (var j=0;j<E.length;j++)
			if (E[j]!=ML[j]) cmp=false;
	}
	if (cmp) console.log("Decryption is OK ");
	else console.log("Decryption Failed ");

	var MS=ctx.HMAC.OAEP_DECODE(sha,null,ML,ctx.RSA.RFS); // OAEP decode message
	console.log("Decoding= 0x" + ctx.RSA.bytestohex(MS) );

	console.log("message= "+ctx.RSA.bytestostring(MS) );

    var T=ctx.HMAC.PSS_ENCODE(sha,M,rng,ctx.RSA.RFS);
    if (ctx.HMAC.PSS_VERIFY(sha,M,T))
        console.log("PSS Encoding OK");
    else
        console.log("PSS Encoding FAILED");

	console.log("Signing message ");
	ctx.HMAC.PKCS15(sha,M,C,ctx.RSA.RFS);

	ctx.RSA.DECRYPT(priv,C,S); // create signature in S

	console.log("Signature= 0x" + ctx.RSA.bytestohex(S) );

	ctx.RSA.ENCRYPT(pub,S,ML);

	cmp=true;
	if (C.length!=ML.length) cmp=false;
	else
	{
		for (var j=0;j<C.length;j++)
			if (C[j]!=ML[j]) cmp=false;
	}
	if (cmp) console.log("Signature is valid ");
	else console.log("Signature is INVALID ");

	ctx.RSA.PRIVATE_KEY_KILL(priv);
}

ED25519();
console.log("\n");
NIST256();
console.log("\n");
GOLDILOCKS();
console.log("\n");
RSA2048();
console.log("\n");
