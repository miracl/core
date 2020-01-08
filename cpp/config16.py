import os
import sys

deltext=""
if sys.platform.startswith("linux")  :
	deltext="rm"
	copytext="cp"
if sys.platform.startswith("darwin")  :
	deltext="rm"
	copytext="cp"
if sys.platform.startswith("win") :
	deltext="del"
	copytext=">NUL copy"

testing=False
if len(sys.argv)==2 :
	if sys.argv[1]=="test":
		testing=True
if testing :
	sys.stdin=open("test16.txt","r")


def replace(namefile,oldtext,newtext):
	f = open(namefile,'r')
	filedata = f.read()
	f.close()

	newdata = filedata.replace(oldtext,newtext)

	f = open(namefile,'w')
	f.write(newdata)
	f.close()


def rsaset(tb,tff,base,ml) :

	itb=int(tb)
	inb=int(itb/8)
	nb=str(inb)

	bd="B"+tb+"_"+base
	fnameh="config_big_"+bd+".h"
	os.system(copytext+" config_big.h "+fnameh)
	replace(fnameh,"XXX",bd)
	replace(fnameh,"@NB@",nb)
	replace(fnameh,"@BASE@",base)

	fnameh="config_ff_"+tff+".h"
	os.system(copytext+" config_ff.h "+fnameh)
	replace(fnameh,"XXX",bd)
	replace(fnameh,"WWW",tff)
	replace(fnameh,"@ML@",ml)

	fnamec="big_"+bd+".cpp"
	fnameh="big_"+bd+".h"

	os.system(copytext+" big.cpp "+fnamec)
	os.system(copytext+" big.h "+fnameh)

	replace(fnamec,"XXX",bd)
	replace(fnameh,"XXX",bd)
	os.system("g++ -O3 -c "+fnamec)

	fnamec="ff_"+tff+".cpp"
	fnameh="ff_"+tff+".h"

	os.system(copytext+" ff.cpp "+fnamec)
	os.system(copytext+" ff.h "+fnameh)

	replace(fnamec,"WWW",tff)
	replace(fnamec,"XXX",bd)
	replace(fnameh,"WWW",tff)
	replace(fnameh,"XXX",bd)
	os.system("g++ -O3 -c "+fnamec)

	fnamec="rsa_"+tff+".cpp"
	fnameh="rsa_"+tff+".h"

	os.system(copytext+" rsa.cpp "+fnamec)
	os.system(copytext+" rsa.h "+fnameh)

	replace(fnamec,"WWW",tff)
	replace(fnamec,"XXX",bd)
	replace(fnameh,"WWW",tff)
	replace(fnameh,"XXX",bd)
	os.system("g++ -O3 -c "+fnamec)

