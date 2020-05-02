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

    fpath="core"+slashtext+"src"+slashtext+tb+slashtext
    os.system("mkdir core"+slashtext+"src"+slashtext+tb)

    with open("core"+slashtext+"src"+slashtext+"lib.rs","a") as libfile:
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

    #os.system("go install core"+slashtext+tb)


def curveset(tc,base,nbt,m8,rz,mt,qi,ct,pf,stw,sx,g2,ab,cs) :
    inbt=int(nbt)
    itb=int(inbt+(8-inbt%8)%8)
    inb=int(itb/8)
    nb=str(inb)

    global deltext,slashtext,copytext
    global cptr,chosen

    chosen.append(tc)
    cptr=cptr+1

    fpath="core"+slashtext+"src"+slashtext+tc+slashtext
    os.system("mkdir core"+slashtext+"src"+slashtext+tc)

    with open("core"+slashtext+"src"+slashtext+"lib.rs","a") as libfile:
        libfile.write("pub mod "+tc+";\n")


    os.system(copytext+"big.rs "+fpath+"big.rs")
    os.system(copytext+"dbig.rs "+fpath+"dbig.rs")
    os.system(copytext+"fp.rs "+fpath+"fp.rs")
    os.system(copytext+"ecp.rs "+fpath+"ecp.rs")
    os.system(copytext+"ecdh.rs "+fpath+"ecdh.rs")
    os.system(copytext+"hpke.rs "+fpath+"hpke.rs")
    os.system(copytext+"rom_"+tc+"_64.rs "+fpath+"rom.rs")

    replace(fpath+"big.rs","xxx",tc)
    replace(fpath+"dbig.rs","xxx",tc)
    replace(fpath+"fp.rs","xxx",tc)
    replace(fpath+"ecp.rs","xxx",tc)
    replace(fpath+"ecdh.rs","xxx",tc)
    replace(fpath+"hpke.rs","xxx",tc)

    replace(fpath+"big.rs","@NB@",nb)
    replace(fpath+"big.rs","@BASE@",base)

    replace(fpath+"fp.rs","@NBT@",nbt)
    replace(fpath+"fp.rs","@M8@",m8)
    replace(fpath+"fp.rs","@MT@",mt)
#    replace(fpath+"fp.rs""@RZ@",rz)

