#ifndef TEMPLATE_H
#define TEMPLATE_H

const uint8_t MINPIN = 0;
const uint8_t MAXPIN = 21;

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
    	B11000,	// D4
    	B11100,	// D5, PWM
    	B11100,	// D6, PWM
    	B11000,	// D7
    	B11000,	// D8
    	B11100,	// D9, PWM
    	B11100,	// D10, PWM, SS
    	B11100,	// D11, PWM, MOSI
    	B11000,	// D12, MISO
    	B11100,	// D13, PWM, LED, SCK
    	B11010,	// D14/A0
    	B11010,	// D15/A1
    	B11010,	// D16/A2
    	B11010,	// D17/A3
    	B11010,	// D18/A4, SDA
    	B11010,	// D19/A5, SCL
    	B11010,	// D20/A6 (exposed on nano)
    	B11010	// D21/A7 (exposed on nano)
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
  return p - ADCBIAS;
}

// convert A# to gpio#
byte adc2pin(byte a) {
  return a + ADCBIAS;
}

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
  Serial.print(":\n\tD [2..13,14..19] <data>\n\tA [0..7] <data>\n");
  Serial.print("<data> provided indicates output.\n");
  Serial.print("(analog out [PWM] to 3,5,6,9,10,11 only.)\n");
  Serial.print("Command consisting of only '\"' repeats previous action.\n");
}
#endif
