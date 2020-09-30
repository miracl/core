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

public class CONFIG_CURVE {
    public static final int WEIERSTRASS = 0;
    public static final int EDWARDS = 1;
    public static final int MONTGOMERY = 2;
    public static final int NOT = 0;
    public static final int BN = 1;
    public static final int BLS12 = 2;
    public static final int BLS24 = 3;
    public static final int BLS48 = 4;
    public static final int D_TYPE = 0;
    public static final int M_TYPE = 1;
    public static final int POSITIVEX = 0;
    public static final int NEGATIVEX = 1;

    public static final int CURVETYPE = @CT@;
    public static final int CURVE_A = @CA@;
    public static final int CURVE_PAIRING_TYPE = @PF@;
    public static final int SEXTIC_TWIST = @ST@;
    public static final int SIGN_OF_X = @SX@;
    public static final int ATE_BITS = @AB@;
    public static final int G2_TABLE = @G2@;
    public static final int HTC_ISO = @HC@;
    public static final int HTC_ISO_G2 = @HC2@;

    public static final int SHA256 = 32;
    public static final int SHA384 = 48;
    public static final int SHA512 = 64;

    public static final int HASH_TYPE = @HT@;
    public static final int AESKEY = @AK@;

    public static final boolean ALLOW_ALT_COMPRESS = false;
    public static final boolean USE_GLV = true;
    public static final boolean USE_GS_G2 = true;
    public static final boolean USE_GS_GT = true;
}
