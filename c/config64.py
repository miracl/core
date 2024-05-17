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
import getopt
import shutil
import fnmatch

intr=False
arg_options=False
testing=False
fast_fail=True
keep_querying=True
ignore_variables=False

my_compiler = "gcc -Wno-stringop-overflow"
generated_files = []

def copy_keep_file(file, target):
    shutil.copyfile(file, target)

def copy_temp_file(file, target):
    shutil.copyfile(file, target)
    generated_files.append(target)

def delete_file(expression):
    for root, dirs, files in os.walk(os.path.abspath(os.path.dirname(__file__))):
        for name in files:
            if fnmatch.fnmatch(name, expression):
                os.remove(os.path.join(root, name))

def request_compile(compiler_path, cflags, optim, file, lib, bin):
    flags = " -std=c99"
    if optim != 0:
        flags += " -O%d" % optim
    if cflags != None:
        flags += " %s" % cflags
    if lib == None and bin == None:
        flags += " -c %s" % (file)
    else:
        if sys.platform.startswith("win"):
            bin += ".exe"
        flags += " %s %s -o %s" % (file, lib, bin)
    print("Processing " + file + "..", end = "", flush=True)
    if sys.platform.startswith("win") :
        os.system(compiler_path + flags)
        print(". [DONE]")
    else :
        if os.WEXITSTATUS(os.system(compiler_path + flags)) == 0:
            print(". [DONE]")
        elif fast_fail:
            print("unable to process. Fast-fail enabled, quitting!")
            sys.exit(1)
        else:
            print(". [ERROR]")

class miracl_compile:
    def compile_file(optim, file):
        if (os.environ.get('CC') != None and not ignore_variables):
            request_compile(os.environ.get('CC'), os.environ.get('CFLAGS'), optim, file, None, None)
        else:
            request_compile(my_compiler, None, optim, file, None, None)

    def compile_binary(optim, file, lib, bin):
        if (os.environ.get('CC') != None and not ignore_variables):
            request_compile(os.environ.get('CC'), os.environ.get('CFLAGS'), optim, file, lib, bin)
        else:
            request_compile(my_compiler, None, optim, file, lib, bin)

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
    copy_keep_file("config_big.h", fnameh)
    replace(fnameh,"XXX",bd)
    replace(fnameh,"@NB@",nb)
    replace(fnameh,"@BASE@",base)

    fnameh="config_ff_"+tff+".h"
    copy_keep_file("config_ff.h", fnameh)
    replace(fnameh,"XXX",bd)
    replace(fnameh,"WWW",tff)
    replace(fnameh,"@ML@",ml);

    fnamec="big_"+bd+".c"
    fnameh="big_"+bd+".h"

    copy_temp_file("big.c", fnamec)
    copy_keep_file("big.h", fnameh)

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

    copy_temp_file("ff.c", fnamec)
    copy_keep_file("ff.h", fnameh)

    replace(fnamec,"WWW",tff)
    replace(fnamec,"XXX",bd)
    replace(fnameh,"WWW",tff)
    replace(fnameh,"XXX",bd)
    miracl_compile.compile_file(3, fnamec)

    fnamec="rsa_"+tff+".c"
    fnameh="rsa_"+tff+".h"

    copy_temp_file("rsa.c", fnamec)
    copy_keep_file("rsa.h", fnameh)

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
# modulus_type is NOT_SPECIAL, or PSEUDO_MERSENNE, or MONTGOMERY_Friendly, or GENERALISED_MERSENNE (supported for Ed448 only)
# i for Fp2 QNR 2^i+sqrt(-1) (relevant for PFCs only, else =0). Or QNR over Fp if p=1 mod 8
# curve_type is WEIERSTRASS, EDWARDS or MONTGOMERY
# Curve A parameter
# pairing_friendly is BN, BLS or NOT (if not pairing friendly)
# if pairing friendly. M or D type twist, and sign of the family parameter x
# g2_table size is number of entries in precomputed table
# ate bits is number of bits in Ate parameter (from romgen program)
# curve security is AES equiavlent, rounded up.