def curveset(nbt,tf,tc,base,m8,mt,qi,ct,pf,stw,sx,g2,ab,cs) :

	inbt=int(nbt)
	itb=int(inbt+(8-inbt%8)%8)
	inb=int(itb/8)
	tb=str(itb)
	nb=str(inb)

	bd="B"+tb+"_"+base
	fnameh="config_big_"+bd+".h"
	os.system(copytext+" config_big.h "+fnameh)
	replace(fnameh,"XXX",bd)
	replace(fnameh,"@NB@",nb)
	replace(fnameh,"@BASE@",base)

	fnameh="config_field_"+tf+".h"
	os.system(copytext+" config_field.h "+fnameh)
	replace(fnameh,"XXX",bd)
	replace(fnameh,"YYY",tf)
	replace(fnameh,"@NBT@",nbt)
	replace(fnameh,"@M8@",m8)
	replace(fnameh,"@MT@",mt)

	itw=int(qi)%10
	replace(fnameh,"@QI@",str(itw))
	if int(qi)//10 > 0 :
		replace(fnameh,"@TW@","POSITOWER")
	else :
		replace(fnameh,"@TW@","NEGATOWER")

	ib=int(base)
	inb=int(nb)
	inbt=int(nbt)
	sh=ib*(1+((8*inb-1)//ib))-inbt

	if sh > 6 :
		sh=6
	replace(fnameh,"@SH@",str(sh))

	fnameh="config_curve_"+tc+".h"	
	os.system(copytext+" config_curve.h "+fnameh)
	replace(fnameh,"XXX",bd)
	replace(fnameh,"YYY",tf)
	replace(fnameh,"ZZZ",tc)
	replace(fnameh,"@CT@",ct)
	replace(fnameh,"@PF@",pf)

	replace(fnameh,"@ST@",stw)
	replace(fnameh,"@SX@",sx)
	replace(fnameh,"@CS@",cs)
	replace(fnameh,"@AB@",ab)
	replace(fnameh,"@G2@",g2)

	fnamec="big_"+bd+".cpp"
	fnameh="big_"+bd+".h"

	os.system(copytext+" big.cpp "+fnamec)
	os.system(copytext+" big.h "+fnameh)

	replace(fnamec,"XXX",bd)
	replace(fnameh,"XXX",bd)
	os.system("g++ -O3 -c "+fnamec)

	fnamec="fp_"+tf+".cpp"
	fnameh="fp_"+tf+".h"

	os.system(copytext+" fp.cpp "+fnamec)
	os.system(copytext+" fp.h "+fnameh)

	replace(fnamec,"YYY",tf)
	replace(fnamec,"XXX",bd)
	replace(fnameh,"YYY",tf)
	replace(fnameh,"XXX",bd)
	os.system("g++ -O3 -c "+fnamec)

	os.system("g++ -O3 -c rom_field_"+tf+".cpp")

	fnamec="ecp_"+tc+".cpp"
	fnameh="ecp_"+tc+".h"

	os.system(copytext+" ecp.cpp "+fnamec)
	os.system(copytext+" ecp.h "+fnameh)

	replace(fnamec,"ZZZ",tc)
	replace(fnamec,"YYY",tf)
	replace(fnamec,"XXX",bd)
	replace(fnameh,"ZZZ",tc)
	replace(fnameh,"YYY",tf)
	replace(fnameh,"XXX",bd)
	os.system("g++ -O3 -c "+fnamec)

	fnamec="ecdh_"+tc+".cpp"
	fnameh="ecdh_"+tc+".h"

	os.system(copytext+" ecdh.cpp "+fnamec)
	os.system(copytext+" ecdh.h "+fnameh)

	replace(fnamec,"ZZZ",tc)
	replace(fnamec,"YYY",tf)
	replace(fnamec,"XXX",bd)
	replace(fnameh,"ZZZ",tc)
	replace(fnameh,"YYY",tf)
	replace(fnameh,"XXX",bd)
	os.system("g++ -O3 -c "+fnamec)

	os.system("g++ -O3 -c rom_curve_"+tc+".cpp")

	if pf != "NOT" :
		fnamec="fp2_"+tf+".cpp"
		fnameh="fp2_"+tf+".h"

		os.system(copytext+" fp2.cpp "+fnamec)
		os.system(copytext+" fp2.h "+fnameh)
		replace(fnamec,"YYY",tf)
		replace(fnamec,"XXX",bd)
		replace(fnameh,"YYY",tf)
		replace(fnameh,"XXX",bd)
		os.system("g++ -O3 -c "+fnamec)

		fnamec="fp4_"+tf+".cpp"
		fnameh="fp4_"+tf+".h"

		os.system(copytext+" fp4.cpp "+fnamec)
		os.system(copytext+" fp4.h "+fnameh)
		replace(fnamec,"YYY",tf)
		replace(fnamec,"XXX",bd)
		replace(fnamec,"ZZZ",tc)
		replace(fnameh,"YYY",tf)
		replace(fnameh,"XXX",bd)
		replace(fnameh,"ZZZ",tc)
		os.system("g++ -O3 -c "+fnamec)

		fnamec="fp12_"+tf+".cpp"
		fnameh="fp12_"+tf+".h"

		os.system(copytext+" fp12.cpp "+fnamec)
		os.system(copytext+" fp12.h "+fnameh)
		replace(fnamec,"YYY",tf)
		replace(fnamec,"XXX",bd)
		replace(fnamec,"ZZZ",tc)
		replace(fnameh,"YYY",tf)
		replace(fnameh,"XXX",bd)
		replace(fnameh,"ZZZ",tc)
		os.system("g++ -O3 -c "+fnamec)

		fnamec="ecp2_"+tc+".cpp"
		fnameh="ecp2_"+tc+".h"

		os.system(copytext+" ecp2.cpp "+fnamec)
		os.system(copytext+" ecp2.h "+fnameh)
		replace(fnamec,"ZZZ",tc)
		replace(fnamec,"YYY",tf)
		replace(fnamec,"XXX",bd)
		replace(fnameh,"ZZZ",tc)
		replace(fnameh,"YYY",tf)
		replace(fnameh,"XXX",bd)
		os.system("g++ -O3 -c "+fnamec)

		fnamec="pair_"+tc+".cpp"
		fnameh="pair_"+tc+".h"

		os.system(copytext+" pair.cpp "+fnamec)
		os.system(copytext+" pair.h "+fnameh)
		replace(fnamec,"ZZZ",tc)
		replace(fnamec,"YYY",tf)
		replace(fnamec,"XXX",bd)
		replace(fnameh,"ZZZ",tc)
		replace(fnameh,"YYY",tf)
		replace(fnameh,"XXX",bd)
		os.system("g++ -O3 -c "+fnamec)

		fnamec="mpin_"+tc+".cpp"
		fnameh="mpin_"+tc+".h"

		os.system(copytext+" mpin.cpp "+fnamec)
		os.system(copytext+" mpin.h "+fnameh)
		replace(fnamec,"ZZZ",tc)
		replace(fnamec,"YYY",tf)
		replace(fnamec,"XXX",bd)
		replace(fnameh,"ZZZ",tc)
		replace(fnameh,"YYY",tf)
		replace(fnameh,"XXX",bd)
		os.system("g++ -O3 -c "+fnamec)

		fnamec="bls_"+tc+".cpp"
		fnameh="bls_"+tc+".h"

		os.system(copytext+" bls.cpp "+fnamec)
		os.system(copytext+" bls.h "+fnameh)
		replace(fnamec,"ZZZ",tc)
		replace(fnamec,"YYY",tf)
		replace(fnamec,"XXX",bd)
		replace(fnameh,"ZZZ",tc)
		replace(fnameh,"YYY",tf)
		replace(fnameh,"XXX",bd)
		os.system("g++ -O3 -c "+fnamec)



replace("arch.h","@WL@","16")
print("Elliptic Curves")
print("1. ED25519")
print("2. NUMS256E")
print("3. SECP160R1")

print("Pairing-Friendly Elliptic Curves")
print("4. BN254")
print("5. BN254CX")

print("RSA")
print("6. RSA2048")

selection=[]
ptr=0
max=7

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
#	print("Choice= ",x)
	already=False
	for i in range(0,ptr):
		if x==selection[i]:
			already=True
			break
	if already:
		continue
	
	selection.append(x)
	ptr=ptr+1

# curveset(modulus_bits,field,curve,bits_in_base,modulus_mod_8,modulus_type,curve_type,pairing_friendly,sextic twist,sign of x,g2_table size,ate bits,curve security)
# for each curve give names for field and curve. In many cases the latter two will be the same. 
# modulus_bits is the bit length of the modulus, typically the same or slightly smaller than "big"
# Typically "field" describes the modulus, and "curve" is the common name for the elliptic curve   
# Next give the number base used for 32 bit architecture, as n where the base is 2^n (note that these must be fixed for the same "big" name, if is ever re-used for another curve)
# m8 max m such that 2^m | modulus-1
# modulus_type is NOT_SPECIAL, or PSEUDO_MERSENNE, or MONTGOMERY_Friendly, or GENERALISED_MERSENNE (supported for GOLDILOCKS only)
# i for Fp2 QNR 2^i+sqrt(-1) (relevant for PFCs only, else =0)
# curve_type is WEIERSTRASS, EDWARDS or MONTGOMERY
# pairing_friendly is BN, BLS or NOT (if not pairing friendly)
# if pairing friendly. M or D type twist, and sign of the family parameter x
# ate bits is number of bits in Ate parameter (from romgen program)
# g2_table size is number of entries in precomputed table 
# curve security is AES equivalent, rounded up.



	if x==1:
		curveset("255","F25519","ED25519","13","2","PSEUDO_MERSENNE","0","EDWARDS","NOT","","","","","128")
		curve_selected=True
	if x==2:
		curveset("256","F256PME","NUMS256E","13","1","PSEUDO_MERSENNE","0","EDWARDS","NOT","","","","","128")
		curve_selected=True

	if x==3:
		curveset("160","SECP160R1","SECP160R1","13","1","NOT_SPECIAL","0","WEIERSTRASS","NOT","","","","","128")
		curve_selected=True

	if x==4:
		curveset("254","BN254","BN254","13","1","NOT_SPECIAL","0","WEIERSTRASS","BN","D_TYPE","NEGATIVEX","71","66","128")
		pfcurve_selected=True
	if x==5:
		curveset("254","BN254CX","BN254CX","13","1","NOT_SPECIAL","0","WEIERSTRASS","BN","D_TYPE","NEGATIVEX","76","66","128")
		pfcurve_selected=True

# rsaset(big,ring,bit_bits_in_base,multiplier)
# for each choice give distinct names for "big" and "ring".
# Typically "big" is the length in bits of the underlying big number type
# "ring" is the RSA modulus size = "big" times 2^m
# Next give the number base used for 32 bit architectures, as n where the base is 2^n
# multiplier is 2^m (see above)


# There are choices here, different ways of getting the same result, but some faster than others
	if x==6:
		#256 is slower but may allow reuse of 256-bit BIGs used for elliptic curve
		#512 is faster.. but best is 1024
		rsaset("256","RSA2048","13","8")
		rsa_selected=True


os.system(deltext+" big.*")
os.system(deltext+" fp.*")
os.system(deltext+" ecp.*")
os.system(deltext+" ecdh.*")
os.system(deltext+" ff.*")
os.system(deltext+" rsa.*")
os.system(deltext+" config_big.h")
os.system(deltext+" config_field.h")
os.system(deltext+" config_curve.h")
os.system(deltext+" config_ff.h")
os.system(deltext+" fp2.*")
os.system(deltext+" fp4.*")
os.system(deltext+" fp12.*")
os.system(deltext+" ecp2.*")
os.system(deltext+" pair.*")
os.system(deltext+" mpin.*")
os.system(deltext+" bls.*")

# create library
os.system("g++ -O3 -c randapi.cpp")
#if curve_selected :
#	os.system("g++ -O3 -c ecdh_support.cpp")
#if rsa_selected :
#	os.system("g++ -O3 -c rsa_support.cpp")
#if pfcurve_selected :
#	os.system("g++ -O3 -c pbc_support.cpp")

os.system("g++ -O3 -c hash.cpp")
os.system("g++ -O3 -c hmac.cpp")
os.system("g++ -O3 -c rand.cpp")
os.system("g++ -O3 -c oct.cpp")
os.system("g++ -O3 -c aes.cpp")
os.system("g++ -O3 -c gcm.cpp")
os.system("g++ -O3 -c newhope.cpp")

if sys.platform.startswith("win") :
	os.system("for %i in (*.o) do @echo %~nxi >> f.list")
	os.system("ar rc core.a @f.list")
	os.system(deltext+" f.list")

else :
	os.system("ar rc core.a *.o")

os.system(deltext+" *.o")


if testing :
	if sys.platform.startswith("win") :
		os.system("g++ -O2  testecc.cpp core.a -o testecc.exe")
		os.system("g++ -O2  testmpin.cpp core.a -o testmpin.exe")
		os.system("g++ -O2  testbls.cpp core.a -o testbls.exe")
		os.system("g++ -O2  benchtest_all.cpp core.a -o benchtest_all.exe")
		os.system("testecc")
		os.system("testmpin < pins.txt")
		os.system("testbls")
		os.system("benchtest_all")
	else :
		os.system("g++ -O2  testecc.cpp core.a -o testecc")
		os.system("g++ -O2  testmpin.cpp core.a -o testmpin")
		os.system("g++ -O2  testbls.cpp core.a -o testbls")
		os.system("g++ -O2  benchtest_all.cpp core.a -o benchtest_all")
		os.system("./testecc")
		os.system("./testmpin < pins.txt")
		os.system("./testbls")
		os.system("./benchtest_all")

#print("Your section was ")
#for i in range(0,ptr):
#	print (selection[i])

