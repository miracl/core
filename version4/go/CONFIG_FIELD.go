
package XXX

// Modulus types
const NOT_SPECIAL int = 0
const PSEUDO_MERSENNE int = 1
const MONTGOMERY_FRIENDLY int = 2
const GENERALISED_MERSENNE int = 3

const NEGATOWER int = 0
const POSITOWER int = 1

// Modulus details
const MODBITS uint = @NBT@ /* Number of bits in Modulus */
const MOD8 uint = @M8@  /* Modulus mod 8 */
const MODTYPE int = @MT@ //NOT_SPECIAL
const QNRI int = @QI@    // Fp2 QNR
const TOWER int = @TW@   // Tower type
const FEXCESS int32=((int32(1)<<@SH@)-1)

// Modulus Masks
const OMASK Chunk = ((Chunk(-1)) << (MODBITS % BASEBITS))
const TBITS uint = MODBITS % BASEBITS // Number of active bits in top word
const TMASK Chunk = (Chunk(1) << TBITS) - 1

