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
makedir="mkdir -p "
org1text="org"
org2text="miracl"

if sys.platform.startswith("win") :
    copytext=">NUL copy "
    deltext="del "
    slashtext="\\"
    makedir="md "

corepath = "core" + slashtext + "src" + slashtext + "main" + slashtext + "java" + slashtext + org1text + slashtext + org2text +slashtext + "core"
coreTestPath = "core" + slashtext + "src" + slashtext + "test" + slashtext + "java" + slashtext + org1text + slashtext + org2text + slashtext + "core"
chosen=[]
cptr=0

testing=False
if len(sys.argv)==2 :
    if sys.argv[1]=="test":
        testing=True
if testing :
    sys.stdin=open("test.txt","r")


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

    fpath=corepath+slashtext+tb+slashtext
    fpathTest=coreTestPath+slashtext+tb+slashtext  #ms
    os.system(makedir+corepath+slashtext+tb)
    os.system(makedir+coreTestPath+slashtext+tb)  #ms

    os.system(copytext+"CONFIG_BIG.java "+fpath+"CONFIG_BIG.java")
    os.system(copytext+"CONFIG_FF.java "+fpath+"CONFIG_FF.java")
    os.system(copytext+"BIG32.java "+fpath+"BIG.java")
    os.system(copytext+"DBIG32.java "+fpath+"DBIG.java")
    os.system(copytext+"FF32.java "+fpath+"FF.java")
    os.system(copytext+"RSA.java "+fpath+"RSA.java")
    os.system(copytext+"private_key.java "+fpath+"private_key.java")
    os.system(copytext+"public_key.java "+fpath+"public_key.java")
    os.system(copytext+"TestRSA.java "+fpathTest+"TestRSA.java")    #ms
    os.system(copytext+"TesttimeRSA.java "+fpathTest+"TesttimeRSA.java")    #ms

    replace(fpath+"CONFIG_BIG.java","XXX",tb)
    replace(fpath+"CONFIG_FF.java","XXX",tb)
    replace(fpath+"BIG.java","XXX",tb)
    replace(fpath+"DBIG.java","XXX",tb)
    replace(fpath+"FF.java","XXX",tb)
    replace(fpath+"RSA.java","XXX",tb)
    replace(fpath+"private_key.java","XXX",tb)
    replace(fpath+"public_key.java","XXX",tb)
    replace(fpathTest+"TestRSA.java","XXX",tb)  #ms
    replace(fpathTest+"TesttimeRSA.java","XXX",tb)  #ms

    replace(fpath+"CONFIG_BIG.java","@NB@",nb)
    replace(fpath+"CONFIG_BIG.java","@BASE@",base)

    replace(fpath+"CONFIG_FF.java","@ML@",ml);


def curveset(tc,base,nbt,m8,rz,mt,qi,ct,ca,pf,stw,sx,g2,ab,cs) :

    inbt=int(nbt)
    itb=int(inbt+(8-inbt%8)%8)
    inb=int(itb/8)
    nb=str(inb)

    global deltext,slashtext,copytext
    global cptr,chosen

    chosen.append(tc)
    cptr=cptr+1

    fpath=corepath+slashtext+tc+slashtext
    fpathTest=coreTestPath+slashtext+tc+slashtext  #ms
    os.system(makedir+corepath+slashtext+tc)
    os.system(makedir+coreTestPath+slashtext+tc)  #ms

    os.system(copytext+"CONFIG_BIG.java "+fpath+"CONFIG_BIG.java")
    os.system(copytext+"CONFIG_FIELD.java "+fpath+"CONFIG_FIELD.java")
    os.system(copytext+"CONFIG_CURVE.java "+fpath+"CONFIG_CURVE.java")
    os.system(copytext+"BIG32.java "+fpath+"BIG.java")
    os.system(copytext+"DBIG32.java "+fpath+"DBIG.java")
    os.system(copytext+"FP32.java "+fpath+"FP.java")
    os.system(copytext+"ECP.java "+fpath+"ECP.java")
    os.system(copytext+"ECDH.java "+fpath+"ECDH.java")
    os.system(copytext+"HPKE.java "+fpath+"HPKE.java")
    os.system(copytext+"ROM_"+tc+"_32.java "+fpath+"ROM.java")
    os.system(copytext+"TestECDH.java "+fpathTest+"TestECDH.java")    #ms
    os.system(copytext+"TestHPKE.java "+fpathTest+"TestHPKE.java")    #ms
    os.system(copytext+"TestHTP.java "+fpathTest+"TestHTP.java")    #ms
    os.system(copytext+"TesttimeECDH.java "+fpathTest+"TesttimeECDH.java")    #ms

    replace(fpath+"CONFIG_BIG.java","XXX",tc)
    replace(fpath+"CONFIG_FIELD.java","XXX",tc)
    replace(fpath+"CONFIG_CURVE.java","XXX",tc)
    replace(fpath+"BIG.java","XXX",tc)
    replace(fpath+"DBIG.java","XXX",tc)
    replace(fpath+"FP.java","XXX",tc)
    replace(fpath+"ECP.java","XXX",tc)
    replace(fpath+"ECDH.java","XXX",tc)
    replace(fpath+"HPKE.java","XXX",tc)
    replace(fpathTest+"TestECDH.java","XXX",tc)  #ms
    replace(fpathTest+"TestHPKE.java","XXX",tc)  #ms
    replace(fpathTest+"TestHTP.java","XXX",tc)  #ms
    replace(fpathTest+"TesttimeECDH.java","XXX",tc)  #ms

    replace(fpath+"CONFIG_BIG.java","@NB@",nb)
    replace(fpath+"CONFIG_BIG.java","@BASE@",base)

    replace(fpath+"CONFIG_FIELD.java","@NBT@",nbt)
    replace(fpath+"CONFIG_FIELD.java","@M8@",m8)
    replace(fpath+"CONFIG_FIELD.java","@MT@",mt)

    if ca == "0" :
        replace(fpath+"ECP.java","CAISZS","*/")
        replace(fpath+"ECP.java","CAISZF","/*")

    hc="0"
    hc2="0"
