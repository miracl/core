
#
# Number Theoretic functions
# M Scott August 2013
#

import math
import types
import random

from XXX import curve


def to_bytes(b):
    return b.to_bytes(curve.EFS, byteorder='big')


def from_bytes(B):
    return int.from_bytes(B, byteorder='big')

# extract i-th bit


def bit(k, i):
    if i == 0:
        return k & 1
    return ((k >> i) & 1)
# gcd


def gcd(x, y):
    a = x
    b = y
    while b != 0:
        a, b = b, a % b
    return a

# inverse mod prime p


def invmodp(a, p):
    n = p
    x = a % n
    kn = n
    if x < 0:
        x += n
    if gcd(x, n) != 1:
        return 0
    a = 1
    la = 0
    while x > 1:
        q, r = divmod(n, x)
        t = la - a * q
        la = a
        a = t
        n = x
        x = r
    if a < 0:
        a += kn
    return a

# Modular arithmetic


def modmul(a1, b1, p):

    a = a1 % p
    b = b1 % p

    if a < 0:
        a += p
    if b < 0:
        b += p
    return a * b % p


def modadd(a, b, p):
    c = a + b
    if c >= p:
        c -= p
    return c


def modsub(a, b, p):
    c = a - b
    if c < 0:
        c += p
    return c


def moddiv(a, b, p):
    i = invmodp(b, p)
    if i != 0:
        return modmul(a, i, p)
    return 0

# modular square root. Fails spectacularly if p!=3 mod 4


def sqrtmodp(a, p):
    if p % 4 == 3:
        return pow(a, (p + 1) // 4, p)

    if p % 8 == 5:
        b = (p - 5) / 8
        i = a * 2
        v = pow(i, b, p)
        i = modmul(i, v, p)
        i = modmul(i, v, p)
        i -= 1
        r = modmul(a, v, p)
        r = modmul(r, i, p)
        return r

    return 0


# chinese remainder theorem


def crt(rp, p, rq, q):
    c = inverse(p, q)
    t = modmult(c, rq - rp, q)
    return t * p + rp

# find jacobi symbol for (x/p). Only defined for
# positive x and p, p odd. Otherwise returns 0


def jacobi(a, p):
    if a < 1 or p < 2 or p % 2 == 0:
        return 0
    n = p
    x = a % n
    m = 0
    while n > 1:
        if x == 0:
            return 0
        n8 = n % 8
        k = 0
        while x % 2 == 0:
            k += 1
            x //= 2
        if k % 2 == 1:
            m += (n8 * n8 - 1) // 8
        m += (n8 - 1) * (x % 4 - 1) // 4
        t = n
        t %= x
        n = x
        x = t
        m %= 2
    if m == 0:
        return 1
    else:
        return -1


def rand(m):
    return random.SystemRandom().randint(2, m - 1)
