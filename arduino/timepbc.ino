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

/* Timing Pairing-Based Crypto */
/* Only suitable for BN and BLS12 curves of embedding degree 12 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pair_XXX.h>
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
using namespace XXX;
//using namespace XXX_FP;
using namespace XXX_BIG;


csprng RNG;                // Crypto Strong RNG
ECP2 Q;
ECP G;
BIG r;

#ifdef ESP32
#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif
void myloop( void *pvParameters );
#endif

void intro()
{
    int i;
    ECP P;
    ECP2 W;
    FP12 g, w;

    char raw[100];
    octet RAW = {0, sizeof(raw), raw};
    RAW.len = 100;
#ifdef HAVE_ECCX08
    for (i = 0; i < 100; i++) RAW.val[i] = ECCX08.random(256);
#else
    for (i = 0; i < 100; i++) RAW.val[i] = i + 1;
#endif
    CREATE_CSPRNG(&RNG, &RAW);  // initialise strong RNG
    Serial.println("Timing XXX PBC");

    ECP_generator(&G);
    ECP2_generator(&Q);
    BIG_rcopy(r, CURVE_Order);

    ECP_copy(&P, &G);
    PAIR_G1mul(&P, r);

    if (!ECP_isinf(&P))
    {
        Serial.println("FAILURE in G1 - rG!=O");
        while (1)  delay(1000);

    }

    ECP2_copy(&W, &Q);
    PAIR_G2mul(&W, r);

    if (!ECP_isinf(&P))
    {
        Serial.println("FAILURE in G2 - rQ!=O");
        while (1)  delay(1000);

    }

    PAIR_ate(&w, &Q, &G);
    PAIR_fexp(&w);

    FP12_copy(&g, &w);
    PAIR_GTpow(&g, r);

    if (!FP12_isunity(&g))
    {
        Serial.println("FAILURE in GT - g^r!=1");
        while (1)  delay(1000);
    }

}

void setup()
{
    Serial.begin(115200);
    while (!Serial);
#ifdef HAVE_ECCX08
    if (!ECCX08.begin()) {
        Serial.println("Failed to communicate with ECC508/ECC608!");
        while (1);
    }
#endif
#ifdef ESP32
    xTaskCreatePinnedToCore(
        myloop
        ,  "timepbc"   // A name just for humans
        ,  32768  // This stack size can be checked & adjusted by reading the Stack Highwater
        ,  NULL
        ,  3  // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
        ,  NULL 
        ,  ARDUINO_RUNNING_CORE);
#else
    intro();
#endif
}

#ifdef ESP32
void loop()
{
}

void myloop(void *pvParameters) {
    (void) pvParameters;
    ECP P;
    ECP2 W;
    FP12 g, w;
    BIG s;
    intro();
#else
void loop() {
    ECP P;
    ECP2 W;
    FP12 g, w;
    BIG s;
#endif
    BIG_randtrunc(s, r, 2 * CURVE_SECURITY_XXX, &RNG);
    Serial.println("Start G1 point multiplication");
    unsigned long start_time=start();
    ECP_copy(&P, &G);
    PAIR_G1mul(&P, s);

    stop(start_time);
    Serial.println("Start G2 point multiplication");
    start_time=start();
    ECP2_copy(&W, &Q);
    PAIR_G2mul(&W, s);
    stop(start_time);
    Serial.println("Start Ate pairing");
    start_time=start();
    PAIR_ate(&w, &Q, &P);
    stop(start_time);
    Serial.println("Start final exponentiation");
    start_time=start();
    PAIR_fexp(&w);
    stop(start_time);
    Serial.println("Start GT exponentiation");
    start_time=start();
    FP12_copy(&g, &w);
    PAIR_GTpow(&g, s);
    stop(start_time);

    while (1) delay(1000);
}


