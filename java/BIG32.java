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

/* CORE BIG number class */

package org.miracl.core.XXX;
import org.miracl.core.RAND;

public class BIG {

    public static final int CHUNK = 32; /* Set word size */

    public static final int NLEN = (1 + ((8 * CONFIG_BIG.MODBYTES - 1) / CONFIG_BIG.BASEBITS));
    public static final int DNLEN = 2 * NLEN;
    public static final int BMASK = (((int)1 << CONFIG_BIG.BASEBITS) - 1);

    public static final int HBITS = CONFIG_BIG.BASEBITS / 2;
    public static final int HMASK = (((int)1 << HBITS) - 1);
    public static final int NEXCESS = ((int)1 << (CHUNK - CONFIG_BIG.BASEBITS - 1));
    public static final int BIGBITS = (CONFIG_BIG.MODBYTES * 8);

    protected int[] w = new int[NLEN];

//	public static int BIGMULS=0;

    /* Constructors */

    public BIG() {
        for (int i = 0; i < NLEN; i++)
            w[i] = 0;
    }

    public BIG(int x) {
        w[0] = x;
        for (int i = 1; i < NLEN; i++)
            w[i] = 0;
    }

    public BIG(BIG x) {
        for (int i = 0; i < NLEN; i++)
            w[i] = x.w[i];
    }

    public BIG(DBIG x) {
        for (int i = 0; i < NLEN; i++)
            w[i] = x.w[i];
    }

    public BIG(int[] x) {
        for (int i = 0; i < NLEN; i++)
            w[i] = x[i];
    }

    public int get(int i) {
        return w[i];
    }

    public void set(int i, int x) {
        w[i] = x;
    }

    /* Conditional swap of two bigs depending on d using XOR - no branches */
    public int cswap(BIG b, int d) {
        int i;
        int e,r,ra,s,t, c = -d;
        s=0; 
        r=w[0]^b.w[1];  // sort of random
        ra=r+r; ra>>=1;
        for (i = 0; i < NLEN; i++) {
            t = c & (w[i] ^ b.w[i]);
            t^=r; 
            e=w[i]^t; s^=e;  // to force calculation of e
            w[i] = e^ra;
            e=b.w[i]^t; s^=e;
            b.w[i] = e^ra;
        }
        return s;
    }

    public int cmove(BIG g, int d) {
        int i;
        int e,r,ra,s,t,b = -d;
        s=0; 
        r=w[0]^g.w[1];  // sort of random
        ra=r+r; ra>>=1;
        for (i = 0; i < NLEN; i++) {
            t=(w[i] ^ g.w[i])&b;
            t^=r;
            e=w[i]^t; s^=e;
            w[i] = e^ra;
        }
        return s;
    }

    public static int cast_to_chunk(int x) {
        return (int)x;
    }

    /* normalise BIG - force all digits < 2^BASEBITS */
    public long norm() {
        int d, carry = 0;
        for (int i = 0; i < NLEN - 1; i++) {
            d = w[i] + carry;
            w[i] = d & BMASK;
            carry = d >> CONFIG_BIG.BASEBITS;
        }
        w[NLEN - 1] = (w[NLEN - 1] + carry);
        return (long)(w[NLEN - 1] >> ((8 * CONFIG_BIG.MODBYTES) % CONFIG_BIG.BASEBITS));
    }

    /* return number of bits */
    public int nbits() {
        BIG t = new BIG(this);
        int bts, k = NLEN - 1;
        int c;
        t.norm();
        while (k >= 0 && t.w[k] == 0) k--;
        if (k < 0) return 0;
        bts = CONFIG_BIG.BASEBITS * k;
        c = t.w[k];
        while (c != 0) {c /= 2; bts++;}
        return bts;
    }

    public String toRawString() {
        BIG b = new BIG(this);
        String s = "(";
        for (int i = 0; i < NLEN - 1; i++) {
            s += Integer.toHexString(b.w[i]); s += ",";
        }
        s += Integer.toHexString(b.w[NLEN - 1]); s += ")";
        return s;
    }

