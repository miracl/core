const CTX = require('../../index').CTX;
const {performance} = require('perf_hooks');

function ED25519() {
		var ctx = new CTX('ED25519');
		var i;
		var MIN_ITERS=5;
		var MIN_TIME=3;
		var fail=false;
		var RAW=[];

		var rng=new ctx.RAND();

		rng.clean();
		for (i=0;i<100;i++) RAW[i]=i;

		rng.seed(100,RAW);

		console.log("Curve ED25519");
		if (ctx.ECP.CURVETYPE==ctx.ECP.WEIERSTRASS)
		{
			console.log("Weierstrass parameterization");
		}
		if (ctx.ECP.CURVETYPE==ctx.ECP.EDWARDS)
		{
			console.log("Edwards parameterization");
		}
		if (ctx.ECP.CURVETYPE==ctx.ECP.MONTGOMERY)
		{
			console.log("Motngomery parameterization");
		}

		if (ctx.FP.MODTYPE==ctx.FP.PSEUDO_MERSENNE)
		{
			console.log("Pseudo-Mersenne Modulus");
		}
		if (ctx.FP.MODTYPE==ctx.FP.MONTGOMERY_FRIENDLY)
		{
			console.log("Montgomery firendly Modulus");
		}
		if (ctx.FP.MODTYPE==ctx.FP.GENERALISED_MERSENNE)
		{
			console.log("Generalised-Mersenne Modulus");
		}
		if (ctx.FP.MODTYPE==ctx.FP.NOT_SPECIAL)
		{
			console.log("Not special Modulus");
		}

		console.log("Modulus size " + ctx.FP.MODBITS + " bits");

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
		console.log("EC mul - " + iterations + " iterations "+dur.toFixed(2) + " ms per iteration");
		if (!fail) console.log("All tests pass");
	}

