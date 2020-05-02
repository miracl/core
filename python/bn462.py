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
# BN462 curve constants

from constants import *

SHA = 'sha256'   # hash type to use with this curve
EFS = 58   # Elliptic curve Field Size in bytes
CurveType = WEIERSTRASS
CurveCof = 1

SexticTwist = D_TYPE
SignOfX = POSITIVEX
PairingFriendly = BN
QNRI = 1

x = 0x4001FFFFFFFFFFFFFFFFFFFFFBFFF

if SignOfX == NEGATIVEX:
    p = 36 * x * x * x * x - 36 * x * x * x + 24 * x * x - 6 * x + 1
else:
    p = 36 * x * x * x * x + 36 * x * x * x + 24 * x * x + 6 * x + 1

t = 6 * x * x + 1
r = p + 1 - t

# elliptic curve
A = 0
B = 5

# generator point on G1
Gx = 0x21a6d67ef250191fadba34a0a30160b9ac9264b6f95f63b3edbec3cf4b2e689db1bbb4e69a416a0b1e79239c0372e5cd70113c98d91f36b6980d
Gy = 0x0118ea0460f7f7abb82b33676a7432a490eeda842cccfa7d788c659650426e6af77df11b8ae40eb80f475432c66600622ecaa8a5734d36fb03de

# Frobenius constant
Fra = 0x16FF4348CBB40BCFDB5A8CC1172708592ED03A9A87E11C792504D547C0562173310CC78D90D45D1D93FA1180FC7D89659DBDEE4BE3FF2575D1A
Frb = 0x23C37F80940E90EA882008E6B68325B27493CA001E85F7B00D8E4B1DBADB14A1A7A84FF0D00F94F4BD630A20902388657BEB62204AE0955FF85B

# Generator point on G2
Pxa = 0x257CCC85B58DDA0DFB38E3A8CBDC5482E0337E7C1CD96ED61C913820408208F9AD2699BAD92E0032AE1F0AA6A8B48807695468E3D934AE1E4DF
Pxb = 0x1D2E4343E8599102AF8EDCA849566BA3C98E2A354730CBED9176884058B18134DD86BAE555B783718F50AF8B59BF7E850E9B73108BA6AA8CD283
Pya = 0xA0650439DA22C1979517427A20809ECA035634706E23C3FA7A6BB42FE810F1399A1F41C9DDAE32E03695A140E7B11D7C3376E5B68DF0DB7154E
Pyb = 0x73EF0CBD438CBE0172C8AE37306324D44D5E6B0C69AC57B393F1AB370FD725CC647692444A04EF87387AA68D53743493B9EBA14CC552CA2A93A