# Get Z for G1 and G2
    if isinstance(rz,list) :
        replace(fpath+"fp.rs","@RZ@",rz[0])
        replace(fpath+"fp.rs","@RZA@",rz[1])
        replace(fpath+"fp.rs","@RZB@",rz[2])
    else :
        replace(fpath+"fp.rs","@RZ@",rz)
        replace(fpath+"fp.rs","@RZA@","0")
        replace(fpath+"fp.rs","@RZB@","0")

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
    replace(fpath+"ecp.rs","@PF@",pf)

    replace(fpath+"ecp.rs","@ST@",stw)
    replace(fpath+"ecp.rs","@SX@",sx)
    replace(fpath+"ecp.rs","@AB@",ab)
    replace(fpath+"ecp.rs","@G2@",g2)


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
        replace(fpath+"fp2.rs","xxx",tc)
        replace(fpath+"fp4.rs","xxx",tc)

        if cs == "128" :
            os.system(copytext+"ecp2.rs "+fpath+"ecp2.rs")
            os.system(copytext+"fp12.rs "+fpath+"fp12.rs")
            os.system(copytext+"pair.rs "+fpath+"pair.rs")
            os.system(copytext+"mpin.rs "+fpath+"mpin.rs")
            os.system(copytext+"bls.rs "+fpath+"bls.rs")

            replace(fpath+"fp12.rs","xxx",tc)
            replace(fpath+"ecp2.rs","xxx",tc)
            replace(fpath+"pair.rs","xxx",tc)
            replace(fpath+"mpin.rs","xxx",tc)
            replace(fpath+"bls.rs","xxx",tc)

            os.system(copytext+"modpf.rs "+fpath+"mod.rs")


        if cs == "192" :
            os.system(copytext+"fp8.rs "+fpath+"fp8.rs")
            os.system(copytext+"ecp4.rs "+fpath+"ecp4.rs")
            os.system(copytext+"fp24.rs "+fpath+"fp24.rs")
            os.system(copytext+"pair192.rs "+fpath+"pair192.rs")
            os.system(copytext+"mpin192.rs "+fpath+"mpin192.rs")
            os.system(copytext+"bls192.rs "+fpath+"bls192.rs")

            replace(fpath+"fp8.rs","xxx",tc)
            replace(fpath+"fp24.rs","xxx",tc)
            replace(fpath+"ecp4.rs","xxx",tc)
            replace(fpath+"pair192.rs","xxx",tc)
            replace(fpath+"mpin192.rs","xxx",tc)
            replace(fpath+"bls192.rs","xxx",tc)

            os.system(copytext+"modpf192.rs "+fpath+"mod.rs")

        if cs == "256" :
            os.system(copytext+"fp8.rs "+fpath+"fp8.rs")
            os.system(copytext+"fp16.rs "+fpath+"fp16.rs")
            os.system(copytext+"ecp8.rs "+fpath+"ecp8.rs")
            os.system(copytext+"fp48.rs "+fpath+"fp48.rs")
            os.system(copytext+"pair256.rs "+fpath+"pair256.rs")
            os.system(copytext+"mpin256.rs "+fpath+"mpin256.rs")
            os.system(copytext+"bls256.rs "+fpath+"bls256.rs")

            replace(fpath+"fp8.rs","xxx",tc)
            replace(fpath+"fp16.rs","xxx",tc)
            replace(fpath+"fp48.rs","xxx",tc)
            replace(fpath+"ecp8.rs","xxx",tc)
            replace(fpath+"pair256.rs","xxx",tc)
            replace(fpath+"mpin256.rs","xxx",tc)
            replace(fpath+"bls256.rs","xxx",tc)

            os.system(copytext+"modpf256.rs "+fpath+"mod.rs")

    else :
        os.system(copytext+"modecc.rs "+fpath+"mod.rs")

os.system("cargo new core")
#os.system("mkdir core"+slashtext+"src")
os.system(copytext+ "hash*.rs core"+slashtext+"src"+slashtext+".")
os.system(copytext+ "hmac.rs core"+slashtext+"src"+slashtext+".")
os.system(copytext+ "sha3.rs core"+slashtext+"src"+slashtext+".")
os.system(copytext+ "rand.rs core"+slashtext+"src"+slashtext+".")
os.system(copytext+ "aes.rs core"+slashtext+"src"+slashtext+".")
os.system(copytext+ "gcm.rs core"+slashtext+"src"+slashtext+".")
os.system(copytext+ "nhs.rs core"+slashtext+"src"+slashtext+".")
os.system(copytext+ "arch64.rs core"+slashtext+"src"+slashtext+"arch.rs")
os.system(copytext+ "lib.rs core"+slashtext+"src"+slashtext+"lib.rs")


print("Elliptic Curves")
print("1. ed25519")
print("2. c25519")
print("3. nist256")
print("4. brainpool")
print("5. anssi")
print("6. hifive")
print("7. goldilocks")
print("8. nist384")
print("9. c41417")
print("10. nist521\n")
print("11. nums256w")
print("12. nums256e")
print("13. nums384w")
print("14. nums384e")
print("15. nums512w")
print("16. nums512e")
print("17. secp256k1")
print("18. sm2")
print("19. c13318")
print("20. jubjub")
print("21. x448")
print("22. secp160r1")
print("23. c1174")
print("24. c1665\n")

print("Pairing-Friendly Elliptic Curves")
print("25. bn254")
print("26. bn254CX")
print("27. bls12383")
print("28. bls12381")
print("29. fp256BN")
print("30. fp512BN")
print("31. bls12461")
print("32. bn462")
print("33. bls24479")
print("34. bls48556")
print("35. bls48581\n")

