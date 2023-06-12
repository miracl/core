#!/usr/bin/env python3

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

testing=False
keep_querying=True

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

corepath = "core" + slashtext + "src" + slashtext + "main" + slashtext + "java" + slashtext + org1text + slashtext + org2text + slashtext + "core"
coreTestPath = "core" + slashtext + "src" + slashtext + "test" + slashtext + "java" + slashtext + org1text + slashtext + org2text + slashtext +  "core"

testing=False
if len(sys.argv)==2 :
    if sys.argv[1]=="test":
        testing=True

def replace(namefile,oldtext,newtext):
    f = open(namefile,'r')
    filedata = f.read()
    f.close()

    newdata = filedata.replace(oldtext,newtext)

    f = open(namefile,'w')
    f.write(newdata)
    f.close()

# rsaset(rsaname,big_length_bytes,bits_in_base,multiplier)
# The RSA name reflects the modulus size, which is a 2^m multiplier
# of the underlying big length
# There are choices here, different ways of getting the same result, but some faster than others
def rsaset(tb,nb,base,ml) :
    global deltext,slashtext,copytext

    fpath=corepath+slashtext+tb+slashtext
    fpathTest=coreTestPath+slashtext+tb+slashtext #ms
    os.system(makedir+corepath+slashtext+tb)
    os.system(makedir+coreTestPath+slashtext+tb) #ms

    os.system(copytext+"CONFIG_BIG.java "+fpath+"CONFIG_BIG.java")
    os.system(copytext+"CONFIG_FF.java "+fpath+"CONFIG_FF.java")
    os.system(copytext+"BIG64.java "+fpath+"BIG.java")
    os.system(copytext+"DBIG64.java "+fpath+"DBIG.java")
    os.system(copytext+"FF64.java "+fpath+"FF.java")
    os.system(copytext+"RSA.java "+fpath+"RSA.java")
    os.system(copytext+"private_key.java "+fpath+"private_key.java")
    os.system(copytext+"public_key.java "+fpath+"public_key.java")
    os.system(copytext+"TestRSA.java "+fpathTest+"TestRSA.java") #ms
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

