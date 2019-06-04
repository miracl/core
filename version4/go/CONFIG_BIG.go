package XXX

// BIG length in bytes and number base
const MODBYTES uint = @NB@
const BASEBITS uint = @BASE@

// BIG lengths and Masks
const NLEN int = int((1 + ((8*MODBYTES - 1) / BASEBITS)))
const DNLEN int = 2 * NLEN
const BMASK Chunk = ((Chunk(1) << BASEBITS) - 1)
const HBITS uint = (BASEBITS / 2)
const HMASK Chunk = ((Chunk(1) << HBITS) - 1)
const NEXCESS int = (1 << (uint(CHUNK) - BASEBITS - 1))

const BIGBITS int = int(MODBYTES * 8)

