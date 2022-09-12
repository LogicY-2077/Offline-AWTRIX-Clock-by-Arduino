//****************RTC clock***************
//thanks to jarzebski, u can download ds1307 library from https://github.com/jarzebski/Arduino-DS1307
//a DS3231 is OK, too, u can also find the driver library in his repositories
#include <DS1307.h>
#include <Wire.h>

DS1307 myclock;
RTCDateTime dt;
//caution! the default assignment in the ds1307 library's example is "DS1307 clock", which may lead to a error called "'clock' redeclared as different kind of entity'"
//this is because ur using esp8266/esp32, where "clock is aready used as a system variable. So, I made this little change: "clock" ->"myclock"
//u can use any else word u like to define DS1307 except "clock"


//*************LED Matrix***************
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

//this is a font customized from offical font "picocpixel" by myself to imitate the original awtrix pattern.
//it's included the repository, after downloading the whole package, put the file "forAWTRIX.h" into "libraries\Adafruit_GFX_Library\Fonts" folder.
#include <Fonts/forAWTRIX.h>
#ifndef PSTR
#define PSTR // Make Arduino Due happy
#endif

#define PIN 0//u can use ur own pin definition

// if there' s something wrong with ur ws2812 display, it might be the matrix setting mismatching(eg. position of the first pixel,the way wire goes through pixels)
//check lib "Adafruit_NeoMatrix" examples for help
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(32, 8, PIN,
                            NEO_MATRIX_TOP     + NEO_MATRIX_LEFT +
                            NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
                            NEO_GRB            + NEO_KHZ800);

const uint16_t colors[] = {
  matrix.Color(120, 230 , 255), 
  matrix.Color(125, 60, 110), 
  matrix.Color(200, 200, 255) , 
  matrix.Color(60, 40, 60), 
  matrix.Color(200, 255, 40), 
  matrix.Color(255, 150, 80),
  matrix.Color(220, 100, 180)
};

int old[] = {9, 9, 9, 9, 9, 9};
int pos[] = {3, 7, 13, 17, 23, 27};
int frame_delay[] = {25, 25, 50, 50, 75, 75, 100, 550};

void setup() {
  //**********Genegal************
  Serial.begin(9600);
  //Wire.begin(0, 5);//define SDA, SCL

  //**********RTC clock**********
  // Initialize DS3231
  Serial.println("Initialize DS3231");
  myclock.begin();
  // Set sketch compiling time
  myclock.setDateTime(__DATE__, __TIME__);

  //**********LED Matrix*********
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setFont(&forAWTRIX);
  matrix.setBrightness(30);

  
}
  


void loop() {
  
  //***********RTC clock************
  
  dt = myclock.getDateTime();
  
  int hour = dt.hour;
  int minute = dt.minute;
  int second = dt.second;
  int dow = dt.dayOfWeek;
  int day = dt.day;
  int month = dt.month;

  int hour1 = hour / 10;
  int hour2 = hour % 10;
  
  int minute1 = minute / 10;
  int minute2 = minute % 10;
  
  int second1 = second / 10;
  int second2 = second % 10;

  int current[] = {hour1, hour2, minute1, minute2, second1, second2};
  Serial.println(current[5]);

  //*************LED Matrix**************
  matrix.fillScreen(0);
  matrix.setTextColor(colors[0]);
    
  //draw each frame, total: 8 frames
  for (int frame = 0; frame < 8; frame++)
  {
    matrix.fillScreen(0);

    //draw time digit
    for (int i = 0; i < 6; i++)
    {
        if (current[i] == old[i])
        {
          matrix.setCursor(pos[i], 5);
          matrix.print(old[i]);
        }
        else
        {
          matrix.setCursor(pos[i], 6+frame);
          matrix.print(old[i]);
          matrix.setCursor(pos[i], -2+frame);
          matrix.print(current[i]);            
        }
     }

     //draw time separaion
     //matrix.setTextColor(colors[2]);
     matrix.drawPixel(11, 2,  colors[6]);
     matrix.drawPixel(11, 4,  colors[6]);
     matrix.drawPixel(21, 2,  colors[6]);
     matrix.drawPixel(21, 4,  colors[6]);
     
     

     //draw dow
     for (int i = 1; i <= 7; i++)
     {
       if (i == dow)
       {
         matrix.drawLine(3 + (i - 1) * 4, 7, 5 + (i - 1) * 4, 7, colors[2]);
       }
       else
       {
        matrix.drawLine(3 + (i - 1) * 4, 7,  5 + (i - 1) * 4, 7, colors[3]);
       }
     }
     matrix.show();
     delay(frame_delay[frame]);
  }

  for (int i = 0; i < 6; i++)
  {
    old[i] = current[i];
  }
}
  


  

 
