import os
import sys

deltext=""
slashtext=""
if sys.platform.startswith("linux")  :
	deltext="rm"
	copytext="cp"
	slashtext="/"
if sys.platform.startswith("darwin")  :
	deltext="rm"
	copytext="cp"
	slashtext="/"
if sys.platform.startswith("win") :
	deltext="del"
	copytext="copy"
	slashtext="\\"

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

	fnamec="ff_"+tff+".cpp"
	fnameh="ff_"+tff+".h"

	os.system(copytext+" ff.cpp "+fnamec)
	os.system(copytext+" ff.h "+fnameh)

	replace(fnamec,"WWW",tff)
	replace(fnamec,"XXX",bd)
	replace(fnameh,"WWW",tff)
	replace(fnameh,"XXX",bd)

	fnamec="rsa_"+tff+".cpp"
	fnameh="rsa_"+tff+".h"

	os.system(copytext+" rsa.cpp "+fnamec)
	os.system(copytext+" rsa.h "+fnameh)

	replace(fnamec,"WWW",tff)
	replace(fnamec,"XXX",bd)
	replace(fnameh,"WWW",tff)
	replace(fnameh,"XXX",bd)

	replace("testrsa.ino","XXX",tff)

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


	fnamec="big_"+bd+".cpp"
	fnameh="big_"+bd+".h"

	os.system(copytext+" big.cpp "+fnamec)
	os.system(copytext+" big.h "+fnameh)

	replace(fnamec,"XXX",bd)
	replace(fnameh,"XXX",bd)

	fnamec="fp_"+tf+".cpp"
	fnameh="fp_"+tf+".h"

	os.system(copytext+" fp.cpp "+fnamec)
	os.system(copytext+" fp.h "+fnameh)

	replace(fnamec,"YYY",tf)
	replace(fnamec,"XXX",bd)
	replace(fnameh,"YYY",tf)
	replace(fnameh,"XXX",bd)

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

	if pf != "NOT" :
		fnamec="fp2_"+tf+".cpp"
		fnameh="fp2_"+tf+".h"

		os.system(copytext+" fp2.cpp "+fnamec)
		os.system(copytext+" fp2.h "+fnameh)
		replace(fnamec,"YYY",tf)
		replace(fnamec,"XXX",bd)
		replace(fnameh,"YYY",tf)
		replace(fnameh,"XXX",bd)

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

		if cs == "128" :
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

		if cs == "192" :
			fnamec="fp8_"+tf+".cpp"
			fnameh="fp8_"+tf+".h"

			os.system(copytext+" fp8.cpp "+fnamec)
			os.system(copytext+" fp8.h "+fnameh)
			replace(fnamec,"YYY",tf)
			replace(fnamec,"XXX",bd)
			replace(fnamec,"ZZZ",tc)
			replace(fnameh,"YYY",tf)
			replace(fnameh,"XXX",bd)
			replace(fnameh,"ZZZ",tc)

			fnamec="fp24_"+tf+".cpp"
			fnameh="fp24_"+tf+".h"

			os.system(copytext+" fp24.cpp "+fnamec)
			os.system(copytext+" fp24.h "+fnameh)
			replace(fnamec,"YYY",tf)
			replace(fnamec,"XXX",bd)
			replace(fnamec,"ZZZ",tc)
			replace(fnameh,"YYY",tf)
			replace(fnameh,"XXX",bd)
			replace(fnameh,"ZZZ",tc)

			fnamec="ecp4_"+tc+".cpp"
			fnameh="ecp4_"+tc+".h"

			os.system(copytext+" ecp4.cpp "+fnamec)
			os.system(copytext+" ecp4.h "+fnameh)
			replace(fnamec,"ZZZ",tc)
			replace(fnamec,"YYY",tf)
			replace(fnamec,"XXX",bd)
			replace(fnameh,"ZZZ",tc)
			replace(fnameh,"YYY",tf)
			replace(fnameh,"XXX",bd)

			fnamec="pair192_"+tc+".cpp"
			fnameh="pair192_"+tc+".h"

			os.system(copytext+" pair192.cpp "+fnamec)
			os.system(copytext+" pair192.h "+fnameh)
			replace(fnamec,"ZZZ",tc)
			replace(fnamec,"YYY",tf)
			replace(fnamec,"XXX",bd)
			replace(fnameh,"ZZZ",tc)
			replace(fnameh,"YYY",tf)
			replace(fnameh,"XXX",bd)

			fnamec="mpin192_"+tc+".cpp"
			fnameh="mpin192_"+tc+".h"

			os.system(copytext+" mpin192.cpp "+fnamec)
			os.system(copytext+" mpin192.h "+fnameh)
			replace(fnamec,"ZZZ",tc)
			replace(fnamec,"YYY",tf)
			replace(fnamec,"XXX",bd)
			replace(fnameh,"ZZZ",tc)
			replace(fnameh,"YYY",tf)
			replace(fnameh,"XXX",bd)

			fnamec="bls192_"+tc+".cpp"
			fnameh="bls192_"+tc+".h"

			os.system(copytext+" bls192.cpp "+fnamec)
			os.system(copytext+" bls192.h "+fnameh)
			replace(fnamec,"ZZZ",tc)
			replace(fnamec,"YYY",tf)
			replace(fnamec,"XXX",bd)
			replace(fnameh,"ZZZ",tc)
			replace(fnameh,"YYY",tf)
			replace(fnameh,"XXX",bd)

		if cs == "256" :

			fnamec="fp8_"+tf+".cpp"
			fnameh="fp8_"+tf+".h"

			os.system(copytext+" fp8.cpp "+fnamec)
			os.system(copytext+" fp8.h "+fnameh)
			replace(fnamec,"YYY",tf)
			replace(fnamec,"XXX",bd)
			replace(fnamec,"ZZZ",tc)
			replace(fnameh,"YYY",tf)
			replace(fnameh,"XXX",bd)
			replace(fnameh,"ZZZ",tc)

			fnamec="ecp8_"+tc+".cpp"
			fnameh="ecp8_"+tc+".h"

			os.system(copytext+" ecp8.cpp "+fnamec)
			os.system(copytext+" ecp8.h "+fnameh)
			replace(fnamec,"ZZZ",tc)
			replace(fnamec,"YYY",tf)
			replace(fnamec,"XXX",bd)
			replace(fnameh,"ZZZ",tc)
			replace(fnameh,"YYY",tf)
			replace(fnameh,"XXX",bd)

			fnamec="fp16_"+tf+".cpp"
			fnameh="fp16_"+tf+".h"

			os.system(copytext+" fp16.cpp "+fnamec)
			os.system(copytext+" fp16.h "+fnameh)
			replace(fnamec,"YYY",tf)
			replace(fnamec,"XXX",bd)
			replace(fnamec,"ZZZ",tc)
			replace(fnameh,"YYY",tf)
			replace(fnameh,"XXX",bd)
			replace(fnameh,"ZZZ",tc)

			fnamec="fp48_"+tf+".cpp"
			fnameh="fp48_"+tf+".h"

			os.system(copytext+" fp48.cpp "+fnamec)
			os.system(copytext+" fp48.h "+fnameh)
			replace(fnamec,"YYY",tf)
			replace(fnamec,"XXX",bd)
			replace(fnamec,"ZZZ",tc)
			replace(fnameh,"YYY",tf)
			replace(fnameh,"XXX",bd)
			replace(fnameh,"ZZZ",tc)

			fnamec="pair256_"+tc+".cpp"
			fnameh="pair256_"+tc+".h"

			os.system(copytext+" pair256.cpp "+fnamec)
			os.system(copytext+" pair256.h "+fnameh)
			replace(fnamec,"ZZZ",tc)
			replace(fnamec,"YYY",tf)
			replace(fnamec,"XXX",bd)
			replace(fnameh,"ZZZ",tc)
			replace(fnameh,"YYY",tf)
			replace(fnameh,"XXX",bd)

			fnamec="mpin256_"+tc+".cpp"
			fnameh="mpin256_"+tc+".h"

			os.system(copytext+" mpin256.cpp "+fnamec)
			os.system(copytext+" mpin256.h "+fnameh)
			replace(fnamec,"ZZZ",tc)
			replace(fnamec,"YYY",tf)
			replace(fnamec,"XXX",bd)
			replace(fnameh,"ZZZ",tc)
			replace(fnameh,"YYY",tf)
			replace(fnameh,"XXX",bd)

			fnamec="bls256_"+tc+".cpp"
			fnameh="bls256_"+tc+".h"

			os.system(copytext+" bls256.cpp "+fnamec)
			os.system(copytext+" bls256.h "+fnameh)
			replace(fnamec,"ZZZ",tc)
			replace(fnamec,"YYY",tf)
			replace(fnamec,"XXX",bd)
			replace(fnameh,"ZZZ",tc)
			replace(fnameh,"YYY",tf)
			replace(fnameh,"XXX",bd)
		replace("testbls.ino","XXX",tc)
		replace("timepbc.ino","XXX",tc)
	else :
		replace("testecc.ino","XXX",tc)
		replace("timeecc.ino","XXX",tc)
		replace("timeecc.ino","YYY",tf)


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
print("18. SM2\n")

