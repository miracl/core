public struct CONFIG_FF{
    static public let FFLEN:UInt=@ML@

    static public let FF_BITS:UInt=(CONFIG_BIG.BIGBITS*CONFIG_FF.FFLEN) /* Finite Field Size in bits - must be BIGBITS.2^n */
    static public let HFLEN=(CONFIG_FF.FFLEN/2);  /* Useful for half-size RSA private key operations */

    static let P_MBITS:UInt=CONFIG_BIG.MODBYTES*8
    static let P_OMASK:Chunk=Chunk(-1)<<Chunk(CONFIG_FF.P_MBITS%CONFIG_BIG.BASEBITS)
    static let P_FEXCESS:Chunk=(1<<Chunk(CONFIG_BIG.BASEBITS*UInt(CONFIG_BIG.NLEN)-CONFIG_FF.P_MBITS-1))
    static let P_TBITS=(CONFIG_FF.P_MBITS%CONFIG_BIG.BASEBITS)
}
