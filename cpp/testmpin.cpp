/*
   Copyright (C) 2019 MIRACL UK Ltd.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.


    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

     https://www.gnu.org/licenses/agpl-3.0.en.html

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

   You can be released from the requirements of the license by purchasing
   a commercial license. Buying such a license is mandatory as soon as you
   develop commercial activities involving the MIRACL Core Crypto SDK
   without disclosing the source code of your own applications, or shipping
   the MIRACL Core Crypto SDK with a closed source product.
*/

/* test driver and function exerciser for MPIN API Functions */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mpin_BN254.h"
#include "randapi.h"

#if CHUNK==32 || CHUNK==64
#include "mpin_BLS12383.h"
#include "mpin192_BLS24479.h"
#include "mpin256_BLS48556.h"
#endif

using namespace core;

#define TIME_SLOT_MINUTES 1440  /**< Time Slot = 1 day */

unsign32 today(void)
{
    // return time in slots since epoch 
    unsign32 ti = (unsign32)time(NULL);
    return (uint32_t)(ti / (60 * TIME_SLOT_MINUTES));
}

unsign32 GET_TIME(void)
{
    return (unsign32)time(NULL);
}

void HASH_ALL(int hlen, octet *HID, octet *xID, octet *xCID, octet *SEC, octet *Y, octet *R, octet *W, octet *H)
{
    char t[1284];   // assumes max modulus of 1024-bits
    octet T = {0, sizeof(t), t};

    OCT_joctet(&T, HID);
    if (xCID != NULL) OCT_joctet(&T, xCID);
    else OCT_joctet(&T, xID);
    OCT_joctet(&T, SEC);
    OCT_joctet(&T, Y);
    OCT_joctet(&T, R);
    OCT_joctet(&T, W);

    GPhash(MC_SHA2,hlen,H,H->max,NULL,-1,&T);
    //mhashit(sha, 0, &T, H);
}

void HASH_ID(int hlen, octet *ID, octet *HID)
{
    GPhash(MC_SHA2,hlen,HID,HID->max,NULL,-1,ID);
    //mhashit(sha, 0, ID, HID);
}

#define PERMITS  // for time permits ON or OFF 
#define PINERROR // For PIN ERROR detection ON or OFF 
#define FULL     // for M-Pin Full or M-Pin regular 

