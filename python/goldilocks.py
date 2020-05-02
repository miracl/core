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
# GOLDILOCKS curve constants

from constants import *

SHA = 'sha512'   # hash type to use with this curve
EFS = 56   # elliptic curve field size in bytes
CurveType = EDWARDS
CurveCof = 4

# field modulus
p = 2**448 - 2**224 - 1
r = 2**446 - 0x8335dc163bb124b65129c96fde933d8d723a70aadc873d6d54a7bb0d

# elliptic curve
A = 1
B = -39081

# generator point
Gx = 0xaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa955555555555555555555555555555555555555555555555555555555
Gy = 0xae05e9634ad7048db359d6205086c2b0036ed7a035884dd7b7e36d728ad8c4b80d6565833a2a3098bbbcb2bed1cda06bdaeafbcdea9386ed