def curveset(nbt,tf,tc,base,m8,rz,mt,qi,ct,ca,pf,stw,sx,g2,ab,cs) :

    inbt=int(nbt)
    itb=int(inbt+(8-inbt%8)%8)
    inb=int(itb/8)
    tb=str(itb)
    nb=str(inb)

    bd=tb+"_"+base
    fnameh="config_big_"+bd+".h"
    copy_keep_file("config_big.h", fnameh)

    replace(fnameh,"XXX",bd)
    replace(fnameh,"@NB@",nb)
    replace(fnameh,"@BASE@",base)

    fnameh="config_field_"+tf+".h"
    copy_keep_file("config_field.h", fnameh)

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

    if sh > 30 :
        sh=30
    replace(fnameh,"@SH@",str(sh))

    fnameh="config_curve_"+tc+".h"
    copy_keep_file("config_curve.h", fnameh)

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

    copy_temp_file("big.c", fnamec)
    copy_keep_file("big.h", fnameh)

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

    copy_temp_file("fp.c", fnamec)
    copy_keep_file("fp.h", fnameh)

    replace(fnamec,"YYY",tf)
    replace(fnamec,"XXX",bd)
    replace(fnameh,"YYY",tf)
    replace(fnameh,"XXX",bd)
    miracl_compile.compile_file(3, fnamec)

    miracl_compile.compile_file(3, "rom_field_"+tf+".c");

    fnamec="ecp_"+tc+".c"
    fnameh="ecp_"+tc+".h"

    copy_temp_file("ecp.c", fnamec)
    copy_keep_file("ecp.h", fnameh)

    replace(fnamec,"ZZZ",tc)
    replace(fnamec,"YYY",tf)
    replace(fnamec,"XXX",bd)
    replace(fnameh,"ZZZ",tc)
    replace(fnameh,"YYY",tf)
    replace(fnameh,"XXX",bd)
    miracl_compile.compile_file(3, fnamec)

    fnamec="ecdh_"+tc+".c"
    fnameh="ecdh_"+tc+".h"

    copy_temp_file("ecdh.c", fnamec)
    copy_keep_file("ecdh.h", fnameh)

    replace(fnamec,"ZZZ",tc)
    replace(fnamec,"YYY",tf)
    replace(fnamec,"XXX",bd)
    replace(fnameh,"ZZZ",tc)
    replace(fnameh,"YYY",tf)
    replace(fnameh,"XXX",bd)
    miracl_compile.compile_file(3, fnamec)


    fnamec="eddsa_"+tc+".c"
    fnameh="eddsa_"+tc+".h"

    copy_temp_file("eddsa.c", fnamec)
    copy_keep_file("eddsa.h", fnameh)

    replace(fnamec,"ZZZ",tc)
    replace(fnamec,"YYY",tf)
    replace(fnamec,"XXX",bd)
    replace(fnameh,"ZZZ",tc)
    replace(fnameh,"YYY",tf)
    replace(fnameh,"XXX",bd)
    miracl_compile.compile_file(3, fnamec)

    fnamec="hpke_"+tc+".c"
    fnameh="hpke_"+tc+".h"

    copy_temp_file("hpke.c", fnamec)
    copy_keep_file("hpke.h", fnameh)

    replace(fnamec,"ZZZ",tc)
    replace(fnameh,"ZZZ",tc)
    miracl_compile.compile_file(3, fnamec)

    miracl_compile.compile_file(3, "rom_curve_"+tc+".c");

    if pf != "NOT_PF" :
        fnamec="fp2_"+tf+".c"
        fnameh="fp2_"+tf+".h"

        copy_temp_file("fp2.c", fnamec)
        copy_keep_file("fp2.h", fnameh)
        replace(fnamec,"YYY",tf)
        replace(fnamec,"XXX",bd)
        replace(fnameh,"YYY",tf)
        replace(fnameh,"XXX",bd)
        miracl_compile.compile_file(3, fnamec)

        fnamec="fp4_"+tf+".c"
        fnameh="fp4_"+tf+".h"

        copy_temp_file("fp4.c", fnamec)
        copy_keep_file("fp4.h", fnameh)

        replace(fnamec,"YYY",tf)
        replace(fnamec,"XXX",bd)
        replace(fnamec,"ZZZ",tc)
        replace(fnameh,"YYY",tf)
        replace(fnameh,"XXX",bd)
        replace(fnameh,"ZZZ",tc)
        miracl_compile.compile_file(3, fnamec)

        if pf == "BN_CURVE" or pf == "BLS12_CURVE" :
            fnamec="fp12_"+tf+".c"
            fnameh="fp12_"+tf+".h"

            copy_temp_file("fp12.c", fnamec)
            copy_keep_file("fp12.h", fnameh)
 
            replace(fnamec,"YYY",tf)
            replace(fnamec,"XXX",bd)
            replace(fnamec,"ZZZ",tc)
            replace(fnameh,"YYY",tf)
            replace(fnameh,"XXX",bd)
            replace(fnameh,"ZZZ",tc)
            miracl_compile.compile_file(3, fnamec)

            fnamec="ecp2_"+tc+".c"
            fnameh="ecp2_"+tc+".h"

            copy_temp_file("ecp2.c", fnamec)
            copy_keep_file("ecp2.h", fnameh)

            replace(fnamec,"ZZZ",tc)
            replace(fnamec,"YYY",tf)
            replace(fnamec,"XXX",bd)
            replace(fnameh,"ZZZ",tc)
            replace(fnameh,"YYY",tf)
            replace(fnameh,"XXX",bd)
            miracl_compile.compile_file(3, fnamec)

            fnamec="pair_"+tc+".c"
            fnameh="pair_"+tc+".h"

            copy_temp_file("pair.c", fnamec)
            copy_keep_file("pair.h", fnameh)

            replace(fnamec,"ZZZ",tc)
            replace(fnamec,"YYY",tf)
            replace(fnamec,"XXX",bd)
            replace(fnameh,"ZZZ",tc)
            replace(fnameh,"YYY",tf)
            replace(fnameh,"XXX",bd)
            miracl_compile.compile_file(3, fnamec)

            fnamec="mpin_"+tc+".c"
            fnameh="mpin_"+tc+".h"

            copy_temp_file("mpin.c", fnamec)
            copy_keep_file("mpin.h", fnameh)
 
            replace(fnamec,"ZZZ",tc)
            replace(fnamec,"YYY",tf)
            replace(fnamec,"XXX",bd)
            replace(fnameh,"ZZZ",tc)
            replace(fnameh,"YYY",tf)
            replace(fnameh,"XXX",bd)
            miracl_compile.compile_file(3, fnamec)

            fnamec="bls_"+tc+".c"
            fnameh="bls_"+tc+".h"

            copy_temp_file("bls.c", fnamec)
            copy_keep_file("bls.h", fnameh)

            replace(fnamec,"ZZZ",tc)
            replace(fnamec,"YYY",tf)
            replace(fnamec,"XXX",bd)
            replace(fnameh,"ZZZ",tc)
            replace(fnameh,"YYY",tf)
            replace(fnameh,"XXX",bd)
            miracl_compile.compile_file(3, fnamec)

        if pf == "BLS24_CURVE" :
            fnamec="fp8_"+tf+".c"
            fnameh="fp8_"+tf+".h"

            copy_temp_file("fp8.c", fnamec)
            copy_keep_file("fp8.h", fnameh)
 
            replace(fnamec,"YYY",tf)
            replace(fnamec,"XXX",bd)
            replace(fnamec,"ZZZ",tc)
            replace(fnameh,"YYY",tf)
            replace(fnameh,"XXX",bd)
            replace(fnameh,"ZZZ",tc)
            miracl_compile.compile_file(3, fnamec)


            fnamec="fp24_"+tf+".c"
            fnameh="fp24_"+tf+".h"

            copy_temp_file("fp24.c", fnamec)
            copy_keep_file("fp24.h", fnameh)

            replace(fnamec,"YYY",tf)
            replace(fnamec,"XXX",bd)
            replace(fnamec,"ZZZ",tc)
            replace(fnameh,"YYY",tf)
            replace(fnameh,"XXX",bd)
            replace(fnameh,"ZZZ",tc)
            miracl_compile.compile_file(3, fnamec)

            fnamec="ecp4_"+tc+".c"
            fnameh="ecp4_"+tc+".h"

            copy_temp_file("ecp4.c", fnamec)
            copy_keep_file("ecp4.h", fnameh)

            replace(fnamec,"ZZZ",tc)
            replace(fnamec,"YYY",tf)
            replace(fnamec,"XXX",bd)
            replace(fnameh,"ZZZ",tc)
            replace(fnameh,"YYY",tf)
            replace(fnameh,"XXX",bd)
            miracl_compile.compile_file(3, fnamec)

            fnamec="pair4_"+tc+".c"
            fnameh="pair4_"+tc+".h"

            copy_temp_file("pair4.c", fnamec)
            copy_keep_file("pair4.h", fnameh)

            replace(fnamec,"ZZZ",tc)
            replace(fnamec,"YYY",tf)
            replace(fnamec,"XXX",bd)
            replace(fnameh,"ZZZ",tc)
            replace(fnameh,"YYY",tf)
            replace(fnameh,"XXX",bd)
            miracl_compile.compile_file(3, fnamec)

            fnamec="mpin192_"+tc+".c"
            fnameh="mpin192_"+tc+".h"

            copy_temp_file("mpin192.c", fnamec)
            copy_keep_file("mpin192.h", fnameh)

            replace(fnamec,"ZZZ",tc)
            replace(fnamec,"YYY",tf)
            replace(fnamec,"XXX",bd)
            replace(fnameh,"ZZZ",tc)
            replace(fnameh,"YYY",tf)
            replace(fnameh,"XXX",bd)
            miracl_compile.compile_file(3, fnamec)

            fnamec="bls192_"+tc+".c"
            fnameh="bls192_"+tc+".h"

            copy_temp_file("bls192.c", fnamec)
            copy_keep_file("bls192.h", fnameh)
 
            replace(fnamec,"ZZZ",tc)
            replace(fnamec,"YYY",tf)
            replace(fnamec,"XXX",bd)
            replace(fnameh,"ZZZ",tc)
            replace(fnameh,"YYY",tf)
            replace(fnameh,"XXX",bd)
            miracl_compile.compile_file(3, fnamec)

        if pf == "BLS48_CURVE" :

            fnamec="fp8_"+tf+".c"
            fnameh="fp8_"+tf+".h"

            copy_temp_file("fp8.c", fnamec)
            copy_keep_file("fp8.h", fnameh)
 
            replace(fnamec,"YYY",tf)
            replace(fnamec,"XXX",bd)
            replace(fnamec,"ZZZ",tc)
            replace(fnameh,"YYY",tf)
            replace(fnameh,"XXX",bd)
            replace(fnameh,"ZZZ",tc)
            miracl_compile.compile_file(3, fnamec)


            fnamec="ecp8_"+tc+".c"
            fnameh="ecp8_"+tc+".h"

            copy_temp_file("ecp8.c", fnamec)
            copy_keep_file("ecp8.h", fnameh)

            replace(fnamec,"ZZZ",tc)
            replace(fnamec,"YYY",tf)
            replace(fnamec,"XXX",bd)
            replace(fnameh,"ZZZ",tc)
            replace(fnameh,"YYY",tf)
            replace(fnameh,"XXX",bd)
            miracl_compile.compile_file(3, fnamec)


            fnamec="fp16_"+tf+".c"
            fnameh="fp16_"+tf+".h"

            copy_temp_file("fp16.c", fnamec)
            copy_keep_file("fp16.h", fnameh)
 
            replace(fnamec,"YYY",tf)
            replace(fnamec,"XXX",bd)
            replace(fnamec,"ZZZ",tc)
            replace(fnameh,"YYY",tf)
            replace(fnameh,"XXX",bd)
            replace(fnameh,"ZZZ",tc)
            miracl_compile.compile_file(3, fnamec)


            fnamec="fp48_"+tf+".c"
            fnameh="fp48_"+tf+".h"

            copy_temp_file("fp48.c", fnamec)
            copy_keep_file("fp48.h", fnameh)
 
            replace(fnamec,"YYY",tf)
            replace(fnamec,"XXX",bd)
            replace(fnamec,"ZZZ",tc)
            replace(fnameh,"YYY",tf)
            replace(fnameh,"XXX",bd)
            replace(fnameh,"ZZZ",tc)
            miracl_compile.compile_file(3, fnamec)


            fnamec="pair8_"+tc+".c"
            fnameh="pair8_"+tc+".h"

            copy_temp_file("pair8.c", fnamec)
            copy_keep_file("pair8.h", fnameh)
  
            replace(fnamec,"ZZZ",tc)
            replace(fnamec,"YYY",tf)
            replace(fnamec,"XXX",bd)
            replace(fnameh,"ZZZ",tc)
            replace(fnameh,"YYY",tf)
            replace(fnameh,"XXX",bd)
            miracl_compile.compile_file(3, fnamec)

            fnamec="mpin256_"+tc+".c"
            fnameh="mpin256_"+tc+".h"

            copy_temp_file("mpin256.c", fnamec)
            copy_keep_file("mpin256.h", fnameh)

            replace(fnamec,"ZZZ",tc)
            replace(fnamec,"YYY",tf)
            replace(fnamec,"XXX",bd)
            replace(fnameh,"ZZZ",tc)
            replace(fnameh,"YYY",tf)
            replace(fnameh,"XXX",bd)
            miracl_compile.compile_file(3, fnamec)

            fnamec="bls256_"+tc+".c"
            fnameh="bls256_"+tc+".h"

            copy_temp_file("bls256.c", fnamec)
            copy_keep_file("bls256.h", fnameh)
 
            replace(fnamec,"ZZZ",tc)
            replace(fnamec,"YYY",tf)
            replace(fnamec,"XXX",bd)
            replace(fnameh,"ZZZ",tc)
            replace(fnameh,"YYY",tf)
            replace(fnameh,"XXX",bd)
            miracl_compile.compile_file(3, fnamec)

