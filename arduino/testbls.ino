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

/* test driver and function exerciser for BLS Signature API Functions */

/* This example program ONLY supports BN and BLS12 curves. But easily edited to support others. 
   Also modifiable to reverse roles of G1 and G2. See ../cpp/testbls.cpp                        */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <bls_XXX.h>
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

static char message[] = "This is a test message";

#ifdef ESP32
#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif
void myloop( void *pvParameters );
#endif

int bls_XXX()
{
    using namespace XXX;

    int res;
    char s[BGS_XXX], w[4 * BFS_XXX + 1], sig[BFS_XXX + 1];
    char ikm[64];
    octet S = {0, sizeof(s), s};
    octet W = {0, sizeof(w), w};
    octet SIG = {0, sizeof(sig), sig};
    octet IKM = {0, sizeof(ikm), ikm};
    octet M = {0,sizeof(message), message};

    OCT_jstring(&M,message);
    OCT_rand(&IKM,&RNG,32);

    Serial.println("Generating key pair");
    res = BLS_KEY_PAIR_GENERATE(&IKM, &S, &W);
    if (res == BLS_FAIL)
    {
        Serial.println("Failed to generate keys");
        return 0;
    }

    Serial.print("Private key= 0x");
    OCT_output(&S);
    Serial.print("Public key= 0x");
    OCT_output(&W);

    Serial.println("Signing");
    BLS_CORE_SIGN(&SIG, &M, &S);
    Serial.print("Signature= 0x");
    OCT_output(&SIG);

    //message[7]='f'; // change the message
    Serial.println("Verifying");
    res = BLS_CORE_VERIFY(&SIG,&M , &W);
    if (res == BLS_OK)
    {
        Serial.println("Signature is OK");
        return 1;
    }
    else
    {
        Serial.println("Signature is *NOT* OK");
        return 0;
    }
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
        ,  "testbls"   // A name just for humans
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
    Serial.println("Testing BLS signature for curve XXX");
    if (!bls_XXX())
    {
        Serial.println("BLS failed");
        while (1) ;
    }
}


