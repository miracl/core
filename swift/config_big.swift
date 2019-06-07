
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
