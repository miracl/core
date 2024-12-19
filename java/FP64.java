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
/* Finite Field arithmetic */
/* CORE mod p functions */

package org.miracl.core.XXX;
import org.miracl.core.RAND;

public final class FP {

    public static final long OMASK = (long)(-1) << (CONFIG_FIELD.MODBITS % CONFIG_BIG.BASEBITS);
    public static final int TBITS = CONFIG_FIELD.MODBITS % CONFIG_BIG.BASEBITS; // Number of active bits in top word
    public static final long TMASK = ((long)1 << TBITS) - 1;

    public final BIG x;
    public int XES;

    /**************** 64-bit specific ************************/

    /* reduce a DBIG to a BIG using the appropriate form of the modulus */
    public static BIG mod(DBIG d) {
        if (CONFIG_FIELD.MODTYPE == CONFIG_FIELD.PSEUDO_MERSENNE) {
            BIG b;
            long v, tw;
            BIG t = d.split(CONFIG_FIELD.MODBITS);
            b = new BIG(d);

            v = t.pmul((int)ROM.MConst);

            t.add(b);
            t.norm();

            tw = t.w[BIG.NLEN - 1];
            t.w[BIG.NLEN - 1] &= FP.TMASK;
            t.w[0] += (ROM.MConst * ((tw >> TBITS) + (v << (CONFIG_BIG.BASEBITS - TBITS))));

            t.norm();
            return t;
        }
        if (CONFIG_FIELD.MODTYPE == CONFIG_FIELD.MONTGOMERY_FRIENDLY) {
            BIG b;
            long[] cr = new long[2];
            for (int i = 0; i < BIG.NLEN; i++) {
                cr = BIG.muladd(d.w[i], ROM.MConst - 1, d.w[i], d.w[BIG.NLEN + i - 1]);
                d.w[BIG.NLEN + i] += cr[0];
                d.w[BIG.NLEN + i - 1] = cr[1];
            }

            b = new BIG(0);
            for (int i = 0; i < BIG.NLEN; i++ )
                b.w[i] = d.w[BIG.NLEN + i];
            b.norm();
            return b;
        }
        if (CONFIG_FIELD.MODTYPE == CONFIG_FIELD.GENERALISED_MERSENNE) {
            // GoldiLocks Only
            BIG b;
            BIG t = d.split(CONFIG_FIELD.MODBITS);
            b = new BIG(d);
            b.add(t);
            DBIG dd = new DBIG(t);
            dd.shl(CONFIG_FIELD.MODBITS / 2);

            BIG tt = dd.split(CONFIG_FIELD.MODBITS);
            BIG lo = new BIG(dd);
            b.add(tt);
            b.add(lo);
            b.norm();
            tt.shl(CONFIG_FIELD.MODBITS / 2);
            b.add(tt);

            long carry = b.w[BIG.NLEN - 1] >> TBITS;
            b.w[BIG.NLEN - 1] &= FP.TMASK;
            b.w[0] += carry;

            b.w[224 / CONFIG_BIG.BASEBITS] += carry << (224 % CONFIG_BIG.BASEBITS);
            b.norm();
            return b;
        }
        if (CONFIG_FIELD.MODTYPE == CONFIG_FIELD.NOT_SPECIAL) {
            return BIG.monty(new BIG(ROM.Modulus), ROM.MConst, d);
        }

        return new BIG(0);
    }

    private static int quo(BIG n, BIG m) {
        int sh;
        long num, den;
        int hb = BIG.CHUNK / 2;
        if (TBITS < hb) {
            sh = hb - TBITS;
            num = (n.w[BIG.NLEN - 1] << sh) | (n.w[BIG.NLEN - 2] >> (CONFIG_BIG.BASEBITS - sh));
            den = (m.w[BIG.NLEN - 1] << sh) | (m.w[BIG.NLEN - 2] >> (CONFIG_BIG.BASEBITS - sh));
        } else {
            num = n.w[BIG.NLEN - 1];
            den = m.w[BIG.NLEN - 1];
        }
        return (int)(num / (den + 1));
    }

