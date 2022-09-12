//****************RTC clock***************
//thanks to jarzebski, u can download ds1307 library from https://github.com/jarzebski/Arduino-DS1307
//a DS3231 is OK, too, u can also find the driver library in his repositories
#include <DS3231.h>
#include <Wire.h>

DS3231 myclock;
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
  matrix.Color(140, 250 , 255), //0 light blue for main texture
  matrix.Color(180, 90, 150),   //1 darker pink
  matrix.Color(240, 220, 255) , //2 lighter white for dow
  matrix.Color(90, 70, 100),    //3 darker white for dow
  matrix.Color(200, 255, 40),   //4 green for digit(calendar
  matrix.Color(255, 150, 0),    //5 orange for dot (calendar
  matrix.Color(255, 100, 180),  //6 lighter pink for dot
  matrix.Color(255, 240, 20),   //7 light yellow
  matrix.Color(80, 240, 0)      //8 pure green
};

int old[] = {9, 9, 9, 9, 9, 9};
int pos[] = {3, 7, 13, 17, 23, 27};
int frame_delay[] = {25, 25, 50, 50, 75, 75, 100, 1550};


//*************Aminations***************
static const uint16_t flash[] = 
{
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x5EFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x5EFF, 0x8AFF, 0x0000,   // 0x0010 (16) pixels
0x0000, 0x0000, 0x0000, 0xFFFF, 0x5EFF, 0x8AFF, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x5EFF, 0x8AFF, 0x0000, 0x0000, 0x0000,   // 0x0020 (32) pixels
0x0000, 0x0000, 0x5EFF, 0x5EFF, 0x5EFF, 0x5EFF, 0x8AFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x5EFF, 0x8AFF, 0x0000, 0x0000,   // 0x0030 (48) pixels
0x0000, 0x0000, 0xFFFF, 0x5EFF, 0x8AFF, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x5EFF, 0x8AFF, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0040 (64) pixels
};

static const uint16_t big_rain[] = 
{
0x0000, 0x0000, 0x0000, 0x07FF, 0x07FF, 0x07FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x07FF, 0x0000, 0x0000, 0x0000, 0x07FF, 0x0000,   // 0x0010 (16) pixels
0x0000, 0x07FF, 0x07FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x07FF, 0x07FF, 0x0000, 0x0000, 0x07FF, 0x07FF, 0x0000, 0x07FF, 0x07FF,   // 0x0020 (32) pixels
0x0000, 0x07FF, 0x07FF, 0x0000, 0x07FF, 0x07FF, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF,   // 0x0030 (48) pixels
0x0000, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0x0000,   // 0x0040 (64) pixels
};
static const uint16_t rainy_sun[] = 
{
0x0000, 0x0000, 0xFE60, 0xFE60, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFE60, 0x0000, 0x07FF, 0xFE60, 0x0000, 0x0000, 0x0000,   // 0x0010 (16) pixels
0x0000, 0xFE60, 0x07FF, 0x0000, 0x07FF, 0x07FF, 0x07FF, 0x0000, 0x07FF, 0x07FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x07FF,   // 0x0020 (32) pixels
0x07FF, 0x0000, 0x0000, 0x07FF, 0x07FF, 0x0000, 0x07FF, 0x0000, 0x0000, 0x07FF, 0x07FF, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000,   // 0x0030 (48) pixels
0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000,   // 0x0040 (64) pixels
};

