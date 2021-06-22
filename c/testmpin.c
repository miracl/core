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

int mpin_BN254(csprng *RNG)
{
    int pin, rtn;
    char x[PGS_BN254], s[PGS_BN254], y[PGS_BN254], client_id[100], sst[4 * PFS_BN254 + 1], token[2 * PFS_BN254 + 1], sec[2 * PFS_BN254 + 1], u[2 * PFS_BN254 + 1];
    char hcid[2*PFS_BN254+1], hsid[2*PFS_BN254+1], dst[256];
    
    char s1[2 * PFS_BN254 + 1],s2[2 * PFS_BN254 + 1],s3[2 * PFS_BN254 + 1],s4[2 * PFS_BN254 + 1],r[128];
    octet S1 = {0, sizeof(s1), s1};
    octet S2 = {0, sizeof(s2), s2};
    octet S3 = {0, sizeof(s3), s3};
    octet S4 = {0, sizeof(s4), s4};
    octet R = {0,sizeof(r),r};
    
    octet S = {0, sizeof(s), s};
    octet X = {0, sizeof(x), x};
    octet Y = {0, sizeof(y), y};
    octet CLIENT_ID = {0, sizeof(client_id), client_id};
    octet SST = {0, sizeof(sst), sst};
    octet TOKEN = {0, sizeof(token), token};
    octet SEC = {0, sizeof(sec), sec};
    octet U = {0, sizeof(u), u};
    octet HCID = {0, sizeof(hcid), hcid};
    octet HSID = {0, sizeof(hsid), hsid};
    octet DST = {0, sizeof(dst), dst};


    OCT_jstring(&DST,(char *)"BN254G1_XMD:SHA-256_SVDW_NU_MPIN"); // Domain Separation Tag

    // Trusted Authority (TA) set-up
    MPIN_BN254_RANDOM_GENERATE(RNG, &S);
    printf("Master Secret= ");
    OCT_output(&S);

// Client Identity
            OCT_jstring(&CLIENT_ID, (char *)"testUser@miracl.com");
            MPIN_BN254_ENCODE_TO_CURVE(&DST,&CLIENT_ID,&HCID);
            printf("Client ID Hashed to Curve= ");
            OCT_output(&HCID);
//            printf("\n");

// Client approaches Trusted Authority and is issued secret

    MPIN_BN254_GET_CLIENT_SECRET(&S, &HCID, &TOKEN);
    printf("Client Secret= ");
    OCT_output(&TOKEN);

// TA sends Client secret to Client

// Server is issued secret by TA
    MPIN_BN254_GET_SERVER_SECRET(&S, &SST);
//    printf("Server Secret= ");
//    OCT_output(&SST);

// TA sends Server secret to Server

// Client extracts PIN from secret to create Token
            //pin = 1234;
            //printf("Client extracts PIN= %d\n", pin);
            pin=0; printf("Client chooses PIN= ");
            if (scanf("%d", &pin)) {}; // to avoid silly compile error
            getchar();

            MPIN_BN254_EXTRACT_PIN(&HCID, pin, &TOKEN);
            printf("Client Token= ");
            OCT_output(&TOKEN);

// Exercise Secret Sharing

            OCT_rand(&R,RNG,128);

        // create 4 unique shares of TOKEN
            share Sh1=getshare(1,3,&S1,&TOKEN,&R);  // indicate 3 shares required for recovery
            share Sh2=getshare(2,3,&S2,&TOKEN,&R);
            share Sh3=getshare(3,3,&S3,&TOKEN,&R);
            share Sh4=getshare(4,3,&S4,&TOKEN,&R);

            OCT_clear(&TOKEN);  // kill token

            share Shares[3];
            Shares[0]=Sh1;  // any 3 shares to recover TOKEN
            Shares[1]=Sh2;
            Shares[2]=Sh4; 
            recover(&TOKEN,Shares);  // recover token


// MPin Protocol

// Client enters ID and PIN
            OCT_clear(&CLIENT_ID);
            OCT_jstring(&CLIENT_ID, (char *)"testUser@miracl.com");
            MPIN_BN254_ENCODE_TO_CURVE(&DST,&CLIENT_ID,&HCID);
            pin=0; printf("Client enters PIN= ");
            if (scanf("%d", &pin)) {}; // to avoid silly compile error
            getchar();

// Client First pass: Inputs H(CLIENT_ID), RNG, pin, and TOKEN. Output x and U = x.H(CLIENT_ID) and re-combined secret SEC

            if (MPIN_BN254_CLIENT_1(&HCID, RNG, &X, pin, &TOKEN, &SEC, &U) != 0)
            {
                printf("Error from Client side - First Pass\n");
                return 1;
            }

// Send CLIENT_ID and U=x.ID to server. Server hashes ID to curve.
                    MPIN_BN254_ENCODE_TO_CURVE(&DST,&CLIENT_ID,&HSID);

// Server generates Random number Y and sends it to Client
                    MPIN_BN254_RANDOM_GENERATE(RNG, &Y);

// Client Second Pass: Inputs Client secret SEC, x and y. Sends -(x+y)*SEC to Server
            if (MPIN_BN254_CLIENT_2(&X, &Y, &SEC) != 0)
            {
                printf("Error from Client side - Second Pass\n");
                return 1;
            }

// Server Second phase. Inputs H(CLIENT_ID), Y, -(x+y)*SEC, U and Server secret SST. 

                    rtn = MPIN_BN254_SERVER(&HSID, &Y, &SST, &U, &SEC);
                    if (rtn != 0)
                    {
                        printf("Server says - Bad Pin.\n");
                        return 1;
                    }
                    else
                    {
                        printf("Server says - PIN is good! You really are ");
                        OCT_output_string(&CLIENT_ID);
                        printf(".\n");
                    }

    return 0;
}

