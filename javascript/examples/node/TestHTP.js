const CTX = require('../../index').CTX;

function ceil(a,b) {
	return Math.floor(((a)-1)/(b)+1);
}

function asciitobytes(s) {
    var b = [],i;
    for (i = 0; i < s.length; i++) {
        b.push(s.charCodeAt(i));
    }
    return b;
}

function hash_to_field(ctx,hash,hlen,DST,M,ctr) {
	var u=[];
    var q=new ctx.BIG(0);
	q.rcopy(ctx.ROM_FIELD.Modulus);
    var k=q.nbits();
    var r=new ctx.BIG(0);
	r.rcopy(ctx.ROM_CURVE.CURVE_Order);
    var m=r.nbits();
    var L=ceil(k+ceil(m,2),8);
    var OKM = ctx.HMAC.XMD_Expand(hash,hlen,L*ctr,DST,M);
    var fd = [];
    for (var i=0;i<ctr;i++)
    {
        for (var j=0;j<L;j++)
            fd[j]=OKM[i*L+j];
        var dx=ctx.DBIG.fromBytes(fd);
        var w=new ctx.FP(dx.mod(q));
        u[i]=new ctx.FP(w);
    }
    return u;
}

function htp(ctx,mess,ro,nu,hlen) {
    var DSTRO = asciitobytes(ro);
    var DSTNU = asciitobytes(nu);
    var M = asciitobytes(mess);
	var u,P,P1;

	if (ctx.ECP.CURVETYPE != ctx.ECP.MONTGOMERY)
	{
		console.log("Random oracle - message= "+mess);

	    u=hash_to_field(ctx,ctx.HMAC.MC_SHA2,hlen,DSTRO,M,2);
		console.log("u[0]= "+u[0].toString());
		console.log("u[1]= "+u[1].toString());

		P=ctx.ECP.map2point(u[0]);
		console.log("Q[0]= "+P.toString());
		P1=ctx.ECP.map2point(u[1]);
		console.log("Q[1]= "+P1.toString());
		P.add(P1);
		P.cfp();
		P.affine();
		console.log("P= "+P.toString());
	}
    console.log("\nNon-Uniform");
    u=hash_to_field(ctx,ctx.HMAC.MC_SHA2,hlen,DSTNU,M,1);
    console.log("u[0]= "+u[0].toString());
    P=ctx.ECP.map2point(u[0]);
    console.log("Q= "+P.toString());
    P.cfp();
    P.affine();
    console.log("P= "+P.toString());
}


function hash_to_field2(ctx,hash,hlen,DST,M,ctr) {
	var u=[];
    var q=new ctx.BIG(0);
	q.rcopy(ctx.ROM_FIELD.Modulus);
    var k=q.nbits();
    var r=new ctx.BIG(0);
	r.rcopy(ctx.ROM_CURVE.CURVE_Order);
    var m=r.nbits();
    var L=ceil(k+ceil(m,2),8);
    var OKM = ctx.HMAC.XMD_Expand(hash,hlen,2*L*ctr,DST,M);
    var fd = [];
    for (var i=0;i<ctr;i++)
    {
        for (var j=0;j<L;j++)
            fd[j]=OKM[2*i*L+j];
        var dx=ctx.DBIG.fromBytes(fd);
        var w1=new ctx.FP(dx.mod(q));

        for (var j=0;j<L;j++)
            fd[j]=OKM[(2*i+1)*L+j];
        dx=ctx.DBIG.fromBytes(fd);
        var w2=new ctx.FP(dx.mod(q));

        u[i]=new ctx.FP2(w1,w2);
    }
    return u;
}

function htp2(ctx,mess,ro,nu,hlen) {
    console.log("Random oracle - message= "+mess);
    var DSTRO = asciitobytes(ro);
    var DSTNU = asciitobytes(nu);
    var M = asciitobytes(mess);

    var u=hash_to_field2(ctx,ctx.HMAC.MC_SHA2,hlen,DSTRO,M,2);
    console.log("u[0]= "+u[0].toString());
    console.log("u[1]= "+u[1].toString());

    var P=ctx.ECP2.map2point(u[0]);
    console.log("Q[0]= "+P.toString());
    var P1=ctx.ECP2.map2point(u[1]);
    console.log("Q[1]= "+P1.toString());
    P.add(P1);
    P.cfp();
    P.affine();
    console.log("P= "+P.toString());

    console.log("\nNon-Uniform");
    u=hash_to_field2(ctx,ctx.HMAC.MC_SHA2,hlen,DSTNU,M,1);
    console.log("u[0]= "+u[0].toString());
    P=ctx.ECP2.map2point(u[0]);
    console.log("Q= "+P.toString());
    P.cfp();
    P.affine();
    console.log("P= "+P.toString());
}


