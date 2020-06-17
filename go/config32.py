#
# Copyright (c) 2012-2020 MIRACL UK Ltd.
#
# This file is part of MIRACL Core
# (see https://github.com/miracl/core).
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
import os
import sys

copytext="cp "
deltext="rm "
slashtext="/"
if sys.platform.startswith("win") :
    copytext=">NUL copy "
    deltext="del "
    slashtext="\\"

testing=False
if len(sys.argv)==2 :
    if sys.argv[1]=="test":
        testing=True
if testing :
    sys.stdin=open("test.txt","r")

chosen=[]
cptr=0

def replace(namefile,oldtext,newtext):
    f = open(namefile,'r')
    filedata = f.read()
    f.close()

    newdata = filedata.replace(oldtext,newtext)

    f = open(namefile,'w')
    f.write(newdata)
    f.close()


def rsaset(tb,nb,base,ml) :
    global deltext,slashtext,copytext
    global cptr,chosen

    chosen.append(tb)
    cptr=cptr+1

    fpath="core"+slashtext+tb+slashtext
    os.system("mkdir core"+slashtext+tb)

    os.system(copytext+"ARCH32.go "+fpath+"ARCH.go")
    os.system(copytext+"BIG32.go "+fpath+"BIG.go")
    os.system(copytext+"DBIG.go "+fpath+"DBIG.go")
    os.system(copytext+"FF32.go "+fpath+"FF.go")
    os.system(copytext+"RSA.go "+fpath+"RSA.go")
    os.system(copytext+"CONFIG_BIG.go "+fpath+"CONFIG_BIG.go")
    os.system(copytext+"CONFIG_FF.go "+fpath+"CONFIG_FF.go")

    replace(fpath+"ARCH.go","XXX",tb)
    replace(fpath+"CONFIG_BIG.go","XXX",tb)
    replace(fpath+"CONFIG_FF.go","XXX",tb)
    replace(fpath+"BIG.go","XXX",tb)
    replace(fpath+"DBIG.go","XXX",tb)
    replace(fpath+"FF.go","XXX",tb)
    replace(fpath+"RSA.go","XXX",tb)

    replace(fpath+"CONFIG_BIG.go","@NB@",nb)
    replace(fpath+"CONFIG_BIG.go","@BASE@",base)

    replace(fpath+"CONFIG_FF.go","@ML@",ml);

def curveset(tc,base,nbt,m8,rz,mt,qi,ct,ca,pf,stw,sx,g2,ab,cs) :

    inbt=int(nbt)
    itb=int(inbt+(8-inbt%8)%8)
    inb=int(itb/8)
    nb=str(inb)

    global deltext,slashtext,copytext
    global cptr,chosen

    chosen.append(tc)
    cptr=cptr+1

    fpath="core"+slashtext+tc+slashtext
    os.system("mkdir core"+slashtext+tc)

    os.system(copytext+"ARCH32.go "+fpath+"ARCH.go")
    os.system(copytext+"BIG32.go "+fpath+"BIG.go")
    os.system(copytext+"DBIG.go "+fpath+"DBIG.go")
    os.system(copytext+"FP.go "+fpath+"FP.go")
    os.system(copytext+"ECP.go "+fpath+"ECP.go")
    os.system(copytext+"ECDH.go "+fpath+"ECDH.go")
    os.system(copytext+"HPKE.go "+fpath+"HPKE.go")
    os.system(copytext+"CONFIG_BIG.go "+fpath+"CONFIG_BIG.go")
    os.system(copytext+"CONFIG_FIELD.go "+fpath+"CONFIG_FIELD.go")
    os.system(copytext+"CONFIG_CURVE.go "+fpath+"CONFIG_CURVE.go")
    os.system(copytext+"ROM_"+tc+"_32.go "+fpath+"ROM.go")

    replace(fpath+"ARCH.go","XXX",tc)
    replace(fpath+"BIG.go","XXX",tc)
    replace(fpath+"DBIG.go","XXX",tc)
    replace(fpath+"FP.go","XXX",tc)
    replace(fpath+"ECP.go","XXX",tc)
    replace(fpath+"ECDH.go","XXX",tc)
    replace(fpath+"HPKE.go","XXX",tc)
    replace(fpath+"CONFIG_BIG.go","XXX",tc)
    replace(fpath+"CONFIG_FIELD.go","XXX",tc)
    replace(fpath+"CONFIG_CURVE.go","XXX",tc)


    replace(fpath+"CONFIG_BIG.go","@NB@",nb)
    replace(fpath+"CONFIG_BIG.go","@BASE@",base)

    replace(fpath+"CONFIG_FIELD.go","@NBT@",nbt)
    replace(fpath+"CONFIG_FIELD.go","@M8@",m8)
    replace(fpath+"CONFIG_FIELD.go","@MT@",mt)

    if ca == "0" :
        replace(fpath+"ECP.go","CAISZS","*/")
        replace(fpath+"ECP.go","CAISZF","/*")

