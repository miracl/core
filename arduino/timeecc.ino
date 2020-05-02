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

/* Timing ECC Functions */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ecp_XXX.h>
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
using namespace XXX_BIG;

csprng RNG;                // Crypto Strong RNG
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

void setup()
{
    int i;
    ECP P;
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

    Serial.println("Testing/Timing XXX ECC");

#if CURVETYPE_XXX==WEIERSTRASS
    Serial.println("Weierstrass parameterization");
#endif
#if CURVETYPE_XXX==EDWARDS
    Serial.println("Edwards parameterization");
#endif
#if CURVETYPE_XXX==MONTGOMERY
    Serial.println("Montgomery parameterization");
#endif

#if MODTYPE_YYY == PSEUDO_MERSENNE
    Serial.println("Pseudo-Mersenne Modulus");
#endif

#if MODTYPE_YYY == GENERALISED_MERSENNE
    Serial.println("Generalised-Mersenne Modulus");
#endif

#if MODTYPE_YYY == MONTGOMERY_FRIENDLY
    Serial.println("Montgomery Friendly Modulus");
#endif

#if MODTYPE_YYY == NOT_SPECIAL
    Serial.println("Not special Modulus");
#endif

    ECP_generator(&G);
    BIG_rcopy(r, CURVE_Order);
    ECP_copy(&P, &G);
    ECP_mul(&P, r);

    if (!ECP_isinf(&P))
    {
        Serial.println("FAILURE - rG!=O");
        while (1)  delay(1000);

    }
#ifdef ESP32
    xTaskCreatePinnedToCore(
        myloop
        ,  "timeecc"   // A name just for humans
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
    ECP P;
    BIG s;

    BIG_randtrunc(s, r, 2 * CURVE_SECURITY_XXX, &RNG);
    ECP_copy(&P, &G);
    Serial.println("Start ECC point multiplication");
    unsigned long start_time=start();
    ECP_mul(&P, s);
    stop(start_time);

    while (1) delay(1000);
}