print("Pairing-Friendly Elliptic Curves")
print("19. BN254")
print("20. BN254CX")
print("21. BLS383")
print("22. BLS381")
print("23. FP256BN")
print("24. FP512BN")
print("25. BLS461")
print("26. BN462")
print("27. BLS24")
print("28. BLS48")
print("29. BLS48_581\n")

print("RSA")
print("30. RSA2048")
print("31. RSA3072")
print("32. RSA4096\n")
print("33. NewHope\n")

selection=[]
ptr=0
max=34

def selected(selection,sel,len) :
	for i in range(0,len):
		if sel==selection[i] :
			return True
	return False


curve_selected=False
pfcurve_selected=False
rsa_selected=False
nhs_selected=False

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
# modulus_mod_8 is the remainder when the modulus is divided by 8
# modulus_type is NOT_SPECIAL, or PSEUDO_MERSENNE, or MONTGOMERY_Friendly, or GENERALISED_MERSENNE (supported for GOLDILOCKS only)
# i for Fp2 QNR 2^i+sqrt(-1) (relevant for PFCs only, else =0)
# curve_type is WEIERSTRASS, EDWARDS or MONTGOMERY
# pairing_friendly is BN, BLS or NOT (if not pairing friendly)
# if pairing friendly. M or D type twist, and sign of the family parameter x
# ate bits is number of bits in Ate parameter (from romgen program)
# g2_table size is number of entries in precomputed table 
# curve security is AES equiavlent, rounded up.

	if x==1:
		curveset("255","F25519","ED25519","29","5","PSEUDO_MERSENNE","0","EDWARDS","NOT","","","","","128")
		curve_selected=True
	if x==2:
		curveset("255","F25519","C25519","29","5","PSEUDO_MERSENNE","0","MONTGOMERY","NOT","","","","","128")
		curve_selected=True
	if x==3:
		curveset("256","NIST256","NIST256","28","7","NOT_SPECIAL","0","WEIERSTRASS","NOT","","","","","128")
		curve_selected=True
	if x==4:
		curveset("256","BRAINPOOL","BRAINPOOL","28","7","NOT_SPECIAL","0","WEIERSTRASS","NOT","","","","","128")
		curve_selected=True
	if x==5:
		curveset("256","ANSSI","ANSSI","28","7","NOT_SPECIAL","0","WEIERSTRASS","NOT","","","","","128")
		curve_selected=True

	if x==6:
		curveset("336","HIFIVE","HIFIVE","29","5","PSEUDO_MERSENNE","0","EDWARDS","NOT","","","","","192")
		curve_selected=True
	if x==7:
		curveset("448","GOLDILOCKS","GOLDILOCKS","29","7","GENERALISED_MERSENNE","0","EDWARDS","NOT","","","","","256")
		curve_selected=True
	if x==8:
		curveset("384","NIST384","NIST384","29","7","NOT_SPECIAL","0","WEIERSTRASS","NOT","","","","","192")
		curve_selected=True
	if x==9:
		curveset("414","C41417","C41417","29","7","PSEUDO_MERSENNE","0","EDWARDS","NOT","","","","","256")
		curve_selected=True
	if x==10:
		curveset("521","NIST521","NIST521","28","7","PSEUDO_MERSENNE","0","WEIERSTRASS","NOT","","","","","256")
		curve_selected=True

	if x==11:
		curveset("256","F256PMW","NUMS256W","28","3","PSEUDO_MERSENNE","0","WEIERSTRASS","NOT","","","","","128")
		curve_selected=True
	if x==12:
		curveset("256","F256PME","NUMS256E","29","3","PSEUDO_MERSENNE","0","EDWARDS","NOT","","","","","128")
		curve_selected=True
	if x==13:
		curveset("384","F384PM","NUMS384W","29","3","PSEUDO_MERSENNE","0","WEIERSTRASS","NOT","","","","","192")
		curve_selected=True
	if x==14:
		curveset("384","F384PM","NUMS384E","29","3","PSEUDO_MERSENNE","0","EDWARDS","NOT","","","","","192")
		curve_selected=True
	if x==15:
		curveset("512","F512PM","NUMS512W","29","7","PSEUDO_MERSENNE","0","WEIERSTRASS","NOT","","","","","256")
		curve_selected=True
	if x==16:
		curveset("512","F512PM","NUMS512E","29","7","PSEUDO_MERSENNE","0","EDWARDS","NOT","","","","","256")
		curve_selected=True

	if x==17:
		curveset("256","SECP256K1","SECP256K1","28","7","NOT_SPECIAL","0","WEIERSTRASS","NOT","","","","","128")
		curve_selected=True
	if x==18:
		curveset("256","SM2","SM2","28","7","NOT_SPECIAL","0","WEIERSTRASS","NOT","","","","","128")
		curve_selected=True


	if x==19:
		curveset("254","BN254","BN254","28","3","NOT_SPECIAL","0","WEIERSTRASS","BN","D_TYPE","NEGATIVEX","71","66","128")
		pfcurve_selected=True
	if x==20:
		curveset("254","BN254CX","BN254CX","28","3","NOT_SPECIAL","0","WEIERSTRASS","BN","D_TYPE","NEGATIVEX","76","66","128")
		pfcurve_selected=True
	if x==21:
		curveset("383","BLS383","BLS383","29","3","NOT_SPECIAL","0","WEIERSTRASS","BLS","M_TYPE","POSITIVEX","68","65","128")
		pfcurve_selected=True

	if x==22:
		curveset("381","BLS381","BLS381","29","3","NOT_SPECIAL","0","WEIERSTRASS","BLS","M_TYPE","NEGATIVEX","69","65","128")
		pfcurve_selected=True


	if x==23:
		curveset("256","FP256BN","FP256BN","28","3","NOT_SPECIAL","0","WEIERSTRASS","BN","M_TYPE","NEGATIVEX","83","66","128")
		pfcurve_selected=True
	if x==24:
		curveset("512","FP512BN","FP512BN","29","3","NOT_SPECIAL","0","WEIERSTRASS","BN","M_TYPE","POSITIVEX","172","130","128")
		pfcurve_selected=True