    /* Convert to Hex String */
    public String toString() {
        BIG b;
        String s = "";
        int len = nbits();

        if (len % 4 == 0) len /= 4;
        else {len /= 4; len++;}
        if (len < CONFIG_BIG.MODBYTES * 2) len = CONFIG_BIG.MODBYTES * 2;

        for (int i = len - 1; i >= 0; i--) {
            b = new BIG(this);
            b.shr(i * 4);
            s += Integer.toHexString(b.w[0] & 15);
        }
        return s;
    }

    public static int[] muladd(int x, int y, int c, int r) {
        int[] tb = new int[2];
        long prod = (long)x * y + c + r;
        tb[1] = (int)prod & BMASK;
        tb[0] = (int)(prod >> CONFIG_BIG.BASEBITS);
        return tb;
    }

    /* this*=x, where x is >NEXCESS */
    public int pmul(int c) {
        int ak, carry = 0;
        int[] cr = new int[2];

        for (int i = 0; i < NLEN; i++) {
            ak = w[i];
            w[i] = 0;
            cr = muladd(ak, c, carry, w[i]);
            carry = cr[0];
            w[i] = cr[1];
        }
        return carry;
    }

    /* return this*c and catch overflow in DBIG */
    public DBIG pxmul(int c) {
        DBIG m = new DBIG(0);
        int[] cr = new int[2];
        int carry = 0;
        for (int j = 0; j < NLEN; j++) {
            cr = muladd(w[j], c, carry, m.w[j]);
            carry = cr[0];
            m.w[j] = cr[1];
        }
        m.w[NLEN] = carry;
        return m;
    }

    /* divide by 3 */
    public int div3() {
        int ak, base, carry = 0;
        norm();
        base = ((int)1 << CONFIG_BIG.BASEBITS);
        for (int i = NLEN - 1; i >= 0; i--) {
            ak = (carry * base + w[i]);
            w[i] = ak / 3;
            carry = ak % 3;
        }
        return (int)carry;
    }

    /* return a*b where result fits in a BIG */
    public static BIG smul(BIG a, BIG b) {
        int carry;
        BIG c = new BIG(0);
        int[] cr = new int[2];
        for (int i = 0; i < NLEN; i++) {
            carry = 0;
            for (int j = 0; j < NLEN; j++) {
                if (i + j < NLEN) {
                    cr = muladd(a.w[i], b.w[j], carry, c.w[i + j]);
                    carry = cr[0];
                    c.w[i + j] = cr[1];
                }
            }
        }
        return c;
    }

    /* return a*b as DBIG */
    /* Inputs must be normed */
    public static DBIG mul(BIG a, BIG b) {
        long t, co;
        DBIG c = new DBIG(0);

/*
        boolean chekit=false;
        for (int i=0;i<NLEN-1;i++ )
        {
            int carry=(a.w[i]>>CONFIG_BIG.BASEBITS);
            if (carry!=0)
            {
                chekit=true;
                break;
            }
            carry=(b.w[i]>>CONFIG_BIG.BASEBITS);
            if (carry!=0)
            {
                chekit=true;
                break;
            }
        }
        if (chekit) System.out.println("Overflow in mul ********************** ");
*/
//BIGMULS++;

        long[] d = new long[NLEN];
        long s;
        int i, k;

        for (i = 0; i < NLEN; i++)
            d[i] = (long)a.w[i] * b.w[i];

        s = d[0];
        t = s; c.w[0] = (int)t & BMASK; co = t >> CONFIG_BIG.BASEBITS;

        for (k = 1; k < NLEN; k++) {
            s += d[k]; t = co + s; for (i = k; i >= 1 + k / 2; i--) t += (long)(a.w[i] - a.w[k - i]) * (b.w[k - i] - b.w[i]); c.w[k] = (int)t & BMASK; co = t >> CONFIG_BIG.BASEBITS;
        }
        for (k = NLEN; k < 2 * NLEN - 1; k++) {
            s -= d[k - NLEN]; t = co + s; for (i = NLEN - 1; i >= 1 + k / 2; i--) t += (long)(a.w[i] - a.w[k - i]) * (b.w[k - i] - b.w[i]); c.w[k] = (int)t & BMASK; co = t >> CONFIG_BIG.BASEBITS;
        }
        c.w[2 * NLEN - 1] = (int)co;

        return c;
    }