# curveset(curve,big_length_bytes,bits_in_base,modulus_bits,modulus_mod_8,Z,modulus_type,curve_type,Curve A,pairing_friendly,sextic twist,sign of x,g2_table size,ate bits,curve security)
# where "curve" is the common name for the elliptic curve
# big_length_bytes is the modulus size rounded up to a number of bytes
# bits_in_base gives the number base used for 64 bit architectures, as n where the base is 2^n
# modulus_bits is the actual bit length of the modulus.
# modulus_mod_8 is the remainder when the modulus is divided by 8
# rz Z value for hash_to_point, If list G1 Z value is in [0], G2 Z value (=a+bz) is in [1], [2]
# modulus_type is NOT_SPECIAL, or PSEUDO_MERSENNE, or MONTGOMERY_Friendly, or GENERALISED_MERSENNE (supported for ED448 only)
# i for Fp2 QNR 2^i+sqrt(-1) (relevant for PFCs only, else =0). Or QNR over Fp if p=1 mod 8
# curve_type is WEIERSTRASS, EDWARDS or MONTGOMERY
# Curve A parameter
# pairing_friendly is BN, BLS or NOT (if not pairing friendly)
# if pairing friendly. M or D type twist, and sign of the family parameter x
# g2_table size is number of entries in precomputed table
# ate bits is number of bits in Ate parameter (from romgen program)
# curve security is AES equivalent, rounded up.
def curveset(tc,base,nbt,m8,rz,mt,qi,ct,ca,pf,stw,sx,g2,ab,cs) :
    global deltext,slashtext,copytext

    inbt=int(nbt)
    itb=int(inbt+(8-inbt%8)%8)
    inb=int(itb/8)
    nb=str(inb)

    tcu=tc.upper()

    fpath=corepath+slashtext+tcu+slashtext
    fpathTest=coreTestPath+slashtext+tcu+slashtext  #ms
    os.system(makedir+corepath+slashtext+tcu)
    os.system(makedir+coreTestPath+slashtext+tcu)  #ms

    os.system(copytext+"CONFIG_BIG.java "+fpath+"CONFIG_BIG.java")
    os.system(copytext+"CONFIG_FIELD.java "+fpath+"CONFIG_FIELD.java")
    os.system(copytext+"CONFIG_CURVE.java "+fpath+"CONFIG_CURVE.java")
    os.system(copytext+"BIG64.java "+fpath+"BIG.java")
    os.system(copytext+"DBIG64.java "+fpath+"DBIG.java")
    os.system(copytext+"FP64.java "+fpath+"FP.java")
    os.system(copytext+"ECP.java "+fpath+"ECP.java")
    os.system(copytext+"ECDH.java "+fpath+"ECDH.java")
    os.system(copytext+"EDDSA.java "+fpath+"EDDSA.java")
    os.system(copytext+"HPKE.java "+fpath+"HPKE.java")
    os.system(copytext+"ROM_"+tcu+"_64.java "+fpath+"ROM.java")
    os.system(copytext+"TestECDH.java "+fpathTest+"TestECDH.java")    #ms
    os.system(copytext+"TestEDDSA.java "+fpathTest+"TestEDDSA.java")    #ms
    os.system(copytext+"TestHPKE.java "+fpathTest+"TestHPKE.java")    #ms
    os.system(copytext+"TestHTP.java "+fpathTest+"TestHTP.java")    #ms
    os.system(copytext+"TesttimeECDH.java "+fpathTest+"TesttimeECDH.java")    #ms

    replace(fpath+"CONFIG_BIG.java","XXX",tcu)
    replace(fpath+"CONFIG_FIELD.java","XXX",tcu)
    replace(fpath+"CONFIG_CURVE.java","XXX",tcu)
    replace(fpath+"BIG.java","XXX",tcu)
    replace(fpath+"DBIG.java","XXX",tcu)
    replace(fpath+"FP.java","XXX",tcu)
    replace(fpath+"ECP.java","XXX",tcu)
    replace(fpath+"ECDH.java","XXX",tcu)
    replace(fpath+"EDDSA.java","XXX",tcu)
    replace(fpath+"EDDSA.java","Xxx",tc)
    replace(fpath+"HPKE.java","XXX",tcu)
    replace(fpathTest+"TestECDH.java","XXX",tcu)  #ms
    replace(fpathTest+"TestEDDSA.java","XXX",tcu)  #ms
    replace(fpathTest+"TestHPKE.java","XXX",tcu)  #ms
    replace(fpathTest+"TestHTP.java","XXX",tcu)  #ms
    replace(fpathTest+"TesttimeECDH.java","XXX",tcu)  #ms

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
    if sh > 30 :
        sh=30
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

        replace(fpath+"FP2.java","XXX",tcu)
        replace(fpath+"FP4.java","XXX",tcu)

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

            replace(fpath+"FP12.java","XXX",tcu)
            replace(fpath+"ECP2.java","XXX",tcu)
            replace(fpath+"PAIR.java","XXX",tcu)
            replace(fpath+"MPIN.java","XXX",tcu)
            replace(fpath+"BLS.java","XXX",tcu)
            replace(fpathTest+"TestMPIN.java","XXX",tcu)  #ms
            replace(fpathTest+"TestHTP2.java","XXX",tcu)  #ms
            replace(fpathTest+"TestBLS.java","XXX",tcu)  #ms
            replace(fpathTest+"TesttimeMPIN.java","XXX",tcu)  #ms

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
            replace(fpath+"FP8.java","XXX",tcu)
            replace(fpath+"FP24.java","XXX",tcu)
            replace(fpath+"ECP4.java","XXX",tcu)
            replace(fpath+"PAIR4.java","XXX",tcu)
            replace(fpath+"MPIN192.java","XXX",tcu)
            replace(fpath+"BLS192.java","XXX",tcu)
            replace(fpathTest+"TestMPIN192.java","XXX",tcu)  #ms
            replace(fpathTest+"TestBLS192.java","XXX",tcu)  #ms
            replace(fpathTest+"TesttimeMPIN192.java","XXX",tcu)  #ms

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
            replace(fpath+"FP8.java","XXX",tcu)
            replace(fpath+"FP16.java","XXX",tcu)
            replace(fpath+"FP48.java","XXX",tcu)
            replace(fpath+"ECP8.java","XXX",tcu)
            replace(fpath+"PAIR8.java","XXX",tcu)
            replace(fpath+"MPIN256.java","XXX",tcu)
            replace(fpath+"BLS256.java","XXX",tcu)
            replace(fpathTest+"TestMPIN256.java","XXX",tcu)  #ms
            replace(fpathTest+"TestBLS256.java","XXX",tcu)  #ms
            replace(fpathTest+"TesttimeMPIN256.java","XXX",tcu)  #ms