# https://eprint.iacr.org/2017/334.pdf
	if x==25:
		curveset("461","BLS461","BLS461","28","3","NOT_SPECIAL","0","WEIERSTRASS","BLS","M_TYPE","NEGATIVEX","79","78","128")
		pfcurve_selected=True

	if x==26:
		curveset("462","BN462","BN462","28","3","NOT_SPECIAL","1","WEIERSTRASS","BN","D_TYPE","POSITIVEX","125","118","128")
		pfcurve_selected=True
	if x==27:
		curveset("479","BLS24","BLS24","29","3","NOT_SPECIAL","0","WEIERSTRASS","BLS","M_TYPE","POSITIVEX","52","49","192")
		pfcurve_selected=True

	if x==28:
		curveset("556","BLS48","BLS48","29","3","NOT_SPECIAL","0","WEIERSTRASS","BLS","M_TYPE","POSITIVEX","35","32","256")
		pfcurve_selected=True

	if x==29:
		curveset("581","BLS48_581","BLS48_581","29","3","NOT_SPECIAL","10","WEIERSTRASS","BLS","D_TYPE","NEGATIVEX","36","33","256")
		pfcurve_selected=True
# rsaset(big,ring,big_length_bytes,bit_bits_in_base,multiplier)
# for each choice give distinct names for "big" and "ring".
# Typically "big" is the length in bits of the underlying big number type
# "ring" is the RSA modulus size = "big" times 2^m
# big_length_bytes is "big" divided by 8
# Next give the number base used for 32 bit architectures, as n where the base is 2^n
# multiplier is 2^m (see above)

