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
import shutil
import fnmatch

testing=False
rsa_selected=False
curve_selected=False
pfcurve_selected=False

if len(sys.argv)==2 :
    if sys.argv[1]=="test":
        testing=True
if testing :
    sys.stdin=open("test16.txt","r")

compiler_path = ""

class miracl_interaction:
    def delete_file_using_expr(expression):
        for root, dirs, files in os.walk(os.path.abspath(os.path.dirname(__file__))):
            for name in files:
                if fnmatch.fnmatch(name, expression):
                    os.remove(os.path.join(root, name))

    def copy_file_to_working_dir(file, target):
        shutil.copyfile(file, target)

class miracl_compile:
    def detect_supported_compiler():
        search_paths = os.environ['PATH'].split(os.pathsep)
        for path in search_paths:
            for root, dirs, files in os.walk(path):
                for name in files:
                    if fnmatch.fnmatch(name, "*clang*") or fnmatch.fnmatch(name, "*gcc*"):
                        return os.path.join(root, name)
        return ""

    def compile_file(optim, file):
        print("Processing " + file + "..", end = "")
        global compiler_path
        if optim != 0:
            flags = " -std=c99 -O%d -c %s" % (optim, file)
        else:
            flags = " -std=c99 -c %s" % (file)
        rc = os.WEXITSTATUS(os.system(compiler_path + flags))
        if rc == 0:
            print(". [OK]")
        else:
            print(". [ERROR]")

    def compile_binary(optim, file, lib, bin):
        print("Processing " + file + "..", end = "")
        global compiler_path
        if sys.platform.startswith("win"):
            bin += ".exe"
        if optim != 0:
            flags = " -std=c99 -O%d %s %s -o %s" % (optim, file, lib, bin)
        else:
            flags = " -std=c99 %s %s -o %s" % (file, lib, bin)
        rc = os.WEXITSTATUS(os.system(compiler_path + flags))
        if rc == 0:
            print(". [OK]")
        else:
            print(". [ERROR]")

    def compiler_sanity_check():
        global compiler_path
        compiler_path = miracl_compile.detect_supported_compiler()
        return (compiler_path != "")

def inline_mul1(N,base)  :
    str=""
    str+="\tt=(dchunk)a[0]*b[0]; c[0]=(chunk)t & BMASK_XXX; t=t>>BASEBITS_XXX;\n"

    for i in range(1,N) :
        k=0;
        str+="\tt=t"
        while (k<=i) :
            str+="+(dchunk)a[{}]*b[{}]".format(k,i-k)
            k+=1
        str+="; c[{}]=(chunk)t & BMASK_XXX; ".format(i)
        str+="t=t>>BASEBITS_XXX;\n"

    for i in range(N,2*N-1) :
        k=i-(N-1)
        str+="\tt=t"
        while (k<=N-1) :
            str+="+(dchunk)a[{}]*b[{}]".format(k,i-k)
            k+=1
        str+="; c[{}]=(chunk)t & BMASK_XXX; ".format(i)
        str+="t=t>>BASEBITS_XXX;\n"

    str+="\tc[{}]=(chunk)t;\n".format(2*N-1)

    return str.replace("XXX",base)

def inline_mul2(N,base)  :
    str=""
    for i in range(0,N) :
        str+="\td[{}]=(dchunk)a[{}]*b[{}];\n".format(i, i, i)
    str+="\n\ts=d[0];\n\tt = s; c[0]=(chunk)t&BMASK_XXX; co=t>>BASEBITS_XXX;\n"
    for k in range(1,N) :
        str+="\ts+=d[{}]; t=co+s ".format(k)
        for i in range(k,int(k/2),-1) :
            str+="+(dchunk)(a[{}]-a[{}])*(b[{}]-b[{}])".format(i,k - i, k - i, i)
        str+="; c[{}]=(chunk)t&BMASK_XXX; co=t>>BASEBITS_XXX; \n".format(k)
    str+="\n"
    for k in range(N,2 * N - 1) :
        str+="\ts-=d[{}]; t=co+s ".format(k - N)
        for i in range(N-1,int(k/2),-1) :
            str+="+(dchunk)(a[{}]-a[{}])*(b[{}]-b[{}])".format(i, k - i, k - i, i)
        str+="; c[{}]=(chunk)t&BMASK_XXX; co=t>>BASEBITS_XXX; \n".format(k)

    str+="\tc[{}]=(chunk)co;\n".format(2 * N - 1)
    return str.replace("XXX",base)

