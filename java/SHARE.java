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


/* Shamir threshold secret sharing module */
/* Split any byte array into number of shares < 256 */
/* Specify number of shares required for recovery - nsr */

/* See TestMPIN.java for an example of use */

package org.miracl.core;

public class SHARE {

    private byte ID;  // unique share ID
    private byte NSR; // Number of shares required 
    private byte[] B; // Share bytes

/* Field GF(2^8) precalculated tables */
    private static final byte[] ptab = {
        (byte)1, (byte)3, (byte)5, (byte)15, (byte)17, (byte)51, (byte)85, (byte)255, (byte)26, (byte)46, (byte)114, (byte)150, (byte)161, (byte)248, (byte)19, (byte)53,
        (byte)95, (byte)225, (byte)56, (byte)72, (byte)216, (byte)115, (byte)149, (byte)164, (byte)247, (byte)2, (byte)6, (byte)10, (byte)30, (byte)34, (byte)102, (byte)170,
        (byte)229, (byte)52, (byte)92, (byte)228, (byte)55, (byte)89, (byte)235, (byte)38, (byte)106, (byte)190, (byte)217, (byte)112, (byte)144, (byte)171, (byte)230, (byte)49,
        (byte)83, (byte)245, (byte)4, (byte)12, (byte)20, (byte)60, (byte)68, (byte)204, (byte)79, (byte)209, (byte)104, (byte)184, (byte)211, (byte)110, (byte)178, (byte)205,
        (byte)76, (byte)212, (byte)103, (byte)169, (byte)224, (byte)59, (byte)77, (byte)215, (byte)98, (byte)166, (byte)241, (byte)8, (byte)24, (byte)40, (byte)120, (byte)136,
        (byte)131, (byte)158, (byte)185, (byte)208, (byte)107, (byte)189, (byte)220, (byte)127, (byte)129, (byte)152, (byte)179, (byte)206, (byte)73, (byte)219, (byte)118, (byte)154,
        (byte)181, (byte)196, (byte)87, (byte)249, (byte)16, (byte)48, (byte)80, (byte)240, (byte)11, (byte)29, (byte)39, (byte)105, (byte)187, (byte)214, (byte)97, (byte)163,
        (byte)254, (byte)25, (byte)43, (byte)125, (byte)135, (byte)146, (byte)173, (byte)236, (byte)47, (byte)113, (byte)147, (byte)174, (byte)233, (byte)32, (byte)96, (byte)160,
        (byte)251, (byte)22, (byte)58, (byte)78, (byte)210, (byte)109, (byte)183, (byte)194, (byte)93, (byte)231, (byte)50, (byte)86, (byte)250, (byte)21, (byte)63, (byte)65,
        (byte)195, (byte)94, (byte)226, (byte)61, (byte)71, (byte)201, (byte)64, (byte)192, (byte)91, (byte)237, (byte)44, (byte)116, (byte)156, (byte)191, (byte)218, (byte)117,
        (byte)159, (byte)186, (byte)213, (byte)100, (byte)172, (byte)239, (byte)42, (byte)126, (byte)130, (byte)157, (byte)188, (byte)223, (byte)122, (byte)142, (byte)137, (byte)128,
        (byte)155, (byte)182, (byte)193, (byte)88, (byte)232, (byte)35, (byte)101, (byte)175, (byte)234, (byte)37, (byte)111, (byte)177, (byte)200, (byte)67, (byte)197, (byte)84,
        (byte)252, (byte)31, (byte)33, (byte)99, (byte)165, (byte)244, (byte)7, (byte)9, (byte)27, (byte)45, (byte)119, (byte)153, (byte)176, (byte)203, (byte)70, (byte)202,
        (byte)69, (byte)207, (byte)74, (byte)222, (byte)121, (byte)139, (byte)134, (byte)145, (byte)168, (byte)227, (byte)62, (byte)66, (byte)198, (byte)81, (byte)243, (byte)14,
        (byte)18, (byte)54, (byte)90, (byte)238, (byte)41, (byte)123, (byte)141, (byte)140, (byte)143, (byte)138, (byte)133, (byte)148, (byte)167, (byte)242, (byte)13, (byte)23,
        (byte)57, (byte)75, (byte)221, (byte)124, (byte)132, (byte)151, (byte)162, (byte)253, (byte)28, (byte)36, (byte)108, (byte)180, (byte)199, (byte)82, (byte)246, (byte)1
    };