print("RSA")
print("36. rsa2048")
print("37. rsa3072")
print("38. rsa4096")

selection=[]
ptr=0
max=39

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

# curveset(curve,bits_in_base,modulus_bits,modulus_mod_8,Z,modulus_type,curve_type,pairing_friendly,sextic twist,sign of x,g2_table size,ate bits,curve security)
# where "curve" is the common name for the elliptic curve
# bits_in_base gives the number base used for 64 bit architectures, as n where the base is 2^n
# modulus_bits is the actual bit length of the modulus.
# modulus_mod_8 is the remainder when the modulus is divided by 8
# rz Z value for hash_to_point, If list G1 Z value is in [0], G2 Z value (=a+bz) is in [1], [2]
# modulus_type is NOT_SPECIAL, or PSEUDO_MERSENNE, or MONTGOMERY_Friendly, or GENERALISED_MERSENNE (supported for GOLDILOCKS only)
# i for Fp2 QNR 2^i+sqrt(-1) (relevant for PFCs only, else =0). Or QNR over Fp if p=1 mod 8
# curve_type is WEIERSTRASS, EDWARDS or MONTGOMERY
# pairing_friendly is BN, BLS or NOT (if not pairing friendly
# ate bits is number of bits in Ate parameter (from romgen program)
# g2_table size is number of entries in precomputed table
# curve security is AES equiavlent, rounded up.

    if x==1:
        curveset("ed25519","56","255","2","1","PSEUDO_MERSENNE","0","EDWARDS","NOT","NOT","NOT","NOT","NOT","128")
        curve_selected=True
    if x==2:
        curveset("c25519","56","255","2","1","PSEUDO_MERSENNE","0","MONTGOMERY","NOT","NOT","NOT","NOT","NOT","128")
        curve_selected=True
    if x==3:
        curveset("nist256","56","256","1","-10","NOT_SPECIAL","0","WEIERSTRASS","NOT","NOT","NOT","NOT","NOT","128")
        curve_selected=True
    if x==4:
        curveset("brainpool","56","256","1","-3","NOT_SPECIAL","0","WEIERSTRASS","NOT","NOT","NOT","NOT","NOT","128")
        curve_selected=True
    if x==5:
        curveset("anssi","56","256","1","-5","NOT_SPECIAL","0","WEIERSTRASS","NOT","NOT","NOT","NOT","NOT","128")
        curve_selected=True

    if x==6:
        curveset("hifive","60","336","2","1","PSEUDO_MERSENNE","0","EDWARDS","NOT","NOT","NOT","NOT","NOT","192")
        curve_selected=True
    if x==7:
        curveset("goldilocks","58","448","1","0","GENERALISED_MERSENNE","0","EDWARDS","NOT","NOT","NOT","NOT","NOT","256")
        curve_selected=True
    if x==8:
        curveset("nist384","56","384","1","-12","NOT_SPECIAL","0","WEIERSTRASS","NOT","NOT","NOT","NOT","NOT","192")
        curve_selected=True
    if x==9:
        curveset("c41417","60","414","1","1","PSEUDO_MERSENNE","0","EDWARDS","NOT","NOT","NOT","NOT","NOT","256")
        curve_selected=True
    if x==10:
        curveset("nist521","60","521","1","-4","PSEUDO_MERSENNE","0","WEIERSTRASS","NOT","NOT","NOT","NOT","NOT","256")
        curve_selected=True

    if x==11:
        curveset("nums256w","56","256","1","7","PSEUDO_MERSENNE","0","WEIERSTRASS","NOT","NOT","NOT","NOT","NOT","128")
        curve_selected=True
    if x==12:
        curveset("nums256e","56","256","1","0","PSEUDO_MERSENNE","0","EDWARDS","NOT","NOT","NOT","NOT","NOT","128")
        curve_selected=True
    if x==13:
        curveset("nums384w","58","384","1","-4","PSEUDO_MERSENNE","0","WEIERSTRASS","NOT","NOT","NOT","NOT","NOT","192")
        curve_selected=True
    if x==14:
        curveset("nums384e","56","384","1","0","PSEUDO_MERSENNE","0","EDWARDS","NOT","NOT","NOT","NOT","NOT","192")
        curve_selected=True
    if x==15:
        curveset("nums512w","60","512","1","-4","PSEUDO_MERSENNE","0","WEIERSTRASS","NOT","NOT","NOT","NOT","NOT","256")
        curve_selected=True
    if x==16:
        curveset("nums512e","60","512","1","0","PSEUDO_MERSENNE","0","EDWARDS","NOT","NOT","NOT","NOT","NOT","256")
        curve_selected=True

    if x==17:
        curveset("secp256k1","56","256","1","1","NOT_SPECIAL","0","WEIERSTRASS","NOT","NOT","NOT","NOT","NOT","128")
        curve_selected=True
    if x==18:
        curveset("sm2","56","256","1","-9","NOT_SPECIAL","0","WEIERSTRASS","NOT","NOT","NOT","NOT","NOT","128")
        curve_selected=True

    if x==19:
        curveset("c13318","56","255","2","2","PSEUDO_MERSENNE","0","WEIERSTRASS","NOT","NOT","NOT","NOT","NOT","128")
        curve_selected=True

    if x==20:
        curveset("jubjub","56","255","32","1","NOT_SPECIAL","5","EDWARDS","NOT","NOT","NOT","NOT","NOT","128")
        curve_selected=True

    if x==21:
        curveset("x448","58","448","1","0","GENERALISED_MERSENNE","0","MONTGOMERY","NOT","NOT","NOT","NOT","NOT","256")
        curve_selected=True

    if x==22:
        curveset("secp160r1","56","160","1","3","NOT_SPECIAL","0","WEIERSTRASS","NOT","NOT","NOT","NOT","NOT","128")
        curve_selected=True

    if x==23:
        curveset("c1174","56","251","1","0","PSEUDO_MERSENNE","0","EDWARDS","NOT","NOT","NOT","NOT","NOT","128")
        curve_selected=True
    if x==24:
        curveset("c1665","60","166","1","0","PSEUDO_MERSENNE","0","EDWARDS","NOT","NOT","NOT","NOT","NOT","128")
        curve_selected=True

    if x==25:
        curveset("bn254","56","254","1",["-1","0","-1"],"NOT_SPECIAL","0","WEIERSTRASS","BN","D_TYPE","NEGATIVEX","71","66","128")
        pfcurve_selected=True
    if x==26:
        curveset("bn254CX","56","254","1",["-1","0","-1"],"NOT_SPECIAL","0","WEIERSTRASS","BN","D_TYPE","NEGATIVEX","76","66","128")
        pfcurve_selected=True
    if x==27:
        curveset("bls12383","58","383","1",["1","0","1"],"NOT_SPECIAL","0","WEIERSTRASS","BLS","M_TYPE","POSITIVEX","68","65","128")
        pfcurve_selected=True

    if x==28:
        curveset("bls12381","58","381","1",["-3","0","1"],"NOT_SPECIAL","0","WEIERSTRASS","BLS","M_TYPE","NEGATIVEX","69","65","128")
        pfcurve_selected=True

    if x==29:
        curveset("fp256bn","56","256","1",["1","0","1"],"NOT_SPECIAL","0","WEIERSTRASS","BN","M_TYPE","NEGATIVEX","83","66","128")
        pfcurve_selected=True
    if x==30:
        curveset("fp512bn","60","512","1",["1","0","-1"],"NOT_SPECIAL","0","WEIERSTRASS","BN","M_TYPE","POSITIVEX","172","130","128")
        pfcurve_selected=True