    /* reduce this mod Modulus */
    public void reduce() {
        BIG m = new BIG(ROM.Modulus);
        BIG r = new BIG(ROM.Modulus);
        int sr, sb, q;
        long carry;
        x.norm();

        if (XES > 16) {
            q = quo(x, m);
            carry = r.pmul(q);
            r.w[BIG.NLEN - 1] += (carry << CONFIG_BIG.BASEBITS); // correction - put any carry out back in again
            x.sub(r);
            x.norm();
            sb = 2;
        } else  sb = logb2(XES - 1);

        m.fshl(sb);
        while (sb > 0) {
// constant time...
            sr = BIG.ssn(r, x, m); // optimized combined shift, subtract and norm
            x.cmove(r, 1 - sr);
            sb--;
        }

        XES = 1;
    }

    /*********************************************************/


    /* Constructors */
    public FP(int a) {
        if (a<0)
        {
            BIG m = new BIG(ROM.Modulus);
            m.inc(a); m.norm();
            x=new BIG(m);
        } else {
            x = new BIG(a);
        }
        nres();
    }

    public FP() {
        x = new BIG(0);
        XES = 1;
    }

    public FP(BIG a) {
        x = new BIG(a);
        nres();
    }

    public FP(FP a) {
        x = new BIG(a.x);
        XES = a.XES;
    }

    public FP(RAND rng)
    {
        BIG m = new BIG(ROM.Modulus);
        x = BIG.randomnum(m, rng);
        nres();
    }

    /* convert to string */
    public String toString() {
        String s = redc().toString();
        return s;
    }

    public String toRawString() {
        String s = x.toRawString();
        return s;
    }

    /* convert to Montgomery n-residue form */
    public void nres() {
        if (CONFIG_FIELD.MODTYPE != CONFIG_FIELD.PSEUDO_MERSENNE && CONFIG_FIELD.MODTYPE != CONFIG_FIELD.GENERALISED_MERSENNE) {
            DBIG d = BIG.mul(x, new BIG(ROM.R2modp)); /*** Change ***/
            x.copy(mod(d));
            XES = 2;
        } else {
            BIG m = new BIG(ROM.Modulus);
            x.mod(m);
            XES = 1;
        }
    }

    /* convert back to regular form */
    public BIG redc() {
        reduce();
        if (CONFIG_FIELD.MODTYPE != CONFIG_FIELD.PSEUDO_MERSENNE && CONFIG_FIELD.MODTYPE != CONFIG_FIELD.GENERALISED_MERSENNE) {
            DBIG d = new DBIG(x);
            return mod(d);
        } else {
            BIG r = new BIG(x);
            return r;
        }
    }

    /* test this=0? */
    public boolean iszilch() {
        FP z = new FP(this);
        z.reduce();
        return z.x.iszilch();

    }

    public int islarger() {
        if (iszilch()) return 0;
        BIG sx = new BIG(ROM.Modulus);
        BIG fx=redc();
        sx.sub(fx); sx.norm();
        return BIG.comp(fx,sx);
    }

    public void toBytes(byte[] b) {
        redc().toBytes(b);
    }

    public static FP fromBytes(byte[] b) {
        BIG t=BIG.fromBytes(b);
        return new FP(t);
    }

    /* copy from FP b */
    public void copy(FP b) {
        x.copy(b.x);
        XES = b.XES;
    }

    /* set this=0 */
    public void zero() {
        x.zero();
        XES = 1;
    }

    /* set this=1 */
    public void one() {
        x.one(); nres();
    }

/* get sign */ 
    public int sign()
    {
        if (CONFIG_FIELD.BIG_ENDIAN_SIGN)
        {
            BIG m = new BIG(ROM.Modulus);
            m.dec(1);
            m.fshr(1);
            FP n = new FP(this);
            n.reduce();
            BIG w=n.redc();
            int cp=BIG.comp(w,m);
            return ((cp+1)&2)>>1;
        } else {
            FP n = new FP(this);
            n.reduce();
            return n.redc().parity();
        }
    }

