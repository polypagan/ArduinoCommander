/* Arduino Commander utility.
 *  20200118 DdelV
 *  This version (VI) begins process or transporting to ESP's 
 *  and other MCU's.
 *  
 * Command Set:
 *
 * D pin <data>
 * A pin <data>
 * " <repeat previous command>
 * Q pin <describe pin>
 * 
 * if data is given on command line, output to pin.
 * if no data, input and display.
 *
 * Note that "analog output" is translated to PWM on corresponding digital pin.
 * (UNLESS MCU supports DAC output on stated pin.)
 */

/* Some things that *could* be added:
 *
 * translate analog input to voltage (assume or read aRef).
 * 
 * control of analog reference (DEFAULT, INTERNAL, EXTERNAL), toss a few analogRead()'s 
 * (this is slightly dangerous, since analog input must not exceed reference.)
 * syntax: R <arg>
 *
 * control of pullups on analog in (pinMode(An, INPUT), digitalWrite(An, HIGH/LOW))   (?)
 *
 * use of A0-A7 (on ATmega328x) as digital outputs (would require command syntax to distinguish from PWM)
 * (alternatively:  use pin numbers 14-21.) Done (20161122) but,
 * Digital output to A0-A7 1) begs the question of chip/device model (if only between UNO & Nano,
 * 2) forces A pins to a digital level, so that analogRead always returns 0 or 1023 
 * (This *could* be controlled by DIDR0, but when?)
 * (One idea: replace analogRead())
 *
 * a "ditto" command to repeat the previous one (")
 * syntax: "  (done!)
 * 
 * commands to read/write/dump EEPROM.
 * syntax: E address <value>
 * (& dUmp ?)
 * 
 * 
 */
//#include <StreamPrint.h>

#if defined(__AVR_ATmega328P__)
#include "atmega328.h"
#elif defined (__AVR_ATmega32U4__)
#include "ATmega32u4.h"
#elif defined (ESP8266)
#include "ESP8266.h"
#elif defined (ESP32)
#include "ESP32.h"
#endif


const unsigned long int BAUD=115200L;

// parser states (X means eXpect):
enum {X0, X1, X2, X3, X4 };
// these should be obvious:

enum {IN, OUT};   // directions

enum {DIGITAL, ANAlog , QUERY};    // label "ANALOG" conflicts with .arduino15/packages/esp32/1.0.4/cores/esp32/esp-hal-gpio.h

// syntax is case-insensitive, all uppercase internally
inline char upcase(char c) { return c &= 0xdf; }

void setup() {
  Serial.begin(BAUD);
  while (!Serial) { ; }
  Serialprint("\nArduino Commander VI!\n");
}
// usage() function moved to appropriate include


void goDoIt(byte t, byte d, byte p, uint16_t v) {   // now responsible for analog, digital input and output.
/* goDoIt
 *  perform I/O as specified:
 *  
 *  type (t) is query, Digital, or Analog,
 *  direction (d) is In or Out,
 *  pin (p) is pin number (error checked),
 *  value (v) is value to output.
 *  
 */
 byte m;
 
  if (t == QUERY) {
    Serialprint("Pin %d: ",p);
    m = testPin(p, DINmask+DOUTmask);
//    Serialprint("m: %d ", m);   // debug
    switch (m) {
      case DINmask+DOUTmask :
        Serial.print("I/O");
        break;
      case DINmask :
        Serial.print("INPUT");
        break;
      case DOUTmask :
        Serial.print("OUTPUT");
        break;
    }
    if (testPin(p, PWMmask)) {
      Serial.print(", PWM");
    }
    if (testPin(p,ADCmask)) {
      Serialprint(", ADC%d", pin2adc(p));
    }
    if (testPin(p,DACmask)) {
      Serial.print(", DAC");
    }
    if (testPin(p, DINmask+DOUTmask+PWMmask+ADCmask+DACmask) == 0) {
      Serial.print("Do Not Use!");
    }
    Serial.println();
  } else if (t == DIGITAL) {
    switch (d) {
      case IN :
                if(testPin(p, DINmask)) {
//                  Serialprint("read pin %d (t=%d,d=%d,p=%d,v=%d)\n",p,t,d,p,v);
                     pinMode(p,INPUT);
                     if (p < ADCBIAS) {
                      Serialprint("Pin D%d -> %d\n", p, digitalRead(p));
                     } else {
                      Serialprint("Pin A%d -> %d\n", p-ADCBIAS, digitalRead(p));
                     }
                } else {
                  Serialprint("Error: pin %d is not capable of digital input.\n", p);
                  usage();  // here's your punishment...
                }
          break;
      case OUT : 
                if(testPin(p, DOUTmask)) {
//                  Serialprint("output %d on pin %d (t=%d,d=%d,p=%d,v=%d)\n",v,p,t,d,p,v);
                   if(v)v = 1;     // make every non-zero value = 1 for digital output.
                   pinMode(p,OUTPUT);
                   digitalWrite(p,v);
                   if (p < ADCBIAS) {
                    Serialprint("Pin D%d <- %d\n", p, v);
                   } else {
                    Serialprint("Pin A%d <- %d\n", p-ADCBIAS, v);
                   }
                } else {
                  Serialprint("Error: pin %d is not capable of digital input.\n", p);
                  usage();
                }
           break;
      default : Serialprint("BAD CALL TO goDoIt(t=%d,d=%d,p=%d,v=%d\n",t,d,p,v);
          break;
    }
    
  } else if (t == ANAlog) {
    switch (d) {
      case IN :
          if (testPin(p+ADCBIAS, ADCmask)) {
            analogRead(p);
            Serialprint("Pin A%d -> %d\n", p, analogRead(p));
            } else usage();
          break;

      case OUT :
          if (testPin(p, DACmask)) {
            outDAC(p, v);
          } else if (testPin(p, PWMmask)) {
            analogWrite(p,v);
            Serialprint("Pin D%d <- %d (PWM)\n",p, v);
            break;
          } else usage();
          break;

     default :
      Serialprint("Something went wrong in goDoIt(t:%d, d:%d, p%d, v:%d\n", t, d, p, v);
      break;
      }
    }
  }


