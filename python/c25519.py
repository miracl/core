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
