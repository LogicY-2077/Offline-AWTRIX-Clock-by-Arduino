///////////clock/////////
//thanks to jarzebski, u can download ds1307 library from https://github.com/jarzebski/Arduino-DS1307
//a DS3231 is OK, too, u can also find the driver library in his repositories
#include <DS1307.h>
#include <Wire.h>

DS1307 myclock;
RTCDateTime dt;
//caution! the default assignment in the ds1307 library's example is "DS1307 clock", which may lead to a error called "'clock' redeclared as different kind of entity'"
//this is because ur using esp8266/esp32, where "clock is aready used as a system variable. So, I made this little change: "clock" ->"myclock"
//u can use any else word u like to define DS1307 except "clock" 

///////////LED Matrix//////////
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

//this is a font customized from offical font "picocpixel" by myself to imitate the original awtrix pattern.
//it's included the repository, after downloading the whole package, put the file "forAWTRIX.h" into "libraries\Adafruit_GFX_Library\Fonts" folder.
#include <Fonts/forAWTRIX.h>
#ifndef PSTR
 #define PSTR // Make Arduino Due happy
#endif

#define PIN 12//u can use ur own pin definition



// if there' s something wrong with ur ws2812 display, it might be the matrix setting mismatching(eg. position of the first pixel,the way wire goes through pixels)
//check lib "Adafruit_NeoMatrix" examples for help
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(32, 8, PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);


const uint16_t colors[] = {
  matrix.Color(0, 100 ,255), matrix.Color(100, 20, 80), matrix.Color(150, 200, 255) ,matrix.Color(40, 30, 50),matrix.Color(200, 255, 40),matrix.Color(255, 150, 80)};


void setup() {
  
////////////clock////////////
  // Initialize DS3231
  Serial.begin(19200);
  Serial.println("Initialize DS3231");
  myclock.begin();
  // Set sketch compiling time
  myclock.setDateTime(__DATE__, __TIME__);
  
///////////LED Matrix////////
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setFont(&forAWTRIX);
  matrix.setBrightness(60);
}


void loop() {
  ////////////clock////////////
  dt = myclock.getDateTime();
  int shi = dt.hour;
  int fen = dt.minute;
  int sec = dt.second;
  int dow = dt.dayOfWeek;
  int ri = dt.day;
  int yue = dt.month;

  int sw = sec / 3; 
  if (sw - 3* (sw / 3)!= 0)
  {
    int shi1 = shi / 10;
    int shi2 = shi - shi1 * 10;
    String hour1 = String(shi1,HEX);
    String hour2 = String(shi2,HEX);
    String hour = hour1 + hour2;
  
    int fen1 = fen / 10;
    int fen2 = fen - 10 * fen1;
  
    String minute1 = String(fen1);
    String minute2 = String(fen2);
    String minute = minute1 + minute2;
  
    ///////////LED Matrix//////////  
    matrix.fillScreen(0);
    matrix.setTextColor(colors[0]); 
     
    matrix.setCursor(8, 5);
    matrix.println(hour);
    
    matrix.setCursor(17, 5);
    matrix.print(minute);
  
    if (sec - 2 * (sec / 2) ==0)
    {
      matrix.setCursor(15, 5);
      matrix.setTextColor(colors[1]);  
      matrix.println(":");
    }
  }
  else
  {
    int ri1 = ri / 10;
    int ri2 = ri - ri1 * 10;
    String day1 = String(ri1,HEX);
    String day2 = String(ri2,HEX);
    String day = day1 + day2;   

    int yue1 = yue / 10;
    
    int yue2 = yue - yue1 * 10;
    String month1 = String(yue1,HEX);
    String month2 = String(yue2,HEX);

    ///////////LED Matrix//////////  
    matrix.fillScreen(0);
    matrix.setTextColor(colors[4]); 
    
    if (yue1 !=0)
    {
      matrix.setCursor(8, 5);
      matrix.println(month1);
    } 
    matrix.setCursor(11, 5);
    matrix.println(month2);
    matrix.drawPixel(15, 5- (sec - 3*(sec/3)),colors[5]);
    
    matrix.setCursor(17, 5);
    matrix.print(day);
  
  }

//////////for day of week display///////////
  for (int i = 1; i <=7; i++)
  {
    if (i+2 == dow)
    {
      matrix.drawLine(2 + (i-1)*4, 7, 4 + (i-1)*4, 7,colors[2]);
    }
    else
    {
      matrix.drawLine(2 + (i-1)*4, 7,  4 + (i-1)*4, 7, colors[3]);
    }
    
  }

  matrix.show();
  delay(1000);
}