# There are choices here, different ways of getting the same result, but some faster than others
# There are choices here, different ways of getting the same result, but some faster than others
	if x==30:
		#256 is slower but may allow reuse of 256-bit BIGs used for elliptic curve
		#512 is faster.. but best is 1024
		rsaset("1024","RSA2048","28","2")
		#rsaset("512","RSA2048","29","4")
		#rsaset("256","RSA2048","29","8")
		rsa_selected=True
	if x==31:
		rsaset("384","RSA3072","28","8")
		rsa_selected=True
	if x==32:
		#rsaset("256","RSA4096","29","16")
		rsaset("512","RSA4096","29","8")
		rsa_selected=True

	if x==33:
		nhs_selected=True

	break;

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

if not nhs_selected :
	os.system(deltext+" newhope.cpp")
	os.system(deltext+" newhope.h")
else :
	os.system("mkdir examples")
	os.system("mkdir examples"+slashtext+"testnhs")
	os.system(copytext+" testnhs.ino "+"examples"+slashtext+"testnhs"+slashtext+"testnhs.ino")
os.system(deltext+" testnhs.ino")

if not curve_selected :
	os.system(deltext+" ecdh_support.cpp")
	os.system(deltext+" ecdh_support.h")
else :
	os.system("mkdir examples")
	os.system("mkdir examples"+slashtext+"testecc")
	os.system(copytext+" testecc.ino "+"examples"+slashtext+"testecc"+slashtext+"testecc.ino")
	os.system("mkdir examples"+slashtext+"timeecc")
	os.system(copytext+" timeecc.ino "+"examples"+slashtext+"timeecc"+slashtext+"timeecc.ino")
