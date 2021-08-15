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

void HariChord(int frame);

void setup() 
{
  Serial.begin(9600);
  pinMode(LCD_BL, OUTPUT);
  digitalWrite(LCD_BL, HIGH);
  lcd.init();
}

int nFrames = 36;

void loop() 
{
  for (int i=0; i<nFrames; i++) HariChord(i);
  for (int i=(nFrames-1); i >= 0; i--) HariChord(i);
}

// by Hari Wiguna
void HariChord(int frame)
{
  lcd.clearDisplay();
  int n = 7;
  int r = frame * 64 / nFrames;
  float rot = frame * 2*PI / nFrames;
  for (int i=0; i<(n-1); i++) {
    float a = rot + i * 2*PI / n;
    int x1 = SCR_WD/2 + cos(a) * r;
    int y1 = SCR_HT/2 + sin(a) * r;
    for (int j=i+1; j<n; j++) {
      a = rot + j * 2*PI / n;
      int x2 = SCR_WD/2 + cos(a) * r;
      int y2 = SCR_HT/2 + sin(a) * r;
      lcd.drawLine(x1,y1, x2,y2, SET);
    }
  }
  lcd.display();
  delay(50);
}