    public boolean isunity()
    {
        FP n= new FP(this);
        n.reduce();
        return n.redc().isunity();
    }

    /* normalise this */
    public void norm() {
        x.norm();
    }

    /* swap FPs depending on d */
    public void cswap(FP b, int d) {
        x.cswap(b.x, d);
        int t, c = d;
        c = ~(c - 1);
        t = c & (XES ^ b.XES);
        XES ^= t;
        b.XES ^= t;
    }

    /* copy FPs depending on d */
    public void cmove(FP b, int d) {
        x.cmove(b.x, d);
        XES ^= (XES ^ b.XES) & (-d);

    }

    /* this*=b mod Modulus */
    public void mul(FP b) {
        if ((long)XES * b.XES > (long)CONFIG_FIELD.FEXCESS) reduce();

        DBIG d = BIG.mul(x, b.x);
        x.copy(mod(d));
        XES = 2;
    }

    /* this*=c mod Modulus, where c is a small int */
    public void imul(int c) {
//		norm();
        boolean s = false;
        if (c < 0) {
            c = -c;
            s = true;
        }

        if (CONFIG_FIELD.MODTYPE == CONFIG_FIELD.PSEUDO_MERSENNE || CONFIG_FIELD.MODTYPE == CONFIG_FIELD.GENERALISED_MERSENNE) {
            DBIG d = x.pxmul(c);
            x.copy(mod(d));
            XES = 2;
        } else {
            if (XES * c <= CONFIG_FIELD.FEXCESS) {
                x.pmul(c);
                XES *= c;
            } else {
                // this is not good
                FP n = new FP(c);
                mul(n);
            }
        }

        if (s) {neg(); norm();}
    }

    /* this*=this mod Modulus */
    public void sqr() {
        DBIG d;
        if ((long)XES * XES > (long)CONFIG_FIELD.FEXCESS) reduce();

        d = BIG.sqr(x);
        x.copy(mod(d));
        XES = 2;
    }

    /* this+=b */
    public void add(FP b) {
        x.add(b.x);
        XES += b.XES;
        if (XES > CONFIG_FIELD.FEXCESS) reduce();
    }

// https://graphics.stanford.edu/~seander/bithacks.html
// constant time log to base 2 (or number of bits in)

    private static int logb2(int v) {
        int r;
        v |= v >>> 1;
        v |= v >>> 2;
        v |= v >>> 4;
        v |= v >>> 8;
        v |= v >>> 16;

        v = v - ((v >>> 1) & 0x55555555);
        v = (v & 0x33333333) + ((v >>> 2) & 0x33333333);
        r = ((v + (v >>> 4) & 0xF0F0F0F) * 0x1010101) >>> 24;
        return r;
    }

    /* this = -this mod Modulus */
    public void neg() {
        int sb;
        BIG m = new BIG(ROM.Modulus);

        sb = logb2(XES - 1);
        m.fshl(sb);
        x.rsub(m);

        XES = (1 << sb) + 1;
        if (XES > CONFIG_FIELD.FEXCESS) reduce();
    }

    /* this-=b */
    public void sub(FP b) {
        FP n = new FP(b);
        n.neg();
        this.add(n);
    }

    public void rsub(FP b) {
        FP n = new FP(this);
        n.neg();
        this.copy(b);
        this.add(n);
    }

    /* this/=2 mod Modulus */
    public void div2() {
        BIG m=new BIG(ROM.Modulus);
        int pr=x.parity();
        BIG w = new BIG(x);
        x.fshr(1);
        w.add(m); w.norm();
        w.fshr(1);
        x.cmove(w,pr); 

    }

    /* return jacobi symbol (this/Modulus) */
    public int jacobi() {
        BIG w = redc();
        return w.jacobi(new BIG(ROM.Modulus));
    }

