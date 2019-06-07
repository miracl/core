import os
import sys

deltext=""
slashtext=""
copytext=""
makedir=""
org1text="org"
org2text="miracl"

if sys.platform.startswith("linux")  :
	copytext="cp "
	deltext="rm "
	slashtext="/"
	makedir="mkdir -p "
if sys.platform.startswith("win") :
	copytext="copy "
	deltext="del "
	slashtext="\\"
	makedir="md "

corepath = "core" + slashtext + "src" + slashtext + "main" + slashtext + "java" + slashtext + org1text + slashtext + org2text + slashtext + "core"
coreTestPath = "core" + slashtext + "src" + slashtext + "test" + slashtext + "java" + slashtext + org1text + slashtext + org2text + slashtext +  "core"
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

	fpath=corepath+slashtext+tb+slashtext
	fpathTest=coreTestPath+slashtext+tb+slashtext #ms
	os.system(makedir+corepath+slashtext+tb)
	os.system(makedir+coreTestPath+slashtext+tb) #ms
	
	os.system(copytext+"CONFIG_BIG.java "+fpath+"CONFIG_BIG.java")
	os.system(copytext+"CONFIG_FF.java "+fpath+"CONFIG_FF.java")
	os.system(copytext+"BIG64.java "+fpath+"BIG.java")
	os.system(copytext+"DBIG64.java "+fpath+"DBIG.java")
	os.system(copytext+"FF64.java "+fpath+"FF.java")
	os.system(copytext+"RSA.java "+fpath+"RSA.java")
	os.system(copytext+"private_key.java "+fpath+"private_key.java")
	os.system(copytext+"public_key.java "+fpath+"public_key.java")	
	os.system(copytext+"TestRSA.java "+fpathTest+"TestRSA.java") #ms
	os.system(copytext+"TesttimeRSA.java "+fpathTest+"TesttimeRSA.java")	#ms

	replace(fpath+"CONFIG_BIG.java","XXX",tb)
	replace(fpath+"CONFIG_FF.java","XXX",tb)
	replace(fpath+"BIG.java","XXX",tb)
	replace(fpath+"DBIG.java","XXX",tb)
	replace(fpath+"FF.java","XXX",tb)
	replace(fpath+"RSA.java","XXX",tb)
	replace(fpath+"private_key.java","XXX",tb)
	replace(fpath+"public_key.java","XXX",tb)
	replace(fpathTest+"TestRSA.java","XXX",tb)  #ms
	replace(fpathTest+"TesttimeRSA.java","XXX",tb)  #ms


	replace(fpath+"CONFIG_BIG.java","@NB@",nb)
	replace(fpath+"CONFIG_BIG.java","@BASE@",base)

	replace(fpath+"CONFIG_FF.java","@ML@",ml);


