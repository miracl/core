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


def curveset(tc,base,nbt,m8,rz,mt,qi,ct,ca,pf,stw,sx,g2,ab,cs) :
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
    os.system(copytext+"rom_"+tc+"_32.rs "+fpath+"rom.rs")

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
    if sh > 14 :
        sh=14
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
        replace(fpath+"fp2.rs","xxx",tc)
        replace(fpath+"fp4.rs","xxx",tc)

        if pf == "BN" or pf == "BLS12" :

            os.system(copytext+"ecp2.rs "+fpath+"ecp2.rs")
            os.system(copytext+"fp12.rs "+fpath+"fp12.rs")
            os.system(copytext+"pair.rs "+fpath+"pair.rs")
            os.system(copytext+"mpin.rs "+fpath+"mpin.rs")
            os.system(copytext+"bls.rs "+fpath+"bls.rs")

            if hc2!="0" :
                replace(fpath+"ecp2.rs","CAHCZS","*/")
                replace(fpath+"ecp2.rs","CAHCZF","/*")
            else :
                replace(fpath+"ecp2.rs","CAHCNZS","*/")
                replace(fpath+"ecp2.rs","CAHCNZF","/*")

            replace(fpath+"fp12.rs","xxx",tc)
            replace(fpath+"ecp2.rs","xxx",tc)
            replace(fpath+"pair.rs","xxx",tc)
            replace(fpath+"mpin.rs","xxx",tc)
            replace(fpath+"bls.rs","xxx",tc)

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
            replace(fpath+"fp8.rs","xxx",tc)
            replace(fpath+"fp24.rs","xxx",tc)
            replace(fpath+"ecp4.rs","xxx",tc)
            replace(fpath+"pair4.rs","xxx",tc)
            replace(fpath+"mpin192.rs","xxx",tc)
            replace(fpath+"bls192.rs","xxx",tc)

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
            replace(fpath+"fp8.rs","xxx",tc)
            replace(fpath+"fp16.rs","xxx",tc)
            replace(fpath+"fp48.rs","xxx",tc)
            replace(fpath+"ecp8.rs","xxx",tc)
            replace(fpath+"pair8.rs","xxx",tc)
            replace(fpath+"mpin256.rs","xxx",tc)
            replace(fpath+"bls256.rs","xxx",tc)

            os.system(copytext+"modpf256.rs "+fpath+"mod.rs")

    else :
        os.system(copytext+"modecc.rs "+fpath+"mod.rs")

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
    if testing :
        sys.stdin=open("test.txt","r")

    chosen=[]
    cptr=0

    os.system("cargo new core")
    #os.system("mkdir core"+slashtext+"src")
    os.system(copytext+ "hash*.rs core"+slashtext+"src"+slashtext+".")
    os.system(copytext+ "hmac.rs core"+slashtext+"src"+slashtext+".")
    os.system(copytext+ "sha3.rs core"+slashtext+"src"+slashtext+".")
    os.system(copytext+ "rand.rs core"+slashtext+"src"+slashtext+".")
    os.system(copytext+ "share.rs core"+slashtext+"src"+slashtext+".")
    os.system(copytext+ "aes.rs core"+slashtext+"src"+slashtext+".")
    os.system(copytext+ "gcm.rs core"+slashtext+"src"+slashtext+".")
    os.system(copytext+ "nhs.rs core"+slashtext+"src"+slashtext+".")
    os.system(copytext+ "arch32.rs core"+slashtext+"src"+slashtext+"arch.rs")
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

    selection=set()
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
        # Prevent repetitions
        if x in selection:
            continue

        selection.add(x)
        ptr=ptr+1

        # curveset(curve,bits_in_base,modulus_bits,modulus_mod_8,Z,modulus_type,curve_type,Curve A,pairing_friendly,g2_table size,ate_bits,curve security)
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
        # g2_table size is number of entries in precomputed table
        # ate bits is number of bits in Ate parameter (from romgen program)
        # curve security is AES equivalent, rounded up.


        if x==1:
            curveset("ed25519","29","255","2","1","PSEUDO_MERSENNE","0","EDWARDS","-1","NOT","NOT","NOT","NOT","NOT","128")
            curve_selected=True
        if x==2:
            curveset("c25519","29","255","2","1","PSEUDO_MERSENNE","0","MONTGOMERY","486662","NOT","NOT","NOT","NOT","NOT","128")
            curve_selected=True
        if x==3:
            curveset("nist256","28","256","1","-10","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","128")
            curve_selected=True
        if x==4:
            curveset("brainpool","28","256","1","-3","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","128")
            curve_selected=True
        if x==5:
            curveset("anssi","28","256","1","-5","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","128")
            curve_selected=True

        if x==6:
            curveset("hifive","29","336","2","1","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","NOT","192")
            curve_selected=True
        if x==7:
            curveset("goldilocks","29","448","1","0","GENERALISED_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","NOT","256")
            curve_selected=True
        if x==8:
            curveset("nist384","29","384","1","-12","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","192")
            curve_selected=True
        if x==9:
            curveset("c41417","29","414","1","1","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","NOT","256")
            curve_selected=True
        if x==10:
            curveset("nist521","28","521","1","-4","PSEUDO_MERSENNE","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","256")
            curve_selected=True

        if x==11:
            curveset("nums256w","28","256","1","7","PSEUDO_MERSENNE","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","128")
            curve_selected=True
        if x==12:
            curveset("nums256e","29","256","1","0","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","NOT","128")
            curve_selected=True
        if x==13:
            curveset("nums384w","29","384","1","-4","PSEUDO_MERSENNE","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","192")
            curve_selected=True
        if x==14:
            curveset("nums384e","29","384","1","0","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","NOT","192")
            curve_selected=True
        if x==15:
            curveset("nums512w","29","512","1","-4","PSEUDO_MERSENNE","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","256")
            curve_selected=True
        if x==16:
            curveset("nums512e","29","512","1","0","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","NOT","256")
            curve_selected=True

        if x==17:
#                                              ,"1", for SVDW
# set for SSWU plus isogenies
            curveset("secp256k1","28","256","1",["-11","3"],"NOT_SPECIAL","0","WEIERSTRASS","0","NOT","NOT","NOT","NOT","NOT","128")
            curve_selected=True
        if x==18:
            curveset("sm2","28","256","1","-9","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","128")
            curve_selected=True

        if x==19:
            curveset("c13318","29","255","2","2","PSEUDO_MERSENNE","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","128")
            curve_selected=True

        if x==20:
            curveset("jubjub","29","255","32","1","NOT_SPECIAL","5","EDWARDS","-1","NOT","NOT","NOT","NOT","NOT","128")
            curve_selected=True

        if x==21:
            curveset("x448","29","448","1","0","GENERALISED_MERSENNE","0","MONTGOMERY","156326","NOT","NOT","NOT","NOT","NOT","256")
            curve_selected=True

        if x==22:
            curveset("secp160r1","29","160","1","3","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT","NOT","NOT","NOT","NOT","128")
            curve_selected=True

        if x==23:
            curveset("c1174","29","251","1","0","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","NOT","128")
            curve_selected=True
        if x==24:
            curveset("c1665","29","166","1","0","PSEUDO_MERSENNE","0","EDWARDS","1","NOT","NOT","NOT","NOT","NOT","128")
            curve_selected=True

        if x==25:
            curveset("mdc","28","256","1","0","NOT_SPECIAL","0","EDWARDS","1","NOT","NOT","NOT","NOT","NOT","128")
            curve_selected=True

        if x==26:
            curveset("tweedledum","29","255","33","1","NOT_SPECIAL","5","WEIERSTRASS","0","NOT","NOT","NOT","NOT","NOT","128")
            curve_selected=True

        if x==27:
            curveset("tweedledee","29","255","34","1","NOT_SPECIAL","5","WEIERSTRASS","0","NOT","NOT","NOT","NOT","NOT","128")
            curve_selected=True

        pf=28

        if x==pf+0:
            curveset("bn254","28","254","1",["-1","-1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BN","D_TYPE","NEGATIVEX","71","66","128")
            pfcurve_selected=True
        if x==pf+1:
            curveset("bn254CX","28","254","1",["-1","-1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BN","D_TYPE","NEGATIVEX","76","66","128")
            pfcurve_selected=True
        if x==pf+2:
            curveset("bls12383","29","383","1",["1","1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS12","M_TYPE","POSITIVEX","68","65","128")
            pfcurve_selected=True

        if x==pf+3:
#                                              ["-3" ,"-1", "0"]  for SVDW
# set for SSWU plus isogenies
            curveset("bls12381","29","381","1",["11","-2","-1","11","3"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS12","M_TYPE","NEGATIVEX","69","65","128")
            pfcurve_selected=True

        if x==pf+4:
            curveset("fp256bn","28","256","1",["1","1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BN","M_TYPE","NEGATIVEX","83","66","128")
            pfcurve_selected=True
        if x==pf+5:
            curveset("fp512bn","29","512","1",["1","1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BN","M_TYPE","POSITIVEX","172","130","128")
            pfcurve_selected=True
    # https://eprint.iacr.org/2017/334.pdf
        if x==pf+6:
            curveset("bls12461","28","461","1",["1","4","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS12","M_TYPE","NEGATIVEX","79","78","128")
            pfcurve_selected=True

        if x==pf+7:
            curveset("bn462","28","462","1",["1","1","0"],"NOT_SPECIAL","1","WEIERSTRASS","0","BN","D_TYPE","POSITIVEX","125","118","128")
            pfcurve_selected=True

        if x==pf+8:
            curveset("bls24479","29","479","1",["1","4","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS24","M_TYPE","POSITIVEX","52","49","192")
            pfcurve_selected=True

        if x==pf+9:
            curveset("bls48556","29","556","1",["-1","2","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS48","M_TYPE","POSITIVEX","35","32","256")
            pfcurve_selected=True

        if x==pf+10:
            curveset("bls48581","29","581","1",["2","2","0"],"NOT_SPECIAL","10","WEIERSTRASS","0","BLS48","D_TYPE","NEGATIVEX","36","33","256")
            pfcurve_selected=True

        if x==pf+11:
            curveset("bls48286","29","286","1",["1","1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BLS48","M_TYPE","POSITIVEX","20","17","128")
            pfcurve_selected=True


    # rsaset(rsaname,big_length_bytes,bits_in_base,multiplier)
    # The RSA name reflects the modulus size, which is a 2^m multiplier
    # of the underlying big length

    # There are choices here, different ways of getting the same result, but some faster than others
        if x==pf+12:
            #256 is slower but may allow reuse of 256-bit BIGs used for elliptic curve
            #512 is faster.. but best is 1024
            rsaset("rsa2048","128","28","2")
            #rsaset("RSA2048","64","29","4")
            #rsaset("RSA2048","32","29","8")
            rsa_selected=True
        if x==pf+13:
            rsaset("rsa3072","48","28","8")
            rsa_selected=True
        if x==pf+14:
            #rsaset("RSA4096","32","29","16")
            rsaset("rsa4096","64","29","8")
            rsa_selected=True

    os.system("cargo rustc --manifest-path core"+slashtext+"Cargo.toml --release --lib")
    #os.system("cargo rustc  --manifest-path core"+slashtext+"Cargo.toml --release  --lib -- -C opt-level=s")

    # -- --cfg D32
    #os.system("cargo rustc --target wasm32-unknown-emscripten --manifest-path core"+slashtext+"Cargo.toml --release --lib")

    os.system(deltext+" hash*.rs")
    os.system(deltext+" hmac.rs")
    os.system(deltext+" sha3.rs")
    os.system(deltext+" aes.rs")
    os.system(deltext+" rand.rs")
    os.system(deltext+" share.rs")
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