class miracl_crypto:
    np_curves = (
        ( "255", "F25519", "Ed25519", "56", "2", "1", "PSEUDO_MERSENNE", "0", "EDWARDS", "-1", "NOT_PF", "", "", "", "", "128" ),
        ( "255", "F25519", "C25519", "56", "2", "1", "PSEUDO_MERSENNE", "0", "MONTGOMERY", "486662", "NOT_PF", "", "", "", "", "128" ),
        ( "256", "NIST256", "NIST256", "56", "1", "-10", "NOT_SPECIAL", "0", "WEIERSTRASS", "-3", "NOT_PF", "", "", "", "", "128" ),
        ( "256", "BRAINPOOL", "BRAINPOOL", "56", "1", "-3", "NOT_SPECIAL", "0", "WEIERSTRASS", "-3", "NOT_PF", "", "", "", "", "128" ),
        ( "256", "ANSSI", "ANSSI", "56", "1", "-5", "NOT_SPECIAL", "0", "WEIERSTRASS", "-3", "NOT_PF", "", "", "", "", "128" ),
        ( "336", "HIFIVE", "HIFIVE", "60", "2", "1", "PSEUDO_MERSENNE", "0", "EDWARDS", "1", "NOT_PF", "", "", "", "", "192" ),
        ( "448", "F448", "Ed448", "58", "1", "0", "GENERALISED_MERSENNE", "0", "EDWARDS", "1", "NOT_PF", "", "", "", "", "256" ),
        ( "384", "NIST384", "NIST384", "56", "1", "-12", "NOT_SPECIAL", "0", "WEIERSTRASS", "-3", "NOT_PF", "", "", "", "", "192" ),
        ( "414", "C41417", "C41417", "60", "1", "1", "PSEUDO_MERSENNE", "0", "EDWARDS", "1", "NOT_PF", "", "", "", "", "256" ),
        ( "521", "NIST521", "NIST521", "60", "1", "-4", "PSEUDO_MERSENNE", "0", "WEIERSTRASS", "-3", "NOT_PF", "", "", "", "", "256" ),
        ( "256", "F256PMW", "NUMS256W", "56", "1", "7", "PSEUDO_MERSENNE", "0", "WEIERSTRASS", "-3", "NOT_PF", "", "", "", "", "128" ),
        ( "256", "F256PME", "NUMS256E", "56", "1", "0", "PSEUDO_MERSENNE", "0", "EDWARDS", "1", "NOT_PF", "", "", "", "", "128" ),
        ( "384", "F384PM", "NUMS384W", "58", "1", "-4", "PSEUDO_MERSENNE", "0", "WEIERSTRASS", "-3", "NOT_PF", "", "", "", "", "192" ),
        ( "384", "F384PM", "NUMS384E", "58", "1", "0", "PSEUDO_MERSENNE", "0", "EDWARDS", "1", "NOT_PF", "", "", "", "", "192" ),
        ( "512", "F512PM", "NUMS512W", "60", "1", "-4", "PSEUDO_MERSENNE", "0", "WEIERSTRASS", "-3", "NOT_PF", "", "", "", "", "256" ),
        ( "512", "F512PM", "NUMS512E", "60", "1", "0", "PSEUDO_MERSENNE", "0", "EDWARDS", "1", "NOT_PF", "", "", "", "", "256" ),
        #                                            ,"1", for SVDW
        # set for SSWU plus isogenies
        ( "256", "SECP256K1", "SECP256K1", "56", "1",["-11", "3"], "NOT_SPECIAL", "0", "WEIERSTRASS", "0", "NOT_PF", "", "", "", "", "128" ),
        ( "256", "SM2", "SM2", "56", "1", "-9", "NOT_SPECIAL", "0", "WEIERSTRASS", "-3", "NOT_PF", "", "", "", "", "128" ),
        ( "255", "F25519", "C13318", "56", "2", "2", "PSEUDO_MERSENNE", "0", "WEIERSTRASS", "-3", "NOT_PF", "", "", "", "", "128" ),
        ( "255", "JUBJUB", "JUBJUB", "56", "32", "1", "NOT_SPECIAL", "5", "EDWARDS", "-1", "NOT_PF", "", "", "", "", "128" ),
        ( "448", "F448", "X448", "58", "1", "0", "GENERALISED_MERSENNE", "0", "MONTGOMERY", "156326", "NOT_PF", "", "", "", "", "256" ),
        ( "160", "SECP160R1", "SECP160R1", "56", "1", "3", "NOT_SPECIAL", "0", "WEIERSTRASS", "-3", "NOT_PF", "", "", "", "", "128" ),
        ( "251", "C1174", "C1174", "56", "1", "0", "PSEUDO_MERSENNE", "0", "EDWARDS", "1", "NOT_PF", "", "", "", "", "128" ),
        ( "166", "C1665", "C1665", "60", "1", "0", "PSEUDO_MERSENNE", "0", "EDWARDS", "1", "NOT_PF", "", "", "", "", "128" ),
        ( "256", "MDC", "MDC", "56", "1", "0", "NOT_SPECIAL", "0", "EDWARDS", "1", "NOT_PF", "", "", "", "", "128" ),
        ( "255", "TWEEDLEDUM", "TWEEDLEDUM", "56", "33", "1", "NOT_SPECIAL", "5", "WEIERSTRASS", "0", "NOT_PF", "", "", "", "", "128" ),
        ( "255", "TWEEDLEDEE", "TWEEDLEDEE", "56", "34", "1", "NOT_SPECIAL", "5", "WEIERSTRASS", "0", "NOT_PF", "", "", "", "", "128" )
    )

    pf_curves = (
        ( "254", "BN254", "BN254", "56", "1",["-1", "-1", "0"], "NOT_SPECIAL", "0", "WEIERSTRASS", "0", "BN_CURVE", "D_TYPE", "NEGATIVEX", "71", "66", "128" ),
        ( "254", "BN254CX", "BN254CX", "56", "1",["-1", "-1", "0"], "NOT_SPECIAL", "0", "WEIERSTRASS", "0", "BN_CURVE", "D_TYPE", "NEGATIVEX", "76", "66", "128" ),
        ( "383", "BLS12383", "BLS12383", "58", "1",["1", "1", "0"], "NOT_SPECIAL", "0", "WEIERSTRASS", "0", "BLS12_CURVE", "M_TYPE", "POSITIVEX", "68", "65", "128" ),
        #                                           ["-3" ,"-1", "0"]  for SVDW
        # set for SSWU plus isogenies
        ( "381", "BLS12381", "BLS12381", "58", "1",["11", "-2", "-1", "11", "3"], "NOT_SPECIAL", "0", "WEIERSTRASS", "0", "BLS12_CURVE", "M_TYPE", "NEGATIVEX", "69", "65", "128" ),
        ( "256", "FP256BN", "FP256BN", "56", "1",["1", "1", "0"], "NOT_SPECIAL", "0", "WEIERSTRASS", "0", "BN_CURVE", "M_TYPE", "NEGATIVEX", "83", "66", "128" ),
        ( "512", "FP512BN", "FP512BN", "60", "1",["1", "1", "0"], "NOT_SPECIAL", "0", "WEIERSTRASS", "0", "BN_CURVE", "M_TYPE", "POSITIVEX", "172", "130", "128" ),
        ( "443", "BLS12443", "BLS12443", "60", "1",["-7", "1", "1", "11", "3"], "NOT_SPECIAL", "0", "WEIERSTRASS", "0", "BLS12_CURVE", "M_TYPE", "POSITIVEX", "78", "75", "128" ),
        # https://eprint.iacr.org/2017/334.pdf
        ( "461", "BLS12461", "BLS12461", "60", "1",["1", "4", "0"], "NOT_SPECIAL", "0", "WEIERSTRASS", "0", "BLS12_CURVE", "M_TYPE", "NEGATIVEX", "79", "78", "128" ),
        ( "462", "BN462", "BN462", "60", "1",["1", "1", "0"], "NOT_SPECIAL", "1", "WEIERSTRASS", "0", "BN_CURVE", "D_TYPE", "POSITIVEX", "125", "118", "128" ),
        ( "479", "BLS24479", "BLS24479", "56", "1",["1", "4", "0"], "NOT_SPECIAL", "0", "WEIERSTRASS", "0", "BLS24_CURVE", "M_TYPE", "POSITIVEX", "52", "49", "192" ),
        ( "556", "BLS48556", "BLS48556", "58", "1",["-1", "2", "0"], "NOT_SPECIAL", "0", "WEIERSTRASS", "0", "BLS48_CURVE", "M_TYPE", "POSITIVEX", "35", "32", "256" ),
        ( "581", "BLS48581", "BLS48581", "60", "1",["2", "2", "0"], "NOT_SPECIAL", "10", "WEIERSTRASS", "0", "BLS48_CURVE", "D_TYPE", "NEGATIVEX", "36", "33", "256" ),
        ( "286", "BLS48286", "BLS48286", "60", "1",["1", "1", "0"], "NOT_SPECIAL", "0", "WEIERSTRASS", "0", "BLS48_CURVE", "M_TYPE", "POSITIVEX", "20", "17", "128" ),
        ( "158", "BN158", "BN158", "56","1",["1", "1", "0"], "NOT_SPECIAL", "0", "WEIERSTRASS", "0","BN_CURVE","M_TYPE","NEGATIVEX","49","42","128")
    )

    # There are choices here, different ways of getting the same result, but some faster than others
    rsa_params = (
        # 256 is slower but may allow reuse of 256-bit BIGs used for elliptic curve
        # 512 is faster.. but best is 1024
        #("1024", "2048", "58", "2"),
        ("512", "2048", "60", "4"),
        #("256", "2048", "56", "8"),
        ("384", "3072", "56", "8"),
        #("256", "4096", "56", "16"),
        ("512", "4096", "60", "8")
    )

    min_core_library = (
        "randapi.c",
        "hash.c",
        "hmac.c",
        "rand.c",
        "oct.c",
        "share.c",
        "aes.c",
        "gcm.c",
        "newhope.c",
        "dilithium.c",
        "kyber.c",
        "x509.c"
    )

    total_entries = len(np_curves)+len(pf_curves)+len(rsa_params)

    def valid_query(number):
        return number >= 0 and number <= miracl_crypto.total_entries