#if CHUNK==32 || CHUNK==64

int mpin_BLS12383(csprng *RNG)
{
    int pin, rtn;

    char x[PGS_BLS12383], s[PGS_BLS12383], y[PGS_BLS12383], client_id[100], sst[4 * PFS_BLS12383 + 1], token[2 * PFS_BLS12383 + 1], sec[2 * PFS_BLS12383 + 1], u[2 * PFS_BLS12383 + 1];
    char hcid[2*PFS_BLS12383+1], hsid[2*PFS_BLS12383+1], dst[256];

    octet S = {0, sizeof(s), s};
    octet X = {0, sizeof(x), x};
    octet Y = {0, sizeof(y), y};
    octet CLIENT_ID = {0, sizeof(client_id), client_id};
    octet SST = {0, sizeof(sst), sst};
    octet TOKEN = {0, sizeof(token), token};
    octet SEC = {0, sizeof(sec), sec};
    octet U = {0, sizeof(u), u};
    octet HCID = {0, sizeof(hcid), hcid};
    octet HSID = {0, sizeof(hsid), hsid};
    octet DST = {0, sizeof(dst), dst};
    OCT_jstring(&DST,(char *)"BLS12383G1_XMD:SHA-256_SVDW_NU_MPIN"); // Domain Separation Tag

    // Trusted Authority (TA) set-up
    MPIN_BLS12383_RANDOM_GENERATE(RNG, &S);
    printf("Master Secret= ");
    OCT_output(&S);

// Client Identity
            OCT_jstring(&CLIENT_ID, (char *)"testUser@miracl.com");
            MPIN_BLS12383_ENCODE_TO_CURVE(&DST,&CLIENT_ID,&HCID);
            printf("Client ID Hashed to Curve= ");
            OCT_output(&HCID);
//            printf("\n");

// Client approaches Trusted Authority and is issued secret

    MPIN_BLS12383_GET_CLIENT_SECRET(&S, &HCID, &TOKEN);
    printf("Client Secret= ");
    OCT_output(&TOKEN);

// TA sends Client secret to Client

// Server is issued secret by TA
    MPIN_BLS12383_GET_SERVER_SECRET(&S, &SST);
//    printf("Server Secret= ");
//    OCT_output(&SST);

// TA sends Server secret to Server

// Client extracts PIN from secret to create Token
            //pin = 1234;
            //printf("Client extracts PIN= %d\n", pin);
            pin=0; printf("Client chooses PIN= ");
            if (scanf("%d", &pin)) {}; // to avoid silly compile error
            getchar();
            MPIN_BLS12383_EXTRACT_PIN(&HCID, pin, &TOKEN);
            printf("Client Token= ");
            OCT_output(&TOKEN);

// MPin Protocol

// Client enters ID and PIN
            OCT_clear(&CLIENT_ID);
            OCT_jstring(&CLIENT_ID, (char *)"testUser@miracl.com");
            MPIN_BLS12383_ENCODE_TO_CURVE(&DST,&CLIENT_ID,&HCID);
            pin=0; printf("Client enters PIN= ");
            if (scanf("%d", &pin)) {}; // to avoid silly compile error
            getchar();

// Client First pass: Inputs H(CLIENT_ID), RNG, pin, and TOKEN. Output x and U = x.H(CLIENT_ID) and re-combined secret SEC

            if (MPIN_BLS12383_CLIENT_1(&HCID, RNG, &X, pin, &TOKEN, &SEC, &U) != 0)
            {
                printf("Error from Client side - First Pass\n");
                return 1;
            }

// Send CLIENT_ID and U=x.ID to server. Server hashes ID to curve.
                    MPIN_BLS12383_ENCODE_TO_CURVE(&DST,&CLIENT_ID,&HSID);

// Server generates Random number Y and sends it to Client
                    MPIN_BLS12383_RANDOM_GENERATE(RNG, &Y);

// Client Second Pass: Inputs Client secret SEC, x and y. Sends -(x+y)*SEC to Server
            if (MPIN_BLS12383_CLIENT_2(&X, &Y, &SEC) != 0)
            {
                printf("Error from Client side - Second Pass\n");
                return 1;
            }

// Server Second phase. Inputs H(CLIENT_ID), Y, -(x+y)*SEC, U and Server secret SST. 

                    rtn = MPIN_BLS12383_SERVER(&HSID, &Y, &SST, &U, &SEC);
                    if (rtn != 0)
                    {
                        printf("Server says - Bad Pin.\n");
                        return 1;
                    }
                    else
                    {
                        printf("Server says - PIN is good! You really are ");
                        OCT_output_string(&CLIENT_ID);
                        printf(".\n");
                    }
    printf("\n");
    return 0;
}