int mpin_BN254(csprng *RNG)
{

    using namespace BN254;

    int i, pin, rtn, err;
#ifdef PERMITS
    int date = today();
#else
    int date = 0;
#endif
    unsigned long ran;
    char x[PGS_BN254], s[PGS_BN254], y[PGS_BN254], client_id[100], sst[4 * PFS_BN254 + 1], token[2 * PFS_BN254 + 1], sec[2 * PFS_BN254 + 1], permit[2 * PFS_BN254 + 1], xcid[2 * PFS_BN254 + 1], xid[2 * PFS_BN254 + 1], e[12 * PFS_BN254], f[12 * PFS_BN254];
    char hcid[PFS_BN254], hsid[PFS_BN254], hid[2 * PFS_BN254 + 1], htid[2 * PFS_BN254 + 1], h[PGS_BN254];
#ifdef FULL
    char r[PGS_BN254], z[2 * PFS_BN254 + 1], w[PGS_BN254], t[2 * PFS_BN254 + 1];
    char g1[12 * PFS_BN254], g2[12 * PFS_BN254];
    char ck[AESKEY_BN254], sk[AESKEY_BN254];
#endif
    octet S = {0, sizeof(s), s};
    octet X = {0, sizeof(x), x};
    octet Y = {0, sizeof(y), y};
    octet H = {0, sizeof(h), h};
    octet CLIENT_ID = {0, sizeof(client_id), client_id};
    octet SST = {0, sizeof(sst), sst};
    octet TOKEN = {0, sizeof(token), token};
    octet SEC = {0, sizeof(sec), sec};
    octet PERMIT = {0, sizeof(permit), permit};
    octet xCID = {0, sizeof(xcid), xcid};
    octet xID = {0, sizeof(xid), xid};
    octet HCID = {0, sizeof(hcid), hcid};
    octet HSID = {0, sizeof(hsid), hsid};
    octet HID = {0, sizeof(hid), hid};
    octet HTID = {0, sizeof(htid), htid};
    octet E = {0, sizeof(e), e};
    octet F = {0, sizeof(f), f};
#ifdef FULL
    octet R = {0, sizeof(r), r};
    octet Z = {0, sizeof(z), z};
    octet W = {0, sizeof(w), w};
    octet T = {0, sizeof(t), t};
    octet G1 = {0, sizeof(g1), g1};
    octet G2 = {0, sizeof(g2), g2};
    octet SK = {0, sizeof(sk), sk};
    octet CK = {0, sizeof(ck), ck};
#endif
    octet *pxID, *pxCID, *pHID, *pHTID, *pE, *pF, *pPERMIT, *prHID;
    char idhex[100];

    // Trusted Authority set-up
    MPIN_RANDOM_GENERATE(RNG, &S);
    printf("Master Secret= ");
    OCT_output(&S);

    // Create Client Identity
    OCT_jstring(&CLIENT_ID, (char *)"testUser@miracl.com");
    HASH_ID(HASH_TYPE_BN254, &CLIENT_ID, &HCID); // Either Client or TA calculates Hash(ID) - you decide!

    printf("Client ID Hash= ");
    OCT_output(&HCID);
    printf("\n");

    OCT_toHex(&CLIENT_ID, idhex);
    printf("Client ID= %s\n", idhex); // OCT_toHex(&CLIENT_ID); printf("\n");

    MPIN_GET_CLIENT_SECRET(&S, &HCID, &TOKEN);
    printf("Client Secret= ");
    OCT_output(&TOKEN);


// Client and Server are issued secrets by DTA
    MPIN_GET_SERVER_SECRET(&S, &SST);
    printf("Server Secret= ");
    OCT_output(&SST);

    // Client extracts PIN from secret to create Token
    pin = 1234;
    printf("Client extracts PIN= %d\n", pin);
    MPIN_EXTRACT_PIN(HASH_TYPE_BN254, &CLIENT_ID, pin, &TOKEN);
    printf("Client Token= ");
    OCT_output(&TOKEN);

#ifdef FULL
    MPIN_PRECOMPUTE(&TOKEN, &HCID, NULL, &G1, &G2);
#endif

#ifdef PERMITS
    // Client gets "Time Permit" from DTA
    printf("Client gets Time Permit\n");

    MPIN_GET_CLIENT_PERMIT(HASH_TYPE_BN254, date, &S, &HCID, &PERMIT);
    printf("Time Permit= ");
    OCT_output(&PERMIT);

    // This encoding makes Time permit look random
    if (MPIN_ENCODING(RNG, &PERMIT) != 0) printf("Encoding error\n");
    // printf("Encoded Time Permit= "); OCT_output(&PERMIT);
    if (MPIN_DECODING(&PERMIT) != 0) printf("Decoding error\n");
    // printf("Decoded Time Permit= "); OCT_output(&PERMIT);
#endif

    // MPin Protocol

    // Client enters PIN
    printf("\nPIN= ");
    if (scanf("%d", &pin)) {};
    // to avoid silly compile error
    getchar();

    // Set date=0 and PERMIT=NULL if time permits not in use

    // Client First pass: Inputs CLIENT_ID, optional RNG, pin, TOKEN and PERMIT. Output xID = x.H(CLIENT_ID) and re-combined secret SEC
    // If PERMITS are is use, then date!=0 and PERMIT is added to secret and xCID = x.(H(CLIENT_ID)+H(date|H(CLIENT_ID)))
    // Random value x is supplied externally if RNG=NULL, otherwise generated and passed out by RNG

    // HSID - hashed client ID as calculated by the server
    // HCID - hashed client ID as calculated by the client

    // IMPORTANT: To save space and time..
    // If Time Permits OFF set xCID = NULL, HTID=NULL and use xID and HID only
    // If Time permits are ON, AND pin error detection is required then all of xID, xCID, HID and HTID are required
    // If Time permits are ON, AND pin error detection is NOT required, set xID=NULL, HID=NULL and use xCID and HTID only.



    pxID = &xID;
    pxCID = &xCID;
    pHID = &HID;
    pHTID = &HTID;
    pE = &E;
    pF = &F;
    pPERMIT = &PERMIT;

#ifdef PERMITS
    prHID = pHTID;
#ifndef PINERROR
    pxID = NULL;
//   pHID=NULL;  //new
#endif
#else
    prHID = pHID;
    pPERMIT = NULL;
    pxCID = NULL;
    pHTID = NULL;
#endif
#ifndef PINERROR
    pE = NULL;
    pF = NULL;
#endif

    // When set only send hashed IDs to server
    octet *pID;
#ifdef USE_ANONYMOUS
    pID = &HCID;
#else
    pID = &CLIENT_ID;
#endif

#ifdef SINGLE_PASS
    int timeValue;
    printf("MPIN Single Pass\n");
    timeValue = MPIN_GET_TIME();

    rtn = MPIN_CLIENT(HASH_TYPE_BN254, date, &CLIENT_ID, RNG, &X, pin, &TOKEN, &SEC, pxID, pxCID, pPERMIT, NULL, timeValue, &Y);

    if (rtn != 0)
    {
        printf("MPIN_CLIENT ERROR %d\n", rtn);
        return 1;
    }

#ifdef FULL
    MPIN_GET_G1_MULTIPLE(RNG, 1, &R, &HCID, &Z); // Also Send Z=r.ID to Server, remember random r
#endif


    rtn = MPIN_SERVER(HASH_TYPE_BN254, date, pHID, pHTID, &Y, &SST, pxID, pxCID, &SEC, pE, pF, pID, NULL, timeValue, NULL);

#ifdef FULL
    HASH_ID(HASH_TYPE_BN254, &CLIENT_ID, &HSID); // new
    MPIN_GET_G1_MULTIPLE(RNG, 0, &W, prHID, &T); // Also send T=w.ID to client, remember random w
#endif

#else // SINGLE_PASS
    printf("MPIN Multi Pass\n");
    if (MPIN_CLIENT_1(HASH_TYPE_BN254, date, &CLIENT_ID, RNG, &X, pin, &TOKEN, &SEC, pxID, pxCID, pPERMIT) != 0)
    {
        printf("Error from Client side - First Pass\n");
        return 0;
    }

    // Send U=x.ID to server, and recreate secret from token and pin

#ifdef FULL
    HASH_ID(HASH_TYPE_BN254, &CLIENT_ID, &HCID);
    MPIN_GET_G1_MULTIPLE(RNG, 1, &R, &HCID, &Z); // Also Send Z=r.ID to Server, remember random r, DH component
#endif

    // Server calculates H(ID) and H(ID)+H(T|H(ID)) (if time permits enabled), and maps them to points on the curve HID and HTID resp.
    MPIN_SERVER_1(HASH_TYPE_BN254, date, pID, pHID, pHTID);

    // Server generates Random number Y and sends it to Client
    MPIN_RANDOM_GENERATE(RNG, &Y);

#ifdef FULL
    HASH_ID(HASH_TYPE_BN254, &CLIENT_ID, &HSID); //new
    MPIN_GET_G1_MULTIPLE(RNG, 0, &W, prHID, &T); // Also send T=w.ID to client, remember random w, DH component
#endif

    // Client Second Pass: Inputs Client secret SEC, x and y. Outputs -(x+y)*SEC
    if (MPIN_CLIENT_2(&X, &Y, &SEC) != 0)
    {
        printf("Error from Client side - Second Pass\n");
        return 1;
    }

    // Server Second phase. Inputs hashed client id, random Y, -(x+y)*SEC, xID and xCID and Server secret SST. E and F help kangaroos to find error.
    // If PIN error not required, set E and F = NULL
    rtn = MPIN_SERVER_2(date, pHID, pHTID, &Y, &SST, pxID, pxCID, &SEC, pE, pF, NULL);
#endif // SINGLE_PASS

    if (rtn != 0)
    {
        printf("Server says - Bad Pin.\n");
#ifdef PINERROR

        err = MPIN_KANGAROO(&E, &F);
        if (err) printf("(Client PIN is out by %d)\n", err);

#endif
        return 1;
    }
    else
    {
        printf("Server says - PIN is good! You really are ");
        OCT_output_string(&CLIENT_ID);
        printf(".\n");
    }

#ifdef FULL

    HASH_ALL(HASH_TYPE_BN254, &HCID, pxID, pxCID, &SEC, &Y, &Z, &T, &H); // new
    MPIN_CLIENT_KEY(HASH_TYPE_BN254, &G1, &G2, pin, &R, &X, &H, &T, &CK); // new H
    printf("Client Key = ");
    OCT_output(&CK);

    HASH_ALL(HASH_TYPE_BN254, &HSID, pxID, pxCID, &SEC, &Y, &Z, &T, &H);
    MPIN_SERVER_KEY(HASH_TYPE_BN254, &Z, &SST, &W, &H, pHID, pxID, pxCID, &SK); // new H,pHID
    printf("Server Key = ");
    OCT_output(&SK);
#endif
    return 1;
}