// BLS12383 context

	function BLS12383() {
		var ctx = new CTX('BLS12383');
		var i;
		var MIN_ITERS=5;
		var MIN_TIME=3;
		var G,r,s;
		var Q,W;
		var RAW=[];
		var fail = false;

		var rng=new ctx.RAND();

		rng.clean();
		for (i=0;i<100;i++) RAW[i]=i;

		rng.seed(100,RAW);

		console.log("Curve BLS12383");
		if (ctx.ECP.CURVE_PAIRING_TYPE==ctx.ECP.BN)
		{
			console.log("BN Pairing-Friendly Curve");
		}
		if (ctx.ECP.CURVE_PAIRING_TYPE==ctx.ECP.BLS)
		{
			console.log("BLS Pairing-Friendly Curve");
		}

		console.log("Modulus size " + ctx.FP.MODBITS + " bits");

		G=ctx.ECP.generator();

		r=new ctx.BIG(0); r.rcopy(ctx.ROM_CURVE.CURVE_Order);
		s=ctx.BIG.randtrunc(r,16*ctx.ECP.AESKEY,rng);

		var rw=ctx.FP.rand(rng);
		var WP=ctx.ECP.map2point(rw);
		if (WP.is_infinity())
		{
			alert("HASHING FAILURE - P=O");
			fail=true;
		}


		var P=ctx.PAIR.G1mul(G,r);

		if (!P.is_infinity())
		{
			alert("FAILURE - rP!=O");
			fail=true;
		}

		var elapsed;
		var start = performance.now();
		var iterations=0;
		do {
			P=ctx.PAIR.G1mul(G,s);
			iterations++;
			elapsed=(performance.now()-start);
		} while (elapsed<MIN_TIME*1000 || iterations<MIN_ITERS);
		var dur=elapsed/iterations;
		console.log("G1 mul -                " + iterations + " iterations  "+ dur.toFixed(2) + " ms per iteration");

		Q=ctx.ECP2.generator();

		var rw=ctx.FP2.rand(rng);
        W = ctx.ECP2.map2point(rw);
        W.cfp();
        if (W.is_infinity()) {
            alert("HASHING FAILURE - P=O");
			fail=true;
        }
        W = ctx.PAIR.G2mul(W, r);
        if (!W.is_infinity()) {
            alert("FAILURE - rQ!=O");
			fail=true;
        }

		W=ctx.PAIR.G2mul(Q,r);

		if (!W.is_infinity())
		{
			alert("FAILURE - rQ!=O");
			fail=true;
		}

		start = performance.now();
		iterations=0;
		do {
			W=ctx.PAIR.G2mul(Q,s);
			iterations++;
			elapsed=(performance.now()-start);
		} while (elapsed<MIN_TIME*1000 || iterations<MIN_ITERS);
		dur=elapsed/iterations;
		console.log("G2 mul -                " + iterations + " iterations "+ dur.toFixed(2) + " ms per iteration");

		var w=ctx.PAIR.ate(Q,P);
		w=ctx.PAIR.fexp(w);

		var g=ctx.PAIR.GTpow(w,r);

		if (!g.isunity())
		{
			alert("FAILURE - g^r!=1");
			fail=true;
		}

		start = performance.now();
		iterations=0;
		do {
			g=ctx.PAIR.GTpow(w,s);
			iterations++;
			elapsed=(performance.now()-start);
		} while (elapsed<MIN_TIME*1000 || iterations<MIN_ITERS);
		dur=elapsed/iterations;
		console.log("GT pow -                " + iterations + " iterations "+dur.toFixed(2) + " ms per iteration");

		var cr;
		start = performance.now();
		iterations=0;
		do {
			cr=w.compow(s,r);
			iterations++;
			elapsed=(performance.now()-start);
		} while (elapsed<MIN_TIME*1000 || iterations<MIN_ITERS);
		dur=elapsed/iterations;
		console.log("GT pow (compressed) -   " + iterations + " iterations "+dur.toFixed(2) + " ms per iteration");

		start = performance.now();
		iterations=0;
		do {
			w=ctx.PAIR.ate(Q,P);
			iterations++;
			elapsed=(performance.now()-start);
		} while (elapsed<MIN_TIME*1000 || iterations<MIN_ITERS);
		dur=elapsed/iterations;
		console.log("PAIRing ATE   -         " + iterations + " iterations "+dur.toFixed(2) + " ms per iteration");

		start = performance.now();
		iterations=0;
		do {
			g=ctx.PAIR.fexp(w);
			iterations++;
			elapsed=(performance.now()-start);
		} while (elapsed<MIN_TIME*1000 || iterations<MIN_ITERS);
		dur=elapsed/iterations;
		console.log("PAIRing FEXP -          " + iterations + " iterations "+dur.toFixed(2) + " ms per iteration");

		P.copy(G);
		Q.copy(W);

		P=ctx.PAIR.G1mul(P,s);
		g=ctx.PAIR.ate(Q,P);
		g=ctx.PAIR.fexp(g);

		P.copy(G);
		Q=ctx.PAIR.G2mul(Q,s);
		w=ctx.PAIR.ate(Q,P);
		w=ctx.PAIR.fexp(w);

        if (!ctx.PAIR.G1member(P))
        {
            alert("FAILURE - P not in G1 ");
			fail=true;
        }

        if (!ctx.PAIR.G2member(Q))
        {
            alert("FAILURE - Q not in G2 ");
			fail=true;
        }

        if (!ctx.PAIR.GTmember(w))
        {
            alert("FAILURE - e(Q,P) not in GT ");
			fail=true;
        }

		if (!g.equals(w))
		{
			alert("FAILURE - e(sQ,p)!=e(Q,sP)");
			fail=true;
		}

		Q.copy(W);
		g=ctx.PAIR.ate(Q,P);
		g=ctx.PAIR.fexp(g);
		g=ctx.PAIR.GTpow(g,s);

		if (!g.equals(w))
		{
			alert("FAILURE - e(sQ,p)!=e(Q,P)^s");
			fail=true;
		}
		if (!fail) console.log("All tests pass");
	}

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


// GOLDILOCKS context
	function GOLDILOCKS() {

		var ctx = new CTX('GOLDILOCKS');
		var i;
		var MIN_ITERS=5;
		var MIN_TIME=3;
		var fail=false;
		var RAW=[];

		var rng=new ctx.RAND();

		rng.clean();
		for (i=0;i<100;i++) RAW[i]=i;

		rng.seed(100,RAW);

		console.log("Curve GOLDILOCKS");
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
		console.log("EC  mul - " + iterations + " iterations "+dur.toFixed(2) + " ms per iteration");
		if (!fail) console.log("All tests pass");
	}


