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
if sys.platform.startswith("win") :
    copytext=">NUL copy "
    deltext="del "
    slashtext="\\"

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

    fpath="core"+slashtext+tb+slashtext
    os.system("mkdir core"+slashtext+tb)

    os.system(copytext+"ARCH64.go "+fpath+"ARCH.go")
    os.system(copytext+"BIG64.go "+fpath+"BIG.go")
    os.system(copytext+"DBIG.go "+fpath+"DBIG.go")
    os.system(copytext+"FF64.go "+fpath+"FF.go")
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

# curveset(curve,bits_in_base,modulus_bits,modulus_mod_8,Z,modulus_type,curve_type,Curve A,pairing_friendly,sextic twist,sign of x,g2_table size,ate bits,curve security)
# where "curve" is the common name for the elliptic curve
# bits_in_base gives the number base used for 64 bit architectures, as n where the base is 2^n
# modulus_bits is the actual bit length of the modulus.
# modulus_mod_8 is the remainder when the modulus is divided by 8
# rz Z value for hash_to_point, If list G1 Z value is in [0], G2 Z value (=a+bz) is in [1], [2]
# modulus_type is NOT_SPECIAL, or PSEUDO_MERSENNE, or MONTGOMERY_Friendly, or GENERALISED_MERSENNE (supported for GOLDILOCKS only)
# i for Fp2 QNR 2^i+sqrt(-1) (relevant for PFCs only, else =0). Or QNR over Fp if p=1 mod 8
# curve_type is WEIERSTRASS, EDWARDS or MONTGOMERY
# Curve A parameter
# pairing_friendly is BN, BLS or NOT (if not pairing friendly
# g2_table size is number of entries in precomputed table
# ate bits is number of bits in Ate parameter (from romgen program)
# curve security is AES equiavlent, rounded up.

def curveset(tc,base,nbt,m8,rz,mt,qi,ct,ca,pf,stw,sx,g2,ab,cs) :
    global deltext,slashtext,copytext

    inbt=int(nbt)
    itb=int(inbt+(8-inbt%8)%8)
    inb=int(itb/8)
    nb=str(inb)

    fpath="core"+slashtext+tc+slashtext
    os.system("mkdir core"+slashtext+tc)

    os.system(copytext+"ARCH64.go "+fpath+"ARCH.go")
    os.system(copytext+"BIG64.go "+fpath+"BIG.go")
    os.system(copytext+"DBIG.go "+fpath+"DBIG.go")
    os.system(copytext+"FP.go "+fpath+"FP.go")
    os.system(copytext+"ECP.go "+fpath+"ECP.go")
    os.system(copytext+"ECDH.go "+fpath+"ECDH.go")
    os.system(copytext+"HPKE.go "+fpath+"HPKE.go")
    os.system(copytext+"CONFIG_BIG.go "+fpath+"CONFIG_BIG.go")
    os.system(copytext+"CONFIG_FIELD.go "+fpath+"CONFIG_FIELD.go")
    os.system(copytext+"CONFIG_CURVE.go "+fpath+"CONFIG_CURVE.go")
    os.system(copytext+"ROM_"+tc+"_64.go "+fpath+"ROM.go")

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

    hc="0"
    hc2="0"