def inline_sqr(N,base) :
    str=""
    str+="\n\tt=(dchunk)a[0]*a[0]; c[0]=(chunk)t&BMASK_XXX; co=t>>BASEBITS_XXX;\n"

    for k in range(1,N) :
        str+="\tt= "
        for i in range(k,int(k/2),-1) :
            str+="+(dchunk)a[{}]*a[{}]".format(i, k - i)
        str+="; t+=t; t+=co;"
        if k % 2 == 0 :
            str+=" t+=(dchunk)a[{}]*a[{}];".format(int(k/2), int(k/2))
        str+=" c[{}]=(chunk)t&BMASK_XXX; co=t>>BASEBITS_XXX; \n".format(k)
    str+="\n"

    for k in range(N,2*N-2) :
        str+="\tt= "
        for i in range(N-1,int(k/2),-1) :
            str+="+(dchunk)a[{}]*a[{}]".format(i, k - i)
        str+="; t+=t; t+=co;"
        if k % 2 == 0 :
            str+=" t+=(dchunk)a[{}]*a[{}];".format(int(k/2),int(k/2))
        str+=" c[{}]=(chunk)t&BMASK_XXX; co=t>>BASEBITS_XXX; \n".format(k)

    str+="\tt=co; t+=(dchunk)a[{}]*a[{}]; c[{}]=(chunk)t&BMASK_XXX; co=t>>BASEBITS_XXX; \n ".format(N-1,N-1,2*N-2)

    str+="\tc[{}]=(chunk)co;\n".format(2 * N - 1)
    return str.replace("XXX",base)

def inline_redc2(N,base) :
    str=""
    str+="\tt=d[0]; v[0]=((chunk)t*MC)&BMASK_XXX; t+=(dchunk)v[0]*md[0];  s=0; c=(t>>BASEBITS_XXX);\n\n"

    for k in range(1,N) :
        str+="\tt=d[{}]+c+s+(dchunk)v[0]*md[{}]".format(k, k)
        for i in range(k-1,int(k/2),-1) :
            str+="+(dchunk)(v[{}]-v[{}])*(md[{}]-md[{}])".format(k - i, i, i, k - i)
        str+="; v[{}]=((chunk)t*MC)&BMASK_XXX; t+=(dchunk)v[{}]*md[0]; ".format(k, k)
        str+=" dd[{}]=(dchunk)v[{}]*md[{}]; s+=dd[{}]; c=(t>>BASEBITS_XXX); \n".format(k, k, k, k)

    str+="\n"
    for k in range(N,2*N-1) :
        str+="\tt=d[{}]+c+s".format(k)
        for i in range(N-1,int(k/2),-1) :
            str+="+(dchunk)(v[{}]-v[{}])*(md[{}]-md[{}])".format(k - i, i, i, k - i)
        str+="; a[{}]=(chunk)t&BMASK_XXX;  s-=dd[{}]; c=(t>>BASEBITS_XXX); \n".format(k - N, k - N + 1)

    str+="\ta[{}]=d[{}]+((chunk)c&BMASK_XXX);\n".format(N-1,2*N-1)
    return str.replace("XXX",base)

def inline_redc1(N,base) :
    str=""

    str+="\tt = d[0];\n"
    str+="\tv[0] = ((chunk)t * MC)&BMASK_XXX;\n"
    str+="\tt += (dchunk)v[0] * md[0];\n"
    str+="\tt = (t >> BASEBITS_XXX) + d[1];\n"

    for i in range(1,N) :
        k=1
        str+="\tt += (dchunk)v[0] * md[{}] ".format(i)
        while k<i :
            str+="+ (dchunk)v[{}]*md[{}]".format(k,i-k)
            k+=1
        str+="; v[{}] = ((chunk)t * MC)&BMASK_XXX; ".format(i)
        str+="t += (dchunk)v[{}] * md[0]; ".format(i)
        str+="t = (t >> BASEBITS_XXX) + d[{}];\n".format(i+1)

    for i in range(N,2*N-1) :
        k=i-(N-1)
        str+="\tt=t "
        while k<=N-1 :
            str+="+ (dchunk)v[{}]*md[{}] ".format(k,i-k)
            k+=1
        str+="; a[{}] = (chunk)t & BMASK_XXX; ".format(i-N)
        str+="t = (t >> BASEBITS_XXX) + d[{}];\n".format(i+1)

    str+="\ta[{}] = (chunk)t & BMASK_XXX;\n".format(N-1)
    return str.replace("XXX",base)


