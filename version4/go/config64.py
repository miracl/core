import os
import sys

deltext=""
slashtext=""
copytext=""
if sys.platform.startswith("darwin")  :
	copytext="cp "
	deltext="rm "
	slashtext="/"
if sys.platform.startswith("linux")  :
	copytext="cp "
	deltext="rm "
	slashtext="/"
if sys.platform.startswith("win") :
	copytext="copy "
	deltext="del "
	slashtext="\\"

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

	fpath="core"+slashtext+tb+slashtext
	os.system("mkdir core"+slashtext+tb)

	os.system(copytext+"ARCH64.go "+fpath+"ARCH.go")
	os.system(copytext+"BIG64.go "+fpath+"BIG.go")
	os.system(copytext+"DBIG.go "+fpath+"DBIG.go")
	os.system(copytext+"FF64.go "+fpath+"FF.go")
	os.system(copytext+"RSA.go "+fpath+"RSA.go")
	os.system(copytext+"CONFIG_BIG.go "+fpath+"CONFIG_BIG.go")
	os.system(copytext+"CONFIG_FF.go "+fpath+"CONFIG_FF.go")

	replace(fpath+"ARCH.go","XXX",tb)
	replace(fpath+"CONFIG_BIG.go","XXX",tb)
	replace(fpath+"CONFIG_FF.go","XXX",tb)
	replace(fpath+"BIG.go","XXX",tb)
	replace(fpath+"DBIG.go","XXX",tb)
	replace(fpath+"FF.go","XXX",tb)
	replace(fpath+"RSA.go","XXX",tb)

	replace(fpath+"CONFIG_BIG.go","@NB@",nb)
	replace(fpath+"CONFIG_BIG.go","@BASE@",base)

	replace(fpath+"CONFIG_FF.go","@ML@",ml);

