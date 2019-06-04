package XXX

// Modulus as number of BIGs
const FFLEN int = @ML@

// Modulus length in bits
const FF_BITS int = (BIGBITS * FFLEN) /* Finite Field Size in bits - must be 256.2^n */
const HFLEN int = (FFLEN / 2)         /* Useful for half-size RSA private key operations */

const P_MBITS uint = MODBYTES * 8
const P_OMASK Chunk = (Chunk(-1) << (P_MBITS % BASEBITS))
const P_FEXCESS Chunk = (Chunk(1) << (BASEBITS*uint(NLEN) - P_MBITS - 1))
const P_TBITS uint = (P_MBITS % BASEBITS)

