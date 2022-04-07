const CTX = require('../../index').CTX;
const {performance} = require('perf_hooks');

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

BLS12383();
console.log("\n");
