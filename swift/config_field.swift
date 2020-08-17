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

public struct CONFIG_FIELD{
    static public let NOT_SPECIAL=0
    static public let PSEUDO_MERSENNE=1
    static public let MONTGOMERY_FRIENDLY=2
    static public let GENERALISED_MERSENNE=3

    static public let NEGATOWER=0
    static public let POSITOWER=1

    static public let MODBITS:UInt = @NBT@
    static let PM1D2:UInt = @M8@
    static public let MODTYPE =  @MT@   
    static let RIADZ:Int = @RZ@ 
    static let RIADZG2A:Int = @RZ2A@    
    static let RIADZG2B:Int = @RZ2B@   
    static public let QNRI =  @QI@   
    static public let TOWER = @TW@

    static let FEXCESS:Int32 = ((Int32(1)<<@SH@)-1)
    static let OMASK:Chunk=Chunk(-1)<<Chunk(CONFIG_FIELD.MODBITS%CONFIG_BIG.BASEBITS)
    static let TBITS:UInt=CONFIG_FIELD.MODBITS%CONFIG_BIG.BASEBITS; // Number of active bits in top word
    static let TMASK:Chunk=(1<<Chunk(CONFIG_FIELD.TBITS))-1

    static let BIG_ENDIAN_SIGN:Bool = false 

}