static const uint16_t rainbow[] = 
{
0x4000, 0x0000, 0x7800, 0x7800, 0xF800, 0xF800, 0x0000, 0x0000, 0x0000, 0x4920, 0x79E0, 0x0000, 0x79E0, 0xFBE0, 0xFBE0, 0x0000,   // 0x0010 (16) pixels
0x4A40, 0x0000, 0x8C40, 0x8C40, 0x8C40, 0xFFA0, 0xFFA0, 0xFFA0, 0x19E0, 0x0000, 0x4400, 0x4400, 0x0000, 0x4400, 0x7FE0, 0x7FE0,   // 0x0020 (32) pixels
0x0206, 0x03ED, 0x0000, 0x03ED, 0x03ED, 0x07FA, 0x07FA, 0x07FA, 0x0000, 0x00A7, 0x014F, 0x0000, 0x014F, 0x02DF, 0x02DF, 0x0000,   // 0x0030 (48) pixels
0x0000, 0x1806, 0x0000, 0x380F, 0x801F, 0x801F, 0x801F, 0x0000, 0x3006, 0x0000, 0x780F, 0xF81F, 0xF81F, 0x0000, 0x0000, 0x0000,   // 0x0040 (64) pixels
};

static const uint16_t sun1[] = 
{
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFE00, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0010 (16) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0020 (32) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFE00,   // 0x0030 (48) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0040 (64) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFE00, 0xFE00, 0x0000, 0x0000,   // 0x0050 (80) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000,   // 0x0060 (96) pixels
0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFE00, 0x0000, 0xFE00, 0xFE00, 0xFE00, 0xFE00, 0x0000,   // 0x0070 (112) pixels
0xFE00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF,   // 0x0080 (128) pixels
0xFFFF, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFE00, 0xFE00, 0xFE00, 0xFE00, 0x0000,   // 0x0090 (144) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000,   // 0x00A0 (160) pixels
0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFE00, 0xFE00, 0x0000, 0x0000,   // 0x00B0 (176) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000,   // 0x00C0 (192) pixels
0xFFFF, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFE00, 0x0000, 0x0000, 0x0000, 0x0000, 0xFE00,   // 0x00D0 (208) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x00E0 (224) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFE00, 0x0000, 0x0000, 0x0000,   // 0x00F0 (240) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0100 (256) pixels
};

static const uint16_t sun2[] = 
{
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFE00, 0xFE00, 0x0000, 0x0000,   // 0x0010 (16) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0020 (32) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFE00,   // 0x0030 (48) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0040 (64) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFE00, 0x0000, 0x0000, 0xFE00, 0xFE00, 0x0000, 0x0000,   // 0x0050 (80) pixels
0xFE00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000,   // 0x0060 (96) pixels
0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFE00, 0xFE00, 0xFE00, 0xFE00, 0x0000,   // 0x0070 (112) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF,   // 0x0080 (128) pixels
0x0000, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0xFE00, 0x0000, 0x0000, 0xFE00, 0xFE00, 0xFE00, 0xFE00, 0x0000,   // 0x0090 (144) pixels
0xFE00, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0x0000,   // 0x00A0 (160) pixels
0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0xFE00, 0x0000, 0x0000, 0xFE00, 0xFE00, 0x0000, 0x0000,   // 0x00B0 (176) pixels
0xFE00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x00C0 (192) pixels
0xFFFF, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x00D0 (208) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x00E0 (224) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFE00, 0x0000, 0x0000, 0xFE00, 0x0000,   // 0x00F0 (240) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0100 (256) pixels
};

static const uint16_t sun3[] = 
{
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFE00, 0x0000, 0xFE00, 0xFE00, 0x0000, 0x0000,   // 0x0010 (16) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0020 (32) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFE00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFE00,   // 0x0030 (48) pixels
0xFE00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0040 (64) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFE00, 0x0000, 0x0000, 0xFE00, 0xFE00, 0x0000, 0x0000,   // 0x0050 (80) pixels
0xFE00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000,   // 0x0060 (96) pixels
0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFE00, 0xFE00, 0xFE00, 0xFE00, 0x0000,   // 0x0070 (112) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x06FF, 0x06FF, 0x0000,   // 0x0080 (128) pixels
0x06FF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0xFE00, 0x0000, 0x0000, 0xFE00, 0xFE00, 0xFE00, 0xFE00, 0x0000,   // 0x0090 (144) pixels
0xFE00, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0x06FF, 0x0000, 0x0000, 0x0000,   // 0x00A0 (160) pixels
0x0000, 0x06FF, 0x0000, 0x06FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFE00, 0x0000, 0x0000, 0xFE00, 0xFE00, 0x0000, 0x0000,   // 0x00B0 (176) pixels
0xFE00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x06FF, 0x0000, 0x0000, 0x0000,   // 0x00C0 (192) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFE00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x00D0 (208) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x06FF, 0x06FF, 0x06FF, 0x0000, 0x06FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x00E0 (224) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x06FF, 0x0000, 0x06FF, 0x0000, 0x0000, 0xFE00, 0x0000, 0xFE00, 0xFE00, 0x0000,   // 0x00F0 (240) pixels
0x0000, 0x0000, 0x06FF, 0x06FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0100 (256) pixels
};

