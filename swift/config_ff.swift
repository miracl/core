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

public struct CONFIG_FF{
    static public let FFLEN:UInt=@ML@

    static public let FF_BITS:UInt=(CONFIG_BIG.BIGBITS*CONFIG_FF.FFLEN) /* Finite Field Size in bits - must be BIGBITS.2^n */
    static public let HFLEN=(CONFIG_FF.FFLEN/2);  /* Useful for half-size RSA private key operations */

    static let P_MBITS:UInt=CONFIG_BIG.MODBYTES*8
    static let P_OMASK:Chunk=Chunk(-1)<<Chunk(CONFIG_FF.P_MBITS%CONFIG_BIG.BASEBITS)
    static let P_FEXCESS:Chunk=(1<<Chunk(CONFIG_BIG.BASEBITS*UInt(CONFIG_BIG.NLEN)-CONFIG_FF.P_MBITS-1))
    static let P_TBITS=(CONFIG_FF.P_MBITS%CONFIG_BIG.BASEBITS)
}
