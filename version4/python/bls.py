
#
# Python 3.7 Code to implement basic BLS protocol API
# M.Scott August 2018
#

import hashlib
from XXX import ecp
from XXX import ecp2
from XXX import curve
from XXX import big
from XXX.ecp import *
from XXX.ecp2 import *
from XXX import pair
from XXX.fp12 import *

# hash message m to curve point

G2_TAB=[]

def init() :
	global G2_TAB
	G=ecp2.generator()
	G2_TAB=pair.precomp(G)

def BLS_H(m):
    h = hashlib.shake_256()
    h.update(bytes(m, 'utf-8'))
    hm=big.from_bytes(h.digest(curve.EFS))
    HM=ECp()
    while not HM.set(hm):
        hm = hm + 1
    HM = curve.CurveCof * HM;

    return HM 

# generate key pair, private key SK, public key PK

def KeyPairGenerate():
    G=ecp2.generator()
    s = big.rand(curve.r)
    W=s*G
    SK = big.to_bytes(s)
    PK = W.toBytes()
    return (SK,PK)

# Sign message m using private key SK to return signature

def sign(m,SK):
    HM=BLS_H(m)
    s=big.from_bytes(SK)
    D=s*HM
    return D.toBytes(True)

# Verify signature given message m, the signature SIG, and the public key W 

def verify(SIG,m,W):
    HM=BLS_H(m)
    D=ECp()
    D.fromBytes(SIG)
    PK=ECp2()
    PK.fromBytes(W)
    D=-D

# Use new multi-pairing mechanism 
    r=pair.initmp()
    pair.another_pc(r,G2_TAB,D)
    pair.another(r,PK,HM)
    v=pair.miller(r)

#.. or alternatively
#    G=ecp2.generator()
#    v = pair.double_ate(G, D, PK, HM)
    
    v = pair.fexp(v)
    if v.isone():
        return True
    return False
