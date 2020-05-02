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

/* test driver and function exerciser for ECDH/ECIES/ECDSA API Functions */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <rsa_XXX.h>
#include <randapi.h>

//#define M4
//#define ESP32
#define HAVE_ECCX08

#ifdef HAVE_ECCX08
#include <ArduinoECCX08.h>
#endif

#ifdef M4
/* DWT (Data Watchpoint and Trace) registers, only exists on ARM Cortex with a DWT unit */
/*!< DWT Control register */
#define KIN1_DWT_CONTROL             (*((volatile uint32_t*)0xE0001000))
/*!< CYCCNTENA bit in DWT_CONTROL register */
#define KIN1_DWT_CYCCNTENA_BIT       (1UL<<0)
/*!< DWT Cycle Counter register */
#define KIN1_DWT_CYCCNT              (*((volatile uint32_t*)0xE0001004))
/*!< DEMCR: Debug Exception and Monitor Control Register */
#define KIN1_DEMCR                   (*((volatile uint32_t*)0xE000EDFC))
/*!< Trace enable bit in DEMCR register */
#define KIN1_TRCENA_BIT              (1UL<<24)

/*!< TRCENA: Enable trace and debug block DEMCR (Debug Exception and Monitor Control Register */
#define KIN1_InitCycleCounter() \
  KIN1_DEMCR |= KIN1_TRCENA_BIT
/*!< Reset cycle counter */
#define KIN1_ResetCycleCounter() \
  KIN1_DWT_CYCCNT = 0
/*!< Enable cycle counter */
#define KIN1_EnableCycleCounter() \
  KIN1_DWT_CONTROL |= KIN1_DWT_CYCCNTENA_BIT
/*!< Disable cycle counter */
#define KIN1_DisableCycleCounter() \
  KIN1_DWT_CONTROL &= ~KIN1_DWT_CYCCNTENA_BIT
/*!< Read cycle counter register */
#define KIN1_GetCycleCounter() \
  KIN1_DWT_CYCCNT
#endif

unsigned long start()
{
    unsigned long start_time=micros();
#ifdef M4
    KIN1_InitCycleCounter(); /* enable DWT hardware */
    KIN1_ResetCycleCounter(); /* reset cycle counter */
    KIN1_EnableCycleCounter(); /* start counting */
#endif
    return start_time;
}

void stop(unsigned long start_time)
{
    unsigned long end_time=micros();
    Serial.print(end_time-start_time);
    Serial.println(" microseconds");
#ifdef M4
    uint32_t cycles = KIN1_GetCycleCounter(); /* get cycle counter */
    KIN1_DisableCycleCounter(); /* disable counting if not used any more */
    Serial.print(cycles);
    Serial.println(" clock cycles");
#endif
}

using namespace core;

csprng RNG;                // Crypto Strong RNG

#ifdef ESP32
#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif
void myloop( void *pvParameters );
#endif

int rsa_XXX()
{
    using namespace XXX;

    int i, res = 1;
    unsigned long ran;
    char m[RFS_XXX], ml[RFS_XXX], c[RFS_XXX], e[RFS_XXX], s[RFS_XXX];
    rsa_public_key pub;
    rsa_private_key priv;

    octet M = {0, sizeof(m), m};
    octet ML = {0, sizeof(ml), ml};
    octet C = {0, sizeof(c), c};
    octet E = {0, sizeof(e), e};
    octet S = {0, sizeof(s), s};

    Serial.println("Generating public/private key pair");
    RSA_KEY_PAIR(&RNG, 65537, &priv, &pub, NULL, NULL);

    Serial.println("Encrypting test string");
    OCT_jstring(&M, (char *)"Hello World\n");

    OAEP_ENCODE(HASH_TYPE_RSA_XXX, &M, &RNG, NULL, &E); // OAEP encode message m to e

    RSA_ENCRYPT(&pub, &E, &C);   // encrypt encoded message
    Serial.print("Ciphertext= ");
    OCT_output(&C);

    Serial.println("Decrypting test string");
    RSA_DECRYPT(&priv, &C, &ML); // ... and then decrypt it
    Serial.println("Decrypted");
    OAEP_DECODE(HASH_TYPE_RSA_XXX, NULL, &ML);  // decode it
    Serial.println("Decoded");
    OCT_output_string(&ML);

    Serial.println("Signing message");
    PKCS15(HASH_TYPE_RSA_XXX, &M, &C);

    RSA_DECRYPT(&priv, &C, &S); // create signature in S

    Serial.print("Signature= ");
    OCT_output(&S);

    RSA_ENCRYPT(&pub, &S, &ML);

    if (OCT_comp(&C, &ML)) Serial.println("Signature is valid");
    else
    {
        Serial.println("Signature is INVALID");
        res = 0;
    }
    RSA_PRIVATE_KEY_KILL(&priv);

    OCT_clear(&M);
    OCT_clear(&ML);   // clean up afterwards
    OCT_clear(&C);
    OCT_clear(&E);

    return res;
}

void setup()
{
    int i;
    Serial.begin(115200);
    while (!Serial);
#ifdef HAVE_ECCX08
    if (!ECCX08.begin()) {
        Serial.println("Failed to communicate with ECC508/ECC608!");
        while (1);
    }
#endif
    char raw[100];
    octet RAW = {0, sizeof(raw), raw};
    RAW.len = 100;
#ifdef HAVE_ECCX08
    for (i = 0; i < 100; i++) RAW.val[i] = ECCX08.random(256);
#else
    for (i = 0; i < 100; i++) RAW.val[i] = i + 1;
#endif
    CREATE_CSPRNG(&RNG, &RAW);  // initialise strong RNG

#ifdef ESP32
    xTaskCreatePinnedToCore(
        myloop
        ,  "testrsa"   // A name just for humans
        ,  32768  // This stack size can be checked & adjusted by reading the Stack Highwater
        ,  NULL
        ,  3  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
        ,  NULL 
        ,  ARDUINO_RUNNING_CORE);
#endif
}

#ifdef ESP32
void loop()
{
}

void myloop(void *pvParameters) {
    (void) pvParameters;
#else
void loop() {
#endif
    Serial.println("\nTesting RSA protocols");
    if (!rsa_XXX())
    {
        Serial.println("RSA failed");
        while (1) ;
    }
}