#if CHUNK==32 || CHUNK==64

int mpin_BLS12383(csprng *RNG)
{

    using namespace BLS12383;

    int i, pin, rtn, err;
#ifdef PERMITS
    int date = today();
#else
    int date = 0;
#endif
    unsigned long ran;
    char x[PGS_BLS12383], s[PGS_BLS12383], y[PGS_BLS12383], client_id[100], sst[4 * PFS_BLS12383 + 1], token[2 * PFS_BLS12383 + 1], sec[2 * PFS_BLS12383 + 1], permit[2 * PFS_BLS12383 + 1], xcid[2 * PFS_BLS12383 + 1], xid[2 * PFS_BLS12383 + 1], e[12 * PFS_BLS12383], f[12 * PFS_BLS12383];
    char hcid[PFS_BLS12383], hsid[PFS_BLS12383], hid[2 * PFS_BLS12383 + 1], htid[2 * PFS_BLS12383 + 1], h[PGS_BLS12383];
#ifdef FULL
    char r[PGS_BLS12383], z[2 * PFS_BLS12383 + 1], w[PGS_BLS12383], t[2 * PFS_BLS12383 + 1];
    char g1[12 * PFS_BLS12383], g2[12 * PFS_BLS12383];
    char ck[AESKEY_BLS12383], sk[AESKEY_BLS12383];
#endif
    octet S = {0, sizeof(s), s};
    octet X = {0, sizeof(x), x};
    octet Y = {0, sizeof(y), y};
    octet H = {0, sizeof(h), h};
    octet CLIENT_ID = {0, sizeof(client_id), client_id};
    octet SST = {0, sizeof(sst), sst};
    octet TOKEN = {0, sizeof(token), token};
    octet SEC = {0, sizeof(sec), sec};
    octet PERMIT = {0, sizeof(permit), permit};
    octet xCID = {0, sizeof(xcid), xcid};
    octet xID = {0, sizeof(xid), xid};
    octet HCID = {0, sizeof(hcid), hcid};
    octet HSID = {0, sizeof(hsid), hsid};
    octet HID = {0, sizeof(hid), hid};
    octet HTID = {0, sizeof(htid), htid};
    octet E = {0, sizeof(e), e};
    octet F = {0, sizeof(f), f};
#ifdef FULL
    octet R = {0, sizeof(r), r};
    octet Z = {0, sizeof(z), z};
    octet W = {0, sizeof(w), w};
    octet T = {0, sizeof(t), t};
    octet G1 = {0, sizeof(g1), g1};
    octet G2 = {0, sizeof(g2), g2};
    octet SK = {0, sizeof(sk), sk};
    octet CK = {0, sizeof(ck), ck};
#endif
    octet *pxID, *pxCID, *pHID, *pHTID, *pE, *pF, *pPERMIT, *prHID;
    char idhex[100];

    // Trusted Authority set-up
    MPIN_RANDOM_GENERATE(RNG, &S);
    printf("Master Secret= ");
    OCT_output(&S);

    // Create Client Identity
    OCT_jstring(&CLIENT_ID, (char *)"testUser@miracl.com");
    HASH_ID(HASH_TYPE_BLS12383, &CLIENT_ID, &HCID); // Either Client or TA calculates Hash(ID) - you decide!

    printf("Client ID Hash= ");
    OCT_output(&HCID);
    printf("\n");

    OCT_toHex(&CLIENT_ID, idhex);
    printf("Client ID= %s\n", idhex); // OCT_toHex(&CLIENT_ID); printf("\n");

    MPIN_GET_CLIENT_SECRET(&S, &HCID, &TOKEN);
    printf("Client Secret= ");
    OCT_output(&TOKEN);

// Client and Server are issued secrets by DTA
    MPIN_GET_SERVER_SECRET(&S, &SST);
    printf("Server Secret= ");
    OCT_output(&SST);



    // Client extracts PIN from secret to create Token
    pin = 1234;
    printf("Client extracts PIN= %d\n", pin);
    MPIN_EXTRACT_PIN(HASH_TYPE_BLS12383, &CLIENT_ID, pin, &TOKEN);
    printf("Client Token= ");
    OCT_output(&TOKEN);

#ifdef FULL
    MPIN_PRECOMPUTE(&TOKEN, &HCID, NULL, &G1, &G2);
#endif

#ifdef PERMITS
    // Client gets "Time Permit" from DTA
    printf("Client gets Time Permit\n");

    MPIN_GET_CLIENT_PERMIT(HASH_TYPE_BLS12383, date, &S, &HCID, &PERMIT);
    printf("Time Permit= ");
    OCT_output(&PERMIT);

    // This encoding makes Time permit look random
    if (MPIN_ENCODING(RNG, &PERMIT) != 0) printf("Encoding error\n");
    // printf("Encoded Time Permit= "); OCT_output(&PERMIT);
    if (MPIN_DECODING(&PERMIT) != 0) printf("Decoding error\n");
    // printf("Decoded Time Permit= "); OCT_output(&PERMIT);
#endif

    // MPin Protocol

    // Client enters PIN
    printf("\nPIN= ");
    if (scanf("%d", &pin)) {};
    // to avoid silly compile error
    getchar();

    // Set date=0 and PERMIT=NULL if time permits not in use

    // Client First pass: Inputs CLIENT_ID, optional RNG, pin, TOKEN and PERMIT. Output xID = x.H(CLIENT_ID) and re-combined secret SEC
    // If PERMITS are is use, then date!=0 and PERMIT is added to secret and xCID = x.(H(CLIENT_ID)+H(date|H(CLIENT_ID)))
    // Random value x is supplied externally if RNG=NULL, otherwise generated and passed out by RNG

    // HSID - hashed client ID as calculated by the server
    // HCID - hashed client ID as calculated by the client

    // IMPORTANT: To save space and time..
    // If Time Permits OFF set xCID = NULL, HTID=NULL and use xID and HID only
    // If Time permits are ON, AND pin error detection is required then all of xID, xCID, HID and HTID are required
    // If Time permits are ON, AND pin error detection is NOT required, set xID=NULL, HID=NULL and use xCID and HTID only.



    pxID = &xID;
    pxCID = &xCID;
    pHID = &HID;
    pHTID = &HTID;
    pE = &E;
    pF = &F;
    pPERMIT = &PERMIT;

#ifdef PERMITS
    prHID = pHTID;
#ifndef PINERROR
    pxID = NULL;
//   pHID=NULL;  //new
#endif
#else
    prHID = pHID;
    pPERMIT = NULL;
    pxCID = NULL;
    pHTID = NULL;
#endif
#ifndef PINERROR
    pE = NULL;
    pF = NULL;
#endif

    // When set only send hashed IDs to server
    octet *pID;
#ifdef USE_ANONYMOUS
    pID = &HCID;
#else
    pID = &CLIENT_ID;
#endif

#ifdef SINGLE_PASS
    int timeValue;
    printf("MPIN Single Pass\n");
    timeValue = MPIN_GET_TIME();

    rtn = MPIN_CLIENT(HASH_TYPE_BLS12383, date, &CLIENT_ID, RNG, &X, pin, &TOKEN, &SEC, pxID, pxCID, pPERMIT, NULL, timeValue, &Y);

    if (rtn != 0)
    {
        printf("MPIN_CLIENT ERROR %d\n", rtn);
        return 1;
    }

#ifdef FULL
    MPIN_GET_G1_MULTIPLE(RNG, 1, &R, &HCID, &Z); // Also Send Z=r.ID to Server, remember random r
#endif


    rtn = MPIN_SERVER(HASH_TYPE_BLS12383, date, pHID, pHTID, &Y, &SST, pxID, pxCID, &SEC, pE, pF, pID, NULL, timeValue, NULL);

#ifdef FULL
    HASH_ID(HASH_TYPE_BLS12383, &CLIENT_ID, &HSID); // new
    MPIN_GET_G1_MULTIPLE(RNG, 0, &W, prHID, &T); // Also send T=w.ID to client, remember random w
#endif

#else // SINGLE_PASS
    printf("MPIN Multi Pass\n");
    if (MPIN_CLIENT_1(HASH_TYPE_BLS12383, date, &CLIENT_ID, RNG, &X, pin, &TOKEN, &SEC, pxID, pxCID, pPERMIT) != 0)
    {
        printf("Error from Client side - First Pass\n");
        return 0;
    }

    // Send U=x.ID to server, and recreate secret from token and pin

#ifdef FULL
    HASH_ID(HASH_TYPE_BLS12383, &CLIENT_ID, &HCID);
    MPIN_GET_G1_MULTIPLE(RNG, 1, &R, &HCID, &Z); // Also Send Z=r.ID to Server, remember random r, DH component
#endif

    // Server calculates H(ID) and H(ID)+H(T|H(ID)) (if time permits enabled), and maps them to points on the curve HID and HTID resp.
    MPIN_SERVER_1(HASH_TYPE_BLS12383, date, pID, pHID, pHTID);

    // Server generates Random number Y and sends it to Client
    MPIN_RANDOM_GENERATE(RNG, &Y);

#ifdef FULL
    HASH_ID(HASH_TYPE_BLS12383, &CLIENT_ID, &HSID); //new
    MPIN_GET_G1_MULTIPLE(RNG, 0, &W, prHID, &T); // Also send T=w.ID to client, remember random w, DH component
#endif

    // Client Second Pass: Inputs Client secret SEC, x and y. Outputs -(x+y)*SEC
    if (MPIN_CLIENT_2(&X, &Y, &SEC) != 0)
    {
        printf("Error from Client side - Second Pass\n");
        return 1;
    }

    // Server Second phase. Inputs hashed client id, random Y, -(x+y)*SEC, xID and xCID and Server secret SST. E and F help kangaroos to find error.
    // If PIN error not required, set E and F = NULL
    rtn = MPIN_SERVER_2(date, pHID, pHTID, &Y, &SST, pxID, pxCID, &SEC, pE, pF, NULL);
#endif // SINGLE_PASS

    if (rtn != 0)
    {
        printf("Server says - Bad Pin.\n");
#ifdef PINERROR

        err = MPIN_KANGAROO(&E, &F);
        if (err) printf("(Client PIN is out by %d)\n", err);

#endif
        return 1;
    }
    else
    {
        printf("Server says - PIN is good! You really are ");
        OCT_output_string(&CLIENT_ID);
        printf(".\n");
    }

#ifdef FULL

    HASH_ALL(HASH_TYPE_BLS12383, &HCID, pxID, pxCID, &SEC, &Y, &Z, &T, &H); // new
    MPIN_CLIENT_KEY(HASH_TYPE_BLS12383, &G1, &G2, pin, &R, &X, &H, &T, &CK); // new H
    printf("Client Key = ");
    OCT_output(&CK);

    HASH_ALL(HASH_TYPE_BLS12383, &HSID, pxID, pxCID, &SEC, &Y, &Z, &T, &H);
    MPIN_SERVER_KEY(HASH_TYPE_BLS12383, &Z, &SST, &W, &H, pHID, pxID, pxCID, &SK); // new H,pHID
    printf("Server Key = ");
    OCT_output(&SK);
#endif
    return 1;
}

