#!/usr/bin/env python3

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

#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Affero General Public License as
#    published by the Free Software Foundation, either version 3 of the
#    License, or (at your option) any later version.


#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Affero General Public License for more details.

#     https://www.gnu.org/licenses/agpl-3.0.en.html

#    You should have received a copy of the GNU Affero General Public License
#    along with this program.  If not, see <https://www.gnu.org/licenses/>.

#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.

#   You can be released from the requirements of the license by purchasing     
#   a commercial license. Buying such a license is mandatory as soon as you
#   develop commercial activities involving the MIRACL Core Crypto SDK
#   without disclosing the source code of your own applications, or shipping
#   the MIRACL Core Crypto SDK with a closed source product.     

# BLS12443 curve constants

from constants import *

SHA = 'sha256'   # hash type to use with this curve
EFS = 56   # Elliptic curve Field Size in bytes
CurveType = WEIERSTRASS
CurveCof = 0x403FFF0000011FFFFFF

SexticTwist = M_TYPE
SignOfX = POSITIVEX
PairingFriendly = BLS
QNRI = 0

x = 0x403FFF0000012000000
_x2 = x*x
_x4 = _x2*_x2

if SignOfX == NEGATIVEX:
    p = (_x2*_x4 + 2*(_x4 - _x2)*x - x + 1) // 3
    t = -x + 1
else:
    p = (_x2*_x4 - 2*(_x4 - _x2)*x + x + 1) // 3
    t = x + 1

r = _x4 - _x2 + 1

# elliptic curve
A = 0
B = 4

# generator point on G1
Gx = 0x39E6C4B62517BEDDB681E43412B4E0DF13E7ABA282B7CE9049E8814D9BD1F2978929BD7C18640BFDDFC6EC8ECBE2A6D2E6CA5B278ABC574
Gy = 0x12621F87504385DD443B2B19187E10D9FBDDFAD8CED72F5769DB09E5F3D3F429D63476A5568900742F905D6B69C431DC994157597A9CE3F

# Frobenius constant
Fra = 0x44030931DFF2E447F979B8C6E48295004F76300FB086DD029E275E17CAAF11AFC7C145F9D97D38DC4B811B6AF7EB3463D3B144FC49551C8
Frb = 0x13574AAAD217B83E8FA947E14132C823B8B6BC3F5A32814E7BD7B69F569FDF5B9995DA4F26B6EBCFABD1B1EA5E5D365C2BF965AEC1558E3

# Generator point on G2
Pxa = 0x2C5EC45DECE4C1502219258FB84649C73F22760C16CE339FC8E37A84E305763171730875544DD695AF4DBD1F46E75B865025557F3AF148
Pxb = 0xF64B916EC13B532F64C9DA296B65CE7FAEAFE06E165089BD13DD4ACB43BA5202F3F16872759E2951530E32492D8015BC4F6E6BB36279B6
Pya = 0x45EA070046D510457BD2F919E2C134F4019F34AE6E2149122B65B26E1E164C5BA2783587349A91EFB971B0987719FB7DBFC0EA271C0B975
Pyb = 0x34015DEB1BEA8F586B9E91690DECDC940F938F95D948A372182806D6E17CEEF5EB6062A50D0077B77F8557C8A8F0CF1B3EE1FF0082464AD
