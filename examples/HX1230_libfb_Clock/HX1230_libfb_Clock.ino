// HX1230 96x68 LCD FB library example
// Analog Clock without floating-point calculations, no slow sin, cos
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

#include "PropFonts/small4x6_font.h"

// configuration

int cx = SCR_WD/2, cy = SCR_HT/2;  // clock center
int rline0 = 34, rline1 = 30, rdot = 33, rnum = 25; // radius for hour lines, minute dots and numbers
int rhr = 16, rmin = 20, rsec = 25;                 // hour, minute, second hand length
int hrNum = 3;                                      // hour numbers - 0=none, 3-only 12,3,6,9, 1-all
int qdots = 0;                                      // quadrant dots

/*
int cx = 27;
int cy = 27;
int rline0 = 27, rline1 = 24, rdot = 27, rnum = 20;
int rhr = 10, rmin = 14, rsec = 18;
int hrNum = 3;
int qdots = 0;
*/

uint8_t txt2num(const char* p) 
{
  return 10*(*p-'0') + *(p+1)-'0';
}

int sx,sy,mx,my,hx,hy;
int sdeg,mdeg,hdeg;
int osx,osy,omx,omy,ohx,ohy;
int xs,ys,xe,ye;
unsigned long styleTime, ms;
uint8_t hh = txt2num(__TIME__+0);
uint8_t mm = txt2num(__TIME__+3);
uint8_t ss = txt2num(__TIME__+6);
uint8_t start = 1;
int i;
char buf[20];

// ------------------------------------------------
#define MAXSIN 255
const uint8_t sinTab[91] PROGMEM = {
0,4,8,13,17,22,26,31,35,39,44,48,53,57,61,65,70,74,78,83,87,91,95,99,103,107,111,115,119,123,
127,131,135,138,142,146,149,153,156,160,163,167,170,173,177,180,183,186,189,192,195,198,200,203,206,208,211,213,216,218,
220,223,225,227,229,231,232,234,236,238,239,241,242,243,245,246,247,248,249,250,251,251,252,253,253,254,254,254,254,254,
255
};

int fastSin(int i)
{
  while(i<0) i+=360;
  while(i>=360) i-=360;
  if(i<90)  return(pgm_read_byte(&sinTab[i])); else
  if(i<180) return(pgm_read_byte(&sinTab[180-i])); else
  if(i<270) return(-pgm_read_byte(&sinTab[i-180])); else
            return(-pgm_read_byte(&sinTab[360-i]));
}

int fastCos(int i)
{
  return fastSin(i+90);
}

// ------------------------------------------------
void clockConst(void);
void clockUpdate(void);
// ------------------------------------------------

void setup(void) 
{
  Serial.begin(9600);
  pinMode(LCD_BL, OUTPUT);
  digitalWrite(LCD_BL, HIGH);
  lcd.init();
  lcd.setFont(Small4x6PL);

  //for(i=0; i<360; i+=4)  lcd.drawPixel(i/4,34-fastSin(i)*30/MAXSIN,1);
  //for(i=0; i<360; i+=4)  lcd.drawPixel(i/4,34-fastCos(i)*30/MAXSIN,1);
  //lcd.display();  delay(10000);
  /*
  for(i=0; i<360; i++) {
    if((i%30)==0) Serial.println();
    sx = sin(radians(i))*255;
    Serial.print((int)sx); Serial.print(",");
  }
  */
  clockConst();
  ms = styleTime = millis(); 
}

void loop() 
{
  nextClockStyle();
  clockUpdate();
}

int style=0;

