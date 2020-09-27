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

    os.system("swiftc -DD32 "+fpath+"*.swift -L. -lcore -I. -O -Ounchecked -whole-module-optimization -emit-library -emit-module -module-name "+tb)
    os.system(deltext+fpath+"*.*")
    os.system("rmdir core"+slashtext+tb)


def curveset(tc,base,nbt,m8,rz,mt,qi,ct,ca,pf,stw,sx,ab,cs) :
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
            replace(fpath+"fp4.swift","PFGE24S","*/")
            replace(fpath+"fp4.swift","PFGE24F","/*")
            replace(fpath+"fp8.swift","PFGE48S","*/")
            replace(fpath+"fp8.swift","PFGE48F","/*")
    else :
        os.system(copytext+"ecdh.swift "+fpath+"ecdh.swift")
        os.system(copytext+"hpke.swift "+fpath+"hpke.swift")


    os.system("swiftc -DD32 "+fpath+"*.swift -L. -lcore -I. -O -Ounchecked -whole-module-optimization -emit-library -emit-module -module-name "+tc)
#    os.system("swiftc -DD32 "+fpath+"*.swift -L. -lcore -I. -emit-library -emit-module -module-name "+tc)
    os.system(deltext+fpath+"*.*")
    os.system("rmdir core"+slashtext+tc)


os.system("mkdir core")
os.system(copytext+ "hash*.swift core"+slashtext+".")
os.system(copytext+ "hmac.swift core"+slashtext+".")
os.system(copytext+ "sha3.swift core"+slashtext+".")
os.system(copytext+ "rand.swift core"+slashtext+".")
os.system(copytext+ "share.swift core"+slashtext+".")
os.system(copytext+ "aes.swift core"+slashtext+".")
os.system(copytext+ "gcm.swift core"+slashtext+".")
os.system(copytext+ "nhs.swift core"+slashtext+".")

#
os.system("swiftc core"+slashtext+"*.swift -O -Ounchecked -whole-module-optimization -emit-library -emit-module -module-name core")
#os.system("swiftc core"+slashtext+"*.swift -emit-library -emit-module -module-name core")

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
print("25. Million Dollar Curve")
print("26. tweedledum")
print("27. tweedledee\n")

print("Pairing-Friendly Elliptic Curves")
print("28. bn254")
print("29. bn254CX")
print("30. bls12383")
print("31. bls12381")
print("32. fp256BN")
print("33. fp512BN")
print("34. bls12461")
print("35. bn462")
print("36. bls24479")
print("37. bls48556")
print("38. bls48581")
print("39. bls48286\n")

print("RSA")
print("40. rsa2048")
print("41. rsa3072")
print("42. rsa4096")

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

# curveset(curve,bits_in_base,modulus_bits,modulus_mod_8,Z,modulus_type,curve_type,Curve A,pairing_friendly,ate_bits,curve security)
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
# ate bits is number of bits in Ate parameter (from romgen program)
# curve security is AES equivalent, rounded up.


    if x==1:
        curveset("ed25519","29","255","2","1","PSEUDO_MERSENNE","0","EDWARDS","-1","NOT","NOT","NOT","NOT","128")
        curve_selected=True
    if x==2:
        curveset("c25519","29","255","2","1","PSEUDO_MERSENNE","0","MONTGOMERY","486662","NOT","NOT","NOT","NOT","128")
        curve_selected=True
    if x==3:
        curveset("nist256","28","256","1","-10","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","128")
        curve_selected=True
    if x==4:
        curveset("brainpool","28","256","1","-3","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","128")
        curve_selected=True
    if x==5:
        curveset("anssi","28","256","1","-5","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","128")
        curve_selected=True

    if x==6:
        curveset("hifive","29","336","2","1","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","192")
        curve_selected=True
    if x==7:
        curveset("goldilocks","29","448","1","0","GENERALISED_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","256")
        curve_selected=True
    if x==8:
        curveset("nist384","29","384","1","-12","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","192")
        curve_selected=True
    if x==9:
        curveset("c41417","29","414","1","1","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","256")
        curve_selected=True
    if x==10:
        curveset("nist521","28","521","1","-4","PSEUDO_MERSENNE","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","256")
        curve_selected=True


    if x==11:
        curveset("nums256w","28","256","1","7","PSEUDO_MERSENNE","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","128")
        curve_selected=True
    if x==12:
        curveset("nums256e","29","256","1","0","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","128")
        curve_selected=True
    if x==13:
        curveset("nums384w","29","384","1","-4","PSEUDO_MERSENNE","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","192")
        curve_selected=True
    if x==14:
        curveset("nums384e","29","384","1","0","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","192")
        curve_selected=True
    if x==15:
        curveset("nums512w","29","512","1","-4","PSEUDO_MERSENNE","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","256")
        curve_selected=True
    if x==16:
        curveset("nums512e","29","512","1","0","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","256")
        curve_selected=True

    if x==17:
#                                          ,"1", for SVDW
# set for SSWU plus isogenies
        curveset("secp256k1","28","256","1",["-11","3"],"NOT_SPECIAL","0","WEIERSTRASS","0","NOT","NOT","NOT","NOT","128")
        curve_selected=True
    if x==18:
        curveset("sm2","28","256","1","-9","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","128")
        curve_selected=True

    if x==19:
        curveset("c13318","29","255","2","2","PSEUDO_MERSENNE","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","128")
        curve_selected=True

    if x==20:
        curveset("jubjub","29","255","32","1","NOT_SPECIAL","5","EDWARDS","-1","NOT","NOT","NOT","NOT","128")
        curve_selected=True

    if x==21:
        curveset("x448","29","448","1","0","GENERALISED_MERSENNE","0","MONTGOMERY","156326","NOT","NOT","NOT","NOT","256")
        curve_selected=True

    if x==22:
        curveset("secp160r1","29","160","1","3","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","128")
        curve_selected=True

    if x==23:
        curveset("c1174","29","251","1","0","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","128")
        curve_selected=True
    if x==24:
        curveset("c1665","29","166","1","0","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","128")
        curve_selected=True

    if x==25:
        curveset("mdc","28","256","1","0","NOT_SPECIAL","0","EDWARDS","1","NOT","NOT","NOT","NOT","128")
        curve_selected=True


    if x==26:
        curveset("tweedledum","29","255","33","1","NOT_SPECIAL","5","WEIERSTRASS","0","NOT","NOT","NOT","NOT","128")
        curve_selected=True

    if x==27:
        curveset("tweedledee","29","255","34","1","NOT_SPECIAL","5","WEIERSTRASS","0","NOT","NOT","NOT","NOT","128")
        curve_selected=True

    pf=28

    if x==pf+0:
        curveset("bn254","28","254","1",["-1","-1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BN","D_TYPE","NEGATIVEX","66","128")
        pfcurve_selected=True
    if x==pf+1:
        curveset("bn254CX","28","254","1",["-1","-1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BN","D_TYPE","NEGATIVEX","66","128")
        pfcurve_selected=True
    if x==pf+2:
        curveset("bls12383","29","383","1",["1","1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS12","M_TYPE","POSITIVEX","65","128")
        pfcurve_selected=True

    if x==pf+3:
#                                          ["-3" ,"-1", "0"]  for SVDW
# set for SSWU plus isogenies
        curveset("bls12381","29","381","1",["11","-2","-1","11","3"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS12","M_TYPE","NEGATIVEX","65","128")
        pfcurve_selected=True

    if x==pf+4:
        curveset("fp256bn","28","256","1",["1","1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BN","M_TYPE","NEGATIVEX","66","128")
        pfcurve_selected=True
    if x==pf+5:
        curveset("fp512bn","29","512","1",["1","1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BN","M_TYPE","POSITIVEX","130","128")
        pfcurve_selected=True
# https://eprint.iacr.org/2017/334.pdf
    if x==pf+6:
        curveset("bls12461","28","461","1",["1","4","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS12","M_TYPE","NEGATIVEX","78","128")
        pfcurve_selected=True

    if x==pf+7:
        curveset("bn462","28","462","1",["1","1","0"],"NOT_SPECIAL","1","WEIERSTRASS","0","BN","D_TYPE","POSITIVEX","118","128")
        pfcurve_selected=True

    if x==pf+8:
        curveset("bls24479","29","479","1",["1","4","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS24","M_TYPE","POSITIVEX","49","192")
        pfcurve_selected=True

    if x==pf+9:
        curveset("bls48556","29","556","1",["-1","2","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS48","M_TYPE","POSITIVEX","32","256")
        pfcurve_selected=True

    if x==pf+10:
        curveset("bls48581","29","581","1",["2","2","0"],"NOT_SPECIAL","10","WEIERSTRASS","0","BLS48","D_TYPE","NEGATIVEX","33","256")
        pfcurve_selected=True

    if x==pf+11:
        curveset("bls48286","29","286","1",["1","1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS48","M_TYPE","POSITIVEX""17","128")
        pfcurve_selected=True

# rsaset(rsaname,big_length_bytes,bits_in_base,multiplier)
# The RSA name reflects the modulus size, which is a 2^m multiplier
# of the underlying big length

# There are choices here, different ways of getting the same result, but some faster than others
    if x==pf+12:
        #256 is slower but may allow reuse of 256-bit BIGs used for elliptic curve
        #512 is faster.. but best is 1024
        rsaset("rsa2048","128","28","2")
        #rsaset("rsa2048","64","29","60",4")
        #rsaset("rsa2048","32","29","56","8")
        rsa_selected=True
    if x==pf+13:
        rsaset("rsa3072","48","28","8")
        rsa_selected=True
    if x==pf+14:
        #rsaset("rsa4096","32","29","56",16")
        rsaset("rsa4096","64","29","8")
        rsa_selected=True

os.system(deltext+" hash*.swift")
os.system(deltext+" hmac.swift")
os.system(deltext+" sha3.swift")
os.system(deltext+" aes.swift")
os.system(deltext+" rand.swift")
os.system(deltext+" share.swift")
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

