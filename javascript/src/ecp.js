/*
 * Copyright (c) 2012-2020 MIRACL UK Ltd.
 *
 * This file is part of MIRACL Core
 * (see https://github.com/miracl/core).
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* Elliptic Curve Point class */

var ECP = function(ctx) {
    "use strict";

    /* Constructor */
    var ECP = function() {
        this.x = new ctx.FP(0);
        this.y = new ctx.FP(1);
        if (ECP.CURVETYPE != ECP.EDWARDS) {
            this.z = new ctx.FP(0);
        } else {
            this.z = new ctx.FP(1);
        }
    };

    ECP.WEIERSTRASS = 0;
    ECP.EDWARDS = 1;
    ECP.MONTGOMERY = 2;
    ECP.NOT = 0;
    ECP.BN = 1;
    ECP.BLS12 = 2;
    ECP.BLS24 = 3;
    ECP.BLS48 = 4;
    ECP.D_TYPE = 0;
    ECP.M_TYPE = 1;
    ECP.POSITIVEX = 0;
    ECP.NEGATIVEX = 1;

    ECP.CURVETYPE = ctx.config["@CT"];
    ECP.CURVE_A = ctx.config["@CA"];
    ECP.CURVE_PAIRING_TYPE = ctx.config["@PF"];
    ECP.SEXTIC_TWIST = ctx.config["@ST"];
    ECP.SIGN_OF_X = ctx.config["@SX"];
    ECP.ATE_BITS = ctx.config["@AB"];
    ECP.HTC_ISO = ctx.config["@HC"];
    ECP.HTC_ISO_G2 = ctx.config["@HC2"];

    ECP.HASH_TYPE = ctx.config["@HT"];
    ECP.AESKEY = ctx.config["@AK"];
    ECP.ALLOW_ALT_COMPRESS = false;

    ECP.prototype = {
        /* test this=O point-at-infinity */
        is_infinity: function() {
            if (ECP.CURVETYPE == ECP.EDWARDS) {
                return (this.x.iszilch() && this.y.equals(this.z));
            } else if (ECP.CURVETYPE == ECP.WEIERSTRASS) {
                return (this.x.iszilch() && this.z.iszilch());
            } else if (ECP.CURVETYPE == ECP.MONTGOMERY) {
                return (this.z.iszilch());
            }

            return true;
        },

        /* conditional swap of this and Q dependant on d */
        cswap: function(Q, d) {

            this.x.cswap(Q.x, d);
            if (ECP.CURVETYPE != ECP.MONTGOMERY) {
                this.y.cswap(Q.y, d);
            }
            this.z.cswap(Q.z, d);

        },

        /* conditional move of Q to P dependant on d */
        cmove: function(Q, d) {

            this.x.cmove(Q.x, d);
            if (ECP.CURVETYPE != ECP.MONTGOMERY) {
                this.y.cmove(Q.y, d);
            }
            this.z.cmove(Q.z, d);

        },

        /* Constant time select from pre-computed table */
        select: function(W, b) {
            var MP = new ECP(),
                m = b >> 31,
                babs = (b ^ m) - m;

            babs = (babs - 1) / 2;

            this.cmove(W[0], ECP.teq(babs, 0)); // conditional move
            this.cmove(W[1], ECP.teq(babs, 1));
            this.cmove(W[2], ECP.teq(babs, 2));
            this.cmove(W[3], ECP.teq(babs, 3));
            this.cmove(W[4], ECP.teq(babs, 4));
            this.cmove(W[5], ECP.teq(babs, 5));
            this.cmove(W[6], ECP.teq(babs, 6));
            this.cmove(W[7], ECP.teq(babs, 7));

            MP.copy(this);
            MP.neg();
            this.cmove(MP, (m & 1));
        },

        /* Test P == Q */

        equals: function(Q) {
            var a, b;

            a = new ctx.FP(0);
            b = new ctx.FP(0);
            a.copy(this.x);
            a.mul(Q.z);
            a.reduce();
            b.copy(Q.x);
            b.mul(this.z);
            b.reduce();

            if (!a.equals(b)) {
                return false;
            }

            if (ECP.CURVETYPE != ECP.MONTGOMERY) {
                a.copy(this.y);
                a.mul(Q.z);
                a.reduce();
                b.copy(Q.y);
                b.mul(this.z);
                b.reduce();
                if (!a.equals(b)) {
                    return false;
                }
            }

            return true;
        },

        /* copy this=P */
        copy: function(P) {
            this.x.copy(P.x);
            if (ECP.CURVETYPE != ECP.MONTGOMERY) {
                this.y.copy(P.y);
            }
            this.z.copy(P.z);
        },

        /* this=-this */
        neg: function() {
            if (ECP.CURVETYPE == ECP.WEIERSTRASS) {
                this.y.neg();
                this.y.norm();
            } else if (ECP.CURVETYPE == ECP.EDWARDS) {
                this.x.neg();
                this.x.norm();
            }

            return;
        },

        /* set this=O */
        inf: function() {
            this.x.zero();

            if (ECP.CURVETYPE != ECP.MONTGOMERY) {
                this.y.one();
            }

            if (ECP.CURVETYPE != ECP.EDWARDS) {
                this.z.zero();
            } else {
                this.z.one();
            }
        },

        /* set this=(x,y) where x and y are BIGs */
        setxy: function(ix, iy) {
            var rhs, y2;

            this.x = new ctx.FP(0);
            this.x.bcopy(ix);

            this.y = new ctx.FP(0);
            this.y.bcopy(iy);
            this.z = new ctx.FP(1);
			this.x.norm();
            rhs = ECP.RHS(this.x);

            if (ECP.CURVETYPE == ECP.MONTGOMERY) {
                if (rhs.qr(null) != 1) {
                    this.inf();
                }

            } else {
                y2 = new ctx.FP(0);
                y2.copy(this.y);
                y2.sqr();

                if (!y2.equals(rhs)) {
                    this.inf();
                }
            }
        },

        /* set this=x, where x is ctx.BIG, y is derived from sign s */
        setxi: function(ix, s) {
            var rhs, ny;

            this.x = new ctx.FP(0);
            this.x.bcopy(ix);
			this.x.norm();
            rhs = ECP.RHS(this.x);
            this.z = new ctx.FP(1);
            var hint=new ctx.FP(0);
            if (rhs.qr(hint) == 1) {
                ny = rhs.sqrt(hint);
                if (ny.sign() != s) {
                    ny.neg();
                    ny.norm();
                }
                this.y = ny;
            } else {
                this.inf();
            }
        },

        /* set this=x, y calculated from curve equation */
        setx: function(ix) {
            var rhs;

            this.x = new ctx.FP(0);
            this.x.bcopy(ix);
			this.x.norm();
            rhs = ECP.RHS(this.x);
            this.z = new ctx.FP(1);
            var hint=new ctx.FP(0);

            if (rhs.qr(hint) == 1) {
                if (ECP.CURVETYPE != ECP.MONTGOMERY) {
                    this.y = rhs.sqrt(hint);
                }
            } else {
                this.inf();
            }
        },

        /* set this to affine - from (x,y,z) to (x,y) */
        affine: function() {
            var one;

            if (this.is_infinity()) {
                return;
            }

            one = new ctx.FP(1);

            if (this.z.equals(one)) {
                return;
            }

            this.z.inverse(null);

            if (ECP.CURVETYPE == ECP.EDWARDS || ECP.CURVETYPE == ECP.WEIERSTRASS) {
                this.x.mul(this.z);
                this.x.reduce();
                this.y.mul(this.z);
                this.y.reduce();
                this.z = one;
            }
            if (ECP.CURVETYPE == ECP.MONTGOMERY) {
                this.x.mul(this.z);
                this.x.reduce();
                this.z = one;
            }
        },

        /* extract x as ctx.BIG */
        getX: function() {
			var W=new ECP(); W.copy(this); W.affine();
            return W.x.redc();
        },

        /* extract y as ctx.BIG */
        getY: function() {
			var W=new ECP(); W.copy(this); W.affine();
            return W.y.redc();
        },

        /* get sign of Y */
        getS: function() {
			var W=new ECP(); W.copy(this); W.affine();
            return W.y.sign();
        },

        /* extract x as ctx.FP */
        getx: function() {
            return this.x;
        },

        /* extract y as ctx.FP */
        gety: function() {
            return this.y;
        },

        /* extract z as ctx.FP */
        getz: function() {
            return this.z;
        },

        /* convert to byte array */
        toBytes: function(b,compress) {
            var t = [];
            var alt=false;
			var W=new ECP(); W.copy(this);
            W.affine();
            W.x.redc().toBytes(t);

            if (ECP.CURVETYPE == ECP.MONTGOMERY) {
                for (i = 0; i < ctx.BIG.MODBYTES; i++) {
                    b[i] = t[i];
                }
                return;
            }

            if ((ctx.FP.MODBITS-1)%8<=4 && ECP.ALLOW_ALT_COMPRESS) alt=true;

            if (alt)
            {
                for (var i=0;i<ctx.BIG.MODBYTES;i++) b[i]=t[i];
                if (compress)
                {
                    b[0]|=0x80;
                    if (W.y.islarger()==1) b[0]|=0x20;
                } else {
                    W.y.redc().toBytes(t);
                    for (var i=0;i<ctx.BIG.MODBYTES;i++) b[i+ctx.BIG.MODBYTES]=t[i];
                }
            } else {
                for (var i = 0; i < ctx.BIG.MODBYTES; i++) b[i + 1] = t[i];
                if (compress) {
                    b[0]=0x02;
                    if (W.y.sign()==1) b[0]=0x03;
                    return;
                }
                b[0]=0x04;
                W.y.redc().toBytes(t);
                for (var i = 0; i < ctx.BIG.MODBYTES; i++) b[i + ctx.BIG.MODBYTES + 1] = t[i];
            }
        },
        /* convert to hex string */
        toString: function() {
			var W=new ECP(); W.copy(this);
            if (W.is_infinity()) {
                return "infinity";
            }

            W.affine();

            if (ECP.CURVETYPE == ECP.MONTGOMERY) {
                return "(" + W.x.redc().toString() + ")";
            } else {
                return "(" + W.x.redc().toString() + "," + W.y.redc().toString() + ")";
            }
        },

        /* this+=this */
        dbl: function() {
            var t0, t1, t2, t3, x3, y3, z3, b,
                C, D, H, J,
                A, B, AA, BB;

            if (ECP.CURVETYPE == ECP.WEIERSTRASS) {

                if (ECP.CURVE_A == 0) {
                    t0 = new ctx.FP(0);
                    t0.copy(this.y);
                    t0.sqr();
                    t1 = new ctx.FP(0);
                    t1.copy(this.y);
                    t1.mul(this.z);
                    t2 = new ctx.FP(0);
                    t2.copy(this.z);
                    t2.sqr();

                    this.z.copy(t0);
                    this.z.add(t0);
                    this.z.norm();
                    this.z.add(this.z);
                    this.z.add(this.z);
                    this.z.norm();

                    t2.imul(3 * ctx.ROM_CURVE.CURVE_B_I);

                    x3 = new ctx.FP(0);
                    x3.copy(t2);
                    x3.mul(this.z);
                    y3 = new ctx.FP(0);
                    y3.copy(t0);
                    y3.add(t2);
                    y3.norm();
                    this.z.mul(t1);
                    t1.copy(t2);
                    t1.add(t2);
                    t2.add(t1);
                    t0.sub(t2);
                    t0.norm();
                    y3.mul(t0);
                    y3.add(x3);
                    t1.copy(this.x);
                    t1.mul(this.y);
                    this.x.copy(t0);
                    this.x.norm();
                    this.x.mul(t1);
                    this.x.add(this.x);

                    this.x.norm();
                    this.y.copy(y3);
                    this.y.norm();
                } else {
                    t0 = new ctx.FP(0);
                    t0.copy(this.x);
                    t1 = new ctx.FP(0);
                    t1.copy(this.y);
                    t2 = new ctx.FP(0);
                    t2.copy(this.z);
                    t3 = new ctx.FP(0);
                    t3.copy(this.x);
                    z3 = new ctx.FP(0);
                    z3.copy(this.z);
                    y3 = new ctx.FP(0);
                    x3 = new ctx.FP(0);
                    b = new ctx.FP(0);

                    if (ctx.ROM_CURVE.CURVE_B_I == 0) {
                        b.rcopy(ctx.ROM_CURVE.CURVE_B);
                    }

                    t0.sqr(); //1    x^2
                    t1.sqr(); //2    y^2
                    t2.sqr(); //3

                    t3.mul(this.y); //4
                    t3.add(t3);
                    t3.norm(); //5
                    z3.mul(this.x); //6
                    z3.add(z3);
                    z3.norm(); //7
                    y3.copy(t2);

                    if (ctx.ROM_CURVE.CURVE_B_I == 0) {
                        y3.mul(b); //8
                    } else {
                        y3.imul(ctx.ROM_CURVE.CURVE_B_I);
                    }

                    y3.sub(z3); //9  ***
                    x3.copy(y3);
                    x3.add(y3);
                    x3.norm(); //10

                    y3.add(x3); //11
                    x3.copy(t1);
                    x3.sub(y3);
                    x3.norm(); //12
                    y3.add(t1);
                    y3.norm(); //13
                    y3.mul(x3); //14
                    x3.mul(t3); //15
                    t3.copy(t2);
                    t3.add(t2);  //16
                    t2.add(t3);  //17

                    if (ctx.ROM_CURVE.CURVE_B_I == 0) {
                        z3.mul(b); //18
                    } else {
                        z3.imul(ctx.ROM_CURVE.CURVE_B_I);
                    }

                    z3.sub(t2); //19
                    z3.sub(t0);
                    z3.norm(); //20  ***
                    t3.copy(z3);
                    t3.add(z3); //21

                    z3.add(t3);
                    z3.norm(); //22
                    t3.copy(t0);
                    t3.add(t0); //23
                    t0.add(t3); //24
                    t0.sub(t2);
                    t0.norm(); //25

                    t0.mul(z3); //26
                    y3.add(t0); //27
                    t0.copy(this.y);
                    t0.mul(this.z); //28
                    t0.add(t0);
                    t0.norm(); //29
                    z3.mul(t0); //30
                    x3.sub(z3); //31
                    t0.add(t0);
                    t0.norm(); //32
                    t1.add(t1);
                    t1.norm(); //33
                    z3.copy(t0);
                    z3.mul(t1); //34

                    this.x.copy(x3);
                    this.x.norm();
                    this.y.copy(y3);
                    this.y.norm();
                    this.z.copy(z3);
                    this.z.norm();
                }
            }

            if (ECP.CURVETYPE == ECP.EDWARDS) {
                C = new ctx.FP(0);
                C.copy(this.x);
                D = new ctx.FP(0);
                D.copy(this.y);
                H = new ctx.FP(0);
                H.copy(this.z);
                J = new ctx.FP(0);

                this.x.mul(this.y);
                this.x.add(this.x);
                this.x.norm();
                C.sqr();
                D.sqr();
                if (ECP.CURVE_A == -1) {
                    C.neg();
                }

                this.y.copy(C);
                this.y.add(D);
                this.y.norm();
                H.sqr();
                H.add(H);

                this.z.copy(this.y);
                J.copy(this.y);

                J.sub(H);
                J.norm();

                this.x.mul(J);
                C.sub(D);
                C.norm();
                this.y.mul(C);
                this.z.mul(J);
            }

            if (ECP.CURVETYPE == ECP.MONTGOMERY) {
                A = new ctx.FP(0);
                A.copy(this.x);
                B = new ctx.FP(0);
                B.copy(this.x);
                AA = new ctx.FP(0);
                BB = new ctx.FP(0);
                C = new ctx.FP(0);

                A.add(this.z);
                A.norm();
                AA.copy(A);
                AA.sqr();
                B.sub(this.z);
                B.norm();
                BB.copy(B);
                BB.sqr();
                C.copy(AA);
                C.sub(BB);
                C.norm();
                this.x.copy(AA);
                this.x.mul(BB);

                A.copy(C);
                A.imul((ECP.CURVE_A + 2) >> 2);

                BB.add(A);
                BB.norm();
                this.z.copy(BB);
                this.z.mul(C);
            }

            return;
        },

        /* this+=Q */
        add: function(Q) {
            var b, t0, t1, t2, t3, t4, x3, y3, z3,
                A, B, C, D, E, F, G;

            if (ECP.CURVETYPE == ECP.WEIERSTRASS) {
                if (ECP.CURVE_A == 0) {

                    b = 3 * ctx.ROM_CURVE.CURVE_B_I;
                    t0 = new ctx.FP(0);
                    t0.copy(this.x);
                    t0.mul(Q.x);
                    t1 = new ctx.FP(0);
                    t1.copy(this.y);
                    t1.mul(Q.y);
                    t2 = new ctx.FP(0);
                    t2.copy(this.z);
                    t2.mul(Q.z);
                    t3 = new ctx.FP(0);
                    t3.copy(this.x);
                    t3.add(this.y);
                    t3.norm();
                    t4 = new ctx.FP(0);
                    t4.copy(Q.x);
                    t4.add(Q.y);
                    t4.norm();
                    t3.mul(t4);
                    t4.copy(t0);
                    t4.add(t1);

                    t3.sub(t4);
                    t3.norm();
                    t4.copy(this.y);
                    t4.add(this.z);
                    t4.norm();
                    x3 = new ctx.FP(0);
                    x3.copy(Q.y);
                    x3.add(Q.z);
                    x3.norm();

                    t4.mul(x3);
                    x3.copy(t1);
                    x3.add(t2);

                    t4.sub(x3);
                    t4.norm();
                    x3.copy(this.x);
                    x3.add(this.z);
                    x3.norm();
                    y3 = new ctx.FP(0);
                    y3.copy(Q.x);
                    y3.add(Q.z);
                    y3.norm();
                    x3.mul(y3);
                    y3.copy(t0);
                    y3.add(t2);
                    y3.rsub(x3);
                    y3.norm();
                    x3.copy(t0);
                    x3.add(t0);
                    t0.add(x3);
                    t0.norm();
                    t2.imul(b);

                    z3 = new ctx.FP(0);
                    z3.copy(t1);
                    z3.add(t2);
                    z3.norm();
                    t1.sub(t2);
                    t1.norm();
                    y3.imul(b);

                    x3.copy(y3);
                    x3.mul(t4);
                    t2.copy(t3);
                    t2.mul(t1);
                    x3.rsub(t2);
                    y3.mul(t0);
                    t1.mul(z3);
                    y3.add(t1);
                    t0.mul(t3);
                    z3.mul(t4);
                    z3.add(t0);

                    this.x.copy(x3);
                    this.x.norm();
                    this.y.copy(y3);
                    this.y.norm();
                    this.z.copy(z3);
                    this.z.norm();
                } else {
                    t0 = new ctx.FP(0);
                    t0.copy(this.x);
                    t1 = new ctx.FP(0);
                    t1.copy(this.y);
                    t2 = new ctx.FP(0);
                    t2.copy(this.z);
                    t3 = new ctx.FP(0);
                    t3.copy(this.x);
                    t4 = new ctx.FP(0);
                    t4.copy(Q.x);
                    z3 = new ctx.FP(0);
                    y3 = new ctx.FP(0);
                    y3.copy(Q.x);
                    x3 = new ctx.FP(0);
                    x3.copy(Q.y);
                    b = new ctx.FP(0);

                    if (ctx.ROM_CURVE.CURVE_B_I == 0) {
                        b.rcopy(ctx.ROM_CURVE.CURVE_B);
                    }
                    t0.mul(Q.x); //1
                    t1.mul(Q.y); //2
                    t2.mul(Q.z); //3

                    t3.add(this.y);
                    t3.norm(); //4
                    t4.add(Q.y);
                    t4.norm(); //5
                    t3.mul(t4); //6
                    t4.copy(t0);
                    t4.add(t1);  //7
                    t3.sub(t4);
                    t3.norm(); //8
                    t4.copy(this.y);
                    t4.add(this.z);
                    t4.norm(); //9
                    x3.add(Q.z);
                    x3.norm(); //10
                    t4.mul(x3); //11
                    x3.copy(t1);
                    x3.add(t2); //12

                    t4.sub(x3);
                    t4.norm(); //13
                    x3.copy(this.x);
                    x3.add(this.z);
                    x3.norm(); //14
                    y3.add(Q.z);
                    y3.norm(); //15

                    x3.mul(y3); //16
                    y3.copy(t0);
                    y3.add(t2); //17

                    y3.rsub(x3);
                    y3.norm(); //18
                    z3.copy(t2);

                    if (ctx.ROM_CURVE.CURVE_B_I == 0) {
                        z3.mul(b); //18
                    } else {
                        z3.imul(ctx.ROM_CURVE.CURVE_B_I);
                    }

                    x3.copy(y3);
                    x3.sub(z3);
                    x3.norm(); //20
                    z3.copy(x3);
                    z3.add(x3);  //21

                    x3.add(z3);  //22
                    z3.copy(t1);
                    z3.sub(x3);
                    z3.norm(); //23
                    x3.add(t1);
                    x3.norm(); //24

                    if (ctx.ROM_CURVE.CURVE_B_I == 0) {
                        y3.mul(b); //18
                    } else {
                        y3.imul(ctx.ROM_CURVE.CURVE_B_I);
                    }

                    t1.copy(t2);
                    t1.add(t2); //26
                    t2.add(t1); //27

                    y3.sub(t2);  //28

                    y3.sub(t0);
                    y3.norm(); //29
                    t1.copy(y3);
                    t1.add(y3); //30
                    y3.add(t1);
                    y3.norm(); //31

                    t1.copy(t0);
                    t1.add(t0); //32
                    t0.add(t1); //33
                    t0.sub(t2);
                    t0.norm(); //34
                    t1.copy(t4);
                    t1.mul(y3); //35
                    t2.copy(t0);
                    t2.mul(y3); //36
                    y3.copy(x3);
                    y3.mul(z3); //37
                    y3.add(t2); //38
                    x3.mul(t3); //39
                    x3.sub(t1); //40
                    z3.mul(t4); //41
                    t1.copy(t3);
                    t1.mul(t0); //42
                    z3.add(t1);

                    this.x.copy(x3);
                    this.x.norm();
                    this.y.copy(y3);
                    this.y.norm();
                    this.z.copy(z3);
                    this.z.norm();
                }
            }

            if (ECP.CURVETYPE == ECP.EDWARDS) {
                A = new ctx.FP(0);
                A.copy(this.z);
                B = new ctx.FP(0);
                C = new ctx.FP(0);
                C.copy(this.x);
                D = new ctx.FP(0);
                D.copy(this.y);
                E = new ctx.FP(0);
                F = new ctx.FP(0);
                G = new ctx.FP(0);

                A.mul(Q.z); //A=2
                B.copy(A);
                B.sqr(); //B=2
                C.mul(Q.x); //C=2
                D.mul(Q.y); //D=2

                E.copy(C);
                E.mul(D); //E=2

                if (ctx.ROM_CURVE.CURVE_B_I == 0) {
                    b = new ctx.FP(0);
                    b.rcopy(ctx.ROM_CURVE.CURVE_B);
                    E.mul(b);
                } else {
                    E.imul(ctx.ROM_CURVE.CURVE_B_I); //E=22222
                }

                F.copy(B);
                F.sub(E); //F=22224
                G.copy(B);
                G.add(E); //G=22224

                if (ECP.CURVE_A == 1) {
                    E.copy(D);
                    E.sub(C); //E=4
                }
                C.add(D); //C=4

                B.copy(this.x);
                B.add(this.y); //B=4
                D.copy(Q.x);
                D.add(Q.y);
                B.norm();
                D.norm(); //D=4
                B.mul(D); //B=2
                B.sub(C);
                B.norm();
                F.norm(); // B=6
                B.mul(F); //B=2
                this.x.copy(A);
                this.x.mul(B);
                G.norm(); // x=2

                if (ECP.CURVE_A == 1) {
                    E.norm();
                    C.copy(E);
                    C.mul(G); //C=2
                }

                if (ECP.CURVE_A == -1) {
                    C.norm();
                    C.mul(G);
                }

                this.y.copy(A);
                this.y.mul(C); //y=2
                this.z.copy(F);
                this.z.mul(G);
            }

            return;
        },

        /* Differential Add for Montgomery curves. this+=Q where W is this-Q and is affine. */
        dadd: function(Q, W) {
            var A, B, C, D, DA, CB;

            A = new ctx.FP(0);
            A.copy(this.x);
            B = new ctx.FP(0);
            B.copy(this.x);
            C = new ctx.FP(0);
            C.copy(Q.x);
            D = new ctx.FP(0);
            D.copy(Q.x);
            DA = new ctx.FP(0);
            CB = new ctx.FP(0);

            A.add(this.z);
            B.sub(this.z);

            C.add(Q.z);
            D.sub(Q.z);

            D.norm();
            A.norm();
            DA.copy(D);
            DA.mul(A);
            C.norm();
            B.norm();
            CB.copy(C);
            CB.mul(B);

            A.copy(DA);
            A.add(CB);
            A.norm();
            A.sqr();
            B.copy(DA);
            B.sub(CB);
            B.norm();
            B.sqr();

            this.x.copy(A);
            this.z.copy(W.x);
            this.z.mul(B);

            //  this.x.norm();
        },

        /* this-=Q */
        sub: function(Q) {
			var NQ = new ECP(); NQ.copy(Q);
            NQ.neg();
            this.add(NQ);
        },

        /* constant time multiply by small integer of length bts - use ladder */
        pinmul: function(e, bts) {
            var i, b, P, R0, R1;

            if (ECP.CURVETYPE == ECP.MONTGOMERY) {
                return this.mul(new ctx.BIG(e));
            } else {
                P = new ECP();
                R0 = new ECP();
                R1 = new ECP();
                R1.copy(this);

                for (i = bts - 1; i >= 0; i--) {
                    b = (e >> i) & 1;
                    P.copy(R1);
                    P.add(R0);
                    R0.cswap(R1, b);
                    R1.copy(P);
                    R0.dbl();
                    R0.cswap(R1, b);
                }

                P.copy(R0);

                return P;
            }
        },

        // multiply this by the curves cofactor
        cfp: function() {
            var cf=ctx.ROM_CURVE.CURVE_Cof_I,
                c = new ctx.BIG(0);

            if (cf==1) {
                return;
            }
            if (cf==4) {
                this.dbl(); this.dbl();
                return;
            }
            if (cf==8) {
                this.dbl(); this.dbl(); this.dbl();
                return;
            }
            c.rcopy(ctx.ROM_CURVE.CURVE_Cof);
            this.copy(this.mul(c));
        },

// Point multiplication, multiplies a point P by a scalar e
// This code has no inherent awareness of the order of the curve, or the order of the point.
// The order of the curve will be h.r, where h is a cofactor, and r is a large prime
// Typically P will be of order r (but not always), and typically e will be less than r (but not always)
// A problem can arise if a secret e is a few bits less than r, as the leading zeros in e will leak via a timing attack
// The secret e may however be greater than r (see RFC7748 which combines elimination of a small cofactor h with the point multiplication, using an e>r)
// Our solution is to use as a multiplier an e, whose length in bits is that of the logical OR of e and r, hence allowing e>r while forcing inclusion of leading zeros if e<r. 
// The point multiplication methods used will process leading zeros correctly.

// So this function leaks information about the length of e...
        mul: function(e) {
            return this.clmul(e,e);
        },

// .. but this one does not (typically set maxe=r)
// Set P=e*P 
        /* return e.this - SPA immune, using Ladder */
        clmul: function(e,maxe) {
            var P, D, R0, R1, mt, t, Q, C, W, w,
                i, b, nb, s, ns, max, cm;

            if (e.iszilch() || this.is_infinity()) {
                return new ECP();
            }
            P = new ECP();
            cm =new ctx.BIG(); cm.copy(e); cm.or(maxe);
            max=cm.nbits();

            if (ECP.CURVETYPE == ECP.MONTGOMERY) { /* use ladder */
                D = new ECP();
                R0 = new ECP();
                R0.copy(this);
                R1 = new ECP();
                R1.copy(this);
                R1.dbl();
                D.copy(this); D.affine();
                nb = max;
                for (i = nb - 2; i >= 0; i--) {
                    b = e.bit(i);
                    P.copy(R1);
                    P.dadd(R0, D);

                    R0.cswap(R1, b);
                    R1.copy(P);
                    R0.dbl();
                    R0.cswap(R1, b);
                }
                P.copy(R0);
            } else {
                // fixed size windows
                mt = new ctx.BIG();
                t = new ctx.BIG();
                Q = new ECP();
                C = new ECP();
                W = [];
                w = [];

                // precompute table
                Q.copy(this);
                Q.dbl();
                W[0] = new ECP();
                W[0].copy(this);

                for (i = 1; i < 8; i++) {
                    W[i] = new ECP();
                    W[i].copy(W[i - 1]);
                    W[i].add(Q);
                }

                // make exponent odd - add 2P if even, P if odd
                t.copy(e);
                s = t.parity();
                t.inc(1);
                t.norm();
                ns = t.parity();
                mt.copy(t);
                mt.inc(1);
                mt.norm();
                t.cmove(mt, s);
                Q.cmove(this, ns);
                C.copy(Q);

                nb = 1 + Math.floor((max + 3) / 4);

                // convert exponent to signed 4-bit window
                for (i = 0; i < nb; i++) {
                    w[i] = (t.lastbits(5) - 16);
                    t.dec(w[i]);
                    t.norm();
                    t.fshr(4);
                }
                w[nb] = t.lastbits(5);

                //P.copy(W[Math.floor((w[nb] - 1) / 2)]);
                P.select(W, w[nb]);
                for (i = nb - 1; i >= 0; i--) {
                    Q.select(W, w[i]);
                    P.dbl();
                    P.dbl();
                    P.dbl();
                    P.dbl();
                    P.add(Q);
                }
                P.sub(C);
            }

            return P;
        },

        /* Return e.this+f.Q */

        mul2: function(e, Q, f) {
            var te = new ctx.BIG(),
                tf = new ctx.BIG(),
                mt = new ctx.BIG(),
                S = new ECP(),
                T = new ECP(),
                C = new ECP(),
                W = [],
                w = [],
                i, s, ns, nb,
                a, b;

            te.copy(e);
            tf.copy(f);

            // precompute table
            W[1] = new ECP();
            W[1].copy(this);
            W[1].sub(Q);
            W[2] = new ECP();
            W[2].copy(this);
            W[2].add(Q);
            S.copy(Q);
            S.dbl();
            W[0] = new ECP();
            W[0].copy(W[1]);
            W[0].sub(S);
            W[3] = new ECP();
            W[3].copy(W[2]);
            W[3].add(S);
            T.copy(this);
            T.dbl();
            W[5] = new ECP();
            W[5].copy(W[1]);
            W[5].add(T);
            W[6] = new ECP();
            W[6].copy(W[2]);
            W[6].add(T);
            W[4] = new ECP();
            W[4].copy(W[5]);
            W[4].sub(S);
            W[7] = new ECP();
            W[7].copy(W[6]);
            W[7].add(S);

            // if multiplier is odd, add 2, else add 1 to multiplier, and add 2P or P to correction

            s = te.parity();
            te.inc(1);
            te.norm();
            ns = te.parity();
            mt.copy(te);
            mt.inc(1);
            mt.norm();
            te.cmove(mt, s);
            T.cmove(this, ns);
            C.copy(T);

            s = tf.parity();
            tf.inc(1);
            tf.norm();
            ns = tf.parity();
            mt.copy(tf);
            mt.inc(1);
            mt.norm();
            tf.cmove(mt, s);
            S.cmove(Q, ns);
            C.add(S);

            mt.copy(te);
            mt.add(tf);
            mt.norm();
            nb = 1 + Math.floor((mt.nbits() + 1) / 2);

            // convert exponent to signed 2-bit window
            for (i = 0; i < nb; i++) {
                a = (te.lastbits(3) - 4);
                te.dec(a);
                te.norm();
                te.fshr(2);
                b = (tf.lastbits(3) - 4);
                tf.dec(b);
                tf.norm();
                tf.fshr(2);
                w[i] = (4 * a + b);
            }
            w[nb] = (4 * te.lastbits(3) + tf.lastbits(3));
            //S.copy(W[Math.floor((w[nb] - 1) / 2)]);
            S.select(W, w[nb]);
            for (i = nb - 1; i >= 0; i--) {
                T.select(W, w[i]);
                S.dbl();
                S.dbl();
                S.add(T);
            }
            S.sub(C); /* apply correction */

            return S;
        }
    };

    // set to group generator
    ECP.generator = function() {
        var G=new ECP(),
            gx = new ctx.BIG(0),
            gy = new ctx.BIG(0);

        gx.rcopy(ctx.ROM_CURVE.CURVE_Gx);

        if (ctx.ECP.CURVETYPE != ctx.ECP.MONTGOMERY) {
            gy.rcopy(ctx.ROM_CURVE.CURVE_Gy);
            G.setxy(gx, gy);
        } else {
            G.setx(gx);
        }
        return G;
    };

    /* return 1 if b==c, no branching */
    ECP.teq = function(b, c) {
        var x = b ^ c;
        x -= 1; // if x=0, x now -1
        return ((x >> 31) & 1);
    };

    /* convert from byte array to ECP */
    ECP.fromBytes = function(b) {
        var t = [],
            P = new ECP(),
            p = new ctx.BIG(0),
            px, py, i;
        var alt=false;

        p.rcopy(ctx.ROM_FIELD.Modulus);

        if (ECP.CURVETYPE == ECP.MONTGOMERY) {
            for (i = 0; i < ctx.BIG.MODBYTES; i++) {
                t[i] = b[i];
            }
            px = ctx.BIG.fromBytes(t);
            if (ctx.BIG.comp(px, p) >= 0) {
               return P;
            }
            P.setx(px);
            return P;
        }

        if ((ctx.FP.MODBITS-1)%8<=4 && ECP.ALLOW_ALT_COMPRESS) alt=true;

        if (alt)
        {
            for (i=0;i<ctx.BIG.MODBYTES;i++) t[i]=b[i];
            t[0]&=0x1f;
            px=ctx.BIG.fromBytes(t);
            if ((b[0]&0x80)==0)
            {
                for (i=0;i<ctx.BIG.MODBYTES;i++) t[i]=b[i+ctx.BIG.MODBYTES];
                py=ctx.BIG.fromBytes(t);
                P.setxy(px, py);
                return P;
            } else {
                var sgn=(b[0]&0x20)>>5;
                P.setxi(px,0);
                var cmp=P.y.islarger();
                if ((sgn==1 && cmp!=1) || (sgn==0 && cmp==1)) P.neg();
                return P;
            }
        } else {

            for (i = 0; i < ctx.BIG.MODBYTES; i++) {
                t[i] = b[i + 1];
            }
            px = ctx.BIG.fromBytes(t);
            if (ctx.BIG.comp(px, p) >= 0) {
                return P;
            }

            if (b[0] == 0x04) {
                for (i = 0; i < ctx.BIG.MODBYTES; i++) {
                    t[i] = b[i + ctx.BIG.MODBYTES + 1];
                }

                py = ctx.BIG.fromBytes(t);

                if (ctx.BIG.comp(py, p) >= 0) {
                    return P;
                }

                P.setxy(px, py);

                return P;
            }

            if (b[0]==0x02 || b[0]==0x03) {
                P.setxi(px,b[0]&1);
                return P;
            }
        }

        return P;
    };

    /* Calculate RHS of curve equation */
    ECP.RHS = function(x) {
        var r = new ctx.FP(0),
            b, cx, one, x3;

        //x.norm();
        r.copy(x);
        r.sqr();

        if (ECP.CURVETYPE == ECP.WEIERSTRASS) { // x^3+Ax+B
            b = new ctx.FP(0);
            b.rcopy(ctx.ROM_CURVE.CURVE_B);
            r.mul(x);
            if (ECP.CURVE_A == -3) {
                cx = new ctx.FP(0);
                cx.copy(x);
                cx.imul(3);
                cx.neg();
                cx.norm();
                r.add(cx);
            }
            r.add(b);
        } else if (ECP.CURVETYPE == ECP.EDWARDS) { // (Ax^2-1)/(Bx^2-1)
            b = new ctx.FP(0);
            b.rcopy(ctx.ROM_CURVE.CURVE_B);

            one = new ctx.FP(1);
            b.mul(r);
            b.sub(one);
            b.norm();
            if (ECP.CURVE_A == -1) {
                r.neg();
            }
            r.sub(one);
            r.norm();
            b.inverse(null);

            r.mul(b);
        } else if (ECP.CURVETYPE == ECP.MONTGOMERY) { // x^3+Ax^2+x
            x3 = new ctx.FP(0);
            x3.copy(r);
            x3.mul(x);
            r.imul(ECP.CURVE_A);
            r.add(x3);
            r.add(x);
        }

        r.reduce();

        return r;
    };

// Generic multi-multiplication, fixed 4-bit window, P=Sigma e_i*X_i
    ECP.muln = function(n,X,e) {
        var B = [];
        var P=new ECP();
        var R=new ECP();
        var S=new ECP();
        var t=new ctx.BIG();
        var mr=new ctx.BIG();

        for (var i=0;i<16;i++) {
            B[i]=new ECP();
        }
        var mt=new ctx.BIG(); mt.copy(e[0]); mt.norm();
        for (var i=1;i<n;i++)
        { // find biggest
            t.copy(e[i]); t.norm();
            var k=ctx.BIG.comp(t,mt);
            mt.cmove(t,(k+1)>>1);
        }
        var nb=(mt.nbits()+3)>>2;
        for (var i=nb-1;i>=0;i--)
        {
            for (var j=0;j<16;j++) {
                B[j].inf();
            }
            for (var j=0;j<n;j++)
            {    
                mt.copy(e[j]); mt.norm();
                mt.shr(4*i);
                var k=mt.lastbits(4);
                B[k].add(X[j]);
            }
            R.inf(); S.inf();
            for (var j=15;j>=1;j--)
            {
                R.add(B[j]);
                S.add(R);
            }
            for (var j=0;j<4;j++) {
                P.dbl();
            }
            P.add(S);
        }
        return P;
    };


/* Hunt and Peck a BIG to a curve point
    ECP.hap2point = function(h) {
        var P = new ECP();
        var x=new ctx.BIG(h);
        for (;;) {
            if (ECP.CURVETYPE != ECP.MONTGOMERY) {
                P.setxi(x, 0);
            } else {
                P.setx(x);
            }
            x.inc(1);
            x.norm();
            if (!P.is_infinity()) break;
        }
        return P;
    };       */

/* Constant time Map to Point */
    ECP.map2point = function(h) {
        var P = new ECP();

        if (ECP.CURVETYPE == ECP.MONTGOMERY)
        {
            var X1=new ctx.FP(0);
            var X2=new ctx.FP(0);
            var t=new ctx.FP(h);
            var w=new ctx.FP(0);
            var one=new ctx.FP(1);
            var N=new ctx.FP(0);
            var D=new ctx.FP(0);
            var hint=new ctx.FP(0);
            var A=new ctx.FP(ECP.CURVE_A);
            t.sqr();

            if (ctx.FP.PM1D2 == 2) {
                t.add(t);
            }
            if (ctx.FP.PM1D2 == 1) {
                t.neg();
            }
            if (ctx.FP.PM1D2 > 2) {
                t.imul(ctx.FP.PM1D2);
            }
            t.norm();
            D.copy(t); D.add(one); D.norm();

            X1.copy(A);
            X1.neg(); X1.norm();
            X2.copy(X1);
            X2.mul(t);

            w.copy(X1); w.sqr(); N.copy(w); N.mul(X1);
            w.mul(A); w.mul(D); N.add(w); 
            t.copy(D); t.sqr();
            t.mul(X1);
            N.add(t); N.norm();

            t.copy(N); t.mul(D);
            var qres=t.qr(hint);
            w.copy(t); w.inverse(hint);
            D.copy(w); D.mul(N);
            X1.mul(D);
            X2.mul(D);
            X1.cmove(X2,1-qres);

            var a=X1.redc();
            P.setx(a);
        }
        if (ECP.CURVETYPE == ECP.EDWARDS)
        { // Elligator 2 - map to Montgomery, place point, map back
            var X1=new ctx.FP(0);
            var X2=new ctx.FP(0);
            var t=new ctx.FP(h);
            var w=new ctx.FP(0);
            var one=new ctx.FP(1);
            var A=new ctx.FP(0);
            var w1=new ctx.FP(0);
            var w2=new ctx.FP(0);
            var B = new ctx.FP(0);
            B.rcopy(ctx.ROM_CURVE.CURVE_B);
            var Y = new ctx.FP(0);
            var K=new ctx.FP(0);
            var D = new ctx.FP(0);   
            var hint=new ctx.FP(0);
            //var Y3 = new ctx.FP(0);   
           
            var qres,qnr,rfc=0;

            if (ctx.FP.MODTYPE!=ctx.FP.GENERALISED_MERSENNE)
            {
                A.copy(B);
                if (ECP.CURVE_A==1) {
                    A.add(one);
                    B.sub(one);
                } else {
                    A.sub(one);
                    B.add(one);
                }
                A.norm(); B.norm();

                A.div2();
                B.div2();
                B.div2();

                K.copy(B);
                K.neg(); K.norm();
                //K.inverse(null);
                K.invsqrt(K,w1);

                rfc=ctx.FP.RIADZ;
                if (rfc==1)
                { // RFC7748
                    A.mul(K);
                    K.mul(w1);
                    //K=K.sqrt(null);
                } else {
                    B.sqr();
                }
            } else {
                A.copy(new ctx.FP(156326));
                rfc=1;
            }

            t.sqr();
            if (ctx.FP.PM1D2 == 2) {
                t.add(t);
                qnr=2;
            }
            if (ctx.FP.PM1D2 == 1) {
                t.neg();
                qnr=-1;
            }
            if (ctx.FP.PM1D2 > 2) {
                t.imul(ctx.FP.PM1D2);
                qnr=ctx.FP.PM1D2;
            }
            t.norm();
            D.copy(t); D.add(one); D.norm();
            X1.copy(A);
            X1.neg(); X1.norm();
            X2.copy(X1); X2.mul(t);

// Figure out RHS of Montgomery curve in rational form gx1/d^3            

            w.copy(X1); w.sqr(); w1.copy(w); w1.mul(X1);
            w.mul(A); w.mul(D); w1.add(w);
            w2.copy(D); w2.sqr();

            if (rfc==0)
            {
                w.copy(X1); w.mul(B);
                w2.mul(w);
                w1.add(w2);
            } else {
                w2.mul(X1);
                w1.add(w2);
            }
            w1.norm();

            B.copy(w1); B.mul(D);
            qres=B.qr(hint);
            w.copy(B); w.inverse(hint);
            D.copy(w); D.mul(w1);
            X1.mul(D);
            X2.mul(D);
            D.sqr();

            w1.copy(B); w1.imul(qnr);
            w.rcopy(ctx.ROM_CURVE.CURVE_HTPC);
            w.mul(hint);
            w2.copy(D); w2.mul(h);

            X1.cmove(X2,1-qres);
            B.cmove(w1,1-qres);
            hint.cmove(w,1-qres);
            D.cmove(w2,1-qres);

            Y.copy(B.sqrt(hint));
            Y.mul(D);


/*
            Y.copy(B.sqrt(hint));
            Y.mul(D);

            B.imul(qnr);
            w.rcopy(ctx.ROM_CURVE.CURVE_HTPC);
            hint.mul(w);

            Y3.copy(B.sqrt(hint));
            D.mul(h);
            Y3.mul(D);

            X1.cmove(X2,1-qres);
            Y.cmove(Y3,1-qres);
*/
            w.copy(Y); w.neg(); w.norm();
            Y.cmove(w,qres^Y.sign());

            if (rfc==0)
            {
                X1.mul(K);
                Y.mul(K);
            }

            if (ctx.FP.MODTYPE==ctx.FP.GENERALISED_MERSENNE)
            { // GOLDILOCKS isogeny
                t.copy(X1); t.sqr();
                w.copy(t); w.add(one); w.norm();
                t.sub(one); t.norm();
                w1.copy(t); w1.mul(Y);
                w1.add(w1); X2.copy(w1); X2.add(w1); X2.norm();
                t.sqr();
                Y.sqr(); Y.add(Y); Y.add(Y); Y.norm();
                B.copy(t); B.add(Y); B.norm();

                w2.copy(Y); w2.sub(t); w2.norm();
                w2.mul(X1);
                t.mul(X1);
                Y.div2();
                w1.copy(Y); w1.mul(w);
                w1.rsub(t); w1.norm();
 
                t.copy(X2); t.mul(w1);
                P=new ECP();
                P.x.copy(t);
                t.copy(w2); t.mul(B);
                P.y.copy(t);
                t.copy(w1); t.mul(B);
                P.z.copy(t);

                return P;
            } else {
                w1.copy(X1); w1.add(one); w1.norm();
                w2.copy(X1); w2.sub(one); w2.norm();
                t.copy(w1); t.mul(Y);
                X1.mul(w1);

                if (rfc==1)
                    X1.mul(K);

                Y.mul(w2);
                P=new ECP();
                P.x.copy(X1);
                P.y.copy(Y);
                P.z.copy(t);
                
                return P;
            }
        }
        if (ECP.CURVETYPE == ECP.WEIERSTRASS)
        { // swu method
            var A=new ctx.FP(0);
            var B=new ctx.FP(0);
            var X1=new ctx.FP(0);
            var X2=new ctx.FP(0);
            var X3=new ctx.FP(0);
            var one=new ctx.FP(1);
            var Y=new ctx.FP(0);
            var D=new ctx.FP(0);
            var t=new ctx.FP(h);
            var w=new ctx.FP(0);
            var D2=new ctx.FP(0);
            var hint=new ctx.FP(0);
            var GX1=new ctx.FP(0);
            //var Y3=new ctx.FP(0);
            var sgn=t.sign();

            if (ECP.CURVE_A!=0 || ECP.HTC_ISO!=0)
            {
                if (ECP.HTC_ISO!=0)
                {
                    A.rcopy(ctx.ROM_CURVE.CURVE_Ad);
                    B.rcopy(ctx.ROM_CURVE.CURVE_Bd);
                } else {
                    A.copy(new ctx.FP(ECP.CURVE_A));
                    B.rcopy(ctx.ROM_CURVE.CURVE_B);
                }
                // SSWU Method
                t.sqr();
                t.imul(ctx.FP.RIADZ);
                w.copy(t); w.add(one); w.norm();

                w.mul(t); D.copy(A);
                D.mul(w);

                w.add(one); w.norm();
                w.mul(B);
                w.neg(); w.norm();

                X2.copy(w); 
                X3.copy(t); X3.mul(X2);

// x^3+Ad^2x+Bd^3
                GX1.copy(X2); GX1.sqr(); D2.copy(D);
                D2.sqr(); w.copy(A); w.mul(D2); GX1.add(w); GX1.norm(); GX1.mul(X2); D2.mul(D);w.copy(B); w.mul(D2); GX1.add(w); GX1.norm();

                w.copy(GX1); w.mul(D);
                var qr=w.qr(hint);
                D.copy(w); D.inverse(hint);
                D.mul(GX1);
                X2.mul(D);
                X3.mul(D);
                t.mul(h);
                D2.copy(D); D2.sqr();

                D.copy(D2); D.mul(t);
                t.copy(w); t.imul(ctx.FP.RIADZ);
                X1.rcopy(ctx.ROM_CURVE.CURVE_HTPC);
                X1.mul(hint);

                X2.cmove(X3,1-qr);
                D2.cmove(D,1-qr);
                w.cmove(t,1-qr);
                hint.cmove(X1,1-qr);

                Y.copy(w.sqrt(hint));
                Y.mul(D2);

/*
                Y.copy(w.sqrt(hint));
                Y.mul(D2);

                D2.mul(t);
                w.imul(ctx.FP.RIADZ);

                X1.rcopy(ctx.ROM_CURVE.CURVE_HTPC);
                hint.mul(X1);
                
                Y3.copy(w.sqrt(hint));
                Y3.mul(D2);

                X2.cmove(X3,1-qr);
                Y.cmove(Y3,1-qr);
*/
                var ne=Y.sign()^sgn;
                w.copy(Y); w.neg(); w.norm();
                Y.cmove(w,ne);

                if (ECP.HTC_ISO!=0)
                {
                    var k=0;
                    var isox=ECP.HTC_ISO;
                    var isoy=3*(isox-1)/2;
                // xnum
                    var xnum=new ctx.FP(0);
                    xnum.rcopy(ctx.ROM_CURVE.PC[k++]);
                    for (var i=0;i<isox;i++) {
                        xnum.mul(X2);
                        w.rcopy(ctx.ROM_CURVE.PC[k++]);
                        xnum.add(w); xnum.norm();
                    }
                // xden
                    var xden=new ctx.FP(X2);
                    w.rcopy(ctx.ROM_CURVE.PC[k++]);
                    xden.add(w); xden.norm();
                    for (var i=0;i<isox-2;i++) {
                        xden.mul(X2);
                        w.rcopy(ctx.ROM_CURVE.PC[k++]);
                        xden.add(w); xden.norm();
                    }
                // ynum
                    var ynum=new ctx.FP(0);
                    ynum.rcopy(ctx.ROM_CURVE.PC[k++]);
                    for (var i=0;i<isoy;i++) {
                        ynum.mul(X2);
                        w.rcopy(ctx.ROM_CURVE.PC[k++]);
                        ynum.add(w); ynum.norm();
                    }
                // yden
                    var yden=new ctx.FP(X2);
                    w.rcopy(ctx.ROM_CURVE.PC[k++]);
                    yden.add(w); yden.norm();
                    for (var i=0;i<isoy-1;i++) {
                        yden.mul(X2);
                        w.rcopy(ctx.ROM_CURVE.PC[k++]);
                        yden.add(w); yden.norm();
                    }  
                    ynum.mul(Y);
                    w.copy(xnum); w.mul(yden);
                    P.x.copy(w);
                    w.copy(ynum); w.mul(xden);
                    P.y.copy(w);
                    w.copy(xden); w.mul(yden);
                    P.z.copy(w);
                    return P;
                } else {
                    var x=X2.redc();
                    var y=Y.redc();
                    P.setxy(x,y);
                    return P;
                }
            } else {
// Shallue and van de Woestijne
                var Z=ctx.FP.RIADZ;
                X1.copy(new ctx.FP(Z));
                X3.copy(X1);
                var A=ECP.RHS(X1);
                B.rcopy(ctx.ROM_FIELD.SQRTm3);
                B.imul(Z);

                t.sqr();
                Y.copy(A); Y.mul(t);
                t.copy(one); t.add(Y); t.norm();
                Y.rsub(one); Y.norm();
                D.copy(t); D.mul(Y); 
                D.mul(B);
                
                var w=new ctx.FP(A); 
                ctx.FP.tpo(D,w);

                w.mul(B);
                if (w.sign()==1)
                {
                    w.neg();
                    w.norm();
                }
                w.mul(B);
                w.mul(h); w.mul(Y); w.mul(D);

                X1.neg(); X1.norm(); X1.div2();
                X2.copy(X1);
                X1.sub(w); X1.norm();
                X2.add(w); X2.norm();
                A.add(A); A.add(A); A.norm();
                t.sqr(); t.mul(D); t.sqr();
                A.mul(t);
                X3.add(A); X3.norm();

                var rhs=ECP.RHS(X2);
                X3.cmove(X2,rhs.qr(null));
                rhs.copy(ECP.RHS(X1));
                X3.cmove(X1,rhs.qr(null));
                rhs.copy(ECP.RHS(X3));
                Y.copy(rhs.sqrt(null));
  
                var ne=Y.sign()^sgn;
                w.copy(Y); w.neg(); w.norm();
                Y.cmove(w,ne);

                var x=X3.redc();
                var y=Y.redc();
                P.setxy(x,y);
                return P;
            }
        }
        return P;
    };




/*
    ECP.mapit = function(h) {
        var q = new ctx.BIG(0);
        q.rcopy(ctx.ROM_FIELD.Modulus);
        var dx = ctx.DBIG.fromBytes(h);
        var x=dx.mod(q);
        var P=ctx.ECP.hap2point(x);
        P.cfp();
        return P;
    };
*/
    return ECP;
};

if (typeof module !== "undefined" && typeof module.exports !== "undefined") {
    module.exports.ECP = ECP;
}

