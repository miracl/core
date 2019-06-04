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

/* Fixed Data in ROM - Field and Curve parameters */

package HIFIVE

// Base Bits= 29
var Modulus = [...]Chunk{0x1FFFFFFD, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFF}
var R2modp = [...]Chunk{0x9000000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}

const MConst Chunk = 0x3

const CURVE_Cof_I int = 8

var CURVE_Cof = [...]Chunk{0x8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}

const CURVE_A int = 1
const CURVE_B_I int = 11111

var CURVE_B = [...]Chunk{0x2B67, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}
var CURVE_Order = [...]Chunk{0x1E9FA805, 0x197CACB9, 0x1E4EEA9E, 0x17AD70F, 0x1FA9850C, 0x38A0A, 0x0, 0x0, 0x0, 0x0, 0x0, 0x4000}
var CURVE_Gx = [...]Chunk{0xC, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}
var CURVE_Gy = [...]Chunk{0x5FE8632, 0x15F63428, 0xD976C4, 0x1AACA194, 0x35B6DB5, 0x8E3F7A, 0x52D1B0E, 0xF0A7A36, 0x1C161D00, 0x8170C70, 0x1185AD59, 0x181B}