# https://eprint.iacr.org/2017/334.pdf
    if x==31:
        curveset("bls12461","60","461","1",["1","4","1"],"NOT_SPECIAL","0","WEIERSTRASS","BLS","M_TYPE","NEGATIVEX","79","78","128")
        pfcurve_selected=True

    if x==32:
        curveset("bn462","60","462","1",["1","0","-1"],"NOT_SPECIAL","1","WEIERSTRASS","BN","D_TYPE","POSITIVEX","125","118","128")
        pfcurve_selected=True

    if x==33:
        curveset("bls24479","56","479","1",["1","1","1"],"NOT_SPECIAL","0","WEIERSTRASS","BLS","M_TYPE","POSITIVEX","52","49","192")
        pfcurve_selected=True

    if x==34:
        curveset("bls48556","58","556","1",["-1","1","1"],"NOT_SPECIAL","0","WEIERSTRASS","BLS","M_TYPE","POSITIVEX","35","32","256")
        pfcurve_selected=True

    if x==35:
        curveset("bls48581","60","581","1",["2","1","1"],"NOT_SPECIAL","10","WEIERSTRASS","BLS","D_TYPE","NEGATIVEX","36","33","256")
        pfcurve_selected=True
# rsaset(rsaname,big_length_bytes,bits_in_base,multiplier)
# The RSA name reflects the modulus size, which is a 2^m multiplier
# of the underlying big length