// ED25519 context
function ED25519() {
	var ctx = new CTX('ED25519');
	var ro="QUUX-V01-CS02-with-edwards25519_XMD:SHA-512_ELL2_RO_";
	var nu="QUUX-V01-CS02-with-edwards25519_XMD:SHA-512_ELL2_NU_";
	var hlen=64;//SHA512
	
    htp(ctx,"",ro,nu,hlen);
    htp(ctx,"abc",ro,nu,hlen);
    htp(ctx,"abcdef0123456789",ro,nu,hlen);
    htp(ctx,"q128_qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq",ro,nu,hlen);
    htp(ctx,"a512_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",ro,nu,hlen);
}

// C25519 context
function C25519() {
	var ctx = new CTX('C25519');
	var ro="QUUX-V01-CS02-with-curve25519_XMD:SHA-512_ELL2_RO_";
	var nu="QUUX-V01-CS02-with-curve25519_XMD:SHA-512_ELL2_NU_";
	var hlen=64;//SHA512
	
    htp(ctx,"",ro,nu,hlen);
    htp(ctx,"abc",ro,nu,hlen);
    htp(ctx,"abcdef0123456789",ro,nu,hlen);
    htp(ctx,"q128_qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq",ro,nu,hlen);
    htp(ctx,"a512_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",ro,nu,hlen);
}

// NIST256 context
function NIST256() {
	var ctx = new CTX('NIST256');
	var ro="QUUX-V01-CS02-with-P256_XMD:SHA-256_SSWU_RO_";
	var nu="QUUX-V01-CS02-with-P256_XMD:SHA-256_SSWU_NU_";
	var hlen=ctx.ECP.HASH_TYPE;
	
    htp(ctx,"",ro,nu,hlen);
    htp(ctx,"abc",ro,nu,hlen);
    htp(ctx,"abcdef0123456789",ro,nu,hlen);
    htp(ctx,"q128_qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq",ro,nu,hlen);
    htp(ctx,"a512_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",ro,nu,hlen);
}

// GOLDILOCKS context
function GOLDILOCKS() {
	var ctx = new CTX('GOLDILOCKS');
	var ro="QUUX-V01-CS02-with-edwards448_XMD:SHA-512_ELL2_RO_";
	var nu="QUUX-V01-CS02-with-edwards448_XMD:SHA-512_ELL2_NU_";
	var hlen=ctx.ECP.HASH_TYPE;
	
    htp(ctx,"",ro,nu,hlen);
    htp(ctx,"abc",ro,nu,hlen);
    htp(ctx,"abcdef0123456789",ro,nu,hlen);
    htp(ctx,"q128_qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq",ro,nu,hlen);
    htp(ctx,"a512_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",ro,nu,hlen);
}

// SECP256K1 context
function SECP256K1() {
	var ctx = new CTX('SECP256K1');
	var ro="QUUX-V01-CS02-with-secp256k1_XMD:SHA-256_SSWU_RO_";
	var nu="QUUX-V01-CS02-with-secp256k1_XMD:SHA-256_SSWU_NU_";
	var hlen=ctx.ECP.HASH_TYPE;
	
    htp(ctx,"",ro,nu,hlen);
    htp(ctx,"abc",ro,nu,hlen);
    htp(ctx,"abcdef0123456789",ro,nu,hlen);
    htp(ctx,"q128_qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq",ro,nu,hlen);
    htp(ctx,"a512_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",ro,nu,hlen);
}

// BLS12381 context
function BLS12381() {
	var ctx = new CTX('BLS12381');
	var ro="QUUX-V01-CS02-with-BLS12381G1_XMD:SHA-256_SSWU_RO_";
	var nu="QUUX-V01-CS02-with-BLS12381G1_XMD:SHA-256_SSWU_NU_";
	var hlen=ctx.ECP.HASH_TYPE;
	
    htp(ctx,"",ro,nu,hlen);
    htp(ctx,"abc",ro,nu,hlen);
    htp(ctx,"abcdef0123456789",ro,nu,hlen);
    htp(ctx,"q128_qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq",ro,nu,hlen);
    htp(ctx,"a512_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",ro,nu,hlen);
}

// BLS12381 context
function BLS12381_G2() {
	var ctx = new CTX('BLS12381');
	var ro="QUUX-V01-CS02-with-BLS12381G2_XMD:SHA-256_SSWU_RO_";
	var nu="QUUX-V01-CS02-with-BLS12381G2_XMD:SHA-256_SSWU_NU_";
	var hlen=ctx.ECP.HASH_TYPE;
	
    htp2(ctx,"",ro,nu,hlen);
    htp2(ctx,"abc",ro,nu,hlen);
    htp2(ctx,"abcdef0123456789",ro,nu,hlen);
    htp2(ctx,"q128_qqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqqq",ro,nu,hlen);
    htp2(ctx,"a512_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",ro,nu,hlen);
}

ED25519();
console.log("\n");
C25519();
console.log("\n");
NIST256();
console.log("\n");
GOLDILOCKS();
console.log("\n");
SECP256K1();
console.log("\n");
BLS12381();
console.log("\n");
BLS12381_G2();
console.log("\n");

