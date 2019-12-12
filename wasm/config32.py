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
	copytext="copy"

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

	bd=tb+"_"+base
	fnameh="config_big_"+bd+".h"
	os.system(copytext+" config_big.h "+fnameh)
	replace(fnameh,"XXX",bd)
	replace(fnameh,"@NB@",nb)
	replace(fnameh,"@BASE@",base)

	fnameh="config_ff_"+tff+".h"
	os.system(copytext+" config_ff.h "+fnameh)
	replace(fnameh,"XXX",bd)
	replace(fnameh,"WWW",tff)
	replace(fnameh,"@ML@",ml);

	fnamec="big_"+bd+".c"
	fnamebc="big_"+bd+".bc"
	fnameh="big_"+bd+".h"

	os.system(copytext+" big.c "+fnamec)
	os.system(copytext+" big.h "+fnameh)

	replace(fnamec,"XXX",bd)
	replace(fnameh,"XXX",bd)
	os.system("emcc -O2 -c "+fnamec+" -o "+fnamebc)

	fnamec="ff_"+tff+".c"
	fnamebc="ff_"+tff+".bc"
	fnameh="ff_"+tff+".h"

	os.system(copytext+" ff.c "+fnamec)
	os.system(copytext+" ff.h "+fnameh)

	replace(fnamec,"WWW",tff)
	replace(fnamec,"XXX",bd)
	replace(fnameh,"WWW",tff)
	replace(fnameh,"XXX",bd)
	os.system("emcc -O2 -c "+fnamec+" -o "+fnamebc)

	fnamec="rsa_"+tff+".c"
	fnamebc="rsa_"+tff+".bc"
	fnameh="rsa_"+tff+".h"

	os.system(copytext+" rsa.c "+fnamec)
	os.system(copytext+" rsa.h "+fnameh)

	replace(fnamec,"WWW",tff)
	replace(fnamec,"XXX",bd)
	replace(fnameh,"WWW",tff)
	replace(fnameh,"XXX",bd)
	os.system("emcc -O2 -c "+fnamec+" -o "+fnamebc)

