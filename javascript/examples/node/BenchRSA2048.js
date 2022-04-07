const CTX = require('../../index').CTX;
const {performance} = require('perf_hooks');

// RSA2048 context
	function rsa2048() {
		var ctx = new CTX('RSA2048');
		console.log("Generating 2048 bit RSA public/private key pair");
		var fail=false;
		var RAW=[];

		var rng=new ctx.RAND();

		rng.clean();
		for (var i=0;i<100;i++) RAW[i]=i;

		rng.seed(100,RAW);

		var pub=new ctx.rsa_public_key(ctx.FF.FFLEN);
		var priv=new ctx.rsa_private_key(ctx.FF.HFLEN);

		var M=[];
		var C=[];
		var P=[];

		var MIN_ITERS=1;
		var MIN_TIME=3;
		var elapsed;
		var start = performance.now();
		var iterations=0;
		do {
			ctx.RSA.KEY_PAIR(rng,65537,priv,pub);
			iterations++;
			elapsed=(performance.now()-start);
		} while (elapsed<MIN_TIME*1000 || iterations<MIN_ITERS);
		var dur=elapsed/iterations;
		console.log("RSA gen - " + iterations + " iteration  "+dur.toFixed(2) + " ms per iteration");

		MIN_ITERS=5;

		for (i=0;i<ctx.RSA.RFS;i++) M[i]=(i%128);

		start = performance.now();
		iterations=0;
		do {
			ctx.RSA.ENCRYPT(pub,M,C);
			iterations++;
			elapsed=(performance.now()-start);
		} while (elapsed<MIN_TIME*1000 || iterations<MIN_ITERS);
		dur=elapsed/iterations;
		console.log("RSA enc - " + iterations + " iterations  "+dur.toFixed(2) + " ms per iteration");

		start = performance.now();
		iterations=0;
		do {
			ctx.RSA.DECRYPT(priv,C,P);
			iterations++;
			elapsed=(performance.now()-start);
		} while (elapsed<MIN_TIME*1000 || iterations<MIN_ITERS);
		dur=elapsed/iterations;
		console.log("RSA dec - " + iterations + " iterations  "+dur.toFixed(2) + " ms per iteration");

		var cmp=true;
		for (i=0;i<ctx.RSA.RFS;i++)
		{
			if (P[i]!=M[i]) cmp=false;
		}

		if (!cmp)
		{
			alert("FAILURE - RSA decryption");
			fail=true;
		}
		if (!fail) console.log("All tests pass");
	}

rsa2048();
console.log("\n");