static const uint16_t sun4[] = 
{
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFE00, 0xFE00, 0xFE00, 0xFE00, 0x0000, 0x0000,   // 0x0010 (16) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x06FF,   // 0x0020 (32) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFE00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFE00,   // 0x0030 (48) pixels
0xFE00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x06FF, 0x0000,   // 0x0040 (64) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFE00, 0xFE00, 0x0000, 0x0000, 0xFE00, 0xFE00, 0x0000, 0x0000,   // 0x0050 (80) pixels
0xFE00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000,   // 0x0060 (96) pixels
0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFE00, 0xFE00, 0xFE00, 0xFE00, 0x0000,   // 0x0070 (112) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x06FF, 0x06FF, 0x0000,   // 0x0080 (128) pixels
0x06FF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0xFE00, 0x0000, 0x0000, 0xFE00, 0xFE00, 0xFE00, 0xFE00, 0x0000,   // 0x0090 (144) pixels
0xFE00, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0x06FF, 0x0000, 0x0000, 0x0000,   // 0x00A0 (160) pixels
0x0000, 0x06FF, 0x06FF, 0x06FF, 0x06FF, 0x0000, 0x0000, 0x0000, 0x0000, 0xFE00, 0x0000, 0xFE00, 0xFE00, 0xFE00, 0x0000, 0x0000,   // 0x00B0 (176) pixels
0xFE00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x06FF, 0x0000, 0x0000, 0x0000,   // 0x00C0 (192) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x06FF, 0x0000, 0x0000, 0x0000, 0xFE00, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x00D0 (208) pixels
0xFE00, 0x0000, 0x0000, 0x0000, 0x06FF, 0x06FF, 0x06FF, 0x06FF, 0x06FF, 0x0000, 0x06FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x00E0 (224) pixels
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x06FF, 0x0000, 0x06FF, 0x0000, 0x0000, 0xFE00, 0x0000, 0xFE00, 0xFE00, 0x0000,   // 0x00F0 (240) pixels
0x0000, 0x0000, 0x06FF, 0x06FF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,   // 0x0100 (256) pixels
};

static const uint16_t thermo1[] = 
{
0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000,   // 0x0010 (16) pixels
0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000,   // 0x0020 (32) pixels
0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x063F, 0x063F, 0xFFFF, 0x0000,   // 0x0030 (48) pixels
0x0000, 0x0000, 0xFFFF, 0x063F, 0x063F, 0x063F, 0xFFFF, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000,   // 0x0040 (64) pixels
};

static const uint16_t thermo2[] = 
{
0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000,   // 0x0010 (16) pixels
0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000,   // 0x0020 (32) pixels
0x0000, 0x0000, 0x0000, 0xFFFF, 0xA7E0, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xA7E0, 0xA7E0, 0xFFFF, 0x0000,   // 0x0030 (48) pixels
0x0000, 0x0000, 0xFFFF, 0xA7E0, 0xA7E0, 0xA7E0, 0xFFFF, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000,   // 0x0040 (64) pixels
};

static const uint16_t thermo3[] = 
{
0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000,   // 0x0010 (16) pixels
0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFF80, 0xFFFF, 0x0000, 0x0000,   // 0x0020 (32) pixels
0x0000, 0x0000, 0x0000, 0xFFFF, 0xFF80, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFF80, 0xFF80, 0xFFFF, 0x0000,   // 0x0030 (48) pixels
0x0000, 0x0000, 0xFFFF, 0xFF80, 0xFF80, 0xFF80, 0xFFFF, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000,   // 0x0040 (64) pixels
};

