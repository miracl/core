# NIST256 curve constants

from constants import *

SHA = 'sha256'   # hash type to use with this curve
EFS = 32   # elliptic curve field size in bytes
CurveType = WEIERSTRASS
CurveCof = 1

# field modulus
p = 115792089210356248762697446949407573530086143415290314195533631308867097853951
r = 115792089210356248762697446949407573529996955224135760342422259061068512044369   # group order

# elliptic curve
A = -3
B = 0x5ac635d8aa3a93e7b3ebbd55769886bc651d06b0cc53b0f63bce3c3e27d2604b

# generator point
Gx = 0x6b17d1f2e12c4247f8bce6e563a440f277037d812deb33a0f4a13945d898c296
Gy = 0x4fe342e2fe1a7f9b8ee7eb4a7c0f9e162bce33576b315ececbb6406837bf51f5
