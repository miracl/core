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

package org.miracl.core.XXX;

public class CONFIG_FIELD {
    public static final int NOT_SPECIAL = 0;
    public static final int PSEUDO_MERSENNE = 1;
    public static final int MONTGOMERY_FRIENDLY = 2;
    public static final int GENERALISED_MERSENNE = 3;

    public static final int NEGATOWER = 0;
    public static final int POSITOWER = 1;

    public static final int MODBITS = @NBT@; /* Number of bits in Modulus */
    public static final int PM1D2 = @M8@; /* Modulus mod 8 */
    public static final int MODTYPE = @MT@;
    public static final int QNRI = @QI@;
    public static final int RIADZ = @RZ@;
    public static final int RIADZG2A = @RZ2A@;
    public static final int RIADZG2B = @RZ2B@;
    public static final int TOWER = @TW@;

    public static final boolean BIG_ENDIAN_SIGN = false;

    public static final int FEXCESS = (((int)1 << @SH@) - 1); // BASEBITS*NLEN-MODBITS or 2^30 max!
}