os.system(deltext+" testecc.ino")
os.system(deltext+" timeecc.ino")

if not rsa_selected :
	os.system(deltext+" rsa_support.cpp")
	os.system(deltext+" rsa_support.h")
	os.system(deltext+" X509.cpp")
	os.system(deltext+" X509.h")
else :
	os.system("mkdir examples")
	os.system("mkdir examples"+slashtext+"testrsa")
	os.system(copytext+" testrsa.ino "+"examples"+slashtext+"testrsa"+slashtext+"testrsa.ino")
os.system(deltext+" testrsa.ino")


if not pfcurve_selected :
	os.system(deltext+" pbc_support.cpp")
	os.system(deltext+" pbc_support.h")
else :
	os.system("mkdir examples")
	os.system("mkdir examples"+slashtext+"testbls")
	os.system(copytext+" testbls.ino "+"examples"+slashtext+"testbls"+slashtext+"testbls.ino")
	os.system("mkdir examples"+slashtext+"timepbc")
	os.system(copytext+" timepbc.ino "+"examples"+slashtext+"timepbc"+slashtext+"timepbc.ino")
	os.system(deltext+ " ecdh*.* ")
os.system(deltext+" testbls.ino")
os.system(deltext+" timepbc.ino")

#print("Your selection was ")
#for i in range(1,max) :
#	if selected(selection,i,ptr) :
#		print (i)

if not selected(selection,1,ptr) and not selected(selection,2,ptr) :
	os.system(deltext+" rom_field_F25519.cpp")
if not selected(selection,1,ptr) :
	os.system(deltext+" rom_curve_ED25519.cpp")
if not selected(selection,2,ptr) :
	os.system(deltext+" rom_curve_C25519.cpp")
if not selected(selection,3,ptr) :
	os.system(deltext+" rom_field_NIST256.cpp")
	os.system(deltext+" rom_curve_NIST256.cpp")
if not selected(selection,4,ptr) :
	os.system(deltext+" rom_field_BRAINPOOL.cpp")
	os.system(deltext+" rom_curve_BRAINPOOL.cpp")
if not selected(selection,5,ptr) :
	os.system(deltext+" rom_field_ANSSI.cpp")
	os.system(deltext+" rom_curve_ANSSI.cpp")
if not selected(selection,6,ptr) :
	os.system(deltext+" rom_field_HIFIVE.cpp")
	os.system(deltext+" rom_curve_HIFIVE.cpp")
