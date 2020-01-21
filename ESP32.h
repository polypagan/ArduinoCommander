#ifndef ESP32_H
#define ESP32_H

/*
 * For compatability with AVR's
 * Serialprint is used in main sketch
 * instead of Serial.printf()
 * 
 */
#define Serialprint Serial.printf

const uint8_t MINPIN = 0;
const uint8_t MAXPIN = 39;

const uint8_t ADCBIAS = 36;

const uint8_t DINmask = 0x10,
              DOUTmask = 0x08,
              PWMmask  = 0x04,
              ADCmask  = 0x02,
              DACmask  = 0x01;

const uint8_t PINTAB[] = {
//		IN	OUT	PWM	ADC	DAC
	B11100,	// GPIO0
	B00000,	// GPIO1, U0TXD, don’t use
	B11100,	// GPIO2
	B00000,	// GPIO3, U0RXD, don’t use
	B11100,	// GPIO4
	B11100,	// GPIO5
	B00000,	// GPIO6 don't use          \ <-
	B00000,	// GPIO7 don't use          |
	B00000,	// GPIO8 don't use          \___ SPI Flash
	B00000,	// GPIO9, U1RXD, don’t use  /
	B00000,	// GPIO10, U1TXD, don’t use |
	B00000,	// GPIO11 don't use         /
	B11100,	// GPIO12
	B11100,	// GPIO13
	B11100,	// GPIO14
	B11100,	// GPIO15
	B11100,	// GPIO16, U2RXD
	B11100,	// GPIO17, U2TXD
	B11100,	// GPIO18
	B11100,	// GPIO19
	B00000,	// GPIO20 does not exist
	B11100,	// GPIO21
	B11100,	// GPIO22
	B11100,	// GPIO23
	B00000,	// GPIO24 does not exist
	B11101,	// GPIO25, DAC1
	B11101,	// GPIO26, DAC2
	B11100,	// GPIO27
	B00000,	// GPIO28 does not exist
	B00000,	// GPIO29 does not exist
	B00000,	// GPIO30 does not exist
	B00000,	// GPIO31 does not exist
	B11110,	// GPIO32, ADC1_4
	B11110,	// GPIO33, ADC1_5
	B10010,	// GPIO34, ADC1_6, in only
	B10110,	// GPIO35, ADC1_7, in only
	B10110,	// GPIO36, ADC1_0, in only
	B10110,	// GPIO37, ADC1_1, in only
	B10110,	// GPIO38, ADC1_2, in only
	B10010,	// GPIO39, ADC1_3, in only
};

// methods
byte pin2adc(byte );
byte adc2pin(byte );
byte testPin(byte , byte);
void outDAC(byte , byte);
void usage(void );

byte adc2pin(byte a) {
  a += ADCBIAS;
  if (a > MAXPIN)a -= 8;
  return a;
}
byte pin2adc(byte p) {
  if ((p < ADCBIAS-4) || (p > MAXPIN)) return -1; // error! 
  if (p < ADCBIAS)return (8-(ADCBIAS - p));
  return (p - ADCBIAS);
}
byte testPin(byte p, byte m) {      // not 'boolean' for QUERY type.?
//  Serial.printf("testPin(p:%d, m:%d) ",p, m);
  if ((p < MINPIN) || (p > MAXPIN)) {
    Serial.printf("Bad Pin!\n");
    return 0;
  }
//  Serial.printf("PINTAB[%d]:0x%02x, & 0x%02x --> 0x%02x.\n",p, PINTAB[p], m, m&PINTAB[p]);
  return PINTAB[p] & m;
}

void outDAC(byte p, byte v) {
  Serial.printf("outDAC(p:%d, v:%d)\n",p,v);
  dacWrite(p, v);
}

void analogWrite(byte p, uint16_t v) {    // pin should have been thoroughly vetted before now...
  const int channel = 0;
  const int freq = 5000;
  const int resolution = 8;

  ledcSetup(channel, freq, resolution);

  ledcAttachPin(p, channel);

  ledcWrite(channel, v);
  
}
/* usage() - instruct user in syntax
 *  in: nothing
 *  out: nothing
 *  
 *  side effect: useage messages printed
 */
void usage() {
  Serial.print("Usage");
  Serial.print(":\n\tD [0,2,4.5,12..15..19,21..23,25..27] <data>\n\tA [0..7] <data>\n");
  Serial.print("<data> provided indicates output.\n");
  Serial.print("(analog out [PWM] to any D, DAC on 25,26.)\n");
  Serial.print("Command consisting of only '\"' repeats previous action.\n");
}
#endif
