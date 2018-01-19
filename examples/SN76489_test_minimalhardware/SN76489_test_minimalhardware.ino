// SN76489_test example.
// Example by Maxint R&D for driving the SN76489 using the mxUnifiedIO libraries.
// This example is a variation of the SN76489_test example using minimal hardware.
// This example uses no I/O-expander or external oscillator but uses lots of pins
// of the Arduino. If your MCU has a limited number of pins, use an I/O expander.
//
// For more information and used libraries see
//    https://github.com/maxint-rd/mxUnifiedIO
//    https://github.com/maxint-rd/mxUnifiedSN76489
//    https://github.com/maxint-rd/FaatPwmPin
//
// This example was tested on an Arduino Pro Mini ATmega168 running at 8MHz/3v3,
// using the following connections:
//    Arduino digital pins 2-9: data, connected to SN76489 D7 - D0 (pins 10-13, 15, 1-3)
//    Arduino digital pin 10: write, connected to SN76489 WE (pin 5)
//    Arduino digital pin 11: clock, connected to SN76489 CLOCK (pin 14)
// SN76489 VCC pin 16 is connected to Pro Mini 3v3
// SN76489 OE and GND (pins 6, 8) are connected to Pro Mini GND
// SN76489 Audio out (pin 7) is connected to an audio amplifier
//

#include <FastPwmPin.h>
#include <mxUnifiedIO.h>
#include <mxUnifiedSN76489.h>

// use the mxUnifiedIO API without expander (in Setup() Arduino pins 2-9 will be declared as virtual pins P0-P7)
mxUnifiedIO unio = mxUnifiedIO();

// define the WE pin used and initialize the mxUnifiedSN76489 object
mxUnifiedSN76489 sound(&unio, 10);    // use MCU pin 10 for NotWE (in addtion to using an 8-pin virtual expander)

void setup()
{
  Serial.begin(115200);
  Serial.println(F("mxUnifiedSN76489 SN76489_test_minimalhardware"));

  // Generate clock signal on pin D11
  // ATmega328/168 (Uno/Nano/Pro Mini): pin D3 or pin D11 (D11 toggle only)
  FastPwmPin::enablePwmPin(11, 4000000L, 50);

  // initialize the sound chip
  unio.begin(MXUNIFIED_PINS_2TO9);     // start using the mxUnified API without expander using pins D2 to D9 as P0 to P7

/*
  pinMode(13, OUTPUT);
  digitalWrite(13,HIGH);
  delay(100);
  digitalWrite(13,LOW);
  delay(400);


  // test the mxUnifiedIO pins
  for(int n=0; n<unio.getNumPins(); n++)
  {
    unio.digitalWrite(n, HIGH);
    delay(500);
    unio.digitalWrite(n, LOW);
    delay(100);
  } 
  unio.digitalWrite8(0xFF);
  delay(500);
  unio.digitalWrite8(0xAA);
  delay(500);
  unio.digitalWrite8(0x55);
  delay(500);
  unio.digitalWrite8(0xFF);
  delay(500);
  unio.digitalWrite8(0);
  delay(5000);
*/
  digitalWrite(13,HIGH);
  delay(100);
  digitalWrite(13,LOW);
  delay(400);
  
  Serial.print(F("sound.begin()"));
  sound.begin();    // will call SilenceAllChannels();

  // sound a beep
  Serial.println(F("beep()"));
  sound.beep(100);
  delay(900);

  // make a two-voice bell sound
  Serial.println(F("bell"));
  sound.tone(440.0f,0,SN76489_CHN0);
  delay(100);
  for(int n=0; n<15; n++)
  {
    sound.volume(n, SN76489_CHN0);
    if(n==8) sound.tone(220.0f,0,SN76489_CHN1);
    delay(50);
  }
  for(int n=0; n<15; n++)
  {
    sound.volume(n, SN76489_CHN1);
    delay(50);
  }
  sound.stop();
  delay(500);
 
  // sound a siren
  Serial.println(F("siren"));
  for(int n=0; n<10; n++)
  {
    for(float ftTone=100.0; ftTone<4000.0; ftTone+=10)
    {
      sound.tone(ftTone, n);
      delay(1);
    }
  }
  sound.stop();

  // sound some noise
  Serial.println(F("noise"));
  for(int n=0; n<10; n+=3)
  {
    for(float ftTone=0.0; ftTone<600.0; ftTone+=10)
    {
      sound.tone(ftTone, n, SN76489_CHN3);
      delay(100);
    }
  }
  sound.stop();
  delay(500);

  Serial.println(F("loop"));
}

void loop()
{
  // lets sound a small tick every second
  sound.beep(5);
  delay(995);
}