    /* return a^2 as DBIG */
    /* Input must be normed */
    public static DBIG sqr(BIG a) {
        int i, j, last;
        long t, co;
        DBIG c = new DBIG(0);
/*
        boolean chekit=false;
        for (i=0;i<NLEN-1;i++ )
        {
            int carry=(a.w[i]>>CONFIG_BIG.BASEBITS);
            if (carry!=0)
            {
                chekit=true;
                break;
            }
        }
        if (chekit) System.out.println("Overflow in sqr ********************** ");
*/
        t = (long)a.w[0] * a.w[0];
        c.w[0] = (int)t & BMASK; co = t >> CONFIG_BIG.BASEBITS;

        for (j = 1; j < NLEN - 1; ) {
            t = (long)a.w[j] * a.w[0]; for (i = 1; i < (j + 1) / 2; i++) {t += (long)a.w[j - i] * a.w[i];} t += t;  t += co;
            c.w[j] = (int)t & BMASK; co = t >> CONFIG_BIG.BASEBITS;
            j++;
            t = (long)a.w[j] * a.w[0]; for (i = 1; i < (j + 1) / 2; i++) {t += (long)a.w[j - i] * a.w[i];} t += t; t += co; t += (long)a.w[j / 2] * a.w[j / 2];
            c.w[j] = (int)t & BMASK; co = t >> CONFIG_BIG.BASEBITS;
            j++;
        }

        for (j = NLEN - 1 + NLEN % 2; j < DNLEN - 3; ) {
            t = (long)a.w[NLEN - 1] * a.w[j - NLEN + 1]; for (i = j - NLEN + 2; i < (j + 1) / 2; i++) {t += (long)a.w[j - i] * a.w[i];} t += t; t += co;
            c.w[j] = (int)t & BMASK; co = t >> CONFIG_BIG.BASEBITS;
            j++;
            t = (long)a.w[NLEN - 1] * a.w[j - NLEN + 1]; for (i = j - NLEN + 2; i < (j + 1) / 2; i++) {t += (long)a.w[j - i] * a.w[i];} t += t; t += co; t += (long)a.w[j / 2] * a.w[j / 2];
            c.w[j] = (int)t & BMASK; co = t >> CONFIG_BIG.BASEBITS;
            j++;
        }

        t = (long)a.w[NLEN - 2] * a.w[NLEN - 1];
        t += t; t += co;
        c.w[DNLEN - 3] = (int)t & BMASK; co = t >> CONFIG_BIG.BASEBITS;

        t = (long)a.w[NLEN - 1] * a.w[NLEN - 1] + co;
        c.w[DNLEN - 2] = (int)t & BMASK; co = t >> CONFIG_BIG.BASEBITS;
        c.w[DNLEN - 1] = (int)co;

        return c;
    }

