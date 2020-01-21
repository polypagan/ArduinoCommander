#ifndef ESP8266_H
#define ESP8266_H

/*
 * For compatability with AVR's
 * Serialprint is used in main sketch
 * instead of Serial.printf()
 * 
 */
#define Serialprint Serial.printf

const uint8_t MINPIN = 0;
const uint8_t MAXPIN = 17;

const uint8_t ADCBIAS = 17;

const uint8_t DINmask = 0x10,
              DOUTmask = 0x08,
              PWMmask  = 0x04,
              ADCmask  = 0x02,
              DACmask  = 0x01;

const uint8_t PINTAB[] = {
//		IN	OUT	PWM	ADC	DAC
	B11100,	// GPIO0, has pullup
	B00000,	// GPIO1, TX0, don’t use
	B11100,	// GPIO2, TX1, debug logging output
	B00000,	// GPIO3, RXD, don’t use
	B11100,	// GPIO4
	B11100,	// GPIO5

	B00000,	// GPIO6, don’t use
	B00000,	// GPIO7, don’t use
	B00000,	// GPIO8, don’t use
	B00000,	// GPIO9, don’t use
	B00000,	// GPIO10, don’t use
	B00000,	// GPIO11, don’t use

	B11100,	// GPIO12, “MISO”
	B11100,	// GPIO13, “MOSI”
	B11100,	// GPIO14, “CLK”
	B11100,	// GPIO15, “SS”
	B11100,	// GPIO16, has pulldown, wake from sleep.
  B00010  // treat A0 as 17th pin.
};

// methods
byte pin2adc(byte );
byte adc2pin(byte );
byte testPin(byte , byte);
void outDAC(byte , byte);
void usage(void );

byte pin2adc(byte p) {
  return (byte)(p - ADCBIAS);
}

byte adc2pin(byte a) {
  return (byte)ADCBIAS;
}

byte testPin(byte p, byte m) {
//  Serial.printf("testPin(p:%d, m:%d) ",p, m);
  if ((p < MINPIN) || (p > MAXPIN)) {
    Serial.printf("Bad Pin!\n");
    return 0;
  }
//  Serial.printf("PINTAB[%d]:0x%02x, & 0x%02x --> 0x%02x.\n",p, PINTAB[p], m, m&PINTAB[p]);
  return PINTAB[p] & m;  
}

void outDAC(byte p, byte v) {
  
}
/* usage() - instruct user in syntax
 *  in: nothing
 *  out: nothing
 *  
 *  side effect: useage messages printed
 */
void usage() {
  Serial.print("Usage");
  Serial.print(":\n\tD [0,2,4..5,12..16] <data>\n\tA [0] <data>\n");
  Serial.print("<data> provided indicates output.\n");
  Serial.print("(analog out [PWM] to all but 16)\n");
  Serial.print("Command consisting of only '\"' repeats previous action.\n");
  Serial.print("\nD1 & mini, also NodeMCU devices use 'D' numbering as follows:\n");
  Serial.print("\tGPIO\tD#\tNote\n\t 0\t3\n\t 1\t10\tTX\n\t 2\t4\n\t 3\t9\tRX\n");
  Serial.print("\t 4\t2\tSDA\n\t 5\t1\tSCL\n\t--\t--\t--\n\t12\t6\tMISO\n");
  Serial.print("\t13\t7\tMOSI\n\t14\t5\tCLK\n\t15\t8\n\t16\t0\tLED & wake.\n");
  Serial.print("(This sketch using GPIO numbering.)");
}
#endif
