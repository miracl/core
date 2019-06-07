public struct CONFIG_CURVE{
    static public let WEIERSTRASS=0
    static public let EDWARDS=1
    static public let MONTGOMERY=2
    static public let NOT=0
    static public let BN=1
    static public let BLS=2
    static public let D_TYPE=0
    static public let M_TYPE=1
    static public let POSITIVEX=0
    static public let NEGATIVEX=1

    static public let CURVETYPE = @CT@
    static public let CURVE_PAIRING_TYPE = @PF@
    static public let SEXTIC_TWIST = @ST@
    static public let SIGN_OF_X = @SX@
    static public let ATE_BITS = @AB@    

    static public let HASH_TYPE = @HT@
    static public let AESKEY = @AK@

    static let USE_GLV = true
    static let USE_GS_G2 = true
    static let USE_GS_GT = true    
}