    static BIG monty(BIG m, int MC, DBIG d) {
        BIG b;
        long t, c, s;
        int i, k;
        long[] dd = new long[NLEN];
        int[] v = new int[NLEN];
        b = new BIG(0);

        t = d.w[0]; v[0] = ((int)t * MC) & BMASK; t += (long)v[0] * m.w[0]; c = (t >> CONFIG_BIG.BASEBITS) + d.w[1]; s = 0;

        for (k = 1; k < NLEN; k++) {
            t = c + s + (long)v[0] * m.w[k];
            for (i = k - 1; i > k / 2; i--) t += (long)(v[k - i] - v[i]) * (m.w[i] - m.w[k - i]);
            v[k] = ((int)t * MC) & BMASK; t += (long)v[k] * m.w[0]; c = (t >> CONFIG_BIG.BASEBITS) + d.w[k + 1];
            dd[k] = (long)v[k] * m.w[k]; s += dd[k];
        }
        for (k = NLEN; k < 2 * NLEN - 1; k++) {
            t = c + s;
            for (i = NLEN - 1; i >= 1 + k / 2; i--) t += (long)(v[k - i] - v[i]) * (m.w[i] - m.w[k - i]);
            b.w[k - NLEN] = (int)t & BMASK; c = (t >> CONFIG_BIG.BASEBITS) + d.w[k + 1]; s -= dd[k - NLEN + 1];
        }
        b.w[NLEN - 1] = (int)c & BMASK;
        return b;
    }

    public static int ssn(BIG r, BIG a, BIG m) {
        int n = NLEN - 1;
        m.w[0] = (m.w[0] >> 1) | ((m.w[1] << (CONFIG_BIG.BASEBITS - 1)) & BMASK);
        r.w[0] = a.w[0] - m.w[0];
        int carry = r.w[0] >> CONFIG_BIG.BASEBITS;
        r.w[0] &= BMASK;
        for (int i = 1; i < n; i++) {
            m.w[i] = (m.w[i] >> 1) | ((m.w[i + 1] << (CONFIG_BIG.BASEBITS - 1)) & BMASK);
            r.w[i] = a.w[i] - m.w[i] + carry;
            carry = r.w[i] >> CONFIG_BIG.BASEBITS;
            r.w[i] &= BMASK;
        }
        m.w[n] >>= 1;
        r.w[n] = a.w[n] - m.w[n] + carry;
        return (int)((r.w[n] >> (CHUNK - 1)) & 1);
    }

    /* test for zero */
    public boolean iszilch() {
        int d=0;
        for (int i = 0; i < NLEN; i++)
            d|=w[i];
        return ((1 & ((d-1)>>CONFIG_BIG.BASEBITS)) != 0);
    }

    /* Test for equal to one */
    public boolean isunity() {
        int d=0;
        for (int i = 1; i < NLEN; i++)
            d|=w[i];
        return ((1 & ((d-1)>>CONFIG_BIG.BASEBITS) & (((w[0]^1)-1)>>CONFIG_BIG.BASEBITS)) != 0);
    }

    /* Compare a and b, return 0 if a==b, -1 if a<b, +1 if a>b. Inputs must be normalised */
    public static int comp(BIG a, BIG b) {
        int gt=0; int eq=1;
        for (int i = NLEN-1; i>=0; i--)
        {
            gt |= ((b.w[i]-a.w[i]) >> CONFIG_BIG.BASEBITS) & eq;
            eq &= ((b.w[i]^a.w[i])-1) >> CONFIG_BIG.BASEBITS;
        }
        return (int)(gt+gt+eq-1);
    }

    /****************************************************************************/
    public void xortop(long x) {
        w[NLEN - 1] ^= x;
    }

    /* set x = x mod 2^m */
    public void mod2m(int m) {
        int i, wd, bt;
        wd = m / CONFIG_BIG.BASEBITS;
        bt = m % CONFIG_BIG.BASEBITS;
        w[wd] &= ((cast_to_chunk(1) << bt) - 1);
        for (i = wd + 1; i < NLEN; i++) w[i] = 0;
    }

    /* return n-th bit */
    public int bit(int n) {
        return (int)((w[n / CONFIG_BIG.BASEBITS] & (cast_to_chunk(1) << (n % CONFIG_BIG.BASEBITS))) >> (n % CONFIG_BIG.BASEBITS));
//        if ((w[n / CONFIG_BIG.BASEBITS] & (cast_to_chunk(1) << (n % CONFIG_BIG.BASEBITS))) > 0) return 1;
//        else return 0;
    }

