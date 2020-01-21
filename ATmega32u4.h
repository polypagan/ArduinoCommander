#ifndef ATMEGA32U4_H
#define ATMEGA32U4_H

/*
 * AVR's don't support printf (Why not?),
 * so, use StreamPrint.
 */
 
#include <StreamPrint.h>

const uint8_t MINPIN = 0;
const uint8_t MAXPIN = 19;

const uint8_t ADCBIAS = 14;

const uint8_t DINmask = 0x10,
              DOUTmask = 0x08,
	            PWMmask  = 0x04,
	            ADCmask  = 0x02,
	            DACmask  = 0x01;


const uint8_t PINTAB[MAXPIN+1] = {
// 		IN	OUT	PWM	ADC	DAC
    	B00000,	// RXD, leave alone
    	B00000,	// TXD, leave alone
    	B11000,	// D2
    	B11100,	// D3, PWM
    	B11010,	// D4, A6
    	B11100,	// D5, PWM
    	B11110,	// D6, PWM, A7
    	B11010,	// D7
    	B11010,	// D8, A8
    	B11110,	// D9, PWM, A9
    	B11110,	// D10, PWM, A10
    	B11100,	// D11, PWM
    	B11010,	// D12, A11
    	B11100,	// D13, PWM, LED, SCK
    	B11010,	// D14/A0
    	B11010,	// D15/A1
    	B11010,	// D16/A2
    	B11010,	// D17/A3
    	B11010,	// D18/A4
    	B11010,	// D19/A5
/*
    	B11010,	// D20/A6 (exposed on nano)
    	B11010	// D21/A7 (exposed on nano)
*/
};

// methods
// these are  MCU-specific and required (even if they do nothing)

byte pin2adc(byte );
byte adc2pin(byte );
byte testPin(byte , byte);
void outDAC(byte , byte);
void usage(void );

// convert gpio# to A#
byte pin2adc(byte p) {
//byte r; // for debug

//  Serialprint("pin2adc(%d",p);
  if ((p >= ADCBIAS) && (p <= MAXPIN)) {
    return (p - ADCBIAS);
  } else {
    switch (p) {
      case 4 :
        return 6;
        break;
      case 6 :
        return 7;
        break;
      case  8 :
      case  9 :
      case 10 :
        return p;
        break;
      case 12:
        return 11;
        break;
      default :
        Serialprint("%d is not an analog input pin!", p);
        return -1;
        break;
    }
  }
//  Serialprint(") returns %d\n",r);
//  return r;
}

/* never called...
// convert A# to gpio#
byte adc2pin(byte a) {
  if ((a < 6) && (a >= 0)) {
    return a + ADCBIAS;
  } else {
    switch (a) {
      case 6 :
        return 4;
        break;
      case 7 :
        return 6;
        break;
      case  8 :
      case  9 :
      case 10 :
        return a;
        break;
      case 11 :
        return 12;
        break;
      default :
        Serialprint("There is no A%d!", a);
        break;
    }
  }
}
*/
// apply mask to PINTAB[pin] and return residue
byte testPin(byte p, byte m) {    // not 'boolean' to allow Query to work
  if ((p < MINPIN) || (p > MAXPIN)) {
    return false;
  }
  return PINTAB[p] & m;
}

// output to DAC, is any.
void outDAC(byte p, byte v) {
  // has no DAC pins.
}

/* usage() - instruct user in syntax
 *  in: nothing
 *  out: nothing
 *  
 *  side effect: useage messages printed
 */
void usage() {
  Serial.print("Usage");
  Serial.print(":\n\tD [2..13,14..19] <data>\n\tA [0..12] <data>\n");
  Serial.print("<data> provided indicates output.\n");
  Serial.print("(analog out [PWM] to 3,5,6,9,10,11,13 only.)\n");
  Serial.print("Command consisting of only '\"' repeats previous action.\n");
  Serial.print("Q<pin#> displays capability of named pin\n");
}
#endif
