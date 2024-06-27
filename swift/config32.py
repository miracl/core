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
    copytext="copy "
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

    os.system(copytext+"big.swift "+fpath+"big.swift")
    os.system(copytext+"config_big.swift "+fpath+"config_big.swift")
    os.system(copytext+"config_ff.swift "+fpath+"config_ff.swift")
    os.system(copytext+"dbig.swift "+fpath+"dbig.swift")
    os.system(copytext+"ff.swift "+fpath+"ff.swift")
    os.system(copytext+"rsa.swift "+fpath+"rsa.swift")

    replace(fpath+"config_big.swift","@NB@",nb)
    replace(fpath+"config_big.swift","@BASE32@",base)
    replace(fpath+"config_big.swift","@BASE64@",base)

    replace(fpath+"config_ff.swift","@ML@",ml);

    print("Processing "+tb+ "..",end = "",flush=True)
    os.system("swiftc -DD32 "+fpath+"*.swift -L. -lcore -I. -O -Ounchecked -whole-module-optimization -emit-library -emit-module -module-name "+tb)
    os.system(deltext+fpath+"*.*")
    os.system("rmdir core"+slashtext+tb)
    print(". [DONE]")

# curveset(curve,bits_in_base,modulus_bits,modulus_mod_8,Z,modulus_type,curve_type,Curve A,pairing_friendly,ate_bits,curve security)
# where "curve" is the common name for the elliptic curve
# bits_in_base gives the number base used for 32 bit architectures, as n where the base is 2^n
# modulus_bits is the actual bit length of the modulus.
# modulus_mod_8 is the remainder when the modulus is divided by 8
# rz Z value for hash_to_point, If list G1 Z value is in [0], G2 Z value (=a+bz) is in [1], [2]
# modulus_type is NOT_SPECIAL, or PSEUDO_MERSENNE, or MONTGOMERY_Friendly, or GENERALISED_MERSENNE (supported for Ed448 only)
# i for Fp2 QNR 2^i+sqrt(-1) (relevant for PFCs only, else =0). Or QNR over Fp if p=1 mod 8
# curve_type is WEIERSTRASS, EDWARDS or MONTGOMERY
# Curve A parameter
# pairing_friendly is BN, BLS or NOT (if not pairing friendly)
# if pairing friendly. M or D type twist, and sign of the family parameter x
# ate bits is number of bits in Ate parameter (from romgen program)
# curve security is AES equivalent, rounded up.
def curveset(tc,base,nbt,m8,rz,mt,qi,ct,ca,pf,stw,sx,ab,cs) :
    global deltext,slashtext,copytext

    inbt=int(nbt)
    itb=int(inbt+(8-inbt%8)%8)
    inb=int(itb/8)
    nb=str(inb)

    tcl=tc.lower()

    fpath="core"+slashtext+tcl+slashtext
    os.system("mkdir core"+slashtext+tcl)

    os.system(copytext+"big.swift "+fpath+"big.swift")
    os.system(copytext+"config_big.swift "+fpath+"config_big.swift")
    os.system(copytext+"config_field.swift "+fpath+"config_field.swift")
    os.system(copytext+"config_curve.swift "+fpath+"config_curve.swift")
    os.system(copytext+"dbig.swift "+fpath+"dbig.swift")
    os.system(copytext+"fp.swift "+fpath+"fp.swift")
    os.system(copytext+"ecp.swift "+fpath+"ecp.swift")
    os.system(copytext+"rom_"+tcl+".swift "+fpath+"rom.swift")

    replace(fpath+"config_big.swift","@NB@",nb)
    replace(fpath+"config_big.swift","@BASE32@",base)
    replace(fpath+"config_big.swift","@BASE64@",base)

    replace(fpath+"config_field.swift","@NBT@",nbt)
    replace(fpath+"config_field.swift","@M8@",m8)
    replace(fpath+"config_field.swift","@MT@",mt)

    if ca == "0" :
        replace(fpath+"ecp.swift","CAISZS","*/")
        replace(fpath+"ecp.swift","CAISZF","/*")

    hc="0"
    hc2="0"
