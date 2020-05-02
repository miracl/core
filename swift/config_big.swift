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


#if D32
public typealias Chunk = Int32
public typealias DChunk = Int64
#endif

#if D64
public typealias Chunk = Int64
#endif


public struct CONFIG_BIG{
    static let MODBYTES:UInt = @NB@
#if D32
	static public let CHUNK:Int=32
    static let BASEBITS:UInt = @BASE32@    
#endif
#if D64
    static public let CHUNK:Int=64
    static let BASEBITS:UInt = @BASE64@    
#endif  

    static let NLEN:Int=Int(1+((8*CONFIG_BIG.MODBYTES-1)/CONFIG_BIG.BASEBITS))
    static let DNLEN:Int=2*CONFIG_BIG.NLEN
    static let BMASK:Chunk=((1<<Chunk(CONFIG_BIG.BASEBITS))-1)
    //static let HBITS = (CONFIG_BIG.BASEBITS/2)
    //static let HMASK:Chunk = ((1<<Chunk(CONFIG_BIG.HBITS))-1) 
    //static let NEXCESS:Int = (1<<(CONFIG_BIG.CHUNK-Int(CONFIG_BIG.BASEBITS)-1))
    static let BIGBITS:UInt = (CONFIG_BIG.MODBYTES*8)
}
