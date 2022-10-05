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

/* CORE BLS Curve Pairing functions */

package org.miracl.core.XXX;

public final class PAIR4 {

//	public static final boolean GT_STRONG=false;

// Point doubling for pairings
    private static void dbl(ECP4 A, FP4 AA, FP4 BB, FP4 CC) {
        CC.copy(A.getx());		//X
        FP4 YY = new FP4(A.gety());	//Y
        BB.copy(A.getz());		//Z
        AA.copy(YY);

        AA.mul(BB);                //AA
        CC.sqr();	               //X^2
        YY.sqr();	               //Y^2
        BB.sqr();			       //Z^2

        AA.add(AA);
        AA.neg(); AA.norm();       //-2AA
        AA.times_i();

        int sb = 3 * ROM.CURVE_B_I;
        BB.imul(sb);
        CC.imul(3);
        if (CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.D_TYPE) {
            YY.times_i();
            CC.times_i();
        }
        if (CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.M_TYPE) {
            BB.times_i();
        }
        BB.sub(YY);
        BB.norm();

        A.dbl();


    }

// Point addition for pairings
    private static void add(ECP4 A, ECP4 B, FP4 AA, FP4 BB, FP4 CC) {
        AA.copy(A.getx());    // AA
        CC.copy(A.gety());    // CC
        FP4 T1 = new FP4(A.getz());  // Z1
        BB.copy(T1);    // Z1

        T1.mul(B.gety());    // T1=Z1.Y2
        BB.mul(B.getx());    // BB=Z1.X2

        AA.sub(BB); AA.norm();  // AA=AA-Z1.X2
        CC.sub(T1); CC.norm();  // CC=CC-Z1.Y2

        T1.copy(AA);            // T1=AA-Z1.X2

        if (CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.M_TYPE) {
            AA.times_i();
            AA.norm();
        }

        T1.mul(B.gety());       // T1=(AA-Z1.X2).Y2

        BB.copy(CC);            // BB=CC-Z1.Y2
        BB.mul(B.getx());       // BB=(CC-Z1.Y2).X2
        BB.sub(T1); BB.norm();          // BB=(CC-Z1.Y2).X2 - (AA-Z1.X2).Y2
        CC.neg(); CC.norm(); // CC=-(CC-Z1.Y2).Xs

        A.add(B);
    }

    /* Line function */
    public static FP24 line(ECP4 A, ECP4 B, FP Qx, FP Qy) {
        FP8 a, b, c;
        FP4 AA = new FP4();
        FP4 BB = new FP4();
        FP4 CC = new FP4();

        if (A == B)
            dbl(A, AA, BB, CC);
        else
            add(A, B, AA, BB, CC);

        CC.qmul(Qx);
        AA.qmul(Qy);

        a = new FP8(AA, BB);

        if (CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.D_TYPE) {
            b = new FP8(CC);           // L(0,1) | L(0,0) | L(1,0)
            c = new FP8();
        }
        if (CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.M_TYPE) {
            b = new FP8();
            c = new FP8(CC); c.times_i();
        }

        FP24 r = new FP24(a, b, c);
        r.settype(FP24.SPARSER);
        return r;
    }

    /* prepare ate parameter, n=6u+2 (BN) or n=u (BLS), n3=3*n */
    public static int lbits(BIG n3, BIG n) {
        n.copy(new BIG(ROM.CURVE_Bnx));
        n3.copy(n);
        n3.pmul(3);
        n3.norm();
        return n3.nbits();
    }

    /* prepare for multi-pairing */
    public static FP24[] initmp() {
        FP24[] r = new FP24[CONFIG_CURVE.ATE_BITS];
        for (int i = CONFIG_CURVE.ATE_BITS - 1; i >= 0; i--)
            r[i] = new FP24(1);
        return r;
    }

