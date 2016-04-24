// Demo Code for Heatshrink (Copyright (c) 2013-2015, Scott Vokes <vokes.s@gmail.com>)
// embedded compression library
// Craig Versek, Apr. 2016

#include <stdint.h>
#include <ctype.h>
#include "heatshrink_encoder.h"
#include "heatshrink_decoder.h"

#define arduinoLED 13   // Arduino LED on board

/******************************************************************************/
// TEST CODE from adapted from test_heatshrink_static.c
#if HEATSHRINK_DYNAMIC_ALLOC
#error HEATSHRINK_DYNAMIC_ALLOC must be false for static allocation test suite.
#endif

#define HEATSHRINK_DEBUG

static heatshrink_encoder hse;
static heatshrink_decoder hsd;

//static void fill_with_pseudorandom_letters(uint8_t *buf, uint16_t size, uint32_t seed) {
//    uint64_t rn = 9223372036854775783; // prime under 2^64
//    for (int i=0; i<size; i++) {
//        rn = rn*seed + seed;
//        buf[i] = (rn % 26) + 'a';
//    }
//}

#ifdef HEATSHRINK_DEBUG
static void dump_buf(const char *name, uint8_t *buf, uint16_t count) {
    for (int i=0; i<count; i++) {
        uint8_t c = (uint8_t)buf[i];
        //printf("%s %d: 0x%02x ('%c')\n", name, i, c, isprint(c) ? c : '.');
        Serial.print(name);
        Serial.print(" ");
        Serial.print(i);
        Serial.print(": 0x");
        Serial.print(c, HEX);
        Serial.print(" ('");
        Serial.print(isprint(c) ? c : '.');
        Serial.print("')\n");
    }
}
#endif

static void compress(uint8_t *input,
                     uint32_t input_size,
                     uint8_t *output,
                     uint32_t &output_size
                    ){
    heatshrink_encoder_reset(&hse);
    
    #ifdef HEATSHRINK_DEBUG
    Serial.print("\n^^ COMPRESSING\n");
    dump_buf("input", input, input_size);
    #endif
        
    size_t   count  = 0;
    uint32_t sunk   = 0;
    uint32_t polled = 0;
    while (sunk < input_size) {
        //ASSERT(heatshrink_encoder_sink(&hse, &input[sunk], input_size - sunk, &count) >= 0);
        heatshrink_encoder_sink(&hse, &input[sunk], input_size - sunk, &count);
        sunk += count;
        #ifdef HEATSHRINK_DEBUG
        Serial.print("^^ sunk ");
        Serial.print(count);
        Serial.print("\n");
        #endif
        if (sunk == input_size) {
            //ASSERT_EQ(HSER_FINISH_MORE, heatshrink_encoder_finish(&hse));
            heatshrink_encoder_finish(&hse);
        }

        HSE_poll_res pres;
        do {                    /* "turn the crank" */
            pres = heatshrink_encoder_poll(&hse,
                                           &output[polled],
                                           output_size - polled,
                                           &count);
            //ASSERT(pres >= 0);
            polled += count;
            #ifdef HEATSHRINK_DEBUG
            Serial.print("^^ polled ");
            Serial.print(polled);
            Serial.print("\n");
            #endif
        } while (pres == HSER_POLL_MORE);
        //ASSERT_EQ(HSER_POLL_EMPTY, pres);
        #ifdef HEATSHRINK_DEBUG
        if (polled >= output_size){
            Serial.print(F("FAIL: compression should never expand that much"));
        }
        #endif
        if (sunk == input_size) {
            //ASSERT_EQ(HSER_FINISH_DONE, heatshrink_encoder_finish(&hse));
            heatshrink_encoder_finish(&hse);
        }
    }
    #ifdef HEATSHRINK_DEBUG
    Serial.print("in: ");
    Serial.print(input_size);
    Serial.print(" compressed: ");
    Serial.print(polled);
    Serial.print(" \n");
    #endif
    //update the output size to the (smaller) compressed size
    output_size = polled;
}

/******************************************************************************/

#define BYTE_BUFFER_SIZE 128
uint8_t byte_buffer[BYTE_BUFFER_SIZE];

void setup() {
  pinMode(arduinoLED, OUTPUT);      // Configure the onboard LED for output
  digitalWrite(arduinoLED, LOW);    // default to LED off

  Serial.begin(9600);
}

void loop() {
  
}