int mpin192_BLS24479(csprng *RNG)
{
    int pin, rtn;
    char x[PGS_BLS24479], s[PGS_BLS24479], y[PGS_BLS24479], client_id[100], sst[8 * PFS_BLS24479 + 1], token[2 * PFS_BLS24479 + 1], sec[2 * PFS_BLS24479 + 1], u[2 * PFS_BLS24479 + 1];
    char hcid[2*PFS_BLS24479+1], hsid[2*PFS_BLS24479+1], dst[256];

    octet S = {0, sizeof(s), s};
    octet X = {0, sizeof(x), x};
    octet Y = {0, sizeof(y), y};
    octet CLIENT_ID = {0, sizeof(client_id), client_id};
    octet SST = {0, sizeof(sst), sst};
    octet TOKEN = {0, sizeof(token), token};
    octet SEC = {0, sizeof(sec), sec};
    octet U = {0, sizeof(u), u};
    octet HCID = {0, sizeof(hcid), hcid};
    octet HSID = {0, sizeof(hsid), hsid};
    octet DST = {0, sizeof(dst), dst};
    OCT_jstring(&DST,(char *)"BLS24479G1_XMD:SHA-384_SVDW_NU_MPIN"); // Domain Separation Tag


    // Trusted Authority (TA) set-up
    MPIN_BLS24479_RANDOM_GENERATE(RNG, &S);
    printf("Master Secret= ");
    OCT_output(&S);

// Client Identity
            OCT_jstring(&CLIENT_ID, (char *)"testUser@miracl.com");
            MPIN_BLS24479_ENCODE_TO_CURVE(&DST,&CLIENT_ID,&HCID);
            printf("Client ID Hashed to Curve= ");
            OCT_output(&HCID);
//            printf("\n");

// Client approaches Trusted Authority and is issued secret

    MPIN_BLS24479_GET_CLIENT_SECRET(&S, &HCID, &TOKEN);
    printf("Client Secret= ");
    OCT_output(&TOKEN);

// TA sends Client secret to Client

// Server is issued secret by TA
    MPIN_BLS24479_GET_SERVER_SECRET(&S, &SST);
//    printf("Server Secret= ");
//    OCT_output(&SST);

// TA sends Server secret to Server

// Client extracts PIN from secret to create Token
            //pin = 1234;
            //printf("Client extracts PIN= %d\n", pin);
            pin=0; printf("Client chooses PIN= ");
            if (scanf("%d", &pin)) {}; // to avoid silly compile error
            getchar();
            MPIN_BLS24479_EXTRACT_PIN(&HCID, pin, &TOKEN);
            printf("Client Token= ");
            OCT_output(&TOKEN);

// MPin Protocol

// Client enters ID and PIN
            OCT_clear(&CLIENT_ID);
            OCT_jstring(&CLIENT_ID, (char *)"testUser@miracl.com");
            MPIN_BLS24479_ENCODE_TO_CURVE(&DST,&CLIENT_ID,&HCID);
            pin=0; printf("Client enters PIN= ");
            if (scanf("%d", &pin)) {}; // to avoid silly compile error
            getchar();

// Client First pass: Inputs H(CLIENT_ID), RNG, pin, and TOKEN. Output x and U = x.H(CLIENT_ID) and re-combined secret SEC

            if (MPIN_BLS24479_CLIENT_1(&HCID, RNG, &X, pin, &TOKEN, &SEC, &U) != 0)
            {
                printf("Error from Client side - First Pass\n");
                return 1;
            }

// Send CLIENT_ID and U=x.ID to server. Server hashes ID to curve.
                    MPIN_BLS24479_ENCODE_TO_CURVE(&DST,&CLIENT_ID,&HSID);

// Server generates Random number Y and sends it to Client
                    MPIN_BLS24479_RANDOM_GENERATE(RNG, &Y);

// Client Second Pass: Inputs Client secret SEC, x and y. Sends -(x+y)*SEC to Server
            if (MPIN_BLS24479_CLIENT_2(&X, &Y, &SEC) != 0)
            {
                printf("Error from Client side - Second Pass\n");
                return 1;
            }

// Server Second phase. Inputs H(CLIENT_ID), Y, -(x+y)*SEC, U and Server secret SST. 

                    rtn = MPIN_BLS24479_SERVER(&HSID, &Y, &SST, &U, &SEC);
                    if (rtn != 0)
                    {
                        printf("Server says - Bad Pin.\n");
                        return 1;
                    }
                    else
                    {
                        printf("Server says - PIN is good! You really are ");
                        OCT_output_string(&CLIENT_ID);
                        printf(".\n");
                    }
    printf("\n");
    return 0;
}