    /* basic Miller loop */
    public static FP24 miller(FP24[] r) {
        FP24 res = new FP24(1);
        for (int i = CONFIG_CURVE.ATE_BITS - 1; i >= 1; i--) {
            res.sqr();
            res.ssmul(r[i]);
            r[i].zero();
        }

        if (CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX)
            res.conj();
        res.ssmul(r[0]);
        r[0].zero();

        return res;
    }

// Store precomputed line details in an FP4
    private static FP8 pack(FP4 AA, FP4 BB, FP4 CC) {
        FP4 i = new FP4(CC);
        i.inverse(null);
        FP4 a = new FP4(AA);
        a.mul(i);
        FP4 b = new FP4(BB);
        b.mul(i);
        return new FP8(a, b);
    }

// Unpack G2 line function details and include G1
    private static FP24 unpack(FP8 T, FP Qx, FP Qy) {
        FP8 a, b, c;
        a = new FP8(T);
        a.geta().qmul(Qy);
        FP4 t = new FP4(Qx);
        if (CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.D_TYPE) {
            b = new FP8(t);
            c = new FP8();
        }
        if (CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.M_TYPE) {
            b = new FP8();
            c = new FP8(t); c.times_i();
        }
        FP24 v = new FP24(a, b, c);
        v.settype(FP24.SPARSEST);
        return v;
    }

// Precompute table of line functions for fixed G2 value
    public static FP8[] precomp(ECP4 GV) {
        BIG n = new BIG(0);
        BIG n3 = new BIG(0);
        FP4 AA = new FP4();
        FP4 BB = new FP4();
        FP4 CC = new FP4();
        int bt;

        ECP4 P = new ECP4(GV);
        ECP4 A = new ECP4();
        A.copy(P);

        ECP4 MP = new ECP4();
        MP.copy(P); MP.neg();

        int nb = lbits(n3, n);
        int j = 0;

        FP8[] T = new FP8[CONFIG_CURVE.G2_TABLE];

        for (int i = nb - 2; i >= 1; i--) {
            dbl(A, AA, BB, CC);
            T[j++] = pack(AA, BB, CC);

            bt = n3.bit(i) - n.bit(i);
            if (bt == 1) {
                add(A, P, AA, BB, CC);
                T[j++] = pack(AA, BB, CC);

            }
            if (bt == -1) {
                add(A, MP, AA, BB, CC);
                T[j++] = pack(AA, BB, CC);
            }
        }
        return T;
    }

    /* Accumulate another set of line functions for n-pairing, assuming precomputation on G2 */
    public static void another_pc(FP24[] r, FP8[] T, ECP QV) {
        BIG n = new BIG(0);
        BIG n3 = new BIG(0);
        FP24 lv, lv2;
        int bt;

        if (QV.is_infinity()) return;

        ECP Q = new ECP(QV);
        Q.affine();

        FP Qx = new FP(Q.getx());
        FP Qy = new FP(Q.gety());

        int nb = lbits(n3, n);
        int j = 0;

        for (int i = nb - 2; i >= 1; i--) {
            lv = unpack(T[j++], Qx, Qy);

            bt = n3.bit(i) - n.bit(i);
            if (bt == 1) {
                lv2 = unpack(T[j++], Qx, Qy);
                lv.smul(lv2);
            }
            if (bt == -1) {
                lv2 = unpack(T[j++], Qx, Qy);
                lv.smul(lv2);
            }
            r[i].ssmul(lv);
        }
    }