    /* Shift right by less than a word */
    public int fshr(int k) {
        int r = (int)(w[0] & ((cast_to_chunk(1) << k) - 1)); /* shifted out part */
        for (int i = 0; i < NLEN - 1; i++)
            w[i] = (w[i] >> k) | ((w[i + 1] << (CONFIG_BIG.BASEBITS - k)) & BMASK);
        w[NLEN - 1] = w[NLEN - 1] >> k;
        return r;
    }

    /* Shift right by less than a word */
    public int fshl(int k) {
        w[NLEN - 1] = ((w[NLEN - 1] << k)) | (w[NLEN - 2] >> (CONFIG_BIG.BASEBITS - k));
        for (int i = NLEN - 2; i > 0; i--)
            w[i] = ((w[i] << k) & BMASK) | (w[i - 1] >> (CONFIG_BIG.BASEBITS - k));
        w[0] = (w[0] << k) & BMASK;
        return (int)(w[NLEN - 1] >> ((8 * CONFIG_BIG.MODBYTES) % CONFIG_BIG.BASEBITS)); /* return excess - only used in FF.java */
    }



    /* set to zero */
    public void zero() {
        for (int i = 0; i < NLEN; i++)
            w[i] = 0;
    }

    /* set to one */
    public void one() {
        w[0] = 1;
        for (int i = 1; i < NLEN; i++)
            w[i] = 0;
    }

    /* Copy from another BIG */
    public void copy(BIG x) {
        for (int i = 0; i < NLEN; i++)
            w[i] = x.w[i];
    }

    public void copy(DBIG x) {
        for (int i = 0; i < NLEN; i++)
            w[i] = x.w[i];
    }

    /* general shift right */
    public void shr(int k) {
        int n = k % CONFIG_BIG.BASEBITS;
        int m = k / CONFIG_BIG.BASEBITS;
        for (int i = 0; i < NLEN - m - 1; i++)
            w[i] = (w[m + i] >> n) | ((w[m + i + 1] << (CONFIG_BIG.BASEBITS - n)) & BMASK);
        if (NLEN > m) w[NLEN - m - 1] = w[NLEN - 1] >> n;
        for (int i = NLEN - m; i < NLEN; i++) w[i] = 0;
    }

    /* general shift left */
    public void shl(int k) {
        int n = k % CONFIG_BIG.BASEBITS;
        int m = k / CONFIG_BIG.BASEBITS;

        w[NLEN - 1] = ((w[NLEN - 1 - m] << n));
        if (NLEN >= m + 2) w[NLEN - 1] |= (w[NLEN - m - 2] >> (CONFIG_BIG.BASEBITS - n));

        for (int i = NLEN - 2; i > m; i--)
            w[i] = ((w[i - m] << n) & BMASK) | (w[i - m - 1] >> (CONFIG_BIG.BASEBITS - n));
        w[m] = (w[0] << n) & BMASK;
        for (int i = 0; i < m; i++) w[i] = 0;
    }

    /* return this+x */
    public BIG plus(BIG x) {
        BIG s = new BIG(0);
        for (int i = 0; i < NLEN; i++)
            s.w[i] = w[i] + x.w[i];
        s.norm();
        return s;
    }

    /* this+=x */
    public void add(BIG x) {
        for (int i = 0; i < NLEN; i++)
            w[i] += x.w[i];
    }

    /* this|=x */
    public void or(BIG x) {
        for (int i = 0; i < NLEN; i++)
            w[i] |= x.w[i];
    }

    /* this+=x, where x is int */
    public void inc(int x) {
        norm();
        w[0] += x;
    }

    /* this+=x, where x is long */
    public void incl(long x) {
        norm();
        w[0] += x;
    }

    /* return this-x */
    public BIG minus(BIG x) {
        BIG d = new BIG(0);
        for (int i = 0; i < NLEN; i++)
            d.w[i] = w[i] - x.w[i];
        return d;
    }

