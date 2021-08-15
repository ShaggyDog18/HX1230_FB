// HX1230 96x68 LCD FB library example
// (c) 2019 Pawel A. Hernik
// Modified by ShaggyDog AUG-2021, tested with STM32F1 at SPI2 i/f

/*
  HX1230 96x68 LCD connections (header on bottom, from left):
  #1 RST - D6 or any digital  // any or PA8 STM32F1 
  #2 CE  - D7 or any digital  // PB12 STM32F1 SPI2
  #3 N/C
  #4 DIN - D11/MOSI // PB15 STM32F1 SPI2
  #5 CLK - D13/SCK  // PB13 STM32F1 SPI2
  #6 VCC - 3V3
  #7 BL  - 3V3 or any digital
  #8 GND - GND
*/

#ifdef __arm__
	//#define SPI_SPI1
	#define	SPI_SPI2			// use SPI2 in ARM STM32
  #define LCD_RST PA8   // any or PA8 STM32F1 
  #define LCD_CS  PB12  // SPI2 STM32F1
  #define LCD_BL  PA7
#elif defined( __AVR__ )
  #define LCD_RST 6
  #define LCD_CS  7
  #define LCD_BL  8
#endif

#include "HX1230_FB.h"
#include <SPI.h>
HX1230_FB lcd(LCD_RST, LCD_CS);

// from PropFonts library
#include "PropFonts/c64enh_font.h"

void setup() 
{
  Serial.begin(9600);
  pinMode(LCD_BL, OUTPUT);
  //analogWrite(LCD_BL,30);
  digitalWrite(LCD_BL, HIGH);

  SPI.begin();
  lcd.init();
  lcd.cls();
  lcd.setFont(c64enh);
  lcd.printStr(ALIGN_CENTER, 30, "Hello World!");
  lcd.drawRect(4,20,95-4*2,67-20*2,1);
  lcd.display();
}

void loop() 
{
}

