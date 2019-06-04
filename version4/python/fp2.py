
#
# Fp^2 CLass
# M.Scott August 2018
#

import copy
from XXX.fp import *

# a+ib, where a,b are Fp, i is "imaginary" sqrt(-1) mod p


class Fp2:

    def __init__(self, a=None, b=None):
        if b is None:
            if a is None:
                self.a = Fp(0)
                self.b = Fp(0)
            else:
                self.a = a.copy()
                self.b = Fp(0)
        else:
            self.a = a.copy()
            self.b = b.copy()

    def copy(self):
        return copy.deepcopy(self)

    def get(self):
        return(self.a.int(), self.b.int())

    def set(self, a, b):
        self.a = Fp(a)
        self.b = Fp(b)
        return self

    def __add__(self, other):
        return Fp2(self.a + other.a, self.b + other.b)

    def __iadd__(self, other):
        self.a += other.a
        self.b += other.b
        return self

    def __sub__(self, other):
        return Fp2(self.a - other.a, self.b - other.b)

    def __isub__(self, other):
        self.a -= other.a
        self.b -= other.b
        return self

    def __eq__(self, other):
        return (self.a == other.a and self.b == other.b)

    def __ne__(self, other):
        return (self.a != other.a or self.b != other.b)

    def conj(self):
        return Fp2(self.a, -self.b)

    def sqr(self):
        newa = (self.a + self.b) * (self.a - self.b)
        self.b *= self.a
        self.b += self.b
        self.a = newa.copy()
        return self

    def times_i(self):
        return Fp2(-self.b,self.a)

    def __imul__(self, other):
        t1 = self.a * other.a
        t2 = self.b * other.b
        t3 = other.a + other.b
        self.b += self.a
        self.b *= t3
        self.b -= t1
        self.b -= t2
        self.a = t1 - t2
        return self

    def __mul__(self, other):
        R = self.copy()
        if R == other:
            R.sqr()
        else:
            R *= other
        return R

    def muli(self, other):
        return Fp2(self.a.muli(other), self.b.muli(other))

    def muls(self, other):
        return Fp2(self.a * other, self.b * other)

    def __neg__(self):
        return Fp2(-self.a, -self.b)

    def real(self):
        return self.a

    def imaginary(self):
        return self.b

    def iszero(self):
        if self.a.iszero() and self.b.iszero():
            return True
        return False

    def isone(self):
        if self.a.isone() and self.b.iszero():
            return True
        return False

    def rand(self):
        r = Fp()
        r.rand()
        self.a = r.copy()
        r.rand()
        self.b = r.copy()
        return self

    def __str__(self):			# pretty print
        return "[%x,%x]" % (self.a.int(), self.b.int())

    def mulQNR(self):				# assume QNR=2^i+sqrt(-1)
        return (self.times_i()+self.muli(1<<curve.QNRI))

    def inverse(self):
        w = self.conj()
        c = self.a * self.a + self.b * self.b
        c = c.inverse()
        w.a *= c
        w.b *= c
        return w

    def div2(self):
        newa = self.a.div2()
        newb = self.b.div2()
        return Fp2(newa, newb)

    def divQNR(self):				# assume QNR=2^i+sqrt(-1)
        z = Fp2(Fp(1<<curve.QNRI),Fp(1))
        return self*z.inverse()
