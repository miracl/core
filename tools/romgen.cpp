
/* ROMGEN - Helper MIRACL program to generate ROM constants for curves

A bit rough-and-ready - the output might need minor syntactical corrections

(MINGW build)

g++ -O3 romgen.cpp big.cpp zzn.cpp ecn.cpp zzn2.cpp ecn2.cpp zzn4.cpp ecn4.cpp zzn8.cpp ecn8.cpp miracl.a -o romgen.exe

*/

#include <iostream>
#include "big.h"
#include "ecn.h"
#include "zzn2.h"
#include "ecn2.h"
#include "ecn4.h"
#include "ecn8.h"

#define NOT 0
#define BN 1
#define BLS 2

using namespace std;

Miracl precision(100, 0);

char open, close, term, el = 0;
int PS=28;  // start of pairing friendly curves

//
// Shanks method modified to find cube roots
//

ZZn2 shanks(ZZn2 n)
{
    int i,s;
    Big q,p=get_modulus();
    ZZn2 t,W,R,V;
    BOOL inv;

    if (pow(n,(p*p-1)/3)!=1)
    {
 //      cout << "Not a cubic residue" << endl;
       return (ZZn2)0;
    }

    W=randn2();
    while (pow(W,(p*p-1)/3)==1) W=randn2();
 
    s=0;
    q=p*p-1;
    while (q%3==0)
    {
        q/=3;
        s++;
    }
 
    if ((q+1)%3==0)
    {
        R=pow(n,(q+1)/3);
        inv=FALSE;
    }
    else
    {
        R=pow(n,(q-1)/3);
        inv=TRUE;
    }

    V=pow(W,q);

    forever
    {
        if (!inv) t=(R*R*R)/n;
        else      t=(R*R*R)*n;

        for (i=0;;i++ )
        {
            if (t==1) break;
            t=t*t*t;
        }
        if (i==0)
        {
            if (!inv) return R;
            else      return (ZZn2)1/R;
        }
        R=R*pow(V,pow((Big)3,s-i-1));
    }

}

int findZ8(Big AB)
{
    ZZn8 Z,B;
    if (get_mip()->TWIST==MR_SEXTIC_M)
        B=tx((ZZn8)AB);
    else
        B=txd((ZZn8)AB);
    int a=0;
    forever {
        if (a>0)
            a=-a;
        else
            a=(-a)+1;
    
        Z.set(a,(ZZn4)0);
  //      cout << "Z= " << Z << endl;
        ZZn8 ZZ=Z;
        ZZn8 GZ=ZZ*ZZ*ZZ+B;
        if (GZ==0) continue;
        ZZn8 W=-(3*ZZ*ZZ)/(4*GZ);
        if (W==0) continue;
        if (!qr(W)) continue;
        ZZ=-ZZ/2;
        ZZn8 GNZ=ZZ*ZZ*ZZ+B; 
        if (!qr(GZ) && !qr(GNZ)) continue;
        break;
    }
    return a;
}

int findZ4(Big AB)
{
    ZZn4 Z,B;
    if (get_mip()->TWIST==MR_SEXTIC_M)
        B=tx((ZZn4)AB);
    else
        B=txd((ZZn4)AB);
    int a=0;
    forever {
        if (a>0)
            a=-a;
        else
            a=(-a)+1;
    
        Z.set(a,(ZZn2)0);
        ZZn4 ZZ=Z;
        ZZn4 GZ=ZZ*ZZ*ZZ+B;
        if (GZ==0) continue;
        ZZn4 W=-(3*ZZ*ZZ)/(4*GZ);
        if (W==0) continue;
        if (!qr(W)) continue;
        ZZ=-ZZ/2;
        ZZn4 GNZ=ZZ*ZZ*ZZ+B; 
        if (!qr(GZ) && !qr(GNZ)) continue;
        break;
    }
    return a;
}

int findZ2(Big AB)
{
    ZZn2 Z,B;
    if (get_mip()->TWIST==MR_SEXTIC_M)
        B=txx((ZZn2)AB);
    else
        B=txd((ZZn2)AB);
    int a=0;
    forever {
        if (a>0)
            a=-a;
        else
            a=(-a)+1;
        
        Z.set(a,(ZZn)0);
        //cout << "Z= " << Z << endl;
        ZZn2 ZZ=Z;
        ZZn2 GZ=ZZ*ZZ*ZZ+B;
        if (GZ==0) continue;
        ZZn2 W=-(3*ZZ*ZZ)/(4*GZ);
        if (W==0) continue;
        if (!qr(W)) continue;
        ZZ=-ZZ/2;
        ZZn2 GNZ=ZZ*ZZ*ZZ+B; 
        if (!qr(GZ) && !qr(GNZ)) continue;
        break;
    }
    return a;
}

int findZ(Big AA,Big AB,Big p)
{
    ZZn x,D,BB;
    ZZn2 r,r3,r1,r2,CD,cu;
    int Z,i,j,lt,gt;

    modulo(p);
    do
    {
        cu=pow((ZZn2)randn2(),(p*p-1)/3);
    } while(cu==1);

    ZZn A=(ZZn)AA;
    ZZn B=(ZZn)AB;

    if (AA==0 || AB==0)
    {
        i=0;
        forever
        {
            if (i>0) i=-i;
            else i=(-i)+1;
            Z=i;
            ZZn ZZ=Z;
            ZZn GZ=ZZ*ZZ*ZZ+A*ZZ+B;
            if (GZ==0) continue;
            ZZn W=-(3*ZZ*ZZ+4*A)/(4*GZ);

            if (W==0) continue;
            if (!qr(W)) continue;
            ZZ=-ZZ/2;
            ZZn GNZ=ZZ*ZZ*ZZ+A*ZZ+B; 
            if (!qr(GZ) && !qr(GNZ)) continue;
            break;
        }
        return Z;
    }

    i=0;
    forever
    {
        if (i>0) i=-i;
        else i=(-i)+1;

        //printf("i= %d\n",i);

        Z=i;
        if (Z==-1) continue;

        if (qr((ZZn)Z)) continue;
        x=B/(A*Z);
        if (qnr(x*x*x+A*x+B)) continue;
       

        BB=B-Z;

        D=(BB*BB)/4 + (A*A*A)/27;

        CD=sqrt((ZZn2)D);  // Solution may be "complex"

        r1=(ZZn2)-BB/2+CD; r2=(ZZn2)-BB/2-CD;

        r1=shanks(r1);   // cube roots
        r2=shanks(r2);

        if (r1==0 || r2==0)
        {
            //cout << "Z= " << Z << endl;
            //cout << "No roots exist" << endl;
            return Z;
            //return 0;
        }

// search for "right" r2

        if (r1*r2!=-A/3)
            r2*=cu;

        if (r1*r2!=-A/3)
            r2*=cu;   
 
        r=r1+r2;

//        cout << "root 1= " << r << endl;
        if (r*r*r+A*r+BB!=0) cout << "Check failed" << endl;

// try next value for r1

        r1*=cu;

        if (r1*r2!=-A/3)
            r2*=cu;

        if (r1*r2!=-A/3)
            r2*=cu;

 
        r=r1+r2;

//        cout << "root 2= " << r << endl;
        if (r*r*r+A*r+BB!=0) cout << "Check failed" << endl;

        r1*=cu;

        if (r1*r2!=-A/3)
            r2*=cu;

        if (r1*r2!=-A/3)
            r2*=cu;

        r=r1+r2;

 //       cout << "root 3= " << r << endl;
        if (r*r*r+A*r+BB!=0) cout << "Check failed" << endl;
    }
}

// Decide on RFC7748 or Generic method
int findR(Big AA,Big AB,Big p)
{
    modulo(p);

    ZZn A=(ZZn)AA;
    ZZn B=(ZZn)AB;

    ZZn K=(ZZn)4/(A-B);
    if (qr(K)) return 1;
    return 0;
}

Big finde(Big p)
{
    Big e,p1=p-1;
    e=1;
    while (p1%2==0)
    {
        p1/=2;
        e*=2;
    }
    return (p-e-1)/(2*e);
}

Big output(int chunk, int w, Big t, Big m)
{
    Big last, y = t;

    cout << open;
    for (int i = 0; i < w; i++)
    {
        last = y % m;
        cout << "0x" << last;
        y /= m;
        if (i == w - 1) break;
        if (el != 0) cout << el << ",";
        else cout << ",";
    }

    if (el != 0) cout << el << close;
    else cout << close;
    return last;
}

#define NOT_SPECIAL 0
#define PSEUDO_MERSENNE 1
#define GENERALISED_MERSENNE 2
#define MONTGOMERY_FRIENDLY 3

#define WEIERSTRASS 0
#define EDWARDS 1
#define MONTGOMERY 2

// set Frobenius constant - depends on embedding degree
void set_frobenius_constant(ZZn2 &X, int ed)
{
    Big p = get_modulus();
    switch (get_mip()->pmod8)
    {
    case 5:
        X.set((Big)0, (Big)1); // = (sqrt(-2)^(p-1)/2
        break;
    case 3:                    // = (1+sqrt(-1))^(p-1)/2
        X.set((Big)1, (Big)1);
        break;
    case 7:
        X.set((Big)2, (Big)1); // = (2+sqrt(-1))^(p-1)/2
    default: break;
    }
    if (ed == 12) X = pow(X, (p - 1) / 6);
    if (ed == 24) X = pow(X, (p - 7) / 12);
    if (ed == 48) X = pow(X, (p - 19) / 24);
}

void q_power_frobenius(ECn2 &A, ZZn2 &F)
{
// Fast multiplication of A by q (for Trace-Zero group members only)
    ZZn2 x, y, z, w, r;

    A.get(x, y);

    w = F * F;
    r = F;

    if (get_mip()->TWIST == MR_SEXTIC_M) r = inverse(F); // could be precalculated
    if (get_mip()->TWIST == MR_SEXTIC_D) r = F;

    w = r * r;
    x = w * conj(x);
    y = r * w * conj(y);

    A.set(x, y);

}

//
// Faster Hashing to G2 - Fuentes-Castaneda, Knapp and Rodriguez-Henriquez
//

void cofactor(ECn2& S, ZZn2 &F, Big& x)
{
    ECn2 T, K;
    T = S;
    T *= (-x);
    T.norm();
    K = (T + T) + T;
    K.norm();
    q_power_frobenius(K, F);
    q_power_frobenius(S, F); q_power_frobenius(S, F); q_power_frobenius(S, F);
    S += T; S += K;
    q_power_frobenius(T, F); q_power_frobenius(T, F);
    S += T;
    S.norm();
}

void help()
{
    printf("Elliptic Curves\n");
    printf("1. ED25519\n");
    printf("2. C25519\n");
    printf("3. NIST256\n");
    printf("4. BRAINPOOL\n");
    printf("5. ANSSI\n");
    printf("6. HIFIVE\n");
    printf("7. GOLDILOCKS\n");
    printf("8. NIST384\n");
    printf("9. C41417\n");
    printf("10. NIST521\n");
    printf("11. NUMS256W\n");
    printf("12. NUMS256E\n");
    printf("13. NUMS384W\n");
    printf("14. NUMS384E\n");
    printf("15. NUMS512W\n");
    printf("16. NUMS512E\n");
    printf("17. SECP256K1\n");
    printf("18. SM2\n");
    printf("19. C13318\n");
    printf("20. JUBJUB\n");
    printf("21. X448\n");
    printf("22. SECP160R1\n");
    printf("23. C1174\n");
    printf("24. C1665\n");
    printf("25. MDC (Million Dollar Curve)\n");

    printf("26. TWEEDLEDUM\n");
    printf("27. TWEEDLEDEE\n");

    printf("28. BN254\n");
    printf("29. BN254CX\n");
    printf("30. BLS12383\n");
    printf("31. BLS12381\n");
    printf("32. FP256BN\n");
    printf("33. FP512BN\n");
    printf("34. BLS12461\n");
    printf("35. BN462\n");

    printf("36. BLS24479\n");
    printf("37. BLS48556\n");
    printf("38. BLS48581\n");
    printf("39. BLS48286\n");

    printf("\nromgen curve wordlength language\n");
    printf("where wordlength is 16, 32 or 64 (always 32 for javascript)\n");
    printf("language is c, cpp, java, javascript, go, rust or swift\n\n");
}

char* toupperit(char *s, int lang)
{
    int i;
    static char t[50];
    if (lang != 5) return s;
    for (i = 0;; i++)
    {
        t[i] = s[i];
        if (t[i] == 0) break;
        t[i] = toupper(t[i]);
    }

    return t;
}

int hamming(Big x)
{
    Big n = x;
    Big n3 = 3 * x;
    int hw = 0;
    int nb = bits(n3);
    for (int i = nb - 2; i >= 1; i--)
    {
        int bt = bit(n3, i) - bit(n, i);
        if (bt == 1 || bt == -1) hw++;
    }
    return hw;
}