int mpin192_BLS24479(csprng *RNG)
{

    using namespace BLS24479;

    int i, pin, rtn, err;
#ifdef PERMITS
    int date = today();
#else
    int date = 0;
#endif
    unsigned long ran;
    char x[PGS_BLS24479], s[PGS_BLS24479], y[PGS_BLS24479], client_id[100], sst[8 * PFS_BLS24479 + 1], token[2 * PFS_BLS24479 + 1], sec[2 * PFS_BLS24479 + 1], permit[2 * PFS_BLS24479 + 1], xcid[2 * PFS_BLS24479 + 1], xid[2 * PFS_BLS24479 + 1], e[24 * PFS_BLS24479], f[24 * PFS_BLS24479];
    char hcid[PFS_BLS24479], hsid[PFS_BLS24479], hid[2 * PFS_BLS24479 + 1], htid[2 * PFS_BLS24479 + 1], h[PGS_BLS24479];
#ifdef FULL
    char r[PGS_BLS24479], z[2 * PFS_BLS24479 + 1], w[PGS_BLS24479], t[2 * PFS_BLS24479 + 1];
    char g1[24 * PFS_BLS24479], g2[24 * PFS_BLS24479];
    char ck[AESKEY_BLS24479], sk[AESKEY_BLS24479];
#endif
    octet S = {0, sizeof(s), s};
    octet X = {0, sizeof(x), x};
    octet Y = {0, sizeof(y), y};
    octet H = {0, sizeof(h), h};
    octet CLIENT_ID = {0, sizeof(client_id), client_id};
    octet SST = {0, sizeof(sst), sst};
    octet TOKEN = {0, sizeof(token), token};
    octet SEC = {0, sizeof(sec), sec};
    octet PERMIT = {0, sizeof(permit), permit};
    octet xCID = {0, sizeof(xcid), xcid};
    octet xID = {0, sizeof(xid), xid};
    octet HCID = {0, sizeof(hcid), hcid};
    octet HSID = {0, sizeof(hsid), hsid};
    octet HID = {0, sizeof(hid), hid};
    octet HTID = {0, sizeof(htid), htid};
    octet E = {0, sizeof(e), e};
    octet F = {0, sizeof(f), f};
#ifdef FULL
    octet R = {0, sizeof(r), r};
    octet Z = {0, sizeof(z), z};
    octet W = {0, sizeof(w), w};
    octet T = {0, sizeof(t), t};
    octet G1 = {0, sizeof(g1), g1};
    octet G2 = {0, sizeof(g2), g2};
    octet SK = {0, sizeof(sk), sk};
    octet CK = {0, sizeof(ck), ck};
#endif
    octet *pxID, *pxCID, *pHID, *pHTID, *pE, *pF, *pPERMIT, *prHID;
    char idhex[100];

    // Trusted Authority set-up
    MPIN_RANDOM_GENERATE(RNG, &S);
    printf("Master Secret= ");
    OCT_output(&S);

    // Create Client Identity
    OCT_jstring(&CLIENT_ID, (char *)"testUser@miracl.com");
    HASH_ID(HASH_TYPE_BLS24479, &CLIENT_ID, &HCID); // Either Client or TA calculates Hash(ID) - you decide!

    printf("Client ID Hash= ");
    OCT_output(&HCID);
    printf("\n");

    OCT_toHex(&CLIENT_ID, idhex);
    printf("Client ID= %s\n", idhex); // OCT_toHex(&CLIENT_ID); printf("\n");

    MPIN_GET_CLIENT_SECRET(&S, &HCID, &TOKEN);
    printf("Client Secret= ");
    OCT_output(&TOKEN);

// Client and Server are issued secrets by DTA
    MPIN_GET_SERVER_SECRET(&S, &SST);
    printf("Server Secret= ");
    OCT_output(&SST);

    // Client extracts PIN from secret to create Token
    pin = 1234;
    printf("Client extracts PIN= %d\n", pin);
    MPIN_EXTRACT_PIN(HASH_TYPE_BLS24479, &CLIENT_ID, pin, &TOKEN);
    printf("Client Token= ");
    OCT_output(&TOKEN);

#ifdef FULL
    MPIN_PRECOMPUTE(&TOKEN, &HCID, NULL, &G1, &G2);
#endif

#ifdef PERMITS
    // Client gets "Time Permit" from DTA
    printf("Client gets Time Permit\n");

    MPIN_GET_CLIENT_PERMIT(HASH_TYPE_BLS24479, date, &S, &HCID, &PERMIT);
    printf("Time Permit= ");
    OCT_output(&PERMIT);

    // This encoding makes Time permit look random
    if (MPIN_ENCODING(RNG, &PERMIT) != 0) printf("Encoding error\n");
    // printf("Encoded Time Permit= "); OCT_output(&PERMIT);
    if (MPIN_DECODING(&PERMIT) != 0) printf("Decoding error\n");
    // printf("Decoded Time Permit= "); OCT_output(&PERMIT);
#endif

    // MPin Protocol

    // Client enters PIN
    printf("\nPIN= ");
    if (scanf("%d", &pin)) {};
    // to avoid silly compile error
    getchar();

    // Set date=0 and PERMIT=NULL if time permits not in use

    // Client First pass: Inputs CLIENT_ID, optional RNG, pin, TOKEN and PERMIT. Output xID = x.H(CLIENT_ID) and re-combined secret SEC
    // If PERMITS are is use, then date!=0 and PERMIT is added to secret and xCID = x.(H(CLIENT_ID)+H(date|H(CLIENT_ID)))
    // Random value x is supplied externally if RNG=NULL, otherwise generated and passed out by RNG

    // HSID - hashed client ID as calculated by the server
    // HCID - hashed client ID as calculated by the client

    // IMPORTANT: To save space and time..
    // If Time Permits OFF set xCID = NULL, HTID=NULL and use xID and HID only
    // If Time permits are ON, AND pin error detection is required then all of xID, xCID, HID and HTID are required
    // If Time permits are ON, AND pin error detection is NOT required, set xID=NULL, HID=NULL and use xCID and HTID only.



    pxID = &xID;
    pxCID = &xCID;
    pHID = &HID;
    pHTID = &HTID;
    pE = &E;
    pF = &F;
    pPERMIT = &PERMIT;

#ifdef PERMITS
    prHID = pHTID;
#ifndef PINERROR
    pxID = NULL;
//   pHID=NULL;  //new
#endif
#else
    prHID = pHID;
    pPERMIT = NULL;
    pxCID = NULL;
    pHTID = NULL;
#endif
#ifndef PINERROR
    pE = NULL;
    pF = NULL;
#endif

    // When set only send hashed IDs to server
    octet *pID;
#ifdef USE_ANONYMOUS
    pID = &HCID;
#else
    pID = &CLIENT_ID;
#endif

#ifdef SINGLE_PASS
    int timeValue;
    printf("MPIN Single Pass\n");
    timeValue = MPIN_GET_TIME();

    rtn = MPIN_CLIENT(HASH_TYPE_BLS24479, date, &CLIENT_ID, RNG, &X, pin, &TOKEN, &SEC, pxID, pxCID, pPERMIT, NULL, timeValue, &Y);

    if (rtn != 0)
    {
        printf("MPIN_CLIENT ERROR %d\n", rtn);
        return 1;
    }

#ifdef FULL
    MPIN_GET_G1_MULTIPLE(RNG, 1, &R, &HCID, &Z); // Also Send Z=r.ID to Server, remember random r
#endif


    rtn = MPIN_SERVER(HASH_TYPE_BLS24479, date, pHID, pHTID, &Y, &SST, pxID, pxCID, &SEC, pE, pF, pID, NULL, timeValue, NULL);

#ifdef FULL
    HASH_ID(HASH_TYPE_BLS24479, &CLIENT_ID, &HSID); // new
    MPIN_GET_G1_MULTIPLE(RNG, 0, &W, prHID, &T); // Also send T=w.ID to client, remember random w
#endif

#else // SINGLE_PASS
    printf("MPIN Multi Pass\n");
    if (MPIN_CLIENT_1(HASH_TYPE_BLS24479, date, &CLIENT_ID, RNG, &X, pin, &TOKEN, &SEC, pxID, pxCID, pPERMIT) != 0)
    {
        printf("Error from Client side - First Pass\n");
        return 0;
    }

    // Send U=x.ID to server, and recreate secret from token and pin

#ifdef FULL
    HASH_ID(HASH_TYPE_BLS24479, &CLIENT_ID, &HCID);
    MPIN_GET_G1_MULTIPLE(RNG, 1, &R, &HCID, &Z); // Also Send Z=r.ID to Server, remember random r, DH component
#endif

    // Server calculates H(ID) and H(ID)+H(T|H(ID)) (if time permits enabled), and maps them to points on the curve HID and HTID resp.
    MPIN_SERVER_1(HASH_TYPE_BLS24479, date, pID, pHID, pHTID);

    // Server generates Random number Y and sends it to Client
    MPIN_RANDOM_GENERATE(RNG, &Y);

#ifdef FULL
    HASH_ID(HASH_TYPE_BLS24479, &CLIENT_ID, &HSID); //new
    MPIN_GET_G1_MULTIPLE(RNG, 0, &W, prHID, &T); // Also send T=w.ID to client, remember random w, DH component
#endif

    // Client Second Pass: Inputs Client secret SEC, x and y. Outputs -(x+y)*SEC
    if (MPIN_CLIENT_2(&X, &Y, &SEC) != 0)
    {
        printf("Error from Client side - Second Pass\n");
        return 1;
    }

    // Server Second phase. Inputs hashed client id, random Y, -(x+y)*SEC, xID and xCID and Server secret SST. E and F help kangaroos to find error.
    // If PIN error not required, set E and F = NULL
    rtn = MPIN_SERVER_2(date, pHID, pHTID, &Y, &SST, pxID, pxCID, &SEC, pE, pF, NULL);
#endif // SINGLE_PASS

    if (rtn != 0)
    {
        printf("Server says - Bad Pin.\n");
#ifdef PINERROR

        err = MPIN_KANGAROO(&E, &F);
        if (err) printf("(Client PIN is out by %d)\n", err);

#endif
        return 1;
    }
    else
    {
        printf("Server says - PIN is good! You really are ");
        OCT_output_string(&CLIENT_ID);
        printf(".\n");
    }

#ifdef FULL

    HASH_ALL(HASH_TYPE_BLS24479, &HCID, pxID, pxCID, &SEC, &Y, &Z, &T, &H); // new
    i = MPIN_CLIENT_KEY(HASH_TYPE_BLS24479, &G1, &G2, pin, &R, &X, &H, &T, &CK); // new H
    printf("Client Key = ");
    OCT_output(&CK);

    HASH_ALL(HASH_TYPE_BLS24479, &HSID, pxID, pxCID, &SEC, &Y, &Z, &T, &H);
    i = MPIN_SERVER_KEY(HASH_TYPE_BLS24479, &Z, &SST, &W, &H, pHID, pxID, pxCID, &SK); // new H,pHID
    printf("Server Key = ");
    OCT_output(&SK);
#endif
    return 1;
}

