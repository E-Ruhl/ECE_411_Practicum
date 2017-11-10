/***************************************************
This is a example sketch demonstrating the graphics
capabilities of the SSD1331 library  for the 0.96"
16-bit Color OLED with SSD1331 driver chip
Pick one up today in the adafruit shop!
------> http://www.adafruit.com/products/684
These displays use SPI to communicate, 4 or 5 pins are required to
interface
Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!
Written by Limor Fried/Ladyada for Adafruit Industries.
BSD license, all text above must be included in any redistribution
****************************************************/


/***************************************************************************
This is a library for the AMG88xx GridEYE 8x8 IR camera

This sketch tries to read the pixels from the sensor

Designed specifically to work with the Adafruit AMG88 breakout
----> http://www.adafruit.com/products/3538

These sensors use I2C to communicate. The device's I2C address is 0x69

Adafruit invests time and resources providing this open source code,
please support Adafruit andopen-source hardware by purchasing products
from Adafruit!

Written by Dean Miller for Adafruit Industries.
BSD license, all text above must be included in any redistribution
***************************************************************************/

// You can use any (4 or) 5 pins 
#define sclk 13
#define mosi 11
#define cs   10
#define rset 9
#define dc   8


// Color definitions
#define BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0  
#define WHITE           0xFFFF

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1331.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_AMG88xx.h>

Adafruit_AMG88xx amg;

// see if a bit code x86 proc code cant be used to create an array of defines or a #defined array, ie get colors to take up flash not dynamic
const uint16_t colors[] = {
    0x0000, 0x0801, 0x1002, 0x1803, 0x1803, 0x2004, 0x2805, 0x3006, 0x3006, 0x3807, 
    0x4008, 0x4809, 0x4809, 0x500A, 0x580B, 0x600C, 0x600C, 0x580C, 0x580C, 0x500C, 
    0x480C, 0x400C, 0x400C, 0x380C, 0x300C, 0x280C, 0x280C, 0x200C, 0x180C, 0x100C, 
    0x100C, 0x080C, 0x000C, 0x000C, 0x000D, 0x000E, 0x000E, 0x000F, 0x0010, 0x0011, 
    0x0011, 0x0012, 0x0013, 0x0014, 0x0014, 0x0015, 0x0016, 0x0017, 0x0017, 0x0018, 
    0x0019, 0x001A, 0x001A, 0x001B, 0x001C, 0x001D, 0x001D, 0x001E, 0x001F, 0x001F, 
    0x003F, 0x007F, 0x009F, 0x00DF, 0x00FF, 0x013F, 0x015F, 0x019F, 0x019F, 0x01BF, 
    0x01FF, 0x021F, 0x025F, 0x027F, 0x02BF, 0x02DF, 0x031F, 0x033F, 0x037F, 0x039F, 
    0x03FF, 0x045F, 0x04BF, 0x051F, 0x057F, 0x05DF, 0x063F, 0x069F, 0x06FF, 0x075F, 
    0x07BF, 0x07FF, 0x07FE, 0x07FC, 0x07FB, 0x07F9, 0x07F8, 0x07F6, 0x07F5, 0x07F3, 
    0x07F2, 0x07F0, 0x07EF, 0x07ED, 0x07EC, 0x07EA, 0x07E9, 0x07E7, 0x07E6, 0x07E4, 
    0x07E3, 0x07E1, 0x07E0, 0x0FE0, 0x1FE0, 0x27E0, 0x37E0, 0x3FE0, 0x4FE0, 0x57E0, 
    0x67E0, 0x6FE0, 0x7FE0, 0x87E0, 0x97E0, 0x9FE0, 0xAFE0, 0xB7E0, 0xC7E0, 0xCFE0, 
    0xDFE0, 0xE7E0, 0xF7E0, 0xFFE0, 0xFF80, 0xFF20, 0xFEC0, 0xFE60, 0xFE00, 0xFDA0, 
    0xFD40, 0xFCE0, 0xFC80, 0xFC20, 0xFBC0, 0xFB60, 0xFB00, 0xFAA0, 0xFA40, 0xF9E0, 
    0xF980, 0xF920, 0xF8C0, 0xF860, 0xF800, 0xF820, 0xF841, 0xF861, 0xF8A2, 0xF8C3, 
    0xF8E3, 0xF904, 0xF945, 0xF965, 0xF986, 0xF9A6, 0xF9E7, 0xFA08, 0xFA28, 0xFA49, 
    0xFA8A, 0xFAAA, 0xFACB, 0xFAEB, 0xFB2C, 0xFB4D, 0xFB6D, 0xFB8E, 0xFBCF, 0xFBEF, 
    0xFC10, 0xFC30, 0xFC71, 0xFCD3, 0xFD14, 0xFD34, 0xFD55, 0xFD75, 0xFDB6, 0xFDD7, 
    0xFDF7, 0xFE18, 0xFE59, 0xFE79, 0xFE9A, 0xFEBA, 0xFEFB, 0xFF1C, 0xFF3C, 0xFF5D, 
    0xFF9E, 0xFFBE, 0xFFDF, 0xFFFF,
};