def curveset(tc,base,nbt,m8,mt,qi,ct,pf,stw,sx,g2,ab,cs) :
	global deltext,slashtext,copytext
	global cptr,chosen

	inbt=int(nbt)
	itb=int(inbt+(8-inbt%8)%8)
	inb=int(itb/8)
	nb=str(inb)

	chosen.append(tc)
	cptr=cptr+1

	fpath=corepath+slashtext+tc+slashtext
	fpathTest=coreTestPath+slashtext+tc+slashtext  #ms
	os.system(makedir+corepath+slashtext+tc)
	os.system(makedir+coreTestPath+slashtext+tc)  #ms

	os.system(copytext+"CONFIG_BIG.java "+fpath+"CONFIG_BIG.java")
	os.system(copytext+"CONFIG_FIELD.java "+fpath+"CONFIG_FIELD.java")
	os.system(copytext+"CONFIG_CURVE.java "+fpath+"CONFIG_CURVE.java")
	os.system(copytext+"BIG64.java "+fpath+"BIG.java")
	os.system(copytext+"DBIG64.java "+fpath+"DBIG.java")
	os.system(copytext+"FP64.java "+fpath+"FP.java")
	os.system(copytext+"ECP.java "+fpath+"ECP.java")
	os.system(copytext+"ECDH.java "+fpath+"ECDH.java")
	os.system(copytext+"ROM_"+tc+"_64.java "+fpath+"ROM.java")
	os.system(copytext+"TestECDH.java "+fpathTest+"TestECDH.java")	#ms
	os.system(copytext+"TesttimeECDH.java "+fpathTest+"TesttimeECDH.java")	#ms
	
	replace(fpath+"CONFIG_BIG.java","XXX",tc)	
	replace(fpath+"CONFIG_FIELD.java","XXX",tc)	
	replace(fpath+"CONFIG_CURVE.java","XXX",tc)	
	replace(fpath+"BIG.java","XXX",tc)
	replace(fpath+"DBIG.java","XXX",tc)
	replace(fpath+"FP.java","XXX",tc)
	replace(fpath+"ECP.java","XXX",tc)
	replace(fpath+"ECDH.java","XXX",tc)
	replace(fpathTest+"TestECDH.java","XXX",tc)  #ms
	replace(fpathTest+"TesttimeECDH.java","XXX",tc)  #ms

	replace(fpath+"CONFIG_BIG.java","@NB@",nb)
	replace(fpath+"CONFIG_BIG.java","@BASE@",base)

	replace(fpath+"CONFIG_FIELD.java","@NBT@",nbt)
	replace(fpath+"CONFIG_FIELD.java","@M8@",m8)
	replace(fpath+"CONFIG_FIELD.java","@MT@",mt)

	itw=int(qi)%10
	replace(fpath+"CONFIG_FIELD.java","@QI@",str(itw))
	if int(qi)//10 > 0 :
		replace(fpath+"CONFIG_FIELD.java","@TW@","POSITOWER")
	else :
		replace(fpath+"CONFIG_FIELD.java","@TW@","NEGATOWER")


	ib=int(base)
	inb=int(nb)
	inbt=int(nbt)
	sh=ib*(1+((8*inb-1)//ib))-inbt
	if sh > 30 :
		sh=30
	replace(fpath+"CONFIG_FIELD.java","@SH@",str(sh))


	replace(fpath+"CONFIG_CURVE.java","@CT@",ct)
	replace(fpath+"CONFIG_CURVE.java","@PF@",pf)
	replace(fpath+"CONFIG_CURVE.java","@ST@",stw)
	replace(fpath+"CONFIG_CURVE.java","@SX@",sx)
	replace(fpath+"CONFIG_CURVE.java","@AB@",ab)
	replace(fpath+"CONFIG_CURVE.java","@G2@",g2)

	if cs == "128" :
		replace(fpath+"CONFIG_CURVE.java","@HT@","32")
		replace(fpath+"CONFIG_CURVE.java","@AK@","16")
	if cs == "192" :
		replace(fpath+"CONFIG_CURVE.java","@HT@","48")
		replace(fpath+"CONFIG_CURVE.java","@AK@","24")
	if cs == "256" :
		replace(fpath+"CONFIG_CURVE.java","@HT@","64")
		replace(fpath+"CONFIG_CURVE.java","@AK@","32")

	if pf != "NOT" :
		os.system(copytext+"FP2.java "+fpath+"FP2.java")
		os.system(copytext+"FP4.java "+fpath+"FP4.java")

		replace(fpath+"FP2.java","XXX",tc)
		replace(fpath+"FP4.java","XXX",tc)

		if cs == "128" :

			os.system(copytext+"ECP2.java "+fpath+"ECP2.java")
			os.system(copytext+"FP12.java "+fpath+"FP12.java")
			os.system(copytext+"PAIR.java "+fpath+"PAIR.java")
			os.system(copytext+"MPIN.java "+fpath+"MPIN.java")
			os.system(copytext+"BLS.java "+fpath+"BLS.java")
			os.system(copytext+"TestMPIN.java "+fpathTest+"TestMPIN.java")	#ms
			os.system(copytext+"TestBLS.java "+fpathTest+"TestBLS.java")	#ms
			os.system(copytext+"TesttimeMPIN.java "+fpathTest+"TesttimeMPIN.java")	#ms

			replace(fpath+"FP12.java","XXX",tc)
			replace(fpath+"ECP2.java","XXX",tc)
			replace(fpath+"PAIR.java","XXX",tc)
			replace(fpath+"MPIN.java","XXX",tc)
			replace(fpath+"BLS.java","XXX",tc)
			replace(fpathTest+"TestMPIN.java","XXX",tc)  #ms
			replace(fpathTest+"TestBLS.java","XXX",tc)  #ms
			replace(fpathTest+"TesttimeMPIN.java","XXX",tc)  #ms

		if cs == "192" :
			os.system(copytext+"ECP4.java "+fpath+"ECP4.java")
			os.system(copytext+"FP8.java "+fpath+"FP8.java")
			os.system(copytext+"FP24.java "+fpath+"FP24.java")
			os.system(copytext+"PAIR192.java "+fpath+"PAIR192.java")
			os.system(copytext+"MPIN192.java "+fpath+"MPIN192.java")
			os.system(copytext+"BLS192.java "+fpath+"BLS192.java")
			os.system(copytext+"TestMPIN192.java "+fpathTest+"TestMPIN192.java")	#ms
			os.system(copytext+"TestBLS192.java "+fpathTest+"TestBLS192.java")	#ms
			os.system(copytext+"TesttimeMPIN192.java "+fpathTest+"TesttimeMPIN192.java")	#ms

			replace(fpath+"FP8.java","XXX",tc)
			replace(fpath+"FP24.java","XXX",tc)
			replace(fpath+"ECP4.java","XXX",tc)
			replace(fpath+"PAIR192.java","XXX",tc)
			replace(fpath+"MPIN192.java","XXX",tc)
			replace(fpath+"BLS192.java","XXX",tc)
			replace(fpathTest+"TestMPIN192.java","XXX",tc)  #ms
			replace(fpathTest+"TestBLS192.java","XXX",tc)  #ms
			replace(fpathTest+"TesttimeMPIN192.java","XXX",tc)  #ms

		if cs == "256" :
			os.system(copytext+"FP8.java "+fpath+"FP8.java")
			os.system(copytext+"ECP8.java "+fpath+"ECP8.java")
			os.system(copytext+"FP16.java "+fpath+"FP16.java")
			os.system(copytext+"FP48.java "+fpath+"FP48.java")
			os.system(copytext+"PAIR256.java "+fpath+"PAIR256.java")
			os.system(copytext+"MPIN256.java "+fpath+"MPIN256.java")
			os.system(copytext+"BLS256.java "+fpath+"BLS256.java")
			os.system(copytext+"TestMPIN256.java "+fpathTest+"TestMPIN256.java")	#ms
			os.system(copytext+"TestBLS256.java "+fpathTest+"TestBLS256.java")	#ms
			os.system(copytext+"TesttimeMPIN256.java "+fpathTest+"TesttimeMPIN256.java")	#ms

			replace(fpath+"FP8.java","XXX",tc)
			replace(fpath+"FP16.java","XXX",tc)
			replace(fpath+"FP48.java","XXX",tc)
			replace(fpath+"ECP8.java","XXX",tc)
			replace(fpath+"PAIR256.java","XXX",tc)
			replace(fpath+"MPIN256.java","XXX",tc)
			replace(fpath+"BLS256.java","XXX",tc)
			replace(fpathTest+"TestMPIN256.java","XXX",tc)  #ms
			replace(fpathTest+"TestBLS256.java","XXX",tc)  #ms
			replace(fpathTest+"TesttimeMPIN256.java","XXX",tc)  #ms



os.system(makedir + corepath)

os.system(copytext + "pom.xml " + "core" + slashtext + ".")
for file in ['HASH*.java', 'SHA3.java', 'RAND.java', 'AES.java', 'GCM.java', 'NHS.java']:
	os.system(copytext + file + " " + corepath+slashtext+".")

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

# curveset(curve,big_length_bytes,bits_in_base,modulus_bits,modulus_mod_8,modulus_type,curve_type,pairing_friendly,sextic twist,sign of x,g2_table size,ate bits,curve security)
# where "curve" is the common name for the elliptic curve   
# big_length_bytes is the modulus size rounded up to a number of bytes
# bits_in_base gives the number base used for 64 bit architectures, as n where the base is 2^n
# modulus_bits is the actual bit length of the modulus.
# modulus_mod_8 is the remainder when the modulus is divided by 8
# modulus_type is NOT_SPECIAL, or PSEUDO_MERSENNE, or MONTGOMERY_Friendly, or GENERALISED_MERSENNE (supported for GOLDILOCKS only)
# i for FP2 QNR 2^i+sqrt(-1) (relevant for PFCs only, else =0)
# curve_type is WEIERSTRASS, EDWARDS or MONTGOMERY
# pairing_friendly is BN, BLS or NOT (if not pairing friendly)
# if pairing friendly. M or D type twist, and sign of the family parameter x
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
		curveset("GOLDILOCKS","58","448","7","GENERALISED_MERSENNE","0","EDWARDS","NOT","NOT","NOT","NOT","NOT","256")   # change to 58
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
		curveset("NUMS512E","56","512","7","PSEUDO_MERSENNE","0","EDWARDS","NOT","NOT","NOT","NOT","NOT","256")
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
		curveset("BLS383","58","383","3","NOT_SPECIAL","0","WEIERSTRASS","BLS","M_TYPE","POSITIVEX","68","65","128")  # change to 58
		pfcurve_selected=True

	if x==22:
		curveset("BLS381","58","381","3","NOT_SPECIAL","0","WEIERSTRASS","BLS","M_TYPE","NEGATIVEX","69","65","128")  # change to 58
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
		curveset("BLS48","58","556","3","NOT_SPECIAL","0","WEIERSTRASS","BLS","M_TYPE","POSITIVEX","35","32","256")
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


