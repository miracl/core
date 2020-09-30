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

public struct CONFIG_CURVE{
    static public let WEIERSTRASS=0
    static public let EDWARDS=1
    static public let MONTGOMERY=2
    static public let NOT=0
    static public let BN=1
    static public let BLS12=2
    static public let BLS24=3
    static public let BLS48=4
    static public let D_TYPE=0
    static public let M_TYPE=1
    static public let POSITIVEX=0
    static public let NEGATIVEX=1

    static public let CURVETYPE = @CT@
    static public let CURVE_A = @CA@
    static public let CURVE_PAIRING_TYPE = @PF@
    static public let SEXTIC_TWIST = @ST@
    static public let SIGN_OF_X = @SX@
    static public let ATE_BITS = @AB@    
    static public let HTC_ISO = @HC@
    static public let HTC_ISO_G2 = @HC2@

    static public let HASH_TYPE = @HT@
    static public let AESKEY = @AK@

    static let ALLOW_ALT_COMPRESS = false

    static let USE_GLV = true
    static let USE_GS_G2 = true
    static let USE_GS_GT = true    
}