os.system(makedir + corepath)

os.system(copytext + "pom.xml " + "core" + slashtext + ".")
for file in ['HASH*.java', 'HMAC.java', 'SHA3.java', 'RAND.java', 'AES.java', 'GCM.java', 'NHS.java', 'KYBER.java', 'DILITHIUM.java','SHARE.java']:
    os.system(copytext + file + " " + corepath+slashtext+".")

class miracl_crypto:
    np_curves = (
        ("Ed25519","56","255","2","1","PSEUDO_MERSENNE","0","EDWARDS","-1","NOT","NOT","NOT","NOT","NOT","128"),
        ("C25519","56","255","2","1","PSEUDO_MERSENNE","0","MONTGOMERY","486662","NOT","NOT","NOT","NOT","NOT","128"),
        ("NIST256","56","256","1","-10","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","128"),
        ("BRAINPOOL","56","256","1","-3","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","128"),
        ("ANSSI","56","256","1","-5","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","128"),
        ("HIFIVE","60","336","2","1","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","NOT","192"),
        ("Ed448","58","448","1","0","GENERALISED_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","NOT","256"),
        ("NIST384","56","384","1","-12","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","192"),
        ("C41417","60","414","1","1","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","NOT","256"),
        ("NIST521","60","521","1","-4","PSEUDO_MERSENNE","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","256"),
        ("NUMS256W","56","256","1","7","PSEUDO_MERSENNE","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","128"),
        ("NUMS256E","56","256","1","0","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","NOT","128"),
        ("NUMS384W","58","384","1","-4","PSEUDO_MERSENNE","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","192"),
        ("NUMS384E","56","384","1","0","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","NOT","192"),
        ("NUMS512W","60","512","1","-4","PSEUDO_MERSENNE","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","256"),
        ("NUMS512E","60","512","1","0","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","NOT","256"),
        ("SECP256K1","56","256","1",["-11","3"],"NOT_SPECIAL","0","WEIERSTRASS","0","NOT","NOT","NOT","NOT","NOT","128"),
        ("SM2","56","256","1","-9","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","128"),
        ("C13318","56","255","2","2","PSEUDO_MERSENNE","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","128"),
        ("JUBJUB","56","255","32","1","NOT_SPECIAL","5","EDWARDS","-1","NOT","NOT","NOT","NOT","NOT","128"),
        ("X448","58","448","1","0","GENERALISED_MERSENNE","0","MONTGOMERY","156326","NOT","NOT","NOT","NOT","NOT","256"),
        ("SECP160R1","56","160","1","3","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","128"),
        ("C1174","56","251","1","0","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","NOT","128"),
        ("C1665","60","166","1","0","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","NOT","128"),
        ("MDC","56","256","1","0","NOT_SPECIAL","0","EDWARDS","1","NOT","NOT","NOT","NOT","NOT","128"),
        ("TWEEDLEDUM","56","255","33","1","NOT_SPECIAL","5","WEIERSTRASS","0","NOT","NOT","NOT","NOT","NOT","128"),
        ("TWEEDLEDEE","56","255","34","1","NOT_SPECIAL","5","WEIERSTRASS","0","NOT","NOT","NOT","NOT","NOT","128")
    )

    pf_curves = (
        ("BN254","56","254","1",["-1","-1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BN","D_TYPE","NEGATIVEX","71","66","128"),
        ("BN254CX","56","254","1",["-1","-1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BN","D_TYPE","NEGATIVEX","76","66","128"),
        ("BLS12383","58","383","1",["1","1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS12","M_TYPE","POSITIVEX","68","65","128"),
        ("BLS12381","58","381","1",["11","-2","-1","11","3"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS12","M_TYPE","NEGATIVEX","69","65","128"), 
        ("FP256BN","56","256","1",["1","1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BN","M_TYPE","NEGATIVEX","83","66","128"),
        ("FP512BN","60","512","1",["1","1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BN","M_TYPE","POSITIVEX","172","130","128"),
        ("BLS12443","60","443","1",["-7","1","1","11","3"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS12","M_TYPE","POSITIVEX","78","75","128"),
        ("BLS12461","60","461","1",["1","4","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS12","M_TYPE","NEGATIVEX","79","78","128"),
        ("BN462","60","462","1",["1","1","0"],"NOT_SPECIAL","1","WEIERSTRASS","0","BN","D_TYPE","POSITIVEX","125","118","128"),
        ("BLS24479","56","479","1",["1","4","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS24","M_TYPE","POSITIVEX","52","49","192"),
        ("BLS48556","58","556","1",["-1","2","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS48","M_TYPE","POSITIVEX","35","32","256"),
        ("BLS48581","60","581","1",["2","2","0"],"NOT_SPECIAL","10","WEIERSTRASS","0","BLS48","D_TYPE","NEGATIVEX","36","33","256"),
        ("BLS48286","60","286","1",["1","1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS48","M_TYPE","POSITIVEX","20","17","128"),
        ("BN158","56","158","1",["1","1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BN","M_TYPE","NEGATIVEX","49","42","128")
    )

    rsa_params = (
        ("RSA2048","128","58","2"),
        ("RSA3072","48","56","8"),
        ("RSA4096","64","60","8"),
    )

    total_entries = len(np_curves)+len(pf_curves)+len(rsa_params)

    def valid_query(number):
        return number >= 0 and number <= miracl_crypto.total_entries

def interactive_prompt_print():
    index = 1
    print("Elliptic Curves")
    for tuple in miracl_crypto.np_curves:
        print(str(index) + ".", tuple[0])
        index += 1

    print("\nPairing-Friendly Elliptic Curves")
    for tuple in miracl_crypto.pf_curves:
        print(str(index) + ".", tuple[0])
        index += 1

    print("\nRSA")
    for tuple in miracl_crypto.rsa_params:
        print(str(index) + ".", tuple[0])
        index += 1

def interactive_prompt_exect(index):
    index -= 1 # Python internally is zero-indexed
    if index < len(miracl_crypto.np_curves):
        tuple = miracl_crypto.np_curves[index]
        curveset(
            tuple[0], tuple[1], tuple[2], tuple[3], tuple[4],
            tuple[5], tuple[6], tuple[7], tuple[8], tuple[9],
            tuple[10], tuple[11], tuple[12],
            tuple[13], tuple[14]
        )
    elif index < len(miracl_crypto.np_curves) + len(miracl_crypto.pf_curves):
        tuple = miracl_crypto.pf_curves[index-len(miracl_crypto.np_curves)]
        curveset(
            tuple[0], tuple[1], tuple[2], tuple[3], tuple[4],
            tuple[5], tuple[6], tuple[7], tuple[8], tuple[9],
            tuple[10], tuple[11], tuple[12],
            tuple[13], tuple[14]
        )
    else:
        tuple = miracl_crypto.rsa_params[index-(len(miracl_crypto.np_curves)+len(miracl_crypto.pf_curves))]
        rsaset(
            tuple[0], tuple[1], tuple[2], tuple[3]
        )

def interactive_prompt_input():
    while True:
        userInput = input("\nChoose schemes to support (select 0 to finish): ")
        try:
            return int(userInput)
        except:
            if (userInput == ''):
                return 0
            print("Non-integer input, select values between 1 and " + str(miracl_crypto.total_entries))
            interactive_prompt_input()


interactive_prompt_print()
while keep_querying and not testing:
    query_val = -1
    while not miracl_crypto.valid_query(query_val):
        query_val = interactive_prompt_input()
        if not miracl_crypto.valid_query(query_val):
            print("Number out of range, select values between 1 and " + str(miracl_crypto.total_entries))
        elif query_val == 0:
            keep_querying = False
        else:
            interactive_prompt_exect(query_val)

if testing:
    for i in range(0, miracl_crypto.total_entries):
        interactive_prompt_exect(i+1)

os.system(copytext+"TestNHS.java "+coreTestPath+slashtext+"TestNHS.java")
os.system(copytext+"TestKYBER.java "+coreTestPath+slashtext+"TestKYBER.java")
os.system(copytext+"TestDLTHM.java "+coreTestPath+slashtext+"TestDLTHM.java")

if testing :
    os.chdir("core")
    os.system("mvn clean install")

