# BN254 curve constants

from constants import *

SHA = 'sha256'   # hash type to use with this curve
EFS = 32   # Elliptic curve Field Size in bytes
CurveType = WEIERSTRASS
CurveCof = 1

SexticTwist = D_TYPE
SignOfX = NEGATIVEX
PairingFriendly = BN
QNRI = 0

x = 0x4000000003C012B1
if SignOfX == NEGATIVEX:
    p = 36 * x * x * x * x - 36 * x * x * x + 24 * x * x - 6 * x + 1
else:
    p = 36 * x * x * x * x + 36 * x * x * x + 24 * x * x + 6 * x + 1

t = 6 * x * x + 1
r = p + 1 - t

# elliptic curve
A = 0
B = 2

# generator point on G1
Gx = p - 1
Gy = 1

# Frobenius constant
Fra = 0x1359082FA63A0164AFE18B8AACA7189868677326F173F8215BD9083355C80EA3
Frb = 0x10A6F7D0623628A900DC53D9CDBC4E3ABBD863C7269546C0628D1BBC06534710

# Generator point on G2

Pxa = 0x1AC95A5B7F9A9B4FA7121C73CB19C2F0944FB8E61B99362851C3E806CB5FAAF5
Pxb = 0x53D3001036566043F11C2D1681F364C1BBA33A1ABDA094DEDE50060FC15F433
Pya = 0x15727970B09E6FB2EE06BBFEF918406479DFF0936EA3484E6833216A9772A299
Pyb = 0x11BCB56C02AC189FD57BD77F56070166D320056091A061ADC41BC11F9C49E8CD
