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

public final class PAIR8 {

//	public static final boolean GT_STRONG=false;

// Point doubling for pairings
    private static void dbl(ECP8 A, FP8 AA, FP8 BB, FP8 CC) {
        CC.copy(A.getx());		//X
        FP8 YY = new FP8(A.gety());	//Y
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
    private static void add(ECP8 A, ECP8 B, FP8 AA, FP8 BB, FP8 CC) {
        AA.copy(A.getx());    // AA
        CC.copy(A.gety());    // CC
        FP8 T1 = new FP8(A.getz());  // Z1
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
    public static FP48 line(ECP8 A, ECP8 B, FP Qx, FP Qy) {
        FP16 a, b, c;
        FP8 AA = new FP8();
        FP8 BB = new FP8();
        FP8 CC = new FP8();

        if (A == B)
            dbl(A, AA, BB, CC);
        else
            add(A, B, AA, BB, CC);

        CC.tmul(Qx);
        AA.tmul(Qy);

        a = new FP16(AA, BB);

        if (CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.D_TYPE) {
            b = new FP16(CC);           // L(0,1) | L(0,0) | L(1,0)
            c = new FP16();
        }
        if (CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.M_TYPE) {
            b = new FP16();
            c = new FP16(CC); c.times_i();
        }

        FP48 r = new FP48(a, b, c);
        r.settype(FP48.SPARSER);
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
    public static FP48[] initmp() {
        FP48[] r = new FP48[CONFIG_CURVE.ATE_BITS];
        for (int i = CONFIG_CURVE.ATE_BITS - 1; i >= 0; i--)
            r[i] = new FP48(1);
        return r;
    }

    /* basic Miller loop */
    public static FP48 miller(FP48[] r) {
        FP48 res = new FP48(1);
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
    private static FP16 pack(FP8 AA, FP8 BB, FP8 CC) {
        FP8 i = new FP8(CC);
        i.inverse(null);
        FP8 a = new FP8(AA);
        a.mul(i);
        FP8 b = new FP8(BB);
        b.mul(i);
        return new FP16(a, b);
    }

// Unpack G2 line function details and include G1
    private static FP48 unpack(FP16 T, FP Qx, FP Qy) {
        FP16 a, b, c;
        a = new FP16(T);
        a.geta().tmul(Qy);
        FP8 t = new FP8(Qx);
        if (CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.D_TYPE) {
            b = new FP16(t);
            c = new FP16();
        }
        if (CONFIG_CURVE.SEXTIC_TWIST == CONFIG_CURVE.M_TYPE) {
            b = new FP16();
            c = new FP16(t); c.times_i();
        }
        FP48 v = new FP48(a, b, c);
        v.settype(FP48.SPARSEST);
        return v;
    }

// Precompute table of line functions for fixed G2 value
    public static FP16[] precomp(ECP8 GV) {
        BIG n = new BIG(0);
        BIG n3 = new BIG(0);
        FP8 AA = new FP8();
        FP8 BB = new FP8();
        FP8 CC = new FP8();
        int bt;

        ECP8 P = new ECP8(GV);
        ECP8 A = new ECP8();
        A.copy(P);

        ECP8 MP = new ECP8();
        MP.copy(P); MP.neg();

        int nb = lbits(n3, n);
        int j = 0;

        FP16[] T = new FP16[CONFIG_CURVE.G2_TABLE];

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
    public static void another_pc(FP48[] r, FP16[] T, ECP QV) {
        BIG n = new BIG(0);
        BIG n3 = new BIG(0);
        FP48 lv, lv2;
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
    public static void another(FP48[] r, ECP8 P1, ECP Q1) {
        FP2 f;
        BIG n = new BIG(0);
        BIG n3 = new BIG(0);
        FP48 lv, lv2;
        int bt;

        if (Q1.is_infinity()) return;
// P is needed in affine form for line function, Q for (Qx,Qy) extraction
        ECP8 P = new ECP8(P1);
        ECP Q = new ECP(Q1);

        P.affine();
        Q.affine();

        FP Qx = new FP(Q.getx());
        FP Qy = new FP(Q.gety());

        ECP8 A = new ECP8();
        A.copy(P);

        ECP8 MP = new ECP8();
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
    public static FP48 ate(ECP8 P1, ECP Q1) {
        FP2 f;
        BIG n = new BIG(0);
        BIG n3 = new BIG(0);
        FP48 lv, lv2;
        int bt;

        if (Q1.is_infinity()) return new FP48(1);

        ECP8 P = new ECP8(P1);
        ECP Q = new ECP(Q1);

        P.affine();
        Q.affine();

        FP Qx = new FP(Q.getx());
        FP Qy = new FP(Q.gety());

        ECP8 A = new ECP8();
        FP48 r = new FP48(1);
        A.copy(P);

        ECP8 MP = new ECP8();
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
    public static FP48 ate2(ECP8 P1, ECP Q1, ECP8 R1, ECP S1) {
        FP2 f;
        BIG n = new BIG(0);
        BIG n3 = new BIG(0);
        FP48 lv, lv2;
        int bt;

        if (Q1.is_infinity()) return ate(R1, S1);
        if (S1.is_infinity()) return ate(P1, Q1);

        ECP8 P = new ECP8(P1);
        ECP Q = new ECP(Q1);

        P.affine();
        Q.affine();

        ECP8 R = new ECP8(R1);
        ECP S = new ECP(S1);

        R.affine();
        S.affine();

        FP Qx = new FP(Q.getx());
        FP Qy = new FP(Q.gety());
        FP Sx = new FP(S.getx());
        FP Sy = new FP(S.gety());

        ECP8 A = new ECP8();
        ECP8 B = new ECP8();
        FP48 r = new FP48(1);

        A.copy(P);
        B.copy(R);

        ECP8 MP = new ECP8();
        MP.copy(P); MP.neg();
        ECP8 MR = new ECP8();
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
    public static FP48 fexp(FP48 m) {
        FP2 f = new FP2(new BIG(ROM.Fra), new BIG(ROM.Frb));
        BIG x = new BIG(ROM.CURVE_Bnx);
        FP48 r = new FP48(m);

        /* Easy part of final exp */
        FP48 lv = new FP48(r);
        lv.inverse();
        r.conj();
        r.mul(lv);
        lv.copy(r);
        r.frob(f, 8);
        r.mul(lv);


// See https://eprint.iacr.org/2020/875.pdf
        FP48 t0,y0,y1;
        y1=new FP48(r);
        y1.usqr();
        y1.mul(r); // y1=r^3

        y0=new FP48(r.pow(x));
        if (CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX) {
            y0.conj();
        }
        t0=new FP48(r); t0.conj();
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

// ^(x^4+p^4)
        y0.copy(r.pow(x));
        y0.copy(y0.pow(x));
        y0.copy(y0.pow(x));
        y0.copy(y0.pow(x));
        t0.copy(r);
        t0.frob(f,4);
        r.copy(y0);
        r.mul(t0);

// ^(x^8+p^8-1)
        y0.copy(r.pow(x));
        y0.copy(y0.pow(x));
        y0.copy(y0.pow(x));
        y0.copy(y0.pow(x));
        y0.copy(y0.pow(x));
        y0.copy(y0.pow(x));
        y0.copy(y0.pow(x));
        y0.copy(y0.pow(x));
        t0.copy(r);
        t0.frob(f,8);
        y0.mul(t0);
        t0.copy(r); t0.conj();
        r.copy(y0);
        r.mul(t0);

        r.mul(y1);
        r.reduce();        
        

        /* Hard part of final exp */
/*
// Ghamman & Fouotsa Method
        FP48 t0, t1, t2, t3, t4, t5, t6, t7;
        t7 = new FP48(r); t7.usqr();

        if (x.parity() == 1) {
            t2 = r.pow(x);
            t1 = new FP48(t2); t1.usqr();
            t2 = t2.pow(x);
        } else {
            t1 = t7.pow(x);
            x.fshr(1);
            t2 = t1.pow(x);
            x.fshl(1);
        }

        if (CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX) {
            t1.conj();
        }

        t3 = new FP48(t1); t3.conj();
        t2.mul(t3);
        t2.mul(r);

        r.mul(t7);

        t1 = t2.pow(x);
        if (CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX) {
            t1.conj();
        }
        t3.copy(t1);
        t3.frob(f, 14);
        r.mul(t3);
        t1 = t1.pow(x);
        if (CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX) {
            t1.conj();
        }

        t3.copy(t1);
        t3.frob(f, 13);
        r.mul(t3);
        t1 = t1.pow(x);
        if (CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX) {
            t1.conj();
        }

        t3.copy(t1);
        t3.frob(f, 12);
        r.mul(t3);
        t1 = t1.pow(x);
        if (CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX) {
            t1.conj();
        }

        t3.copy(t1);
        t3.frob(f, 11);
        r.mul(t3);
        t1 = t1.pow(x);
        if (CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX) {
            t1.conj();
        }

        t3.copy(t1);
        t3.frob(f, 10);
        r.mul(t3);
        t1 = t1.pow(x);
        if (CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX) {
            t1.conj();
        }

        t3.copy(t1);
        t3.frob(f, 9);
        r.mul(t3);
        t1 = t1.pow(x);
        if (CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX) {
            t1.conj();
        }

        t3.copy(t1);
        t3.frob(f, 8);
        r.mul(t3);
        t1 = t1.pow(x);
        if (CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX) {
            t1.conj();
        }

        t3.copy(t2); t3.conj();
        t1.mul(t3);
        t3.copy(t1);
        t3.frob(f, 7);
        r.mul(t3);
        t1 = t1.pow(x);
        if (CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX) {
            t1.conj();
        }

        t3.copy(t1);
        t3.frob(f, 6);
        r.mul(t3);
        t1 = t1.pow(x);
        if (CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX) {
            t1.conj();
        }

        t3.copy(t1);
        t3.frob(f, 5);
        r.mul(t3);
        t1 = t1.pow(x);
        if (CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX) {
            t1.conj();
        }

        t3.copy(t1);
        t3.frob(f, 4);
        r.mul(t3);
        t1 = t1.pow(x);
        if (CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX) {
            t1.conj();
        }

        t3.copy(t1);
        t3.frob(f, 3);
        r.mul(t3);
        t1 = t1.pow(x);
        if (CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX) {
            t1.conj();
        }

        t3.copy(t1);
        t3.frob(f, 2);
        r.mul(t3);
        t1 = t1.pow(x);
        if (CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX) {
            t1.conj();
        }

        t3.copy(t1);
        t3.frob(f, 1);
        r.mul(t3);
        t1 = t1.pow(x);
        if (CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX) {
            t1.conj();
        }

        r.mul(t1);
        t2.frob(f, 15);
        r.mul(t2);

        r.reduce();
*/
        return r;
    }

    /* GLV method */
    public static BIG[] glv(BIG ee) {
        BIG[] u = new BIG[2];
// -(x^8).P = (Beta.x,y)
        BIG q = new BIG(ROM.CURVE_Order);
        BIG x = new BIG(ROM.CURVE_Bnx);
        BIG x2 = BIG.smul(x, x);
        x = BIG.smul(x2, x2);
        x2 = BIG.smul(x, x);
        int bd=q.nbits()-x2.nbits();
        u[0] = new BIG(ee);
        u[0].ctmod(x2,bd);
        u[1] = new BIG(ee);
        u[1].ctdiv(x2,bd);
        u[1].rsub(q);

        return u;
    }

    /* Galbraith & Scott Method */
    public static BIG[] gs(BIG ee) {
        BIG[] u = new BIG[16];

        BIG q = new BIG(ROM.CURVE_Order);
        BIG x = new BIG(ROM.CURVE_Bnx);
        int bd=q.nbits()-x.nbits();
        BIG w = new BIG(ee);
        for (int i = 0; i < 15; i++) {
            u[i] = new BIG(w);
            u[i].ctmod(x,bd);
            w.ctdiv(x,bd);
        }
        u[15] = new BIG(w);
        if (CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX) {
            u[1].copy(BIG.modneg(u[1], q));
            u[3].copy(BIG.modneg(u[3], q));
            u[5].copy(BIG.modneg(u[5], q));
            u[7].copy(BIG.modneg(u[7], q));
            u[9].copy(BIG.modneg(u[9], q));
            u[11].copy(BIG.modneg(u[11], q));
            u[13].copy(BIG.modneg(u[13], q));
            u[15].copy(BIG.modneg(u[15], q));
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
    public static ECP8 G2mul(ECP8 P, BIG e) {
        ECP8 R;
        BIG q = new BIG(ROM.CURVE_Order);
        BIG ee = new BIG(e); ee.mod(q);
        if (CONFIG_CURVE.USE_GS_G2) {
            ECP8[] Q = new ECP8[16];
            FP2[] F = ECP8.frob_constants();

            BIG[] u = gs(ee);

            BIG t = new BIG(0);
            int i, np, nn;

            Q[0] = new ECP8(); Q[0].copy(P);
            for (i = 1; i < 16; i++) {
                Q[i] = new ECP8(); Q[i].copy(Q[i - 1]);
                Q[i].frob(F, 1);
            }
            //System.out.println("");
            for (i = 0; i < 16; i++) {
                np = u[i].nbits();
                t.copy(BIG.modneg(u[i], q));
                nn = t.nbits();
                if (nn < np) {
                    u[i].copy(t);
                    Q[i].neg();
                }
                u[i].norm();
                //System.out.println("Q= "+Q[i].toString());
                //System.out.println("u= "+u[i].toString());
            }

            R = ECP8.mul16(Q, u);
        } else {
            R = P.mul(ee);
        }
        return R;
    }

    /* f=f^e */
    /* Note that this method requires a lot of RAM! Better to use compressed XTR method, see FP16.java */
    public static FP48 GTpow(FP48 d, BIG e) {
        FP48 r;
        BIG q = new BIG(ROM.CURVE_Order);
        BIG ee = new BIG(e); ee.mod(q);
        if (CONFIG_CURVE.USE_GS_GT) {
            FP48[] g = new FP48[16];
            FP2 f = new FP2(new BIG(ROM.Fra), new BIG(ROM.Frb));
            BIG t = new BIG(0);
            int i, np, nn;
            BIG[] u = gs(ee);

            g[0] = new FP48(d);
            for (i = 1; i < 16; i++) {
                g[i] = new FP48(); g[i].copy(g[i - 1]);
                g[i].frob(f, 1);
            }
            for (i = 0; i < 16; i++) {
                np = u[i].nbits();
                t.copy(BIG.modneg(u[i], q));
                nn = t.nbits();
                if (nn < np) {
                    u[i].copy(t);
                    g[i].conj();
                }
                u[i].norm();
            }
            r = FP48.pow16(g, u);
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
        T=T.mul(x); T=T.mul(x); T=T.mul(x); T=T.mul(x); T=T.mul(x); T=T.mul(x); T=T.mul(x); T.neg();
        W.getx().mul(cru);
        if (!W.equals(T)) return false;

// Not needed
//        W.add(P);
//        T.getx().mul(cru);
//        W.add(T);
//        if (!W.is_infinity()) return false;
/*
		BIG q=new BIG(ROM.CURVE_Order);  
        if (P.is_infinity()) return false;
        ECP W=P.mul(q);
        if (!W.is_infinity()) return false; */
        return true;
    }

/* test G2 group membership */
    public static boolean G2member(ECP8 P)
    {
        if (P.is_infinity()) return false;
        FP2[] F = ECP8.frob_constants();
        BIG x = new BIG(ROM.CURVE_Bnx);

        ECP8 W=new ECP8(P); W.frob(F,1);
        ECP8 T=P.mul(x);

        if (CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX) {
            T.neg();
        }
/*
        ECP8 R=new ECP8(W);
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
        ECP8 W=P.mul(q);
        if (!W.is_infinity()) return false;
        return true; */
    }

/* Check that m is in cyclotomic sub-group */
/* Check that m!=1, conj(m)*m==1, and m.m^{p^16}=m^{p^8} */
    public static boolean GTcyclotomic(FP48 m)
    {
        if (m.isunity()) return false;
        FP48 r=new FP48(m);
        r.conj();
        r.mul(m);
        if (!r.isunity()) return false;

        FP2 f=new FP2(new BIG(ROM.Fra),new BIG(ROM.Frb));

        r.copy(m); r.frob(f,8); 
        FP48 w=new FP48(r); w.frob(f,8);
        w.mul(m);

        if (!w.equals(r)) return false;
        return true;
    }

/* test for full GT group membership */
    public static boolean GTmember(FP48 m)
    {
        if (!GTcyclotomic(m)) return false;

        FP2 f=new FP2(new BIG(ROM.Fra),new BIG(ROM.Frb));
        BIG x = new BIG(ROM.CURVE_Bnx);

        FP48 r=new FP48(m); r.frob(f,1);
        FP48 t=m.pow(x);

        if (CONFIG_CURVE.SIGN_OF_X == CONFIG_CURVE.NEGATIVEX) {
            t.conj();
        }    
        if (r.equals(t))
            return true;
        return false;

/*
        BIG q=new BIG(ROM.CURVE_Order);
        FP48 r=m.pow(q);
        if (!r.isunity()) return false;
        return true; */
    }
}