    /* Accumulate another set of line functions for n-pairing */
    public static void another(FP24[] r, ECP4 P1, ECP Q1) {
        FP2 f;
        BIG n = new BIG(0);
        BIG n3 = new BIG(0);
        FP24 lv, lv2;
        int bt;

        if (Q1.is_infinity()) return;
// P is needed in affine form for line function, Q for (Qx,Qy) extraction
        ECP4 P = new ECP4(P1);
        ECP Q = new ECP(Q1);

        P.affine();
        Q.affine();

        FP Qx = new FP(Q.getx());
        FP Qy = new FP(Q.gety());

        ECP4 A = new ECP4();
        A.copy(P);

        ECP4 MP = new ECP4();
        MP.copy(P); MP.neg();

        int nb = lbits(n3, n);

        for (int i = nb - 2; i >= 1; i--) {
            lv = line(A, A, Qx, Qy);

            bt = n3.bit(i) - n.bit(i);
            if (bt == 1) {
                lv2 = line(A, P, Qx, Qy);
                lv.smul(lv2);
            }
            if (bt == -1) {
                lv2 = line(A, MP, Qx, Qy);
                lv.smul(lv2);
            }
            r[i].ssmul(lv);
        }

    }

    /* Optimal R-ate pairing */
    public static FP24 ate(ECP4 P1, ECP Q1) {
        FP2 f;
        BIG n = new BIG(0);
        BIG n3 = new BIG(0);
        FP24 lv, lv2;
        int bt;

        if (Q1.is_infinity()) return new FP24(1);

        ECP4 P = new ECP4(P1);
        ECP Q = new ECP(Q1);

        P.affine();
        Q.affine();

        FP Qx = new FP(Q.getx());
        FP Qy = new FP(Q.gety());

        ECP4 A = new ECP4();
        FP24 r = new FP24(1);
        A.copy(P);

        ECP4 MP = new ECP4();
        MP.copy(P); MP.neg();

        int nb = lbits(n3, n);

        for (int i = nb - 2; i >= 1; i--) {
            r.sqr();
            lv = line(A, A, Qx, Qy);

            bt = n3.bit(i) - n.bit(i);
            if (bt == 1) {
                lv2 = line(A, P, Qx, Qy);
                lv.smul(lv2);
            }
            if (bt == -1) {
                lv2 = line(A, MP, Qx, Qy);
                lv.smul(lv2);
            }
            r.ssmul(lv);
        }

        if (CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX) {
            r.conj();
        }

        return r;
    }

    /* Optimal R-ate double pairing e(P,Q).e(R,S) */
    public static FP24 ate2(ECP4 P1, ECP Q1, ECP4 R1, ECP S1) {
        FP2 f;
        BIG n = new BIG(0);
        BIG n3 = new BIG(0);
        FP24 lv, lv2;
        int bt;

        if (Q1.is_infinity()) return ate(R1, S1);
        if (S1.is_infinity()) return ate(P1, Q1);

        ECP4 P = new ECP4(P1);
        ECP Q = new ECP(Q1);

        P.affine();
        Q.affine();

        ECP4 R = new ECP4(R1);
        ECP S = new ECP(S1);

        R.affine();
        S.affine();

        FP Qx = new FP(Q.getx());
        FP Qy = new FP(Q.gety());
        FP Sx = new FP(S.getx());
        FP Sy = new FP(S.gety());

        ECP4 A = new ECP4();
        ECP4 B = new ECP4();
        FP24 r = new FP24(1);

        A.copy(P);
        B.copy(R);

        ECP4 MP = new ECP4();
        MP.copy(P); MP.neg();
        ECP4 MR = new ECP4();
        MR.copy(R); MR.neg();


        int nb = lbits(n3, n);

        for (int i = nb - 2; i >= 1; i--) {
            r.sqr();
            lv = line(A, A, Qx, Qy);
            lv2 = line(B, B, Sx, Sy);
            lv.smul(lv2);
            r.ssmul(lv);

            bt = n3.bit(i) - n.bit(i);
            if (bt == 1) {
                lv = line(A, P, Qx, Qy);
                lv2 = line(B, R, Sx, Sy);
                lv.smul(lv2);
                r.ssmul(lv);
            }
            if (bt == -1) {
                lv = line(A, MP, Qx, Qy);
                lv2 = line(B, MR, Sx, Sy);
                lv.smul(lv2);
                r.ssmul(lv);
            }
        }

        if (CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX) {
            r.conj();
        }

        return r;
    }