# Get Hash-to-Curve Z for G1 and G2

    if isinstance(rz,list) :
        if len(rz)==2 :     # Z followed by SSWU isogeny degree
            replace(fpath+"CONFIG_FIELD.go","@RZ@",rz[0])
            replace(fpath+"CONFIG_FIELD.go","@RZ2A@","0")
            replace(fpath+"CONFIG_FIELD.go","@RZ2B@","0")
            hc=rz[1]
        if len(rz)==3 :     # Z for G1 followed by Z for G2 (for SVDW)
            replace(fpath+"CONFIG_FIELD.go","@RZ@",rz[0])
            replace(fpath+"CONFIG_FIELD.go","@RZ2A@",rz[1])
            replace(fpath+"CONFIG_FIELD.go","@RZ2B@",rz[2])
        if len(rz)==5 :     # Z for G1, Z for G2, SSWU isogeny degree for G1, SSWU isogeny degree for G2
            replace(fpath+"CONFIG_FIELD.go","@RZ@",rz[0])
            replace(fpath+"CONFIG_FIELD.go","@RZ2A@",rz[1])
            replace(fpath+"CONFIG_FIELD.go","@RZ2B@",rz[2])
            hc=rz[3]
            hc2=rz[4]
    else :
        replace(fpath+"CONFIG_FIELD.go","@RZ@",rz)   # just Z for SSWU, or indicates RFC7748 or Generic for Elligator
        replace(fpath+"CONFIG_FIELD.go","@RZ2A@","0")
        replace(fpath+"CONFIG_FIELD.go","@RZ2B@","0")

    if hc!="0" :
        replace(fpath+"ECP.go","CAHCZS","*/")
        replace(fpath+"ECP.go","CAHCZF","/*")

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
    if sh > 30 :
        sh=30
    replace(fpath+"CONFIG_FIELD.go","@SH@",str(sh))


    replace(fpath+"CONFIG_CURVE.go","@CT@",ct)
    replace(fpath+"CONFIG_CURVE.go","@CA@",ca)
    replace(fpath+"CONFIG_CURVE.go","@PF@",pf)

    replace(fpath+"CONFIG_CURVE.go","@ST@",stw)
    replace(fpath+"CONFIG_CURVE.go","@SX@",sx)
    replace(fpath+"CONFIG_CURVE.go","@AB@",ab)
    replace(fpath+"CONFIG_CURVE.go","@G2@",g2)

    replace(fpath+"CONFIG_CURVE.go","@HC@",hc) 
    replace(fpath+"CONFIG_CURVE.go","@HC2@",hc2) 

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

        if pf == "BN" or pf == "BLS12" :

            os.system(copytext+"ECP2.go "+fpath+"ECP2.go")
            os.system(copytext+"FP12.go "+fpath+"FP12.go")
            os.system(copytext+"PAIR.go "+fpath+"PAIR.go")
            os.system(copytext+"MPIN.go "+fpath+"MPIN.go")
            os.system(copytext+"BLS.go "+fpath+"BLS.go")

            if hc2!="0" :
                replace(fpath+"ECP2.go","CAHCZS","*/")
                replace(fpath+"ECP2.go","CAHCZF","/*")
            else :
                replace(fpath+"ECP2.go","CAHCNZS","*/")
                replace(fpath+"ECP2.go","CAHCNZF","/*")

            replace(fpath+"FP12.go","XXX",tc)
            replace(fpath+"ECP2.go","XXX",tc)
            replace(fpath+"PAIR.go","XXX",tc)
            replace(fpath+"MPIN.go","XXX",tc)
            replace(fpath+"BLS.go","XXX",tc)

            if pf == "BN" :
                replace(fpath+"PAIR.go","PFBNS","*/")
                replace(fpath+"PAIR.go","PFBNF","/*")

        if pf == "BLS24" :
            os.system(copytext+"FP24.go "+fpath+"FP24.go")
            os.system(copytext+"FP8.go "+fpath+"FP8.go")
            os.system(copytext+"ECP4.go "+fpath+"ECP4.go")
            os.system(copytext+"PAIR4.go "+fpath+"PAIR4.go")
            os.system(copytext+"MPIN192.go "+fpath+"MPIN192.go")
            os.system(copytext+"BLS192.go "+fpath+"BLS192.go")

            replace(fpath+"FP4.go","PFGE24S","*/")
            replace(fpath+"FP4.go","PFGE24F","/*")
            replace(fpath+"FP24.go","XXX",tc)
            replace(fpath+"FP8.go","XXX",tc)
            replace(fpath+"ECP4.go","XXX",tc)
            replace(fpath+"PAIR4.go","XXX",tc)
            replace(fpath+"MPIN192.go","XXX",tc)
            replace(fpath+"BLS192.go","XXX",tc)

        if pf == "BLS48" :
            os.system(copytext+"FP48.go "+fpath+"FP48.go")
            os.system(copytext+"FP16.go "+fpath+"FP16.go")
            os.system(copytext+"FP8.go "+fpath+"FP8.go")
            os.system(copytext+"ECP8.go "+fpath+"ECP8.go")
            os.system(copytext+"PAIR8.go "+fpath+"PAIR8.go")
            os.system(copytext+"MPIN256.go "+fpath+"MPIN256.go")
            os.system(copytext+"BLS256.go "+fpath+"BLS256.go")

            replace(fpath+"FP4.go","PFGE24S","*/")
            replace(fpath+"FP4.go","PFGE24F","/*")
            replace(fpath+"FP8.go","PFGE48S","*/")
            replace(fpath+"FP8.go","PFGE48F","/*")
            replace(fpath+"FP48.go","XXX",tc)
            replace(fpath+"FP16.go","XXX",tc)
            replace(fpath+"FP8.go","XXX",tc)
            replace(fpath+"ECP8.go","XXX",tc)
            replace(fpath+"PAIR8.go","XXX",tc)
            replace(fpath+"MPIN256.go","XXX",tc)
            replace(fpath+"BLS256.go","XXX",tc)

os.system("mkdir core")
os.system(copytext+ "HASH*.go core"+slashtext+".")
os.system(copytext+ "HMAC.go core"+slashtext+".")
os.system(copytext+ "SHA3.go core"+slashtext+".")
os.system(copytext+ "RAND.go core"+slashtext+".")
os.system(copytext+ "SHARE.go core"+slashtext+".")
os.system(copytext+ "AES.go core"+slashtext+".")
os.system(copytext+ "GCM.go core"+slashtext+".")
os.system(copytext+ "NHS.go core"+slashtext+".")

class miracl_crypto:
    np_curves = (
        ("ED25519","56","255","2","1","PSEUDO_MERSENNE","0","EDWARDS","-1","NOT","NOT","NOT","NOT","NOT","128"),
        ("C25519","56","255","2","1","PSEUDO_MERSENNE","0","MONTGOMERY","486662","NOT","NOT","NOT","NOT","NOT","128"),
        ("NIST256","56","256","1","-10","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","128"),
        ("BRAINPOOL","56","256","1","-3","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","128"),
        ("ANSSI","56","256","1","-5","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","128"),
        ("HIFIVE","60","336","2","1","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","NOT","192"),
        ("GOLDILOCKS","58","448","1","0","GENERALISED_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","NOT","256"),
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
        ("BLS48556","58","556","1",["-1","2","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS48","M_TYPE","POSITIVEX","35","42","256"),
        ("BLS48581","60","581","1",["2","2","0"],"NOT_SPECIAL","10","WEIERSTRASS","0","BLS48","D_TYPE","NEGATIVEX","36","33","256"),
        ("BLS48286","60","286","1",["1","1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS48","M_TYPE","POSITIVEX","20","17","128")
    )

    rsa_params = (
        ("RSA2048","128","58","2"),
        ("RSA3072","48","56","8"),
        ("RSA4096","64","60","8")
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