/* nextChar() -- parse command line one character per call.
 *  
 *  uses finite state machine and static variables.
 *  
 *  in: the next character,
 *  out: nothing - action is completed when command terminates.
 *  
 */
void nextChar(char c) {
static byte state = X0;
static byte type, pin, dir;
static uint16_t val;

  // Serialprint("state = %d\n",state);
  
  switch (state) {
      case X0 : if (c == '"') {  // Ditto command?
                   goDoIt(type, dir, pin, val);   // repeat using last variables
                   } else if (c == 'Q') {   // if command is Query
                    type = QUERY;
                    state = X1;
                   } else if (c == 'D') {  // if command is 'd'
                   type = DIGITAL;  // digital pin type
                   state = X1;  // advance to pin number
                   } else if (c == 'A') { // if it's 'a',
                     type = ANAlog;     // type is analog
                     state = X1;  // advance to pin #
                   } else if (c != '\n') {  // or explain syntax to user
                     usage();
                 }
                 break;
                 
    case X1 : if (isdigit(c)) { // when digit seen,
                      pin = c - '0';  // start collecting pin #
                      state = X2; // get rest or end of line
                    }                 // else no change
                    break;
                    
    case X2 : if (c == '\n') {  // line done, reset state and do command
                    state = X0;
                    dir = IN;
                    goDoIt(type, IN, pin, 0);
                  } else if (isdigit(c)) {    // more digits in pin #
                    pin = (pin * 10) + (c - '0');
                  } else if (c == ' ') {      // a space is required here if there's more
                    state = X3;
                  } else {
                    usage();
                  }
                  break;
                  
    case X3 : if (c == '\n') {    // allow sloppy trailing space
                      state = X0;
                      dir = IN;
                      goDoIt(type, IN, pin, 0);
                    } else if (isdigit(c)) {  // more likely a value to output
                      val = c -'0';           // start collecting value
                      state = X4;
                    } // else no change
                    break;
                    
    case X4 : if (isdigit(c)) {         // more digits in value
                      val = (val * 10) + (c - '0');
                    } else {                  // anything else, we're done.
                      state = X0;
                      dir = OUT;
                      goDoIt(type, OUT, pin, val);
                    }
                    break;
                    
    default:        Serialprint("STATE OUTTA WACK (%d)\n",state);
                    state = X0;
                    break;
           
  }
}
void loop() {
  char c;
  
  if (Serial.available() != 0) {  // if there is a character in the buffer,
    c = Serial.read();            // get it.
    if (isalpha(c)) {             // if alphabetic,
      c = upcase(c);              // ensure single case.
    }
    if (c == '\r') c = '\n';
//    Serialprint("c is %02x '%c'\n",c,c);
    nextChar(c);                  // feed it to the parser.
  }
  delay(1);                       // yield scheduler.
  // **
  // if you have something else to do, do it here.
  // **
}