int mpin256_BLS48556(csprng *RNG)
{

    using namespace BLS48556;

    int i, pin, rtn, err;
#ifdef PERMITS
    int date = today();
#else
    int date = 0;
#endif
    unsigned long ran;
    char x[PGS_BLS48556], s[PGS_BLS48556], y[PGS_BLS48556], client_id[100], sst[16 * PFS_BLS48556 + 1], token[2 * PFS_BLS48556 + 1], sec[2 * PFS_BLS48556 + 1], permit[2 * PFS_BLS48556 + 1], xcid[2 * PFS_BLS48556 + 1], xid[2 * PFS_BLS48556 + 1], e[48 * PFS_BLS48556], f[48 * PFS_BLS48556];
    char hcid[PFS_BLS48556], hsid[PFS_BLS48556], hid[2 * PFS_BLS48556 + 1], htid[2 * PFS_BLS48556 + 1], h[PGS_BLS48556];
#ifdef FULL
    char r[PGS_BLS48556], z[2 * PFS_BLS48556 + 1], w[PGS_BLS48556], t[2 * PFS_BLS48556 + 1];
    char g1[48 * PFS_BLS48556], g2[48 * PFS_BLS48556];
    char ck[AESKEY_BLS48556], sk[AESKEY_BLS48556];
#endif
    octet S = {0, sizeof(s), s};
    octet X = {0, sizeof(x), x};
    octet Y = {0, sizeof(y), y};
    octet H = {0, sizeof(h), h};
    octet CLIENT_ID = {0, sizeof(client_id), client_id};
    octet SST = {0, sizeof(sst), sst};
    octet TOKEN = {0, sizeof(token), token};
    octet SEC = {0, sizeof(sec), sec};
    octet PERMIT = {0, sizeof(permit), permit};
    octet xCID = {0, sizeof(xcid), xcid};
    octet xID = {0, sizeof(xid), xid};
    octet HCID = {0, sizeof(hcid), hcid};
    octet HSID = {0, sizeof(hsid), hsid};
    octet HID = {0, sizeof(hid), hid};
    octet HTID = {0, sizeof(htid), htid};
    octet E = {0, sizeof(e), e};
    octet F = {0, sizeof(f), f};
#ifdef FULL
    octet R = {0, sizeof(r), r};
    octet Z = {0, sizeof(z), z};
    octet W = {0, sizeof(w), w};
    octet T = {0, sizeof(t), t};
    octet G1 = {0, sizeof(g1), g1};
    octet G2 = {0, sizeof(g2), g2};
    octet SK = {0, sizeof(sk), sk};
    octet CK = {0, sizeof(ck), ck};
#endif
    octet *pxID, *pxCID, *pHID, *pHTID, *pE, *pF, *pPERMIT, *prHID;
    char idhex[100];

    // Trusted Authority set-up
    MPIN_RANDOM_GENERATE(RNG, &S);
    printf("Master Secret= ");
    OCT_output(&S);

    // Create Client Identity
    OCT_jstring(&CLIENT_ID, (char *)"testUser@miracl.com");
    HASH_ID(HASH_TYPE_BLS48556, &CLIENT_ID, &HCID); // Either Client or TA calculates Hash(ID) - you decide!

    printf("Client ID Hash= ");
    OCT_output(&HCID);
    printf("\n");

    OCT_toHex(&CLIENT_ID, idhex);
    printf("Client ID= %s\n", idhex); // OCT_toHex(&CLIENT_ID); printf("\n");

    MPIN_GET_CLIENT_SECRET(&S, &HCID, &TOKEN);
    printf("Client Secret= ");
    OCT_output(&TOKEN);

// Client and Server are issued secrets by DTA
    MPIN_GET_SERVER_SECRET(&S, &SST);
    printf("Server Secret= ");
    OCT_output(&SST);

    // Client extracts PIN from secret to create Token
    pin = 1234;
    printf("Client extracts PIN= %d\n", pin);
    MPIN_EXTRACT_PIN(HASH_TYPE_BLS48556, &CLIENT_ID, pin, &TOKEN);
    printf("Client Token= ");
    OCT_output(&TOKEN);

#ifdef FULL
    MPIN_PRECOMPUTE(&TOKEN, &HCID, NULL, &G1, &G2);
#endif

#ifdef PERMITS
    // Client gets "Time Permit" from DTA
    printf("Client gets Time Permit\n");

    MPIN_GET_CLIENT_PERMIT(HASH_TYPE_BLS48556, date, &S, &HCID, &PERMIT);
    printf("Time Permit= ");
    OCT_output(&PERMIT);

    // This encoding makes Time permit look random
    if (MPIN_ENCODING(RNG, &PERMIT) != 0) printf("Encoding error\n");
    // printf("Encoded Time Permit= "); OCT_output(&PERMIT);
    if (MPIN_DECODING(&PERMIT) != 0) printf("Decoding error\n");
    // printf("Decoded Time Permit= "); OCT_output(&PERMIT);
#endif

    // MPin Protocol

    // Client enters PIN
    printf("\nPIN= ");
    if (scanf("%d", &pin)) {};
    // to avoid silly compile error
    getchar();

    // Set date=0 and PERMIT=NULL if time permits not in use

    // Client First pass: Inputs CLIENT_ID, optional RNG, pin, TOKEN and PERMIT. Output xID = x.H(CLIENT_ID) and re-combined secret SEC
    // If PERMITS are is use, then date!=0 and PERMIT is added to secret and xCID = x.(H(CLIENT_ID)+H(date|H(CLIENT_ID)))
    // Random value x is supplied externally if RNG=NULL, otherwise generated and passed out by RNG

    // HSID - hashed client ID as calculated by the server
    // HCID - hashed client ID as calculated by the client

    // IMPORTANT: To save space and time..
    // If Time Permits OFF set xCID = NULL, HTID=NULL and use xID and HID only
    // If Time permits are ON, AND pin error detection is required then all of xID, xCID, HID and HTID are required
    // If Time permits are ON, AND pin error detection is NOT required, set xID=NULL, HID=NULL and use xCID and HTID only.



    pxID = &xID;
    pxCID = &xCID;
    pHID = &HID;
    pHTID = &HTID;
    pE = &E;
    pF = &F;
    pPERMIT = &PERMIT;

#ifdef PERMITS
    prHID = pHTID;
#ifndef PINERROR
    pxID = NULL;
//   pHID=NULL;  //new
#endif
#else
    prHID = pHID;
    pPERMIT = NULL;
    pxCID = NULL;
    pHTID = NULL;
#endif
#ifndef PINERROR
    pE = NULL;
    pF = NULL;
#endif

    // When set only send hashed IDs to server
    octet *pID;
#ifdef USE_ANONYMOUS
    pID = &HCID;
#else
    pID = &CLIENT_ID;
#endif

#ifdef SINGLE_PASS
    int timeValue;
    printf("MPIN Single Pass\n");
    timeValue = MPIN_GET_TIME();

    rtn = MPIN_CLIENT(HASH_TYPE_BLS48556, date, &CLIENT_ID, RNG, &X, pin, &TOKEN, &SEC, pxID, pxCID, pPERMIT, NULL, timeValue, &Y);

    if (rtn != 0)
    {
        printf("MPIN_CLIENT ERROR %d\n", rtn);
        return 1;
    }

#ifdef FULL
    MPIN_GET_G1_MULTIPLE(RNG, 1, &R, &HCID, &Z); // Also Send Z=r.ID to Server, remember random r
#endif


    rtn = MPIN_SERVER(HASH_TYPE_BLS48556, date, pHID, pHTID, &Y, &SST, pxID, pxCID, &SEC, pE, pF, pID, NULL, timeValue, NULL);

#ifdef FULL
    HASH_ID(HASH_TYPE_BLS48556, &CLIENT_ID, &HSID); // new
    MPIN_GET_G1_MULTIPLE(RNG, 0, &W, prHID, &T); // Also send T=w.ID to client, remember random w
#endif

#else // SINGLE_PASS
    printf("MPIN Multi Pass\n");
    if (MPIN_CLIENT_1(HASH_TYPE_BLS48556, date, &CLIENT_ID, RNG, &X, pin, &TOKEN, &SEC, pxID, pxCID, pPERMIT) != 0)
    {
        printf("Error from Client side - First Pass\n");
        return 0;
    }

    // Send U=x.ID to server, and recreate secret from token and pin

#ifdef FULL
    HASH_ID(HASH_TYPE_BLS48556, &CLIENT_ID, &HCID);
    MPIN_GET_G1_MULTIPLE(RNG, 1, &R, &HCID, &Z); // Also Send Z=r.ID to Server, remember random r, DH component
#endif

    // Server calculates H(ID) and H(ID)+H(T|H(ID)) (if time permits enabled), and maps them to points on the curve HID and HTID resp.
    MPIN_SERVER_1(HASH_TYPE_BLS48556, date, pID, pHID, pHTID);

    // Server generates Random number Y and sends it to Client
    MPIN_RANDOM_GENERATE(RNG, &Y);

#ifdef FULL
    HASH_ID(HASH_TYPE_BLS48556, &CLIENT_ID, &HSID); //new
    MPIN_GET_G1_MULTIPLE(RNG, 0, &W, prHID, &T); // Also send T=w.ID to client, remember random w, DH component
#endif

    // Client Second Pass: Inputs Client secret SEC, x and y. Outputs -(x+y)*SEC
    if (MPIN_CLIENT_2(&X, &Y, &SEC) != 0)
    {
        printf("Error from Client side - Second Pass\n");
        return 1;
    }

    // Server Second phase. Inputs hashed client id, random Y, -(x+y)*SEC, xID and xCID and Server secret SST. E and F help kangaroos to find error.
    // If PIN error not required, set E and F = NULL
    rtn = MPIN_SERVER_2(date, pHID, pHTID, &Y, &SST, pxID, pxCID, &SEC, pE, pF, NULL);
#endif // SINGLE_PASS

    if (rtn != 0)
    {
        printf("Server says - Bad Pin.\n");
#ifdef PINERROR

        err = MPIN_KANGAROO(&E, &F);
        if (err) printf("(Client PIN is out by %d)\n", err);

#endif
        return 1;
    }
    else
    {
        printf("Server says - PIN is good! You really are ");
        OCT_output_string(&CLIENT_ID);
        printf(".\n");
    }

#ifdef FULL

    HASH_ALL(HASH_TYPE_BLS48556, &HCID, pxID, pxCID, &SEC, &Y, &Z, &T, &H); // new
    i = MPIN_CLIENT_KEY(HASH_TYPE_BLS48556, &G1, &G2, pin, &R, &X, &H, &T, &CK); // new H
    printf("Client Key = ");
    OCT_output(&CK);

    HASH_ALL(HASH_TYPE_BLS48556, &HSID, pxID, pxCID, &SEC, &Y, &Z, &T, &H);
    i = MPIN_SERVER_KEY(HASH_TYPE_BLS48556, &Z, &SST, &W, &H, pHID, pxID, pxCID, &SK); // new H,pHID
    printf("Server Key = ");
    OCT_output(&SK);
#endif
    return 1;
}
#endif

int main()
{
    int i, res;
    unsigned long ran;

    char raw[100];
    octet RAW = {0, sizeof(raw), raw};
    csprng RNG;                // Crypto Strong RNG

    time((time_t *)&ran);

    RAW.len = 100;              // fake random seed source
    RAW.val[0] = ran;
    RAW.val[1] = ran >> 8;
    RAW.val[2] = ran >> 16;
    RAW.val[3] = ran >> 24;
    for (i = 4; i < 100; i++) RAW.val[i] = i;

    CREATE_CSPRNG(&RNG, &RAW);  // initialise strong RNG

    printf("\nTesting MPIN protocols for curve BN254\n");
    mpin_BN254(&RNG);

#if CHUNK!=16
    printf("\nTesting MPIN protocols for curve BLS12383\n");
    mpin_BLS12383(&RNG);
    printf("\nTesting MPIN protocols for curve BLS24479\n");
    mpin192_BLS24479(&RNG);
    printf("\nTesting MPIN protocols for curve BLS48556\n");
    mpin256_BLS48556(&RNG);
#endif

    KILL_CSPRNG(&RNG);
}

