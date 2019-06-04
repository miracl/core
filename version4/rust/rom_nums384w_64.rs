/*
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
*/

use crate::arch::Chunk;
use crate::nums384w::big::NLEN;

// Base Bits= 58
// nums384 Modulus
pub const MODULUS: [Chunk; NLEN] = [
    0x3FFFFFFFFFFFEC3,
    0x3FFFFFFFFFFFFFF,
    0x3FFFFFFFFFFFFFF,
    0x3FFFFFFFFFFFFFF,
    0x3FFFFFFFFFFFFFF,
    0x3FFFFFFFFFFFFFF,
    0xFFFFFFFFF,
];
pub const R2MODP: [Chunk; NLEN] = [0x88900000000000, 0x6, 0x0, 0x0, 0x0, 0x0, 0x0];
pub const MCONST: Chunk = 0x13D;

// nums384w Curve
pub const CURVE_COF_I: isize = 1;
pub const CURVE_A: isize = -3;
pub const CURVE_B_I: isize = -34568;
pub const CURVE_COF: [Chunk; NLEN] = [0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0];
pub const CURVE_B: [Chunk; NLEN] = [
    0x3FFFFFFFFFF77BB,
    0x3FFFFFFFFFFFFFF,
    0x3FFFFFFFFFFFFFF,
    0x3FFFFFFFFFFFFFF,
    0x3FFFFFFFFFFFFFF,
    0x3FFFFFFFFFFFFFF,
    0xFFFFFFFFF,
];
pub const CURVE_ORDER: [Chunk; NLEN] = [
    0x4D81F67B0E61B9,
    0x2A74F530DF89E98,
    0x2F1EEB5D6881BED,
    0x3FFFFFFFFFF587A,
    0x3FFFFFFFFFFFFFF,
    0x3FFFFFFFFFFFFFF,
    0xFFFFFFFFF,
];
pub const CURVE_GX: [Chunk; NLEN] = [
    0x39CBA042098152A,
    0x3BB4403D8711EFA,
    0x291B2A6CC742522,
    0x337EE07E7D3CEF4,
    0x24105F1A60225C1,
    0x5BC60712203288,
    0x757956F0B,
];
pub const CURVE_GY: [Chunk; NLEN] = [
    0x74B8EC66180716,
    0x1AD36EFFD2B4997,
    0x37D121A837EBCD,
    0x1DFE1FDCE72E482,
    0x584CF7EB00469,
    0x66E38E35F8CF4C,
    0xACDEE368E,
];