// BN254 context
	function BN254() {

		var ctx = new CTX('BN254');
		var i;
		var MIN_ITERS=5;
		var MIN_TIME=3;
		var G,r,s;
		var Q,W;
		var RAW=[];
		var fail = false;

		var rng=new ctx.RAND();

		rng.clean();
		for (i=0;i<100;i++) RAW[i]=i;

		rng.seed(100,RAW);

		console.log("Curve BN254");
		if (ctx.ECP.CURVE_PAIRING_TYPE==ctx.ECP.BN)
		{
			console.log("BN Pairing-Friendly Curve");
		}
		if (ctx.ECP.CURVE_PAIRING_TYPE==ctx.ECP.BLS)
		{
			console.log("BLS Pairing-Friendly Curve");
		}

		console.log("Modulus size " + ctx.FP.MODBITS + " bits");

		G=ctx.ECP.generator();

		r=new ctx.BIG(0); r.rcopy(ctx.ROM_CURVE.CURVE_Order);
		s=ctx.BIG.randtrunc(r,16*ctx.ECP.AESKEY,rng);

		var rw=ctx.FP.rand(rng);
		var WP=ctx.ECP.map2point(rw);
		if (WP.is_infinity())
		{
			alert("HASHING FAILURE - P=O");
			fail=true;
		}

		var P=ctx.PAIR.G1mul(G,r);

		if (!P.is_infinity())
		{
			alert("FAILURE - rP!=O");
			fail=true;
		}

		var elapsed;
		var start = performance.now();
		var iterations=0;
		do {
			P=ctx.PAIR.G1mul(G,s);
			iterations++;
			elapsed=(performance.now()-start);
		} while (elapsed<MIN_TIME*1000 || iterations<MIN_ITERS);
		var dur=elapsed/iterations;
		console.log("G1 mul -                " + iterations + " iterations "+dur.toFixed(2) + " ms per iteration");

		Q=ctx.ECP2.generator();

		var rw=ctx.FP2.rand(rng);
        W = ctx.ECP2.map2point(rw);
        W.cfp();
        if (W.is_infinity()) {
            alert("HASHING FAILURE - P=O");
			fail=true;
        }
        W = ctx.PAIR.G2mul(W, r);
        if (!W.is_infinity()) {
            alert("FAILURE - rQ!=O");
			fail=true;
        }

		W=ctx.PAIR.G2mul(Q,r);

		if (!W.is_infinity())
		{
			alert("FAILURE - rQ!=O");
			fail=true;
		}

		start = performance.now();
		iterations=0;
		do {
			W=ctx.PAIR.G2mul(Q,s);
			iterations++;
			elapsed=(performance.now()-start);
		} while (elapsed<MIN_TIME*1000 || iterations<MIN_ITERS);
		dur=elapsed/iterations;
		console.log("G2 mul -                " + iterations + " iterations "+dur.toFixed(2) + " ms per iteration");

		var w=ctx.PAIR.ate(Q,P);
		w=ctx.PAIR.fexp(w);

		var g=ctx.PAIR.GTpow(w,r);

		if (!g.isunity())
		{
			alert("FAILURE - g^r!=1");
			fail=true;
		}

		start = performance.now();
		iterations=0;
		do {
			g=ctx.PAIR.GTpow(w,s);
			iterations++;
			elapsed=(performance.now()-start);
		} while (elapsed<MIN_TIME*1000 || iterations<MIN_ITERS);
		dur=elapsed/iterations;
		console.log("GT pow -                " + iterations + " iterations "+dur.toFixed(2) + " ms per iteration");


		var cr;
		start = performance.now();
		iterations=0;
		do {
			cr=w.compow(s,r);
			iterations++;
			elapsed=(performance.now()-start);
		} while (elapsed<MIN_TIME*1000 || iterations<MIN_ITERS);
		dur=elapsed/iterations;
		console.log("GT pow (compressed) -   " + iterations + " iterations "+dur.toFixed(2) + " ms per iteration");

		start = performance.now();
		iterations=0;
		do {
			w=ctx.PAIR.ate(Q,P);
			iterations++;
			elapsed=(performance.now()-start);
		} while (elapsed<MIN_TIME*1000 || iterations<MIN_ITERS);
		dur=elapsed/iterations;
		console.log("PAIRing ATE   -         " + iterations + " iterations "+dur.toFixed(2) + " ms per iteration");

		start = performance.now();
		iterations=0;
		do {
			g=ctx.PAIR.fexp(w);
			iterations++;
			elapsed=(performance.now()-start);
		} while (elapsed<MIN_TIME*1000 || iterations<MIN_ITERS);
		dur=elapsed/iterations;
		console.log("PAIRing FEXP -          " + iterations + " iterations "+dur.toFixed(2) + " ms per iteration");

		P.copy(G);
		Q.copy(W);

		P=ctx.PAIR.G1mul(P,s);
		g=ctx.PAIR.ate(Q,P);
		g=ctx.PAIR.fexp(g);

		P.copy(G);
		Q=ctx.PAIR.G2mul(Q,s);
		w=ctx.PAIR.ate(Q,P);
		w=ctx.PAIR.fexp(w);

        if (!ctx.PAIR.G1member(P))
        {
            alert("FAILURE - P not in G1 ");
			fail=true;
        }

        if (!ctx.PAIR.G2member(Q))
        {
            alert("FAILURE - Q not in G2 ");
			fail=true;
        }

        if (!ctx.PAIR.GTmember(w))
        {
            alert("FAILURE - e(Q,P) not in GT ");
			fail=true;
        }

		if (!g.equals(w))
		{
			alert("FAILURE - e(sQ,p)!=e(Q,sP)");
			fail=true;
		}

		Q.copy(W);
		g=ctx.PAIR.ate(Q,P);
		g=ctx.PAIR.fexp(g);
		g=ctx.PAIR.GTpow(g,s);

		if (!g.equals(w))
		{
			alert("FAILURE - e(sQ,p)!=e(Q,P)^s");
			fail=true;
		}
		if (!fail) console.log("All tests pass");
	}

