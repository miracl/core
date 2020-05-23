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
    copytext="copy "
    deltext="del "
    slashtext="\\"

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

    os.system("swiftc -DD64 "+fpath+"*.swift -L. -lcore -I. -O -Ounchecked -whole-module-optimization -emit-library -emit-module -module-name "+tb)
    os.system(deltext+fpath+"*.*")
    os.system("rmdir core"+slashtext+tb)


def curveset(tc,base,nbt,m8,rz,mt,qi,ct,pf,stw,sx,ab,cs) :
    global deltext,slashtext,copytext
    global cptr,chosen

    inbt=int(nbt)
    itb=int(inbt+(8-inbt%8)%8)
    inb=int(itb/8)
    nb=str(inb)

    chosen.append(tc)
    cptr=cptr+1

    fpath="core"+slashtext+tc+slashtext
    os.system("mkdir core"+slashtext+tc)

    os.system(copytext+"big.swift "+fpath+"big.swift")
    os.system(copytext+"config_big.swift "+fpath+"config_big.swift")
    os.system(copytext+"config_field.swift "+fpath+"config_field.swift")
    os.system(copytext+"config_curve.swift "+fpath+"config_curve.swift")
    os.system(copytext+"dbig.swift "+fpath+"dbig.swift")
    os.system(copytext+"fp.swift "+fpath+"fp.swift")
    os.system(copytext+"ecp.swift "+fpath+"ecp.swift")
    os.system(copytext+"rom_"+tc+".swift "+fpath+"rom.swift")

    replace(fpath+"config_big.swift","@NB@",nb)
    replace(fpath+"config_big.swift","@BASE32@",base)
    replace(fpath+"config_big.swift","@BASE64@",base)

    replace(fpath+"config_field.swift","@NBT@",nbt)
    replace(fpath+"config_field.swift","@M8@",m8)
    replace(fpath+"config_field.swift","@MT@",mt)
#    replace(fpath+"config_field.swift","@RZ@",rz)

# Get Z for G1 and G2
    if isinstance(rz,list) :
        replace(fpath+"config_field.swift","@RZ@",rz[0])
        replace(fpath+"config_field.swift","@RZA@",rz[1])
        replace(fpath+"config_field.swift","@RZB@",rz[2])
    else :
        replace(fpath+"config_field.swift","@RZ@",rz)
        replace(fpath+"config_field.swift","@RZA@","0")
        replace(fpath+"config_field.swift","@RZB@","0")

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
    if sh > 30 :
        sh=30
    replace(fpath+"config_field.swift","@SH@",str(sh))

    replace(fpath+"config_curve.swift","@CT@",ct)
    replace(fpath+"config_curve.swift","@PF@",pf)

    replace(fpath+"config_curve.swift","@ST@",stw)
    replace(fpath+"config_curve.swift","@SX@",sx)
    replace(fpath+"config_curve.swift","@AB@",ab)

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
        if cs == "128" :
            os.system(copytext+"ecp2.swift "+fpath+"ecp2.swift")
            os.system(copytext+"fp12.swift "+fpath+"fp12.swift")
            os.system(copytext+"pair.swift "+fpath+"pair.swift")
            os.system(copytext+"mpin.swift "+fpath+"mpin.swift")
            os.system(copytext+"bls.swift "+fpath+"bls.swift")
        if cs == "192" :
            os.system(copytext+"fp8.swift "+fpath+"fp8.swift")
            os.system(copytext+"ecp4.swift "+fpath+"ecp4.swift")
            os.system(copytext+"fp24.swift "+fpath+"fp24.swift")
            os.system(copytext+"pair192.swift "+fpath+"pair192.swift")
            os.system(copytext+"mpin192.swift "+fpath+"mpin192.swift")
            os.system(copytext+"bls192.swift "+fpath+"bls192.swift")
        if cs == "256" :
            os.system(copytext+"fp8.swift "+fpath+"fp8.swift")
            os.system(copytext+"fp16.swift "+fpath+"fp16.swift")
            os.system(copytext+"ecp8.swift "+fpath+"ecp8.swift")
            os.system(copytext+"fp48.swift "+fpath+"fp48.swift")
            os.system(copytext+"pair256.swift "+fpath+"pair256.swift")
            os.system(copytext+"mpin256.swift "+fpath+"mpin256.swift")
            os.system(copytext+"bls256.swift "+fpath+"bls256.swift")

    else :
        os.system(copytext+"ecdh.swift "+fpath+"ecdh.swift")
        os.system(copytext+"hpke.swift "+fpath+"hpke.swift")


    os.system("swiftc -DD64 "+fpath+"*.swift -L. -lcore -I. -O -Ounchecked -whole-module-optimization -emit-library -emit-module -module-name "+tc)
    os.system(deltext+fpath+"*.*")
    os.system("rmdir core"+slashtext+tc)