    /* return TRUE if this==a */
    public boolean equals(FP a) {
        FP f = new FP(this);
        FP s = new FP(a);
        f.reduce();
        s.reduce();
        if (BIG.comp(f.x, s.x) == 0) return true;
        return false;
    }

    private FP pow(BIG e) {
        byte[] w = new byte[1 + (BIG.NLEN * CONFIG_BIG.BASEBITS + 3) / 4];
        FP [] tb = new FP[16];
        norm();
        BIG t = new BIG(e);
        t.norm();
        int nb = 1 + (t.nbits() + 3) / 4;

        for (int i = 0; i < nb; i++) {
            int lsbs = t.lastbits(4);
            t.dec(lsbs);
            t.norm();
            w[i] = (byte)lsbs;
            t.fshr(4);
        }
        tb[0] = new FP(1);
        tb[1] = new FP(this);
        for (int i = 2; i < 16; i++) {
            tb[i] = new FP(tb[i - 1]);
            tb[i].mul(this);
        }
        FP r = new FP(tb[w[nb - 1]]);
        for (int i = nb - 2; i >= 0; i--) {
            r.sqr();
            r.sqr();
            r.sqr();
            r.sqr();
            r.mul(tb[w[i]]);
        }
        r.reduce();
        return r;
    }


// See eprint paper https://eprint.iacr.org/2018/1038
    private FP fpow() {
        int i, j, k, bw, w, c, nw, lo, m, n, nd, e=CONFIG_FIELD.PM1D2;
        FP [] xp = new FP[11];
        int[]  ac = {1, 2, 3, 6, 12, 15, 30, 60, 120, 240, 255};
// phase 1

        xp[0] = new FP(this);	// 1
        xp[1] = new FP(this); xp[1].sqr(); // 2
        xp[2] = new FP(xp[1]); xp[2].mul(this); //3
        xp[3] = new FP(xp[2]); xp[3].sqr(); // 6
        xp[4] = new FP(xp[3]); xp[4].sqr(); // 12
        xp[5] = new FP(xp[4]); xp[5].mul(xp[2]); // 15
        xp[6] = new FP(xp[5]); xp[6].sqr(); // 30
        xp[7] = new FP(xp[6]); xp[7].sqr(); // 60
        xp[8] = new FP(xp[7]); xp[8].sqr(); // 120
        xp[9] = new FP(xp[8]); xp[9].sqr(); // 240
        xp[10] = new FP(xp[9]); xp[10].mul(xp[5]); // 255

        n = CONFIG_FIELD.MODBITS;
        if (CONFIG_FIELD.MODTYPE == CONFIG_FIELD.GENERALISED_MERSENNE) // Goldilocks ONLY
            n /= 2;

        n-=(e+1);
        c=((int)ROM.MConst+(1<<e)+1)/(1<<(e+1));

// need c to be odd
        nd=0;
        while (c%2==0)
        {
            c/=2;
            n-=1;
            nd++;
        }

        bw = 0; w = 1; while (w < c) {w *= 2; bw += 1;}
        k = w - c;

        FP key = new FP(); i = 10;
        if (k != 0) {
            while (ac[i] > k) i--;
            key.copy(xp[i]);
            k -= ac[i];
        }
        while (k != 0) {
            i--;
            if (ac[i] > k) continue;
            key.mul(xp[i]);
            k -= ac[i];
        }

// phase 2
        xp[1].copy(xp[2]);
        xp[2].copy(xp[5]);
        xp[3].copy(xp[10]);

        j = 3; m = 8;
        nw = n - bw;
        FP t = new FP();

        while (2 * m < nw) {
            t.copy(xp[j++]);
            for (i = 0; i < m; i++)
                t.sqr();
            xp[j].copy(xp[j - 1]);
            xp[j].mul(t);
            m *= 2;
        }

        lo = nw - m;
        FP r = new FP(xp[j]);

        while (lo != 0) {
            m /= 2; j--;
            if (lo < m) continue;
            lo -= m;
            t.copy(r);
            for (i = 0; i < m; i++)
                t.sqr();
            r.copy(t);
            r.mul(xp[j]);
        }

// phase 3
        if (bw != 0) {
            for (i = 0; i < bw; i++ )
                r.sqr();
            r.mul(key);
        }

        if (CONFIG_FIELD.MODTYPE == CONFIG_FIELD.GENERALISED_MERSENNE) { // Goldilocks ONLY
            key.copy(r);
            r.sqr();
            r.mul(this);
            for (i = 0; i < n + 1; i++)
                r.sqr();
            r.mul(key);
        }

        for (i=0;i<nd;i++)
            r.sqr();

        return r;
    }

// calculates r=x^(p-1-2^e)/2^{e+1) where 2^e|p-1
    private void progen() {
        if (CONFIG_FIELD.MODTYPE == CONFIG_FIELD.PSEUDO_MERSENNE || CONFIG_FIELD.MODTYPE == CONFIG_FIELD.GENERALISED_MERSENNE) 
        {
            copy(fpow());
            return;
        }
        int e=CONFIG_FIELD.PM1D2;
        BIG m = new BIG(ROM.Modulus);
        m.dec(1);
        m.shr(e);
        m.dec(1);
        m.fshr(1);
        copy(pow(m));
    }

