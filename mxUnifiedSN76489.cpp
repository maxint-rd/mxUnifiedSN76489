/*********************************************************************
This is the mxUnifiedIO library for the SN76489 sound chip

Parts of this library are based on the following information and examples:
   http://danceswithferrets.org/geekblog/?p=93
   http://danceswithferrets.org/geekblog/wp-content/uploads/2014/01/sn76489_test.txt
   https://github.com/allenhuffman/MusicSequencerTest/blob/master/SN76489.ino

Library made by Maxint R&D to drive the SN76489 via an I2C I/O expander
or the 74HC595 shift register using the mxUnifiedIO API.
https://github.com/maxint-rd/mxUnifiedIO
*********************************************************************/
#include "mxUnifiedSN76489.h"

//
// Constructors
//
mxUnifiedSN76489::mxUnifiedSN76489(mxUnifiedIO *pUniOut)
{
	_pUniOut=pUniOut;
  _nPin_NotWE = 8;
  _fUseMCUpin=false;
}

mxUnifiedSN76489::mxUnifiedSN76489(mxUnifiedIO *pUniOut, int8_t NotWE)
{
	_pUniOut=pUniOut;
  _nPin_NotWE = NotWE;
  _fUseMCUpin=true;
}

//
// Public methods
//
void mxUnifiedSN76489::begin(void)
{
	pnMode(_nPin_NotWE, OUTPUT, _fUseMCUpin);
  silenceAllChannels();
}


void mxUnifiedSN76489::beep(uint16_t nDuration)
{
#if(OPT_SN7648_NOTE)
	note(50,nDuration/15);
#else
	tone(440.0f);
	delay(nDuration);
	noTone();
#endif
}

void mxUnifiedSN76489::tone(uint16_t nFreqVal, uint8_t nVolume, uint8_t nChannel)
{	// start to play a tone as specified. Use nVolume=15 to stop playing the tone
	// nChannel=0-3, 3=noise
	// nVolume=0-15, 0=highest, 15=silence
	sendByte(SN76489_NIBBLE_TONE(nChannel) | (nFreqVal & 0b1111)); // lowest nibble
  if (nFreqVal > 0b1111)					// more than one byte needed?
		sendByte((nFreqVal >>4));
	uint8_t nVol=min(nVolume, 15);
	sendByte(SN76489_NIBBLE_VOL(nChannel) | nVol);		// vol 15-nVol
}

void mxUnifiedSN76489::tone(float ftFrequency, uint8_t nVolume, uint8_t nChannel)		// A4=440Hz, standard tuning
{	// start to play a tone as specified. Use nVolume=15 to stop playing the tone
	// convert given freqency to freq val
 	// formula from datasheet: n= clock / (32*freq) = 4exp6 / 32 / freq = 125000/freq
 	uint16_t freqval=SN76489_CLKDIV32/ftFrequency;
 	tone(freqval, nVolume, nChannel);
}

void mxUnifiedSN76489::volume(uint8_t nVolume, uint8_t nChannel)		// nChannel=0
{	// set the volume on the specified channel. Use nVolume=15 to stop playing the tone
	uint8_t nVol=min(nVolume, 15);
	sendByte(SN76489_NIBBLE_VOL(nChannel)+nVol);		// vol 15-nVol
}

void mxUnifiedSN76489::noTone(uint8_t nChannel)
{
	volume(0xF, nChannel);
}

#if(OPT_SN7648_NOTE)
void mxUnifiedSN76489::note(uint8_t nNote, uint8_t nDecay)		// nNote=50, nDecay=10
{		// simple beep sound, note: 0-87 (for midi notes 21-108)
	float ftFreq=pgm_read_float(&FREQ_TABLE[nNote+10]);
	tone(ftFreq);
	if(nDecay>0)
	{
		for(uint8_t nVol=0; nVol<=15; nVol++)
		{
			volume(nVol);
			delay(nDecay);
		}
	}
}
#endif