os.system("mkdir core")
os.system(copytext+ "hash*.swift core"+slashtext+".")
os.system(copytext+ "hmac.swift core"+slashtext+".")
os.system(copytext+ "sha3.swift core"+slashtext+".")
os.system(copytext+ "rand.swift core"+slashtext+".")
os.system(copytext+ "aes.swift core"+slashtext+".")
os.system(copytext+ "gcm.swift core"+slashtext+".")
os.system(copytext+ "nhs.swift core"+slashtext+".")

os.system("swiftc core"+slashtext+"*.swift -O -Ounchecked -whole-module-optimization -emit-library -emit-module -module-name core")

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
print("10. nist521")
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
print("24. c1665")
print("25. Million Dollar Curve\n")

print("Pairing-Friendly Elliptic Curves")
print("26. bn254")
print("27. bn254CX")
print("28. bls12383")
print("29. bls12381")
print("30. fp256BN")
print("31. fp512BN")
print("32. bls12461")
print("33. bn462")
print("34. bls24479")
print("35. bls48556")
print("36. bls48581\n")

print("RSA")
print("37. rsa2048")
print("38. rsa3072")
print("39. rsa4096")

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

# curveset(curve,bits_in_base,modulus_bits,modulus_mod_8,Z,modulus_type,curve_type,pairing_friendly,ate_bits,curve security)
# where "curve" is the common name for the elliptic curve
# bits_in_base gives the number base used for 32 bit architectures, as n where the base is 2^n
# modulus_bits is the actual bit length of the modulus.
# modulus_mod_8 is the remainder when the modulus is divided by 8
# rz Z value for hash_to_point, If list G1 Z value is in [0], G2 Z value (=a+bz) is in [1], [2]
# modulus_type is NOT_SPECIAL, or PSEUDO_MERSENNE, or MONTGOMERY_Friendly, or GENERALISED_MERSENNE (supported for GOLDILOCKS only)
# i for Fp2 QNR 2^i+sqrt(-1) (relevant for PFCs only, else =0). Or QNR over Fp if p=1 mod 8
# curve_type is WEIERSTRASS, EDWARDS or MONTGOMERY
# pairing_friendly is BN, BLS or NOT (if not pairing friendly)
# if pairing friendly. M or D type twist, and sign of the family parameter x
# ate bits is number of bits in Ate parameter (from romgen program)
# curve security is AES equivalent, rounded up.

    if x==1:
        curveset("ed25519","56","255","2","1","PSEUDO_MERSENNE","0","EDWARDS","NOT","NOT","NOT","NOT","128")
        curve_selected=True
    if x==2:
        curveset("c25519","56","255","2","1","PSEUDO_MERSENNE","0","MONTGOMERY","NOT","NOT","NOT","NOT","128")
        curve_selected=True
    if x==3:
        curveset("nist256","56","256","1","-10","NOT_SPECIAL","0","WEIERSTRASS","NOT","NOT","NOT","NOT","128")
        curve_selected=True
    if x==4:
        curveset("brainpool","56","256","1","-3","NOT_SPECIAL","0","WEIERSTRASS","NOT","NOT","NOT","NOT","128")
        curve_selected=True
    if x==5:
        curveset("anssi","56","256","1","-5","NOT_SPECIAL","0","WEIERSTRASS","NOT","NOT","NOT","NOT","128")
        curve_selected=True

    if x==6:
        curveset("hifive","60","336","2","1","PSEUDO_MERSENNE","0","EDWARDS","NOT","NOT","NOT","NOT","192")
        curve_selected=True
    if x==7:
        curveset("goldilocks","58","448","1","0","GENERALISED_MERSENNE","0","EDWARDS","NOT","NOT","NOT","NOT","256")
        curve_selected=True
    if x==8:
        curveset("nist384","56","384","1","-12","NOT_SPECIAL","0","WEIERSTRASS","NOT","NOT","NOT","NOT","192")
        curve_selected=True
    if x==9:
        curveset("c41417","60","414","1","1","PSEUDO_MERSENNE","0","EDWARDS","NOT","NOT","NOT","NOT","256")
        curve_selected=True
    if x==10:
        curveset("nist521","60","521","1","-4","PSEUDO_MERSENNE","0","WEIERSTRASS","NOT","NOT","NOT","NOT","256")
        curve_selected=True

    if x==11:
        curveset("nums256w","56","256","1","7","PSEUDO_MERSENNE","0","WEIERSTRASS","NOT","NOT","NOT","NOT","128")
        curve_selected=True
    if x==12:
        curveset("nums256e","56","256","1","0","PSEUDO_MERSENNE","0","EDWARDS","NOT","NOT","NOT","NOT","128")
        curve_selected=True
    if x==13:
        curveset("nums384w","58","384","1","-4","PSEUDO_MERSENNE","0","WEIERSTRASS","NOT","NOT","NOT","NOT","192")
        curve_selected=True
    if x==14:
        curveset("nums384e","56","384","1","0","PSEUDO_MERSENNE","0","EDWARDS","NOT","NOT","NOT","NOT","192")
        curve_selected=True
    if x==15:
        curveset("nums512w","60","512","1","-4","PSEUDO_MERSENNE","0","WEIERSTRASS","NOT","NOT","NOT","NOT","256")
        curve_selected=True
    if x==16:
        curveset("nums512e","60","512","1","0","PSEUDO_MERSENNE","0","EDWARDS","NOT","NOT","NOT","NOT","256")
        curve_selected=True

    if x==17:
        curveset("secp256k1","56","256","1","1","NOT_SPECIAL","0","WEIERSTRASS","NOT","NOT","NOT","NOT","128")
        curve_selected=True
    if x==18:
        curveset("sm2","56","256","1","-9","NOT_SPECIAL","0","WEIERSTRASS","NOT","NOT","NOT","NOT","128")
        curve_selected=True

    if x==19:
        curveset("c13318","56","255","2","2","PSEUDO_MERSENNE","0","WEIERSTRASS","NOT","NOT","NOT","NOT","128")
        curve_selected=True

    if x==20:
        curveset("jubjub","56","255","32","1","NOT_SPECIAL","5","EDWARDS","NOT","NOT","NOT","NOT","128")
        curve_selected=True

    if x==21:
        curveset("x448","58","448","1","0","GENERALISED_MERSENNE","0","MONTGOMERY","NOT","NOT","NOT","NOT","256")
        curve_selected=True

    if x==22:
        curveset("secp160r1","56","160","1","3","NOT_SPECIAL","0","WEIERSTRASS","NOT","NOT","NOT","NOT","128")
        curve_selected=True

    if x==23:
        curveset("c1174","56","251","1","0","PSEUDO_MERSENNE","0","EDWARDS","NOT","NOT","NOT","NOT","128")
        curve_selected=True
    if x==24:
        curveset("c1665","60","166","1","0","PSEUDO_MERSENNE","0","EDWARDS","NOT","NOT","NOT","NOT","128")

    if x==25:
        curveset("mdc","56","256","1","0","NOT_SPECIAL","0","EDWARDS","NOT","NOT","NOT","NOT","128")
        curve_selected=True


    if x==26:
        curveset("bn254","56","254","1",["-1","0","-1"],"NOT_SPECIAL","0","WEIERSTRASS","BN","D_TYPE","NEGATIVEX","66","128")
        pfcurve_selected=True
    if x==27:
        curveset("bn254CX","56","254","1",["-1","0","-1"],"NOT_SPECIAL","0","WEIERSTRASS","BN","D_TYPE","NEGATIVEX","66","128")
        pfcurve_selected=True
    if x==28:
        curveset("bls12383","58","383","1",["1","0","1"],"NOT_SPECIAL","0","WEIERSTRASS","BLS","M_TYPE","POSITIVEX","65","128")
        pfcurve_selected=True

    if x==29:
        curveset("bls12381","58","381","1",["-3","0","1"],"NOT_SPECIAL","0","WEIERSTRASS","BLS","M_TYPE","NEGATIVEX","65","128")
        pfcurve_selected=True


    if x==30:
        curveset("fp256bn","56","256","1",["1","0","1"],"NOT_SPECIAL","0","WEIERSTRASS","BN","M_TYPE","NEGATIVEX","66","128")
        pfcurve_selected=True
    if x==31:
        curveset("fp512bn","60","512","1",["1","0","-1"],"NOT_SPECIAL","0","WEIERSTRASS","BN","M_TYPE","POSITIVEX","130","128")
        pfcurve_selected=True
