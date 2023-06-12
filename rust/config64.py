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


    fpath="mcore"+slashtext+"src"+slashtext+tb+slashtext
    os.system("mkdir mcore"+slashtext+"src"+slashtext+tb)

    with open("mcore"+slashtext+"src"+slashtext+"lib.rs","a") as libfile:
        libfile.write("pub mod "+tb+";\n")

    os.system(copytext+"modrsa.rs "+fpath+"mod.rs")
    os.system(copytext+"big.rs "+fpath+"big.rs")
    os.system(copytext+"dbig.rs "+fpath+"dbig.rs")
    os.system(copytext+"ff.rs "+fpath+"ff.rs")
    os.system(copytext+"rsa.rs "+fpath+"rsa.rs")

    replace(fpath+"big.rs","xxx",tb)
    replace(fpath+"dbig.rs","xxx",tb)
    replace(fpath+"ff.rs","xxx",tb)
    replace(fpath+"rsa.rs","xxx",tb)

    replace(fpath+"big.rs","@NB@",nb)
    replace(fpath+"big.rs","@BASE@",base)

    replace(fpath+"ff.rs","@ML@",ml)

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
# curve security is AES equivalent, rounded up.
def curveset(tc,base,nbt,m8,rz,mt,qi,ct,ca,pf,stw,sx,g2,ab,cs) :
    inbt=int(nbt)
    itb=int(inbt+(8-inbt%8)%8)
    inb=int(itb/8)
    nb=str(inb)

    global deltext,slashtext,copytext

    tcl=tc.lower()

    fpath="mcore"+slashtext+"src"+slashtext+tcl+slashtext
    os.system("mkdir mcore"+slashtext+"src"+slashtext+tcl)

    with open("mcore"+slashtext+"src"+slashtext+"lib.rs","a") as libfile:
        libfile.write("pub mod "+tcl+";\n")


    os.system(copytext+"big.rs "+fpath+"big.rs")
    os.system(copytext+"dbig.rs "+fpath+"dbig.rs")
    os.system(copytext+"fp.rs "+fpath+"fp.rs")
    os.system(copytext+"ecp.rs "+fpath+"ecp.rs")
    os.system(copytext+"ecdh.rs "+fpath+"ecdh.rs")
    os.system(copytext+"eddsa.rs "+fpath+"eddsa.rs")
    os.system(copytext+"hpke.rs "+fpath+"hpke.rs")
    os.system(copytext+"rom_"+tcl+"_64.rs "+fpath+"rom.rs")

    replace(fpath+"big.rs","xxx",tcl)
    replace(fpath+"dbig.rs","xxx",tcl)
    replace(fpath+"fp.rs","xxx",tcl)
    replace(fpath+"ecp.rs","xxx",tcl)
    replace(fpath+"ecdh.rs","xxx",tcl)
    replace(fpath+"eddsa.rs","xxx",tcl)
    replace(fpath+"eddsa.rs","Xxx",tc)
    replace(fpath+"hpke.rs","xxx",tcl)

    replace(fpath+"big.rs","@NB@",nb)
    replace(fpath+"big.rs","@BASE@",base)

    replace(fpath+"fp.rs","@NBT@",nbt)
    replace(fpath+"fp.rs","@M8@",m8)
    replace(fpath+"fp.rs","@MT@",mt)

    if ca == "0" :
        replace(fpath+"ecp.rs","CAISZS","*/")
        replace(fpath+"ecp.rs","CAISZF","/*")

    hc="0"
    hc2="0"