def interactive_prompt_print():
    index = 1
    print("Elliptic Curves")
    for tuple in miracl_crypto.np_curves:
        print(str(index) + ".", tuple[2])
        index += 1

    print("\nPairing-Friendly Elliptic Curves")
    for tuple in miracl_crypto.pf_curves:
        print(str(index) + ".", tuple[2])
        index += 1

    print("\nRSA")
    for tuple in miracl_crypto.rsa_params:
        print(str(index) + ".", "RSA" + str(tuple[1]))
        index += 1

def interactive_prompt_exect(index):
    index -= 1 # Python internally is zero-indexed
    if index < len(miracl_crypto.np_curves):
        tuple = miracl_crypto.np_curves[index]
        curveset(
            tuple[0], tuple[1], tuple[2], tuple[3], tuple[4],
            tuple[5], tuple[6], tuple[7], tuple[8], tuple[9],
            tuple[10], tuple[11], tuple[12],
            tuple[13], tuple[14], tuple[15]
        )
        #curve_selected=True
    elif index < len(miracl_crypto.np_curves) + len(miracl_crypto.pf_curves):
        tuple = miracl_crypto.pf_curves[index-len(miracl_crypto.np_curves)]
        curveset(
            tuple[0], tuple[1], tuple[2], tuple[3], tuple[4],
            tuple[5], tuple[6], tuple[7], tuple[8], tuple[9],
            tuple[10], tuple[11], tuple[12],
            tuple[13], tuple[14], tuple[15]
        )
        #pfcurve_selected=True
    else:
        tuple = miracl_crypto.rsa_params[index-(len(miracl_crypto.np_curves)+len(miracl_crypto.pf_curves))]
        rsaset(
            tuple[0], tuple[1], tuple[2], tuple[3]
        )
        #rsa_selected=True

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