    private static final byte[] ltab = {
        (byte)0, (byte)255, (byte)25, (byte)1, (byte)50, (byte)2, (byte)26, (byte)198, (byte)75, (byte)199, (byte)27, (byte)104, (byte)51, (byte)238, (byte)223, (byte)3,
        (byte)100, (byte)4, (byte)224, (byte)14, (byte)52, (byte)141, (byte)129, (byte)239, (byte)76, (byte)113, (byte)8, (byte)200, (byte)248, (byte)105, (byte)28, (byte)193,
        (byte)125, (byte)194, (byte)29, (byte)181, (byte)249, (byte)185, (byte)39, (byte)106, (byte)77, (byte)228, (byte)166, (byte)114, (byte)154, (byte)201, (byte)9, (byte)120,
        (byte)101, (byte)47, (byte)138, (byte)5, (byte)33, (byte)15, (byte)225, (byte)36, (byte)18, (byte)240, (byte)130, (byte)69, (byte)53, (byte)147, (byte)218, (byte)142,
        (byte)150, (byte)143, (byte)219, (byte)189, (byte)54, (byte)208, (byte)206, (byte)148, (byte)19, (byte)92, (byte)210, (byte)241, (byte)64, (byte)70, (byte)131, (byte)56,
        (byte)102, (byte)221, (byte)253, (byte)48, (byte)191, (byte)6, (byte)139, (byte)98, (byte)179, (byte)37, (byte)226, (byte)152, (byte)34, (byte)136, (byte)145, (byte)16,
        (byte)126, (byte)110, (byte)72, (byte)195, (byte)163, (byte)182, (byte)30, (byte)66, (byte)58, (byte)107, (byte)40, (byte)84, (byte)250, (byte)133, (byte)61, (byte)186,
        (byte)43, (byte)121, (byte)10, (byte)21, (byte)155, (byte)159, (byte)94, (byte)202, (byte)78, (byte)212, (byte)172, (byte)229, (byte)243, (byte)115, (byte)167, (byte)87,
        (byte)175, (byte)88, (byte)168, (byte)80, (byte)244, (byte)234, (byte)214, (byte)116, (byte)79, (byte)174, (byte)233, (byte)213, (byte)231, (byte)230, (byte)173, (byte)232,
        (byte)44, (byte)215, (byte)117, (byte)122, (byte)235, (byte)22, (byte)11, (byte)245, (byte)89, (byte)203, (byte)95, (byte)176, (byte)156, (byte)169, (byte)81, (byte)160,
        (byte)127, (byte)12, (byte)246, (byte)111, (byte)23, (byte)196, (byte)73, (byte)236, (byte)216, (byte)67, (byte)31, (byte)45, (byte)164, (byte)118, (byte)123, (byte)183,
        (byte)204, (byte)187, (byte)62, (byte)90, (byte)251, (byte)96, (byte)177, (byte)134, (byte)59, (byte)82, (byte)161, (byte)108, (byte)170, (byte)85, (byte)41, (byte)157,
        (byte)151, (byte)178, (byte)135, (byte)144, (byte)97, (byte)190, (byte)220, (byte)252, (byte)188, (byte)149, (byte)207, (byte)205, (byte)55, (byte)63, (byte)91, (byte)209,
        (byte)83, (byte)57, (byte)132, (byte)60, (byte)65, (byte)162, (byte)109, (byte)71, (byte)20, (byte)42, (byte)158, (byte)93, (byte)86, (byte)242, (byte)211, (byte)171,
        (byte)68, (byte)17, (byte)146, (byte)217, (byte)35, (byte)32, (byte)46, (byte)137, (byte)180, (byte)124, (byte)184, (byte)38, (byte)119, (byte)153, (byte)227, (byte)165,
        (byte)103, (byte)74, (byte)237, (byte)222, (byte)197, (byte)49, (byte)254, (byte)24, (byte)13, (byte)99, (byte)140, (byte)128, (byte)192, (byte)247, (byte)112, (byte)7
    };

/* field addition */
    private static byte add(byte x,byte y) {
        return (byte)(x^y);
    }
/* x.y= AntiLog(Log(x) + Log(y)) */
    private static byte mul(byte x,byte y) {
        /* x.y= AntiLog(Log(x) + Log(y)) */

        int ix = ((int)x) & 0xff;
        int iy = ((int)y) & 0xff;
        int lx = ((int)ltab[ix]) & 0xff;
        int ly = ((int)ltab[iy]) & 0xff;
        if (x != 0 && y != 0) return ptab[(lx + ly) % 255];
        else return (byte)0;
    }
/* Field inversion */
    private static byte inv(byte x) {
        int ix = ((int)x) & 0xff;
        int lx = ((int)ltab[ix]) & 0xff;
        return ptab[255-lx];
    }
/* Lagrange interpolation */
    private static byte interpolate(int n, byte[] x, byte[] y) {
        int i,j;
        byte p,yp=0;
        for (i=0;i<n;i++)
        {
            p=1;
            for (j=0;j<n;j++)
                if (i!=j) p=mul(p,mul(x[j],inv(add(x[i],x[j]))));
            yp=add(yp,mul(p,y[i]));
        }
        return yp;
    }

/* Return a share of M */
/* input id - Unique share ID */
/* input nsr - Number of shares required for recovery */
/* input Message M to be shared */
/* input Random seed R */
/* return share structure */
    public SHARE(int id,int nsr,byte[] M,byte[] R) {
        if (id<1 || id>=256 || nsr<2 || nsr>=256)
        {
            ID=0;
            NSR=0;
            B=null;
            return;
        }
        ID=(byte)id;
        NSR=(byte)nsr;
        int m=M.length;
        B=new byte[m];
        RAND rng=new RAND();
        rng.clean();
        rng.seed(R.length,R);
        for (int j=0;j<m;j++)
        {
            byte x=ID;
            B[j]=M[j];
            for (int n=1;n<nsr;n++)
            {
                B[j]=add(B[j],mul((byte)rng.getByte(),x));
                x=mul(x,ID);
            }
        }        
    }
/* recover M from shares */
    public static byte[] recover(SHARE[] S) {
        int m=S[0].B.length;
        int nsr=S[0].NSR;
        if (nsr!=S.length) return null;
        for (int i=1;i<nsr;i++)
        {
            if ((int)S[i].NSR != nsr || S[i].B.length!=m)
                return null;
        }
        byte [] M=new byte[m];
        byte[] x=new byte[nsr];
        byte[] y=new byte[nsr];
        for (int j=0;j<m;j++)
        {
            for (int i=0;i<nsr;i++)
            {
                x[i]=S[i].ID;
                y[i]=S[i].B[j];
            }
            M[j]=interpolate(nsr,x,y);
        }
        return M;
    }
}
