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

// Base Bits= 29
// nums384 Modulus
pub const MODULUS: [Chunk; NLEN] = [
    0x1FFFFEC3, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF,
    0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x7F,
];
pub const R2MODP: [Chunk; NLEN] = [
    0x0, 0x4448000, 0x6, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
];
pub const MCONST: Chunk = 0x13D;

// nums384w Curve
pub const CURVE_COF_I: isize = 1;
pub const CURVE_A: isize = -3;
pub const CURVE_B_I: isize = -34568;
pub const CURVE_COF: [Chunk; NLEN] = [
    0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
];
pub const CURVE_B: [Chunk; NLEN] = [
    0x1FFF77BB, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF,
    0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x7F,
];
pub const CURVE_ORDER: [Chunk; NLEN] = [
    0x1B0E61B9, 0x26C0FB3, 0xDF89E98, 0x153A7A98, 0x16881BED, 0x178F75AE, 0x1FFF587A, 0x1FFFFFFF,
    0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x7F,
];
pub const CURVE_GX: [Chunk; NLEN] = [
    0x98152A, 0x1CE5D021, 0x18711EFA, 0x1DDA201E, 0xC742522, 0x148D9536, 0x7D3CEF4, 0x19BF703F,
    0x60225C1, 0x12082F8D, 0x12203288, 0x2DE3038, 0x17956F0B, 0x3A,
];
pub const CURVE_GY: [Chunk; NLEN] = [
    0x6180716, 0x3A5C763, 0x1D2B4997, 0xD69B77F, 0x837EBCD, 0x1BE890D, 0xE72E482, 0xEFF0FEE,
    0x1EB00469, 0x2C267B, 0x15F8CF4C, 0x3371C71, 0xDEE368E, 0x56,
];