def usage():
    print("Usage: ./config64.py [OPTIONS] [ARGUMENTS]\n")
    print("Option              Long Option              Action")
    print("   -h, -?           --help                   Shows this message\n")
    print("Script Behaviour:")
    print("   -d               --disable-fastfail       Disable script termination if build fails (default: false)")
    print("   -i               --ignore-environment     Do not read environment variables, rely on shell instead (default: false)\n")
    print("Build Configuration:")
    print("   -o               --options=1 --options=64 Disable interactive mode and select specific options")
    print("   -r               --run-tests              Disable interactive mode, select all options, build and run tests")
    print("                                             (implies --disable-fastfail, --ignore-environment)\n")

def arg_manager(argv, build_options):
    global testing, arg_options, fast_fail, intr, ignore_variables
    # Account for legacy behaviour
    # Should be removed in future releases
    if len(argv) == 0:
        intr = True
        return False
    if len(argv) == 1:
        if (argv[0] == "test"):
            testing = True
            return False
    # Now let's try to actually make sense of arguments
    try:
        opts, args = getopt.getopt(
            argv,
            "hdiro:",
            ["help", "disable-fastfail", "ignore-environment", "run-tests", "options="]
        )
    except getopt.GetoptError as err:
        print(err)
        usage()
        sys.exit(2)

    for opt, arg in opts:
        if opt in ['-h', '--help']:
            usage()
            sys.exit(0)
        elif opt in ['-d', '--disable-fastfail']:
            fast_fail = False
        elif opt in ['-i', '--ignore-environment']:
            ignore_variables = True
        elif opt in ['-r', '--run-tests']:
            testing = True
            fast_fail = False
            ignore_variables = True
        elif opt in ['-o', '--options']:
            arg_options = True
            try:
                intarg = int(arg)
                if (intarg <= miracl_crypto.total_entries and intarg > 0):
                    build_options.append(intarg)
                else:
                    raise ValueError("Value too low or too high!")
            except:
                print("config64.py: Error! You have passed an invalid option, exiting.")
                print("             If you are trying to select multiple options, please repeat the flag\n")
                print("             e.g. ")
                print("                  ./config64.py -o 1 -o 23 -o 25")
                sys.exit(1)
    return len(opts) > 0 and not testing and arg_options