void mxUnifiedSN76489::stop(void)
{
  silenceAllChannels();
}

//
// Private helper methods
//

// Original example sketch to play a tune: http://danceswithferrets.org/geekblog/?p=93

void mxUnifiedSN76489::putByte(byte b)
{
/*
  unio.digitalWrite(kPin_D0, (b&1)?HIGH:LOW);
  unio.digitalWrite(kPin_D1, (b&2)?HIGH:LOW);
  unio.digitalWrite(kPin_D2, (b&4)?HIGH:LOW);
  unio.digitalWrite(kPin_D3, (b&8)?HIGH:LOW);
  unio.digitalWrite(kPin_D4, (b&16)?HIGH:LOW);
  unio.digitalWrite(kPin_D5, (b&32)?HIGH:LOW);
  unio.digitalWrite(kPin_D6, (b&64)?HIGH:LOW);
  unio.digitalWrite(kPin_D7, (b&128)?HIGH:LOW);
*/
  _pUniOut->digitalWrite8(b);   // set 8 expanded pins
}

void mxUnifiedSN76489::sendByte(byte b)
{
  digitWrite(_nPin_NotWE, HIGH, _fUseMCUpin);
  putByte(b);
  digitWrite(_nPin_NotWE, LOW, _fUseMCUpin);
  //delay(1);	// one ms seems rather long. According datasheet the Ready pin should be monitored
  delayMicroseconds(SN76489_PULSE_WE);
  digitWrite(_nPin_NotWE, HIGH, _fUseMCUpin);
}

void mxUnifiedSN76489::silenceAllChannels()
{	// set volume of all channels to 0xF
  sendByte(0x9f);
  sendByte(0xbf);
  sendByte(0xdf);
  sendByte(0xff);
}

void mxUnifiedSN76489::digitWrite(uint8_t nPin, uint8_t nValue, bool fUseMCUpin)		// fUseMCUpin=true
{	// set a pin of the I2C I/O expander/shiftregister
	if(fUseMCUpin)
		::digitalWrite(nPin, nValue);		// call the regular digitalWrite using the global scope resolution operator
	else
		_pUniOut->digitalWrite(nPin, nValue);
}

void mxUnifiedSN76489::pnMode(uint8_t nPin, uint8_t nMode, bool fUseMCUpin)		// fUseMCUpin=true
{	// currently only output pins are supported by mxUnifiedIO
	if(fUseMCUpin)
		::pinMode(nPin, nMode);		// call the regular pinMode using the global scope resolution operator
	else
		_pUniOut->pinMode(nPin, nMode);
}


