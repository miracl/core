package org.apache.milagro.amcl.XXX;

public class CONFIG_FIELD {
	public static final int NOT_SPECIAL=0;
	public static final int PSEUDO_MERSENNE=1;
	public static final int MONTGOMERY_FRIENDLY=2;
	public static final int GENERALISED_MERSENNE=3;

	public static final int NEGATOWER=0;
	public static final int POSITOWER=1;

	public static final int MODBITS=@NBT@; /* Number of bits in Modulus */
	public static final int MOD8=@M8@;  /* Modulus mod 8 */
	public static final int MODTYPE=@MT@;
	public static final int QNRI=@QI@;
	public static final int TOWER=@TW@;

	public static final int FEXCESS = (((int)1<<@SH@)-1); // BASEBITS*NLEN-MODBITS or 2^30 max!
}
