
#
# bigs modulo n
# M.Scott August 2013
#

import copy
from XXX import big
from XXX import curve


class Fp:
    p = curve.p

    def __init__(self, x=None):
        if (x is None or x == 0):
            self.x = 0
        else:
            if (x < 0):
                y = -x
                self.x = Fp.p - (y % Fp.p)
            else:
                self.x = x % Fp.p

    def copy(self):
        return copy.copy(self)

    def __add__(self, other):
        return Fp(big.modadd(self.x, other.x, Fp.p))

    def __iadd__(self, other):
        self.x = big.modadd(self.x, other.x, Fp.p)
        return self

    def __sub__(self, other):
        return Fp(big.modsub(self.x, other.x, Fp.p))

    def __isub__(self, other):
        self.x = big.modsub(self.x, other.x, Fp.p)
        return self

    def __mul__(self, other):
        return Fp(big.modmul(self.x, other.x, Fp.p))

    def __imul__(self, other):
        self.x = big.modmul(self.x, other.x, Fp.p)
        return self

    def __neg__(self):
        return Fp(Fp.p - self.x)

    def muli(self, other):
        return Fp((other * self.x) % Fp.p)

    def inverse(self):
        return Fp(big.invmodp(self.x, Fp.p))

    def div2(self):
        if self.x % 2 == 1:
            return Fp((Fp.p + self.x) // 2)
        else:
            return Fp(self.x // 2)

    def __eq__(self, other):
        return self.x == other.x

    def __ne__(self, other):
        return self.x != other.x

    def iszero(self):
        if self.x == 0:
            return True
        return False

    def isone(self):
        if self.x == 1:
            return True
        return False

    def pow(self, other):
        return Fp(pow(self.x, other, Fp.p))

    def jacobi(self):
        return big.jacobi(self.x, Fp.p)

    def gcd(self):
        return big.gcd(self.x, Fp.p)

    def sqrt(self):
        return Fp(big.sqrtmodp(self.x, Fp.p))

    def int(self):
        return self.x

    def rand(self):
        self.x = big.rand(Fp.p)
        return self