static const uint16_t thermo4[] = 
{
0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000,   // 0x0010 (16) pixels
0x0000, 0x0000, 0x0000, 0xFFFF, 0xFDA0, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFDA0, 0xFFFF, 0x0000, 0x0000,   // 0x0020 (32) pixels
0x0000, 0x0000, 0x0000, 0xFFFF, 0xFDA0, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFDA0, 0xFDA0, 0xFFFF, 0x0000,   // 0x0030 (48) pixels
0x0000, 0x0000, 0xFFFF, 0xFDA0, 0xFDA0, 0xFDA0, 0xFFFF, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000,   // 0x0040 (64) pixels
};

static const uint16_t bili1[] = 
{
0x0000, 0x075F, 0x0000, 0x0000, 0x0000, 0x0000, 0x075F, 0x0000, 0x0000, 0x0000, 0x075F, 0x0000, 0x0000, 0x075F, 0x0000, 0x0000,   // 0x0010 (16) pixels
0x0000, 0x075F, 0x075F, 0x075F, 0x075F, 0x075F, 0x075F, 0x0000, 0x075F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x075F,   // 0x0020 (32) pixels
0x075F, 0x0000, 0x075F, 0x0000, 0x0000, 0x075F, 0x0000, 0x075F, 0x075F, 0x0000, 0x075F, 0x0000, 0x0000, 0x075F, 0x0000, 0x075F,   // 0x0030 (48) pixels
0x075F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x075F, 0x0000, 0x075F, 0x075F, 0x075F, 0x075F, 0x075F, 0x075F, 0x0000,   // 0x0040 (64) pixels
};

static const uint16_t bili2[] = 
{
0x0000, 0x075F, 0x0000, 0x0000, 0x0000, 0x0000, 0x075F, 0x0000, 0x0000, 0x0000, 0x075F, 0x0000, 0x0000, 0x075F, 0x0000, 0x0000,   // 0x0010 (16) pixels
0x0000, 0x075F, 0x075F, 0x075F, 0x075F, 0x075F, 0x075F, 0x0000, 0x075F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x075F,   // 0x0020 (32) pixels
0x075F, 0x0000, 0x0000, 0x0000, 0x0000, 0x075F, 0x0000, 0x075F, 0x075F, 0x0000, 0x075F, 0x0000, 0x0000, 0x075F, 0x0000, 0x075F,   // 0x0030 (48) pixels
0x075F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x075F, 0x0000, 0x075F, 0x075F, 0x075F, 0x075F, 0x075F, 0x075F, 0x0000,   // 0x0040 (64) pixels
};

static const uint16_t bili3[] = 
{
0x0000, 0x075F, 0x0000, 0x0000, 0x0000, 0x0000, 0x075F, 0x0000, 0x0000, 0x0000, 0x075F, 0x0000, 0x0000, 0x075F, 0x0000, 0x0000,   // 0x0010 (16) pixels
0x0000, 0x075F, 0x075F, 0x075F, 0x075F, 0x075F, 0x075F, 0x0000, 0x075F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x075F,   // 0x0020 (32) pixels
0x075F, 0x0000, 0x075F, 0x0000, 0x0000, 0x075F, 0x0000, 0x075F, 0x075F, 0x0000, 0x0000, 0x075F, 0x0000, 0x075F, 0x0000, 0x075F,   // 0x0030 (48) pixels
0x075F, 0x0000, 0x075F, 0x0000, 0x0000, 0x0000, 0x0000, 0x075F, 0x0000, 0x075F, 0x075F, 0x075F, 0x075F, 0x075F, 0x075F, 0x0000,   // 0x0040 (64) pixels
};