// clang-format off
/*
#define CLK 4000000/32
static const uint16_t  mxUnifiedSN76489::NOTE_TABLE[] PROGMEM = {
     0,
  //1                2                3                4                5                6                7                8                9                10               11               12
  //C                C#               D                D#               E                F                F#               G                G#               A                A#               B
    CLK / 16.35f,   CLK / 17.32f,   CLK / 18.35f,   CLK / 19.45f,   CLK / 20.60f,   CLK / 21.83f,   CLK / 23.12f,   CLK / 24.50f,   CLK / 25.96f,   CLK / 27.50f,   CLK / 29.14f,   CLK / 30.87f,   // Octave 0
    CLK / 32.70f,   CLK / 34.65f,   CLK / 36.71f,   CLK / 38.89f,   CLK / 41.20f,   CLK / 43.65f,   CLK / 46.25f,   CLK / 49.00f,   CLK / 51.91f,   CLK / 55.00f,   CLK / 58.27f,   CLK / 61.74f,   // Octave 1
    CLK / 65.41f,   CLK / 69.30f,   CLK / 73.42f,   CLK / 77.78f,   CLK / 82.41f,   CLK / 87.31f,   CLK / 92.50f,   CLK / 98.00f,   CLK / 103.83f,  CLK / 110.00f,  CLK / 116.54f,  CLK / 123.47f,  // Octave 2
    CLK / 130.81f,  CLK / 138.59f,  CLK / 146.83f,  CLK / 155.56f,  CLK / 164.81f,  CLK / 174.62f,  CLK / 185.00f,  CLK / 196.00f,  CLK / 207.65f,  CLK / 220.00f,  CLK / 233.08f,  CLK / 246.94f,  // Octave 3
    CLK / 261.63f,  CLK / 277.18f,  CLK / 293.67f,  CLK / 311.13f,  CLK / 329.63f,  CLK / 349.23f,  CLK / 370.00f,  CLK / 392.00f,  CLK / 415.31f,  CLK / 440.00f,  CLK / 466.17f,  CLK / 493.89f,  // Octave 4
    CLK / 523.25f,  CLK / 554.37f,  CLK / 587.33f,  CLK / 622.26f,  CLK / 659.26f,  CLK / 698.46f,  CLK / 739.99f,  CLK / 783.99f,  CLK / 830.61f,  CLK / 880.00f,  CLK / 932.33f,  CLK / 987.77f,  // Octave 5
    CLK / 1046.51f, CLK / 1108.74f, CLK / 1174.67f, CLK / 1244.51f, CLK / 1318.52f, CLK / 1396.92f, CLK / 1479.99f, CLK / 1567.99f, CLK / 1661.23f, CLK / 1760.01f, CLK / 1864.66f, CLK / 1975.54f, // Octave 6
    CLK / 2093.02f, CLK / 2217.47f, CLK / 2349.33f, CLK / 2489.03f, CLK / 2637.03f, CLK / 2793.84f, CLK / 2959.97f, CLK / 3135.98f, CLK / 3322.45f, CLK / 3520.02f, CLK / 3729.33f, CLK / 3951.09f, // Octave 7
};
*/

#if(OPT_SN7648_NOTE)
static const float mxUnifiedSN76489::FREQ_TABLE[] PROGMEM = {
    0,
  //1         2         3         4         5         6         7         8         9         10        11        12
  //C         C#        D         D#        E         F         F#        G         G#        A         A#        B
    16.35f,   17.32f,   18.35f,   19.45f,   20.60f,   21.83f,   23.12f,   24.50f,   25.96f,   27.50f,   29.14f,   30.87f,   // Octave 0
    32.70f,   34.65f,   36.71f,   38.89f,   41.20f,   43.65f,   46.25f,   49.00f,   51.91f,   55.00f,   58.27f,   61.74f,   // Octave 1
    65.41f,   69.30f,   73.42f,   77.78f,   82.41f,   87.31f,   92.50f,   98.00f,   103.83f,  110.00f,  116.54f,  123.47f,  // Octave 2
    130.81f,  138.59f,  146.83f,  155.56f,  164.81f,  174.62f,  185.00f,  196.00f,  207.65f,  220.00f,  233.08f,  246.94f,  // Octave 3
    261.63f,  277.18f,  293.67f,  311.13f,  329.63f,  349.23f,  370.00f,  392.00f,  415.31f,  440.00f,  466.17f,  493.89f,  // Octave 4
    523.25f,  554.37f,  587.33f,  622.26f,  659.26f,  698.46f,  739.99f,  783.99f,  830.61f,  880.00f,  932.33f,  987.77f,  // Octave 5
    1046.51f, 1108.74f, 1174.67f, 1244.51f, 1318.52f, 1396.92f, 1479.99f, 1567.99f, 1661.23f, 1760.01f, 1864.66f, 1975.54f, // Octave 6
    2093.02f, 2217.47f, 2349.33f, 2489.03f, 2637.03f, 2793.84f, 2959.97f, 3135.98f, 3322.45f, 3520.02f, 3729.33f, 3951.09f, // Octave 7
};
// clang-format on
#endif