# Get Hash-to-Curve Z for G1 and G2

    if isinstance(rz,list) :
        if len(rz)==2 :     # Z followed by SSWU isogeny degree
            replace(fpath+"config_field.swift","@RZ@",rz[0])
            replace(fpath+"config_field.swift","@RZ2A@","0")
            replace(fpath+"config_field.swift","@RZ2B@","0")
            hc=rz[1]
        if len(rz)==3 :     # Z for G1 followed by Z for G2 (for SVDW)
            replace(fpath+"config_field.swift","@RZ@",rz[0])
            replace(fpath+"config_field.swift","@RZ2A@",rz[1])
            replace(fpath+"config_field.swift","@RZ2B@",rz[2])
        if len(rz)==5 :     # Z for G1, Z for G2, SSWU isogeny degree for G1, SSWU isogeny degree for G2
            replace(fpath+"config_field.swift","@RZ@",rz[0])
            replace(fpath+"config_field.swift","@RZ2A@",rz[1])
            replace(fpath+"config_field.swift","@RZ2B@",rz[2])
            hc=rz[3]
            hc2=rz[4]
    else :
        replace(fpath+"config_field.swift","@RZ@",rz)   # just Z for SSWU, or indicates RFC7748 or Generic for Elligator
        replace(fpath+"config_field.swift","@RZ2A@","0")
        replace(fpath+"config_field.swift","@RZ2B@","0")

    if hc!="0" :
        replace(fpath+"ecp.swift","CAHCZS","*/")
        replace(fpath+"ecp.swift","CAHCZF","/*")

    itw=int(qi)%10
    replace(fpath+"config_field.swift","@QI@",str(itw))
    if int(qi)//10 > 0 :
        replace(fpath+"config_field.swift","@TW@","POSITOWER")
    else :
        replace(fpath+"config_field.swift","@TW@","NEGATOWER")


    ib=int(base)
    inb=int(nb)
    inbt=int(nbt)
    sh=ib*(1+((8*inb-1)//ib))-inbt
    if sh > 14 :
        sh=14
    replace(fpath+"config_field.swift","@SH@",str(sh))


    replace(fpath+"config_curve.swift","@CT@",ct)
    replace(fpath+"config_curve.swift","@CA@",ca)
    replace(fpath+"config_curve.swift","@PF@",pf)

    replace(fpath+"config_curve.swift","@ST@",stw)
    replace(fpath+"config_curve.swift","@SX@",sx)
    replace(fpath+"config_curve.swift","@AB@",ab)

    replace(fpath+"config_curve.swift","@HC@",hc) 
    replace(fpath+"config_curve.swift","@HC2@",hc2) 

    if cs == "128" :
        replace(fpath+"config_curve.swift","@HT@","32")
        replace(fpath+"config_curve.swift","@AK@","16")
    if cs == "192" :
        replace(fpath+"config_curve.swift","@HT@","48")
        replace(fpath+"config_curve.swift","@AK@","24")
    if cs == "256" :
        replace(fpath+"config_curve.swift","@HT@","64")
        replace(fpath+"config_curve.swift","@AK@","32")

    if pf != "NOT" :

        os.system(copytext+"fp2.swift "+fpath+"fp2.swift")
        os.system(copytext+"fp4.swift "+fpath+"fp4.swift")
        if pf == "BN" or pf == "BLS12" :
            os.system(copytext+"ecp2.swift "+fpath+"ecp2.swift")
            os.system(copytext+"fp12.swift "+fpath+"fp12.swift")
            os.system(copytext+"pair.swift "+fpath+"pair.swift")
            os.system(copytext+"mpin.swift "+fpath+"mpin.swift")
            os.system(copytext+"bls.swift "+fpath+"bls.swift")
            replace(fpath+"bls.swift","XXX",tcl.upper())

            if hc2!="0" :
                replace(fpath+"ecp2.swift","CAHCZS","*/")
                replace(fpath+"ecp2.swift","CAHCZF","/*")
            else :
                replace(fpath+"ecp2.swift","CAHCNZS","*/")
                replace(fpath+"ecp2.swift","CAHCNZF","/*")

            if pf == "BN" :
                replace(fpath+"pair.swift","PFBNS","*/")
                replace(fpath+"pair.swift","PFBNF","/*")

        if pf == "BLS24" :
            os.system(copytext+"fp8.swift "+fpath+"fp8.swift")
            os.system(copytext+"ecp4.swift "+fpath+"ecp4.swift")
            os.system(copytext+"fp24.swift "+fpath+"fp24.swift")
            os.system(copytext+"pair4.swift "+fpath+"pair4.swift")
            os.system(copytext+"mpin192.swift "+fpath+"mpin192.swift")
            os.system(copytext+"bls192.swift "+fpath+"bls192.swift")
            replace(fpath+"bls192.swift","XXX",tcl.upper())

            replace(fpath+"fp4.swift","PFGE24S","*/")
            replace(fpath+"fp4.swift","PFGE24F","/*")

        if pf == "BLS48" :
            os.system(copytext+"fp8.swift "+fpath+"fp8.swift")
            os.system(copytext+"fp16.swift "+fpath+"fp16.swift")
            os.system(copytext+"ecp8.swift "+fpath+"ecp8.swift")
            os.system(copytext+"fp48.swift "+fpath+"fp48.swift")
            os.system(copytext+"pair8.swift "+fpath+"pair8.swift")
            os.system(copytext+"mpin256.swift "+fpath+"mpin256.swift")
            os.system(copytext+"bls256.swift "+fpath+"bls256.swift")
            replace(fpath+"bls256.swift","XXX",tcl.upper())

            replace(fpath+"fp4.swift","PFGE24S","*/")
            replace(fpath+"fp4.swift","PFGE24F","/*")
            replace(fpath+"fp8.swift","PFGE48S","*/")
            replace(fpath+"fp8.swift","PFGE48F","/*")
    else :
        os.system(copytext+"ecdh.swift "+fpath+"ecdh.swift")
        os.system(copytext+"eddsa.swift "+fpath+"eddsa.swift")
        os.system(copytext+"hpke.swift "+fpath+"hpke.swift")
        replace(fpath+"eddsa.swift","Xxx",tc)

    print("Processing "+tcl+ "..",end = "",flush=True)
    os.system("swiftc -DD32 "+fpath+"*.swift -L. -lcore -I. -O -Ounchecked -whole-module-optimization -emit-library -emit-module -module-name "+tcl)
#    os.system("swiftc -DD32 "+fpath+"*.swift -L. -lcore -I. -emit-library -emit-module -module-name "+tcl)
    os.system(deltext+fpath+"*.*")
    os.system("rmdir core"+slashtext+tcl)
    print(". [DONE]")

os.system("mkdir core")
os.system(copytext+ "hash*.swift core"+slashtext+".")
os.system(copytext+ "hmac.swift core"+slashtext+".")
os.system(copytext+ "sha3.swift core"+slashtext+".")
os.system(copytext+ "rand.swift core"+slashtext+".")
os.system(copytext+ "share.swift core"+slashtext+".")
os.system(copytext+ "aes.swift core"+slashtext+".")
os.system(copytext+ "gcm.swift core"+slashtext+".")
os.system(copytext+ "nhs.swift core"+slashtext+".")
os.system(copytext+ "kyber.swift core"+slashtext+".")
os.system(copytext+ "dilithium.swift core"+slashtext+".")

#
os.system("swiftc core"+slashtext+"*.swift -O -Ounchecked -whole-module-optimization -emit-library -emit-module -module-name core")
#os.system("swiftc core"+slashtext+"*.swift -emit-library -emit-module -module-name core")

class miracl_crypto:
    np_curves = (
        ("Ed25519","29","255","2","1","PSEUDO_MERSENNE","0","EDWARDS","-1","NOT","NOT","NOT","NOT","128"),
        ("c25519","29","255","2","1","PSEUDO_MERSENNE","0","MONTGOMERY","486662","NOT","NOT","NOT","NOT","128"),
        ("nist256","28","256","1","-10","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","128"),
        ("brainpool","28","256","1","-3","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","128"),
        ("anssi","28","256","1","-5","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","128"),
        ("hifive","29","336","2","1","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","192"),
        ("Ed448","29","448","1","0","GENERALISED_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","256"),
        ("nist384","29","384","1","-12","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","192"),
        ("c41417","29","414","1","1","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","256"),
        ("nist521","28","521","1","-4","PSEUDO_MERSENNE","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","256"),
        ("nums256w","28","256","1","7","PSEUDO_MERSENNE","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","128"),
        ("nums256e","29","256","1","0","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","128"),
        ("nums384w","29","384","1","-4","PSEUDO_MERSENNE","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","192"),
        ("nums384e","29","384","1","0","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","192"),
        ("nums512w","29","512","1","-4","PSEUDO_MERSENNE","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","256"),
        ("nums512e","29","512","1","0","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","256"),
        ("secp256k1","28","256","1",["-11","3"],"NOT_SPECIAL","0","WEIERSTRASS","0","NOT","NOT","NOT","NOT","128"),
        ("sm2","28","256","1","-9","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","128"),
        ("c13318","29","255","2","2","PSEUDO_MERSENNE","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","128"),
        ("jubjub","29","255","32","1","NOT_SPECIAL","5","EDWARDS","-1","NOT","NOT","NOT","NOT","128"),
        ("x448","29","448","1","0","GENERALISED_MERSENNE","0","MONTGOMERY","156326","NOT","NOT","NOT","NOT","256"),
        ("secp160r1","29","160","1","3","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","128"),
        ("c1174","29","251","1","0","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","128"),
        ("c1665","29","166","1","0","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","128"),
        ("mdc","28","256","1","0","NOT_SPECIAL","0","EDWARDS","1","NOT","NOT","NOT","NOT","128"),
        ("tweedledum","29","255","33","1","NOT_SPECIAL","5","WEIERSTRASS","0","NOT","NOT","NOT","NOT","128"),
        ("tweedledee","29","255","34","1","NOT_SPECIAL","5","WEIERSTRASS","0","NOT","NOT","NOT","NOT","128")
    )

    pf_curves = (
        ("bn254","28","254","1",["-1","-1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BN","D_TYPE","NEGATIVEX","66","128"),
        ("bn254cx","28","254","1",["-1","-1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BN","D_TYPE","NEGATIVEX","66","128"),
        ("bls12383","29","383","1",["1","1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS12","M_TYPE","POSITIVEX","65","128"),
        ("bls12381","29","381","1",["11","-2","-1","11","3"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS12","M_TYPE","NEGATIVEX","65","128"),
        ("fp256bn","28","256","1",["1","1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BN","M_TYPE","NEGATIVEX","66","128"),
        ("fp512bn","29","512","1",["1","1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BN","M_TYPE","POSITIVEX","130","128"),
        ("bls12443","29","443","1",["-7","1","1","11","3"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS12","M_TYPE","POSITIVEX","75","128"),
        ("bls12461","28","461","1",["1","4","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS12","M_TYPE","NEGATIVEX","78","128"),
        ("bn462","28","462","1",["1","1","0"],"NOT_SPECIAL","1","WEIERSTRASS","0","BN","D_TYPE","POSITIVEX","118","128"),
        ("bls24479","29","479","1",["1","4","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS24","M_TYPE","POSITIVEX","49","192"),
        ("bls48556","29","556","1",["-1","2","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS48","M_TYPE","POSITIVEX","32","256"),
        ("bls48581","29","581","1",["2","2","0"],"NOT_SPECIAL","10","WEIERSTRASS","0","BLS48","D_TYPE","NEGATIVEX","33","256"),
        ("bls48286","29","286","1",["1","1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS48","M_TYPE","POSITIVEX","17","128"),
        ("bn158","28","158","1",["1","1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BN","M_TYPE","NEGATIVEX","42","128")
    )

    rsa_params = (
        ("rsa2048","128","28","2"),
        ("rsa3072","48","28","8"),
        ("rsa4096","64","29","8")
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
            tuple[10], tuple[11], tuple[12], tuple[13]
        )
    elif index < len(miracl_crypto.np_curves) + len(miracl_crypto.pf_curves):
        tuple = miracl_crypto.pf_curves[index-len(miracl_crypto.np_curves)]
        curveset(
            tuple[0], tuple[1], tuple[2], tuple[3], tuple[4],
            tuple[5], tuple[6], tuple[7], tuple[8], tuple[9],
            tuple[10], tuple[11], tuple[12], tuple[13]
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

os.system(deltext+" hash*.swift")
os.system(deltext+" hmac.swift")
os.system(deltext+" sha3.swift")
os.system(deltext+" aes.swift")
os.system(deltext+" rand.swift")
os.system(deltext+" share.swift")
os.system(deltext+" gcm.swift")
os.system(deltext+" nhs.swift")
os.system(deltext+" kyber.swift")
os.system(deltext+" dilithium.swift")

os.system(deltext+" big.swift")
os.system(deltext+" dbig.swift")
os.system(deltext+" fp*.swift")
os.system(deltext+" config*.swift")

os.system(deltext+" ecp*.swift")
os.system(deltext+" ecdh.swift")
os.system(deltext+" eddsa.swift")
os.system(deltext+" hpke.swift")
os.system(deltext+" ff.swift")
os.system(deltext+" rsa.swift")
os.system(deltext+" pair*.swift")
os.system(deltext+" mpin*.swift")
os.system(deltext+" bls*.swift")
os.system(deltext+" rom*.swift")

os.system(deltext+"core"+slashtext+"*.*")
os.system("rmdir core")

if testing :
    os.system("swiftc -I. -L. -lcore -led25519 -lnist256 -led448 -lrsa2048 TestECC.swift ")
    os.system("swiftc -I. -L. -lcore -led25519 -led448 TestEDDSA.swift ")
    os.system("swiftc -I. -L. -lcore -lbn254 -lbls12383 -lbls24479 -lbls48556 TestMPIN.swift ")
    os.system("swiftc -I. -L. -lcore -lbn254 -lbls12383 -lbls24479 -lbls48556 TestBLS.swift")
    os.system("swiftc -I. -L. -lcore -led25519 -lnist256 -led448 -lbn254 -lbls12383 -lbls24479 -lbls48556 -lrsa2048 BenchtestALL.swift ")
    #os.system("swiftc -I. -L. -lcore TestNHS.swift")
    os.system("swiftc -I. -L. -lcore TestKYBER.swift")
    os.system("swiftc -I. -L. -lcore TestDLTHM.swift")
os.system("sudo cp lib*.so /usr/lib/.")