float pixels[AMG88xx_PIXEL_ARRAY_SIZE];
//uint8_t delta[AMG88xx_PIXEL_ARRAY_SIZE];

// Option 1: use any pins but a little slower
Adafruit_SSD1331 display = Adafruit_SSD1331(cs, dc, mosi, sclk, rset);

void setup(void) {
  Serial.begin(9600);
  display.begin();

  Serial.println("init");
  display.fillScreen(BLACK);

  char* text = "if u can read this you dont need glasses";  
  testdrawtext(text, WHITE);

  display.fillScreen(BLACK);

  // optimized lines
  testfastlines(RED, BLUE);
  delay(500);
  
  testtriangles();
  delay(500);

  bool status;

  // default settings
  status = amg.begin();
  
  if (!status) 
  {
    //Serial.println("Could not find a valid AMG88xx sensor, check wiring!");
    while (1)
    {
      display.fillScreen(RED);
      delay(1000);
      display.fillScreen(BLACK);
      delay(1000);
    }
  }
  display.fillScreen(BLACK);
  delay(1000);

  for (int i = 0; i < 202; i++)
  {
    Serial.println(colors[i], HEX);
  }

  Serial.println("done");
  delay(1000);
}

void loop() {
  //read all the pixels
  amg.readPixels(pixels);

  for(int i = 0; i < 64; i++)
  {
    nearestNeighbor(i);
  }
  
  delay(100);
}

void nearestNeighbor(uint8_t index)
{
  float temp = pixels[index];
  uint8_t x = 2 * (index / 8)+40;
  uint8_t y = 2 * (index % 8)+24;
  uint8_t i;
  if (index != 63)
  {
    if (index % 8 == 7)
    {
      float temp2 = pixels[index+8];
      float midTemp = average(temp, temp2, 2);
      i = midTemp*2 + 41;
      display.drawPixel(x, y + 1, colors[i]);
    }
    else if (index >= 56 && index <= 62)
    {
      float temp2 = pixels[index+1];
      float midTemp = average(temp, temp2, 2);
      i = midTemp*2 + 41;
      display.drawPixel(x + 1, y, colors[i]);
    }
    else
    {
      float total = temp;
      
      float corner = pixels[index+1];
      float mid = average(temp, corner, 2);
      i = mid*2 + 41;
      display.drawPixel(x+1, y, colors[i]);
      total += mid + corner;
      
      float corner2 = pixels[index * 8 + 1];
      mid = average(corner, corner2, 2);
      total += mid + corner2;
      
      corner = pixels[index * 8];
      mid = average(corner, corner2, 2);
      total += mid + corner;

      mid = average(temp, corner, 2);
      i = mid*2 + 41;
      display.drawPixel(x, y+1, colors[i]);
      total += mid;
      
      mid = average(total, 0, 8);
      i = mid*2 + 41;
      display.drawPixel(x+1, y+1, colors[i]);
    }
  }
  i = temp*2 + 41;
  display.drawPixel(x, y, colors[i]);
}

float average(float a, float b, float d)
{
  float ret = (a + b) / d;
  //float dec = ret % 1.0f;
  float dec = fmod(ret, 1.0f);
  ret -= dec;
  if (dec >= .25f && dec < .75f)
  {
    ret += .5f;
  }
  else if (dec >= .75f)
  {
    ret = ret + 1;
  }
  return ret;
}

void testdrawtext(char *text, uint16_t color) 
{
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);

  for (uint8_t i = 0; i < 168; i++) 
  {
    if (i == '\n') 
    {
      continue;
    }
    display.write(i);
    if ((i > 0) && (i % 21 == 0))
    {
      display.println();
    }
  }
}

void testfastlines(uint16_t color1, uint16_t color2) 
{
  display.fillScreen(BLACK);
  for (int16_t y = 0; y < display.height() - 1; y += 5) 
  {
    display.drawFastHLine(0, y, display.width() - 1, color1);
  }
  for (int16_t x = 0; x < display.width() - 1; x += 5) 
  {
    display.drawFastVLine(x, 0, display.height() - 1, color2);
  }
}

void testtriangles() 
{
  display.fillScreen(BLACK);
  int color = 0xF800;
  int t;
  int w = display.width() / 2;
  int x = display.height();
  int y = 0;
  int z = display.width();
  for (t = 0; t <= 15; t += 1) 
  {
    display.drawTriangle(w, y, y, x, z, x, color);
    x -= 4;
    y += 4;
    z -= 4;
    color += 100;
  }
}