# Get Hash-to-Curve Z for G1 and G2

    if isinstance(rz,list) :
        if len(rz)==2 :     # Z followed by SSWU isogeny degree
            replace(fpath+"CONFIG_FIELD.java","@RZ@",rz[0])
            replace(fpath+"CONFIG_FIELD.java","@RZ2A@","0")
            replace(fpath+"CONFIG_FIELD.java","@RZ2B@","0")
            hc=rz[1]
        if len(rz)==3 :     # Z for G1 followed by Z for G2 (for SVDW)
            replace(fpath+"CONFIG_FIELD.java","@RZ@",rz[0])
            replace(fpath+"CONFIG_FIELD.java","@RZ2A@",rz[1])
            replace(fpath+"CONFIG_FIELD.java","@RZ2B@",rz[2])
        if len(rz)==5 :     # Z for G1, Z for G2, SSWU isogeny degree for G1, SSWU isogeny degree for G2
            replace(fpath+"CONFIG_FIELD.java","@RZ@",rz[0])
            replace(fpath+"CONFIG_FIELD.java","@RZ2A@",rz[1])
            replace(fpath+"CONFIG_FIELD.java","@RZ2B@",rz[2])
            hc=rz[3]
            hc2=rz[4]
    else :
        replace(fpath+"CONFIG_FIELD.java","@RZ@",rz)   # just Z for SSWU, or indicates RFC7748 or Generic for Elligator
        replace(fpath+"CONFIG_FIELD.java","@RZ2A@","0")
        replace(fpath+"CONFIG_FIELD.java","@RZ2B@","0")

    if hc!="0" :
        replace(fpath+"ECP.java","CAHCZS","*/")
        replace(fpath+"ECP.java","CAHCZF","/*")

    itw=int(qi)%10
    replace(fpath+"CONFIG_FIELD.java","@QI@",str(itw))
    if int(qi)//10 > 0 :
        replace(fpath+"CONFIG_FIELD.java","@TW@","POSITOWER")
    else :
        replace(fpath+"CONFIG_FIELD.java","@TW@","NEGATOWER")


    ib=int(base)
    inb=int(nb)
    inbt=int(nbt)
    sh=ib*(1+((8*inb-1)//ib))-inbt
    if sh > 14 :
        sh=14
    replace(fpath+"CONFIG_FIELD.java","@SH@",str(sh))


    replace(fpath+"CONFIG_CURVE.java","@CT@",ct)
    replace(fpath+"CONFIG_CURVE.java","@CA@",ca)
    replace(fpath+"CONFIG_CURVE.java","@PF@",pf)

    replace(fpath+"CONFIG_CURVE.java","@ST@",stw)
    replace(fpath+"CONFIG_CURVE.java","@SX@",sx)
    replace(fpath+"CONFIG_CURVE.java","@AB@",ab)
    replace(fpath+"CONFIG_CURVE.java","@G2@",g2)

    replace(fpath+"CONFIG_CURVE.java","@HC@",hc) 
    replace(fpath+"CONFIG_CURVE.java","@HC2@",hc2) 

    if cs == "128" :
        replace(fpath+"CONFIG_CURVE.java","@HT@","32")
        replace(fpath+"CONFIG_CURVE.java","@AK@","16")
    if cs == "192" :
        replace(fpath+"CONFIG_CURVE.java","@HT@","48")
        replace(fpath+"CONFIG_CURVE.java","@AK@","24")
    if cs == "256" :
        replace(fpath+"CONFIG_CURVE.java","@HT@","64")
        replace(fpath+"CONFIG_CURVE.java","@AK@","32")

    if pf != "NOT" :
        os.system(copytext+"FP2.java "+fpath+"FP2.java")
        os.system(copytext+"FP4.java "+fpath+"FP4.java")

        replace(fpath+"FP2.java","XXX",tc)
        replace(fpath+"FP4.java","XXX",tc)

        if pf == "BN" or pf == "BLS12" :

            os.system(copytext+"ECP2.java "+fpath+"ECP2.java")
            os.system(copytext+"FP12.java "+fpath+"FP12.java")
            os.system(copytext+"PAIR.java "+fpath+"PAIR.java")
            os.system(copytext+"MPIN.java "+fpath+"MPIN.java")
            os.system(copytext+"BLS.java "+fpath+"BLS.java")
            os.system(copytext+"TestMPIN.java "+fpathTest+"TestMPIN.java")    #ms
            os.system(copytext+"TestHTP2.java "+fpathTest+"TestHTP2.java")    #ms
            os.system(copytext+"TestBLS.java "+fpathTest+"TestBLS.java")    #ms
            os.system(copytext+"TesttimeMPIN.java "+fpathTest+"TesttimeMPIN.java")    #ms

            if hc2!="0" :
                replace(fpath+"ECP2.java","CAHCZS","*/")
                replace(fpath+"ECP2.java","CAHCZF","/*")
            else :
                replace(fpath+"ECP2.java","CAHCNZS","*/")
                replace(fpath+"ECP2.java","CAHCNZF","/*")

            replace(fpath+"FP12.java","XXX",tc)
            replace(fpath+"ECP2.java","XXX",tc)
            replace(fpath+"PAIR.java","XXX",tc)
            replace(fpath+"MPIN.java","XXX",tc)
            replace(fpath+"BLS.java","XXX",tc)
            replace(fpathTest+"TestMPIN.java","XXX",tc)  #ms
            replace(fpathTest+"TestHTP2.java","XXX",tc)  #ms
            replace(fpathTest+"TestBLS.java","XXX",tc)  #ms
            replace(fpathTest+"TesttimeMPIN.java","XXX",tc)  #ms

            if pf == "BN" :
                replace(fpath+"PAIR.java","PFBNS","*/")
                replace(fpath+"PAIR.java","PFBNF","/*")

        if pf == "BLS24" :
            os.system(copytext+"ECP4.java "+fpath+"ECP4.java")
            os.system(copytext+"FP8.java "+fpath+"FP8.java")
            os.system(copytext+"FP24.java "+fpath+"FP24.java")
            os.system(copytext+"PAIR4.java "+fpath+"PAIR4.java")
            os.system(copytext+"MPIN192.java "+fpath+"MPIN192.java")
            os.system(copytext+"BLS192.java "+fpath+"BLS192.java")
            os.system(copytext+"TestMPIN192.java "+fpathTest+"TestMPIN192.java")    #ms
            os.system(copytext+"TestBLS192.java "+fpathTest+"TestBLS192.java")    #ms
            os.system(copytext+"TesttimeMPIN192.java "+fpathTest+"TesttimeMPIN192.java")    #ms

            replace(fpath+"FP4.java","PFGE24S","*/")
            replace(fpath+"FP4.java","PFGE24F","/*")
            replace(fpath+"FP8.java","XXX",tc)
            replace(fpath+"FP24.java","XXX",tc)
            replace(fpath+"ECP4.java","XXX",tc)
            replace(fpath+"PAIR4.java","XXX",tc)
            replace(fpath+"MPIN192.java","XXX",tc)
            replace(fpath+"BLS192.java","XXX",tc)
            replace(fpathTest+"TestMPIN192.java","XXX",tc)  #ms
            replace(fpathTest+"TestBLS192.java","XXX",tc)  #ms
            replace(fpathTest+"TesttimeMPIN192.java","XXX",tc)  #ms

        if pf == "BLS48" :

            os.system(copytext+"FP8.java "+fpath+"FP8.java")
            os.system(copytext+"ECP8.java "+fpath+"ECP8.java")
            os.system(copytext+"FP16.java "+fpath+"FP16.java")
            os.system(copytext+"FP48.java "+fpath+"FP48.java")
            os.system(copytext+"PAIR8.java "+fpath+"PAIR8.java")
            os.system(copytext+"MPIN256.java "+fpath+"MPIN256.java")
            os.system(copytext+"BLS256.java "+fpath+"BLS256.java")
            os.system(copytext+"TestMPIN256.java "+fpathTest+"TestMPIN256.java")    #ms
            os.system(copytext+"TestBLS256.java "+fpathTest+"TestBLS256.java")    #ms
            os.system(copytext+"TesttimeMPIN256.java "+fpathTest+"TesttimeMPIN256.java")    #ms

            replace(fpath+"FP4.java","PFGE24S","*/")
            replace(fpath+"FP4.java","PFGE24F","/*")
            replace(fpath+"FP8.java","PFGE48S","*/")
            replace(fpath+"FP8.java","PFGE48F","/*")
            replace(fpath+"FP8.java","XXX",tc)
            replace(fpath+"FP16.java","XXX",tc)
            replace(fpath+"FP48.java","XXX",tc)
            replace(fpath+"ECP8.java","XXX",tc)
            replace(fpath+"PAIR8.java","XXX",tc)
            replace(fpath+"MPIN256.java","XXX",tc)
            replace(fpath+"BLS256.java","XXX",tc)
            replace(fpathTest+"TestMPIN256.java","XXX",tc)  #ms
            replace(fpathTest+"TestBLS256.java","XXX",tc)  #ms
            replace(fpathTest+"TesttimeMPIN256.java","XXX",tc)  #ms


os.system(makedir + corepath)

os.system(copytext + "pom.xml " + "core" + slashtext + ".")
for file in ['HASH*.java', 'HMAC.java', 'SHA3.java', 'RAND.java', 'AES.java', 'GCM.java', 'NHS.java', 'SHARE.java']:
    os.system(copytext + file + " " + corepath+slashtext+".")

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
print("25. Million Dollar Curve")
print("26. TWEEDLEDUM")
print("27. TWEEDLEDEE\n")

print("Pairing-Friendly Elliptic Curves")
print("28. BN254")
print("29. BN254CX")
print("30. BLS12383")
print("31. BLS12381")
print("32. FP256BN")
print("33. FP512BN")
print("34. BLS12461")
print("35. BN462")
print("36. BLS24479")
print("37. BLS48556")
print("38. BLS48581")
print("39. BLS48286\n")

print("RSA")
print("40. RSA2048")
print("41. RSA3072")
print("42. RSA4096")

selection=[]
ptr=0
max=43

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

# curveset(curve,bits_in_base,modulus_bits,modulus_mod_8,Z,modulus_type,curve_type,Curve A,pairing_friendly,sextic twist,sign of x,g2_table size,ate bits,curve security)
# where "curve" is the common name for the elliptic curve
# bits_in_base gives the number base used for 32 bit architectures, as n where the base is 2^n
# modulus_bits is the actual bit length of the modulus.
# modulus_mod_8 is the remainder when the modulus is divided by 8
# rz Z value for hash_to_point, If list G1 Z value is in [0], G2 Z value (=a+bz) is in [1], [2]
# modulus_type is NOT_SPECIAL, or PSEUDO_MERSENNE, or MONTGOMERY_Friendly, or GENERALISED_MERSENNE (supported for GOLDILOCKS only)
# i for Fp2 QNR 2^i+sqrt(-1) (relevant for PFCs only, else =0). Or QNR over Fp if p=1 mod 8
# curve_type is WEIERSTRASS, EDWARDS or MONTGOMERY
# Curve A parameter
# pairing_friendly is BN, BLS or NOT (if not pairing friendly)
# if pairing friendly. M or D type twist, and sign of the family parameter x
# g2_table size is number of entries in precomputed table
# ate bits is number of bits in Ate parameter (from romgen program)
# curve security is AES equivalent, rounded up.


    if x==1:
        curveset("ED25519","29","255","2","1","PSEUDO_MERSENNE","0","EDWARDS","-1","NOT","NOT","NOT","NOT","NOT","128")
        curve_selected=True
    if x==2:
        curveset("C25519","29","255","2","1","PSEUDO_MERSENNE","0","MONTGOMERY","486662","NOT","NOT","NOT","NOT","NOT","128")
        curve_selected=True
    if x==3:
        curveset("NIST256","28","256","1","-10","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","128")   # Change to 28
        curve_selected=True
    if x==4:
        curveset("BRAINPOOL","28","256","1","-3","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","128") # Change to 28
        curve_selected=True
    if x==5:
        curveset("ANSSI","28","256","1","-5","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","128") # Change to 28
        curve_selected=True

    if x==6:
        curveset("HIFIVE","29","336","2","1","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","NOT","192")
        curve_selected=True
    if x==7:
        curveset("GOLDILOCKS","29","448","1","0","GENERALISED_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","NOT","256")
        curve_selected=True
    if x==8:
        curveset("NIST384","29","384","1","-12","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","192")  # change to 29
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
#                                          ,"1", for SVDW
# set for SSWU plus isogenies
        curveset("SECP256K1","28","256","1",["-11","3"],"NOT_SPECIAL","0","WEIERSTRASS","0","NOT","NOT","NOT","NOT","NOT","128")   # Change to 28
        curve_selected=True
    if x==18:
        curveset("SM2","28","256","1","-9","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","128")   # Change to 28
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
        curveset("TWEEDLEDUM","29","255","33","1","NOT_SPECIAL","5","WEIERSTRASS","0","NOT","NOT","NOT","NOT","NOT","128")
        curve_selected=True

    if x==27:
        curveset("TWEEDLEDEE","29","255","34","1","NOT_SPECIAL","5","WEIERSTRASS","0","NOT","NOT","NOT","NOT","NOT","128")
        curve_selected=True


    pf=28

    if x==pf+0:
        curveset("BN254","28","254","1",["-1","-1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BN","D_TYPE","NEGATIVEX","71","66","128")
        pfcurve_selected=True
    if x==pf+1:
        curveset("BN254CX","28","254","1",["-1","-1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BN","D_TYPE","NEGATIVEX","76","66","128")
        pfcurve_selected=True
    if x==pf+2:
        curveset("BLS12383","29","383","1",["1","1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS12","M_TYPE","POSITIVEX","68","65","128")
        pfcurve_selected=True

    if x==pf+3:
#                                          ["-3" ,"-1", "0"]  for SVDW
# set for SSWU plus isogenies
        curveset("BLS12381","29","381","1",["11","-2","-1","11","3"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS12","M_TYPE","NEGATIVEX","69","65","128")
        pfcurve_selected=True


    if x==pf+4:
        curveset("FP256BN","28","256","1",["1","1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BN","M_TYPE","NEGATIVEX","83","66","128")
        pfcurve_selected=True
    if x==pf+5:
        curveset("FP512BN","29","512","1",["1","1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BN","M_TYPE","POSITIVEX","172","130","128")
        pfcurve_selected=True
# https://eprint.iacr.org/2017/334.pdf
    if x==pf+6:
        curveset("BLS12461","28","461","1",["1","4","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS12","M_TYPE","NEGATIVEX","79","78","128")
        pfcurve_selected=True

    if x==pf+7:
        curveset("BN462","28","462","1",["1","1","0"],"NOT_SPECIAL","1","WEIERSTRASS","0","BN","D_TYPE","POSITIVEX","125","118","128")
        pfcurve_selected=True

    if x==pf+8:
        curveset("BLS24479","29","479","1",["1","4","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS24","M_TYPE","POSITIVEX","52","49","192")
        pfcurve_selected=True
    if x==pf+9:
        curveset("BLS48556","29","556","1",["-1","2","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS48","M_TYPE","POSITIVEX","35","32","256")
        pfcurve_selected=True

    if x==pf+10:
        curveset("BLS48581","29","581","1",["2","2","0"],"NOT_SPECIAL","10","WEIERSTRASS","0","BLS48","D_TYPE","NEGATIVEX","36","33","256")
        pfcurve_selected=True

    if x==pf+11:
        curveset("BLS48286","29","286","1",["1","1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS48","M_TYPE","POSITIVEX","20","17","128")
        pfcurve_selected=True


# rsaset(rsaname,big_length_bytes,bits_in_base,multiplier)
# The RSA name reflects the modulus size, which is a 2^m multiplier
# of the underlying big length

# There are choices here, different ways of getting the same result, but some faster than others
    if x==pf+12:
        #256 is slower but may allow reuse of 256-bit BIGs used for elliptic curve
        #512 is faster.. but best is 1024
        rsaset("RSA2048","128","28","2")
        #rsaset("RSA2048","64","29","4")
        #rsaset("RSA2048","32","29","8")
        rsa_selected=True
    if x==pf+13:
        rsaset("RSA3072","48","28","8")
        rsa_selected=True
    if x==pf+14:
        #rsaset("RSA4096","32","29","16")
        rsaset("RSA4096","64","29","8")
        rsa_selected=True

os.system(copytext+"TestNHS.java "+coreTestPath+slashtext+"TestNHS.java")

if testing :
    os.chdir("core")
    os.system("mvn clean install")
