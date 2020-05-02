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

deltext = ""
slashtext = ""
copytext = ""
org1text = "org"
org2text = "apache"
org3text = "milagro"

if sys.platform.startswith("linux") or sys.platform.startswith("darwin"):
    copytext = "cp "
    deltext = "rm "
    slashtext = "/"
if sys.platform.startswith("win"):
    copytext = "copy "
    deltext = "del "
    slashtext = "\\"

testing=False
if len(sys.argv)==2 :
    if sys.argv[1]=="test":
        testing=True
if testing :
    sys.stdin=open("test.txt","r")

chosen = []
cptr = 0


def replace(namefile, oldtext, newtext):
    f = open(namefile, 'r')
    filedata = f.read()
    f.close()

    newdata = filedata.replace(oldtext, newtext)

    f = open(namefile, 'w')
    f.write(newdata)
    f.close()


def curveset(tc, pf):
    global deltext, slashtext, copytext
    global cptr, chosen

    chosen.append(tc)
    cptr = cptr + 1

    fpath = tc + slashtext
    os.system("mkdir " + tc)

    os.system(copytext + "big.py " + fpath + "big.py")
    os.system(copytext + "fp.py " + fpath + "fp.py")
    os.system(copytext + "ecp.py " + fpath + "ecp.py")
    os.system(copytext + "ecdh.py " + fpath + "ecdh.py")
    os.system(copytext + tc + ".py " + fpath + "curve.py")

    replace(fpath + "big.py", "XXX", tc)
    replace(fpath + "fp.py", "XXX", tc)
    replace(fpath + "ecp.py", "XXX", tc)
    replace(fpath + "ecdh.py", "XXX", tc)

    if pf != "NOT":
        os.system(copytext + "fp2.py " + fpath + "fp2.py")
        os.system(copytext + "fp4.py " + fpath + "fp4.py")

        replace(fpath + "fp2.py", "XXX", tc)
        replace(fpath + "fp4.py", "XXX", tc)

        os.system(copytext + "ecp2.py " + fpath + "ecp2.py")
        os.system(copytext + "fp12.py " + fpath + "fp12.py")
        os.system(copytext + "pair.py " + fpath + "pair.py")
        os.system(copytext + "mpin.py " + fpath + "mpin.py")
        os.system(copytext + "bls.py " + fpath + "bls.py")

        replace(fpath + "fp12.py", "XXX", tc)
        replace(fpath + "ecp2.py", "XXX", tc)
        replace(fpath + "pair.py", "XXX", tc)
        replace(fpath + "mpin.py", "XXX", tc)
        replace(fpath + "bls.py", "XXX", tc)


print("Elliptic Curves")
print("1. ED25519")
print("2. C25519")
print("3. NIST256")
print("4. GOLDILOCKS")
print("5. NIST384")
print("6. NIST521")
print("7. SEC256K1")


print("Pairing-Friendly Elliptic Curves")
print("8. BN254")
print("9. BN254CX")
print("10. BLS12383")
print("11. BLS12381")
print("12. BN462")

selection = []
ptr = 0
max = 13

curve_selected = False
pfcurve_selected = False

while ptr < max:
    x = int(input("Choose a Scheme to support - 0 to finish: "))
    if x == 0:
        break
#	print("Choice= ",x)
    already = False
    for i in range(0, ptr):
        if x == selection[i]:
            already = True
            break
    if already:
        continue

    selection.append(x)
    ptr = ptr + 1

# curveset(curve,pairing_friendly)
# where "curve" is the common name for the elliptic curve
# pairing_friendly is BN, BLS or NOT (if not pairing friendly)

    if x == 1:
        curveset("ed25519", "NOT")
        curve_selected = True
    if x == 2:
        curveset("c25519", "NOT")
        curve_selected = True
    if x == 3:
        curveset("nist256", "NOT")
        curve_selected = True
    if x == 4:
        curveset("goldilocks", "NOT")
        curve_selected = True
    if x == 5:
        curveset("nist384", "NOT")
        curve_selected = True
    if x == 6:
        curveset("nist521", "NOT")
        curve_selected = True
    if x == 6:
        curveset("sec256k1", "NOT")
        curve_selected = True

    if x == 8:
        curveset("bn254", "BN")
        pfcurve_selected = True
    if x == 9:
        curveset("bn254cx", "BN")
        pfcurve_selected = True
    if x == 10:
        curveset("bls12383", "BLS")
        pfcurve_selected = True
    if x == 11:
        curveset("bls12381", "BLS")
        pfcurve_selected = True
    if x == 12:
        curveset("bn462", "BN")
        pfcurve_selected = True

os.system(deltext + " big.py")
os.system(deltext + " fp.py")
os.system(deltext + " ecp.py")
os.system(deltext + " ecdh.py")
os.system(deltext + " fp2.py")
os.system(deltext + " fp4.py")
os.system(deltext + " fp12.py")
os.system(deltext + " mpin.py")
os.system(deltext + " bls.py")
os.system(deltext + " pair.py")
os.system(deltext + " ecp2.py")
os.system(deltext + " c25519.py")
os.system(deltext + " ed25519.py")
os.system(deltext + " nist256.py")
os.system(deltext + " bn254.py")
os.system(deltext + " bn254cx.py")
os.system(deltext + " bls12383.py")
os.system(deltext + " goldilocks.py")
os.system(deltext + " bls12381.py")
os.system(deltext + " bn462.py")
os.system(deltext + " nist384.py")
os.system(deltext + " nist521.py")
os.system(deltext + " sec256k1.py")

if testing:
    os.system("python test.py < pins.txt")