def main(argv):
    global testing, keep_querying, my_compiler, generated_files, arg_options, intr
    options_list = []

    replace("arch.h","@WL@","64")

    if not arg_manager(argv, options_list):
        if intr:
            if not testing:
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
        elif not arg_options and not testing:
            print("config64.py: Invalid input, program terminating")
            sys.exit(2)
    else:
        for i in options_list:
            interactive_prompt_exect(i)

    if testing:
        for i in range(0, miracl_crypto.total_entries):
            interactive_prompt_exect(i+1)

    # create library
    for f in miracl_crypto.min_core_library:
        miracl_compile.compile_file(3, f)

    if sys.platform.startswith("win") :
        os.system("for %i in (*.o) do @echo %~nxi >> f.list")
        os.system("ar rc core.a @f.list")
        delete_file("f.list")
    else :
        os.system("ar rc core.a *.o")

    if testing :
        miracl_compile.compile_binary(2, "testecc.c", "core.a", "testecc")
        miracl_compile.compile_binary(2, "testeddsa.c", "core.a", "testeddsa")
        miracl_compile.compile_binary(2, "testmpin.c", "core.a", "testmpin")
        miracl_compile.compile_binary(2, "testbls.c", "core.a", "testbls")
        miracl_compile.compile_binary(2, "benchtest_all.c", "core.a", "benchtest_all")
        miracl_compile.compile_binary(2, "testnhs.c", "core.a", "testnhs")
        miracl_compile.compile_binary(2, "testdlthm.c", "core.a", "testdlthm")
        miracl_compile.compile_binary(2, "testkyber.c", "core.a", "testkyber")

    #clean up
    for file in generated_files:
        delete_file(file)
    delete_file("*.o")

    sys.exit(0)

if __name__ == "__main__":
    main(sys.argv[1:])