def replace(namefile,oldtext,newtext):
    f = open(namefile,'r')
    filedata = f.read()
    f.close()

    newdata = filedata.replace(oldtext,newtext)

    f = open(namefile,'w')
    f.write(newdata)
    f.close()

# rsaset(big,ring,bit_bits_in_base,multiplier)
# for each choice give distinct names for "big" and "ring".
# Typically "big" is the length in bits of the underlying big number type
# "ring" is the RSA modulus size = "big" times 2^m
# Next give the number base used for 32 bit architectures, as n where the base is 2^n
# multiplier is 2^m (see above)
def rsaset(tb,tff,base,ml) :

    itb=int(tb)
    inb=int(itb/8)
    nb=str(inb)
    ib=int(base)
    inb=int(nb)

    nlen=(1+((8*inb-1)//ib))

    bd=tb+"_"+base
    fnameh="config_big_"+bd+".h"
    miracl_interaction.copy_file_to_working_dir("config_big.h", fnameh)
    replace(fnameh,"XXX",bd)
    replace(fnameh,"@NB@",nb)
    replace(fnameh,"@BASE@",base)

    fnameh="config_ff_"+tff+".h"
    miracl_interaction.copy_file_to_working_dir("config_ff.h", fnameh)
    replace(fnameh,"XXX",bd)
    replace(fnameh,"WWW",tff)
    replace(fnameh,"@ML@",ml);

    fnamec="big_"+bd+".c"
    fnameh="big_"+bd+".h"

    miracl_interaction.copy_file_to_working_dir("big.c", fnamec)
    miracl_interaction.copy_file_to_working_dir("big.h", fnameh)

    replace(fnamec,"XXX",bd)
    replace(fnameh,"XXX",bd)

    replace(fnamec,"INLINE_MUL1",inline_mul1(nlen,bd))
    replace(fnamec,"INLINE_MUL2",inline_mul2(nlen,bd))
    replace(fnamec,"INLINE_SQR",inline_sqr(nlen,bd))
    replace(fnamec,"INLINE_REDC1",inline_redc1(nlen,bd))
    replace(fnamec,"INLINE_REDC2",inline_redc2(nlen,bd))

    miracl_compile.compile_file(3, fnamec)

    fnamec="ff_"+tff+".c"
    fnameh="ff_"+tff+".h"

    miracl_interaction.copy_file_to_working_dir("ff.c", fnamec)
    miracl_interaction.copy_file_to_working_dir("ff.h", fnameh)

    replace(fnamec,"WWW",tff)
    replace(fnamec,"XXX",bd)
    replace(fnameh,"WWW",tff)
    replace(fnameh,"XXX",bd)
    miracl_compile.compile_file(3, fnamec)

    fnamec="rsa_"+tff+".c"
    fnameh="rsa_"+tff+".h"

    miracl_interaction.copy_file_to_working_dir("rsa.c", fnamec)
    miracl_interaction.copy_file_to_working_dir("rsa.h", fnameh)

    replace(fnamec,"WWW",tff)
    replace(fnamec,"XXX",bd)
    replace(fnameh,"WWW",tff)
    replace(fnameh,"XXX",bd)
    miracl_compile.compile_file(3, fnamec)

# curveset(modulus_bits,field,curve,bits_in_base,modulus_mod_8,Z,modulus_type,curve_type,Curve A,pairing_friendly,sextic twist,sign of x,g2_table size,ate bits,curve security)
# for each curve give names for field and curve. In many cases the latter two will be the same.
# modulus_bits is the bit length of the modulus, typically the same or slightly smaller than "big"
# Typically "field" describes the modulus, and "curve" is the common name for the elliptic curve
# Next give the number base used for 32 bit architecture, as n where the base is 2^n (note that these must be fixed for the same "big" name, if is ever re-used for another curve)
# m8 max m such that 2^m | modulus-1
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
def curveset(nbt,tf,tc,base,m8,rz,mt,qi,ct,ca,pf,stw,sx,g2,ab,cs) :
    inbt=int(nbt)
    itb=int(inbt+(8-inbt%8)%8)
    inb=int(itb/8)
    tb=str(itb)
    nb=str(inb)

    bd=tb+"_"+base
    fnameh="config_big_"+bd+".h"
    miracl_interaction.copy_file_to_working_dir("config_big.h", fnameh)
    replace(fnameh,"XXX",bd)
    replace(fnameh,"@NB@",nb)
    replace(fnameh,"@BASE@",base)

    fnameh="config_field_"+tf+".h"
    miracl_interaction.copy_file_to_working_dir("config_field.h", fnameh)
    replace(fnameh,"XXX",bd)
    replace(fnameh,"YYY",tf)
    replace(fnameh,"@NBT@",nbt)
    replace(fnameh,"@M8@",m8)
    replace(fnameh,"@MT@",mt)
    hc="0"
    hc2="0"
# Get Hash-to-Curve Z for G1 and G2

    if isinstance(rz,list) :
        if len(rz)==2 :     # Z followed by SSWU isogeny degree
            replace(fnameh,"@RZ@",rz[0])
            replace(fnameh,"@RZ2A@","0")
            replace(fnameh,"@RZ2B@","0")
            hc=rz[1]
        if len(rz)==3 :     # Z for G1 followed by Z for G2 (for SVDW)
            replace(fnameh,"@RZ@",rz[0])
            replace(fnameh,"@RZ2A@",rz[1])
            replace(fnameh,"@RZ2B@",rz[2])
        if len(rz)==5 :     # Z for G1, Z for G2, SSWU isogeny degree for G1, SSWU isogeny degree for G2
            replace(fnameh,"@RZ@",rz[0])
            replace(fnameh,"@RZ2A@",rz[1])
            replace(fnameh,"@RZ2B@",rz[2])
            hc=rz[3]
            hc2=rz[4]
    else :
        replace(fnameh,"@RZ@",rz)   # just Z for SSWU, or indicates RFC7748 or Generic for Elligator
        replace(fnameh,"@RZ2A@","0")
        replace(fnameh,"@RZ2B@","0")

    itw=int(qi)%10
    replace(fnameh,"@QI@",str(itw))
    if int(qi)//10 > 0 :
        replace(fnameh,"@TW@","POSITOWER")
    else :
        replace(fnameh,"@TW@","NEGATOWER")

    ib=int(base)
    inb=int(nb)
    inbt=int(nbt)
    nlen=(1+((8*inb-1)//ib))
    sh=ib*nlen-inbt

    if sh > 6 :
        sh=6
    replace(fnameh,"@SH@",str(sh))

    fnameh="config_curve_"+tc+".h"
    miracl_interaction.copy_file_to_working_dir("config_curve.h", fnameh)
    replace(fnameh,"XXX",bd)
    replace(fnameh,"YYY",tf)
    replace(fnameh,"ZZZ",tc)
    replace(fnameh,"@CT@",ct)
    replace(fnameh,"@CA@",ca)
    replace(fnameh,"@PF@",pf)

    replace(fnameh,"@ST@",stw)
    replace(fnameh,"@SX@",sx)
    replace(fnameh,"@CS@",cs)
    replace(fnameh,"@AB@",ab)
    replace(fnameh,"@G2@",g2)

    replace(fnameh,"@HC@",hc)
    replace(fnameh,"@HC2@",hc2)

    fnamec="big_"+bd+".c"
    fnameh="big_"+bd+".h"

    miracl_interaction.copy_file_to_working_dir("big.c", fnamec)
    miracl_interaction.copy_file_to_working_dir("big.h", fnameh)

    replace(fnamec,"XXX",bd)
    replace(fnameh,"XXX",bd)

    replace(fnamec,"INLINE_MUL1",inline_mul1(nlen,bd))
    replace(fnamec,"INLINE_MUL2",inline_mul2(nlen,bd))
    replace(fnamec,"INLINE_SQR",inline_sqr(nlen,bd))
    replace(fnamec,"INLINE_REDC1",inline_redc1(nlen,bd))
    replace(fnamec,"INLINE_REDC2",inline_redc2(nlen,bd))

    miracl_compile.compile_file(3, fnamec)

    fnamec="fp_"+tf+".c"
    fnameh="fp_"+tf+".h"

    miracl_interaction.copy_file_to_working_dir("fp.c", fnamec)
    miracl_interaction.copy_file_to_working_dir("fp.h", fnameh)

    replace(fnamec,"YYY",tf)
    replace(fnamec,"XXX",bd)
    replace(fnameh,"YYY",tf)
    replace(fnameh,"XXX",bd)
    miracl_compile.compile_file(3, fnamec)

    os.system("gcc -O3 -std=c99 -c rom_field_"+tf+".c");

    fnamec="ecp_"+tc+".c"
    fnameh="ecp_"+tc+".h"

    miracl_interaction.copy_file_to_working_dir("ecp.c", fnamec)
    miracl_interaction.copy_file_to_working_dir("ecp.h", fnameh)

    replace(fnamec,"ZZZ",tc)
    replace(fnamec,"YYY",tf)
    replace(fnamec,"XXX",bd)
    replace(fnameh,"ZZZ",tc)
    replace(fnameh,"YYY",tf)
    replace(fnameh,"XXX",bd)
    miracl_compile.compile_file(3, fnamec)

    fnamec="ecdh_"+tc+".c"
    fnameh="ecdh_"+tc+".h"

    miracl_interaction.copy_file_to_working_dir("ecdh.c", fnamec)
    miracl_interaction.copy_file_to_working_dir("ecdh.h", fnameh)

    replace(fnamec,"ZZZ",tc)
    replace(fnamec,"YYY",tf)
    replace(fnamec,"XXX",bd)
    replace(fnameh,"ZZZ",tc)
    replace(fnameh,"YYY",tf)
    replace(fnameh,"XXX",bd)
    miracl_compile.compile_file(3, fnamec)

    os.system("gcc -O3 -std=c99 -c rom_curve_"+tc+".c");

    if pf != "NOT_PF" :
        fnamec="fp2_"+tf+".c"
        fnameh="fp2_"+tf+".h"

        miracl_interaction.copy_file_to_working_dir("fp2.c", fnamec)
        miracl_interaction.copy_file_to_working_dir("fp2.h", fnameh)
        replace(fnamec,"YYY",tf)
        replace(fnamec,"XXX",bd)
        replace(fnameh,"YYY",tf)
        replace(fnameh,"XXX",bd)
        miracl_compile.compile_file(3, fnamec)

        fnamec="fp4_"+tf+".c"
        fnameh="fp4_"+tf+".h"

        miracl_interaction.copy_file_to_working_dir("fp4.c", fnamec)
        miracl_interaction.copy_file_to_working_dir("fp4.h", fnameh)
        replace(fnamec,"YYY",tf)
        replace(fnamec,"XXX",bd)
        replace(fnamec,"ZZZ",tc)
        replace(fnameh,"YYY",tf)
        replace(fnameh,"XXX",bd)
        replace(fnameh,"ZZZ",tc)
        miracl_compile.compile_file(3, fnamec)

        fnamec="fp12_"+tf+".c"
        fnameh="fp12_"+tf+".h"

        miracl_interaction.copy_file_to_working_dir("fp12.c", fnamec)
        miracl_interaction.copy_file_to_working_dir("fp12.h", fnameh)
        replace(fnamec,"YYY",tf)
        replace(fnamec,"XXX",bd)
        replace(fnamec,"ZZZ",tc)
        replace(fnameh,"YYY",tf)
        replace(fnameh,"XXX",bd)
        replace(fnameh,"ZZZ",tc)
        miracl_compile.compile_file(3, fnamec)

        fnamec="ecp2_"+tc+".c"
        fnameh="ecp2_"+tc+".h"

        miracl_interaction.copy_file_to_working_dir("ecp2.c", fnamec)
        miracl_interaction.copy_file_to_working_dir("ecp2.h", fnameh)
        replace(fnamec,"ZZZ",tc)
        replace(fnamec,"YYY",tf)
        replace(fnamec,"XXX",bd)
        replace(fnameh,"ZZZ",tc)
        replace(fnameh,"YYY",tf)
        replace(fnameh,"XXX",bd)
        miracl_compile.compile_file(3, fnamec)

        fnamec="pair_"+tc+".c"
        fnameh="pair_"+tc+".h"

        miracl_interaction.copy_file_to_working_dir("pair.c", fnamec)
        miracl_interaction.copy_file_to_working_dir("pair.h", fnameh)
        replace(fnamec,"ZZZ",tc)
        replace(fnamec,"YYY",tf)
        replace(fnamec,"XXX",bd)
        replace(fnameh,"ZZZ",tc)
        replace(fnameh,"YYY",tf)
        replace(fnameh,"XXX",bd)
        miracl_compile.compile_file(3, fnamec)

        fnamec="mpin_"+tc+".c"
        fnameh="mpin_"+tc+".h"

        miracl_interaction.copy_file_to_working_dir("mpin.c", fnamec)
        miracl_interaction.copy_file_to_working_dir("mpin.h", fnameh)
        replace(fnamec,"ZZZ",tc)
        replace(fnamec,"YYY",tf)
        replace(fnamec,"XXX",bd)
        replace(fnameh,"ZZZ",tc)
        replace(fnameh,"YYY",tf)
        replace(fnameh,"XXX",bd)
        miracl_compile.compile_file(3, fnamec)

        fnamec="bls_"+tc+".c"
        fnameh="bls_"+tc+".h"

        miracl_interaction.copy_file_to_working_dir("bls.c", fnamec)
        miracl_interaction.copy_file_to_working_dir("bls.h", fnameh)
        replace(fnamec,"ZZZ",tc)
        replace(fnamec,"YYY",tf)
        replace(fnamec,"XXX",bd)
        replace(fnameh,"ZZZ",tc)
        replace(fnameh,"YYY",tf)
        replace(fnameh,"XXX",bd)
        miracl_compile.compile_file(3, fnamec)

def sanity_checks():
    if not miracl_compile.compiler_sanity_check():
        raise Exception("Supported compiler not found in PATH variable, exiting!")

sanity_checks()

replace("arch.h","@WL@","16")
print("Elliptic Curves")
print("1. ED25519")
print("2. NUMS256E")
print("3. SECP160R1\n")

print("Pairing-Friendly Elliptic Curves")
print("4. BN254")
print("5. BN254CX")

print("RSA")
print("6. RSA2048")

while True:
    if testing :
        x=int(input())
    else :
        x=int(input("Choose a Scheme to support - 0 to finish: "))
    if x == 0:
        break
#    print("Choice= ",x)
    if x==1:
        curveset("255","F25519","ED25519","13","2","1","PSEUDO_MERSENNE","0","EDWARDS","-1","NOT_PF","","","","","128")
        curve_selected=True
    if x==2:
        curveset("256","F256PME","NUMS256E","13","1","1","PSEUDO_MERSENNE","0","EDWARDS","1","NOT_PF","","","","","128")
        curve_selected=True

    if x==3:
        curveset("160","SECP160R1","SECP160R1","13","1","3","NOT_SPECIAL","0","WEIERSTRASS","-3","NOT_PF","","","","","128")
        curve_selected=True


    if x==4:
        curveset("254","BN254","BN254","13","1",["-1","-1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BN_CURVE","D_TYPE","NEGATIVEX","71","66","128")
        pfcurve_selected=True
    if x==5:
        curveset("254","BN254CX","BN254CX","13",["-1","-1","0"],"NOT_SPECIAL","0","WEIERSTRASS","0","BN_CURVE","D_TYPE","NEGATIVEX","76","66","128")
        pfcurve_selected=True

# There are choices here, different ways of getting the same result, but some faster than others
    if x==6:
        #256 is slower but may allow reuse of 256-bit BIGs used for elliptic curve
        #512 is faster.. but best is 1024
        rsaset("256","2048","13","8")
        rsa_selected=True

# create library
miracl_compile.compile_file(3, "randapi.c")
miracl_compile.compile_file(3, "hash.c")
miracl_compile.compile_file(3, "hmac.c")
miracl_compile.compile_file(3, "rand.c")
miracl_compile.compile_file(3, "oct.c")
miracl_compile.compile_file(3, "share.c")
miracl_compile.compile_file(3, "aes.c")
miracl_compile.compile_file(3, "gcm.c")
miracl_compile.compile_file(3, "newhope.c")
miracl_compile.compile_file(3, "x509.c")

if sys.platform.startswith("win") :
    os.system("for %i in (*.o) do @echo %~nxi >> f.list")
    os.system("ar rc core.a @f.list")
    miracl_interaction.delete_file_using_expr("f.list")

else :
    os.system("ar rc core.a *.o")

miracl_interaction.delete_file_using_expr("*.o")

if testing :
    miracl_compile.compile_binary(2, "testecc.c", "core.a", "testecc")
    miracl_compile.compile_binary(2, "testmpin.c", "core.a", "testmpin")
    miracl_compile.compile_binary(2, "testbls.c", "core.a", "testbls")
    miracl_compile.compile_binary(2, "benchtest_all.c", "core.a", "benchtest_all")
