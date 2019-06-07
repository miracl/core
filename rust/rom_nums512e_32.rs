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
use crate::nums512e::big::NLEN;

// Base Bits= 29
// nums512 Modulus
pub const MODULUS: [Chunk; NLEN] = [
    0x1FFFFDC7, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF,
    0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF,
    0x1FFFFFFF, 0x7FFFF,
];
pub const R2MODP: [Chunk; NLEN] = [
    0xB100000, 0x278, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
    0x0,
];
pub const MCONST: Chunk = 0x239;

// nums512e Curve
pub const CURVE_COF_I: isize = 4;
pub const CURVE_A: isize = 1;
pub const CURVE_B_I: isize = -78296;
pub const CURVE_COF: [Chunk; NLEN] = [
    0x4, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
];
pub const CURVE_B: [Chunk; NLEN] = [
    0x1FFECBEF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF,
    0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF,
    0x1FFFFFFF, 0x7FFFF,
];
pub const CURVE_ORDER: [Chunk; NLEN] = [
    0x1BEED46D, 0x1A3467A8, 0x1BFB3FD9, 0xC0AF0DB, 0x86F52A4, 0xC64B85B, 0x6EA78FF, 0xDA5F9F2,
    0x1FB4F063, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF,
    0x1FFFFFFF, 0x1FFFF,
];
pub const CURVE_GX: [Chunk; NLEN] = [
    0x19EC57FE, 0xDCD594C, 0x113C0571, 0xA4A84F9, 0x104AD0FE, 0x4C92B44, 0xC3DE2F7, 0x9DDC8CE,
    0x74621C1, 0x1139DC0A, 0x9E85FAF, 0x1B894704, 0x1D1E79F4, 0x9E29997, 0x32DE223, 0x16D38F43,
    0x116D128D, 0x6FC71,
];
pub const CURVE_GY: [Chunk; NLEN] = [
    0x1E2F5E1, 0x136EF606, 0x1C7407CC, 0xDA71537, 0xC1FD026, 0x3431576, 0x15898068, 0x1E5D32C6,
    0x120CA53, 0xC84F41A, 0xA4ADAE5, 0x104B3A45, 0x76F726D, 0x1512B772, 0x3D5DEA0, 0x194E3316,
    0x1FF39D49, 0x3684D,
];