    /* final exponentiation - keep separate for multi-pairings and to avoid thrashing stack */
    public static FP24 fexp(FP24 m) {
        FP2 f = new FP2(new BIG(ROM.Fra), new BIG(ROM.Frb));
        BIG x = new BIG(ROM.CURVE_Bnx);
        FP24 r = new FP24(m);

        /* Easy part of final exp */
        FP24 lv = new FP24(r);
        lv.inverse();
        r.conj();

        r.mul(lv);
        lv.copy(r);
        r.frob(f, 4);
        r.mul(lv);


        /* Hard part of final exp */

// See https://eprint.iacr.org/2020/875.pdf
        FP24 t0,y0,y1;
        y1=new FP24(r);
        y1.usqr();
        y1.mul(r); // y1=r^3

        y0=new FP24(r.pow(x));
        if (CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX) {
            y0.conj();
        }
        t0=new FP24(r); t0.conj();
        r.copy(y0);
        r.mul(t0);

        y0.copy(r.pow(x));
        if (CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX) {
            y0.conj();
        }
        t0.copy(r); t0.conj();
        r.copy(y0);
        r.mul(t0);

// ^(x+p)
        y0.copy(r.pow(x));
        if (CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX) {
            y0.conj();
        }
        t0.copy(r);
        t0.frob(f,1);
        r.copy(y0);
        r.mul(t0);

// ^(x^2+p^2)
        y0.copy(r.pow(x));
        y0.copy(y0.pow(x));
        t0.copy(r);
        t0.frob(f,2);
        r.copy(y0);
        r.mul(t0);

// ^(x^4+p^4-1)
        y0.copy(r.pow(x));
        y0.copy(y0.pow(x));
        y0.copy(y0.pow(x));
        y0.copy(y0.pow(x));
        t0.copy(r);
        t0.frob(f,4);
        y0.mul(t0);
        t0.copy(r); t0.conj();
        r.copy(y0);
        r.mul(t0);

        r.mul(y1);
        r.reduce();
/*

// Ghamman & Fouotsa Method
        FP24 t0, t1, t2, t3, t4, t5, t6, t7;
        t7 = new FP24(r); t7.usqr();
        t1 = t7.pow(x);

        x.fshr(1);
        t2 = t1.pow(x);
        x.fshl(1);

        if (CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX) {
            t1.conj();
        }
        t3 = new FP24(t1); t3.conj();
        t2.mul(t3);
        t2.mul(r);

        t3 = t2.pow(x);
        t4 = t3.pow(x);
        t5 = t4.pow(x);

        if (CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX) {
            t3.conj(); t5.conj();
        }

        t3.frob(f, 6); t4.frob(f, 5);
        t3.mul(t4);

        t6 = t5.pow(x);
        if (CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX) {
            t6.conj();
        }

        t5.frob(f, 4);
        t3.mul(t5);

        t0 = new FP24(t2); t0.conj();
        t6.mul(t0);

        t5.copy(t6);
        t5.frob(f, 3);

        t3.mul(t5);
        t5 = t6.pow(x);
        t6 = t5.pow(x);

        if (CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX) {
            t5.conj();
        }

        t0.copy(t5);
        t0.frob(f, 2);
        t3.mul(t0);
        t0.copy(t6);
        t0.frob(f, 1);

        t3.mul(t0);
        t5 = t6.pow(x);

        if (CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX) {
            t5.conj();
        }
        t2.frob(f, 7);

        t5.mul(t7);
        t3.mul(t2);
        t3.mul(t5);

        r.mul(t3);

        r.reduce();
*/
        return r;

    }