# There are choices here, different ways of getting the same result, but some faster than others
    if x==36:
        #256 is slower but may allow reuse of 256-bit BIGs used for elliptic curve
        #512 is faster.. but best is 1024
        rsaset("rsa2048","128","58","2")
        #rsaset("RSA2048","64","60","4")
        #rsaset("RSA2048","32","56","8")
        rsa_selected=True
    if x==37:
        rsaset("rsa3072","48","56","8")
        rsa_selected=True
    if x==38:
        #rsaset("RSA4096","32","56","16")
        rsaset("rsa4096","64","60","8")
        rsa_selected=True

os.system("cargo rustc  --manifest-path core"+slashtext+"Cargo.toml --release  --lib")
#os.system("cargo rustc  --manifest-path core"+slashtext+"Cargo.toml --release  --lib -- -C opt-level=s")

#-- --cfg D64
#os.system("cargo rustc --target wasm64-unknown-emscripten --manifest-path core"+slashtext+"Cargo.toml --release --lib")
# maybe wasm64 will exist some day...!

os.system(deltext+" hash*.rs")
os.system(deltext+" hmac.rs")
os.system(deltext+" sha3.rs")
os.system(deltext+" aes.rs")
os.system(deltext+" rand.rs")
os.system(deltext+" gcm.rs")
os.system(deltext+" nhs.rs")

os.system(deltext+" arch*.rs")
os.system(deltext+" big.rs")
os.system(deltext+" dbig.rs")
os.system(deltext+" fp*.rs")
os.system(deltext+" mod*.rs")
os.system(deltext+" lib.rs")

os.system(deltext+" ecdh.rs")
os.system(deltext+" hpke.rs")
os.system(deltext+" ff.rs")
os.system(deltext+" rsa.rs")
os.system(deltext+" ecp*.rs")
os.system(deltext+" pair*.rs")
os.system(deltext+" mpin*.rs")
os.system(deltext+" bls*.rs")
os.system(deltext+" rom*.rs")

if testing :
    os.system(copytext+" core"+slashtext+"target"+slashtext+"release"+slashtext+"libcore.rlib .")
    os.system("rustc TestECC.rs --extern core=libcore.rlib")
    os.system("rustc TestMPIN.rs --extern core=libcore.rlib")
    os.system("rustc TestBLS.rs --extern core=libcore.rlib")
    os.system("rustc BenchtestALL.rs --extern core=libcore.rlib")
    os.system("rustc TestNHS.rs --extern core=libcore.rlib")
    if sys.platform.startswith("win") :
        os.system("TestECC")
        os.system("TestMPIN < pins.txt")
        os.system("TestBLS")
        os.system("BenchtestALL")
        os.system("TestNHS")
    else :
        os.system("./TestECC")
        os.system("./TestMPIN < pins.txt")
        os.system("./TestBLS")
        os.system("./BenchtestALL")
        os.system("./TestNHS")


# create library