def curveset(tc,base,nbt,m8,mt,qi,ct,pf,stw,sx,g2,ab,cs) :
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

	os.system(copytext+"ARCH64.go "+fpath+"ARCH.go")
	os.system(copytext+"BIG64.go "+fpath+"BIG.go")
	os.system(copytext+"DBIG.go "+fpath+"DBIG.go")
	os.system(copytext+"FP.go "+fpath+"FP.go")
	os.system(copytext+"ECP.go "+fpath+"ECP.go")
	os.system(copytext+"ECDH.go "+fpath+"ECDH.go")
	os.system(copytext+"CONFIG_BIG.go "+fpath+"CONFIG_BIG.go")
	os.system(copytext+"CONFIG_FIELD.go "+fpath+"CONFIG_FIELD.go")
	os.system(copytext+"CONFIG_CURVE.go "+fpath+"CONFIG_CURVE.go")
	os.system(copytext+"ROM_"+tc+"_64.go "+fpath+"ROM.go")

	replace(fpath+"ARCH.go","XXX",tc)
	replace(fpath+"BIG.go","XXX",tc)
	replace(fpath+"DBIG.go","XXX",tc)
	replace(fpath+"FP.go","XXX",tc)
	replace(fpath+"ECP.go","XXX",tc)
	replace(fpath+"ECDH.go","XXX",tc)
	replace(fpath+"CONFIG_BIG.go","XXX",tc)
	replace(fpath+"CONFIG_FIELD.go","XXX",tc)
	replace(fpath+"CONFIG_CURVE.go","XXX",tc)


	replace(fpath+"CONFIG_BIG.go","@NB@",nb)
	replace(fpath+"CONFIG_BIG.go","@BASE@",base)

	replace(fpath+"CONFIG_FIELD.go","@NBT@",nbt)
	replace(fpath+"CONFIG_FIELD.go","@M8@",m8)
	replace(fpath+"CONFIG_FIELD.go","@MT@",mt)

	itw=int(qi)%10
	replace(fpath+"CONFIG_FIELD.go","@QI@",str(itw))
	if int(qi)//10 > 0 :
		replace(fpath+"CONFIG_FIELD.go","@TW@","POSITOWER")
	else :
		replace(fpath+"CONFIG_FIELD.go","@TW@","NEGATOWER")


	ib=int(base)
	inb=int(nb)
	inbt=int(nbt)
	sh=ib*(1+((8*inb-1)//ib))-inbt
	if sh > 30 :
		sh=30
	replace(fpath+"CONFIG_FIELD.go","@SH@",str(sh))


	replace(fpath+"CONFIG_CURVE.go","@CT@",ct)
	replace(fpath+"CONFIG_CURVE.go","@PF@",pf)

	replace(fpath+"CONFIG_CURVE.go","@ST@",stw)
	replace(fpath+"CONFIG_CURVE.go","@SX@",sx)
	replace(fpath+"CONFIG_CURVE.go","@AB@",ab)
	replace(fpath+"CONFIG_CURVE.go","@G2@",g2)

	if cs == "128" :
		replace(fpath+"CONFIG_CURVE.go","@HT@","32")
		replace(fpath+"CONFIG_CURVE.go","@AK@","16")
	if cs == "192" :
		replace(fpath+"CONFIG_CURVE.go","@HT@","48")
		replace(fpath+"CONFIG_CURVE.go","@AK@","24")
	if cs == "256" :
		replace(fpath+"CONFIG_CURVE.go","@HT@","64")
		replace(fpath+"CONFIG_CURVE.go","@AK@","32")

	if pf != "NOT" :

		os.system(copytext+"FP2.go "+fpath+"FP2.go")
		os.system(copytext+"FP4.go "+fpath+"FP4.go")

		replace(fpath+"FP2.go","XXX",tc)
		replace(fpath+"FP4.go","XXX",tc)
		
		if cs == "128" :

			os.system(copytext+"ECP2.go "+fpath+"ECP2.go")
			os.system(copytext+"FP12.go "+fpath+"FP12.go")
			os.system(copytext+"PAIR.go "+fpath+"PAIR.go")
			os.system(copytext+"MPIN.go "+fpath+"MPIN.go")
			os.system(copytext+"BLS.go "+fpath+"BLS.go")

			replace(fpath+"FP12.go","XXX",tc)
			replace(fpath+"ECP2.go","XXX",tc)
			replace(fpath+"PAIR.go","XXX",tc)
			replace(fpath+"MPIN.go","XXX",tc)
			replace(fpath+"BLS.go","XXX",tc)

		if cs == "192" :
			os.system(copytext+"FP24.go "+fpath+"FP24.go")
			os.system(copytext+"FP8.go "+fpath+"FP8.go")
			os.system(copytext+"ECP4.go "+fpath+"ECP4.go")
			os.system(copytext+"PAIR192.go "+fpath+"PAIR192.go")
			os.system(copytext+"MPIN192.go "+fpath+"MPIN192.go")
			os.system(copytext+"BLS192.go "+fpath+"BLS192.go")

			replace(fpath+"FP24.go","XXX",tc)
			replace(fpath+"FP8.go","XXX",tc)
			replace(fpath+"ECP4.go","XXX",tc)
			replace(fpath+"PAIR192.go","XXX",tc)
			replace(fpath+"MPIN192.go","XXX",tc)
			replace(fpath+"BLS192.go","XXX",tc)

		if cs == "256" :
			os.system(copytext+"FP48.go "+fpath+"FP48.go")
			os.system(copytext+"FP16.go "+fpath+"FP16.go")
			os.system(copytext+"FP8.go "+fpath+"FP8.go")
			os.system(copytext+"ECP8.go "+fpath+"ECP8.go")
			os.system(copytext+"PAIR256.go "+fpath+"PAIR256.go")
			os.system(copytext+"MPIN256.go "+fpath+"MPIN256.go")
			os.system(copytext+"BLS256.go "+fpath+"BLS256.go")

			replace(fpath+"FP48.go","XXX",tc)
			replace(fpath+"FP16.go","XXX",tc)
			replace(fpath+"FP8.go","XXX",tc)
			replace(fpath+"ECP8.go","XXX",tc)
			replace(fpath+"PAIR256.go","XXX",tc)
			replace(fpath+"MPIN256.go","XXX",tc)
			replace(fpath+"BLS256.go","XXX",tc)


os.system("mkdir core")
os.system(copytext+ "HASH*.go core"+slashtext+".")
os.system(copytext+ "SHA3.go core"+slashtext+".")
os.system(copytext+ "RAND.go core"+slashtext+".")
os.system(copytext+ "AES.go core"+slashtext+".")
os.system(copytext+ "GCM.go core"+slashtext+".")
os.system(copytext+ "NHS.go core"+slashtext+".")

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
print("26. BN462\n")
print("27. BLS24")
print("28. BLS48")
print("29. BLS48_581\n")

print("RSA")
print("30. RSA2048")
print("31. RSA3072")
print("32. RSA4096")

selection=[]
ptr=0
max=33

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

# curveset(curve,bits_in_base,modulus_bits,modulus_mod_8,modulus_type,curve_type,pairing_friendly,sextic twist,sign of x,g2_table size,ate bits,curve security)
# where "curve" is the common name for the elliptic curve
# bits_in_base gives the number base used for 64 bit architectures, as n where the base is 2^n
# modulus_bits is the actual bit length of the modulus.
# modulus_mod_8 is the remainder when the modulus is divided by 8
# modulus_type is NOT_SPECIAL, or PSEUDO_MERSENNE, or MONTGOMERY_Friendly, or GENERALISED_MERSENNE (supported for GOLDILOCKS only)
# i for FP2 QNR 2^i+sqrt(-1) (relevant for PFCs only, else =0)
# curve_type is WEIERSTRASS, EDWARDS or MONTGOMERY
# pairing_friendly is BN, BLS or NOT (if not pairing friendly
# ate bits is number of bits in Ate parameter (from romgen program)
# g2_table size is number of entries in precomputed table 
# curve security is AES equiavlent, rounded up.


	if x==1:
		curveset("ED25519","56","255","5","PSEUDO_MERSENNE","0","EDWARDS","NOT","NOT","NOT","NOT","NOT","128")
		curve_selected=True
	if x==2:
		curveset("C25519","56","255","5","PSEUDO_MERSENNE","0","MONTGOMERY","NOT","NOT","NOT","NOT","NOT","128")
		curve_selected=True
	if x==3:
		curveset("NIST256","56","256","7","NOT_SPECIAL","0","WEIERSTRASS","NOT","NOT","NOT","NOT","NOT","128")
		curve_selected=True
	if x==4:
		curveset("BRAINPOOL","56","256","7","NOT_SPECIAL","0","WEIERSTRASS","NOT","NOT","NOT","NOT","NOT","128")
		curve_selected=True
	if x==5:
		curveset("ANSSI","56","256","7","NOT_SPECIAL","0","WEIERSTRASS","NOT","NOT","NOT","NOT","NOT","128")
		curve_selected=True

	if x==6:
		curveset("HIFIVE","60","336","5","PSEUDO_MERSENNE","0","EDWARDS","NOT","NOT","NOT","NOT","NOT","192")
		curve_selected=True
	if x==7:
		curveset("GOLDILOCKS","58","448","7","GENERALISED_MERSENNE","0","EDWARDS","NOT","NOT","NOT","NOT","NOT","256")
		curve_selected=True
	if x==8:
		curveset("NIST384","56","384","7","NOT_SPECIAL","0","WEIERSTRASS","NOT","NOT","NOT","NOT","NOT","192")
		curve_selected=True
	if x==9:
		curveset("C41417","60","414","7","PSEUDO_MERSENNE","0","EDWARDS","NOT","NOT","NOT","NOT","NOT","256")
		curve_selected=True
	if x==10:
		curveset("NIST521","60","521","7","PSEUDO_MERSENNE","0","WEIERSTRASS","NOT","NOT","NOT","NOT","NOT","256")
		curve_selected=True

	if x==11:
		curveset("NUMS256W","56","256","3","PSEUDO_MERSENNE","0","WEIERSTRASS","NOT","NOT","NOT","NOT","NOT","128")
		curve_selected=True
	if x==12:
		curveset("NUMS256E","56","256","3","PSEUDO_MERSENNE","0","EDWARDS","NOT","NOT","NOT","NOT","NOT","128")
		curve_selected=True
	if x==13:
		curveset("NUMS384W","58","384","3","PSEUDO_MERSENNE","0","WEIERSTRASS","NOT","NOT","NOT","NOT","NOT","192")
		curve_selected=True
	if x==14:
		curveset("NUMS384E","56","384","3","PSEUDO_MERSENNE","0","EDWARDS","NOT","NOT","NOT","NOT","NOT","192")
		curve_selected=True
	if x==15:
		curveset("NUMS512W","60","512","7","PSEUDO_MERSENNE","0","WEIERSTRASS","NOT","NOT","NOT","NOT","NOT","256")
		curve_selected=True
	if x==16:
		curveset("NUMS512E","60","512","7","PSEUDO_MERSENNE","0","EDWARDS","NOT","NOT","NOT","NOT","NOT","256")
		curve_selected=True

	if x==17:
		curveset("SECP256K1","56","256","7","NOT_SPECIAL","0","WEIERSTRASS","NOT","NOT","NOT","NOT","NOT","128")
		curve_selected=True
	if x==18:
		curveset("SM2","56","256","7","NOT_SPECIAL","0","WEIERSTRASS","NOT","NOT","NOT","NOT","NOT","128")
		curve_selected=True

	if x==19:
		curveset("BN254","56","254","3","NOT_SPECIAL","0","WEIERSTRASS","BN","D_TYPE","NEGATIVEX","71","66","128")
		pfcurve_selected=True
	if x==20:
		curveset("BN254CX","56","254","3","NOT_SPECIAL","0","WEIERSTRASS","BN","D_TYPE","NEGATIVEX","76","66","128")
		pfcurve_selected=True
	if x==21:
		curveset("BLS383","58","383","3","NOT_SPECIAL","0","WEIERSTRASS","BLS","M_TYPE","POSITIVEX","68","65","128")
		pfcurve_selected=True

	if x==22:
		curveset("BLS381","58","381","3","NOT_SPECIAL","0","WEIERSTRASS","BLS","M_TYPE","NEGATIVEX","69","65","128")
		pfcurve_selected=True


	if x==23:
		curveset("FP256BN","56","256","3","NOT_SPECIAL","0","WEIERSTRASS","BN","M_TYPE","NEGATIVEX","83","66","128")
		pfcurve_selected=True
	if x==24:
		curveset("FP512BN","60","512","3","NOT_SPECIAL","0","WEIERSTRASS","BN","M_TYPE","POSITIVEX","172","130","128")
		pfcurve_selected=True
# https://eprint.iacr.org/2017/334.pdf
	if x==25:
		curveset("BLS461","60","461","3","NOT_SPECIAL","0","WEIERSTRASS","BLS","M_TYPE","NEGATIVEX","79","78","128")
		pfcurve_selected=True

	if x==26:
		curveset("BN462","60","462","3","NOT_SPECIAL","1","WEIERSTRASS","BN","D_TYPE","POSITIVEX","125","118","128")
		pfcurve_selected=True

	if x==27:
		curveset("BLS24","56","479","3","NOT_SPECIAL","0","WEIERSTRASS","BLS","M_TYPE","POSITIVEX","52","49","192")
		pfcurve_selected=True

	if x==28:
		curveset("BLS48","58","556","3","NOT_SPECIAL","0","WEIERSTRASS","BLS","M_TYPE","POSITIVEX","35","42","256")
		pfcurve_selected=True

	if x==29:
		curveset("BLS48_581","60","581","3","NOT_SPECIAL","10","WEIERSTRASS","BLS","D_TYPE","NEGATIVEX","36","33","256")
		pfcurve_selected=True


# rsaset(rsaname,big_length_bytes,bits_in_base,multiplier)
# The RSA name reflects the modulus size, which is a 2^m multiplier
# of the underlying big length

# There are choices here, different ways of getting the same result, but some faster than others
	if x==30:
		#256 is slower but may allow reuse of 256-bit BIGs used for elliptic curve
		#512 is faster.. but best is 1024
		rsaset("RSA2048","128","58","2")
		#rsaset("RSA2048","64","60","4")
		#rsaset("RSA2048","32","56","8")
		rsa_selected=True
	if x==31:
		rsaset("RSA3072","48","56","8")
		rsa_selected=True
	if x==32:
		#rsaset("RSA4096","32","56","16")
		rsaset("RSA4096","64","60","8")
		rsa_selected=True
