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

package TWEEDLEDEE

// Base  bits= 56
var Modulus= [...]Chunk {0x2CAFD400000001,0xA127696286C984,0x38A,0x0,0x40000000}
var R2modp= [...]Chunk {0x21CA8949BBE5DF,0x717E3D9E719200,0xB8A93694FCEE5,0xE6FCB03AA0A27E,0x2013F4C0}
var ROI= [...]Chunk {0xBD3234869D57EC,0x7DD1B8084FBF1C,0x645A9872930A28,0xDC03C0BF1DBCB,0x113EFC51}
var SQRTm3= [...]Chunk {0xFCD5A6291F49FA,0x7AA6D867D6B902,0xBFF2E63FFCD014,0x3C0934B10F5B16,0x2D8CDA74}
const MConst Chunk=0x2CAFD3FFFFFFFF

//*** rom curve parameters *****
const CURVE_Cof_I int= 1
var CURVE_Cof= [...]Chunk {0x1,0x0,0x0,0x0,0x0}
const CURVE_B_I int= 5
var CURVE_B= [...]Chunk {0x5,0x0,0x0,0x0,0x0}
var CURVE_Order= [...]Chunk {0x4064E200000001,0xA1276C3F59B9A1,0x38A,0x0,0x40000000}
var CURVE_Gx= [...]Chunk {0x2CAFD400000000,0xA127696286C984,0x38A,0x0,0x40000000}
var CURVE_Gy= [...]Chunk {0x2,0x0,0x0,0x0,0x0}
var CURVE_HTPC= [...]Chunk {0x861CBDAD69C848,0x754672EF68733,0xE5BC1A0E9704AC,0x2A64972082C6A2,0x3AC188DE}