if not selected(selection,7,ptr) :
	os.system(deltext+" rom_field_GOLDILOCKS.cpp")
	os.system(deltext+" rom_curve_GOLDILOCKS.cpp")
if not selected(selection,8,ptr) :
	os.system(deltext+" rom_field_NIST384.cpp")
	os.system(deltext+" rom_curve_NIST384.cpp")
if not selected(selection,9,ptr) :
	os.system(deltext+" rom_field_C41417.cpp")
	os.system(deltext+" rom_curve_C41417.cpp")
if not selected(selection,10,ptr) :
	os.system(deltext+" rom_field_NIST521.cpp")
	os.system(deltext+" rom_curve_NIST521.cpp")
if not selected(selection,11,ptr) :
	os.system(deltext+" rom_field_F256PMW.cpp")
	os.system(deltext+" rom_curve_NUMS256W.cpp")
if not selected(selection,12,ptr) :
	os.system(deltext+" rom_field_F256PME.cpp")
	os.system(deltext+" rom_curve_NUMS256E.cpp")

if not selected(selection,13,ptr) and not selected(selection,14,ptr) :
	os.system(deltext+" rom_field_F384PM.cpp")
if not selected(selection,13,ptr) :
	os.system(deltext+" rom_curve_NUMS384W.cpp")
if not selected(selection,14,ptr) :
	os.system(deltext+" rom_curve_NUMS384E.cpp")

if not selected(selection,15,ptr) and not selected(selection,16,ptr) :
	os.system(deltext+" rom_field_F512PM.cpp")
if not selected(selection,15,ptr) :
	os.system(deltext+" rom_curve_NUMS512W.cpp")
if not selected(selection,16,ptr) :
	os.system(deltext+" rom_curve_NUMS512E.cpp")

if not selected(selection,17,ptr) :
	os.system(deltext+" rom_field_SECP256K1.cpp")
	os.system(deltext+" rom_curve_SECP256K1.cpp")

if not selected(selection,18,ptr) :
	os.system(deltext+" rom_field_SM2.cpp")
	os.system(deltext+" rom_curve_SM2.cpp")


if not selected(selection,19,ptr) :
	os.system(deltext+" rom_field_BN254.cpp")
	os.system(deltext+" rom_curve_BN254.cpp")
if not selected(selection,20,ptr) :
	os.system(deltext+" rom_field_BN254CX.cpp")
	os.system(deltext+" rom_curve_BN254CX.cpp")
if not selected(selection,21,ptr) :
	os.system(deltext+" rom_field_BLS383.cpp")
	os.system(deltext+" rom_curve_BLS383.cpp")
if not selected(selection,22,ptr) :
	os.system(deltext+" rom_field_BLS381.cpp")
	os.system(deltext+" rom_curve_BLS381.cpp")
if not selected(selection,23,ptr) :
	os.system(deltext+" rom_field_FP256BN.cpp")
	os.system(deltext+" rom_curve_FP256BN.cpp")
if not selected(selection,24,ptr) :
	os.system(deltext+" rom_field_FP512BN.cpp")
	os.system(deltext+" rom_curve_FP512BN.cpp")
if not selected(selection,25,ptr) :
	os.system(deltext+" rom_field_BLS461.cpp")
	os.system(deltext+" rom_curve_BLS461.cpp")

if not selected(selection,26,ptr) :
	os.system(deltext+" rom_field_BN462.cpp")
	os.system(deltext+" rom_curve_BN462.cpp")

if not selected(selection,27,ptr) :
	os.system(deltext+" rom_field_BLS24.cpp")
	os.system(deltext+" rom_curve_BLS24.cpp")
if not selected(selection,28,ptr) :
	os.system(deltext+" rom_field_BLS48.cpp")
	os.system(deltext+" rom_curve_BLS48.cpp")

if not selected(selection,29,ptr) :
	os.system(deltext+" rom_field_BLS48_581.cpp")
	os.system(deltext+" rom_curve_BLS48_581.cpp")


	os.system(deltext+" testbls.cpp")
	os.system(deltext+" testall.cpp")
	os.system(deltext+" testnhs.cpp")

	os.system(deltext+" config*.py")
	os.system(deltext+" benchtest_all.cpp")
