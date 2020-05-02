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
# NIST384 curve constants

from constants import *

SHA = 'sha384'   # hash type to use with this curve
EFS = 48   # elliptic curve field size in bytes
CurveType = WEIERSTRASS
CurveCof = 1

# field modulus
p = 39402006196394479212279040100143613805079739270465446667948293404245721771496870329047266088258938001861606973112319
r = 39402006196394479212279040100143613805079739270465446667946905279627659399113263569398956308152294913554433653942643    # group order

# elliptic curve
A = -3
B = 27580193559959705877849011840389048093056905856361568521428707301988689241309860865136260764883745107765439761230575

# generator point
Gx = 26247035095799689268623156744566981891852923491109213387815615900925518854738050089022388053975719786650872476732087
Gy = 8325710961489029985546751289520108179287853048861315594709205902480503199884419224438643760392947333078086511627871