    /* GLV method */
    public static BIG[] glv(BIG ee) {
        BIG[] u = new BIG[2];
// -(x^4).P = (Beta.x,y)
        BIG q = new BIG(ROM.CURVE_Order);
        BIG x = new BIG(ROM.CURVE_Bnx);
        BIG x2 = BIG.smul(x, x);
        x = BIG.smul(x2, x2);
        int bd = q.nbits()-x.nbits();
        u[0] = new BIG(ee);
        u[0].ctmod(x,bd);
        u[1] = new BIG(ee);
        u[1].ctdiv(x,bd);
        u[1].rsub(q);

        return u;
    }

    /* Galbraith & Scott Method */
    public static BIG[] gs(BIG ee) {
        BIG[] u = new BIG[8];
        BIG q = new BIG(ROM.CURVE_Order);
        BIG x = new BIG(ROM.CURVE_Bnx);
        int bd = q.nbits()-x.nbits();
        BIG w = new BIG(ee);
        for (int i = 0; i < 7; i++) {
            u[i] = new BIG(w);
            u[i].ctmod(x,bd);
            w.ctdiv(x,bd);
        }
        u[7] = new BIG(w);
        if (CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX) {
            u[1].copy(BIG.modneg(u[1], q));
            u[3].copy(BIG.modneg(u[3], q));
            u[5].copy(BIG.modneg(u[5], q));
            u[7].copy(BIG.modneg(u[7], q));
        }

        return u;
    }

    /* Multiply P by e in group G1 */
    public static ECP G1mul(ECP P, BIG e) {
        ECP R;
        BIG q = new BIG(ROM.CURVE_Order);
        BIG ee = new BIG(e); ee.mod(q);
        if (CONFIG_CURVE.USE_GLV) {
            R = new ECP();
            R.copy(P);
            int i, np, nn;
            ECP Q = new ECP();
            Q.copy(P); Q.affine();
 
            FP cru = new FP(new BIG(ROM.CRu));
            BIG t = new BIG(0);
            BIG[] u = glv(ee);
            Q.getx().mul(cru);

            np = u[0].nbits();
            t.copy(BIG.modneg(u[0], q));
            nn = t.nbits();
            if (nn < np) {
                u[0].copy(t);
                R.neg();
            }

            np = u[1].nbits();
            t.copy(BIG.modneg(u[1], q));
            nn = t.nbits();
            if (nn < np) {
                u[1].copy(t);
                Q.neg();
            }
            u[0].norm();
            u[1].norm();
            R = R.mul2(u[0], Q, u[1]);

        } else {
            R = P.clmul(ee,q);
        }
        return R;
    }

    /* Multiply P by e in group G2 */
    public static ECP4 G2mul(ECP4 P, BIG e) {
        ECP4 R;
        BIG q = new BIG(ROM.CURVE_Order);
        BIG ee = new BIG(e); ee.mod(q);
        if (CONFIG_CURVE.USE_GS_G2) {
            ECP4[] Q = new ECP4[8];
            FP2[] F = ECP4.frob_constants();

            BIG[] u = gs(ee);

            BIG t = new BIG(0);
            int i, np, nn;

            Q[0] = new ECP4(); Q[0].copy(P);
            for (i = 1; i < 8; i++) {
                Q[i] = new ECP4(); Q[i].copy(Q[i - 1]);
                Q[i].frob(F, 1);
            }
            for (i = 0; i < 8; i++) {
                np = u[i].nbits();
                t.copy(BIG.modneg(u[i], q));
                nn = t.nbits();
                if (nn < np) {
                    u[i].copy(t);
                    Q[i].neg();
                }
                u[i].norm();
            }

            R = ECP4.mul8(Q, u);
        } else {
            R = P.mul(e);
        }
        return R;
    }

