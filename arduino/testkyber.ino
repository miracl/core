/*
 * Copyright (c) 2012-2021 MIRACL UK Ltd.
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

/* test driver and function exerciser for Dilithium Functions */

// See https://eprint.iacr.org/2017/633 (Ducas, Lepoint, Lyubashevsky, Schwabe, Seiler, Stehle)

#include <stdio.h>
#include <time.h>
#include <kyber.h>
#include <randapi.h>

// requires too much RAM for some boards :(

//#define M4
//#define ESP32
//#define HAVE_ECCX08

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

csprng RNG;

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

//setCpuFrequencyMhz(80);
    xTaskCreatePinnedToCore(
        myloop
        ,  "testkyber"   // A name just for humans
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

    char sk[KYBER_SECRET_CPA], pk[KYBER_PUBLIC],ct[KYBER_CIPHERTEXT],skc[KYBER_SECRET_CCA];
    octet SKC = {0, sizeof(skc), skc};
    octet SK = {0, sizeof(sk), sk};
    octet PK = {0, sizeof(pk), pk};
    octet CT = {0, sizeof(ct), ct};
    byte tau[32],ss1[32],ss2[32],r64[64],r32[32];
    byte coins[32];
    unsigned long start_time;
    bool success;

    Serial.println("Testing Kyber CPA");
   
    for (int i=0;i<32;i++)
			ss1[i]=42+i;

    for (int i=0;i<32;i++)
        tau[i]=RAND_byte(&RNG);

    for (int i=0;i<32;i++)
        coins[i]=RAND_byte(&RNG); 
  
    Serial.println("Kyber CPA generate key pair");
    start_time=start();
    KYBER_CPA_keypair(tau,&SK,&PK);
    stop(start_time);

    Serial.println("Kyber CPA encrypt");
    start_time=start();
    KYBER_CPA_encrypt(coins,&PK,ss1,&CT); 
    stop(start_time);

    Serial.println("Kyber CPA decrypt");
    start_time=start();
    KYBER_CPA_decrypt(&SK,&CT,ss2);
    stop(start_time);
       
    success=true;
    for (int i=0;i<32;i++)
        if (ss1[i]!=ss2[i]) success=false;
    if (success) Serial.println("CPA Decapsulation succeeded");
    else Serial.println("CPA Decapsulation failed");

    Serial.println("Testing Kyber CCA");

    for (int i=0;i<64;i++)
        r64[i]=RAND_byte(&RNG); 
  
    for (int i=0;i<32;i++)
        r32[i]=RAND_byte(&RNG);

    Serial.println("Kyber CCA generate key pair");
    start_time=start();
    KYBER_CCA_keypair(r64,&SKC,&PK);
    stop(start_time);

    Serial.println("Kyber CCA encrypt");
    start_time=start();
    KYBER_CCA_encrypt(r32,&PK,ss1,&CT); 
    stop(start_time);

    Serial.println("Kyber CCA decrypt");
    start_time=start();
    KYBER_CCA_decrypt(&SKC,&CT,ss2);
    stop(start_time);
        
    success=true;
    for (int i=0;i<32;i++)
        if (ss1[i]!=ss2[i]) success=false;
    if (success) Serial.println("CCA Decapsulation succeeded");
    else Serial.println("CCA Decapsulation failed");

#ifdef ESP32
    Serial.print("Amount of unused stack memory ");     // useful information!
    Serial.println(uxTaskGetStackHighWaterMark(NULL));
#endif

    while (1) delay(1000);

}