int main(int argc, char **argv)
{
    miracl *mip = &precision;
    Big p, R, B, mc, curve_b, cru, cof, tau[9],ad,bd;
    Big m, x, y, w, t, c, n, r, a, b, gx, gy, r2modp,roi;
    Big np, PP, TT, FF, htpc, htpc2, sqrtm3, twk;
    Big pc[60];
    int xt, yt, ncs, ncs2, e;

    Big adr,adi,bdr,bdi,pcr[13],pci[13],p1;

    int i, A, curve, bb, chunk, words, mbits, bytes, ip = 0;
    int modtype, curvetype, curve_a, curve_b_i, cof_i, lang = 0;
    int atebits, hw;
    ZZn2 X;
    ECn P;
    ECn2 Q;
    ECn4 QQ;
    ECn8 Q8;
    ZZn4 XA, YA, AA, BB;
    ZZn8 X8, Y8;
    ZZn2 Aa, Ab, Ba, Bb;
    ZZn2 Xa, Ya;
    ZZn zcru,zsqrtm3;
    ZZn CA, CB;
    ZZn2 CA2, CB2;
    ZZn4 CA4, CB4;
    ZZn8 CA8, CB8;
    char pre0[60], pre1[60], pre2[60], pre3[60], pre4[60], pre5[60], pre6[60];
    char post0[60], post1[60], post2[60], post3[60], post4[60], post5[60], post6[60];
    char pre7[60], post7[60], lg[60];
    char pre8[60], post8[60], post9[60];

    char xxx[50], yyy[50], zzz[50];

    char curvename[40], fieldname[40];

    argv++; argc--;

    if (argc < 3)
    {
        help();
        return 0;
    }

    strcpy(curvename, argv[0]);

//	curve=atoi(argv[ip++]);
    for (i = 0;; i++)
    {
        if (curvename[i] == 0) break;
        curvename[i] = toupper(curvename[i]);
    }

//    cout << "curvename= " << curvename << " " << strlen(curvename) << endl;

    curve = 0; ip++;
    chunk = atoi(argv[ip++]);
    //bb = atoi(argv[ip++]);

    strcpy(lg, argv[ip]);

    if (chunk != 16 && chunk != 32 && chunk != 64) {help(); return 0;}
    //if (bb < 0 || bb >= chunk) {help(); return 0;}

    htpc=(Big)0;
    htpc2=(Big)0;
    sqrtm3=(Big)0;
    cru=(Big)0;

// Specify curve constants

    if (strcmp(curvename, "ED25519") == 0)
    {   // ED25519
        curve = 1; 
        if (chunk==16)
            bb=13;
        if (chunk==32)
            bb=29;
        if (chunk==64)
            bb=56;
        if (strcmp(lg, "javascript") == 0)
            bb=24;
        
        printf("Curve is ED25519\n");
        strcpy(fieldname, "F25519");
        mbits = 255;               // bits in modulus
        words = (1 + ((mbits - 1) / bb)); // words per Big
        curvetype = EDWARDS;
        modtype = PSEUDO_MERSENNE;
        curve_a = -1;			   // Curve A parameter
        cof = 8;
        p = pow((Big)2, mbits) - 19; // Modulus
        r = pow((Big)2, 252) + (char *)"27742317777372353535851937790883648493"; // group order
        mip->IOBASE = 16;
        curve_b = (char *)"52036CEE2B6FFE738CC740797779E89800700A4D4141D8AB75EB4DCA135978A3"; // curve B parameter
        gx = (char *)"216936D3CD6E53FEC0A4E231FDD6DC5C692CC7609525A7B2C9562D608F25D51A";     // generator point
        gy = (char *)"6666666666666666666666666666666666666666666666666666666666666658";
        htpc=pow((Big)2,finde(p),p);
    }

    if (strcmp(curvename, "C25519") == 0)
    {
        curve = 2;
        if (chunk==16)
            bb=13;
        if (chunk==32)
            bb=29;
        if (chunk==64)
            bb=56;
        if (strcmp(lg, "javascript") == 0)
            bb=24;
        printf("Curve is C25519\n");
        strcpy(fieldname, "F25519");
        mbits = 255;
        words = (1 + ((mbits - 1) / bb));
        curvetype = MONTGOMERY;
        modtype = PSEUDO_MERSENNE;
        curve_a = 486662;
        cof = 8;
        p = pow((Big)2, mbits) - 19;
        r = pow((Big)2, 252) + (char *)"27742317777372353535851937790883648493";

        curve_b = 0;
        mip->IOBASE = 16;
        gx = (char *)"9";
        gy = 0;
        htpc=pow((Big)2,finde(p),p);
    }

    if (strcmp(curvename, "X448") == 0)
    {
        curve = 21;
        if (chunk==16)
            bb=13;
        if (chunk==32)
            bb=29;
        if (chunk==64)
            bb=58;
        if (strcmp(lg, "javascript") == 0)
            bb=23;
        printf("Curve is X448\n");
        strcpy(fieldname, "GOLDILOCKS");
        mbits = 448;
        words = (1 + ((mbits - 1) / bb));
        curvetype = MONTGOMERY;
        modtype = GENERALISED_MERSENNE;
        curve_a = 156326;
        cof = 4;
        p = pow((Big)2, mbits) - pow((Big)2, 224) - 1;
        mip->IOBASE = 16;
        r = pow((Big)2, 446) - (char *)"8335dc163bb124b65129c96fde933d8d723a70aadc873d6d54a7bb0d";
        curve_b = 0;
        gx = (char *)"5";
        gy = 0;
        htpc=pow(p-1,finde(p),p);
    }

    if (strcmp(curvename, "SECP160R1") == 0)
    {
        curve = 22;
        if (chunk==16)
            bb=13;
        if (chunk==32)
            bb=29;
        if (chunk==64)
            bb=56;
        if (strcmp(lg, "javascript") == 0)
            bb=24;
        printf("Curve is SECP160R1\n");
        strcpy(fieldname, curvename);
        mbits = 160;
        words = (1 + ((mbits - 1) / bb));
        curvetype = WEIERSTRASS;
        modtype = NOT_SPECIAL;
        curve_a = -3;
        cof = 1;

        mip->IOBASE = 16;
        p = (char *)"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF7FFFFFFF";
        r = (char *)"100000000000000000001F4C8F927AED3CA752257";
        curve_b = (char *)"1C97BEFC54BD7A8B65ACF89F81D4D4ADC565FA45";
        gx = (char *)"4A96B5688EF573284664698968C38BB913CBFC82";
        gy = (char *)"23A628553168947D59DCC912042351377AC5FB32";

        htpc=pow((Big)findZ(curve_a,curve_b,p),finde(p),p);
    }


    if (strcmp(curvename, "NIST256") == 0)
    {
        curve = 3;
        if (chunk==16)
            bb=13;
        if (chunk==32)
            bb=28;
        if (chunk==64)
            bb=56;
        if (strcmp(lg, "javascript") == 0)
            bb=24;
        printf("Curve is NIST256\n");
        strcpy(fieldname, curvename);
        mbits = 256;
        words = (1 + ((mbits - 1) / bb));
        curvetype = WEIERSTRASS;
        modtype = NOT_SPECIAL;
        curve_a = -3;
        cof = 1;

        p = (char *)"115792089210356248762697446949407573530086143415290314195533631308867097853951";
        r = (char *)"115792089210356248762697446949407573529996955224135760342422259061068512044369";
        mip->IOBASE = 16;
        curve_b = (char *)"5ac635d8aa3a93e7b3ebbd55769886bc651d06b0cc53b0f63bce3c3e27d2604b";
        gx = (char *)"6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296";
        gy = (char *)"4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5";

        htpc=pow((Big)findZ(curve_a,curve_b,p),finde(p),p);
    }

    if (strcmp(curvename, "SM2") == 0)
    {
        curve = 18;
        if (chunk==16)
            bb=13;
        if (chunk==32)
            bb=28;
        if (chunk==64)
            bb=56;
        if (strcmp(lg, "javascript") == 0)
            bb=24;
        printf("Curve is SM2\n");
        strcpy(fieldname, curvename);
        mbits = 256;
        words = (1 + ((mbits - 1) / bb));
        curvetype = WEIERSTRASS;
        modtype = NOT_SPECIAL;
        curve_a = -3;
        cof = 1;

        mip->IOBASE = 16;
        p = (char *)"FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF00000000FFFFFFFFFFFFFFFF";
        r = (char *)"FFFFFFFEFFFFFFFFFFFFFFFFFFFFFFFF7203DF6B21C6052B53BBF40939D54123";
        curve_b = (char *)"28E9FA9E9D9F5E344D5A9E4BCF6509A7F39789F515AB8F92DDBCBD414D940E93";
        gx = (char *)"32C4AE2C1F1981195F9904466A39C9948FE30BBFF2660BE1715A4589334C74C7";
        gy = (char *)"BC3736A2F4F6779C59BDCEE36B692153D0A9877CC62A474002DF32E52139F0A0";

        htpc=pow((Big)findZ(curve_a,curve_b,p),finde(p),p);
    }

    if (strcmp(curvename, "C13318") == 0)
    {
        curve = 19;
        if (chunk==16)
            bb=13;
        if (chunk==32)
            bb=29;
        if (chunk==64)
            bb=56;
        if (strcmp(lg, "javascript") == 0)
            bb=24;
        printf("Curve is C13318\n");
        strcpy(fieldname, "F25519");
        mbits = 255;
        words = (1 + ((mbits - 1) / bb));
        curvetype = WEIERSTRASS;  
        modtype = PSEUDO_MERSENNE;
        curve_a = -3;
        cof = 1;
        p=pow((Big)2, mbits) - 19;
        
        r=pow((Big)2,255)+(char *)"325610659388873400306201440571661405155";
        curve_b=13318;
        mip->IOBASE = 16;
        gx=5 ;                                                                              // (-7,114) is better
        gy=(char *)"6675AAD926BCA6F1381630E5166966369D4CCB04CF016DB5B8C8D3546B6EAD0B";

        htpc=pow((Big)findZ(curve_a,curve_b,p),finde(p),p);
    }

    if (strcmp(curvename, "JUBJUB") == 0)
    {
        curve = 20;
        if (chunk==16)
            bb=13;
        if (chunk==32)
            bb=29;
        if (chunk==64)
            bb=56;
        if (strcmp(lg, "javascript") == 0)
            bb=24;
        printf("Curve is JUBJUB\n");
        strcpy(fieldname, curvename);
        mbits=255;
        words = (1 + ((mbits - 1) / bb));
        curvetype = EDWARDS;
        modtype = NOT_SPECIAL;
        curve_a = -1;			   // Curve A parameter
        cof=8;
        mip->IOBASE = 16;
        p=(char *)"73eda753299d7d483339d80809a1d80553bda402fffe5bfeffffffff00000001";
        r=(char *)"0e7db4ea6533afa906673b0101343b00a6682093ccc81082d0970e5ed6f72cb7";
        curve_b=(char *)"2A9318E74BFA2B48F5FD9207E6BD7FD4292D7F6D37579D2601065FD6D6343EB1";

        gx=(char *)"5183972af8eff38ca624b4df00384882000c546bf2f39ede7f4ecf1a74f976c4";
        gy=(char *)"3b43f8472ca2fc2c9e8fcc5abd9dc308096c8707ffa6833b146bad709349702e";

        htpc=pow((Big)5,finde(p),p);
    }

    if (strcmp(curvename, "BRAINPOOL") == 0)
    {
        curve = 4;
        if (chunk==16)
            bb=13;
        if (chunk==32)
            bb=28;
        if (chunk==64)
            bb=56;
        if (strcmp(lg, "javascript") == 0)
            bb=24;
        printf("Curve is BRAINPOOL\n");
        strcpy(fieldname, curvename);
        mbits = 256;
        words = (1 + ((mbits - 1) / bb));
        curvetype = WEIERSTRASS;
        modtype = NOT_SPECIAL;
        curve_a = -3;
        cof = 1;
        p = (char *)"76884956397045344220809746629001649093037950200943055203735601445031516197751";
        mip->IOBASE = 16;
        r = (char *)"A9FB57DBA1EEA9BC3E660A909D838D718C397AA3B561A6F7901E0E82974856A7";
        mip->IOBASE = 10;
        curve_b = (char *)"46214326585032579593829631435610129746736367449296220983687490401182983727876";
        mip->IOBASE = 16;
        gx = (char *)"a3e8eb3cc1cfe7b7732213b23a656149afa142c47aafbc2b79a191562e1305f4";
        gy = (char *)"2d996c823439c56d7f7b22e14644417e69bcb6de39d027001dabe8f35b25c9be";

        htpc=pow((Big)findZ(curve_a,curve_b,p),finde(p),p);
    }

    if (strcmp(curvename, "ANSSI") == 0)
    {
        curve = 5;
        if (chunk==16)
            bb=13;
        if (chunk==32)
            bb=28;
        if (chunk==64)
            bb=56;
        if (strcmp(lg, "javascript") == 0)
            bb=24;
        printf("Curve is ANSSI\n");
        strcpy(fieldname, curvename);
        mbits = 256;
        words = (1 + ((mbits - 1) / bb));
        curvetype = WEIERSTRASS;
        modtype = NOT_SPECIAL;
        curve_a = -3;
        cof = 1;
        p = (char *)"109454571331697278617670725030735128145969349647868738157201323556196022393859";
        mip->IOBASE = 16;
        r = (char *)"F1FD178C0B3AD58F10126DE8CE42435B53DC67E140D2BF941FFDD459C6D655E1";
        mip->IOBASE = 10;
        curve_b = (char *)"107744541122042688792155207242782455150382764043089114141096634497567301547839";
        mip->IOBASE = 16;
        gx = (char *)"b6b3d4c356c139eb31183d4749d423958c27d2dcaf98b70164c97a2dd98f5cff";
        gy = (char *)"6142e0f7c8b204911f9271f0f3ecef8c2701c307e8e4c9e183115a1554062cfb";

        htpc=pow((Big)findZ(curve_a,curve_b,p),finde(p),p);
    }

    if (strcmp(curvename, "HIFIVE") == 0)
    {
        curve = 6;
        if (chunk==16)
            bb=13;
        if (chunk==32)
            bb=29;
        if (chunk==64)
            bb=60;
        if (strcmp(lg, "javascript") == 0)
            bb=23;
        printf("Curve is HIFIVE\n");
        strcpy(fieldname, curvename);
        mbits = 336;
        words = (1 + ((mbits - 1) / bb));
        curvetype = EDWARDS;
        modtype = PSEUDO_MERSENNE;
        curve_a = 1;
        cof = 8;
        p = pow((Big)2, 336) - 3;
        mip->IOBASE = 16;
        r = (char *)"200000000000000000000000000000000000000000071415FA9850C0BD6B87F93BAA7B2F95973E9FA805";
        mip->IOBASE = 10;
        curve_b = (char *)"11111";
//cout << "Square= " << pow(curve_b,(p-1)/2,p) << endl;
        mip->IOBASE = 16;
        gx = (char *)"C";
        gy = (char *)"C0DC616B56502E18E1C161D007853D1B14B46C3811C7EF435B6DB5D5650CA0365DB12BEC68505FE8632";
        htpc=pow((Big)2,finde(p),p);
    }

    if (strcmp(curvename, "GOLDILOCKS") == 0)
    {
        curve = 7;
        if (chunk==16)
            bb=13;
        if (chunk==32)
            bb=29;
        if (chunk==64)
            bb=58;
        if (strcmp(lg, "javascript") == 0)
            bb=23;
        printf("Curve is GOLDILOCKS\n");
        strcpy(fieldname, curvename);
        mbits = 448;
        words = (1 + ((mbits - 1) / bb));
        curvetype = EDWARDS;
        modtype = GENERALISED_MERSENNE;
        curve_a = 1;
        cof = 4;
        p = pow((Big)2, 448) - pow((Big)2, 224) - 1;
        r = (p + 1 - (char *)"28312320572429821613362531907042076847709625476988141958474579766324") / 4;
        mip->IOBASE = 10;
        curve_b = p - 39081;
        mip->IOBASE = 16;

        gx = (char *)"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa955555555555555555555555555555555555555555555555555555555";
        gy = (char *)"ae05e9634ad7048db359d6205086c2b0036ed7a035884dd7b7e36d728ad8c4b80d6565833a2a3098bbbcb2bed1cda06bdaeafbcdea9386ed";
        htpc=pow(p-1,finde(p),p);
    }

    if (strcmp(curvename, "NIST384") == 0)
    {
        curve = 8;
        if (chunk==16)
            bb=13;
        if (chunk==32)
            bb=29;
        if (chunk==64)
            bb=56;
        if (strcmp(lg, "javascript") == 0)
            bb=23;
        printf("Curve is NIST384\n");
        strcpy(fieldname, curvename);
        mbits = 384;
        words = (1 + ((mbits - 1) / bb));
        curvetype = WEIERSTRASS;
        modtype = NOT_SPECIAL;
        curve_a = -3;
        cof = 1;
        p = (char *)"39402006196394479212279040100143613805079739270465446667948293404245721771496870329047266088258938001861606973112319";
        r = p + 1 - (char *)"1388124618062372383606759648309780106643088307173319169677";

        curve_b = (char *)"27580193559959705877849011840389048093056905856361568521428707301988689241309860865136260764883745107765439761230575";
        mip->IOBASE = 16;
        gx = (char *)"aa87ca22be8b05378eb1c71ef320ad746e1d3b628ba79b9859f741e082542a385502f25dbf55296c3a545e3872760ab7";
        gy = (char *)"3617de4a96262c6f5d9e98bf9292dc29f8f41dbd289a147ce9da3113b5f0b8c00a60b1ce1d7e819d7a431d7c90ea0e5f";

        htpc=pow((Big)findZ(curve_a,curve_b,p),finde(p),p);
    }

    if (strcmp(curvename, "C41417") == 0)
    {
        curve = 9;
        if (chunk==16)
            bb=13;
        if (chunk==32)
            bb=29;
        if (chunk==64)
            bb=60;
        if (strcmp(lg, "javascript") == 0)
            bb=22;
        printf("Curve is C41417\n");
        strcpy(fieldname, curvename);
        mbits = 414;
        words = (1 + ((mbits - 1) / bb));
        curvetype = EDWARDS;
        modtype = PSEUDO_MERSENNE;
        curve_a = 1;
        p = pow((Big)2, mbits) - 17;
        cof = 8;
        r = pow((Big)2, 411) - (char *)"33364140863755142520810177694098385178984727200411208589594759"; // Group Order

        curve_b = (char *)"3617";
        mip->IOBASE = 16;
        gx = (char *)"1a334905141443300218c0631c326e5fcd46369f44c03ec7f57ff35498a4ab4d6d6ba111301a73faa8537c64c4fd3812f3cbc595";
        gy = (char *)"22";
        htpc=pow(p-1,finde(p),p);
    }

    if (strcmp(curvename, "C1174") == 0)
    {
        curve = 23;
        if (chunk==16)
            bb=13;
        if (chunk==32)
            bb=29;
        if (chunk==64)
            bb=56;
        if (strcmp(lg, "javascript") == 0)
            bb=24;
        printf("Curve is C1174\n");
        strcpy(fieldname, curvename);
        mbits = 251;
        words = (1 + ((mbits - 1) / bb));
        curvetype = EDWARDS;
        modtype = PSEUDO_MERSENNE;
        curve_a = 1;
        p = pow((Big)2, mbits) - 9;
        cof = 4;
        r=(Big)(char *)"904625697166532776746648320380374280092339035279495474023489261773642975601";
        curve_b = p-1174;
        mip->IOBASE = 16;

        gx=(char *)"037fbb0cea308c479343aee7c029a190c021d96a492ecd6516123f27bce29eda";
        gy=(char *)"06b72f82d47fb7cc6656841169840e0c4fe2dee2af3f976ba4ccb1bf9b46360e";
        htpc=pow(p-1,finde(p),p);

    }

    if (strcmp(curvename, "C1665") == 0)
    {
        curve = 24;
        if (chunk==16)
            bb=13;
        if (chunk==32)
            bb=29;
        if (chunk==64)
            bb=60;
        if (strcmp(lg, "javascript") == 0)
            bb=23;
        printf("Curve is C1665\n");
        strcpy(fieldname, curvename);
        mbits = 166;
        words = (1 + ((mbits - 1) / bb));
        curvetype = EDWARDS;
        modtype = PSEUDO_MERSENNE;
        curve_a = 1;
        p = pow((Big)2, mbits) - 5;
        cof = 4;
        curve_b = 5766;

//cout << "Square= " << pow(curve_b,(p-1)/2,p) << endl;

        mip->IOBASE = 16;
        r=(char *)"FFFFFFFFFFFFFFFFFFFFD5EF0180FF0A99DBA8B27";
        //r=(char*)"1000000000000000000002A10FE7F00F56624574D7";
        gx=(char *)"14C94DA505B809A0618EE0F666C671B9DBF9D52398";
        gy=(char *)"29414549BC0A13BFC7739D62DC2C7087B244920345";
        htpc=pow(p-1,finde(p),p);
    }

    if (strcmp(curvename, "MDC") == 0)
    {
        curve = 25;
        if (chunk==16)
            bb=13;
        if (chunk==32)
            bb=28;
        if (chunk==64)
            bb=56;
        if (strcmp(lg, "javascript") == 0)
            bb=24;
        printf("Curve is Million Dollar Curve\n");
        strcpy(fieldname, curvename);
        mbits = 256;
        words = (1 + ((mbits - 1) / bb));
        curvetype = EDWARDS;
        modtype = NOT_SPECIAL;
        mip->IOBASE = 10;
        curve_a = 1;
        p = (char *)"109112363276961190442711090369149551676330307646118204517771511330536253156371";
        cof = 4;
        curve_b = (char *)"39384817741350628573161184301225915800358770588933756071948264625804612259721";
        r=(char *)"27278090819240297610677772592287387918930509574048068887630978293185521973243";
        gx=(char *)"82549803222202399340024462032964942512025856818700414254726364205096731424315";
        gy=(char *)"91549545637415734422658288799119041756378259523097147807813396915125932811445";
        mip->IOBASE = 16;
        htpc=pow(p-1,finde(p),p);
    }


    if (strcmp(curvename, "NIST521") == 0)
    {
        curve = 10;
        if (chunk==16)
            bb=13;
        if (chunk==32)
            bb=28;
        if (chunk==64)
            bb=60;
        if (strcmp(lg, "javascript") == 0)
            bb=23;
        printf("Curve is NIST521\n");
        strcpy(fieldname, curvename);
        mbits = 521;
        words = (1 + ((mbits - 1) / bb));
        curvetype = WEIERSTRASS;
        modtype = NOT_SPECIAL;
        curve_a = -3;
        cof = 1;
        p = pow((Big)2, mbits) - 1;

        mip->IOBASE = 16;
        r = (char *)"1FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFA51868783BF2F966B7FCC0148F709A5D03BB5C9B8899C47AEBB6FB71E91386409"; // Group Order
        curve_b = (char *)"51953EB9618E1C9A1F929A21A0B68540EEA2DA725B99B315F3B8B489918EF109E156193951EC7E937B1652C0BD3BB1BF073573DF883D2C34F1EF451FD46B503F00";

        gx = (char *)"C6858E06B70404E9CD9E3ECB662395B4429C648139053FB521F828AF606B4D3DBAA14B5E77EFE75928FE1DC127A2FFA8DE3348B3C1856A429BF97E7E31C2E5BD66";
        gy = (char *)"11839296A789A3BC0045C8A5FB42C7D1BD998F54449579B446817AFBD17273E662C97EE72995EF42640C550B9013FAD0761353C7086A272C24088BE94769FD16650";

        htpc=pow((Big)findZ(curve_a,curve_b,p),finde(p),p);

    }

    if (strcmp(curvename, "NUMS256W") == 0)
    {
        curve = 11;
        if (chunk==16)
            bb=13;
        if (chunk==32)
            bb=28;
        if (chunk==64)
            bb=56;
        if (strcmp(lg, "javascript") == 0)
            bb=24;
        printf("Curve is NUMS256W\n");
        strcpy(fieldname, "256PMW");
        mbits = 256;
        words = (1 + ((mbits - 1) / bb));
        curvetype = WEIERSTRASS;
        modtype = PSEUDO_MERSENNE;
        curve_a = -3;
        cof = 1;
        p = pow((Big)2, mbits) - 189;

        mip->IOBASE = 16;
        r = (char *)"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFE43C8275EA265C6020AB20294751A825"; // Group Order
        curve_b = (char *)"25581";
        gx = (char *)"BC9ED6B65AAADB61297A95A04F42CB0983579B0903D4C73ABC52EE1EB21AACB1";
        gy = (char *)"D08FC0F13399B6A673448BF77E04E035C955C3D115310FBB80B5B9CB2184DE9F";

        htpc=pow((Big)findZ(curve_a,curve_b,p),finde(p),p);
    }

    if (strcmp(curvename, "NUMS256E") == 0)
    {
        curve = 12;
        if (chunk==16)
            bb=13;
        if (chunk==32)
            bb=29;
        if (chunk==64)
            bb=56;
        if (strcmp(lg, "javascript") == 0)
            bb=24;
        printf("Curve is NUMS256E\n");
        strcpy(fieldname, "256PME");
        mbits = 256;
        words = (1 + ((mbits - 1) / bb));
        curvetype = EDWARDS;
        modtype = PSEUDO_MERSENNE;
        curve_a = 1;
        cof = 4;
        p = pow((Big)2, mbits) - 189;

        mip->IOBASE = 16;
        r = (char *)"4000000000000000000000000000000041955AA52F59439B1A47B190EEDD4AF5"; // Group Order
        curve_b = (char *)"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFC355";
        gx = (char *)"8A7514FB6AEA237DCD1E3D5F69209BD60C398A0EE3083586A0DEC0902EED13DA";
        gy = (char *)"44D53E9FD9D925C7CE9665D9A64B8010715F61D810856ED32FA616E7798A89E6";
        htpc=pow(p-1,finde(p),p);
    }


    if (strcmp(curvename, "NUMS384W") == 0)
    {
        curve = 13;
        if (chunk==16)
            bb=13;
        if (chunk==32)
            bb=29;
        if (chunk==64)
            bb=58;
        if (strcmp(lg, "javascript") == 0)
            bb=23;
        printf("Curve is NUMS384W\n");
        strcpy(fieldname, "384PM");
        mbits = 384;
        words = (1 + ((mbits - 1) / bb));
        curvetype = WEIERSTRASS;
        modtype = PSEUDO_MERSENNE;
        curve_a = -3;
        cof = 1;
        p = pow((Big)2, mbits) - 317;

        mip->IOBASE = 16;
        r = (char *)"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFD61EAF1EEB5D6881BEDA9D3D4C37E27A604D81F67B0E61B9"; // Group Order
        curve_b = (char *)"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF77BB";
        gx = (char *)"757956F0B16F181C4880CA224105F1A60225C1CDFB81F9F4F3BD291B2A6CC742522EED100F61C47BEB9CBA042098152A";
        gy = (char *)"ACDEE368E19B8E38D7E33D300584CF7EB0046977F87F739CB920837D121A837EBCD6B4DBBFF4AD265C74B8EC66180716";

        htpc=pow((Big)findZ(curve_a,curve_b,p),finde(p),p);
    }

    if (strcmp(curvename, "NUMS384E") == 0)
    {
        curve = 14;
        if (chunk==16)
            bb=13;
        if (chunk==32)
            bb=29;
        if (chunk==64)
            bb=56;
        if (strcmp(lg, "javascript") == 0)
            bb=23;
        printf("Curve is NUMS384E\n");
        strcpy(fieldname, "384PM");
        mbits = 384;
        words = (1 + ((mbits - 1) / bb));
        curvetype = EDWARDS;
        modtype = PSEUDO_MERSENNE;
        curve_a = 1;
        cof = 4;
        p = pow((Big)2, mbits) - 317;

        mip->IOBASE = 16;
        r = (char *)"3FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFE2471A1CB46BE1CF61E4555AAB35C87920B9DCC4E6A3897D"; // Group Order
        curve_b = (char *)"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFD19F";
        gx = (char *)"61B111FB45A9266CC0B6A2129AE55DB5B30BF446E5BE4C005763FFA8F33163406FF292B16545941350D540E46C206BDE";
        gy = (char *)"82983E67B9A6EEB08738B1A423B10DD716AD8274F1425F56830F98F7F645964B0072B0F946EC48DC9D8D03E1F0729392";
        htpc=pow(p-1,finde(p),p);
    }

    if (strcmp(curvename, "NUMS512W") == 0)
    {
        curve = 15;
        if (chunk==16)
            bb=13;
        if (chunk==32)
            bb=29;
        if (chunk==64)
            bb=60;
        if (strcmp(lg, "javascript") == 0)
            bb=23;
        printf("Curve is NUMS512W\n");
        strcpy(fieldname, "512PM");
        mbits = 512;
        words = (1 + ((mbits - 1) / bb));
        curvetype = WEIERSTRASS;
        modtype = PSEUDO_MERSENNE;
        curve_a = -3;
        cof = 1;
        p = pow((Big)2, mbits) - 569;

        mip->IOBASE = 16;
        r = (char *)"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF5B3CA4FB94E7831B4FC258ED97D0BDC63B568B36607CD243CE153F390433555D"; // Group Order
        curve_b = (char *)"1D99B";
        gx = (char *)"3AC03447141D0A93DA2B7002A03D3B5298CAD83BB501F6854506E0C25306D9F95021A151076B359E93794286255615831D5D60137D6F5DE2DC8287958CABAE57";
        gy = (char *)"943A54CA29AD56B3CE0EEEDC63EBB1004B97DBDEABBCBB8C8F4B260C7BD14F14A28415DA8B0EEDE9C121A840B25A5602CF2B5C1E4CFD0FE923A08760383527A6";

        htpc=pow((Big)findZ(curve_a,curve_b,p),finde(p),p);
    }

    if (strcmp(curvename, "NUMS512E") == 0)
    {
        curve = 16;
        if (chunk==16)
            bb=13;
        if (chunk==32)
            bb=29;
        if (chunk==64)
            bb=60;
        if (strcmp(lg, "javascript") == 0)
            bb=23;
        printf("Curve is NUMS512E\n");
        strcpy(fieldname, "512PM");
        mbits = 512;
        words = (1 + ((mbits - 1) / bb));
        curvetype = EDWARDS;
        modtype = PSEUDO_MERSENNE;
        curve_a = 1;
        cof = 4;
        p = pow((Big)2, mbits) - 569;

        mip->IOBASE = 16;
        r = (char *)"3FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFB4F0636D2FCF91BA9E3FD8C970B686F52A4605786DEFECFF67468CF51BEED46D"; // Group Order
        curve_b = (char *)"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFECBEF";
        gx = (char *)"DF8E316D128DB69C7A18CB7888D3C5332FD1E79F4DC4A38227A17EBE273B81474621C14EEE46730F78BDC992568904AD0FE525427CC4F015C5B9AB2999EC57FE";
        gy = (char *)"6D09BFF39D49CA7198B0F577A82A256EE476F726D8259D22A92B6B95909E834120CA53F2E9963562601A06862AECC1FD0266D38A9BF1D01F326DDEC0C1E2F5E1";
        htpc=pow(p-1,finde(p),p);
    }


    if (strcmp(curvename, "SECP256K1") == 0)
    {   // SECP256K1
        curve = 17;
        if (chunk==16)
            bb=13;
        if (chunk==32)
            bb=28;
        if (chunk==64)
            bb=56;
        if (strcmp(lg, "javascript") == 0)
            bb=24;
        printf("Curve is SECP256K1\n");
        strcpy(fieldname, "SECP256K1");
        mbits = 256;               // bits in modulus
        words = (1 + ((mbits - 1) / bb)); // words per Big
        curvetype = WEIERSTRASS;
        /*if (chunk<64)*/ modtype = NOT_SPECIAL;
        //modtype=PSEUDO_MERSENNE;
        curve_a = 0;			   // Curve A parameter
        cof = 1;
        p = pow((Big)2, mbits) - pow((Big)2, 32) - 977; // Modulus
        mip->IOBASE = 16;
        r = (char *)"FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEBAAEDCE6AF48A03BBFD25E8CD0364141"; // group order

        curve_b = (char *)"7"; // curve B parameter
        gx = (char *)"79BE667EF9DCBBAC55A06295CE870B07029BFCDB2DCE28D959F2815B16F81798";     // generator point
        gy = (char *)"483ADA7726A3C4655DA4FBFC0E1108A8FD17B448A68554199C47D08FFB10D4B8";

        modulo(p);

        zsqrtm3=sqrt((ZZn)-3);
        sqrtm3=(Big)zsqrtm3;
        if (sqrtm3%2==1)
            sqrtm3=p-sqrtm3;
// Use above if using SVDW for hashing, else..
        htpc=pow((Big)-11,(p-3)/4,p);

        ad = (char *)"3f8731abdd661adca08a5558f0f5d272e953d363cb6f0e5d405447c01a444533";

        bd=1771;
        xt=4; yt=4; ncs=xt+xt-2+yt+yt-1;

        pc[0]=(char *)"8e38e38e38e38e38e38e38e38e38e38e38e38e38e38e38e38e38e38daaaaa88c";
        pc[1]=(char *)"534c328d23f234e6e2a413deca25caece4506144037c40314ecbd0b53d9dd262";
        pc[2]=(char *)"07d3d4c80bc321d5b9f315cea7fd44c5d595d2fc0bf63b92dfff1044f17c6581";
        pc[3]=(char *)"8e38e38e38e38e38e38e38e38e38e38e38e38e38e38e38e38e38e38daaaaa8c7";
        pc[4]=(char *)"edadc6f64383dc1df7c4b2d51b54225406d36b641f5e41bbc52a56612a8c6d14";
        pc[5]=(char *)"d35771193d94918a9ca34ccbb7b640dd86cd409542f8487d9fe6b745781eb49b";
        pc[6]=(char *)"2f684bda12f684bda12f684bda12f684bda12f684bda12f684bda12f38e38d84";
        pc[7]=(char *)"29a6194691f91a73715209ef6512e576722830a201be2018a765e85a9ecee931";
        pc[8]=(char *)"c75e0c32d5cb7c0fa9d0a54b12a0a6d5647ab046d686da6fdffc90fc201d71a3";
        pc[9]=(char *)"4bda12f684bda12f684bda12f684bda12f684bda12f684bda12f684b8e38e23c";
        pc[10]=(char *)"6484aa716545ca2cf3a70c3fa8fe337e0a3d21162f0d6299a7bf8192bfd2a76f";
        pc[11]=(char *)"7a06534bb8bdb49fd5e9e6632722c2989467c1bfc8e8d978dfb425d2685c2573";
        pc[12]=(char *)"fffffffffffffffffffffffffffffffffffffffffffffffffffffffefffff93b";

    }


    if (strcmp(curvename, "TWEEDLEDUM") == 0)
    {   // 
        curve = 26;
        if (chunk==16)
            bb=13;
        if (chunk==32)
            bb=29;
        if (chunk==64)
            bb=56;
        if (strcmp(lg, "javascript") == 0)
            bb=24;
        printf("Curve is TWEEDLEDUM\n");
        strcpy(fieldname, "TWEEDLEDUM");
        mbits = 255;               // bits in modulus
        words = (1 + ((mbits - 1) / bb)); // words per Big
        curvetype = WEIERSTRASS;
        /*if (chunk<64)*/ modtype = NOT_SPECIAL;
        //modtype=PSEUDO_MERSENNE;
        curve_a = 0;			   // Curve A parameter
        cof = 1;
        p = pow((Big)2, 254) + (Big)(char *)"4707489545178046908921067385359695873"; // Modulus
        r = pow((Big)2, 254) + (Big)(char *)"4707489544292117082687961190295928833"; // group order

        curve_b = (char *)"5"; // curve B parameter
        gx = p-1;  // generator point
        gy = 2;

        modulo(p);

        zsqrtm3=sqrt((ZZn)-3);
        sqrtm3=(Big)zsqrtm3;
        if (sqrtm3%2==1)
            sqrtm3=p-sqrtm3;

        htpc=pow((Big)5,finde(p),p);  // 5 is a QNR
        mip->IOBASE=16;
    }

    if (strcmp(curvename, "TWEEDLEDEE") == 0)
    {   // 
        curve = 26;
        if (chunk==16)
            bb=13;
        if (chunk==32)
            bb=29;
        if (chunk==64)
            bb=56;
        if (strcmp(lg, "javascript") == 0)
            bb=24;
        printf("Curve is TWEEDLEDEE\n");
        strcpy(fieldname, "TWEEDLEDEE");
        mbits = 255;               // bits in modulus
        words = (1 + ((mbits - 1) / bb)); // words per Big
        curvetype = WEIERSTRASS;
        /*if (chunk<64)*/ modtype = NOT_SPECIAL;
        //modtype=PSEUDO_MERSENNE;
        curve_a = 0;			   // Curve A parameter
        cof = 1;
        p = pow((Big)2, 254) + (Big)(char *)"4707489544292117082687961190295928833"; // Modulus
        r = pow((Big)2, 254) + (Big)(char *)"4707489545178046908921067385359695873"; // group order

        curve_b = (char *)"5"; // curve B parameter
        gx = p-1;  // generator point
        gy = 2;

        modulo(p);

        zsqrtm3=sqrt((ZZn)-3);
        sqrtm3=(Big)zsqrtm3;
        if (sqrtm3%2==1)
            sqrtm3=p-sqrtm3;

        htpc=pow((Big)5,finde(p),p); // 5 is a QNR
        mip->IOBASE=16;
    }

    if (strcmp(curvename, "BN254") == 0)
    {
        curve = PS+0;
        if (chunk==16)
            bb=13;
        if (chunk==32)
            bb=28;
        if (chunk==64)
            bb=56;
        if (strcmp(lg, "javascript") == 0)
            bb=24;
        printf("Curve is BN254\n");
        strcpy(fieldname, curvename);
        mbits = 254;
        words = (1 + ((mbits - 1) / bb));
        curvetype = WEIERSTRASS;
        modtype = NOT_SPECIAL;
        curve_a = 0;
        mip->IOBASE = 16;
        x = (char *)"4080000000000001";  // Fast but not GT_STRONG parameter

        atebits = bits(3 * (6 * x - 2)) - 1;
        hw = hamming(6 * x - 2) + 2;


        p = 36 * pow(x, 4) - 36 * pow(x, 3) + 24 * x * x - 6 * x + 1; // Modulus
        t = 6 * x * x + 1;
        r = p + 1 - t;                 // Group order
        curve_b = 2;
        gx = p - 1;                    // generator point in G1
        gy = 1;
        cof = 1;
        ecurve((Big)0, curve_b, p, MR_AFFINE);
        mip->TWIST = MR_SEXTIC_D;      // twist type

        Xa.set((ZZn)0, (ZZn) - 1);
        Ya.set((ZZn)1, ZZn(0));
        Q.set(Xa, Ya);

        Q = (p - 1 + t) * Q;           // generator point in G2
        cru = (18 * pow(x, 3) - 18 * x * x + 9 * x - 2); // cube root of unity for GLV method

        sqrtm3=p-(36*pow(x,3) - 36*x*x + 18*x - 3);

        htpc=pow((Big)findZ(curve_a,curve_b,p),finde(p),p);

        cout << "Hash to Curve Z for G2= " << findZ2(curve_b) << endl;
    }


    if (strcmp(curvename, "BN254CX") == 0)
    {
        curve = PS+1;
        if (chunk==16)
            bb=13;
        if (chunk==32)
            bb=28;
        if (chunk==64)
            bb=56;
        if (strcmp(lg, "javascript") == 0)
            bb=24;
        printf("Curve is BN254CX\n");
        strcpy(fieldname, curvename);
        mbits = 254;
        words = (1 + ((mbits - 1) / bb));
        curvetype = WEIERSTRASS;
        modtype = NOT_SPECIAL;
        curve_a = 0;
        mip->IOBASE = 16;
        x = (char *)"4000000003C012B1";

        atebits = bits(3 * (6 * x - 2)) - 1;
        hw = hamming(6 * x - 2) + 2;


        p = 36 * pow(x, 4) - 36 * pow(x, 3) + 24 * x * x - 6 * x + 1;
        t = 6 * x * x + 1;
        r = p + 1 - t;
        curve_b = 2;
        gx = p - 1;
        gy = 1;
        cof = 1;
        ecurve((Big)0, curve_b, p, MR_AFFINE);
        mip->TWIST = MR_SEXTIC_D;

        Xa.set((ZZn)0, (ZZn) - 1);
        Ya.set((ZZn)1, ZZn(0));
        Q.set(Xa, Ya);

        Q = (p - 1 + t) * Q;
        //cofactor(Q,X,x);
        cru = (18 * pow(x, 3) - 18 * x * x + 9 * x - 2);
        sqrtm3=p-(36*pow(x,3) - 36*x*x + 18*x - 3);

        htpc=pow((Big)findZ(curve_a,curve_b,p),finde(p),p);

        cout << "Hash to Curve Z for G2= " << findZ2(curve_b) << endl;
    }

    if (strcmp(curvename, "BLS12383") == 0)
    {
        curve = PS+2;
        if (chunk==16)
            bb=13;
        if (chunk==32)
            bb=29;
        if (chunk==64)
            bb=58;
        if (strcmp(lg, "javascript") == 0)
            bb=23;
        printf("Curve is BLS12383\n");
        strcpy(fieldname, curvename);
        mbits = 383;
        words = (1 + ((mbits - 1) / bb));
        curvetype = WEIERSTRASS;
        modtype = NOT_SPECIAL;
        curve_a = 0;

        mip->IOBASE = 16;
        x = (char *)"10008000001001200";	// SIGN_OF_X is POSITIVE

        atebits = bits(3 * x) - 1;
        hw = hamming(x);


        p = (pow(x, 6) - 2 * pow(x, 5) + 2 * pow(x, 3) + x + 1) / 3;
        t = x + 1;
        r = pow(x, 4) - x * x + 1;
        cof = (p + 1 - t) / r;

        cof = x - 1;

        //	gx=-2; gy=-1;
        //	curve_b=9;
        gx = 1; gy = 4;
        curve_b = 15;
        ecurve((Big)0, curve_b, p, MR_AFFINE);
        //	mip->TWIST=MR_SEXTIC_D;
        mip->TWIST = MR_SEXTIC_M;

        P.set(gx, gy);
        P *= (x - 1) * (x - 1) / 3;
        P.get(gx, gy);

        Xa = 1;
        while (!Q.set(Xa))
        {
            Xa = Xa + (ZZn2)1;
        }

        TT = t * t - 2 * p;
        PP = p * p;
        FF = sqrt((4 * PP - TT * TT) / 3);
        np = PP + 1 - (-3 * FF + TT) / 2; // 2 possibilities...

        Q = (np / r) * Q;

        zcru = pow((ZZn)2, (p - 1) / 3);
        //	zcru*=zcru;   // right cube root of unity
        cru = (Big)zcru;

        sqrtm3=p-(2 * pow(x, 5) - 6*pow(x,4) + 6*pow(x,3) - 2*x + 3);

        htpc=pow((Big)findZ(curve_a,curve_b,p),finde(p),p);

        cout << "Hash to Curve Z for G2= " << findZ2(curve_b) << endl;
    }

    if (strcmp(curvename, "BLS12381") == 0)
    {
        curve = PS+3;
        if (chunk==16)
            bb=13;
        if (chunk==32)
            bb=29;
        if (chunk==64)
            bb=58;
        if (strcmp(lg, "javascript") == 0)
            bb=23;
        printf("Curve is BLS12381\n");
        strcpy(fieldname, curvename);
        mbits = 381;
        words = (1 + ((mbits - 1) / bb));
        curvetype = WEIERSTRASS;
        modtype = NOT_SPECIAL;
        curve_a = 0;
        mip->IOBASE = 16;
        x = (char *)"d201000000010000";            // SIGN_OF_X is NEGATIVE

        atebits = bits(3 * x) - 1;
        hw = hamming(x);

        p = (pow(x, 6) + 2 * pow(x, 5) - 2 * pow(x, 3) - x + 1) / 3;
        t = -x + 1;
        r = pow(x, 4) - x * x + 1;
        cof = (p + 1 - t) / r;

        cof = x + 1;

        curve_b = 4;
        ecurve((Big)0, curve_b, p, MR_AFFINE);
        mip->TWIST = MR_SEXTIC_M;

        gx = 1;
        while (!P.set(gx))
        {
            gx = gx + 1;
        }
        P *= (x + 1) * (x + 1) / 3;
        P.get(gx, gy);

        Xa = 1;
        while (!Q.set(Xa))
        {
            Xa = Xa + (ZZn2)1;
        }
        Q.get(Xa, Ya);
        Ya = -Ya;
        Q.set(Xa, Ya);

        TT = t * t - 2 * p;
        PP = p * p;
        FF = sqrt((4 * PP - TT * TT) / 3);
        np = PP + 1 - (-3 * FF + TT) / 2; // 2 possibilities...

        Q = (np / r) * Q;

        zcru = pow((ZZn)2, (p - 1) / 3);
        //zcru*=zcru;   // right cube root of unity ?? if x>0 do this??
        cru = (Big)zcru;
        sqrtm3=  p+(-2 * pow(x, 5) - 6*pow(x,4) - 6*pow(x,3) + 2*x + 3);

// Use above if using SVDW for hashing, else..
        htpc=pow((Big)11,(p-3)/4,p);
        htpc2=pow((Big)5,(p-3)/4,p);  // where 5 is the norm of (-2,-1)

        adr = 0;
        adi = 240;
        bdr = 1012;
        bdi = 1012;

        xt=4; yt=4; ncs2=xt+xt-2+yt+yt-1;

        pcr[0]=(char *)"171d6541fa38ccfaed6dea691f5fb614cb14b4e7f4e810aa22d6108f142b85757098e38d0f671c7188e2aaaaaaaa5ed1";
        pci[0]=0;
        pcr[1]=(char *)"11560bf17baa99bc32126fced787c88f984f87adf7ae0c7f9a208c6b4f20a4181472aaa9cb8d555526a9ffffffffc71e";
        pci[1]=(char *)"8ab05f8bdd54cde190937e76bc3e447cc27c3d6fbd7063fcd104635a790520c0a395554e5c6aaaa9354ffffffffe38d";
        pcr[2]=0;
        pci[2]=(char *)"11560bf17baa99bc32126fced787c88f984f87adf7ae0c7f9a208c6b4f20a4181472aaa9cb8d555526a9ffffffffc71a";
        pcr[3]=(char *)"5c759507e8e333ebb5b7a9a47d7ed8532c52d39fd3a042a88b58423c50ae15d5c2638e343d9c71c6238aaaaaaaa97d6";
        pci[3]=(char *)"5c759507e8e333ebb5b7a9a47d7ed8532c52d39fd3a042a88b58423c50ae15d5c2638e343d9c71c6238aaaaaaaa97d6";
        pcr[4]=(char *)"c";
        pci[4]=(char *)"1a0111ea397fe69a4b1ba7b6434bacd764774b84f38512bf6730d2a0f6b0f6241eabfffeb153ffffb9feffffffffaa9f";
        pcr[5]=0;
        pci[5]=(char *)"1a0111ea397fe69a4b1ba7b6434bacd764774b84f38512bf6730d2a0f6b0f6241eabfffeb153ffffb9feffffffffaa63";
        pcr[6]=(char *)"124c9ad43b6cf79bfbf7043de3811ad0761b0f37a1e26286b0e977c69aa274524e79097a56dc4bd9e1b371c71c718b10";
        pci[6]=0;
        pcr[7]=(char *)"11560bf17baa99bc32126fced787c88f984f87adf7ae0c7f9a208c6b4f20a4181472aaa9cb8d555526a9ffffffffc71c";
        pci[7]=(char *)"8ab05f8bdd54cde190937e76bc3e447cc27c3d6fbd7063fcd104635a790520c0a395554e5c6aaaa9354ffffffffe38f";
        pcr[8]=0;
        pci[8]=(char *)"5c759507e8e333ebb5b7a9a47d7ed8532c52d39fd3a042a88b58423c50ae15d5c2638e343d9c71c6238aaaaaaaa97be";
        pcr[9]=(char *)"1530477c7ab4113b59a4c18b076d11930f7da5d4a07f649bf54439d87d27e500fc8c25ebf8c92f6812cfc71c71c6d706";
        pci[9]=(char *)"1530477c7ab4113b59a4c18b076d11930f7da5d4a07f649bf54439d87d27e500fc8c25ebf8c92f6812cfc71c71c6d706";
        pcr[10]=(char *)"12";
        pci[10]=(char *)"1a0111ea397fe69a4b1ba7b6434bacd764774b84f38512bf6730d2a0f6b0f6241eabfffeb153ffffb9feffffffffaa99";
        pcr[11]=0;
        pci[11]=(char *)"1a0111ea397fe69a4b1ba7b6434bacd764774b84f38512bf6730d2a0f6b0f6241eabfffeb153ffffb9feffffffffa9d3";
        pcr[12]=(char *)"1a0111ea397fe69a4b1ba7b6434bacd764774b84f38512bf6730d2a0f6b0f6241eabfffeb153ffffb9feffffffffa8fb";
        pci[12]=(char *)"1a0111ea397fe69a4b1ba7b6434bacd764774b84f38512bf6730d2a0f6b0f6241eabfffeb153ffffb9feffffffffa8fb";


        ad = (char *)"144698a3b8e9433d693a02c96d4982b0ea985383ee66a8d8e8981aefd881ac98936f8da0e0f97f5cf428082d584c1d";
        bd = (char *)"12e2908d11688030018b12e8753eee3b2016c1f0f24f4070a0b9c14fcef35ef55a23215a316ceaa5d1cc48e98e172be0";
        xt=12; yt=16; ncs=xt+xt-2+yt+yt-1;

        pc[0]=(char *)"6e08c248e260e70bd1e962381edee3d31d79d7e22c837bc23c0bf1bc24c6b68c24b1b80b64d391fa9c8ba2e8ba2d229";
        pc[1]=(char *)"10321da079ce07e272d8ec09d2565b0dfa7dccdde6787f96d50af36003b14866f69b771f8c285decca67df3f1605fb7b";
        pc[2]=(char *)"169b1f8e1bcfa7c42e0c37515d138f22dd2ecb803a0c5c99676314baf4bb1b7fa3190b2edc0327797f241067be390c9e";
        pc[3]=(char *)"80d3cf1f9a78fc47b90b33563be990dc43b756ce79f5574a2c596c928c5d1de4fa295f296b74e956d71986a8497e317";
        pc[4]=(char *)"17b81e7701abdbe2e8743884d1117e53356de5ab275b4db1a682c62ef0f2753339b7c8f8c8f475af9ccb5618e3f0c88e";
        pc[5]=(char *)"d6ed6553fe44d296a3726c38ae652bfb11586264f0f8ce19008e218f9c86b2a8da25128c1052ecaddd7f225a139ed84";
        pc[6]=(char *)"1630c3250d7313ff01d1201bf7a74ab5db3cb17dd952799b9ed3ab9097e68f90a0870d2dcae73d19cd13c1c66f652983";
        pc[7]=(char *)"e99726a3199f4436642b4b3e4118e5499db995a1257fb3f086eeb65982fac18985a286f301e77c451154ce9ac8895d9";
        pc[8]=(char *)"1778e7166fcc6db74e0609d307e55412d7f5e4656a8dbf25f1b33289f1b330835336e25ce3107193c5b388641d9b6861";
        pc[9]=(char *)"d54005db97678ec1d1048c5d10a9a1bce032473295983e56878e501ec68e25c958c3e3d2a09729fe0179f9dac9edcb0";
        pc[10]=(char *)"17294ed3e943ab2f0588bab22147a81c7c17e75b2f6a8417f565e33c70d1e86b4838f2a6f318c356e834eef1b3cb83bb";
        pc[11]=(char *)"11a05f2b1e833340b809101dd99815856b303e88a2d7005ff2627b56cdb4e2c85610c2d5f2e62d6eaeac1662734649b7";

        pc[12]=(char *)"95fc13ab9e92ad4476d6e3eb3a56680f682b4ee96f7d03776df533978f31c1593174e4b4b7865002d6384d168ecdd0a";        
        pc[13]=(char *)"a10ecf6ada54f825e920b3dafc7a3cce07f8d1d7161366b74100da67f39883503826692abba43704776ec3a79a1d641";
        pc[14]=(char *)"14a7ac2a9d64a8b230b3f5b074cf01996e7f63c21bca68a81996e1cdf9822c580fa5b9489d11e2d311f7d99bbdcc5a5e";
        pc[15]=(char *)"772caacf16936190f3e0c63e0596721570f5799af53a1894e2e073062aede9cea73b3538f0de06cec2574496ee84a3a";
        pc[16]=(char *)"e7355f8e4e667b955390f7f0506c6e9395735e9ce9cad4d0a43bcef24b8982f7400d24bc4228f11c02df9a29f6304a5";
        pc[17]=(char *)"13a8e162022914a80a6f1d5f43e7a07dffdfc759a12062bb8d6b44e833b306da9bd29ba81f35781d539d395b3532a21e";
        pc[18]=(char *)"3425581a58ae2fec83aafef7c40eb545b08243f16b1655154cca8abc28d6fd04976d5243eecf5c4130de8938dc62cd8";
        pc[19]=(char *)"b2962fe57a3225e8137e629bff2991f6f89416f5a718cd1fca64e00b11aceacd6a3d0967c94fedcfcc239ba5cb83e19";
        pc[20]=(char *)"12561a5deb559c4348b4711298e536367041e8ca0cf0800c0126c2588c48bf5713daa8846cb026e9e5c8276ec82b3bff";
        pc[21]=(char *)"8ca8d548cff19ae18b2e62f4bd3fa6f01d5ef4ba35b48ba9c9588617fc8ac62b558d681be343df8993cf9fa40d21b1c";

        pc[22]=(char *)"15e6be4e990f03ce4ea50b3b42df2eb5cb181d8f84965a3957add4fa95af01b2b665027efec01c7704b456be69c8b604";
        pc[23]=(char *)"5c129645e44cf1102a159f748c4a3fc5e673d81d7e86568d9ab0f5d396a7ce46ba1049b6579afb7866b1e715475224b";
        pc[24]=(char *)"245a394ad1eca9b72fc00ae7be315dc757b3b080d4c158013e6632d3c40659cc6cf90ad1c232a6442d9d3f5db980133";
        pc[25]=(char *)"b182cac101b9399d155096004f53f447aa7b12a3426b08ec02710e807b4633f06c851c1919211f20d4c04f00b971ef8";
        pc[26]=(char *)"18b46a908f36f6deb918c143fed2edcc523559b8aaf0c2462e6bfe7f911f643249d9cdf41b44d606ce07c8a4d0074d8e";
        pc[27]=(char *)"19713e47937cd1be0dfd0b8f1d43fb93cd2fcbcb6caf493fd1183e416389e61031bf3a5cce3fbafce813711ad011c132";
        pc[28]=(char *)"e1bba7a1186bdb5223abde7ada14a23c42a0ca7915af6fe06985e7ed1e4d43b9b3f7055dd4eba6f2bafaaebca731c30";
        pc[29]=(char *)"9fc4018bd96684be88c9e221e4da1bb8f3abd16679dc26c1e8b6e6a1f20cabe69d65201c78607a360370e577bdba587";
        pc[30]=(char *)"987c8d5333ab86fde9926bd2ca6c674170a05bfe3bdd81ffd038da6c26c842642f64550fedfe935a15e4ca31870fb29";
        pc[31]=(char *)"4ab0b9bcfac1bbcb2c977d027796b3ce75bb8ca2be184cb5231413c4d634f3747a87ac2460f415ec961f8855fe9d6f2";
        pc[32]=(char *)"16603fca40634b6a2211e11db8f0a6a074a7d0d4afadb7bd76505c3d3ad5544e203f6326c95a807299b23ab13633a5f0";
        pc[33]=(char *)"8cc03fdefe0ff135caf4fe2a21529c4195536fbe3ce50b879833fd221351adc2ee7f8dc099040a841b6daecf2e8fedb";
        pc[34]=(char *)"1f86376e8981c217898751ad8746757d42aa7b90eeb791c09e4a3ec03251cf9de405aba9ec61deca6355c77b0e5f4cb";
        pc[35]=(char *)"cc786baa966e66f4a384c86a3b49942552e2d658a31ce2c344be4b91400da7d26d521628b00523b8dfe240c72de1f6";
        pc[36]=(char *)"134996a104ee5811d51036d776fb46831223e96c254f383d0f906343eb67ad34d6c56711962fa8bfe097e75a2e41c696";
        pc[37]=(char *)"90d97c81ba24ee0259d1f094980dcfa11ad138e48a869522b52af6c956543d3cd0c7aee9b3ba3c2be9845719707bb33";

        pc[38]=(char *)"e0fa1d816ddc03e6b24255e0d7819c171c40f65e273b853324efcd6356caa205ca2f570f13497804415473a1d634b8f";
        pc[39]=(char *)"2660400eb2e4f3b628bdd0d53cd76f2bf565b94e72927c1cb748df27942480e420517bd8714cc80d1fadc1326ed06f7";        
        pc[40]=(char *)"ad6b9514c767fe3c3613144b45f1496543346d98adf02267d5ceef9a00d9b8693000763e3b90ac11e99b138573345cc";
        pc[41]=(char *)"accbb67481d033ff5852c1e48c50c477f94ff8aefce42d28c0f9a88cea7913516f968986f7ebbea9684b529e2561092";
        pc[42]=(char *)"4d2f259eea405bd48f010a01ad2911d9c6dd039bb61a6290e591b36e636a5c871a5c29f4f83060400f8b49cba8f6aa8";
        pc[43]=(char *)"167a55cda70a6e1cea820597d94a84903216f763e13d87bb5308592e7ea7d4fbc7385ea3d529b35e346ef48bb8913f55";
        pc[44]=(char *)"1866c8ed336c61231a1be54fd1d74cc4f9fb0ce4c6af5920abc5750c4bf39b4852cfe2f7bb9248836b233d9d55535d4a";
        pc[45]=(char *)"16a3ef08be3ea7ea03bcddfabba6ff6ee5a4375efa1f4fd7feb34fd206357132b920f5b00801dee460ee415a15812ed9";
        pc[46]=(char *)"166007c08a99db2fc3ba8734ace9824b5eecfdfa8d0cf8ef5dd365bc400a0051d5fa9c01a58b1fb93d1a1399126a775c";
        pc[47]=(char *)"8d9e5297186db2d9fb266eaac783182b70152c65550d881c5ecd87b6f0f5a6449f38db9dfa9cce202c6477faaf9b7ac";
        pc[48]=(char *)"be0e079545f43e4b00cc912f8228ddcc6d19c9f0f69bbb0542eda0fc9dec916a20b15dc0fd2ededda39142311a5001d";
        pc[49]=(char *)"16b7d288798e5395f20d23bf89edb4d1d115c5dbddbcd30e123da489e726af41727364f2c28297ada8d26d98445f5416";
        pc[50]=(char *)"58df3306640da276faaae7d6e8eb15778c4855551ae7f310c35a5dd279cd2eca6757cd636f96f891e2538b53dbf67f2";
        pc[51]=(char *)"1962d75c2381201e1a0cbd6c43c348b885c84ff731c4d59ca4a10356f453e01f78a4260763529e3532f6102c2e49a03d";
        pc[52]=(char *)"16112c4c3a9c98b252181140fad0eae9601a6de578980be6eec3232b5be72e7a07f3688ef60c206d01479253b03663c1";

        cout << "Hash to Curve Z for G2= " << findZ2(curve_b) << endl;
        // cout << "p%16= " << p%16 << endl;
    }


    if (strcmp(curvename, "BLS12461") == 0)
    {
        curve = PS+6;
        if (chunk==16)
            bb=13;
        if (chunk==32)
            bb=28;
        if (chunk==64)
            bb=60;
        if (strcmp(lg, "javascript") == 0)
            bb=23;
        printf("Curve is BLS12461\n");
        strcpy(fieldname, curvename);
        mbits = 461;
        words = (1 + ((mbits - 1) / bb));
        curvetype = WEIERSTRASS;
        modtype = NOT_SPECIAL;
        curve_a = 0;
        mip->IOBASE = 16;
        x = (char *)"1FFFFFFBFFFE00000000";            // SIGN_OF_X is NEGATIVE

        atebits = bits(3 * x) - 1;
        hw = hamming(x);

        p = (pow(x, 6) + 2 * pow(x, 5) - 2 * pow(x, 3) - x + 1) / 3;
        t = -x + 1;
        r = pow(x, 4) - x * x + 1;
        cof = (p + 1 - t) / r;

        cof = x + 1;

        gx = -2; gy = -1;
        curve_b = 9;
        ecurve((Big)0, curve_b, p, MR_AFFINE);
        mip->TWIST = MR_SEXTIC_M;

        P.set(gx, gy);
        P *= (x + 1) * (x + 1) / 3;
        P.get(gx, gy);

        while (!Q.set(randn2())) ;  // probably not best way to choose this

        TT = t * t - 2 * p;
        PP = p * p;
        FF = sqrt((4 * PP - TT * TT) / 3);
        np = PP + 1 - (-3 * FF + TT) / 2; // 2 possibilities...

        Q = (np / r) * Q;

        zcru = pow((ZZn)2, (p - 1) / 3);
        //zcru*=zcru;   // right cube root of unity
        cru = (Big)zcru;
        sqrtm3=  p+(-2 * pow(x, 5) - 6*pow(x,4) - 6*pow(x,3) + 2*x + 3);

        htpc=pow((Big)findZ(curve_a,curve_b,p),finde(p),p);

        cout << "Hash to Curve Z for G2= " << findZ2(curve_b) << endl;
    }

    if (strcmp(curvename, "BN462") == 0)
    {
        curve = PS+7;
        if (chunk==16)
            bb=13;
        if (chunk==32)
            bb=28;
        if (chunk==64)
            bb=60;
        if (strcmp(lg, "javascript") == 0)
            bb=23;
        printf("Curve is BN462\n");
        strcpy(fieldname, curvename);
        mbits = 462;
        words = (1 + ((mbits - 1) / bb));
        curvetype = WEIERSTRASS;
        modtype = NOT_SPECIAL;
        curve_a = 0;
        mip->IOBASE = 16;
        x = pow((Big)2, 114) + pow((Big)2, 101) - pow((Big)2, 14) - 1; // SIGN_OF_X is POSITIVE

        //cout << "x= " << x << endl;

        atebits = bits(3 * (6 * x + 2)) - 1;
        hw = hamming(6 * x + 2) + 2;

        p = 36 * pow(x, 4) + 36 * pow(x, 3) + 24 * x * x + 6 * x + 1;
        t = 6 * x * x + 1;
        r = p + 1 - t;
        cof = 1;

        curve_b = 5;
        ecurve((Big)0, curve_b, p, MR_AFFINE);
        mip->TWIST = MR_SEXTIC_D;
        mip->pmod8 = 7; // to fool it into using 2+sqrt(-1)!


        gx = (char *)"21a6d67ef250191fadba34a0a30160b9ac9264b6f95f63b3edbec3cf4b2e689db1bbb4e69a416a0b1e79239c0372e5cd70113c98d91f36b6980d";
        gy = (char *)"0118ea0460f7f7abb82b33676a7432a490eeda842cccfa7d788c659650426e6af77df11b8ae40eb80f475432c66600622ecaa8a5734d36fb03de";
        P.set(gx, gy);

        Big xd = (char *)"041b04cbe3413297c49d81297eed075947d86135c4abf0be9d5b64be02d6ae7834047ea4079cd30fe28a68ba0cb8f7b72836437dc75b2567ff2b98dbb93f68fac828d8221e4e1d89475e2d85f2063cbc4a74f6f66268b6e6da1162ee055365bb30283bde614a17f61a255d6882417164bc500498";
        Big yd = (char *)"0104fa796cbc29890f9a37982c353da13b299391be45ddb1c15ca42abdf8bf502a5dd7ac0a3d351a859980e89be676d00e92c128714d6f3c6aba56ca6e0fc6a5468c12d42762b29d840f13ce5c3323ff016233ec7d76d4a812e25bbeb2c250243f2cbd2780527ec5ad208d7224334db3c1b4a49c";

        Big xda = xd % p;
        Big xdb = xd / p;

        Big yda = yd % p;
        Big ydb = yd / p;

        ZZn2 QQx(xda, xdb);
        ZZn2 QQy(yda, ydb);

        //cout << "QQx= " << QQx << endl;
        //cout << "QQy= " << QQy << endl;

        Q.set(QQx, QQy);

        zcru = pow((ZZn)2, (p - 1) / 3);
        zcru *= zcru; // right cube root of unity
        cru = (Big)zcru;

        cru = p - (18 * pow(x, 3) + 18 * x * x + 9 * x + 2);

        sqrtm3=p+(-36*pow(x,3) - 36*x*x - 18*x - 3);

        htpc=pow((Big)findZ(curve_a,curve_b,p),finde(p),p);
        cout << "Hash to Curve Z for G2= " << findZ2(curve_b) << endl;
    }

    if (strcmp(curvename, "FP256BN") == 0)
    {
        curve = PS+4;
        if (chunk==16)
            bb=13;
        if (chunk==32)
            bb=28;
        if (chunk==64)
            bb=56;
        if (strcmp(lg, "javascript") == 0)
            bb=24;
        printf("Curve is FP256BN\n");
        strcpy(fieldname, curvename);
        mbits = 256;
        words = (1 + ((mbits - 1) / bb));
        curvetype = WEIERSTRASS;
        modtype = NOT_SPECIAL;
        curve_a = 0;
        mip->IOBASE = 16;
        x = (char *)"6882F5C030B0A801";  // Slow!

        atebits = bits(3 * (6 * x - 2)) - 1;
        hw = hamming(6 * x - 2) + 2;

        p = 36 * pow(x, 4) - 36 * pow(x, 3) + 24 * x * x - 6 * x + 1;
//mip->IOBASE=10;
//cout << "p= " << p << endl;
//mip->IOBASE=16;
        t = 6 * x * x + 1;
        r = p + 1 - t;
        curve_b = 3;
        gx = 1;
        gy = 2;
        cof = 1;
        ecurve((Big)0, curve_b, p, MR_AFFINE);
        mip->TWIST = MR_SEXTIC_M;


        Q.set((ZZn2)1);
        //while (!Q.set(randn2())) ;
        Q = (p - 1 + t) * Q;

        cru = (18 * pow(x, 3) - 18 * x * x + 9 * x - 2);

        sqrtm3=p-(36*pow(x,3) - 36*x*x + 18*x - 3);

        htpc=pow((Big)findZ(curve_a,curve_b,p),finde(p),p);
        cout << "Hash to Curve Z for G2= " << findZ2(curve_b) << endl;
    }

    if (strcmp(curvename, "FP512BN") == 0)
    {
        curve = PS+5;
        if (chunk==16)
            bb=13;
        if (chunk==32)
            bb=29;
        if (chunk==64)
            bb=60;
        if (strcmp(lg, "javascript") == 0)
            bb=23;
        printf("Curve is FP512BN\n");
        strcpy(fieldname, curvename);
        mbits = 512;
        words = (1 + ((mbits - 1) / bb));
        curvetype = WEIERSTRASS;
        modtype = NOT_SPECIAL;
        curve_a = 0;
        mip->IOBASE = 16;
        x = (char *)"6882F5C030B0F7F010B306BB5E1BD80F";  // Slow!

        atebits = bits(3 * (6 * x + 2)) - 1;
        hw = hamming(6 * x + 2) + 2;

        p = 36 * pow(x, 4) + 36 * pow(x, 3) + 24 * x * x + 6 * x + 1;

//mip->IOBASE=10;
//cout << "p= " << p << endl;
//mip->IOBASE=16;

        t = 6 * x * x + 1;
        r = p + 1 - t;
        curve_b = 3;
        gx = 1;
        gy = 2;
        cof = 1;
        ecurve((Big)0, curve_b, p, MR_AFFINE);
        mip->TWIST = MR_SEXTIC_M;

        Q.set((ZZn2)1);
        //while (!Q.set(randn2())) ;
        Q = (p - 1 + t) * Q;

        cru = p - (18 * pow(x, 3) + 18 * x * x + 9 * x + 2);

        sqrtm3=p - (36*pow(x,3) + 36*x*x + 18*x + 3);

        htpc=pow((Big)findZ(curve_a,curve_b,p),finde(p),p);
        cout << "Hash to Curve Z for G2= " << findZ2(curve_b) << endl;
    }

    if (strcmp(curvename, "BLS24479") == 0)
    {
        curve = PS+8;
        if (chunk==16)
            bb=13;
        if (chunk==32)
            bb=29;
        if (chunk==64)
            bb=56;
        if (strcmp(lg, "javascript") == 0)
            bb=23;
        printf("Curve is BLS24479\n");
        strcpy(fieldname, curvename);

        mbits = 479;

        words = (1 + ((mbits - 1) / bb));
        curvetype = WEIERSTRASS;
        modtype = NOT_SPECIAL;
        curve_a = 0;
        mip->IOBASE = 16;

// Note - this is GT-Strong curve

        x = (char *)"100020011FF80";				// SIGN_OF_X is POSITIVE

        atebits = bits(3 * x) - 1;
        hw = hamming(x);

        p = (1 + x + x * x - pow(x, 4) + 2 * pow(x, 5) - pow(x, 6) + pow(x, 8) - 2 * pow(x, 9) + pow(x, 10)) / 3;
        t = x + 1;
        r = pow(x, 8) - pow(x, 4) + 1;
        cof = (p + 1 - t) / r;

        cof = x - 1;
        //x=-x;

        gx = 5; gy = 12;
        curve_b = 19;
        ecurve((Big)0, curve_b, p, MR_AFFINE);

        mip->TWIST = MR_SEXTIC_M;

        P.set(gx, gy);
        P *= (x - 1) * (x - 1) / 3;

        P.get(gx, gy);

        Big x0 = 0;

        forever
        {
            ZZn4 X;
            ZZn2 t;

            x0 += 1;
            t.set((ZZn)0, (ZZn)x0);
            X.set(t, (ZZn2)0);
            if (!QQ.set(X)) continue;
            break;
        }

        TT = t * t * t * t - 4 * p * t * t + 2 * p * p;
        PP = pow(p, 4);
        FF = sqrt((4 * PP - TT * TT) / 3);
        np = PP + 1 - (3 * FF + TT) / 2;

        QQ = (np / r) * QQ;

//cout << "QQ= " << QQ << endl;
//cout << "2*QQ= " << QQ+QQ << endl;
//cout << "3*QQ= " << (QQ+QQ)+QQ << endl;

        zcru = pow((ZZn)2, (p - 1) / 3);
        //zcru*=zcru;   // right cube root of unity -  not for M-TYPE
        cru = (Big)zcru;

        zsqrtm3=sqrt((ZZn)-3);
        sqrtm3=(Big)zsqrtm3;
        if (sqrtm3%2==1)
            sqrtm3=p-sqrtm3;

        htpc=pow((Big)findZ(curve_a,curve_b,p),finde(p),p);
        cout << "Hash to Curve Z for G2= " << findZ4(curve_b) << endl;

        twk=(Big)((ZZn)1/pow((ZZn)2,(p-3)/4));  // tweak for square root, norm=2

    //ZZn ttt=2;
    //cout << "ttt= " << (ZZn)1/pow(ttt,(p-3)/4) << endl;
    //cout << "ttt= " << pow(ttt,(p-3)/4) << endl;
    
    }


    if (strcmp(curvename, "BLS48556") == 0)
    {
        curve = PS+9;
        if (chunk==16)
            bb=13;
        if (chunk==32)
            bb=29;
        if (chunk==64)
            bb=58;
        if (strcmp(lg, "javascript") == 0)
            bb=23;
        printf("Curve is BLS48556\n");
        strcpy(fieldname, curvename);

        mbits = 556;

        words = (1 + ((mbits - 1) / bb));
        curvetype = WEIERSTRASS;
        modtype = NOT_SPECIAL;
        curve_a = 0;
        mip->IOBASE = 16;

// Note - this is GT-Strong curve

        x = (char *)"7DE40020";				// SIGN_OF_X is POSITIVE

        atebits = bits(3 * x) - 1;
        hw = hamming(x);

        r = pow(x, 16) - pow(x, 8) + 1;
        p = r * (((x - 1) * (x - 1)) / 3) + x;
        t = x + 1;
        cof = (p + 1 - t) / r;

        cof = x - 1;

// p=19 mod 24

        gx = 2; gy = 5;
        curve_b = 17;
        ecurve((Big)0, curve_b, p, MR_AFFINE);

        mip->TWIST = MR_SEXTIC_M;

        P.set(gx, gy);
        P *= (x - 1) * (x - 1) / 3;

        P.get(gx, gy);

        Big x0 = 0;

        forever
        {
            ZZn8 XX;
            ZZn4 X;
            ZZn2 t;

            x0 += 1;
            t.set((ZZn)0, (ZZn)x0);
            X.set(t, (ZZn2)0);
            XX.set(X, (ZZn4)0);
            if (!Q8.set(X)) continue;
            break;
        }


        tau[0] = 2; // count points on twist over extension p^8
        tau[1] = t;
        for (int jj = 1; jj < 8; jj++ ) tau[jj + 1] = t * tau[jj] - p * tau[jj - 1];

        TT = tau[8];

        PP = pow(p, 8);
        FF = sqrt((4 * PP - TT * TT) / 3);
        np = PP + 1 - (3 * FF + TT) / 2; //?

        Q8 = (np / r) * Q8;

        zcru = pow((ZZn)2, (p - 1) / 3);
        //zcru*=zcru;   // right cube root of unity -  not for M-TYPE
        cru = (Big)zcru;

        zsqrtm3=sqrt((ZZn)-3);
        sqrtm3=(Big)zsqrtm3;
        if (sqrtm3%2==1)
            sqrtm3=p-sqrtm3;
        htpc=pow((Big)findZ(curve_a,curve_b,p),finde(p),p);
        cout << "Hash to Curve Z for G2= " << findZ8(curve_b) << endl;

        twk=(Big)((ZZn)1/pow((ZZn)2,(p-3)/4));  // tweak for square root, norm=2

    }

    if (strcmp(curvename, "BLS48581") == 0)
    {
        curve = PS+10;
        if (chunk==16)
            bb=13;
        if (chunk==32)
            bb=29;
        if (chunk==64)
            bb=60;
        if (strcmp(lg, "javascript") == 0)
            bb=23;
        printf("Curve is BLS48581\n");
        strcpy(fieldname, curvename);

        mbits = 581;

        words = (1 + ((mbits - 1) / bb));
        curvetype = WEIERSTRASS;
        modtype = NOT_SPECIAL;
        curve_a = 0;
        mip->IOBASE = 16;

        x = (char *)"140000381";				// SIGN_OF_X is NEGATIVE

        atebits = bits(3 * x) - 1;
        hw = hamming(x);

        r = pow(x, 16) - pow(x, 8) + 1;
        p = r * (((x + 1) * (x + 1)) / 3) - x;
        t = -x + 1;
        cof = (p + 1 - t) / r;

        cof = x + 1;


// p=19 mod 24

        curve_b = 1;
        ecurve((Big)0, curve_b, p, MR_AFFINE);

        mip->TWIST = MR_SEXTIC_D;


        gx = (char *)"2af59b7ac340f2baf2b73df1e93f860de3f257e0e86868cf61abdbaedffb9f7544550546a9df6f9645847665d859236ebdbc57db368b11786cb74da5d3a1e6d8c3bce8732315af640";
        gy = (char *)"cefda44f6531f91f86b3a2d1fb398a488a553c9efeb8a52e991279dd41b720ef7bb7beffb98aee53e80f678584c3ef22f487f77c2876d1b2e35f37aef7b926b576dbb5de3e2587a70";
        P.set(gx, gy);


        Big A, B, C, D, E, F, G, H;

        mip->IOBASE = 16;

        Big xd = (char *)"01690ae06061530e3164040ce6e7466974a0865edb6d5b825df11e5db6b724681c2b5a805af2c7c45f60300c3c4238a1f5f6d3b64429f5b655a4709a8bddf790ec477b5fb1ed4a0156dec43f7f6c401164da6b6f9af79b9fc2c0e09d2cd4b65900d2394b61aa3bb48c7c731a1468de0a17346e34e17d58d8707f845face35202bb9d64b5eff29cbfc85f5c6d601d794c8796c20e6781dffed336fc1ff6d3ae3193dec0060391acb6811f1fbde38027a0ef591e6b21c6e31c5f1fda66eb05582b6b0399c6a2459cb2abfd0d5d953447a92786e194b289588e63ef1b8b61ad354bed299b5a497c549d7a56a74879b7665a7042fbcaf1190d915f945fef6c0fcec14b4afc403f507747204d810c5700de16926309352f660f26a5529a2f74cb9d10440595dc25d6d12fcce84fc56557217bd4bc2d645ab4ca167fb812de7cacc3b9427fc78212985680b883bf7fee7eae01991eb7a52a0f4cbb01f5a8e3c16c41350dc62be2c19cbd2b98d9c9d2687cd811db7863779c97e9a15bd6967d5eb21f972d28ad9d437de412342524931998f280a9a9c799c33ff8f838ca35bddebbb79cdc2967946cc0f77995411692e18519243d5598bdb4623a11dc97ca388949f32c65db3fc6a47124bd5d063549e50b0f8b030d3a9830e1e3bef5cd4283939d33a28cfdc3df89640df257c0fc254477a9c8eff69b57cff042e6fd1ef3e293c57beca2cd61dc44838014c208eda095e10d5e89e705ff690704789596e419699650879771f58935d768cdc3b55150cca3693e2833b62df34f1e2491ef8c5824f8a80cd86e65193a";
        Big yd = (char *)"00951682f010b08932b28b4a851ec79469f9437fc4f9cfa8ccdec25c3cc847890c65e1bcd2df994b835b71e49c0fc69e6d9ea5da9dbb020a9dfb2942dd022fa962fb0233de016c8c80e9387b0b28786785523e68eb7c008f81b99ee3b5d10a72e5321a09b74b39c58b75d09d73e4155b76dc11d8dd416b7fa63557fcddb0a955f6f5e0028d4af2150bfd757a898b548912e2c0c6e570449113fcee54cda9cb8bfd7f182825b371f06961b62ca441bfcb3d13ce6840432bf8bc4736003c64d695e984ddc2ef4aee1747044157fd2f9b81c43eed97d3452898996d24c66aad191dba634f3e04c89485e06f8308b8afaedf1c98b1a466deab2c1581f96b6f3c64d440f2a16a6275000cf38c09453b5b9dc8278eabe44292a154dc69faa74ad76ca847b786eb2fd686b9be509fe24b8293861cc35d76be88c2711704bfe118e4db1fad86c2a6424da6b3e5807258a2d166d3e0e43d15e3b6464fb99f382f57fd10499f8c8e11df718c98a049bd0e5d1301bc9e6ccd0f063b06eb06422afa469b5b529b8bba3d4c6f219affe4c57d7310a92119c98884c3b6c0bbcc113f6826b3ae70e3bbbaadab3ff8abf3b905c23138dfe385134807fcc1f9c19e68c0ec468213bc9f0387ca1f4ffe406fda92d6553cd4cfd50a2c895e85fe25409ffe8bb43b458f9befab4d59bee20e2f01de48c2affb03a97ceede87214e3bb90183303b672e50b87b36a61534034578db0195fd81a46beb55f75d20049d044c3fa5c3678c783db3120c2580359a7b33cac5ce21e4cecda9e2e2d6d2ff202ff43c1bb2d4b5e53dae010423ce";
        //mip->IOBASE=10;

        A = xd % p; xd /= p;
        B = xd % p; xd /= p;
        C = xd % p; xd /= p;
        D = xd % p; xd /= p;
        E = xd % p; xd /= p;
        F = xd % p; xd /= p;
        G = xd % p; xd /= p;
        H = xd;

        ZZn2 xaa(A, B);
        ZZn2 xab(C, D);
        ZZn2 xba(E, F);
        ZZn2 xbb(G, H);

        ZZn4 xa(xaa, xab);
        ZZn4 xb(xba, xbb);

        X8.set(xa, xb);

        A = yd % p; yd /= p;
        B = yd % p; yd /= p;
        C = yd % p; yd /= p;
        D = yd % p; yd /= p;
        E = yd % p; yd /= p;
        F = yd % p; yd /= p;
        G = yd % p; yd /= p;
        H = yd;

        ZZn2 yaa(A, B);
        ZZn2 yab(C, D);
        ZZn2 yba(E, F);
        ZZn2 ybb(G, H);

        ZZn4 ya(yaa, yab);
        ZZn4 yb(yba, ybb);

        Y8.set(ya, yb);


        zcru = pow((ZZn)2, (p - 1) / 3);
        zcru *= zcru; // right cube root of unity -  not for M-TYPE
        cru = (Big)zcru;

        zsqrtm3=sqrt((ZZn)-3);
        sqrtm3=(Big)zsqrtm3;
        if (sqrtm3%2==1)
            sqrtm3=p-sqrtm3;
        htpc=pow((Big)findZ(curve_a,curve_b,p),finde(p),p);
        cout << "Hash to Curve Z for G2= " << findZ8(curve_b) << endl;

        twk=(Big)((ZZn)1/pow((ZZn)2,(p-3)/4));  // tweak for square root, norm=2
    }


    if (strcmp(curvename, "BLS48286") == 0)
    {
        curve = PS+11;
        if (chunk==16)
            bb=13;
        if (chunk==32)
            bb=29;
        if (chunk==64)
            bb=60;
        if (strcmp(lg, "javascript") == 0)
            bb=23;
        printf("Curve is BLS48286\n");
        strcpy(fieldname, curvename);

        mbits = 286;

        words = (1 + ((mbits - 1) / bb));
        curvetype = WEIERSTRASS;
        modtype = NOT_SPECIAL;
        curve_a = 0;
        mip->IOBASE = 16;

        x = (char *)"F5EF";				// SIGN_OF_X is POSITIVE

        atebits = bits(3 * x) - 1;
        hw = hamming(x);

        r = pow(x, 16) - pow(x, 8) + 1;
        p = r * (((x - 1) * (x - 1)) / 3) + x;
        t = x + 1;
        cof = (p + 1 - t) / r;

        cof = x - 1;

        gx = -1; gy = 3;
        curve_b = 10;
        ecurve((Big)0, curve_b, p, MR_AFFINE);

        mip->TWIST = MR_SEXTIC_M;

        P.set(gx, gy);

        P *= (x - 1) * (x - 1) / 3;

        P.get(gx, gy);

        Big x0 = 0;

        forever
        {
            ZZn8 XX;
            ZZn4 X;
            ZZn2 t;

            x0 += 1;
            t.set((ZZn)0, (ZZn)x0);
            X.set(t, (ZZn2)0);
            XX.set(X, (ZZn4)0);
            if (!Q8.set(X)) continue;
            break;
        }


        tau[0] = 2; // count points on twist over extension p^8
        tau[1] = t;
        for (int jj = 1; jj < 8; jj++ ) tau[jj + 1] = t * tau[jj] - p * tau[jj - 1];

        TT = tau[8];

        PP = pow(p, 8);
        FF = sqrt((4 * PP - TT * TT) / 3);
        np = PP + 1 - (3 * FF + TT) / 2; //?

        Q8 = (np / r) * Q8;

        zcru = pow((ZZn)2, (p - 1) / 3);
        //zcru*=zcru;   // right cube root of unity -  not for M-TYPE
        cru = (Big)zcru;

        zsqrtm3=sqrt((ZZn)-3);
        sqrtm3=(Big)zsqrtm3;
        if (sqrtm3%2==1)
            sqrtm3=p-sqrtm3;
        htpc=pow((Big)findZ(curve_a,curve_b,p),finde(p),p);
        cout << "Hash to Curve Z for G2= " << findZ8(curve_b) << endl;

        twk=(Big)((ZZn)1/pow((ZZn)2,(p-3)/4));  // tweak for square root, norm=2
    }

    if (curve == 0) {help(); return 0;}

    if (curvetype == WEIERSTRASS) printf("Hash to Curve Z= %d\n",findZ(curve_a,curve_b,p));
    if (curvetype == EDWARDS) printf("Hash to Curve Z= %d\n",findR(curve_a,curve_b,p));

    bytes = mbits / 8;
    if (mbits % 8 != 0) bytes++;
    bytes *= 8;


    lang = 0;
    if (strcmp(lg, "c") == 0)
    {
        open = '{';
        close = '}';
        term = ';';
        lang = 1;
        if (chunk == 64) el = 'L';

        sprintf(xxx, "_%d_%d ", bytes, bb);

        sprintf(pre0, "const int ");
        sprintf(pre1, "const BIG%s", xxx);
        sprintf(pre2, "const chunk ");
        sprintf(pre3, "const BIG%s", xxx);
        sprintf(pre4, "const BIG%s", xxx);
        sprintf(pre5, "const BIG%s", xxx);
        sprintf(pre6, "const BIG%s", xxx);
        sprintf(pre7, "const BIG%s", xxx);
        sprintf(pre8, "const BIG%s", xxx);

        sprintf(zzz, "_%s", curvename);
        sprintf(yyy, "_%s", fieldname);

        sprintf(post0, "%s= ", zzz);
        sprintf(post1, "%s= ", zzz);
        sprintf(post2, "%s= ", yyy);
        sprintf(post3, "%s[2]= ", zzz);
        sprintf(post4, "%s[2][2]= ", zzz);
        sprintf(post5, "%s[4]= ", zzz);
        sprintf(post6, "%s[4][4]= ", zzz);
        sprintf(post7, "%s= ", yyy);
        sprintf(post8, "%s[%d]= ", zzz, ncs);
        sprintf(post9, "%s[%d]= ", zzz, ncs2);

    }

    if (strcmp(lg, "cpp") == 0)
    {
        open = '{';
        close = '}';
        term = ';';
        lang = 7;
        if (chunk == 64) el = 'L';

        strcpy(pre0, "const int ");
        strcpy(pre1, "const BIG ");
        strcpy(pre2, "const chunk ");
        strcpy(pre3, "const BIG ");
        strcpy(pre4, "const BIG ");
        strcpy(pre5, "const BIG ");
        strcpy(pre6, "const BIG ");
        strcpy(pre7, "const BIG ");
        strcpy(pre8, "const BIG ");

        strcpy(post0, "= ");
        strcpy(post1, "= ");
        strcpy(post2, "= ");
        strcpy(post3, "[2]= ");
        strcpy(post4, "[2][2]= ");
        strcpy(post5, "[4]= ");
        strcpy(post6, "[4][4]= ");
        strcpy(post7, "= ");
        sprintf(post8, "[%d]= ",ncs);
        sprintf(post9, "[%d]= ",ncs2);
    }

    if (strcmp(lg, "java") == 0)
    {
        open = '{';
        close = '}';
        term = ';';
        lang = 2;
        if (chunk == 64)
        {
            el = 'L';
            strcpy(pre0, "public static final int ");
            strcpy(pre1, "public static final long[] ");
            strcpy(pre2, "public static final long ");
            strcpy(pre3, "public static final long[][] ");
            strcpy(pre4, "public static final long[][][] ");
            strcpy(pre5, "public static final long[][] ");
            strcpy(pre6, "public static final long[][][] ");
            strcpy(pre7, "public static final long[] ");
            strcpy(pre8, "public static final long[][] ");

        }
        else
        {
            strcpy(pre0, "public static final int ");
            strcpy(pre1, "public static final int[] ");
            strcpy(pre2, "public static final int ");
            strcpy(pre3, "public static final int[][] ");
            strcpy(pre4, "public static final int[][][] ");
            strcpy(pre5, "public static final int[][] ");
            strcpy(pre6, "public static final int[][][] ");
            strcpy(pre7, "public static final int[] ");
            strcpy(pre8, "public static final int[][] ");

        }
        strcpy(post0, "= ");
        strcpy(post1, "= ");
        strcpy(post2, "= ");
        strcpy(post3, "= ");
        strcpy(post4, "= ");
        strcpy(post5, "= ");
        strcpy(post6, "= ");
        strcpy(post7, "= ");
        strcpy(post8, "= ");
        strcpy(post9, "= ");
    }

    if (strcmp(lg, "javascript") == 0)
    {
        open = '[';
        close = ']';
        term = ',';
        lang = 3;
        strcpy(pre0, "");
        strcpy(pre1, "");
        strcpy(pre2, "");
        strcpy(pre3, "");
        strcpy(pre4, "");
        strcpy(pre5, "");
        strcpy(pre6, "");
        strcpy(pre7, "");
        strcpy(pre8, "");
        strcpy(post0, ": ");
        strcpy(post1, ": ");
        strcpy(post2, ": ");
        strcpy(post3, ": ");
        strcpy(post4, ": ");
        strcpy(post5, ": ");
        strcpy(post6, ": ");
        strcpy(post7, ": ");
        strcpy(post8, ": ");
        strcpy(post9, ": ");
    }

    if (strcmp(lg, "go") == 0)
    {
        open = '{';
        close = '}';
        term = ' ';
        lang = 4;
        strcpy(pre0, "const ");
        strcpy(pre1, "var ");
        strcpy(pre2, "const ");
        strcpy(pre3, "var ");
        strcpy(pre4, "var ");
        strcpy(pre5, "var ");
        strcpy(pre6, "var ");
        strcpy(pre7, "var ");
        strcpy(pre8, "var ");
        strcpy(post0, " int= ");
        strcpy(post1, "= [...]Chunk ");
        strcpy(post2, " Chunk=");
        sprintf(post3, "=[2][%d]Chunk ", words);
        sprintf(post4, "=[2][2][%d]Chunk ", words);
        sprintf(post5, "=[4][%d]Chunk ", words);
        sprintf(post6, "=[4][4][%d]Chunk ", words);
        strcpy(post7, "= [...]Chunk ");
        sprintf(post8, "=[%d][%d]Chunk ", ncs, words);
        sprintf(post9, "=[%d][%d]Chunk ", ncs2, words);
    }

    if (strcmp(lg, "rust") == 0)
    {
        open = '[';
        close = ']';
        term = ';';
        lang = 5;
        strcpy(pre0, "pub const ");
        strcpy(pre1, "pub const ");
        strcpy(pre2, "pub const ");
        strcpy(pre3, "pub const ");
        strcpy(pre4, "pub const ");
        strcpy(pre5, "pub const ");
        strcpy(pre6, "pub const ");
        strcpy(pre7, "pub const ");
        strcpy(pre8, "pub const ");
        strcpy(post0, ":isize = ");
        strcpy(post1, ":[Chunk;NLEN]=");
        strcpy(post2, ":Chunk=");
        strcpy(post3, ":[[Chunk;NLEN];2]=");
        strcpy(post4, ":[[[Chunk;NLEN];2];2]=");
        strcpy(post5, ":[[Chunk;NLEN];4]=");
        strcpy(post6, ":[[[Chunk;NLEN];4];4]=");
        strcpy(post7, ":[Chunk;NLEN]=");
        sprintf(post8, ":[[Chunk;NLEN];%d]=",ncs);
        sprintf(post9, ":[[Chunk;NLEN];%d]=",ncs2);

    }

    if (strcmp(lg, "swift") == 0)
    {
        open = '[';
        close = ']';
        term = ' ';
        lang = 6;
        strcpy(pre0, "static let ");
        strcpy(pre1, "static public let ");
        strcpy(pre2, "static let ");
        strcpy(pre3, "static let ");
        strcpy(pre4, "static let ");
        strcpy(pre5, "static let ");
        strcpy(pre6, "static let ");
        strcpy(pre7, "static let ");
        strcpy(pre8, "static let ");
        strcpy(post0, ":Int = ");
        strcpy(post1, ":[Chunk] = ");
        strcpy(post2, ":Chunk = ");
        strcpy(post3, ":[[Chunk]] = ");
        strcpy(post4, ":[[[Chunk]]] = ");
        strcpy(post5, ":[[Chunk]] = ");
        strcpy(post6, ":[[[Chunk]]] = ");
        strcpy(post7, ":[Chunk] = ");
        strcpy(post8, ":[[Chunk]] = ");
        strcpy(post9, ":[[Chunk]] = ");
    }


    if (lang == 0) {help(); return 0;}

    //cout << "\nMOD8 = " << p%8 << endl;

    m = pow((Big)2, bb);

//    cout << "p mod 3= " << p%3 << endl;
    cout << "//*** rom field parameters *****" << endl;
    cout << "// Chunk bits= " << chunk << endl;
    cout << "// Base  bits= " << bb << endl;

    cout << pre7 << toupperit((char *)"Modulus", lang) << post7; mc = output(chunk, words, p, m); cout << term << endl;
    r2modp = pow((Big)2, 2 * words * bb) % p;

    e=0;
    Big is=p-1;
    while (is%2==0)
    {
        is/=2;
        e++;
    }
    if (e==1) roi=p-1;
    if (e==2) roi=pow((Big)2,(p-1)/4,p);
    if (e>2) 
    {
        int qnr=2;
        while (pow(qnr,(p-1)/2,p)==1)
        {
            qnr+=1;
        }
        roi=pow((Big)qnr,(p-1)/pow((Big)2,e),p);
        //cout << "qnr= " << qnr << endl;
    }

//cout << "\ngx*R2modp= " << r2modp*gx << endl;
//cout << "mod p = " << redc(r2modp*gx) << endl << endl;

    cout << pre7 << toupperit((char *)"R2modp", lang) << post7; output(chunk, words, r2modp, m); cout << term << endl;
    cout << pre7 << toupperit((char *)"ROI", lang) << post7; output(chunk, words, roi, m); cout << term << endl;
    if (sqrtm3!=0)
    {
        cout << pre7 << toupperit((char *)"SQRTm3", lang) << post7; output(chunk, words, sqrtm3, m); cout << term << endl;
    }  
    if (cru!=0)
    {
        cout << pre1 << toupperit((char *)"CRu", lang) << post1; output(chunk, words, cru, m); cout << term << endl;
    }

    if (modtype == NOT_SPECIAL)
        cout << pre2 << toupperit((char *)"MConst", lang)  << post2 << "0x" << inverse(m - p % m, m);

    if (modtype == MONTGOMERY_FRIENDLY)
        cout << pre2 << toupperit((char *)"MConst", lang) << post2 << "0x" << mc + 1;

    if (modtype == PSEUDO_MERSENNE)
        cout << pre2 << toupperit((char *)"MConst", lang)  << post2 << "0x" << pow((Big)2, mbits) - p;

    if (modtype == GENERALISED_MERSENNE)
        cout << pre2 << toupperit((char *)"MConst", lang)  << post2 << "0x1";

    if (el != 0) cout << "L;" << endl;
    else cout << term << endl;


    if (curve >= PS)
    {   // Frobenius constants -  depend on embedding degree
        if (curve < PS+8)
            set_frobenius_constant(X, 12);
        else
        {
            if (curve == PS+8) set_frobenius_constant(X, 24);
            if (curve == PS+9 || curve == PS+10 || curve == PS+11) set_frobenius_constant(X, 48);
            if (curve == PS+10) X = -X;
            cout << pre1 << toupperit((char *)"TWK", lang) << post7; output(chunk, words, twk, m); cout << term << endl;
        }



        X.get(a, b);
        cout << pre1 << toupperit((char *)"Fra", lang) << post7; output(chunk, words, a, m); cout << term << endl;
        cout << pre1 << toupperit((char *)"Frb", lang) << post7; output(chunk, words, b, m); cout << term << endl;

    }

    cout << endl;

    //cout << "X= " << X << endl;
    //cout << "Gx= " << gx << endl;
    //cout << "Gy= " << gy << endl;
    //cout << "Q= " << Q << endl;

    cout << "//*** rom curve parameters *****" << endl;
//    cout << "// Base Bits= " << bb << endl;
    if (curve >= PS)
    {
        cout << "// Ate Bits= " << atebits << endl;
        cout << "// G2 Table size= " << atebits + hw - 1 << endl;
    }
//    cout << "\n" << pre0 << "CURVE_A"  << post0 << curve_a << term << endl;

    curve_b_i = toint(curve_b);
    if (curve_b_i == MR_TOOBIG)
    {
        curve_b_i = toint(p - curve_b);
        if (curve_b_i == MR_TOOBIG)
            curve_b_i = 0;
        else curve_b_i = -curve_b_i;
    }

    cof_i = toint(cof);
    if (cof_i == MR_TOOBIG)
        cof_i = 0;


    cout << pre0 << toupperit((char *)"CURVE_Cof_I", lang) << post0 << cof_i << term << endl;
    cout << pre1 << toupperit((char *)"CURVE_Cof", lang) << post1; output(chunk, words, cof, m); cout << term << endl;
    cout << pre0 << "CURVE_B_I" << post0 << curve_b_i << term << endl;
    cout << pre1 << "CURVE_B" << post1; output(chunk, words, curve_b, m); cout << term << endl;
    cout << pre1 << toupperit((char *)"CURVE_Order", lang) << post1; output(chunk, words, r, m); cout << term << endl;
    cout << pre1 << toupperit((char *)"CURVE_Gx", lang) << post1; output(chunk, words, gx, m); cout << term << endl;
    cout << pre1 << toupperit((char *)"CURVE_Gy", lang) << post1; output(chunk, words, gy, m); cout << term << endl;
    cout << pre1 << toupperit((char *)"CURVE_HTPC", lang) << post1; output(chunk, words, htpc, m); cout << term << endl; // A Hash To Point Constant -  differs depending on HTP method.


    if (curve == 17)   // secp256k1
    {
        cout << pre1 << toupperit((char *)"CURVE_Ad", lang) << post1; output(chunk, words, ad, m); cout << term << endl;
        cout << pre1 << toupperit((char *)"CURVE_Bd", lang) << post1; output(chunk, words, bd, m); cout << term << endl;
        cout << pre8 << "PC" << post8 << open;
        for (i=0;i<ncs-1;i++)
        {
            output(chunk, words, pc[i], m); cout << ",";
        }
        output(chunk, words, pc[ncs-1], m);  
        cout << close << term << endl;
    }

// BN curves, negative x
    if (curve == PS || curve == PS+1 || curve == PS+4)
    {
        cout << endl;

        cout << pre1 << toupperit((char *)"CURVE_Bnx", lang) << post1; output(chunk, words, x, m); cout << term << endl;

//cout << "Q= " << Q << endl;
        Q.get(Xa, Ya);
        Xa.get(a, b);
        cout << pre1 << toupperit((char *)"CURVE_Pxa", lang) << post1; output(chunk, words, a, m); cout << term << endl;
        cout << pre1 << toupperit((char *)"CURVE_Pxb", lang) << post1; output(chunk, words, b, m); cout << term << endl;
        Ya.get(a, b);
        cout << pre1 << toupperit((char *)"CURVE_Pya", lang) << post1; output(chunk, words, a, m); cout << term << endl;
        cout << pre1 << toupperit((char *)"CURVE_Pyb", lang) << post1; output(chunk, words, b, m); cout << term << endl;

        cout << pre3 << "CURVE_W"  << post3 << open; output(chunk, words, 6 * x * x - 4 * x + 1, m); cout << ","; output(chunk, words, (2 * x - 1), m); cout << close << term << endl;
        cout << pre4 << "CURVE_SB" << post4 << open; cout << open; output(chunk, words, 6 * x * x - 2 * x, m); cout << ","; output(chunk, words, (2 * x - 1), m); cout << close; cout << ","; cout << open; output(chunk, words, (2 * x - 1), m); cout << ","; output(chunk, words, r - (6 * x * x - 4 * x + 1), m); cout << close; cout << close << term  << endl;

        cout << pre5 << "CURVE_WB" << post5 << open; output(chunk, words, 2 * x * x - 3 * x + 1, m); cout << ","; output(chunk, words, 12 * x * x * x - 8 * x * x + x, m);
        cout << ","; output(chunk, words, 6 * x * x * x - 4 * x * x + x, m); cout << ","; output(chunk, words, 2 * x * x - x, m); cout << close << term << endl;

        cout << pre6 << "CURVE_BB" << post6 << open;
        cout << open;
        output(chunk, words, r - x + 1, m);
        cout << ","; output(chunk, words, r - x, m);
        cout << ","; output(chunk, words, r - x, m);
        cout << ","; output(chunk, words, 2 * x, m);
        cout << close;

        cout << ","; cout << open; output(chunk, words, 2 * x - 1, m);
        cout << ","; output(chunk, words, r - x, m);
        cout << ","; output(chunk, words, r - x + 1, m);
        cout << ","; output(chunk, words, r - x, m);
        cout << close;
        cout << ","; cout << open; output(chunk, words, 2 * x, m);
        cout << ","; output(chunk, words, 2 * x - 1, m);
        cout << ","; output(chunk, words, 2 * x - 1, m);
        cout << ","; output(chunk, words, 2 * x - 1, m);
        cout << close;

        cout << ","; cout << open; output(chunk, words, x + 1, m);
        cout << ","; output(chunk, words, 4 * x - 2, m);
        cout << ","; output(chunk, words, r - 2 * x - 1, m);
        cout << ","; output(chunk, words, x + 1, m);
        cout << close;
        cout << close << term << endl;

    }

// BN curves, positive x
    if (curve == PS+5 || curve == PS+7)
    {
        cout << endl;

        cout << pre1 << toupperit((char *)"CURVE_Bnx", lang) << post1; output(chunk, words, x, m); cout << term << endl;
//        cout << pre1 << toupperit((char *)"CURVE_Cru", lang) << post1; output(chunk, words, cru, m); cout << term << endl;
//cout << "Q= " << Q << endl;
        Q.get(Xa, Ya);
        Xa.get(a, b);
        cout << pre1 << toupperit((char *)"CURVE_Pxa", lang) << post1; output(chunk, words, a, m); cout << term << endl;
        cout << pre1 << toupperit((char *)"CURVE_Pxb", lang) << post1; output(chunk, words, b, m); cout << term << endl;
        Ya.get(a, b);
        cout << pre1 << toupperit((char *)"CURVE_Pya", lang) << post1; output(chunk, words, a, m); cout << term << endl;
        cout << pre1 << toupperit((char *)"CURVE_Pyb", lang) << post1; output(chunk, words, b, m); cout << term << endl;


        cout << pre3 << "CURVE_W"  << post3 << open; output(chunk, words, 6 * x * x + 4 * x + 1, m); cout << ","; output(chunk, words, (2 * x + 1), m); cout << close << term << endl;
        cout << pre4 << "CURVE_SB" << post4 << open; cout << open; output(chunk, words, 6 * x * x + 2 * x, m); cout << ","; output(chunk, words, r - (2 * x + 1), m); cout << close; cout << ",";
        cout << open; output(chunk, words, (2 * x + 1), m); cout << ","; output(chunk, words, (6 * x * x + 4 * x + 1), m); cout << close; cout << close << term  << endl;

        cout << pre5 << "CURVE_WB" << post5 << open; output(chunk, words, 2 * x * x + 3 * x + 1, m); cout << ","; output(chunk, words, (12 * x * x * x + 8 * x * x + x), m);
        cout << ","; output(chunk, words, (6 * x * x * x + 4 * x * x + x), m); cout << ","; output(chunk, words, (2 * x * x + x), m); cout << close << term << endl;

        cout << pre6 << "CURVE_BB" << post6 << open;
        cout << open;
        output(chunk, words, x + 1, m);
        cout << ","; output(chunk, words, x, m);
        cout << ","; output(chunk, words, x, m);
        cout << ","; output(chunk, words, r - 2 * x, m);
        cout << close;

        cout << ","; cout << open; output(chunk, words, (2 * x + 1), m);
        cout << ","; output(chunk, words, r - x, m);
        cout << ","; output(chunk, words, r - (x + 1), m);
        cout << ","; output(chunk, words, r - x, m);
        cout << close;
        cout << ","; cout << open; output(chunk, words, 2 * x, m);
        cout << ","; output(chunk, words, 2 * x + 1, m);
        cout << ","; output(chunk, words, 2 * x + 1, m);
        cout << ","; output(chunk, words, 2 * x + 1, m);
        cout << close;

        cout << ","; cout << open; output(chunk, words, r - (x - 1), m);
        cout << ","; output(chunk, words, r - (4 * x + 2), m);
        cout << ","; output(chunk, words, (2 * x - 1), m);
        cout << ","; output(chunk, words, r - (x - 1), m);
        cout << close;
        cout << close << term << endl;

    }


//BLS curves
    if (curve == PS+2 || curve == PS+3 || curve == PS+6)
    {
        cout << endl;

        cout << pre1 << toupperit((char *)"CURVE_Bnx", lang) << post1 ; output(chunk, words, x, m); cout << term << endl;
//        cout << pre1 << toupperit((char *)"CURVE_Cru", lang) << post1; output(chunk, words, cru, m); cout << term << endl;

        Q.get(Xa, Ya);
        Xa.get(a, b);
        cout << pre1 << toupperit((char *)"CURVE_Pxa", lang) << post1; output(chunk, words, a, m); cout << term << endl;
        cout << pre1 << toupperit((char *)"CURVE_Pxb", lang) << post1; output(chunk, words, b, m); cout << term << endl;
        Ya.get(a, b);
        cout << pre1 << toupperit((char *)"CURVE_Pya", lang) << post1; output(chunk, words, a, m); cout << term << endl;
        cout << pre1 << toupperit((char *)"CURVE_Pyb", lang) << post1; output(chunk, words, b, m); cout << term << endl;

        Q *= r;
        if (!Q.iszero())
        {
            cout << "**** Failed ****" << endl;
            cout << "\nQ= " << Q << endl << endl;
        }

        cout << close;
        cout << close << term << endl;

    }

    if (curve == PS+3)   // bls12381
    {
        cout << pre1 << toupperit((char *)"CURVE_Ad", lang) << post1; output(chunk, words, ad, m); cout << term << endl;
        cout << pre1 << toupperit((char *)"CURVE_Bd", lang) << post1; output(chunk, words, bd, m); cout << term << endl;
        cout << pre8 << "PC" << post8 << open;
        for (i=0;i<ncs-1;i++)
        {
            output(chunk, words, pc[i], m); cout << ",";
        }
        output(chunk, words, pc[ncs-1], m);  
        cout << close << term << endl;
    }


    if (curve == PS+3)   // bls12381
    {
        cout << pre1 << toupperit((char *)"CURVE_HTPC2", lang) << post1; output(chunk, words, htpc2, m); cout << term << endl;
        cout << pre1 << toupperit((char *)"CURVE_Adr", lang) << post1; output(chunk, words, adr, m); cout << term << endl;
        cout << pre1 << toupperit((char *)"CURVE_Adi", lang) << post1; output(chunk, words, adi, m); cout << term << endl;
        cout << pre1 << toupperit((char *)"CURVE_Bdr", lang) << post1; output(chunk, words, bdr, m); cout << term << endl;
        cout << pre1 << toupperit((char *)"CURVE_Bdi", lang) << post1; output(chunk, words, bdi, m); cout << term << endl;
        cout << pre8 << "PCR" << post9 << open;
        for (i=0;i<ncs2-1;i++)
        {
            output(chunk, words, pcr[i], m); cout << ",";
        }
        output(chunk, words, pcr[ncs2-1], m);  
        cout << close << term << endl;
        cout << pre8 << "PCI" << post9 << open;
        for (i=0;i<ncs2-1;i++)
        {
            output(chunk, words, pci[i], m); cout << ",";
        }
        output(chunk, words, pci[ncs2-1], m);  
        cout << close << term << endl;

    }

    if (curve == PS+8)
    {
        cout << endl;

        cout << pre1 << toupperit((char *)"CURVE_Bnx", lang) << post1 ; output(chunk, words, x, m); cout << term << endl;
//        cout << pre1 << toupperit((char *)"CURVE_Cru", lang) << post1; output(chunk, words, cru, m); cout << term << endl;

        QQ.get(XA, YA);
        XA.get(Aa, Bb);
        Aa.get(a, b);
        cout << pre1 << toupperit((char *)"CURVE_Pxaa", lang) << post1; output(chunk, words, a, m); cout << term << endl;
        cout << pre1 << toupperit((char *)"CURVE_Pxab", lang) << post1; output(chunk, words, b, m); cout << term << endl;
        Bb.get(a, b);
        cout << pre1 << toupperit((char *)"CURVE_Pxba", lang) << post1; output(chunk, words, a, m); cout << term << endl;
        cout << pre1 << toupperit((char *)"CURVE_Pxbb", lang) << post1; output(chunk, words, b, m); cout << term << endl;

        YA.get(Aa, Bb);
        Aa.get(a, b);
        cout << pre1 << toupperit((char *)"CURVE_Pyaa", lang) << post1; output(chunk, words, a, m); cout << term << endl;
        cout << pre1 << toupperit((char *)"CURVE_Pyab", lang) << post1; output(chunk, words, b, m); cout << term << endl;
        Bb.get(a, b);
        cout << pre1 << toupperit((char *)"CURVE_Pyba", lang) << post1; output(chunk, words, a, m); cout << term << endl;
        cout << pre1 << toupperit((char *)"CURVE_Pybb", lang) << post1; output(chunk, words, b, m); cout << term << endl;


        QQ *= r;
        if (!Q.iszero())
        {
            cout << "**** Failed ****" << endl;
            cout << "\nQQ= " << QQ << endl << endl;
        }

        cout << close;
        cout << close << term << endl;

    }

    if (curve == PS+9 || curve == PS+10 || curve == PS+11)
    {
        cout << endl;

        cout << pre1 << toupperit((char *)"CURVE_Bnx", lang) << post1 ; output(chunk, words, x, m); cout << term << endl;
//        cout << pre1 << toupperit((char *)"CURVE_Cru", lang) << post1; output(chunk, words, cru, m); cout << term << endl;

        if (curve == PS+9 || curve == PS+11)
            Q8.get(X8, Y8);
        X8.get(AA, BB);

        AA.get(Aa, Bb);
        Aa.get(a, b);
        cout << pre1 << toupperit((char *)"CURVE_Pxaaa", lang) << post1; output(chunk, words, a, m); cout << term << endl;
        cout << pre1 << toupperit((char *)"CURVE_Pxaab", lang) << post1; output(chunk, words, b, m); cout << term << endl;
        Bb.get(a, b);
        cout << pre1 << toupperit((char *)"CURVE_Pxaba", lang) << post1; output(chunk, words, a, m); cout << term << endl;
        cout << pre1 << toupperit((char *)"CURVE_Pxabb", lang) << post1; output(chunk, words, b, m); cout << term << endl;

        BB.get(Aa, Bb);
        Aa.get(a, b);
        cout << pre1 << toupperit((char *)"CURVE_Pxbaa", lang) << post1; output(chunk, words, a, m); cout << term << endl;
        cout << pre1 << toupperit((char *)"CURVE_Pxbab", lang) << post1; output(chunk, words, b, m); cout << term << endl;
        Bb.get(a, b);
        cout << pre1 << toupperit((char *)"CURVE_Pxbba", lang) << post1; output(chunk, words, a, m); cout << term << endl;
        cout << pre1 << toupperit((char *)"CURVE_Pxbbb", lang) << post1; output(chunk, words, b, m); cout << term << endl;

        Y8.get(AA, BB);

        AA.get(Aa, Bb);
        Aa.get(a, b);
        cout << pre1 << toupperit((char *)"CURVE_Pyaaa", lang) << post1; output(chunk, words, a, m); cout << term << endl;
        cout << pre1 << toupperit((char *)"CURVE_Pyaab", lang) << post1; output(chunk, words, b, m); cout << term << endl;
        Bb.get(a, b);
        cout << pre1 << toupperit((char *)"CURVE_Pyaba", lang) << post1; output(chunk, words, a, m); cout << term << endl;
        cout << pre1 << toupperit((char *)"CURVE_Pyabb", lang) << post1; output(chunk, words, b, m); cout << term << endl;

        BB.get(Aa, Bb);
        Aa.get(a, b);
        cout << pre1 << toupperit((char *)"CURVE_Pybaa", lang) << post1; output(chunk, words, a, m); cout << term << endl;
        cout << pre1 << toupperit((char *)"CURVE_Pybab", lang) << post1; output(chunk, words, b, m); cout << term << endl;
        Bb.get(a, b);
        cout << pre1 << toupperit((char *)"CURVE_Pybba", lang) << post1; output(chunk, words, a, m); cout << term << endl;
        cout << pre1 << toupperit((char *)"CURVE_Pybbb", lang) << post1; output(chunk, words, b, m); cout << term << endl;

        if (curve == PS+9 || curve == PS+11)
        {
            Q8 *= r;
            if (!Q.iszero())
            {
                cout << "**** Failed ****" << endl;
                cout << "\nQQ= " << QQ << endl << endl;
            }
        }

    }
}