    /* this=1/this mod Modulus */
    public void inverse(FP h) {
        int e=CONFIG_FIELD.PM1D2;
        norm();
        FP s=new FP(this);

        for (int i=0;i<e-1;i++)
        {
            s.sqr();
            s.mul(this);
        }
        if (h==null)
            progen();
        else
            copy(h);
        for (int i=0;i<=e;i++)
            sqr();
        mul(s);
        reduce();
    } 

    /* test for quadratic residue */
    public int qr(FP h) {
        FP r=new FP(this);
        int e=CONFIG_FIELD.PM1D2;
        r.progen();
        if (h!=null)
            h.copy(r);

        r.sqr();
        r.mul(this);
        for (int i=0;i<e-1;i++)
            r.sqr();


//        for (int i=0;i<e;i++ )
//            r.sqr();
//        FP s=new FP(this);
//        for (int i=0;i<e-1;i++ )
//            s.sqr();
//        r.mul(s);
        return r.isunity()?1:0;
    }

    /* return sqrt(this) mod Modulus */
    public FP sqrt(FP h) {
        int u,e=CONFIG_FIELD.PM1D2;
        FP g=new FP(this);
        if (h==null)
            g.progen();
        else
            g.copy(h);

        BIG m = new BIG(ROM.ROI);
        FP v=new FP(m);

        FP t=new FP(g);
        t.sqr();
        t.mul(this);

        FP r=new FP(this);
        r.mul(g);
        FP b=new FP(t);
        for (int k=e;k>1;k--)
        {
            for (int j=1;j<k-1;j++)
                b.sqr();
            u=b.isunity()? 0:1;
            g.copy(r); g.mul(v);
            r.cmove(g,u);
            v.sqr();
            g.copy(t); g.mul(v);
            t.cmove(g,u);
            b.copy(t);
        }

        int sgn=r.sign();
        FP nr=new FP(r);
        nr.neg(); nr.norm();
        r.cmove(nr,sgn);

        return r;
    }
// Calculate both inverse and square root of this, return QR
    public int invsqrt(FP i, FP s)
    {
        FP h=new FP(0);
        int qr=qr(h);
        s.copy(sqrt(h));
        i.copy(this);
        i.inverse(h);
        return qr;
    }

// Two for Price of One - See Hamburg https://eprint.iacr.org/2012/309.pdf
// Calculate inverse of i and square root of s, return QR
    public static int tpo(FP i, FP s)
    {
        FP w = new FP(s);
        FP t = new FP(i);
        w.mul(i);
        t.mul(w);
        int qr=t.invsqrt(i,s);
        i.mul(w);
        s.mul(i);
        return qr;
    }
}