// BLS24479 context
	function BLS24479() {

		var ctx = new CTX('BLS24479');
		var i;
		var MIN_ITERS=3;
		var MIN_TIME=3;
		var G,r,s;
		var Q,W;
		var RAW=[];
		var fail = false;

		var rng=new ctx.RAND();

		rng.clean();
		for (i=0;i<100;i++) RAW[i]=i;

		rng.seed(100,RAW);

		console.log("Curve BLS24479");
		if (ctx.ECP.CURVE_PAIRING_TYPE==ctx.ECP.BN)
		{
			console.log("BN Pairing-Friendly Curve");
		}
		if (ctx.ECP.CURVE_PAIRING_TYPE==ctx.ECP.BLS)
		{
			console.log("BLS24 Pairing-Friendly Curve");
		}

		console.log("Modulus size " + ctx.FP.MODBITS + " bits");

		G=ctx.ECP.generator();

		r=new ctx.BIG(0); r.rcopy(ctx.ROM_CURVE.CURVE_Order);
		s=ctx.BIG.randtrunc(r,16*ctx.ECP.AESKEY,rng);

		var rw=ctx.FP.rand(rng);
		var WP=ctx.ECP.map2point(rw);
		if (WP.is_infinity())
		{
			alert("HASHING FAILURE - P=O");
			fail=true;
		}

		var P=ctx.PAIR4.G1mul(G,r);

		if (!P.is_infinity())
		{
			alert("FAILURE - rP!=O");
			fail=true;
		}

		var elapsed;
		var start = performance.now();
		var iterations=0;
		do {
			P=ctx.PAIR4.G1mul(G,s);
			iterations++;
			elapsed=(performance.now()-start);
		} while (elapsed<MIN_TIME*1000 || iterations<MIN_ITERS);
		var dur=elapsed/iterations;
		console.log("G1 mul -                " + iterations + " iterations "+dur.toFixed(2) + " ms per iteration");

		Q=ctx.ECP4.generator();

		var rw=ctx.FP4.rand(rng);
        W = ctx.ECP4.map2point(rw);
        W.cfp();
        if (W.is_infinity()) {
            alert("HASHING FAILURE - P=O");
			fail=true;
        }
        W = ctx.PAIR4.G2mul(W, r);
        if (!W.is_infinity()) {
            alert("FAILURE - rQ!=O");
			fail=true;
        }

		W=ctx.PAIR4.G2mul(Q,r);

		if (!W.is_infinity())
		{
			alert("FAILURE - rQ!=O");
			fail=true;
		}

		start = performance.now();
		iterations=0;
		do {
			W=ctx.PAIR4.G2mul(Q,s);
			iterations++;
			elapsed=(performance.now()-start);
		} while (elapsed<MIN_TIME*1000 || iterations<MIN_ITERS);
		dur=elapsed/iterations;
		console.log("G2 mul -                " + iterations + " iterations "+dur.toFixed(2) + " ms per iteration");

		var w=ctx.PAIR4.ate(Q,P);
		w=ctx.PAIR4.fexp(w);

		var g=ctx.PAIR4.GTpow(w,r);

		if (!g.isunity())
		{
			alert("FAILURE - g^r!=1");
			fail=true;
		}

		start = performance.now();
		iterations=0;
		do {
			g=ctx.PAIR4.GTpow(w,s);
			iterations++;
			elapsed=(performance.now()-start);
		} while (elapsed<MIN_TIME*1000 || iterations<MIN_ITERS);
		dur=elapsed/iterations;
		console.log("GT pow -                " + iterations + " iterations "+dur.toFixed(2) + " ms per iteration");

		start = performance.now();
		iterations=0;
		do {
			w=ctx.PAIR4.ate(Q,P);
			iterations++;
			elapsed=(performance.now()-start);
		} while (elapsed<MIN_TIME*1000 || iterations<MIN_ITERS);
		dur=elapsed/iterations;
		console.log("PAIRing ATE   -         " + iterations + " iterations "+dur.toFixed(2) + " ms per iteration");

		start = performance.now();
		iterations=0;
		do {
			g=ctx.PAIR4.fexp(w);
			iterations++;
			elapsed=(performance.now()-start);
		} while (elapsed<MIN_TIME*1000 || iterations<MIN_ITERS);
		dur=elapsed/iterations;
		console.log("PAIRing FEXP -          " + iterations + " iterations "+dur.toFixed(2) + " ms per iteration");

		P.copy(G);
		Q.copy(W);

		P=ctx.PAIR4.G1mul(P,s);
		g=ctx.PAIR4.ate(Q,P);
		g=ctx.PAIR4.fexp(g);

		P.copy(G);
		Q=ctx.PAIR4.G2mul(Q,s);
		w=ctx.PAIR4.ate(Q,P);
		w=ctx.PAIR4.fexp(w);

        if (!ctx.PAIR4.G1member(P))
        {
            alert("FAILURE - P not in G1 ");
			fail=true;
        }

        if (!ctx.PAIR4.G2member(Q))
        {
            alert("FAILURE - Q not in G2 ");
			fail=true;
        }

        if (!ctx.PAIR4.GTmember(w))
        {
            alert("FAILURE - e(Q,P) not in GT ");
			fail=true;
        }


		if (!g.equals(w))
		{
			alert("FAILURE - e(sQ,p)!=e(Q,sP)");
			fail=true;
		}

		Q.copy(W);
		g=ctx.PAIR4.ate(Q,P);
		g=ctx.PAIR4.fexp(g);
		g=ctx.PAIR4.GTpow(g,s);

		if (!g.equals(w))
		{
			alert("FAILURE - e(sQ,p)!=e(Q,P)^s");
			fail=true;
		}
		if (!fail) console.log("All tests pass");
	}

