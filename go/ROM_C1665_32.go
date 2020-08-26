/*
 * Copyright (c) 2012-2020 MIRACL UK Ltd.
 *
 * This file is part of MIRACL Core
 * (see https://github.com/miracl/core).
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* Fixed Data in ROM - Field and Curve parameters */

package C1665

// Base Bits= 29
var Modulus= [...]Chunk {0x1FFFFFFB,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFF}
var R2modp= [...]Chunk {0x190000,0x0,0x0,0x0,0x0,0x0}
var ROI= [...]Chunk {0x1FFFFFFA,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFF}
const MConst Chunk=0x5

//*** rom curve parameters *****
// Base Bits= 29

const CURVE_Cof_I int= 4
var CURVE_Cof= [...]Chunk {0x4,0x0,0x0,0x0,0x0,0x0}
const CURVE_B_I int= 5766
var CURVE_B= [...]Chunk {0x1686,0x0,0x0,0x0,0x0,0x0}
var CURVE_Order= [...]Chunk {0x1DBA8B27,0x7F854C,0x1F57BC06,0x1FFFFFFF,0x1FFFFFFF,0x7FFFF}
var CURVE_Gx= [...]Chunk {0x19D52398,0x138DCEDF,0x183D99B1,0x1340C31D,0x1A505B80,0xA64A6}
var CURVE_Gy= [...]Chunk {0x4920345,0x3843D92,0x758B70B,0x77F8EE7,0x149BC0A1,0x14A0A2}
var CURVE_HTPC= [...]Chunk {0x1,0x0,0x0,0x0,0x0,0x0}