    /* this-=x */
    public void sub(BIG x) {
        for (int i = 0; i < NLEN; i++)
            w[i] -= x.w[i];
    }

    /* reverse subtract this=x-this */
    public void rsub(BIG x) {
        for (int i = 0; i < NLEN; i++)
            w[i] = x.w[i] - w[i];
    }

    /* this-=x where x is int */
    public void dec(int x) {
        norm();
        w[0] -= x;
    }

    /* this*=x, where x is small int<NEXCESS */
    public void imul(int c) {
        for (int i = 0; i < NLEN; i++) w[i] *= c;
    }

    /* convert this BIG to byte array */
    public void tobytearray(byte[] b, int n) {
        BIG c = new BIG(this);
        c.norm();

        for (int i = CONFIG_BIG.MODBYTES - 1; i >= 0; i--) {
            b[i + n] = (byte)c.w[0];
            c.fshr(8);
        }
    }

    /* convert from byte array to BIG */
    public static BIG frombytearray(byte[] b, int n) {
        BIG m = new BIG(0);

        for (int i = 0; i < CONFIG_BIG.MODBYTES; i++) {
            m.fshl(8); m.w[0] += (int)b[i + n] & 0xff;
            //m.inc((int)b[i]&0xff);
        }
        return m;
    }

    public void toBytes(byte[] b) {
        tobytearray(b, 0);
    }

    public static BIG fromBytes(byte[] b) {
        return frombytearray(b, 0);
    }

    /* Arazi and Qi inversion mod 256 */
    public static int invmod256(int a) {
        int U, t1, t2, b, c;
        t1 = 0;
        c = (a >> 1) & 1;
        t1 += c;
        t1 &= 1;
        t1 = 2 - t1;
        t1 <<= 1;
        U = t1 + 1;

// i=2
        b = a & 3;
        t1 = U * b; t1 >>= 2;
        c = (a >> 2) & 3;
        t2 = (U * c) & 3;
        t1 += t2;
        t1 *= U; t1 &= 3;
        t1 = 4 - t1;
        t1 <<= 2;
        U += t1;

// i=4
        b = a & 15;
        t1 = U * b; t1 >>= 4;
        c = (a >> 4) & 15;
        t2 = (U * c) & 15;
        t1 += t2;
        t1 *= U; t1 &= 15;
        t1 = 16 - t1;
        t1 <<= 4;
        U += t1;

        return U;
    }

    /* a=1/a mod 2^256. This is very fast! */
    public void invmod2m() {
        int i;
        BIG U = new BIG(0);
        BIG b = new BIG(0);
        BIG c = new BIG(0);

        U.inc(invmod256(lastbits(8)));

        for (i = 8; i < BIGBITS; i <<= 1) {
            U.norm();
            b.copy(this); b.mod2m(i);
            BIG t1 = BIG.smul(U, b);
            t1.shr(i);

            c.copy(this); c.shr(i); c.mod2m(i);

            BIG t2 = BIG.smul(U, c); t2.mod2m(i);

            t1.add(t2);
            t1.norm();
            b = BIG.smul(t1, U); t1.copy(b);
            t1.mod2m(i);

            t2.one(); t2.shl(i); t1.rsub(t2); t1.norm();

            t1.shl(i);
            U.add(t1);
        }
        U.mod2m(BIGBITS);
        copy(U);
        norm();
    }

    public void ctmod(BIG m,int bd) {
        int k=bd;
        BIG r=new BIG(0);
        BIG c=new BIG(m);
        norm();

        c.shl(k);

        while (true)
        {
            r.copy(this);
            r.sub(c);
            r.norm();
            cmove(r, (int)(1 - ((r.w[NLEN - 1] >> (CHUNK - 1)) & 1)));
            if (k==0) break;
            c.fshr(1);
            k--;
        }
    }

