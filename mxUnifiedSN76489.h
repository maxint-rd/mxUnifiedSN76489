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
#ifndef _MXUNIFIEDSN76489_H
#define _MXUNIFIEDSN76489_H

#include <mxUnifiedIO.h>

#define SN76489_CMD  0x8
#define SN76489_VOL  0x1
#define SN76489_TONE 0x0
#define SN76489_CHN0 0x0
#define SN76489_CHN1 0x1
#define SN76489_CHN2 0x2
#define SN76489_CHN3 0x3

#define SN76489_NIBBLE_VOL(chan) ((SN76489_CMD | ((chan&0b11)<<1) |SN76489_VOL) << 4)
#define SN76489_NIBBLE_TONE(chan) ((SN76489_CMD | ((chan&0b11)<<1) |SN76489_TONE) << 4)

#define SN76489_PULSE_WE 50		// Duration of /WE pulse in micro-seconds. (1ms in example seems way too long)
#define SN76489_CLKDIV32 125000.0	// clock divided by 32, required to calculate prescaler value for tone

#define OPT_SN7648_NOTE 0			// Set to 1 to include a note method that uses note-numbers and a frequency table
															// BTW: when using optimization the linker will exclude code that is not used.


class mxUnifiedSN76489
{
 public:
 	mxUnifiedSN76489(mxUnifiedIO *pUniOut);										// use expanded pins 0-7 for data, expanded pin 8 for /WE
 	mxUnifiedSN76489(mxUnifiedIO *pUniOut, int8_t NotWE);			// use expanded pins 0-7 for data, specify regular for /WE
 	void begin(void);
	void tone(float ftFrequency, uint8_t nVolume=0, uint8_t nChannel=0);		// A4=440Hz, standard tuning, volume 0-15, 15 for silence
	void volume(uint8_t nVolume, uint8_t nChannel=0);	// set the volume on the specified channel. Use nVolume=15 to stop playing the tone
  void noTone(uint8_t nChannel=0);	// kill sound on specified channel
 	void beep(void);									// simple beep sound
 	void stop();											// stop sound on all channels

#if(OPT_SN7648_NOTE)
 	void note(uint8_t nNote=50, uint8_t nDecay=10);		// simple beep sound, note: 0-87 (for midi notes 21-108)

	//  static const uint16_t NOTE_TABLE[];
  static const float FREQ_TABLE[];
#endif

 private:
  mxUnifiedIO *_pUniOut;
  int8_t _nPin_NotWE;		// regular pin or set to 0 to use expanded pin 8
  bool _fUseMCUpin;		// use MCU pins for NotWE or use mxUnifiedIO pins NotWE=P8

 	void putByte(byte b);		// put byte on expanded pins 0-7
 	void sendByte(byte b);
  void silenceAllChannels(void);
	void tone(uint16_t nFreqVal, uint8_t nVolume=0, uint8_t nChannel=0);		// nFreqVal= clock / 32 / freq
  void digitWrite(uint8_t nPin, uint8_t nValue, bool fUseMCUpin=true);
	void pnMode(uint8_t, uint8_t, bool fUseMCUpin=true);

};

#if 0
// NO USED (kept for informational purposes)
// https://github.com/allenhuffman/MusicSequencerTest/blob/master/SN76489.ino
// SN76489 note table.
// This table defines the values used to represent all 88 notes of a piano.
// 4Mhz Crystal
// formula from datasheet: n= clock / 32*freq = 4exp6 / (32*freq) = 125000/freq
/* ___________________________________________      _______________________
   # # | # # | # # # | # # | # # # | # # | # #......# # # | # # | # # # | #
   # # | # # | # # # | # # | # # # | # # | # #......# # # | # # | # # # | #
   |_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|......|_|_|_|_|_|_|_|_|_|_|_|
    A B C D E F G A B C D E F G A B C D E F G        G A B C D E F G A B C 
    0 0 1 1 1 1 1 1 1 2 2 2 2 2 2 2 3 3 3 3 3        6 6 6 7 7 7 7 7 7 7 8 
*/

