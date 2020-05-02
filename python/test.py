#
# Copyright (c) 2012-2020 MIRACL UK Ltd.
#
# This file is part of MIRACL Core
# (see https://github.com/miracl/core).
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU Affero General Public License as
#    published by the Free Software Foundation, either version 3 of the
#    License, or (at your option) any later version.

#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU Affero General Public License for more details.

#     https://www.gnu.org/licenses/agpl-3.0.en.html

#    You should have received a copy of the GNU Affero General Public License
#    along with this program.  If not, see <https://www.gnu.org/licenses/>.

#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.

#   You can be released from the requirements of the license by purchasing
#   a commercial license. Buying such a license is mandatory as soon as you
#   develop commercial activities involving the MIRACL Core Crypto SDK
#   without disclosing the source code of your own applications, or shipping
#   the MIRACL Core Crypto SDK with a closed source product.

#
# test driver for ECDH/ECDSA and MPIN APIs
#

import sys
import hashlib
from constants import *

import ed25519.ecdh
import ed25519.curve
import bn254.mpin
import bn254.bls

# first test ECDH/ECDSA on curve ed25519

print("Test ECDH/ECDSA on curve ed25519\n")
salt = bytearray(8)
for i in range(0, 8):
    salt[i] = i + 1
password = bytearray("mongoose".encode())

AS = hashlib.pbkdf2_hmac(ed25519.curve.SHA, password, salt, 1000, 32)

AS, AP = ed25519.ecdh.ECP_KeyPairGenerate(AS)
print("Alices private key  = ", bytes(AS).hex())
print("Alices public key   = ", bytes(AP).hex())

res = ed25519.ecdh.ECP_PublicKeyValidate(AP)
if res != 0:
    print("Alices public Key is invalid")
    exit

SS, SP = ed25519.ecdh.ECP_KeyPairGenerate(None)
print("Servers private key = ", bytes(SS).hex())
print("Servers public key  = ", bytes(SP).hex())

res = ed25519.ecdh.ECP_PublicKeyValidate(SP)
if res != 0:
    print("Servers public Key is invalid")
    exit

KA = ed25519.ecdh.ECP_SvdpDH(AS, SP)
KS = ed25519.ecdh.ECP_SvdpDH(SS, AP)

if KA != KS:
    print("SVDP_DH Failed")
    exit

print("Alice's DH key = ", bytes(KA).hex())
print("Servers DH key = ", bytes(KS).hex())

M = bytearray(16)
for i in range(0, 8):
    M[i] = i

if ed25519.curve.CurveType != MONTGOMERY:
    print("Testing ECDSA")
    print("Signature= ")
    C, D = ed25519.ecdh.ECP_SpDSA(AS, M)

    print("C= ", bytes(C).hex())
    print("D= ", bytes(D).hex())

    if ed25519.ecdh.ECP_SvDSA(AP, M, C, D):
        print("Signature is Valid")
    else:
        print("Signature is NOT Valid")

# Now test BLS signature

print("\nNow test BLS signature on curve bn254\n")
mess = "This is a test message"
mess2 = "This is a test massage"
rtn = bn254.bls.init()
if not rtn:
    print("ERROR: Failed to initialize")
else :
    SK, PK = bn254.bls.KeyPairGenerate()
    print("Secret Key: ", bytes(SK).hex())
    print("Public Key: ", bytes(PK).hex())
    SIG = bn254.bls.sign(mess, SK)
    print("Signature: ", bytes(SIG).hex())
    rtn = bn254.bls.verify(SIG, mess, PK)  # try with mess2
    if rtn:
        print("SUCCESS: Signature is valid")
    else:
        print("ERROR: Signature is NOT valid")

# Now test a simple Pairing-based protocol MPIN on curve bn254

# Assign the User an ID
print("\nNow test MPIN on curve bn254\n")
MPIN_ID = "testuser@miracl.com"
print("MPIN_ID: ", MPIN_ID)

MS = bn254.mpin.random_generate()
print("Master Secret: ", bytes(MS).hex())

SS = bn254.mpin.get_server_secret(MS)
print("Server Secret: ", bytes(SS).hex())

# TA: Generate client secret : token is the full client secret
CS = bn254.mpin.get_client_secret(MS, MPIN_ID)

print("Client Secret: ", bytes(CS).hex())

PIN = int(input("Please enter your four digit PIN to create M-Pin Token: "))

TOKEN = bn254.mpin.extract_pin(MPIN_ID, PIN, CS)
print("Token: ", bytes(TOKEN).hex())

# Client first pass

PIN = int(input("Please enter four digit PIN to authenticate: "))

X, U = bn254.mpin.client_1(MPIN_ID, None)

print("X: ", bytes(X).hex())
print("U: ", bytes(U).hex())

# Server generates Random number Y and sends it to Client

Y = bn254.mpin.random_generate()
print("Y: ", bytes(Y).hex())

# Client second pass

V = bn254.mpin.client_2(X, Y, MPIN_ID, PIN, TOKEN)
print("V: ", bytes(V).hex())

rtn, E, F = bn254.mpin.server(MPIN_ID, Y, SS, U, V)

if rtn:
    print("SUCCESS: Client is authenticated")
else:
    print("ERROR: Client is not authenticated")
    err = bn254.mpin.kangaroo(E, F)
    if err == 0:
        print("Client has invalid Token")
    else:
        print("Client PIN is out by ", err)