static const uint16_t drink1[] = 
{
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x075F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF,   // 0x0010 (16) pixels
0xFFFF, 0x075F, 0x075F, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x075F, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000,   // 0x0020 (32) pixels
0x0000, 0xFFFF, 0x075F, 0x075F, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0x075F, 0x075F, 0x075F, 0x075F, 0xFFFF, 0x0000,   // 0x0030 (48) pixels
0x0000, 0xFFFF, 0x075F, 0x075F, 0x075F, 0x075F, 0xFFFF, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000,   // 0x0040 (64) pixels
};

static const uint16_t drink2[] = 
{
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF,   // 0x0010 (16) pixels
0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x075F, 0x0000, 0x0000, 0x075F, 0xFFFF, 0x0000,   // 0x0020 (32) pixels
0x0000, 0xFFFF, 0x075F, 0x075F, 0x075F, 0x075F, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0x075F, 0x075F, 0x075F, 0x075F, 0xFFFF, 0x0000,   // 0x0030 (48) pixels
0x0000, 0xFFFF, 0x075F, 0x075F, 0x075F, 0x075F, 0xFFFF, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000,   // 0x0040 (64) pixels
};

static const uint16_t drink3[] = 
{
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x075F, 0xFFFF,   // 0x0010 (16) pixels
0xFFFF, 0x0000, 0x075F, 0x0000, 0x0000, 0x0000, 0x075F, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x075F, 0xFFFF, 0x0000,   // 0x0020 (32) pixels
0x0000, 0xFFFF, 0x0000, 0x075F, 0x075F, 0x075F, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0x075F, 0x075F, 0x075F, 0x075F, 0xFFFF, 0x0000,   // 0x0030 (48) pixels
0x0000, 0xFFFF, 0x075F, 0x075F, 0x075F, 0x075F, 0xFFFF, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000,   // 0x0040 (64) pixels
};

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
  matrix.setBrightness(25);
}

void fan()
{
  for (int i = 0; i>-64; i--)
  {
      matrix.fillScreen(0);
      matrix.setCursor(i, 5);
      matrix.setTextColor(colors[8]);
      matrix.print("Fan! Fan! Fan! Fan! Fan! Fan! Fan! Fan! Fan!");
      matrix.show();
      delay(200);
  }
}
void drink()
{
   matrix.fillScreen(0);
   matrix.drawRGBBitmap(0, 0, drink1, 8,  8);
   matrix.setCursor(12, 6);
   matrix.setTextColor(colors[7]);
   matrix.print("DRINK");
   matrix.show();
   delay(500);

   matrix.fillScreen(0);
   matrix.drawRGBBitmap(0, 0, drink2, 8,  8);
   matrix.setCursor(12, 6);
   matrix.setTextColor(colors[5]);
   matrix.print("DRINK");
   matrix.show();
   delay(500);  

   matrix.fillScreen(0);
   matrix.drawRGBBitmap(0, 0, drink3, 8,  8);
   matrix.setCursor(12, 6);
   matrix.setTextColor(colors[7]);
   matrix.print("DRINK");
   matrix.show();
   delay(500); 

   matrix.fillScreen(0);
   matrix.drawRGBBitmap(0, 0, drink2, 8,  8);
   matrix.setCursor(12, 6);
   matrix.setTextColor(colors[5]);
   matrix.print("DRINK");
   matrix.show();
   delay(500); 
}


void bilibili()
{
   matrix.fillScreen(0);
   matrix.drawRGBBitmap(0, 0, bili1,8,  8);
   matrix.setCursor(14, 5);
   matrix.setTextColor(colors[2]);
   matrix.drawLine(10, 7, 29, 7, colors[6]);
   matrix.print("329");
   matrix.show();
   delay(500);  

   matrix.fillScreen(0);
   matrix.drawRGBBitmap(0, 0, bili2,8,  8);
   matrix.setCursor(14, 5);
   matrix.setTextColor(colors[2]);
   matrix.drawLine(10, 7, 29, 7, colors[6]);
   matrix.print("329");
   matrix.show();
   delay(500);  

   matrix.fillScreen(0);
   matrix.drawRGBBitmap(0, 0, bili3,8,  8);
   matrix.setCursor(14, 5);
   matrix.setTextColor(colors[2]);
   matrix.drawLine(10, 7, 29, 7, colors[6]);
   matrix.print("329");
   matrix.show();
   delay(500);  
}
  