int mpin256_BLS48556(csprng *RNG)
{
    int pin, rtn;
    char x[PGS_BLS48556], s[PGS_BLS48556], y[PGS_BLS48556], client_id[100], sst[16 * PFS_BLS48556 + 1], token[2 * PFS_BLS48556 + 1], sec[2 * PFS_BLS48556 + 1], u[2 * PFS_BLS48556 + 1];
    char hcid[2*PFS_BLS48556+1], hsid[2*PFS_BLS48556+1], dst[256];

    octet S = {0, sizeof(s), s};
    octet X = {0, sizeof(x), x};
    octet Y = {0, sizeof(y), y};
    octet CLIENT_ID = {0, sizeof(client_id), client_id};
    octet SST = {0, sizeof(sst), sst};
    octet TOKEN = {0, sizeof(token), token};
    octet SEC = {0, sizeof(sec), sec};
    octet U = {0, sizeof(u), u};
    octet HCID = {0, sizeof(hcid), hcid};
    octet HSID = {0, sizeof(hsid), hsid};
    octet DST = {0, sizeof(dst), dst};
    OCT_jstring(&DST,(char *)"BLS48556G1_XMD:SHA-512_SVDW_NU_MPIN"); // Domain Separation Tag


    // Trusted Authority (TA) set-up
    MPIN_BLS48556_RANDOM_GENERATE(RNG, &S);
    printf("Master Secret= ");
    OCT_output(&S);

// Client Identity
            OCT_jstring(&CLIENT_ID, (char *)"testUser@miracl.com");
            MPIN_BLS48556_ENCODE_TO_CURVE(&DST,&CLIENT_ID,&HCID);
            printf("Client ID Hashed to Curve= ");
            OCT_output(&HCID);
//            printf("\n");

// Client approaches Trusted Authority and is issued secret

    MPIN_BLS48556_GET_CLIENT_SECRET(&S, &HCID, &TOKEN);
    printf("Client Secret= ");
    OCT_output(&TOKEN);

// TA sends Client secret to Client

// Server is issued secret by TA
    MPIN_BLS48556_GET_SERVER_SECRET(&S, &SST);
//    printf("Server Secret= ");
//    OCT_output(&SST);

// TA sends Server secret to Server

// Client extracts PIN from secret to create Token
            //pin = 1234;
            //printf("Client extracts PIN= %d\n", pin);
            pin=0; printf("Client chooses PIN= ");
            if (scanf("%d", &pin)) {}; // to avoid silly compile error
            getchar();
            MPIN_BLS48556_EXTRACT_PIN(&HCID, pin, &TOKEN);
            printf("Client Token= ");
            OCT_output(&TOKEN);

// MPin Protocol

// Client enters ID and PIN
            OCT_clear(&CLIENT_ID);
            OCT_jstring(&CLIENT_ID, (char *)"testUser@miracl.com");
            MPIN_BLS48556_ENCODE_TO_CURVE(&DST,&CLIENT_ID,&HCID);
            pin=0; printf("Client enters PIN= ");
            if (scanf("%d", &pin)) {}; // to avoid silly compile error
            getchar();

// Client First pass: Inputs H(CLIENT_ID), RNG, pin, and TOKEN. Output x and U = x.H(CLIENT_ID) and re-combined secret SEC

            if (MPIN_BLS48556_CLIENT_1(&HCID, RNG, &X, pin, &TOKEN, &SEC, &U) != 0)
            {
                printf("Error from Client side - First Pass\n");
                return 1;
            }

// Send CLIENT_ID and U=x.ID to server. Server hashes ID to curve.
                    MPIN_BLS48556_ENCODE_TO_CURVE(&DST,&CLIENT_ID,&HSID);

// Server generates Random number Y and sends it to Client
                    MPIN_BLS48556_RANDOM_GENERATE(RNG, &Y);

// Client Second Pass: Inputs Client secret SEC, x and y. Sends -(x+y)*SEC to Server
            if (MPIN_BLS48556_CLIENT_2(&X, &Y, &SEC) != 0)
            {
                printf("Error from Client side - Second Pass\n");
                return 1;
            }

// Server Second phase. Inputs H(CLIENT_ID), Y, -(x+y)*SEC, U and Server secret SST. 

                    rtn = MPIN_BLS48556_SERVER(&HSID, &Y, &SST, &U, &SEC);
                    if (rtn != 0)
                    {
                        printf("Server says - Bad Pin.\n");
                        return 1;
                    }
                    else
                    {
                        printf("Server says - PIN is good! You really are ");
                        OCT_output_string(&CLIENT_ID);
                        printf(".\n");
                    }
    printf("\n");
    return 0;
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

    printf("\nTesting MPIN 2-factor authentication protocol on curve BN254\n");
    mpin_BN254(&RNG);

#if CHUNK!=16
    printf("\nTesting MPIN 2-factor authentication protocol on curve BLS12383\n");
    mpin_BLS12383(&RNG);
    printf("\nTesting MPIN 2-factor authentication protocol on curve BLS24479\n");
    mpin192_BLS24479(&RNG);
    printf("\nTesting MPIN 2-factor authentication protocol on curve BLS48556\n");
    mpin256_BLS48556(&RNG);
#endif

    KILL_CSPRNG(&RNG);
}

