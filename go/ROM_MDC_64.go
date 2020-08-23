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

package MDC

// Base Bits= 56
var Modulus= [...]Chunk {0x3710405779EC13,0x9037EF07AF9EC1,0xFDD7A5FD4F086A,0xD456AFB4532F92,0xF13B68B9}
var R2modp= [...]Chunk {0xDE5B77FDA5D1FE,0x32283852985975,0xA24A9FC95FA423,0x6853D0EA869FAE,0x3EC4B765}
var ROI= [...]Chunk {0x3710405779EC12,0x9037EF07AF9EC1,0xFDD7A5FD4F086A,0xD456AFB4532F92,0xF13B68B9}
const MConst Chunk=0xF10FDDAAE541E5

//*** rom curve parameters *****
// Base Bits= 56

const CURVE_Cof_I int= 4
var CURVE_Cof= [...]Chunk {0x4,0x0,0x0,0x0,0x0}
const CURVE_B_I int= 0
var CURVE_B= [...]Chunk {0x9309250300CF89,0x70F63F21F041EE,0xCCFB0CB9625F12,0x1965B68A7CDFBF,0x57130452}
var CURVE_Order= [...]Chunk {0x8552F421B0F7FB,0x8975FAF974BB58,0xBF75E97F534FB3,0x7515ABED14CBE4,0x3C4EDA2E}
var CURVE_Gx= [...]Chunk {0x2C25BFEE68363B,0x2ABB8D2713E223,0x1E03CBCF6350D7,0x7F903B83D85B42,0xB681886A}
var CURVE_Gy= [...]Chunk {0x73C355BD2864B5,0xBC3D81A93A3A7E,0xCF6563DA421DA8,0xB59C0B0359814D,0xCA6734E1}
var CURVE_HTPC= [...]Chunk {0x1,0x0,0x0,0x0,0x0}
