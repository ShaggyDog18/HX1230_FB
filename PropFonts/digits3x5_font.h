#ifndef Digits3x5_font_h
#define Digits3x5_font_h

#include "Arduino.h"

const uint8_t Digits3x5[] PROGMEM =
{
-3, 6, '0', '9'+5,
        0x03, 0x1F, 0x11, 0x1F,                  //  0
        0x01, 0x1F, 0x00, 0x00,                  //  1
        0x03, 0x1D, 0x15, 0x17,                  //  2
        0x03, 0x15, 0x15, 0x1F,                  //  3
        0x03, 0x07, 0x04, 0x1F,                  //  4
        0x03, 0x17, 0x15, 0x1D,                  //  5
        0x03, 0x1F, 0x15, 0x1D,                  //  6
        0x03, 0x01, 0x01, 0x1F,                  //  7
        0x03, 0x1F, 0x15, 0x1F,                  //  8
        0x03, 0x17, 0x15, 0x1F,                  //  9
        0x01, 0x0A, 0x00, 0x00,                  //  :
        0x01, 0x10, 0x00, 0x00,                  //  .
        0x03, 0x04, 0x04, 0x04,                  //  -
        0x03, 0x04, 0x0E, 0x04,                  //  +
        0x03, 0x1F, 0x10, 0x10                   //  L
};
#endif