# https://eprint.iacr.org/2017/334.pdf
    if x==32:
        curveset("bls12461","60","461","1",["1","4","1"],"NOT_SPECIAL","0","WEIERSTRASS","BLS","M_TYPE","NEGATIVEX","78","128")
        pfcurve_selected=True

    if x==33:
        curveset("bn462","60","462","1",["1","0","-1"],"NOT_SPECIAL","1","WEIERSTRASS","BN","D_TYPE","POSITIVEX","118","128")
        pfcurve_selected=True

    if x==34:
        curveset("bls24479","56","479","1",["1","1","1"],"NOT_SPECIAL","0","WEIERSTRASS","BLS","M_TYPE","POSITIVEX","49","192")
        pfcurve_selected=True

    if x==35:
        curveset("bls48556","58","556","1",["-1","1","1"],"NOT_SPECIAL","0","WEIERSTRASS","BLS","M_TYPE","POSITIVEX","32","256")
        pfcurve_selected=True

    if x==36:
        curveset("bls48581","60","581","1",["2","1","1"],"NOT_SPECIAL","10","WEIERSTRASS","BLS","D_TYPE","NEGATIVEX","33","256")
        pfcurve_selected=True
# rsaset(rsaname,big_length_bytes,bits_in_base,multiplier)
# The RSA name reflects the modulus size, which is a 2^m multiplier
# of the underlying big length

