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

print("Test ECDH/ECDSA on curve ED25519\n")
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
mess= "This is a test message"
mess2="This is a test massage"
bn254.bls.init()
SK,PK=bn254.bls.KeyPairGenerate()
print("Secret Key: ", bytes(SK).hex())
print("Public Key: ", bytes(PK).hex())
SIG=bn254.bls.sign(mess,SK)
print("Signature: ", bytes(SIG).hex())
rtn=bn254.bls.verify(SIG,mess,PK)       # try with mess2
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

X, U = bn254.mpin.client_1(MPIN_ID,None)

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