// BLS48556 context
	function BLS48556() {
		var ctx = new CTX('BLS48556');
		var i;
		var MIN_ITERS=1;
		var MIN_TIME=3;
		var G,r,s;
		var Q,W;
		var RAW=[];
		var fail = false;

		var rng=new ctx.RAND();

		rng.clean();
		for (i=0;i<100;i++) RAW[i]=i;

		rng.seed(100,RAW);

		console.log("Curve BLS48556");
		if (ctx.ECP.CURVE_PAIRING_TYPE==ctx.ECP.BN)
		{
			console.log("BN Pairing-Friendly Curve");
		}
		if (ctx.ECP.CURVE_PAIRING_TYPE==ctx.ECP.BLS)
		{
			console.log("BLS48 Pairing-Friendly Curve");
		}

		console.log("Modulus size " + ctx.FP.MODBITS + " bits");

		G=ctx.ECP.generator();

		r=new ctx.BIG(0); r.rcopy(ctx.ROM_CURVE.CURVE_Order);
		s=ctx.BIG.randtrunc(r,16*ctx.ECP.AESKEY,rng);

		var rw=ctx.FP.rand(rng);
		var WP=ctx.ECP.map2point(rw);
		if (WP.is_infinity())
		{
			alert("HASHING FAILURE - P=O");
			fail=true;
		}

		var P=ctx.PAIR8.G1mul(G,r);

		if (!P.is_infinity())
		{
			alert("FAILURE - rP!=O");
			fail=true;
		}

		var elapsed;
		var start = performance.now();
		var iterations=0;
		do {
			P=ctx.PAIR8.G1mul(G,s);
			iterations++;
			elapsed=(performance.now()-start);
		} while (elapsed<MIN_TIME*1000 || iterations<MIN_ITERS);
		var dur=elapsed/iterations;
		console.log("G1 mul -                " + iterations + " iterations "+dur.toFixed(2) + " ms per iteration");

		Q=ctx.ECP8.generator();

		var rw=ctx.FP8.rand(rng);
        W = ctx.ECP8.map2point(rw);
        W.cfp();
        if (W.is_infinity()) {
            alert("HASHING FAILURE - P=O");
			fail=true;
        }
        W = ctx.PAIR8.G2mul(W, r);
        if (!W.is_infinity()) {
            alert("FAILURE - rQ!=O");
			fail=true;
        }

		W=ctx.PAIR8.G2mul(Q,r);

		if (!W.is_infinity())
		{
			alert("FAILURE - rQ!=O");
			fail=true;
		}

		start = performance.now();
		iterations=0;
		do {
			W=ctx.PAIR8.G2mul(Q,s);
			iterations++;
			elapsed=(performance.now()-start);
		} while (elapsed<MIN_TIME*1000 || iterations<MIN_ITERS);
		dur=elapsed/iterations;
		console.log("G2 mul -                " + iterations + " iterations "+dur.toFixed(2) + " ms per iteration");

		var w=ctx.PAIR8.ate(Q,P);
		w=ctx.PAIR8.fexp(w);

		var g=ctx.PAIR8.GTpow(w,r);

		if (!g.isunity())
		{
			alert("FAILURE - g^r!=1");
			fail=true;
		}

		start = performance.now();
		iterations=0;
		do {
			g=ctx.PAIR8.GTpow(w,s);
			iterations++;
			elapsed=(performance.now()-start);
		} while (elapsed<MIN_TIME*1000 || iterations<MIN_ITERS);
		dur=elapsed/iterations;
		console.log("GT pow -                " + iterations + " iterations "+dur.toFixed(2) + " ms per iteration");

		start = performance.now();
		iterations=0;
		do {
			w=ctx.PAIR8.ate(Q,P);
			iterations++;
			elapsed=(performance.now()-start);
		} while (elapsed<MIN_TIME*1000 || iterations<MIN_ITERS);
		dur=elapsed/iterations;
		console.log("PAIRing ATE   -         " + iterations + " iterations "+dur.toFixed(2) + " ms per iteration");

		start = performance.now();
		iterations=0;
		do {
			g=ctx.PAIR8.fexp(w);
			iterations++;
			elapsed=(performance.now()-start);
		} while (elapsed<MIN_TIME*1000 || iterations<MIN_ITERS);
		dur=elapsed/iterations;
		console.log("PAIRing FEXP -          " + iterations + " iterations "+dur.toFixed(2) + " ms per iteration");

		P.copy(G);
		Q.copy(W);

		P=ctx.PAIR8.G1mul(P,s);
		g=ctx.PAIR8.ate(Q,P);
		g=ctx.PAIR8.fexp(g);

		P.copy(G);
		Q=ctx.PAIR8.G2mul(Q,s);
		w=ctx.PAIR8.ate(Q,P);
		w=ctx.PAIR8.fexp(w);

        if (!ctx.PAIR8.G1member(P))
        {
            alert("FAILURE - P not in G1 ");
			fail=true;
        }

        if (!ctx.PAIR8.G2member(Q))
        {
            alert("FAILURE - Q not in G2 ");
			fail=true;
        }

        if (!ctx.PAIR8.GTmember(w))
        {
            alert("FAILURE - e(Q,P) not in GT ");
			fail=true;
        }

		if (!g.equals(w))
		{
			alert("FAILURE - e(sQ,p)!=e(Q,sP)");
			fail=true;
		}

		Q.copy(W);
		g=ctx.PAIR8.ate(Q,P);
		g=ctx.PAIR8.fexp(g);
		g=ctx.PAIR8.GTpow(g,s);

		if (!g.equals(w))
		{
			alert("FAILURE - e(sQ,p)!=e(Q,P)^s");
			fail=true;
		}
		if (!fail) console.log("All tests pass");
	}

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

ED25519();
console.log("\n");
NIST256();
console.log("\n");
GOLDILOCKS();
console.log("\n");
BN254();
console.log("\n");
BLS12383();
console.log("\n");
BLS24479();
console.log("\n");
BLS48556();
console.log("\n");
rsa2048();
console.log("\n");
