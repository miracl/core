#
# Python 3.7 Code to implement basic ECDH/ECDSA protocol API
# M.Scott August 2018
#

import hashlib
from constants import *

from XXX import big
from XXX import curve
from XXX.ecp import *

# Calculate a public/private EC GF(p) key pair.


def ECP_KeyPairGenerate(S):
    G = generator()

    if S is None:
        s = big.rand(curve.r)
    else:
        s = big.from_bytes(S) % curve.r

    Y = s * G

    SK = big.to_bytes(s)

    PK = Y.toBytes(False)
    return (SK, PK)

# Validate public key


def ECP_PublicKeyValidate(W):
    r = curve.r
    p = curve.p

    WP = ECp()
    if not WP.fromBytes(W):
        return ECDH_INVALID_PUBLIC_KEY

    nb = p.bit_length()
    k = 1
    k = k << (nb + 4) // 2
    k += p
    k //= r
    while k % 2 == 0:
        WP.dbl()
        k //= 2
    if k != 1:
        WP = k * WP
    if WP.isinf():
        return ECDH_INVALID_PUBLIC_KEY
    return 0

# Get Diffie-Hellman shared key


def ECP_SvdpDH(S, W):
    s = big.from_bytes(S)
    WP = ECp()
    if not WP.fromBytes(W):
        return ECDH_ERROR

    r = curve.r
    s %= r
    WP = s * WP

    if WP.isinf():
        return ECDH_ERROR
    x = WP.getx()

    K = big.to_bytes(x)

    return K

# create ECDSA signature


def ECP_SpDSA(S, F):
    FS = curve.EFS
    G = generator()
    m = hashlib.new(curve.SHA)
    m.update(F)
    H = m.digest()
    HS = m.digest_size
    if HS >= FS:
        B = H[0:FS]
    else:
        B = bytearray(FS)
        for i in range(0, HS):
            B[i + FS - HS] = H[i]

    C = bytearray(FS)
    D = bytearray(FS)

    r = curve.r
    s = big.from_bytes(S)
    f = big.from_bytes(B)

    c = 0
    d = 0
    while d == 0:
        u = big.rand(curve.r)
        w = big.rand(curve.r)  # masking
        V = G.copy()
        V = u * V
        vx = V.getx()
        c = vx % r
        if c == 0:
            continue
        u = big.modmul(u, w, r)
        u = big.invmodp(u, r)
        d = big.modmul(s, c, r)
        d += f
        d = big.modmul(d, w, r)
        d = big.modmul(d, u, r)

    C = big.to_bytes(c)
    D = big.to_bytes(d)

    return C, D

# verify signature


def ECP_SvDSA(P, F, C, D):
    FS = curve.EFS
    G = generator()

    m = hashlib.new(curve.SHA)
    m.update(F)
    H = m.digest()
    HS = m.digest_size
    if HS >= FS:
        B = H[0:FS]
    else:
        B = bytearray(FS)
        for i in range(0, HS):
            B[i + FS - HS] = H[i]
    c = big.from_bytes(C)
    d = big.from_bytes(D)
    f = big.from_bytes(B)

    r = curve.r
    if c == 0 or c >= r or d == 0 or d >= r:
        return False
    d = big.invmodp(d, r)
    f = big.modmul(f, d, r)
    h2 = big.modmul(c, d, r)

    WP = ECp()
    if not WP.fromBytes(P):
        return False
    P = ECp.mul(WP, h2, G, f)

    if P.isinf():
        return False
    d = P.getx() % r
    if c != d:
        return False
    return True