def curveset(nbt,tf,tc,base,m8,mt,qi,ct,pf,stw,sx,g2,ab,cs) :
	inbt=int(nbt)
	itb=int(inbt+(8-inbt%8)%8)
	inb=int(itb/8)
	tb=str(itb)
	nb=str(inb)

	bd=tb+"_"+base

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

	ib=int(base)
	inb=int(nb)
	inbt=int(nbt)
	sh=ib*(1+((8*inb-1)//ib))-inbt
	if sh > 14 :
		sh=14
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

	fnamec="big_"+bd+".c"
	fnamebc="big_"+bd+".bc"
	fnameh="big_"+bd+".h"

	os.system(copytext+" big.c "+fnamec)
	os.system(copytext+" big.h "+fnameh)

	replace(fnamec,"XXX",bd)
	replace(fnameh,"XXX",bd)
	os.system("emcc -O2 -c "+fnamec+" -o "+fnamebc)

	fnamec="fp_"+tf+".c"
	fnamebc="fp_"+tf+".bc"
	fnameh="fp_"+tf+".h"

	os.system(copytext+" fp.c "+fnamec)
	os.system(copytext+" fp.h "+fnameh)

	replace(fnamec,"YYY",tf)
	replace(fnamec,"XXX",bd)
	replace(fnameh,"YYY",tf)
	replace(fnameh,"XXX",bd)
	os.system("emcc -O2 -c "+fnamec+" -o "+fnamebc)

	os.system("emcc -O2 -c rom_field_"+tf+".c -o rom_field_"+tf+".bc");

	fnamec="ecp_"+tc+".c"
	fnamebc="ecp_"+tc+".bc"
	fnameh="ecp_"+tc+".h"

	os.system(copytext+" ecp.c "+fnamec)
	os.system(copytext+" ecp.h "+fnameh)

	replace(fnamec,"ZZZ",tc)
	replace(fnamec,"YYY",tf)
	replace(fnamec,"XXX",bd)
	replace(fnameh,"ZZZ",tc)
	replace(fnameh,"YYY",tf)
	replace(fnameh,"XXX",bd)
	os.system("emcc -O2 -c "+fnamec+" -o "+fnamebc)

	fnamec="ecdh_"+tc+".c"
	fnamebc="ecdh_"+tc+".bc"
	fnameh="ecdh_"+tc+".h"

	os.system(copytext+" ecdh.c "+fnamec)
	os.system(copytext+" ecdh.h "+fnameh)

	replace(fnamec,"ZZZ",tc)
	replace(fnamec,"YYY",tf)
	replace(fnamec,"XXX",bd)
	replace(fnameh,"ZZZ",tc)
	replace(fnameh,"YYY",tf)
	replace(fnameh,"XXX",bd)
	os.system("emcc -O2 -c "+fnamec+" -o "+fnamebc)

	fnamec="hpke_"+tc+".c"
	fnamebc="hpke_"+tc+".bc"
	fnameh="hpke_"+tc+".h"

	os.system(copytext+" hpke.c "+fnamec)
	os.system(copytext+" hpke.h "+fnameh)

	replace(fnamec,"ZZZ",tc)
	replace(fnameh,"ZZZ",tc)
	os.system("emcc -O2 -c "+fnamec+" -o "+fnamebc)

	os.system("emcc -O2 -c rom_curve_"+tc+".c -o rom_curve_"+tc+".bc");

	if pf != "NOT" :
		fnamec="fp2_"+tf+".c"
		fnamebc="fp2_"+tf+".bc"
		fnameh="fp2_"+tf+".h"

		os.system(copytext+" fp2.c "+fnamec)
		os.system(copytext+" fp2.h "+fnameh)
		replace(fnamec,"YYY",tf)
		replace(fnamec,"XXX",bd)
		replace(fnameh,"YYY",tf)
		replace(fnameh,"XXX",bd)
		os.system("emcc -O2 -c "+fnamec+" -o "+fnamebc)

		fnamec="fp4_"+tf+".c"
		fnamebc="fp4_"+tf+".bc"
		fnameh="fp4_"+tf+".h"

		os.system(copytext+" fp4.c "+fnamec)
		os.system(copytext+" fp4.h "+fnameh)
		replace(fnamec,"YYY",tf)
		replace(fnamec,"XXX",bd)
		replace(fnamec,"ZZZ",tc)
		replace(fnameh,"YYY",tf)
		replace(fnameh,"XXX",bd)
		replace(fnameh,"ZZZ",tc)
		os.system("emcc -O2 -c "+fnamec+" -o "+fnamebc)


		if cs == "128" :
			fnamec="fp12_"+tf+".c"
			fnamebc="fp12_"+tf+".bc"
			fnameh="fp12_"+tf+".h"

			os.system(copytext+" fp12.c "+fnamec)
			os.system(copytext+" fp12.h "+fnameh)
			replace(fnamec,"YYY",tf)
			replace(fnamec,"XXX",bd)
			replace(fnamec,"ZZZ",tc)
			replace(fnameh,"YYY",tf)
			replace(fnameh,"XXX",bd)
			replace(fnameh,"ZZZ",tc)
			os.system("emcc -O2 -c "+fnamec+" -o "+fnamebc)

			fnamec="ecp2_"+tc+".c"
			fnamebc="ecp2_"+tc+".bc"
			fnameh="ecp2_"+tc+".h"

			os.system(copytext+" ecp2.c "+fnamec)
			os.system(copytext+" ecp2.h "+fnameh)
			replace(fnamec,"ZZZ",tc)
			replace(fnamec,"YYY",tf)
			replace(fnamec,"XXX",bd)
			replace(fnameh,"ZZZ",tc)
			replace(fnameh,"YYY",tf)
			replace(fnameh,"XXX",bd)
			os.system("emcc -O2 -c "+fnamec+" -o "+fnamebc)

			fnamec="pair_"+tc+".c"
			fnamebc="pair_"+tc+".bc"
			fnameh="pair_"+tc+".h"

			os.system(copytext+" pair.c "+fnamec)
			os.system(copytext+" pair.h "+fnameh)
			replace(fnamec,"ZZZ",tc)
			replace(fnamec,"YYY",tf)
			replace(fnamec,"XXX",bd)
			replace(fnameh,"ZZZ",tc)
			replace(fnameh,"YYY",tf)
			replace(fnameh,"XXX",bd)
			os.system("emcc -O2 -c "+fnamec+" -o "+fnamebc)

			fnamec="mpin_"+tc+".c"
			fnamebc="mpin_"+tc+".bc"
			fnameh="mpin_"+tc+".h"

			os.system(copytext+" mpin.c "+fnamec)
			os.system(copytext+" mpin.h "+fnameh)
			replace(fnamec,"ZZZ",tc)
			replace(fnamec,"YYY",tf)
			replace(fnamec,"XXX",bd)
			replace(fnameh,"ZZZ",tc)
			replace(fnameh,"YYY",tf)
			replace(fnameh,"XXX",bd)
			os.system("emcc -O2 -c "+fnamec+" -o "+fnamebc)

			fnamec="bls_"+tc+".c"
			fnamebc="bls_"+tc+".bc"
			fnameh="bls_"+tc+".h"

			os.system(copytext+" bls.c "+fnamec)
			os.system(copytext+" bls.h "+fnameh)
			replace(fnamec,"ZZZ",tc)
			replace(fnamec,"YYY",tf)
			replace(fnamec,"XXX",bd)
			replace(fnameh,"ZZZ",tc)
			replace(fnameh,"YYY",tf)
			replace(fnameh,"XXX",bd)
			os.system("emcc -O2 -c "+fnamec+" -o "+fnamebc)


		if cs == "192" :
			fnamec="fp8_"+tf+".c"
			fnamebc="fp8_"+tf+".bc"
			fnameh="fp8_"+tf+".h"

			os.system(copytext+" fp8.c "+fnamec)
			os.system(copytext+" fp8.h "+fnameh)
			replace(fnamec,"YYY",tf)
			replace(fnamec,"XXX",bd)
			replace(fnamec,"ZZZ",tc)
			replace(fnameh,"YYY",tf)
			replace(fnameh,"XXX",bd)
			replace(fnameh,"ZZZ",tc)
			os.system("emcc -O2 -c "+fnamec+" -o "+fnamebc)


			fnamec="fp24_"+tf+".c"
			fnamebc="fp24_"+tf+".bc"
			fnameh="fp24_"+tf+".h"

			os.system(copytext+" fp24.c "+fnamec)
			os.system(copytext+" fp24.h "+fnameh)
			replace(fnamec,"YYY",tf)
			replace(fnamec,"XXX",bd)
			replace(fnamec,"ZZZ",tc)
			replace(fnameh,"YYY",tf)
			replace(fnameh,"XXX",bd)
			replace(fnameh,"ZZZ",tc)
			os.system("emcc -O2 -c "+fnamec+" -o "+fnamebc)

			fnamec="ecp4_"+tc+".c"
			fnamebc="ecp4_"+tc+".bc"
			fnameh="ecp4_"+tc+".h"

			os.system(copytext+" ecp4.c "+fnamec)
			os.system(copytext+" ecp4.h "+fnameh)
			replace(fnamec,"ZZZ",tc)
			replace(fnamec,"YYY",tf)
			replace(fnamec,"XXX",bd)
			replace(fnameh,"ZZZ",tc)
			replace(fnameh,"YYY",tf)
			replace(fnameh,"XXX",bd)
			os.system("emcc -O2 -c "+fnamec+" -o "+fnamebc)

			fnamec="pair192_"+tc+".c"
			fnamebc="pair192_"+tc+".bc"
			fnameh="pair192_"+tc+".h"

			os.system(copytext+" pair192.c "+fnamec)
			os.system(copytext+" pair192.h "+fnameh)
			replace(fnamec,"ZZZ",tc)
			replace(fnamec,"YYY",tf)
			replace(fnamec,"XXX",bd)
			replace(fnameh,"ZZZ",tc)
			replace(fnameh,"YYY",tf)
			replace(fnameh,"XXX",bd)
			os.system("emcc -O2 -c "+fnamec+" -o "+fnamebc)

			fnamec="mpin192_"+tc+".c"
			fnamebc="mpin192_"+tc+".bc"
			fnameh="mpin192_"+tc+".h"

			os.system(copytext+" mpin192.c "+fnamec)
			os.system(copytext+" mpin192.h "+fnameh)
			replace(fnamec,"ZZZ",tc)
			replace(fnamec,"YYY",tf)
			replace(fnamec,"XXX",bd)
			replace(fnameh,"ZZZ",tc)
			replace(fnameh,"YYY",tf)
			replace(fnameh,"XXX",bd)
			os.system("emcc -O2 -c "+fnamec+" -o "+fnamebc)		

			fnamec="bls192_"+tc+".c"
			fnamebc="bls192_"+tc+".bc"
			fnameh="bls192_"+tc+".h"

			os.system(copytext+" bls192.c "+fnamec)
			os.system(copytext+" bls192.h "+fnameh)
			replace(fnamec,"ZZZ",tc)
			replace(fnamec,"YYY",tf)
			replace(fnamec,"XXX",bd)
			replace(fnameh,"ZZZ",tc)
			replace(fnameh,"YYY",tf)
			replace(fnameh,"XXX",bd)
			os.system("emcc -O2 -c "+fnamec+" -o "+fnamebc)	


		if cs == "256" :

			fnamec="fp8_"+tf+".c"
			fnamebc="fp8_"+tf+".bc"
			fnameh="fp8_"+tf+".h"

			os.system(copytext+" fp8.c "+fnamec)
			os.system(copytext+" fp8.h "+fnameh)
			replace(fnamec,"YYY",tf)
			replace(fnamec,"XXX",bd)
			replace(fnamec,"ZZZ",tc)
			replace(fnameh,"YYY",tf)
			replace(fnameh,"XXX",bd)
			replace(fnameh,"ZZZ",tc)
			os.system("emcc -O2 -c "+fnamec+" -o "+fnamebc)


			fnamec="ecp8_"+tc+".c"
			fnamebc="ecp8_"+tc+".bc"
			fnameh="ecp8_"+tc+".h"

			os.system(copytext+" ecp8.c "+fnamec)
			os.system(copytext+" ecp8.h "+fnameh)
			replace(fnamec,"ZZZ",tc)
			replace(fnamec,"YYY",tf)
			replace(fnamec,"XXX",bd)
			replace(fnameh,"ZZZ",tc)
			replace(fnameh,"YYY",tf)
			replace(fnameh,"XXX",bd)
			os.system("emcc -O2 -c "+fnamec+" -o "+fnamebc)


			fnamec="fp16_"+tf+".c"
			fnamebc="fp16_"+tf+".bc"
			fnameh="fp16_"+tf+".h"

			os.system(copytext+" fp16.c "+fnamec)
			os.system(copytext+" fp16.h "+fnameh)
			replace(fnamec,"YYY",tf)
			replace(fnamec,"XXX",bd)
			replace(fnamec,"ZZZ",tc)
			replace(fnameh,"YYY",tf)
			replace(fnameh,"XXX",bd)
			replace(fnameh,"ZZZ",tc)
			os.system("emcc -O2 -c "+fnamec+" -o "+fnamebc)


			fnamec="fp48_"+tf+".c"
			fnamebc="fp48_"+tf+".bc"
			fnameh="fp48_"+tf+".h"

			os.system(copytext+" fp48.c "+fnamec)
			os.system(copytext+" fp48.h "+fnameh)
			replace(fnamec,"YYY",tf)
			replace(fnamec,"XXX",bd)
			replace(fnamec,"ZZZ",tc)
			replace(fnameh,"YYY",tf)
			replace(fnameh,"XXX",bd)
			replace(fnameh,"ZZZ",tc)
			os.system("emcc -O2 -c "+fnamec+" -o "+fnamebc)


			fnamec="pair256_"+tc+".c"
			fnamebc="pair256_"+tc+".bc"
			fnameh="pair256_"+tc+".h"

			os.system(copytext+" pair256.c "+fnamec)
			os.system(copytext+" pair256.h "+fnameh)
			replace(fnamec,"ZZZ",tc)
			replace(fnamec,"YYY",tf)
			replace(fnamec,"XXX",bd)
			replace(fnameh,"ZZZ",tc)
			replace(fnameh,"YYY",tf)
			replace(fnameh,"XXX",bd)
			os.system("emcc -O2 -c "+fnamec+" -o "+fnamebc)

			fnamec="mpin256_"+tc+".c"
			fnamebc="mpin256_"+tc+".bc"
			fnameh="mpin256_"+tc+".h"

			os.system(copytext+" mpin256.c "+fnamec)
			os.system(copytext+" mpin256.h "+fnameh)
			replace(fnamec,"ZZZ",tc)
			replace(fnamec,"YYY",tf)
			replace(fnamec,"XXX",bd)
			replace(fnameh,"ZZZ",tc)
			replace(fnameh,"YYY",tf)
			replace(fnameh,"XXX",bd)
			os.system("emcc -O2 -c "+fnamec+" -o "+fnamebc)				

			fnamec="bls256_"+tc+".c"
			fnamebc="bls256_"+tc+".bc"
			fnameh="bls256_"+tc+".h"

			os.system(copytext+" bls256.c "+fnamec)
			os.system(copytext+" bls256.h "+fnameh)
			replace(fnamec,"ZZZ",tc)
			replace(fnamec,"YYY",tf)
			replace(fnamec,"XXX",bd)
			replace(fnameh,"ZZZ",tc)
			replace(fnameh,"YYY",tf)
			replace(fnameh,"XXX",bd)
			os.system("emcc -O2 -c "+fnamec+" -o "+fnamebc)		

replace("arch.h","@WL@","32")
print("Elliptic Curves")
print("1. ED25519")
print("2. C25519")
print("3. NIST256")
print("4. BRAINPOOL")
print("5. ANSSI")
print("6. HIFIVE")
print("7. GOLDILOCKS")
print("8. NIST384")
print("9. C41417")
print("10. NIST521\n")
print("11. NUMS256W")
print("12. NUMS256E")
print("13. NUMS384W")
print("14. NUMS384E")
print("15. NUMS512W")
print("16. NUMS512E")
print("17. SECP256K1")
print("18. SM2")
print("19. C13318")
print("20. JUBJUB")
print("21. X448")
print("22. SECP160R1\n")

print("Pairing-Friendly Elliptic Curves")
print("23. BN254")
print("24. BN254CX")
print("25. BLS12383")
print("26. BLS12381")
print("27. FP256BN")
print("28. FP512BN")
print("29. BLS12461")
print("30. BN462\n")
print("31. BLS24479")
print("32. BLS48556\n")
print("33. BLS48581\n")

print("RSA")
print("34. RSA2048")
print("35. RSA3072")
print("36. RSA4096")

selection=[]
ptr=0
max=37


curve_selected=False
pfcurve_selected=False
rsa_selected=False

while ptr<max:
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
# curve security is AES equiavlent, rounded up.

	if x==1:
		curveset("255","F25519","ED25519","29","2","PSEUDO_MERSENNE","0","EDWARDS","NOT","","","","","128")
		curve_selected=True
	if x==2:
		curveset("255","F25519","C25519","29","2","PSEUDO_MERSENNE","0","MONTGOMERY","NOT","","","","","128")
		curve_selected=True
	if x==3:
		curveset("256","NIST256","NIST256","28","1","NOT_SPECIAL","0","WEIERSTRASS","NOT","","","","","128")
		curve_selected=True
	if x==4:
		curveset("256","BRAINPOOL","BRAINPOOL","28","1","NOT_SPECIAL","0","WEIERSTRASS","NOT","","","","","128")
		curve_selected=True
	if x==5:
		curveset("256","ANSSI","ANSSI","28","1","NOT_SPECIAL","0","WEIERSTRASS","NOT","","","","","128")
		curve_selected=True

	if x==6:
		curveset("336","HIFIVE","HIFIVE","29","2","PSEUDO_MERSENNE","0","EDWARDS","NOT","","","","","192")
		curve_selected=True
	if x==7:
		curveset("448","GOLDILOCKS","GOLDILOCKS","29","1","GENERALISED_MERSENNE","0","EDWARDS","NOT","","","","","256")
		curve_selected=True
	if x==8:
		curveset("384","NIST384","NIST384","29","1","NOT_SPECIAL","0","WEIERSTRASS","NOT","","","","","192")
		curve_selected=True
	if x==9:
		curveset("414","C41417","C41417","29","1","PSEUDO_MERSENNE","0","EDWARDS","NOT","","","","","256")
		curve_selected=True
	if x==10:
		curveset("521","NIST521","NIST521","28","1","PSEUDO_MERSENNE","0","WEIERSTRASS","NOT","","","","","256")
		curve_selected=True

	if x==11:
		curveset("256","256PMW","NUMS256W","28","1","PSEUDO_MERSENNE","0","WEIERSTRASS","NOT","","","","","128")
		curve_selected=True
	if x==12:
		curveset("256","256PME","NUMS256E","29","1","PSEUDO_MERSENNE","0","EDWARDS","NOT","","","","","128")
		curve_selected=True
	if x==13:
		curveset("384","384PM","NUMS384W","29","1","PSEUDO_MERSENNE","0","WEIERSTRASS","NOT","","","","","192")
		curve_selected=True
	if x==14:
		curveset("384","384PM","NUMS384E","29","1","PSEUDO_MERSENNE","0","EDWARDS","NOT","","","","","192")
		curve_selected=True
	if x==15:
		curveset("512","512PM","NUMS512W","29","1","PSEUDO_MERSENNE","0","WEIERSTRASS","NOT","","","","","256")
		curve_selected=True
	if x==16:
		curveset("512","512PM","NUMS512E","29","1","PSEUDO_MERSENNE","0","EDWARDS","NOT","","","","","256")
		curve_selected=True

	if x==17:
		curveset("256","SECP256K1","SECP256K1","28","1","NOT_SPECIAL","0","WEIERSTRASS","NOT","","","","","128")
		curve_selected=True

	if x==18:
		curveset("256","SM2","SM2","28","1","NOT_SPECIAL","0","WEIERSTRASS","NOT","","","","","128")
		curve_selected=True

	if x==19:
		curveset("255","F25519","C13318","29","2","PSEUDO_MERSENNE","0","WEIERSTRASS","NOT","","","","","128")
		curve_selected=True

	if x==20:
		curveset("255","JUBJUB","JUBJUB","29","32","NOT_SPECIAL","5","EDWARDS","NOT","","","","","128")
		curve_selected=True

	if x==21:
		curveset("448","GOLDILOCKS","X448","29","1","GENERALISED_MERSENNE","0","MONTGOMERY","NOT","","","","","256")
		curve_selected=True

	if x==22:
		curveset("160","SECP160R1","SECP160R1","29","1","NOT_SPECIAL","0","WEIERSTRASS","NOT","","","","","128")
		curve_selected=True


	if x==23:
		curveset("254","BN254","BN254","28","1","NOT_SPECIAL","0","WEIERSTRASS","BN","D_TYPE","NEGATIVEX","71","66","128")
		pfcurve_selected=True
	if x==24:
		curveset("254","BN254CX","BN254CX","28","1","NOT_SPECIAL","0","WEIERSTRASS","BN","D_TYPE","NEGATIVEX","76","66","128")
		pfcurve_selected=True
	if x==25:
		curveset("383","BLS12383","BLS12383","29","1","NOT_SPECIAL","0","WEIERSTRASS","BLS","M_TYPE","POSITIVEX","68","65","128")
		pfcurve_selected=True

	if x==26:
		curveset("381","BLS12381","BLS12381","29","1","NOT_SPECIAL","0","WEIERSTRASS","BLS","M_TYPE","NEGATIVEX","69","65","128")
		pfcurve_selected=True

	if x==27:
		curveset("256","FP256BN","FP256BN","28","1","NOT_SPECIAL","0","WEIERSTRASS","BN","M_TYPE","NEGATIVEX","83","66","128")
		pfcurve_selected=True
	if x==28:
		curveset("512","FP512BN","FP512BN","29","1","NOT_SPECIAL","0","WEIERSTRASS","BN","M_TYPE","POSITIVEX","172","130","128")
		pfcurve_selected=True
# https://eprint.iacr.org/2017/334.pdf
	if x==29:
		curveset("461","BLS12461","BLS12461","28","1","NOT_SPECIAL","0","WEIERSTRASS","BLS","M_TYPE","NEGATIVEX","79","78","128")
		pfcurve_selected=True

	if x==30:
		curveset("462","BN462","BN462","28","1","NOT_SPECIAL","1","WEIERSTRASS","BN","D_TYPE","POSITIVEX","125","118","128")
		pfcurve_selected=True

	if x==31:
		curveset("479","BLS24479","BLS24479","29","1","NOT_SPECIAL","0","WEIERSTRASS","BLS","M_TYPE","POSITIVEX","52","49","192")
		pfcurve_selected=True


	if x==32:
		curveset("556","BLS48556","BLS48556","29","1","NOT_SPECIAL","0","WEIERSTRASS","BLS","M_TYPE","POSITIVEX","35","32","256")
		pfcurve_selected=True

	if x==33:
		curveset("581","BLS48581","BLS48581","29","1","NOT_SPECIAL","10","WEIERSTRASS","BLS","D_TYPE","NEGATIVEX","36","33","256")
		pfcurve_selected=True

# rsaset(big,ring,bit_bits_in_base,multiplier)
# for each choice give distinct names for "big" and "ring".
# Typically "big" is the length in bits of the underlying big number type
# "ring" is the RSA modulus size = "big" times 2^m
# Next give the number base used for 32 bit architectures, as n where the base is 2^n
# multiplier is 2^m (see above)

# There are choices here, different ways of getting the same result, but some faster than others
	if x==34:
		#256 is slower but may allow reuse of 256-bit BIGs used for elliptic curve
		#512 is faster.. but best is 1024
		rsaset("1024","2048","28","2")
		#rsaset("512","2048","29","4")
		#rsaset("256","2048","29","8")
		rsa_selected=True
	if x==35:
		rsaset("384","3072","28","8")
		rsa_selected=True
	if x==36:
		#rsaset("256","4096","29","16")
		rsaset("512","4096","29","8")
		rsa_selected=True


os.system(deltext+" big.*")
os.system(deltext+" fp.*")
os.system(deltext+" ecp.*")
os.system(deltext+" ecdh.*")
os.system(deltext+" hpke.*")
os.system(deltext+" ff.*")
os.system(deltext+" rsa.*")
os.system(deltext+" config_big.h")
os.system(deltext+" config_field.h")
os.system(deltext+" config_curve.h")
os.system(deltext+" config_ff.h")
os.system(deltext+" fp2.*")
os.system(deltext+" fp4.*")
os.system(deltext+" fp8.*")
os.system(deltext+" fp16.*")


os.system(deltext+" fp12.*")
os.system(deltext+" fp24.*")
os.system(deltext+" fp48.*")

os.system(deltext+" ecp2.*")
os.system(deltext+" ecp4.*")
os.system(deltext+" ecp8.*")

os.system(deltext+" pair.*")
os.system(deltext+" mpin.*")
os.system(deltext+" bls.*")

os.system(deltext+" pair192.*")
os.system(deltext+" mpin192.*")
os.system(deltext+" bls192.*")

os.system(deltext+" pair256.*")
os.system(deltext+" mpin256.*")
os.system(deltext+" bls256.*")


# create library
os.system("emcc -O2 -c  randapi.c -o randapi.bc")


os.system("emcc -O2 -c hash.c -o hash.bc")
os.system("emcc -O2 -c hmac.c -o hmac.bc")
os.system("emcc -O2 -c rand.c -o rand.bc")
os.system("emcc -O2 -c oct.c -o oct.bc")
os.system("emcc -O2 -c aes.c -o aes.bc")
os.system("emcc -O2 -c gcm.c -o gcm.bc")
os.system("emcc -O2 -c newhope.c -o newhope.bc")

if sys.platform.startswith("win") :
	os.system("for %i in (*.bc) do @echo %~nxi >> f.list")
	os.system("emar rc core.a @f.list")
	os.system(deltext+" f.list")

else :
	os.system("emar rc core.a *.bc")
	
os.system(deltext+" *.bc")

#print("Your section was ");	
#for i in range(0,ptr):
#	print (selection[i])

