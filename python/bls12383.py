
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

from constants import *

SHA = 'sha256'   # hash type to use with this curve
EFS = 48   # Elliptic curve Field Size in bytes
CurveType = WEIERSTRASS
CurveCof = 0x100080000010011FF

SexticTwist = M_TYPE
SignOfX = POSITIVEX
PairingFriendly = BLS
QNRI = 0

x = 0x10008000001001200

if SignOfX == NEGATIVEX:
    p = (x**6 + 2 * x**5 - 2 * x**3 - x + 1) // 3
    t = -x + 1
else:
    p = (x**6 - 2 * x**5 + 2 * x**3 + x + 1) // 3
    t = x + 1

r = x * x * x * x - x * x + 1

# elliptic curve
A = 0
B = 15

# generator point on G1
Gx = 0x41FCBA55B979ECE4E3835F4052DDB050F31D9F76B081F42C2F87BAD84AF1E3445C55DBF083F4770478C4773908734573
Gy = 0x68F167274CFB300024AE9CDC31C46D99D0DADC66BB6449107530A94ADEB4D2DDE57D49EC87F98FD212D165E8003F224

# Frobenius constant
Fra = 0x22ACD5BF027F68BC338B9FC2C11B52F10E4C6CD23FBA1A868256744AE550D8C8A3693480FE6773E01852D72D3311DAC1
Frb = 0x32B880D6622C05F96CE20E015ECF3F939260D95AB3AB8A8B29DB43B684104C2BD7230AA88BFD114BC1E446375298D5EA

# Generator point on G2
Pxa = 0x634D22407EC03E8C07990967CB1E746432501C852D5725BD47F1C90F562572EE81FDAB6795D1D3E143CB3B62D7F2D86
Pxb = 0x300D7800600164BAF37A7717288361DD24F498ED9D05CC89DFD98BA88E92D5D75B54AB28D57FE60DEFD9E41EC452DE15
Pya = 0x33792CF93D225347C949D8573FE6347B6E990ED673C99B3E689783ACA913A2E75B49EDE4F20D61965C68F0BB9408CB41
Pyb = 0x20E5247DD2CD8C6B24EE7BA7D15A7AE001CE0D446B3EFA8F7AF0B2F449F7BC98B0143CB0DE6224863AD846437F479093