    /* f=f^e */
    /* Note that this method requires a lot of RAM! Better to use compressed XTR method, see FP8.java */
    public static FP24 GTpow(FP24 d, BIG e) {
        FP24 r;
        BIG q = new BIG(ROM.CURVE_Order);
        BIG ee = new BIG(e); ee.mod(q);
        if (CONFIG_CURVE.USE_GS_GT) {
            FP24[] g = new FP24[8];
            FP2 f = new FP2(new BIG(ROM.Fra), new BIG(ROM.Frb));
 
            BIG t = new BIG(0);
            int i, np, nn;
            BIG[] u = gs(ee);

            g[0] = new FP24(d);
            for (i = 1; i < 8; i++) {
                g[i] = new FP24(); g[i].copy(g[i - 1]);
                g[i].frob(f, 1);
            }
            for (i = 0; i < 8; i++) {
                np = u[i].nbits();
                t.copy(BIG.modneg(u[i], q));
                nn = t.nbits();
                if (nn < np) {
                    u[i].copy(t);
                    g[i].conj();
                }
                u[i].norm();
            }
            r = FP24.pow8(g, u);
        } else {
            r = d.pow(ee);
        }
        return r;
    }

/* test G1 group membership */
    public static boolean G1member(ECP P)
    {
        if (P.is_infinity()) return false;
        BIG x = new BIG(ROM.CURVE_Bnx);
        FP cru = new FP(new BIG(ROM.CRu));
        ECP W=new ECP(P);
        ECP T=P.mul(x); 
        if (P.equals(T)) return false;    // P is of low order        
        T=T.mul(x); T=T.mul(x); T=T.mul(x); T.neg();
        W.getx().mul(cru);
        if (!W.equals(T)) return false;

// Not needed
//        W.add(P);
//        T.getx().mul(cru);
//        W.add(T);
//        if (!W.is_infinity()) return false;

/*		BIG q=new BIG(ROM.CURVE_Order);  
        if (P.is_infinity()) return false;
        ECP W=P.mul(q);
        if (!W.is_infinity()) return false; */
        return true;
    }

/* test G2 group membership */
    public static boolean G2member(ECP4 P)
    {
        if (P.is_infinity()) return false;
        FP2[] F = ECP4.frob_constants();
        BIG x = new BIG(ROM.CURVE_Bnx);

        ECP4 W=new ECP4(P); W.frob(F,1);
        ECP4 T=P.mul(x);

        if (CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX) {
            T.neg();
        }
/*
        ECP4 R=new ECP4(W);
        R.frob(F,1);
        W.sub(R);
        R.copy(T);
        R.frob(F,1);
        W.add(R);
*/
        if (W.equals(T))
            return true;
        return false;
/*
		BIG q=new BIG(ROM.CURVE_Order);  
        if (P.is_infinity()) return false;
        ECP4 W=P.mul(q);
        if (!W.is_infinity()) return false;
        return true; */
    }

/* Check that m is in cyclotomic sub-group */
/* Check that m!=1, conj(m)*m==1, and m.m^{p^8}=m^{p^4} */
    public static boolean GTcyclotomic(FP24 m)
    {
        if (m.isunity()) return false;
        FP24 r=new FP24(m);
        r.conj();
        r.mul(m);
        if (!r.isunity()) return false;

        FP2 f=new FP2(new BIG(ROM.Fra),new BIG(ROM.Frb));

        r.copy(m); r.frob(f,4); 
        FP24 w=new FP24(r); w.frob(f,4);
        w.mul(m);

        if (!w.equals(r)) return false;
        return true;
    }

/* test for full GT group membership */
    public static boolean GTmember(FP24 m)
    {
        if (!GTcyclotomic(m)) return false;
        FP2 f=new FP2(new BIG(ROM.Fra),new BIG(ROM.Frb));
        BIG x = new BIG(ROM.CURVE_Bnx);

        FP24 r=new FP24(m); r.frob(f,1);
        FP24 t=m.pow(x);

        if (CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX) {
            t.conj();
        }    
        if (r.equals(t))
            return true;
        return false;

/*        BIG q=new BIG(ROM.CURVE_Order);
        FP24 r=m.pow(q);
        if (!r.isunity()) return false;
        return true; */
    }
}

