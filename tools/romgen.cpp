
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
int PS=26;  // start of pairing friendly curves

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

ZZn8 findZ8(Big AB)
{
    ZZn8 Z,B;
    if (get_mip()->TWIST==MR_SEXTIC_M)
        B=tx((ZZn8)AB);
    else
        B=txd((ZZn8)AB);
    int a,b;
    a=1; b=-1;
    forever {
        if (b>0)
        {
            a=-a;
            b=-b;
        }
        else
        {    
            b=1;
            a=(-a)+1;
        }
        Z.set(a,b);
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
    return Z;
}

ZZn4 findZ4(Big AB)
{
    ZZn4 Z,B;
    if (get_mip()->TWIST==MR_SEXTIC_M)
        B=tx((ZZn4)AB);
    else
        B=txd((ZZn4)AB);
    int a,b;
    a=1; b=-1;
    forever {
        if (b>0)
        {
            a=-a;
            b=-b;
        }
        else
        {    
            b=1;
            a=(-a)+1;
        }
        Z.set(a,b);
        //cout << "Z= " << Z << endl;
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
    return Z;
}

ZZn2 findZ2(Big AB)
{
    ZZn2 Z,B;
    if (get_mip()->TWIST==MR_SEXTIC_M)
        B=txx((ZZn2)AB);
    else
        B=txd((ZZn2)AB);
    int a,b;
    a=1; b=-1;
    forever {
        if (b>0)
        {
            a=-a;
            b=-b;
        }
        else
        {    
            b=1;
            a=(-a)+1;
        }
        Z.set(a,b);
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
    return Z;
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

int findR(Big AA,Big AB,Big p)
{
    modulo(p);

    ZZn A=(ZZn)AA;
    ZZn B=(ZZn)AB;

    ZZn K=(ZZn)4/(A-B);
    if (qr(K)) return 1;
    return 0;
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

    printf("26. BN254\n");
    printf("27. BN254CX\n");
    printf("28. BLS12383\n");
    printf("29. BLS12381\n");
    printf("30. FP256BN\n");
    printf("31. FP512BN\n");
    printf("32. BLS12461\n");
    printf("33. BN462\n");

    printf("34. BLS24479\n");
    printf("35. BLS48556\n");
    printf("36. BLS48581\n");

    printf("\nromgen curve wordlength basebits language\n");
    printf("where wordlength is 16, 32 or 64\n");
    printf("basebits is less than wordlength\n\n");
    printf("Use check utility to determine best choice for basebits\n");
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
    Big p, R, B, mc, curve_b, cru, cof, tau[9];
    Big m, x, y, w, t, c, n, r, a, b, gx, gy, r2modp,roi;
    Big np, PP, TT, FF;
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
    ZZn zcru;
    char pre0[50], pre1[50], pre2[50], pre3[50], pre4[50], pre5[50], pre6[50];
    char post0[50], post1[50], post2[50], post3[50], post4[50], post5[50], post6[50];
    char pre7[50], post7[50], lg[50];

    char xxx[20], yyy[20], zzz[20];

    char curvename[50], fieldname[50];

    argv++; argc--;

    if (argc < 4)
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

    //cout << "curvename= " << curvename << " " << strlen(curvename) << endl;

    curve = 0; ip++;
    chunk = atoi(argv[ip++]);
    bb = atoi(argv[ip++]);

    strcpy(lg, argv[ip]);

    if (chunk != 16 && chunk != 32 && chunk != 64) {help(); return 0;}
    if (bb < 0 || bb >= chunk) {help(); return 0;}

// Specify curve constants

    if (strcmp(curvename, "ED25519") == 0)
    {   // ED25519
        curve = 1;
        printf("Curve= ED25519\n");
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
    }

    if (strcmp(curvename, "C25519") == 0)
    {
        curve = 2;
        printf("Curve= C25519\n");
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
    }

    if (strcmp(curvename, "X448") == 0)
    {
        curve = 21;
        printf("Curve= X448\n");
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
    }

    if (strcmp(curvename, "SECP160R1") == 0)
    {
        curve = 22;
        printf("Curve= SECP160R1\n");
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
    }


    if (strcmp(curvename, "NIST256") == 0)
    {
        curve = 3;
        printf("Curve= NIST256\n");
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

    }

    if (strcmp(curvename, "SM2") == 0)
    {
        curve = 18;
        printf("Curve= SM2\n");
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

    }

    if (strcmp(curvename, "C13318") == 0)
    {
        curve = 19;
        printf("Curve= C13318\n");
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
        gx=5 ;
        gy=(char *)"6675AAD926BCA6F1381630E5166966369D4CCB04CF016DB5B8C8D3546B6EAD0B";
    }

    if (strcmp(curvename, "JUBJUB") == 0)
    {
        curve = 20;
        printf("Curve= JUBJUB\n");
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
    }

    if (strcmp(curvename, "BRAINPOOL") == 0)
    {
        curve = 4;
        printf("Curve= BRAINPOOL\n");
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
    }

    if (strcmp(curvename, "ANSSI") == 0)
    {
        curve = 5;
        printf("Curve= ANSSI\n");
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
    }

    if (strcmp(curvename, "HIFIVE") == 0)
    {
        curve = 6;
        printf("Curve= HIFIVE\n");
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
    }

    if (strcmp(curvename, "GOLDILOCKS") == 0)
    {
        curve = 7;
        printf("Curve= GOLDILOCKS\n");
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
    }

    if (strcmp(curvename, "NIST384") == 0)
    {
        curve = 8;
        printf("Curve= NIST384\n");
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
    }

    if (strcmp(curvename, "C41417") == 0)
    {
        curve = 9;
        printf("Curve= C41417\n");
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
    }

    if (strcmp(curvename, "C1174") == 0)
    {
        curve = 23;
        printf("Curve= C1174\n");
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

    }

    if (strcmp(curvename, "C1665") == 0)
    {
        curve = 24;
        printf("Curve= C1665\n");
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

    }

    if (strcmp(curvename, "MDC") == 0)
    {
        curve = 25;
        printf("Curve= Million Dollar Curve\n");
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
    }


    if (strcmp(curvename, "NIST521") == 0)
    {
        curve = 10;
        printf("Curve= NIST521\n");
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
    }

    if (strcmp(curvename, "NUMS256W") == 0)
    {
        curve = 11;
        printf("Curve= NUMS256W\n");
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
    }

    if (strcmp(curvename, "NUMS256E") == 0)
    {
        curve = 12;
        printf("Curve= NUMS256E\n");
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
    }


    if (strcmp(curvename, "NUMS384W") == 0)
    {
        curve = 13;
        printf("Curve= NUMS384W\n");
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
    }

    if (strcmp(curvename, "NUMS384E") == 0)
    {
        curve = 14;
        printf("Curve= NUMS384E\n");
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
    }

    if (strcmp(curvename, "NUMS512W") == 0)
    {
        curve = 15;
        printf("Curve= NUMS512W\n");
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
    }

    if (strcmp(curvename, "NUMS512E") == 0)
    {
        curve = 16;
        printf("Curve= NUMS512E\n");
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
    }


    if (strcmp(curvename, "SECP256K1") == 0)
    {   // SECP256K1
        curve = 17;
        printf("Curve= SECP256K1\n");
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
    }


    if (strcmp(curvename, "BN254") == 0)
    {
        curve = PS+0;
        printf("Curve= BN254\n");
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

        cout << "Z for G2= " << findZ2(curve_b) << endl;
    }


    if (strcmp(curvename, "BN254CX") == 0)
    {
        curve = PS+1;
        printf("Curve= BN254CX\n");
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

        cout << "Z for G2= " << findZ2(curve_b) << endl;
    }

    if (strcmp(curvename, "BLS12383") == 0)
    {
        curve = PS+2;
        printf("Curve= BLS12383\n");
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
        cout << "Z for G2= " << findZ2(curve_b) << endl;
    }

    if (strcmp(curvename, "BLS12381") == 0)
    {
        curve = PS+3;
        printf("Curve= BLS12381\n");
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

        cout << "Z for G2= " << findZ2(curve_b) << endl;
    }


    if (strcmp(curvename, "BLS12461") == 0)
    {
        curve = PS+6;
        printf("Curve= BLS12461\n");
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
        cout << "Z for G2= " << findZ2(curve_b) << endl;
    }

    if (strcmp(curvename, "BN462") == 0)
    {
        curve = PS+7;
        printf("Curve= BN462\n");
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
        cout << "Z for G2= " << findZ2(curve_b) << endl;
    }

    if (strcmp(curvename, "FP256BN") == 0)
    {
        curve = PS+4;
        printf("Curve= FP256BN\n");
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
        //cout << pre1 << toupperit((char *)"CURVE_Cru",lang) << post1; output(chunk,words,cru,m); cout << term << endl;
        cout << "Z for G2= " << findZ2(curve_b) << endl;
    }

    if (strcmp(curvename, "FP512BN") == 0)
    {
        curve = PS+5;
        printf("Curve= FP512BN\n");
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
        cout << "Z for G2= " << findZ2(curve_b) << endl;
    }

    if (strcmp(curvename, "BLS24479") == 0)
    {
        curve = PS+8;
        printf("Curve= BLS24479\n");
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
        cout << "Z for G2= " << findZ4(curve_b) << endl;
    }


    if (strcmp(curvename, "BLS48556") == 0)
    {
        curve = PS+9;
        printf("Curve= BLS48556\n");
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

//cout << "Q8= " << Q8 << endl;
//cout << "r*Q8= " << r*Q8 << endl;
//cout << "2*QQ= " << QQ+QQ << endl;
//cout << "3*QQ= " << (QQ+QQ)+QQ << endl;

        zcru = pow((ZZn)2, (p - 1) / 3);
        //zcru*=zcru;   // right cube root of unity -  not for M-TYPE
        cru = (Big)zcru;
        cout << "Z for G2= " << findZ8(curve_b) << endl;
    }

    if (strcmp(curvename, "BLS48581") == 0)
    {
        curve = PS+10;
        printf("Curve= BLS48581\n");
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

        //mip->IOBASE=10;
        //gx=(char *)"4258736819635888510719226922301423369609633131342557579758281231565577240750348576646920600528529682191246450994803200560373551112655126678927104568272621858560534577202293646";
        //gy=(char *)"3427753022670262326398897872606828748191995684951918749279729536371139924954542845900842929251072627395333429414195787875631282056102416831610447264733414174240889928748279117";
        //mip->IOBASE=16;

        gx = (char *)"2af59b7ac340f2baf2b73df1e93f860de3f257e0e86868cf61abdbaedffb9f7544550546a9df6f9645847665d859236ebdbc57db368b11786cb74da5d3a1e6d8c3bce8732315af640";
        gy = (char *)"cefda44f6531f91f86b3a2d1fb398a488a553c9efeb8a52e991279dd41b720ef7bb7beffb98aee53e80f678584c3ef22f487f77c2876d1b2e35f37aef7b926b576dbb5de3e2587a70";
        P.set(gx, gy);

        //mip->IOBASE=10;
        //cout << "gx= " << gx << endl;
        //cout << "gy= " << gy << endl;

        //cout << "P= " << P << endl;

        Big A, B, C, D, E, F, G, H;

        /*
        		A=(char *)"488522055906013098943370946455285145355750022558560065778362316774880512321957078343030869711412368852213762684582445262738574506980017266080882902887975281911127402138877276";
        		B=(char *)"4530198686318730510615089875264680313006345876681012648895720789199763506189688649358387867840809822428403436871165001502645614044016797532584433912003964549501792725823854756";
        		C=(char *)"683837078546764585195750064552918557516365179073606906666761988700558966619842277357379749824515752510816863908059582033421855827477583545210267363313213829241029716522797255";
        		D=(char *)"1981934010367807061915652782383503071917169317527775149288469127162084642848020133860443159207600913612194229288603419175030496303585499135445245021122248421992420686302425383";
        		E=(char *)"168328765107154049987937110753764973401681006189099111956765018971541681538951029395375149352531639886350950053859438193946060081015269247643286488404161156278246150756577891";
        		F=(char *)"3765116545484126637555129173607875429930449845266957448618134161173541239495345577260720369556871499909791600240654025123169064763242585032002364763888696589321867639260119261";
        		G=(char *)"2529386951286407488724612742599500309109994334805943776951296491260985735531939220379556337276196918517309051294000130102897987426747727701435767021012046080593011881209249823";
        		H=(char *)"3350331868018643152265912673171698250622202352563501404345821417390238956281236395761710054662997264976015254771313408174115751829265818705241778095377681706221075354386408203";
        */
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
        /*
        		A=(char *)"1752792025746338528024487525888549368413081275325602641340459351157268126367292017499287064777262888681363907551242455740753520720646621236386819731028718543609868153423984647";
        		B=(char *)"2868397909921871882437653406338359330849869736920682748716626290360573813614970955881513871275020481583722584452488193924358479769164147029352260763115998160803794306498494391";
        		C=(char *)"1863845751178157488246660458090179186845236426212411521385655190629190609029418750286436531057181929252995969970422233447486763009676404818692624871899449613743689472888122869";
        		D=(char *)"4130428535653681329585296374200928046386195800843673002887975436457007892165426804712153353358564618967673827278843186985619333036209283245589330093261961913009398742697993336";
        		E=(char *)"1732340534986452053971826795759250223535962464757445465821947496318112301027427981988601709548889033580829119912332584118587866796935614094152016493282077373930038948286692454";
        		F=(char *)"2960630917170779964461366236940322101671590500139330287091284102683723381820302564880131181856363418825111505237215584983014806947545094962779602044989872378419578311531860585";
        		G=(char *)"2718719781479526894128944593378055641217078194586954694958080216383349654864844236158275766770132073693386915005687021760158794555089580856491736525086318868009818515997753870";
        		H=(char *)"2501101090817478348088902140218203391417482850174097957696120668742105502488232898688748719467444122961977036174041583876415008384452991231279210159249877706951729963181544641";
        */


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
        /*
        		Q8.set(X8,Y8);

        		mip->IOBASE=16;

        		cout << "X= " << X8 << endl;
        		cout << "Y= " << Y8 << endl;
        		cout << "Y*Y=   "  << Y8*Y8 << endl;
        		cout << "X*X*X= "  << X8*X8*X8+txd((ZZn8)1) << endl;
        		cout << "MyQ8= " << Q8 << endl;
        exit(0); */
        /*		Big x0=0;

        		forever
        		{
        			ZZn8 XX;
        			ZZn4 X;
        			ZZn2 t;

        			x0+=1;
        			t.set((ZZn)0,(ZZn)x0);
        			X.set(t,(ZZn2)0);
        			XX.set(X,(ZZn4)0);
        			if (!Q8.set(X)) continue;
        			break;
        		}


        		tau[0]=2;  // count points on twist over extension p^8
        		tau[1]=t;
        		for (int jj=1;jj<8;jj++ ) tau[jj+1]=t*tau[jj]-p*tau[jj-1];

        		TT=tau[8];

        		PP=pow(p,8);
        		FF=sqrt((4*PP-TT*TT)/3);
        		np=PP+1-(3*FF+TT)/2;  //?

        		Q8=(np/r)*Q8;

        cout << "Q8= " << Q8 << endl;
        */
//cout << "r*Q8= " << r*Q8 << endl;
//cout << "2*QQ= " << QQ+QQ << endl;
//cout << "3*QQ= " << (QQ+QQ)+QQ << endl;

        zcru = pow((ZZn)2, (p - 1) / 3);
        zcru *= zcru; // right cube root of unity -  not for M-TYPE
        cru = (Big)zcru;
        cout << "Z for G2= " << findZ8(curve_b) << endl;
    }

    if (curve == 0) {help(); return 0;}

    if (curvetype == WEIERSTRASS) printf("Z= %d\n",findZ(curve_a,curve_b,p));
    if (curvetype == EDWARDS) printf("Z= %d\n",findR(curve_a,curve_b,p));

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

        strcpy(post0, "= ");
        strcpy(post1, "= ");
        strcpy(post2, "= ");
        strcpy(post3, "[2]= ");
        strcpy(post4, "[2][2]= ");
        strcpy(post5, "[4]= ");
        strcpy(post6, "[4][4]= ");
        strcpy(post7, "= ");

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

        }
        strcpy(post0, "= ");
        strcpy(post1, "= ");
        strcpy(post2, "= ");
        strcpy(post3, "= ");
        strcpy(post4, "= ");
        strcpy(post5, "= ");
        strcpy(post6, "= ");
        strcpy(post7, "= ");

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
        strcpy(post0, ": ");
        strcpy(post1, ": ");
        strcpy(post2, ": ");
        strcpy(post3, ": ");
        strcpy(post4, ": ");
        strcpy(post5, ": ");
        strcpy(post6, ": ");
        strcpy(post7, ": ");
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
        strcpy(post0, " int= ");
        strcpy(post1, "= [...]Chunk ");
        strcpy(post2, " Chunk=");
        sprintf(post3, "=[2][%d]Chunk ", words);
        sprintf(post4, "=[2][2][%d]Chunk ", words);
        sprintf(post5, "=[4][%d]Chunk ", words);
        sprintf(post6, "=[4][4][%d]Chunk ", words);
        strcpy(post7, "= [...]Chunk ");
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
        strcpy(post0, ":isize = ");
        strcpy(post1, ":[Chunk;NLEN]=");
        strcpy(post2, ":Chunk=");
        strcpy(post3, ":[[Chunk;NLEN];2]=");
        strcpy(post4, ":[[[Chunk;NLEN];2];2]=");
        strcpy(post5, ":[[Chunk;NLEN];4]=");
        strcpy(post6, ":[[[Chunk;NLEN];4];4]=");
        strcpy(post7, ":[Chunk;NLEN]=");

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
        strcpy(post0, ":Int = ");
        strcpy(post1, ":[Chunk] = ");
        strcpy(post2, ":Chunk = ");
        strcpy(post3, ":[[Chunk]] = ");
        strcpy(post4, ":[[[Chunk]]] = ");
        strcpy(post5, ":[[Chunk]] = ");
        strcpy(post6, ":[[[Chunk]]] = ");
        strcpy(post7, ":[Chunk] = ");
    }


    if (lang == 0) {help(); return 0;}

    //cout << "\nMOD8 = " << p%8 << endl;

    m = pow((Big)2, bb);

    cout << "//*** rom field parameters*****" << endl;
    cout << "// Base Bits= " << bb << endl;

    cout << pre7 << toupperit((char *)"Modulus", lang) << post7; mc = output(chunk, words, p, m); cout << term << endl;
    r2modp = pow((Big)2, 2 * words * bb) % p;

    int e=0;
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
            if (curve == PS+9 || curve == PS+10) set_frobenius_constant(X, 48);
            if (curve == PS+10) X = -X;
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
    cout << "// Base Bits= " << bb << endl;
    if (curve >= PS)
    {
        cout << "// Ate Bits= " << atebits << endl;
        cout << "// G2 Table size= " << atebits + hw - 1 << endl;
    }
    cout << "\n" << pre0 << "CURVE_A"  << post0 << curve_a << term << endl;

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

// BN curves, negative x
    if (curve == PS || curve == PS+1 || curve == PS+4)
    {
        cout << endl;

        cout << pre1 << toupperit((char *)"CURVE_Bnx", lang) << post1; output(chunk, words, x, m); cout << term << endl;
        cout << pre1 << toupperit((char *)"CURVE_Cru", lang) << post1; output(chunk, words, cru, m); cout << term << endl;
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
        cout << pre1 << toupperit((char *)"CURVE_Cru", lang) << post1; output(chunk, words, cru, m); cout << term << endl;
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
        cout << pre1 << toupperit((char *)"CURVE_Cru", lang) << post1; output(chunk, words, cru, m); cout << term << endl;

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
/*
        cout << pre3 << "CURVE_W" << post3 << open; output(chunk, words, (Big)0, m); cout << ","; output(chunk, words, (Big)0, m); cout << close << term << endl;
        cout << pre4 << "CURVE_SB" << post4 << open; cout << open; output(chunk, words, (Big)0, m); cout << ","; output(chunk, words, (Big)0, m); cout << close; cout << ","; cout << open; output(chunk, words, (Big)0, m); cout << ","; output(chunk, words, (Big)0, m); cout << close; cout << close << term << endl;

        cout << pre5 << "CURVE_WB" << post5 << open; output(chunk, words, (Big)0, m); cout << ","; output(chunk, words, (Big)0, m);
        cout << ","; output(chunk, words, (Big)0, m); cout << ","; output(chunk, words, (Big)0, m); cout << close << term << endl;

        cout << pre6 << "CURVE_BB" << post6 << open;
        cout << open;
        output(chunk, words, (Big)0, m);
        cout << ","; output(chunk, words, (Big)0, m);
        cout << ","; output(chunk, words, (Big)0, m);
        cout << ","; output(chunk, words, (Big)0, m);
        cout << close;

        cout << ","; cout << open; output(chunk, words, (Big)0, m);
        cout << ","; output(chunk, words, (Big)0, m);
        cout << ","; output(chunk, words, (Big)0, m);
        cout << ","; output(chunk, words, (Big)0, m);
        cout << close;
        cout << ","; cout << open; output(chunk, words, (Big)0, m);
        cout << ","; output(chunk, words, (Big)0, m);
        cout << ","; output(chunk, words, (Big)0, m);
        cout << ","; output(chunk, words, (Big)0, m);
        cout << close;

        cout << ","; cout << open; output(chunk, words, (Big)0, m);
        cout << ","; output(chunk, words, (Big)0, m);
        cout << ","; output(chunk, words, (Big)0, m);
        cout << ","; output(chunk, words, (Big)0, m);
*/
        cout << close;
        cout << close << term << endl;

    }



    if (curve == PS+8)
    {
        cout << endl;

        cout << pre1 << toupperit((char *)"CURVE_Bnx", lang) << post1 ; output(chunk, words, x, m); cout << term << endl;
        cout << pre1 << toupperit((char *)"CURVE_Cru", lang) << post1; output(chunk, words, cru, m); cout << term << endl;

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
/*
        cout << pre3 << "CURVE_W" << post3 << open; output(chunk, words, (Big)0, m); cout << ","; output(chunk, words, (Big)0, m); cout << close << term << endl;
        cout << pre4 << "CURVE_SB" << post4 << open; cout << open; output(chunk, words, (Big)0, m); cout << ","; output(chunk, words, (Big)0, m); cout << close; cout << ","; cout << open; output(chunk, words, (Big)0, m); cout << ","; output(chunk, words, (Big)0, m); cout << close; cout << close << term << endl;

        cout << pre5 << "CURVE_WB" << post5 << open; output(chunk, words, (Big)0, m); cout << ","; output(chunk, words, (Big)0, m);
        cout << ","; output(chunk, words, (Big)0, m); cout << ","; output(chunk, words, (Big)0, m); cout << close << term << endl;

        cout << pre6 << "CURVE_BB" << post6 << open;
        cout << open;
        output(chunk, words, (Big)0, m);
        cout << ","; output(chunk, words, (Big)0, m);
        cout << ","; output(chunk, words, (Big)0, m);
        cout << ","; output(chunk, words, (Big)0, m);
        cout << close;

        cout << ","; cout << open; output(chunk, words, (Big)0, m);
        cout << ","; output(chunk, words, (Big)0, m);
        cout << ","; output(chunk, words, (Big)0, m);
        cout << ","; output(chunk, words, (Big)0, m);
        cout << close;
        cout << ","; cout << open; output(chunk, words, (Big)0, m);
        cout << ","; output(chunk, words, (Big)0, m);
        cout << ","; output(chunk, words, (Big)0, m);
        cout << ","; output(chunk, words, (Big)0, m);
        cout << close;

        cout << ","; cout << open; output(chunk, words, (Big)0, m);
        cout << ","; output(chunk, words, (Big)0, m);
        cout << ","; output(chunk, words, (Big)0, m);
        cout << ","; output(chunk, words, (Big)0, m);
*/
        cout << close;
        cout << close << term << endl;

    }

    if (curve == PS+9 || curve == PS+10)
    {
        cout << endl;

        cout << pre1 << toupperit((char *)"CURVE_Bnx", lang) << post1 ; output(chunk, words, x, m); cout << term << endl;
        cout << pre1 << toupperit((char *)"CURVE_Cru", lang) << post1; output(chunk, words, cru, m); cout << term << endl;

        if (curve == PS+9)
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

        if (curve == PS+9)
        {
            Q8 *= r;
            if (!Q.iszero())
            {
                cout << "**** Failed ****" << endl;
                cout << "\nQQ= " << QQ << endl << endl;
            }
        }
/*
        cout << pre3 << "CURVE_W" << post3 << open; output(chunk, words, (Big)0, m); cout << ","; output(chunk, words, (Big)0, m); cout << close << term << endl;
        cout << pre4 << "CURVE_SB" << post4 << open; cout << open; output(chunk, words, (Big)0, m); cout << ","; output(chunk, words, (Big)0, m); cout << close; cout << ","; cout << open; output(chunk, words, (Big)0, m); cout << ","; output(chunk, words, (Big)0, m); cout << close; cout << close << term << endl;

        cout << pre5 << "CURVE_WB" << post5 << open; output(chunk, words, (Big)0, m); cout << ","; output(chunk, words, (Big)0, m);
        cout << ","; output(chunk, words, (Big)0, m); cout << ","; output(chunk, words, (Big)0, m); cout << close << term << endl;

        cout << pre6 << "CURVE_BB" << post6 << open;
        cout << open;
        output(chunk, words, (Big)0, m);
        cout << ","; output(chunk, words, (Big)0, m);
        cout << ","; output(chunk, words, (Big)0, m);
        cout << ","; output(chunk, words, (Big)0, m);
        cout << close;

        cout << ","; cout << open; output(chunk, words, (Big)0, m);
        cout << ","; output(chunk, words, (Big)0, m);
        cout << ","; output(chunk, words, (Big)0, m);
        cout << ","; output(chunk, words, (Big)0, m);
        cout << close;
        cout << ","; cout << open; output(chunk, words, (Big)0, m);
        cout << ","; output(chunk, words, (Big)0, m);
        cout << ","; output(chunk, words, (Big)0, m);
        cout << ","; output(chunk, words, (Big)0, m);
        cout << close;

        cout << ","; cout << open; output(chunk, words, (Big)0, m);
        cout << ","; output(chunk, words, (Big)0, m);
        cout << ","; output(chunk, words, (Big)0, m);
        cout << ","; output(chunk, words, (Big)0, m);
*/
        cout << close;
        cout << close << term << endl;
    }
}