    /* reduce this mod m */
    public void mod(BIG m) {
        int k=nbits()-m.nbits();
        if (k<0) k=0;
        ctmod(m,k);
        
/*
        int k = 0;
        BIG r = new BIG(0);
        BIG m = new BIG(m1);

        norm();
        if (comp(this, m) < 0) return;
        do {
            m.fshl(1);
            k++;
        } while (comp(this, m) >= 0);

        while (k > 0) {
            m.fshr(1);

            r.copy(this);
            r.sub(m);
            r.norm();
            cmove(r, (int)(1 - ((r.w[NLEN - 1] >> (CHUNK - 1)) & 1)));
            k--;
        } */
    }

    public void ctdiv(BIG m,int bd) {
        int k=bd;
        BIG e = new BIG(1);
        BIG r = new BIG(0);
        BIG a = new BIG(this);
        BIG c = new BIG(m);
        norm();
        zero();

        c.shl(k);
        e.shl(k);

        while (true)
        {
            r.copy(a);
            r.sub(c);
            r.norm();
            int d = (int)(1 - ((r.w[NLEN - 1] >> (CHUNK - 1)) & 1));
            a.cmove(r, d);
            r.copy(this);
            r.add(e);
            r.norm();
            cmove(r, d);
            if (k==0) break;
            c.fshr(1);
            e.fshr(1);
            k--;
        }
    }
    /* divide this by m */
    public void div(BIG m) {
        int k=nbits()-m.nbits();
        if (k<0) k=0;
        ctdiv(m,k);

/*
        int d, k = 0;
        norm();
        BIG e = new BIG(1);
        BIG m = new BIG(m1);
        BIG b = new BIG(this);
        BIG r = new BIG(0);
        zero();

        while (comp(b, m) >= 0) {
            e.fshl(1);
            m.fshl(1);
            k++;
        }

        while (k > 0) {
            m.fshr(1);
            e.fshr(1);

            r.copy(b);
            r.sub(m);
            r.norm();
            d = (int)(1 - ((r.w[NLEN - 1] >> (CHUNK - 1)) & 1));
            b.cmove(r, d);
            r.copy(this);
            r.add(e);
            r.norm();
            cmove(r, d);
            k--;
        } */
    }

    /* return parity */
    public int parity() {
        return (int)(w[0] % 2);
    }

    /* return n last bits */
    public int lastbits(int n) {
        int msk = (1 << n) - 1;
        norm();
        return ((int)w[0]) & msk;
    }

    /* get 8*CONFIG_BIG.MODBYTES size random number */
    public static BIG random(RAND rng) {
        BIG m = new BIG(0);
        int i, b, j = 0, r = 0;

        /* generate random BIG */
        for (i = 0; i < 8 * CONFIG_BIG.MODBYTES; i++) {
            if (j == 0) r = rng.getByte();
            else r >>= 1;

            b = r & 1;
            m.shl(1); m.w[0] += b; // m.inc(b);
            j++; j &= 7;
        }
        return m;
    }

    /* Create random BIG in portable way, one bit at a time */
    public static BIG randomnum(BIG q, RAND rng) {
        DBIG d = new DBIG(0);
        int i, b, j = 0, r = 0;
        for (i = 0; i < 2 * q.nbits(); i++) {
            if (j == 0) r = rng.getByte();
            else r >>= 1;

            b = r & 1;
            d.shl(1); d.w[0] += b; // m.inc(b);
            j++; j &= 7;
        }
        BIG m = d.mod(q);
        return m;
    }

    /* create randum BIG less than r and less than trunc bits */
    public static BIG randtrunc(BIG q, int trunc, RAND rng) {
        BIG m = BIG.randomnum(q, rng);
        if (q.nbits() > trunc)
            m.mod2m(trunc);
        return m;
    }