void nextClockStyle()
{
  if(millis()-styleTime<10000) return;
  styleTime = millis(); 
  if(++style>8) style=0;
  switch(style) {
    default:
    case 0:  // default, only 12,3,6,9, no quadrant dots
      cx = SCR_WD/2, cy = SCR_HT/2;
      rline0 = 34; rline1 = 30; rdot = 33; rnum = 25;
      rhr = 16; rmin = 20; rsec = 25;
      hrNum = 3; qdots = 0;
      start = 1;
      break;
    case 1: // full option
      hrNum = 1; qdots = 1;
      break;
    case 2:
      qdots = 0; hrNum = 3;
      break;
    case 3: // no hour numbers
      hrNum = 0;
      break;
    case 4: // no minute dots
      hrNum = 3; rdot = 0;
      break;
    case 5: // only hour lines
      hrNum = 0; rdot = 0;
      break;
    case 6: // only minute dots and numbers
      hrNum = 3; rdot = 33; rline0 = 0;
      break;
    case 7: // smaller, no minute dots, with numbers
      cx = 25, cy = 25;
      rline0 = 26; rline1 = 23; rdot = 0; rnum = 17;
      rhr = 9; rmin = 13; rsec = 18;
      hrNum = 3; qdots = 0;
      start = 1;
      break;
    case 8: // smaller, minute dots, no hour lines and numbers
      cx = SCR_WD-20, cy = SCR_HT-20;
      rline0 = 20; rline1 = 17; rdot = 20; rnum = 0;
      rhr = 9; rmin = 12; rsec = 16;
      hrNum = 0; qdots = 0;
      start = 1;
      break;
  }
  clockConst();
}

void clockConst()
{
  lcd.cls();
   // 12 hour lines
  if(rline0>0)
  for(i=0; i<360; i+=30) {
    sx = fastCos(i-90);
    sy = fastSin(i-90);
    xs = cx+sx*rline0/MAXSIN;
    ys = cy+sy*rline0/MAXSIN;
    xe = cx+sx*rline1/MAXSIN;
    ye = cy+sy*rline1/MAXSIN;
    lcd.drawLine(xs, ys, xe, ye, 1);
  }

  // 60 second dots
  if(rdot>0)
  for(i=0; i<360; i+=6) {
    sx = fastCos(i-90);
    sy = fastSin(i-90);
    xs = cx+sx*rdot/MAXSIN;
    ys = cy+sy*rdot/MAXSIN;
    lcd.drawPixel(xs, ys, 1);
    
    // 4 quadrant dots
    if(qdots && i%90==0) lcd.fillCircle(xs, ys, 1, 1);
  }
}

void clockUpdate() 
{
  if(millis()-ms>=1000 || start) 
  {
    ms = millis();
    if(++ss>59) {
      ss=0;
      if(++mm>59) {
        mm=0;
        if(++hh>23) hh=0;
      }
    }

    sdeg = ss*6;
    mdeg = mm*6+sdeg/60;
    hdeg = hh*30+mdeg/12;
    hx = fastCos(hdeg-90);
    hy = fastSin(hdeg-90);
    mx = fastCos(mdeg-90);
    my = fastSin(mdeg-90);
    sx = fastCos(sdeg-90);
    sy = fastSin(sdeg-90);

    if(ss==0 || start) {
      start = 0;
      // clear hour and minute hand positions every minute
      lcd.drawLine(ohx,ohy, cx,cy, 0);
      ohx = cx+hx*rhr/MAXSIN;
      ohy = cy+hy*rhr/MAXSIN;
      lcd.drawLine(omx,omy, cx,cy, 0);
      omx = cx+mx*rmin/MAXSIN;
      omy = cy+my*rmin/MAXSIN;
    }

    // new hand positions
    lcd.drawLine(osx,osy, cx,cy, 0);
    osx = cx+sx*rsec/MAXSIN;
    osy = cy+sy*rsec/MAXSIN;
    lcd.drawLine(ohx,ohy, cx,cy, 1);
    lcd.drawLine(omx,omy, cx,cy, 1);
    lcd.drawLine(osx,osy, cx,cy, 1);
    lcd.fillCircle(cx,cy, 1, 1);

    if(hrNum>0)
    for(i=0; i<12; i+=hrNum) {  // +=3 for 12,3,6,9
      sx = fastCos(i*30-90);
      sy = fastSin(i*30-90);
      xs = cx+sx*rnum/MAXSIN;
      ys = cy+sy*rnum/MAXSIN;
      snprintf(buf,3,"%d",i==0?12:i);
      lcd.printStr(i==0?xs-3:xs-1,ys-3,buf);
    }
    //snprintf(buf,20,"%02d:%02d:%02d",hh,mm,ss);
    //lcd.fillRect(0,0,6*5+4,7,0); lcd.printStr(0,0,buf);
    lcd.display();
  }
}