void temperature()
{
  for (int i = 0; i < 3; i++)
  {
    float temp = myclock.readTemperature();
    matrix.fillScreen(0);
    matrix.setCursor(12, 5);
    matrix.setTextColor(colors[2]);
    matrix.print(temp);
    matrix.drawRGBBitmap(0, 0, thermo1, 8,  8);
    matrix.drawLine(10, 7, 29, 7, colors[4]);
    matrix.show();
    delay(600);

    matrix.fillScreen(0);
    matrix.setCursor(12, 5);
    matrix.setTextColor(colors[2]);
    matrix.print(temp);
    matrix.drawRGBBitmap(0, 0, thermo2, 8,  8);
    matrix.drawLine(10, 7, 29, 7, colors[4]);
    matrix.show();
    delay(600);

    matrix.fillScreen(0);
    matrix.setCursor(12, 5);
    matrix.setTextColor(colors[2]);
    matrix.print(temp);
    matrix.drawRGBBitmap(0, 0, thermo3, 8,  8);
    matrix.drawLine(10, 7, 29, 7, colors[4]);
    matrix.show();
    delay(600);

    matrix.fillScreen(0);
    matrix.setCursor(12, 5);
    matrix.setTextColor(colors[2]);
    matrix.print(temp);
    matrix.drawRGBBitmap(0, 0, thermo4, 8,  8);
    matrix.drawLine(10, 7, 29, 7, colors[4]);
    matrix.show();
    delay(600);
  }
  
}