    /* return a*b mod m */
    public static BIG modmul(BIG a1, BIG b1, BIG m) {
        BIG a = new BIG(a1);
        BIG b = new BIG(b1);
        a.mod(m);
        b.mod(m);
        DBIG d = mul(a, b);
        return d.ctmod(m,m.nbits());
    }

    /* return a^2 mod m */
    public static BIG modsqr(BIG a1, BIG m) {
        BIG a = new BIG(a1);
        a.mod(m);
        DBIG d = sqr(a);
        return d.ctmod(m,m.nbits());
    }

    /* return -a mod m */
    public static BIG modneg(BIG a1, BIG m) {
        BIG a = new BIG(a1);
        a.mod(m);
        a.rsub(m);
        a.norm();
        return a;
    }

    /* return a+b mod m */
    public static BIG modadd(BIG a1, BIG b1, BIG m) {
        BIG a = new BIG(a1);
        BIG b = new BIG(b1);
        a.mod(m);
        b.mod(m);
        a.add(b); a.norm();
        a.ctmod(m,1);
        return a;
    }

    /* return this^e mod m */
    public BIG powmod(BIG e1, BIG m) {
        BIG e = new BIG(e1);
        int bt;
        norm();
        e.norm();
        BIG a = new BIG(1);
        BIG z = new BIG(e);
        BIG s = new BIG(this);
        while (true) {
            bt = z.parity();
            z.fshr(1);
            if (bt == 1) a = modmul(a, s, m);
            if (z.iszilch()) break;
            s = modsqr(s, m);
        }
        return a;
    }

    /* Jacobi Symbol (this/p). Returns 0, 1 or -1 */
    public int jacobi(BIG p) {
        int n8, k, m = 0;
        BIG t = new BIG(0);
        BIG x = new BIG(0);
        BIG n = new BIG(0);
        BIG zilch = new BIG(0);
        BIG one = new BIG(1);
        if (p.parity() == 0 || comp(this, zilch) == 0 || comp(p, one) <= 0) return 0;
        norm();
        x.copy(this);
        n.copy(p);
        x.mod(p);

        while (comp(n, one) > 0) {
            if (comp(x, zilch) == 0) return 0;
            n8 = n.lastbits(3);
            k = 0;
            while (x.parity() == 0) {
                k++;
                x.shr(1);
            }
            if (k % 2 == 1) m += (n8 * n8 - 1) / 8;
            m += (n8 - 1) * (x.lastbits(2) - 1) / 4;
            t.copy(n);
            t.mod(x);
            n.copy(x);
            x.copy(t);
            m %= 2;

        }
        if (m == 0) return 1;
        else return -1;
    }

    /* this=1/this mod p. Binary method */
    public void invmodp(BIG p) {
        mod(p);
        if (iszilch()) return;

        BIG u = new BIG(this);
        BIG v = new BIG(p);
        BIG x1 = new BIG(1);
        BIG x2 = new BIG(0);
        BIG t = new BIG(0);
        BIG one = new BIG(1);

        while (comp(u, one) != 0 && comp(v, one) != 0) {
            while (u.parity() == 0) {
                u.fshr(1);
                t.copy(x1);
                t.add(p);
                x1.cmove(t,x1.parity());
                x1.norm();
                x1.fshr(1);
            }
            while (v.parity() == 0) {
                v.fshr(1);
                t.copy(x2);
                t.add(p);
                x2.cmove(t,x2.parity());
                x2.norm();
                x2.fshr(1);
            }
            if (comp(u, v) >= 0) {
                u.sub(v);
                u.norm();
                t.copy(x1);
                t.add(p);
                x1.cmove(t,(comp(x1,x2)>>1)&1);
                x1.sub(x2);
                x1.norm();
            } else {
                v.sub(u);
                v.norm();
                t.copy(x2);
                t.add(p);
                x2.cmove(t,(comp(x2,x1)>>1)&1);
                x2.sub(x1);
                x2.norm();
            }
        }
        copy(x1);
        cmove(x2,comp(u,one)&1);
    }
}
