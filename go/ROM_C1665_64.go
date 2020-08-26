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

// Base Bits= 60
var Modulus= [...]Chunk {0xFFFFFFFFFFFFFFB,0xFFFFFFFFFFFFFFF,0x3FFFFFFFFFFF}
var R2modp= [...]Chunk {0x190000000,0x0,0x0}
var ROI= [...]Chunk {0xFFFFFFFFFFFFFFA,0xFFFFFFFFFFFFFFF,0x3FFFFFFFFFFF}
const MConst Chunk=0x5

//*** rom curve parameters *****
// Base Bits= 60

const CURVE_Cof_I int= 4
var CURVE_Cof= [...]Chunk {0x4,0x0,0x0}
const CURVE_B_I int= 5766
var CURVE_B= [...]Chunk {0x1686,0x0,0x0}
var CURVE_Order= [...]Chunk {0x80FF0A99DBA8B27,0xFFFFFFFFFD5EF01,0xFFFFFFFFFFF}
var CURVE_Gx= [...]Chunk {0x671B9DBF9D52398,0x9A0618EE0F666C,0x14C94DA505B8}
var CURVE_Gy= [...]Chunk {0xC7087B244920345,0x13BFC7739D62DC2,0x29414549BC0A}
var CURVE_HTPC= [...]Chunk {0x1,0x0,0x0}
