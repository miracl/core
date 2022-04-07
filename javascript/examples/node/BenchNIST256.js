const CTX = require('../../index').CTX;
const {performance} = require('perf_hooks');

// NIST256 context
	function NIST256() {
		var ctx = new CTX('NIST256');
		var i;
		var MIN_ITERS=5;
		var MIN_TIME=3;
		var fail=false;
		var RAW=[];

		var rng=new ctx.RAND();

		rng.clean();
		for (i=0;i<100;i++) RAW[i]=i;

		rng.seed(100,RAW);

		console.log("Curve NIST256");
		if (ctx.ECP.CURVETYPE==ctx.ECP.WEIERSTRASS)
		{
			console.log("Weierstrass parameterization ");
		}
		if (ctx.ECP.CURVETYPE==ctx.ECP.EDWARDS)
		{
			console.log("Edwards parameterization");
		}
		if (ctx.ECP.CURVETYPE==ctx.ECP.MONTGOMERY)
		{
			console.log("Montgomery parameterization");
		}

		if (ctx.FP.MODTYPE==ctx.FP.PSEUDO_MERSENNE)
		{
			console.log("Pseudo-Mersenne Modulus");
		}
		if (ctx.FP.MODTYPE==ctx.FP.MONTGOMERY_FRIENDLY)
		{
			console.log("Montgomery friendly Modulus");
		}
		if (ctx.FP.MODTYPE==ctx.FP.GENERALISED_MERSENNE)
		{
			console.log("Generalised-Mersenne Modulus");
		}
		if (ctx.FP.MODTYPE==ctx.FP.NOT_SPECIAL)
		{
			console.log("Not special Modulus");
		}

		console.log("Modulus size "+ctx.FP.MODBITS+ " bits");

		var r,s;
		var G,WP;

		G=ctx.ECP.generator();

		r=new ctx.BIG(0); r.rcopy(ctx.ROM_CURVE.CURVE_Order);
		s=ctx.BIG.randtrunc(r,16*ctx.ECP.AESKEY,rng);

		var rw=ctx.FP.rand(rng);
		WP=ctx.ECP.map2point(rw);
		if (WP.is_infinity())
		{
			alert("HASHING FAILURE - P=O");
			fail=true;
		}

		WP=G.mul(r);
		if (!WP.is_infinity())
		{
			alert("FAILURE - rG!=O");
			fail=true;
		}
		var elapsed;
		var start = performance.now();
		var iterations=0;
		do {
			WP=G.mul(s);
			iterations++;
			elapsed=(performance.now()-start);
		} while (elapsed<MIN_TIME*1000 || iterations<MIN_ITERS);
		var dur=elapsed/iterations;
		console.log("EC  mul - " + iterations + " iterations  "+dur.toFixed(2) + " ms per iteration");
		if (!fail) console.log("All tests pass");
	}

NIST256();
console.log("\n");
