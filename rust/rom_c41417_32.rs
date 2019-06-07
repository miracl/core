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
use crate::c41417::big::NLEN;

// Base Bits= 29
// c41417 Modulus
pub const MODULUS: [Chunk; NLEN] = [
    0x1FFFFFEF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF,
    0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0xFF,
];
pub const R2MODP: [Chunk; NLEN] = [
    0x0, 0x242000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
];
pub const MCONST: Chunk = 0x11;

// c41417 Curve
pub const CURVE_COF_I: isize = 8;
pub const CURVE_A: isize = 1;
pub const CURVE_B_I: isize = 3617;
pub const CURVE_COF: [Chunk; NLEN] = [
    0x8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
];
pub const CURVE_B: [Chunk; NLEN] = [
    0xE21, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
];
pub const CURVE_ORDER: [Chunk; NLEN] = [
    0x106AF79, 0x18738D2F, 0x18F3C606, 0x1806715A, 0x22B36F1, 0xA67B830, 0xCF32490, 0x1FFFFFFD,
    0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1F,
];
pub const CURVE_GX: [Chunk; NLEN] = [
    0x13CBC595, 0x7E9C097, 0x14DF1931, 0x14E7F550, 0x1A111301, 0x15A6B6B5, 0xD526292, 0x18FEAFFE,
    0x1F44C03E, 0x1E6A31B4, 0x70C9B97, 0x43180C6, 0x1443300, 0x19A4828A, 0x68,
];
pub const CURVE_GY: [Chunk; NLEN] = [
    0x22, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
];
