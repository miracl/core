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

use crate::arch::Chunk;
use crate::nums256w::big::NLEN;

// Base Bits= 56
// nums256 Modulus
pub const MODULUS: [Chunk; NLEN] = [
    0xFFFFFFFFFFFF43,
    0xFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFF,
    0xFFFFFFFF,
];
pub const ROI: [Chunk; NLEN] = [
    0xFFFFFFFFFFFF42,
    0xFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFF,
    0xFFFFFFFF,
];
pub const R2MODP: [Chunk; NLEN] = [0x89000000000000, 0x8B, 0x0, 0x0, 0x0];
pub const MCONST: Chunk = 0xBD;

// nums256w Curve
pub const CURVE_COF_I: isize = 1;
pub const CURVE_B_I: isize = 152961;
pub const CURVE_COF: [Chunk; NLEN] = [0x1, 0x0, 0x0, 0x0, 0x0];
pub const CURVE_B: [Chunk; NLEN] = [0x25581, 0x0, 0x0, 0x0, 0x0];
pub const CURVE_ORDER: [Chunk; NLEN] = [
    0xAB20294751A825,
    0x8275EA265C6020,
    0xFFFFFFFFFFE43C,
    0xFFFFFFFFFFFFFF,
    0xFFFFFFFF,
];
pub const CURVE_GX: [Chunk; NLEN] = [
    0x52EE1EB21AACB1,
    0x9B0903D4C73ABC,
    0xA04F42CB098357,
    0x5AAADB61297A95,
    0xBC9ED6B6,
];
pub const CURVE_GY: [Chunk; NLEN] = [
    0xB5B9CB2184DE9F,
    0xC3D115310FBB80,
    0xF77E04E035C955,
    0x3399B6A673448B,
    0xD08FC0F1,
];
pub const CURVE_HTPC:[Chunk;NLEN]=[0xFC6F75952B84D6,0x92C62040E89E05,0x70CADDC6AE4640,0x5411E3B5B22ED0,0xCF7F44E4];