# Get Hash-to-Curve Z for G1 and G2

    if isinstance(rz,list) :
        if len(rz)==2 :     # Z followed by SSWU isogeny degree
            replace(fpath+"fp.rs","@RZ@",rz[0])
            replace(fpath+"fp.rs","@RZ2A@","0")
            replace(fpath+"fp.rs","@RZ2B@","0")
            hc=rz[1]
        if len(rz)==3 :     # Z for G1 followed by Z for G2 (for SVDW)
            replace(fpath+"fp.rs","@RZ@",rz[0])
            replace(fpath+"fp.rs","@RZ2A@",rz[1])
            replace(fpath+"fp.rs","@RZ2B@",rz[2])
        if len(rz)==5 :     # Z for G1, Z for G2, SSWU isogeny degree for G1, SSWU isogeny degree for G2
            replace(fpath+"fp.rs","@RZ@",rz[0])
            replace(fpath+"fp.rs","@RZ2A@",rz[1])
            replace(fpath+"fp.rs","@RZ2B@",rz[2])
            hc=rz[3]
            hc2=rz[4]
    else :
        replace(fpath+"fp.rs","@RZ@",rz)   # just Z for SSWU, or indicates RFC7748 or Generic for Elligator
        replace(fpath+"fp.rs","@RZ2A@","0")
        replace(fpath+"fp.rs","@RZ2B@","0")

    if hc!="0" :
        replace(fpath+"ecp.rs","CAHCZS","*/")
        replace(fpath+"ecp.rs","CAHCZF","/*")

    itw=int(qi)%10
    replace(fpath+"fp.rs","@QI@",str(itw))
    if int(qi)//10 > 0 :
        replace(fpath+"fp.rs","@TW@","POSITOWER")
    else :
        replace(fpath+"fp.rs","@TW@","NEGATOWER")


    ib=int(base)
    inb=int(nb)
    inbt=int(nbt)
    sh=ib*(1+((8*inb-1)//ib))-inbt
    if sh > 30 :
        sh=30
    replace(fpath+"fp.rs","@SH@",str(sh))


    replace(fpath+"ecp.rs","@CT@",ct)
    replace(fpath+"ecp.rs","@CA@",ca)
    replace(fpath+"ecp.rs","@PF@",pf)

    replace(fpath+"ecp.rs","@ST@",stw)
    replace(fpath+"ecp.rs","@SX@",sx)
    replace(fpath+"ecp.rs","@AB@",ab)
    replace(fpath+"ecp.rs","@G2@",g2)

    replace(fpath+"ecp.rs","@HC@",hc) 
    replace(fpath+"ecp.rs","@HC2@",hc2) 

    if cs == "128" :
        replace(fpath+"ecp.rs","@HT@","32")
        replace(fpath+"ecp.rs","@AK@","16")
    if cs == "192" :
        replace(fpath+"ecp.rs","@HT@","48")
        replace(fpath+"ecp.rs","@AK@","24")
    if cs == "256" :
        replace(fpath+"ecp.rs","@HT@","64")
        replace(fpath+"ecp.rs","@AK@","32")


    if pf != "NOT" :

        os.system(copytext+"fp2.rs "+fpath+"fp2.rs")
        os.system(copytext+"fp4.rs "+fpath+"fp4.rs")
        replace(fpath+"fp2.rs","xxx",tcl)
        replace(fpath+"fp4.rs","xxx",tcl)

        if pf == "BN" or pf == "BLS12" :
            os.system(copytext+"ecp2.rs "+fpath+"ecp2.rs")
            os.system(copytext+"fp12.rs "+fpath+"fp12.rs")
            os.system(copytext+"pair.rs "+fpath+"pair.rs")
            os.system(copytext+"mpin.rs "+fpath+"mpin.rs")
            os.system(copytext+"bls.rs "+fpath+"bls.rs")

            if hc2 != "0" :
                replace(fpath+"ecp2.rs","CAHCZS","*/")
                replace(fpath+"ecp2.rs","CAHCZF","/*")
            else :
                replace(fpath+"ecp2.rs","CAHCNZS","*/")
                replace(fpath+"ecp2.rs","CAHCNZF","/*")

            replace(fpath+"fp12.rs","xxx",tcl)
            replace(fpath+"ecp2.rs","xxx",tcl)
            replace(fpath+"pair.rs","xxx",tcl)
            replace(fpath+"mpin.rs","xxx",tcl)
            replace(fpath+"bls.rs","xxx",tcl)
            replace(fpath+"bls.rs","XXX",tcl.upper())

            if pf == "BN" :
                replace(fpath+"pair.rs","PFBNS","*/")
                replace(fpath+"pair.rs","PFBNF","/*")

            os.system(copytext+"modpf.rs "+fpath+"mod.rs")


        if pf == "BLS24" :
            os.system(copytext+"fp8.rs "+fpath+"fp8.rs")
            os.system(copytext+"ecp4.rs "+fpath+"ecp4.rs")
            os.system(copytext+"fp24.rs "+fpath+"fp24.rs")
            os.system(copytext+"pair4.rs "+fpath+"pair4.rs")
            os.system(copytext+"mpin192.rs "+fpath+"mpin192.rs")
            os.system(copytext+"bls192.rs "+fpath+"bls192.rs")

            replace(fpath+"fp4.rs","PFGE24S","*/")
            replace(fpath+"fp4.rs","PFGE24F","/*")
            replace(fpath+"fp8.rs","xxx",tcl)
            replace(fpath+"fp24.rs","xxx",tcl)
            replace(fpath+"ecp4.rs","xxx",tcl)
            replace(fpath+"pair4.rs","xxx",tcl)
            replace(fpath+"mpin192.rs","xxx",tcl)
            replace(fpath+"bls192.rs","xxx",tcl)
            replace(fpath+"bls192.rs","XXX",tcl.upper())

            os.system(copytext+"modpf192.rs "+fpath+"mod.rs")

        if pf == "BLS48" :
            os.system(copytext+"fp8.rs "+fpath+"fp8.rs")
            os.system(copytext+"fp16.rs "+fpath+"fp16.rs")
            os.system(copytext+"ecp8.rs "+fpath+"ecp8.rs")
            os.system(copytext+"fp48.rs "+fpath+"fp48.rs")
            os.system(copytext+"pair8.rs "+fpath+"pair8.rs")
            os.system(copytext+"mpin256.rs "+fpath+"mpin256.rs")
            os.system(copytext+"bls256.rs "+fpath+"bls256.rs")

            replace(fpath+"fp4.rs","PFGE24S","*/")
            replace(fpath+"fp4.rs","PFGE24F","/*")
            replace(fpath+"fp8.rs","PFGE48S","*/")
            replace(fpath+"fp8.rs","PFGE48F","/*")
            replace(fpath+"fp8.rs","xxx",tcl)
            replace(fpath+"fp16.rs","xxx",tcl)
            replace(fpath+"fp48.rs","xxx",tcl)
            replace(fpath+"ecp8.rs","xxx",tcl)
            replace(fpath+"pair8.rs","xxx",tcl)
            replace(fpath+"mpin256.rs","xxx",tcl)
            replace(fpath+"bls256.rs","xxx",tcl)
            replace(fpath+"bls256.rs","XXX",tcl.upper())

            os.system(copytext+"modpf256.rs "+fpath+"mod.rs")

    else :
        os.system(copytext+"modecc.rs "+fpath+"mod.rs")


class miracl_crypto:
    np_curves = (
        ("Ed25519","56","255","2","1","PSEUDO_MERSENNE","0","EDWARDS","-1","NOT","NOT","NOT","NOT","NOT","128"),
        ("c25519","56","255","2","1","PSEUDO_MERSENNE","0","MONTGOMERY","486662","NOT","NOT","NOT","NOT","NOT","128"),
        ("nist256","56","256","1","-10","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","128"),
        ("brainpool","56","256","1","-3","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","128"),
        ("anssi","56","256","1","-5","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","128"),
        ("hifive","60","336","2","1","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","NOT","192"),
        ("Ed448","58","448","1","0","GENERALISED_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","NOT","256"),
        ("nist384","56","384","1","-12","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","192"),
        ("c41417","60","414","1","1","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","NOT","256"),
        ("nist521","60","521","1","-4","PSEUDO_MERSENNE","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","256"),
        ("nums256w","56","256","1","7","PSEUDO_MERSENNE","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","128"),
        ("nums256e","56","256","1","0","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","NOT","128"),
        ("nums384w","58","384","1","-4","PSEUDO_MERSENNE","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","192"),
        ("nums384e","56","384","1","0","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","NOT","192"),
        ("nums512w","60","512","1","-4","PSEUDO_MERSENNE","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","256"),
        ("nums512e","60","512","1","0","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","NOT","256"),
        ("secp256k1","56","256","1",["-11","3"],"NOT_SPECIAL","0","WEIERSTRASS","0","NOT","NOT","NOT","NOT","NOT","128"),
        ("sm2","56","256","1","-9","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","128"),
        ("c13318","56","255","2","2","PSEUDO_MERSENNE","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","128"),
        ("jubjub","56","255","32","1","NOT_SPECIAL","5","EDWARDS","-1","NOT","NOT","NOT","NOT","NOT","128"),
        ("x448","58","448","1","0","GENERALISED_MERSENNE","0","MONTGOMERY","156326","NOT","NOT","NOT","NOT","NOT","256"),
        ("secp160r1","56","160","1","3","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","128"),
        ("c1174","56","251","1","0","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","NOT","128"),
        ("c1665","60","166","1","0","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","NOT","128"),
        ("mdc","56","256","1","0","NOT_SPECIAL","0","EDWARDS","1","NOT","NOT","NOT","NOT","NOT","128"),
        ("tweedledum","56","255","33","1","NOT_SPECIAL","5","WEIERSTRASS","0","NOT","NOT","NOT","NOT","NOT","128"),
        ("tweedledee","56","255","34","1","NOT_SPECIAL","5","WEIERSTRASS","0","NOT","NOT","NOT","NOT","NOT","128")
    )

    pf_curves = (
        ("bn254","56","254","1",["-1","-1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BN","D_TYPE","NEGATIVEX","71","66","128"),
        ("bn254cx","56","254","1",["-1","-1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BN","D_TYPE","NEGATIVEX","76","66","128") ,
        ("bls12383","58","383","1",["1","1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS12","M_TYPE","POSITIVEX","68","65","128"),
        ("bls12381","58","381","1",["11","-2","-1","11","3"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS12","M_TYPE","NEGATIVEX","69","65","128"),
        ("fp256bn","56","256","1",["1","1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BN","M_TYPE","NEGATIVEX","83","66","128"),
        ("fp512bn","60","512","1",["1","1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BN","M_TYPE","POSITIVEX","172","130","128"),
        ("bls12443","60","443","1",["-7","1","1","11","3"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS12","M_TYPE","POSITIVEX","78","75","128"),
        ("bls12461","60","461","1",["1","4","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS12","M_TYPE","NEGATIVEX","79","78","128"),
        ("bn462","60","462","1",["1","1","0"],"NOT_SPECIAL","1","WEIERSTRASS","0","BN","D_TYPE","POSITIVEX","125","118","128"),
        ("bls24479","56","479","1",["1","4","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS24","M_TYPE","POSITIVEX","52","49","192"),
        ("bls48556","58","556","1",["-1","2","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS48","M_TYPE","POSITIVEX","35","32","256"),
        ("bls48581","60","581","1",["2","2","0"],"NOT_SPECIAL","10","WEIERSTRASS","0","BLS48","D_TYPE","NEGATIVEX","36","33","256"),
        ("bls48286","60","286","1",["1","1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS48","M_TYPE","POSITIVEX","20","17","128"),
        ("bn158","56","158","1",["1","1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BN","M_TYPE","NEGATIVEX","49","42","128") 
    )

    rsa_params = (
        ("rsa2048","128","58","2"),
        ("rsa3072","48","56","8"),
        ("rsa4096","64","60","8")
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

if __name__ == '__main__':
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

    os.system("cargo new mcore")
    #os.system("mkdir mcore"+slashtext+"src")
    os.system(copytext+ "hash*.rs mcore"+slashtext+"src"+slashtext+".")
    os.system(copytext+ "hmac.rs mcore"+slashtext+"src"+slashtext+".")
    os.system(copytext+ "sha3.rs mcore"+slashtext+"src"+slashtext+".")
    os.system(copytext+ "rand.rs mcore"+slashtext+"src"+slashtext+".")
    os.system(copytext+ "share.rs mcore"+slashtext+"src"+slashtext+".")
    os.system(copytext+ "aes.rs mcore"+slashtext+"src"+slashtext+".")
    os.system(copytext+ "gcm.rs mcore"+slashtext+"src"+slashtext+".")
    os.system(copytext+ "nhs.rs mcore"+slashtext+"src"+slashtext+".")
    os.system(copytext+ "dilithium.rs mcore"+slashtext+"src"+slashtext+".")
    os.system(copytext+ "kyber.rs mcore"+slashtext+"src"+slashtext+".")
    os.system(copytext+ "x509.rs mcore"+slashtext+"src"+slashtext+".")
    os.system(copytext+ "arch64.rs mcore"+slashtext+"src"+slashtext+"arch.rs")
    os.system(copytext+ "lib.rs mcore"+slashtext+"src"+slashtext+"lib.rs")


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

    os.system("cargo rustc --manifest-path mcore"+slashtext+"Cargo.toml --release  --lib")
    #os.system("cargo rustc  --manifest-path mcore"+slashtext+"Cargo.toml --release  --lib -- -C opt-level=s")

    #-- --cfg D64
    #os.system("cargo rustc --target wasm64-unknown-emscripten --manifest-path mcore"+slashtext+"Cargo.toml --release --lib")
    # maybe wasm64 will exist some day...!

    os.system(deltext+" hash*.rs")
    os.system(deltext+" hmac.rs")
    os.system(deltext+" sha3.rs")
    os.system(deltext+" aes.rs")
    os.system(deltext+" rand.rs")
    os.system(deltext+" share.rs")
    os.system(deltext+" gcm.rs")
    os.system(deltext+" nhs.rs")
    os.system(deltext+" dilithium.rs")
    os.system(deltext+" kyber.rs")
    os.system(deltext+" x509.rs");

    os.system(deltext+" arch*.rs")
    os.system(deltext+" big.rs")
    os.system(deltext+" dbig.rs")
    os.system(deltext+" fp*.rs")
    os.system(deltext+" mod*.rs")
    os.system(deltext+" lib.rs")

    os.system(deltext+" ecdh.rs")
    os.system(deltext+" eddsa.rs")
    os.system(deltext+" hpke.rs")
    os.system(deltext+" ff.rs")
    os.system(deltext+" rsa.rs")
    os.system(deltext+" ecp*.rs")
    os.system(deltext+" pair*.rs")
    os.system(deltext+" mpin*.rs")
    os.system(deltext+" bls*.rs")
    os.system(deltext+" rom*.rs")

    if testing :
        os.system(copytext+" mcore"+slashtext+"target"+slashtext+"release"+slashtext+"libmcore.rlib .")
        os.system("rustc TestECC.rs --extern mcore=libmcore.rlib")
        os.system("rustc TestEDDSA.rs --extern mcore=libmcore.rlib")
        os.system("rustc TestMPIN.rs --extern mcore=libmcore.rlib")
        os.system("rustc TestBLS.rs --extern mcore=libmcore.rlib")
        os.system("rustc BenchtestALL.rs --extern mcore=libmcore.rlib")
        os.system("rustc TestNHS.rs --extern mcore=libmcore.rlib")
        os.system("rustc TestDLTHM.rs --extern mcore=libmcore.rlib")
        os.system("rustc TestKYBER.rs --extern mcore=libmcore.rlib")