# There are choices here, different ways of getting the same result, but some faster than others
    if x==37:
        #256 is slower but may allow reuse of 256-bit BIGs used for elliptic curve
        #512 is faster.. but best is 1024
        rsaset("rsa2048","128","58","2")
        #rsaset("rsa2048","64","60",4")
        #rsaset("rsa2048","32","56",8")
        rsa_selected=True
    if x==38:
        rsaset("rsa3072","48","56","8")
        rsa_selected=True
    if x==39:
        #rsaset("rsa4096","32","56",16")
        rsaset("rsa4096","64","60","8")
        rsa_selected=True

os.system(deltext+" hash*.swift")
os.system(deltext+" hmac.swift")
os.system(deltext+" sha3.swift")
os.system(deltext+" aes.swift")
os.system(deltext+" rand.swift")
os.system(deltext+" gcm.swift")
os.system(deltext+" nhs.swift")

os.system(deltext+" big.swift")
os.system(deltext+" dbig.swift")
os.system(deltext+" fp*.swift")
os.system(deltext+" config*.swift")

os.system(deltext+" ecp*.swift")
os.system(deltext+" ecdh.swift")
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
    os.system("swiftc -I. -L. -lcore -led25519 -lnist256 -lgoldilocks -lrsa2048 TestECC.swift ")
    os.system("swiftc -I. -L. -lcore -lbn254 -lbls12383 -lbls24479 -lbls48556 TestMPIN.swift ")
    os.system("swiftc -I. -L. -lcore -lbn254 -lbls12383 -lbls24479 -lbls48556 TestBLS.swift")
    os.system("swiftc -I. -L. -lcore -led25519 -lnist256 -lgoldilocks -lbn254 -lbls12383 -lbls24479 -lbls48556 -lrsa2048 BenchtestALL.swift ")
    os.system("swiftc -I. -L. -lcore TestNHS.swift")
    os.system("sudo cp lib*.so /usr/lib/.")
    os.system("./TestECC")
    os.system("./TestMPIN")
    os.system("./TestBLS")
    os.system("./BenchtestALL")
    os.system("./TestNHS")