static const uint16_t PROGMEM G_notes[88] = {
  //DEC     HEX     #   NOTE  MIDI  Comments
  4545,  // 0x11C1  0   A0    21    Lowest piano key. (MIDI note 21)
  4290,  // 0x10C2  1   A0#   22
  4050,  // 0xFD2   2   B0    23

  3822,  // 0xEEE   3   C1    24
  3608,  // 0xE18   4   C1#   25
  3405,  // 0xD4D   5   D1    26
  3214,  // 0xC8E   6   D1#   27
  3034,  // 0xBDA   7   E1    28
  2863,  // 0xB2F   8   F1    29
  2703,  // 0xA8F   9   F1#   30
  2551,  // 0x9F7   10  G1    31
  2408,  // 0x968   11  G1#   32
  2273,  // 0x8E1   12  A1    33
  2145,  // 0x861   13  A1#   34
  2025,  // 0x7E9   14  B1    35
 
  1911,  // 0x777   15  C2    36
  1804,  // 0x70C   16  C2#   37
  1703,  // 0x6A7   17  D2    38
  1607,  // 0x647   18  D2#   39
  1517,  // 0x5ED   19  E2    40
  1432,  // 0x598   20  F2    41
  1351,  // 0x547   21  F2#   42
  1276,  // 0x4FC   22  G2    43
  1204,  // 0x4B4   23  G2#   44
  1136,  // 0x470   24  A2    45
  1073,  // 0x431   25  A2#   46
  1012,  // 0x3F4   26  B2    47    LOWEST NOTE AT 4Mhz (?)

  956,   // 0x3BC   27  C3    48
  902,   // 0x386   28  C3#   49
  851,   // 0x353   29  D3    50
  804,   // 0x324   20  D3#   51
  758,   // 0x2F6   31  E3    52
  716,   // 0x2CC   32  F3    53
  676,   // 0x2A4   33  F3#   54
  638,   // 0x27E   34  G3    55
  602,   // 0x25A   35  G3#   56
  568,   // 0x238   36  A3    57
  536,   // 0x218   37  A3#   58
  506,   // 0x1FA   38  B3    59

  478,   // 0x1DE   39  C4    60    MIDDLE C
  451,   // 0x1C3   40  C4#   61
  426,   // 0x1AA   41  D4    62
  402,   // 0x192   42  D4#   63
  379,   // 0x17B   43  E4    64
  358,   // 0x166   44  F4    65
  338,   // 0x152   45  F4#   66
  319,   // 0x13F   46  G4    67
  301,   // 0x12D   47  G4#   68
  284,   // 0x11C   48  A4    69    440hz (standard tuning)
  268,   // 0x10C   49  A4#   70
  253,   // 0xFD    50  B4    71

  239,   // 0xEF    51  C5    72
  225,   // 0xE1    52  C5#   73
  213,   // 0xD5    53  D5    74
  201,   // 0xC9    54  D5#   75
  190,   // 0xBE    55  E5    76
  179,   // 0xB3    56  F5    77
  169,   // 0xA9    57  F5#   78
  159,   // 0x9F    58  G5    79
  150,   // 0x96    59  G5#   80
  142,   // 0x8E    60  A5    81
  134,   // 0x86    61  A5#   82
  127,   // 0x7F    62  B5    83

  119,   // 0x77    63  C6    84
  113,   // 0x71    64  C6#   85
  106,   // 0x6A    65  D6    86
  100,   // 0x64    66  D6#   87
  95,    // 0x5F    67  E6    88
  89,    // 0x59    68  F6    89
  84,    // 0x54    69  F6#   90
  80,    // 0x50    70  G6    91
  75,    // 0x4B    71  G6#   92
  71,    // 0x47    72  A6    93
  67,    // 0x43    73  A6#   94
  63,    // 0x3F    74  B6    95

  60,    // 0x3C    75  C7    96
  56,    // 0x38    76  C7#   97
  53,    // 0x35    77  D7    98 
  50,    // 0x32    78  D7#   99
  47,    // 0x2F    79  E7    100
  45,    // 0x2D    80  F7    101
  42,    // 0x2A    81  F7#   102
  40,    // 0x28    82  G7    103
  38,    // 0x26    83  G7#   104
  36,    // 0x24    84  A7    105
  34,    // 0x22    85  A7#   106
  32,    // 0x20    86  B7    107

  30,    // 0x1E    87  C8    108   Highest piano key.
/*
 * These are higher notes that are not on a piano.
 * 
  28,    // 0x1C    88  C8#   109 
  27,    // 0x1B    89  D8    110
  25,    // 0x19    90  D8#   111
  24,    // 0x18    91  E8    112
  22,    // 0x16    92  F8    113
  21,    // 0x15    93  G8    114
  20,    // 0x14    94  G8#   115
  19,    // 0x13    95  A8    116
  18,    // 0x12    96  A8#   117
  17,    // 0x11    97  B8    118
 
  16,    // 0x10    98  C9    119   HIGHEST NOTE at 4MHz (?)
  15,    // 0xF     99  C8#   120
  14,    // 0xE     100 D8    121
  13,    // 0xD     101 D8#   122
  13,    // 0xD     102 E8    123
  12,    // 0xC     103 F8    124
  11,    // 0xB     104 F8#   125
  11,    // 0xB     105 G8    126
  10,    // 0xA     106 G8#   127   HIGHEST MIDI NOTE
*/
};
#endif

#endif