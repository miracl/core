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

// Base Bits= 28
var Modulus= [...]Chunk {0x779EC13,0x3710405,0x7AF9EC1,0x9037EF0,0xD4F086A,0xFDD7A5F,0x4532F92,0xD456AFB,0x13B68B9,0xF}
var R2modp= [...]Chunk {0xDA5D1FE,0xDE5B77F,0x2985975,0x3228385,0x95FA423,0xA24A9FC,0xA869FAE,0x6853D0E,0xEC4B765,0x3}
var ROI= [...]Chunk {0x779EC12,0x3710405,0x7AF9EC1,0x9037EF0,0xD4F086A,0xFDD7A5F,0x4532F92,0xD456AFB,0x13B68B9,0xF}
const MConst Chunk=0xAE541E5

const CURVE_Cof_I int= 4
var CURVE_Cof= [...]Chunk {0x4,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0}
const CURVE_B_I int= 0
var CURVE_B= [...]Chunk {0x300CF89,0x9309250,0x1F041EE,0x70F63F2,0x9625F12,0xCCFB0CB,0xA7CDFBF,0x1965B68,0x7130452,0x5}
var CURVE_Order= [...]Chunk {0x1B0F7FB,0x8552F42,0x974BB58,0x8975FAF,0xF534FB3,0xBF75E97,0xD14CBE4,0x7515ABE,0xC4EDA2E,0x3}
var CURVE_Gx= [...]Chunk {0xE68363B,0x2C25BFE,0x713E223,0x2ABB8D2,0xF6350D7,0x1E03CBC,0x3D85B42,0x7F903B8,0x681886A,0xB}
var CURVE_Gy= [...]Chunk {0xD2864B5,0x73C355B,0x93A3A7E,0xBC3D81A,0xA421DA8,0xCF6563D,0x359814D,0xB59C0B0,0xA6734E1,0xC}
var CURVE_HTPC= [...]Chunk {0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0}