void time_digit_scrolling() {
  
  //***********RTC clock************
  
  dt = myclock.getDateTime();
  
  int hour = dt.hour;
  int minute = dt.minute;
  int second = dt.second;
  int dow = dt.dayOfWeek;

  int hour1 = hour / 10;
  int hour2 = hour % 10;
  
  int minute1 = minute / 10;
  int minute2 = minute % 10;
  
  int second1 = second / 10;
  int second2 = second % 10;

  int current[] = {hour1, hour2, minute1, minute2, second1, second2};
  //Serial.println(current[5]);

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

void date_and_time() 
{
  //***********RTC clock************
  dt = myclock.getDateTime();
  int shi = dt.hour;
  int fen = dt.minute;
  int sec = dt.second;
  int dow = dt.dayOfWeek;
  int ri = dt.day;
  int yue = dt.month;

  int sw = sec / 6;
  if (sw % 2 == 0)
  {
    int shi1 = shi / 10;
    int shi2 = shi - shi1 * 10;
    String hour1 = String(shi1, HEX);
    String hour2 = String(shi2, HEX);
    String hour = hour1 + hour2;

    int fen1 = fen / 10;
    int fen2 = fen - 10 * fen1;

    String minute1 = String(fen1);
    String minute2 = String(fen2);
    String minute = minute1 + minute2;

    //*************LED Matrix**************
    matrix.fillScreen(0);
    matrix.setTextColor(colors[0]);

    matrix.setCursor(7, 5);
    matrix.println(hour);

    matrix.setCursor(17, 5);
    matrix.print(minute);

    if (sec - 2 * (sec / 2) == 0)
    {
      matrix.setCursor(15, 5);
      matrix.setTextColor(colors[6]);
      matrix.println(":");
    }
  }
  else
  {
    int ri1 = ri / 10;
    int ri2 = ri - ri1 * 10;
    String day1 = String(ri1, HEX);
    String day2 = String(ri2, HEX);
    String day = day1 + day2;

    int yue1 = yue / 10;

    int yue2 = yue - yue1 * 10;
    String month1 = String(yue1, HEX);
    String month2 = String(yue2, HEX);

    //***************LED Matrix****************
    matrix.fillScreen(0);
    matrix.setTextColor(colors[4]);

    if (yue1 != 0)
    {
      matrix.setCursor(8, 5);
      matrix.println(month1);
    }
    matrix.setCursor(11, 5);
    matrix.println(month2);
    Serial.println(month2);
    matrix.drawPixel(15, 5 - (sec - 3 * (sec / 3)), colors[5]);

    matrix.setCursor(17, 5);
    matrix.print(day);

  }
   //***************for day of week display********************
  for (int i = 1; i <= 7; i++)
  {
    if (i == dow)
    {
      //if (sec - 2 * (sec / 2) ==0)
      //{
      matrix.drawLine(2 + (i - 1) * 4, 7, 4 + (i - 1) * 4, 7, colors[2]);
      //}
    }
    else
    {
      matrix.drawLine(2 + (i - 1) * 4, 7,  4 + (i - 1) * 4, 7, colors[3]);
    }
  }
  matrix.show();
  delay(1000);
}


void animation() {
  
    for (int i=0; i <4; i++)
    {
      matrix.fillScreen(0);
      matrix.setBrightness(25);
        matrix.drawRGBBitmap(8, 0,  flash, 8,  8);
        matrix.show();
        delay(100);
        matrix.setBrightness(10);
        matrix.show();
        delay(200);
    }
    matrix.setBrightness(25); 
    delay(200);
    
    for (int i=-8; i <9; i++)
    {
      matrix.fillScreen(0);
      matrix.drawRGBBitmap(3, i,  flash ,8,  8);
      matrix.show();
      delay(40);
    }

    delay(200);

    for (int i=-8; i <9; i++)
    {
      matrix.fillScreen(0);
      matrix.drawRGBBitmap(18, i,  flash ,8,  8);
      matrix.show();
      delay(40);
    }

    delay(200);

    for (int i=0; i <32; i++)
    {
      matrix.fillScreen(0);
      matrix.drawRGBBitmap(i, 0,  big_rain,8,  8);
      matrix.show();
      delay(125);
    }

    

    for (int i=31; i >=0; i--)
    {
      matrix.fillScreen(0);
      matrix.drawRGBBitmap(i, 0, rainy_sun,8,  8);
      matrix.show();
      delay(125);
    }


    for (int i=0; i <32; i++)
    {
      matrix.fillScreen(0);
      matrix.drawRGBBitmap(i, 0,  rainbow,8,  8);
      matrix.show();
      delay(80);
    }

    for (int i=0; i <2; i++)
    {
      matrix.fillScreen(0);
      matrix.drawRGBBitmap(0, 0, sun1,32,  8);
      matrix.show();
      delay(200);
      matrix.fillScreen(0);
      matrix.drawRGBBitmap(0, 0, sun2,32,  8);
      matrix.show();
      delay(200);
      matrix.fillScreen(0);
      matrix.drawRGBBitmap(0, 0, sun3,32,  8);
      matrix.show();
      delay(200);
      matrix.fillScreen(0);
      matrix.drawRGBBitmap(0, 0, sun4,32,  8);
      matrix.show();
      delay(500);
    }
    
    matrix.fillScreen(0);
    matrix.show();
    
}

void loop()
{
  for (int i = 0; i < 6; i++)
  {
    old[i] = 9;
  }

  dt = myclock.getDateTime();
  int minute_main = dt.minute;
  int hour_main = dt.hour;
  
  if (minute_main % 20 == 0)
  {
    animation();
  }

  if (minute_main % 5 <= 0)
  {
    for (int i = 0; i < 5; i++)
    {
      drink();   
    }
  }
  
  for (int i = 0; i < 12; i++)
  {
    date_and_time() ;   
  }
  
  temperature();
  
  for (int i = 0; i < 5; i++)
  {
    time_digit_scrolling();   
  }
  
  for (int i = 0; i < 5; i++)
  {
    bilibili();   
  }

  if ((hour_main == 3 || hour_main == 6)&(minute_main <=15))
  {
  fan();
  }
}