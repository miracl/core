# C25519 curve constants

from constants import *

SHA = 'sha256'   # hash type to use with this curve
EFS = 32   # elliptic curve field size in bytes
CurveType = MONTGOMERY
CurveCof = 8

# field modulus
p = 2**255 - 19
r = 2**252 + 27742317777372353535851937790883648493

# elliptic curve
A = 486662
B = 0

# generator point
Gx = 9
Gy = 0