# Get Z for G1 and G2
    if isinstance(rz,list) :
        replace(fpath+"CONFIG_FIELD.go","@RZ@",rz[0])
        replace(fpath+"CONFIG_FIELD.go","@RZ2@",rz[1])
    else :
        replace(fpath+"CONFIG_FIELD.go","@RZ@",rz)
        replace(fpath+"CONFIG_FIELD.go","@RZ2@","0")

    itw=int(qi)%10
    replace(fpath+"CONFIG_FIELD.go","@QI@",str(itw))
    if int(qi)//10 > 0 :
        replace(fpath+"CONFIG_FIELD.go","@TW@","POSITOWER")
    else :
        replace(fpath+"CONFIG_FIELD.go","@TW@","NEGATOWER")


    ib=int(base)
    inb=int(nb)
    inbt=int(nbt)
    sh=ib*(1+((8*inb-1)//ib))-inbt
    if sh > 14 :
        sh=14
    replace(fpath+"CONFIG_FIELD.go","@SH@",str(sh))


    replace(fpath+"CONFIG_CURVE.go","@CT@",ct)
    replace(fpath+"CONFIG_CURVE.go","@CA@",ca)
    replace(fpath+"CONFIG_CURVE.go","@PF@",pf)

    replace(fpath+"CONFIG_CURVE.go","@ST@",stw)
    replace(fpath+"CONFIG_CURVE.go","@SX@",sx)
    replace(fpath+"CONFIG_CURVE.go","@AB@",ab)
    replace(fpath+"CONFIG_CURVE.go","@G2@",g2)

    if cs == "128" :
        replace(fpath+"CONFIG_CURVE.go","@HT@","32")
        replace(fpath+"CONFIG_CURVE.go","@AK@","16")
    if cs == "192" :
        replace(fpath+"CONFIG_CURVE.go","@HT@","48")
        replace(fpath+"CONFIG_CURVE.go","@AK@","24")
    if cs == "256" :
        replace(fpath+"CONFIG_CURVE.go","@HT@","64")
        replace(fpath+"CONFIG_CURVE.go","@AK@","32")

    if pf != "NOT" :

        os.system(copytext+"FP2.go "+fpath+"FP2.go")
        os.system(copytext+"FP4.go "+fpath+"FP4.go")

        replace(fpath+"FP2.go","XXX",tc)
        replace(fpath+"FP4.go","XXX",tc)

        if cs == "128" :
            os.system(copytext+"ECP2.go "+fpath+"ECP2.go")
            os.system(copytext+"FP12.go "+fpath+"FP12.go")
            os.system(copytext+"PAIR.go "+fpath+"PAIR.go")
            os.system(copytext+"MPIN.go "+fpath+"MPIN.go")
            os.system(copytext+"BLS.go "+fpath+"BLS.go")

            replace(fpath+"FP12.go","XXX",tc)
            replace(fpath+"ECP2.go","XXX",tc)
            replace(fpath+"PAIR.go","XXX",tc)
            replace(fpath+"MPIN.go","XXX",tc)
            replace(fpath+"BLS.go","XXX",tc)

            if pf == "BN" :
                replace(fpath+"PAIR.go","PFBNS","*/")
                replace(fpath+"PAIR.go","PFBNF","/*")

        if cs == "192" :
            os.system(copytext+"FP24.go "+fpath+"FP24.go")
            os.system(copytext+"FP8.go "+fpath+"FP8.go")
            os.system(copytext+"ECP4.go "+fpath+"ECP4.go")
            os.system(copytext+"PAIR192.go "+fpath+"PAIR192.go")
            os.system(copytext+"MPIN192.go "+fpath+"MPIN192.go")
            os.system(copytext+"BLS192.go "+fpath+"BLS192.go")

            replace(fpath+"FP24.go","XXX",tc)
            replace(fpath+"FP8.go","XXX",tc)
            replace(fpath+"ECP4.go","XXX",tc)
            replace(fpath+"PAIR192.go","XXX",tc)
            replace(fpath+"MPIN192.go","XXX",tc)
            replace(fpath+"BLS192.go","XXX",tc)


        if cs == "256" :
            os.system(copytext+"FP48.go "+fpath+"FP48.go")
            os.system(copytext+"FP16.go "+fpath+"FP16.go")
            os.system(copytext+"FP8.go "+fpath+"FP8.go")
            os.system(copytext+"ECP8.go "+fpath+"ECP8.go")
            os.system(copytext+"PAIR256.go "+fpath+"PAIR256.go")
            os.system(copytext+"MPIN256.go "+fpath+"MPIN256.go")
            os.system(copytext+"BLS256.go "+fpath+"BLS256.go")

            replace(fpath+"FP48.go","XXX",tc)
            replace(fpath+"FP16.go","XXX",tc)
            replace(fpath+"FP8.go","XXX",tc)
            replace(fpath+"ECP8.go","XXX",tc)
            replace(fpath+"PAIR256.go","XXX",tc)
            replace(fpath+"MPIN256.go","XXX",tc)
            replace(fpath+"BLS256.go","XXX",tc)


os.system("mkdir core")
os.system(copytext+ "HASH*.go core"+slashtext+".")
os.system(copytext+ "HMAC.go core"+slashtext+".")
os.system(copytext+ "SHA3.go core"+slashtext+".")
os.system(copytext+ "RAND.go core"+slashtext+".")
os.system(copytext+ "AES.go core"+slashtext+".")
os.system(copytext+ "GCM.go core"+slashtext+".")
os.system(copytext+ "NHS.go core"+slashtext+".")

print("Elliptic Curves")
print("1. ED25519")
print("2. C25519")
print("3. NIST256")
print("4. BRAINPOOL")
print("5. ANSSI")
print("6. HIFIVE")
print("7. GOLDILOCKS")
print("8. NIST384")
print("9. C41417")
print("10. NIST521")
print("11. NUMS256W")
print("12. NUMS256E")
print("13. NUMS384W")
print("14. NUMS384E")
print("15. NUMS512W")
print("16. NUMS512E")
print("17. SECP256K1")
print("18. SM2")
print("19. C13318")
print("20. JUBJUB")
print("21. X448")
print("22. SECP160R1")
print("23. C1174")
print("24. C1665")
print("25. Million Dollar Curve\n")

print("Pairing-Friendly Elliptic Curves")
print("26. BN254")
print("27. BN254CX")
print("28. BLS12383")
print("29. BLS12381")
print("30. FP256BN")
print("31. FP512BN")
print("32. BLS12461")
print("33. BN462")
print("34. BLS24479")
print("35. BLS48556")
print("36. BLS48581\n")

print("RSA")
print("37. RSA2048")
print("38. RSA3072")
print("39. RSA4096")


selection=[]
ptr=0
max=40

curve_selected=False
pfcurve_selected=False
rsa_selected=False

while ptr<max:
    if testing :
        x=int(input())
    else :
        x=int(input("Choose a Scheme to support - 0 to finish: "))
    if x == 0:
        break
#    print("Choice= ",x)
    already=False
    for i in range(0,ptr):
        if x==selection[i]:
            already=True
            break
    if already:
        continue

    selection.append(x)
    ptr=ptr+1

# curveset(curve,bits_in_base,modulus_bits,modulus_mod_8,Z,modulus_type,curve_type,pairing_friendly,g2_table size,ate_bits,curve security)
# where "curve" is the common name for the elliptic curve
# bits_in_base gives the number base used for 32 bit architectures, as n where the base is 2^n
# modulus_bits is the actual bit length of the modulus.
# modulus_mod_8 is the remainder when the modulus is divided by 8
# rz Z value for hash_to_point, If list G1 Z value is in [0], G2 Z value (=a+bz) is in [1], [2]
# modulus_type is NOT_SPECIAL, or PSEUDO_MERSENNE, or MONTGOMERY_Friendly, or GENERALISED_MERSENNE (supported for GOLDILOCKS only)
# i for Fp2 QNR 2^i+sqrt(-1) (relevant for PFCs only, else =0). Or QNR over Fp if p=1 mod 8
# curve_type is WEIERSTRASS, EDWARDS or MONTGOMERY
# Curve A parameter
# pairing_friendly is BN, BLS or NOT (if not pairing friendly
# ate bits is number of bits in Ate parameter (from romgen program)
# g2_table size is number of entries in precomputed table
# curve security is AES equiavlent, rounded up.


    if x==1:
        curveset("ED25519","29","255","2","1","PSEUDO_MERSENNE","0","EDWARDS","-1","NOT","NOT","NOT","NOT","NOT","128")
        curve_selected=True
    if x==2:
        curveset("C25519","29","255","2","1","PSEUDO_MERSENNE","0","MONTGOMERY","486662","NOT","NOT","NOT","NOT","NOT","128")
        curve_selected=True
    if x==3:
        curveset("NIST256","28","256","1","-10","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","128")
        curve_selected=True
    if x==4:
        curveset("BRAINPOOL","28","256","1","-3","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","128")
        curve_selected=True
    if x==5:
        curveset("ANSSI","28","256","1","5","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","128")
        curve_selected=True

    if x==6:
        curveset("HIFIVE","29","336","2","1","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","NOT","192")
        curve_selected=True
    if x==7:
        curveset("GOLDILOCKS","29","448","1","0","GENERALISED_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","NOT","256")
        curve_selected=True
    if x==8:
        curveset("NIST384","29","384","1","-12","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","192")
        curve_selected=True
    if x==9:
        curveset("C41417","29","414","1","1","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","NOT","256")
        curve_selected=True
    if x==10:
        curveset("NIST521","28","521","1","-4","PSEUDO_MERSENNE","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","256")
        curve_selected=True

    if x==11:
        curveset("NUMS256W","28","256","1","7","PSEUDO_MERSENNE","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","128")
        curve_selected=True
    if x==12:
        curveset("NUMS256E","29","256","1","0","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","NOT","128")
        curve_selected=True
    if x==13:
        curveset("NUMS384W","29","384","1","-4","PSEUDO_MERSENNE","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","192")
        curve_selected=True
    if x==14:
        curveset("NUMS384E","29","384","1","0","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","NOT","192")
        curve_selected=True
    if x==15:
        curveset("NUMS512W","29","512","1","-4","PSEUDO_MERSENNE","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","256")
        curve_selected=True
    if x==16:
        curveset("NUMS512E","29","512","1","0","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","NOT","256")
        curve_selected=True

    if x==17:
        curveset("SECP256K1","28","256","1","1","NOT_SPECIAL","0","WEIERSTRASS","0","NOT","NOT","NOT","NOT","NOT","128")
        curve_selected=True
    if x==18:
        curveset("SM2","28","256","1","-9","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","128")
        curve_selected=True

    if x==19:
        curveset("C13318","29","255","2","2","PSEUDO_MERSENNE","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","128")
        curve_selected=True

    if x==20:
        curveset("JUBJUB","29","255","32","1","NOT_SPECIAL","5","EDWARDS","-1","NOT","NOT","NOT","NOT","NOT","128")
        curve_selected=True

    if x==21:
        curveset("X448","29","448","1","0","GENERALISED_MERSENNE","0","MONTGOMERY","156326","NOT","NOT","NOT","NOT","NOT","256")
        curve_selected=True

    if x==22:
        curveset("SECP160R1","29","160","1","3","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","128")
        curve_selected=True
    if x==23:
        curveset("C1174","29","251","1","0","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","NOT","128")
        curve_selected=True
    if x==24:
        curveset("C1665","29","166","1","0","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","NOT","128")
        curve_selected=True

    if x==25:
        curveset("MDC","28","256","1","0","NOT_SPECIAL","0","EDWARDS","1","NOT","NOT","NOT","NOT","NOT","128")
        curve_selected=True


    if x==26:
        curveset("BN254","28","254","1",["-1","-1"],"NOT_SPECIAL","0","WEIERSTRASS","0","BN","D_TYPE","NEGATIVEX","71","66","128")
        pfcurve_selected=True
    if x==27:
        curveset("BN254CX","28","254","1",["-1","-1"],"NOT_SPECIAL","0","WEIERSTRASS","0","BN","D_TYPE","NEGATIVEX","76","66","128")
        pfcurve_selected=True
    if x==28:
        curveset("BLS12383","29","383","1",["1","1"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS","M_TYPE","POSITIVEX","68","65","128")
        pfcurve_selected=True

    if x==29:
        curveset("BLS12381","29","381","1",["-3","-1"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS","M_TYPE","NEGATIVEX","69","65","128")
        pfcurve_selected=True

    if x==30:
        curveset("FP256BN","28","256","1",["1","1"],"NOT_SPECIAL","0","WEIERSTRASS","0","BN","M_TYPE","NEGATIVEX","83","66","128")
        pfcurve_selected=True
    if x==31:
        curveset("FP512BN","29","512","1",["1","1"],"NOT_SPECIAL","0","WEIERSTRASS","0","BN","M_TYPE","POSITIVEX","172","130","128")
        pfcurve_selected=True
# https://eprint.iacr.org/2017/334.pdf
    if x==32:
        curveset("BLS12461","28","461","1",["1","4"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS","M_TYPE","NEGATIVEX","79","78","128")
        pfcurve_selected=True

    if x==33:
        curveset("BN462","28","462","1",["1","1"],"NOT_SPECIAL","1","WEIERSTRASS","0","BN","D_TYPE","POSITIVEX","125","118","128")
        pfcurve_selected=True

    if x==34:
        curveset("BLS24479","29","479","1",["1","4"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS","M_TYPE","POSITIVEX","52","49","192")
        pfcurve_selected=True

    if x==35:
        curveset("BLS48556","29","556","1",["-1","2"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS","M_TYPE","POSITIVEX","35","32","256")
        pfcurve_selected=True


    if x==36:
        curveset("BLS48581","29","581","1",["2","2"],"NOT_SPECIAL","10","WEIERSTRASS","0","BLS","D_TYPE","NEGATIVEX","36","33","256")
        pfcurve_selected=True

# rsaset(rsaname,big_length_bytes,bits_in_base,multiplier)
# The RSA name reflects the modulus size, which is a 2^m multiplier
# of the underlying big length

# There are choices here, different ways of getting the same result, but some faster than others
    if x==37:
        #256 is slower but may allow reuse of 256-bit BIGs used for elliptic curve
        #512 is faster.. but best is 1024
        rsaset("RSA2048","128","28","2")
        #rsaset("RSA2048","64","29","4")
        #rsaset("RSA2048","32","29","8")
        rsa_selected=True
    if x==38:
        rsaset("RSA3072","48","28","8")
        rsa_selected=True
    if x==39:
        #rsaset("RSA4096","32","29","16")
        rsaset("RSA4096","64","29","8")
        rsa_selected=True

if testing :
    os.system("go run TestECC.go")
    os.system("go run TestMPIN.go < pins.txt")
    os.system("go run TestBLS.go")
    os.system("go run BenchtestALL.go")
    os.system("go run TestNHS.go")
