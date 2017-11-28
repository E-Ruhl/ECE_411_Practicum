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

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1331.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_AMG88xx.h>

// see if a bit code x86 proc code cant be used to create an array of defines or a #defined array, ie get colors to take up flash not dynamic
const uint16_t colors[] = {
  0x0000, 0x0801, 0x1002, 0x1803, 0x2004, 0x2805, 0x3006, 0x3807, 0x4008, 0x4809,
  0x500A, 0x580B, 0x600C, 0x580C, 0x500C, 0x480C, 0x400C, 0x380C, 0x300C, 0x280C,
  0x200C, 0x180C, 0x100C, 0x080C, 0x000C, 0x000D, 0x000E, 0x000F, 0x0010, 0x0011,
  0x0012, 0x0013, 0x0014, 0x0015, 0x0016, 0x0017, 0x0018, 0x0019, 0x001A, 0x001B,
  0x001C, 0x001D, 0x001E, 0x001F, 0x003F, 0x007F, 0x009F, 0x00DF, 0x00FF, 0x013F,
  0x015F, 0x019F, 0x01BF, 0x01FF, 0x021F, 0x025F, 0x027F, 0x02BF, 0x02DF, 0x031F,
  0x033F, 0x037F, 0x039F, 0x03FF, 0x045F, 0x051F, 0x057F, 0x05DF, 0x063F, 0x069F,
  0x06FF, 0x075F, 0x07BF, 0x07FF, 0x07FE, 0x07FC, 0x07FB, 0x07F9, 0x07F8, 0x07F6,
  0x07F5, 0x07F3, 0x07F2, 0x07F0, 0x07EF, 0x07ED, 0x07EC, 0x07EA, 0x07E9, 0x07E7,
  0x07E6, 0x07E4, 0x07E3, 0x07E1, 0x07E0, 0x0FE0, 0x1FE0, 0x27E0, 0x37E0, 0x3FE0,
  0x4FE0, 0x57E0, 0x67E0, 0x6FE0, 0x7FE0, 0x87E0, 0x97E0, 0x9FE0, 0xAFE0, 0xB7E0,
  0xC7E0, 0xCFE0, 0xDFE0, 0xE7E0, 0xF7E0, 0xFFE0, 0xFF80, 0xFF20, 0xFEC0, 0xFE60,
  0xFE00, 0xFDA0, 0xFD40, 0xFCE0, 0xFC80, 0xFC20, 0xFBC0, 0xFB60, 0xFB00, 0xFAA0,
  0xFA40, 0xF9E0, 0xF980, 0xF920, 0xF8C0, 0xF860, 0xF800, 0xF820, 0xF841, 0xF861,
  0xF8A2, 0xF8C3, 0xF8E3, 0xF904, 0xF945, 0xF965, 0xF986, 0xF9A6, 0xF9E7, 0xFA08,
  0xFA28, 0xFA49, 0xFA8A, 0xFAAA, 0xFACB, 0xFAEB, 0xFB2C, 0xFB4D, 0xFB6D, 0xFB8E,
  0xFBCF, 0xFBEF, 0xFC10, 0xFC30, 0xFC71, 0xFCD3, 0xFD14, 0xFD34, 0xFD55, 0xFD75,
  0xFDB6, 0xFDD7, 0xFDF7, 0xFE18, 0xFE59, 0xFE79, 0xFE9A, 0xFEBA, 0xFEFB, 0xFF1C,
  0xFF3C, 0xFF5D, 0xFF9E, 0xFFBE, 0xFFDF, 0xFFFF,
};

#define AMG_COLS 8
#define AMG_ROWS 8
#define INTERPOLATED_COLS 16
#define INTERPOLATED_ROWS 16
#define MINTEMP -20     //low range of the sensor (this will be blue on the screen)
#define MAXTEMP 80      //high range of the sensor (this will be red on the screen)
#define XOFF 40
#define YOFF 24

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
#define WHITE           0xFFFF

uint8_t get_point(int8_t *p, uint8_t rows, uint8_t cols, int8_t x, int8_t y);
void set_point(uint8_t *p, uint8_t rows, uint8_t cols, int8_t x, int8_t y, float f);
void get_adjacents_2d(int8_t *src, float *dest, uint8_t rows, uint8_t cols, int8_t x, int8_t y);
float cubicInterpolate(float p[], float x);
float bicubicInterpolate(float p[], float x, float y);
void interpolate_image(uint8_t *src, uint8_t src_rows, uint8_t src_cols,
                       uint8_t *dest, uint8_t dest_rows, uint8_t dest_cols);

int8_t pixels[AMG_COLS * AMG_ROWS];
Adafruit_AMG88xx amg;
unsigned long delayTime;
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

  //for (int i = 165; i < 170; i++)
  //{
    //Serial.println(colors[i], HEX);
    //display.fillScreen(colors[i]);
  //}

  //for (int i = 0; i < 200; i++)
  //{
    //display.fillScreen(colors[i]);
  //}

  Serial.println("done");
  delay(1000);
}

void loop() {
  //read all the pixels
  amg.readPixels(pixels);

  //Serial.print("[");
  //for(int i=1; i<=AMG88xx_PIXEL_ARRAY_SIZE; i++){
  //  Serial.print(pixels[i-1]);
  //  Serial.print(", ");
  //  if( i%8 == 0 ) Serial.println();
  //}
  //Serial.println("]");
  //Serial.println();  
  
  uint8_t dest_2d[INTERPOLATED_ROWS * INTERPOLATED_COLS];

  interpolate_image(pixels, AMG_ROWS, AMG_COLS, dest_2d, INTERPOLATED_ROWS, INTERPOLATED_COLS);

  //for (int i = 0; i < INTERPOLATED_ROWS; i++)
  //{
    //for (int j = 0; j < INTERPOLATED_COLS; j++)
    //{
      //Serial.print(dest_2d[i * INTERPOLATED_ROWS + j * INTERPOLATED_COLS]);
      //Serial.print(' ');
    //}
    //Serial.println("");
  //}

  drawpixels(dest_2d, INTERPOLATED_ROWS, INTERPOLATED_COLS);

  //delay(50);
}

/*---------------------------------------------------------
 * Function: drawpixels
 * Inputs:
 *      uint8_t *p:
 *      uint8_t rows:
 *      uint8_t cols:
 * Outputs:
 *      void (none)
 * Description:
 ---------------------------------------------------------*/
void drawpixels(uint8_t *p, uint8_t rows, uint8_t cols) {
  int colorTemp;
  for (int y = 0; y < rows; y++) {
    for (int x = 0; x < cols; x++) {
      uint8_t val = get_point(p, rows, cols, x, y);
      //if(val >= MAXTEMP) val = MAXTEMP;
      //else if(val <= MINTEMP) val = MINTEMP;
      
      //uint8_t colorIndex = map(val, MINTEMP, MAXTEMP, 40, 203);
      uint8_t colorIndex = 2*val + 41;
      display.drawPixel(x + XOFF, y + YOFF, colors[colorIndex]);
    }
  }
}

/*---------------------------------------------------------
 * Function: testdrawtext
 * Inputs:
 *      char *text:
 *      uint16_t color:
 * Outputs:
 *      void (none)
 * Description:
 ---------------------------------------------------------*/
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

/*---------------------------------------------------------
 * Function: testfastlines
 * Inputs:
 *      uint16_t:
 *      uint16_t:
 * Outputs:
 *      void (none)
 * Description:
 ---------------------------------------------------------*/
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

/*---------------------------------------------------------
 * Function: testtriangles
 * Inputs:
 *      (none)
 * Outputs:
 *      (void)
 * Description:
 ---------------------------------------------------------*/
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

/*---------------------------------------------------------
 * Function: get_point
 * Inputs:
 *      int8_t *p:
 *      uint8_t rows:
 *      uint8_t cols:
 *      uint8_t x:
 *      uint8_t y:
 * Outputs:
 *      uint8_t:
 * Description:
 ---------------------------------------------------------*/
uint8_t get_point(int8_t *p, uint8_t rows, uint8_t cols, int8_t x, int8_t y) {
  if (x < 0)        x = 0;
  if (y < 0)        y = 0;
  if (x >= cols)    x = cols - 1;
  if (y >= rows)    y = rows - 1;
  return p[y * cols + x];
}

/*---------------------------------------------------------
 * Function: set_point
 * Inputs:
 *      uint8_t *p:
 *      uint8_t rows:
 *      uint8_t cols:
 *      int8_t x:
 *      int8_t y:
 *      float f:
 * Outputs:
 *      void (none)
 * Description:
 ---------------------------------------------------------*/
void set_point(uint8_t *p, uint8_t rows, uint8_t cols, int8_t x, int8_t y, float f) {
  if ((x < 0) || (x >= cols)) return;
  if ((y < 0) || (y >= rows)) return;
  p[y * cols + x] = (uint8_t)f;
}

/*---------------------------------------------------------
 * Function: interpolate_image
 * Inputs:
 *      int8_t *src:
 *      uint8_t src_rows:
 *      uint8_t src_cols:
 *      uint8_t *dest:
 *      uint8_t dest_rows:
 *      uint8_t dest_cols:
 * Outputs:
 *      void (none)
 * Description:
 ---------------------------------------------------------*/
// src is a grid src_rows * src_cols
// dest is a pre-allocated grid, dest_rows*dest_cols
void interpolate_image(uint8_t *src, uint8_t src_rows, uint8_t src_cols, 
                       uint8_t *dest, uint8_t dest_rows, uint8_t dest_cols) {
  float mu_x = (src_cols - 1.0) / (dest_cols - 1.0);
  float mu_y = (src_rows - 1.0) / (dest_rows - 1.0);

  float adj_2d[16]; // matrix for storing adjacents
  
  for (uint8_t y_idx=0; y_idx < dest_rows; y_idx++) {
    for (uint8_t x_idx=0; x_idx < dest_cols; x_idx++) {
       float x = x_idx * mu_x;
       float y = y_idx * mu_y;
       //Serial.print("("); Serial.print(y_idx); Serial.print(", "); Serial.print(x_idx); Serial.print(") = ");
       //Serial.print("("); Serial.print(y); Serial.print(", "); Serial.print(x); Serial.print(") = ");
       get_adjacents_2d(src, adj_2d, src_rows, src_cols, x, y);
       
       //Serial.print("[");
       //for (uint8_t i=0; i<16; i++) {
         //Serial.print(adj_2d[i]); Serial.print(", ");
       //}
       //Serial.println("]");
       
       float frac_x = x - (int)x; // we only need the ~delta~ between the points
       float frac_y = y - (int)y; // we only need the ~delta~ between the points
       float out = bicubicInterpolate(adj_2d, frac_x, frac_y);
       //Serial.print("\tInterp: "); Serial.println(out);
       set_point(dest, dest_rows, dest_cols, x_idx, y_idx, out);
    }
  }
}

/*---------------------------------------------------------
 * Function: cubicInterpolate
 * Inputs:
 *      float p[]:
 *      float x:
 * Outputs:
 *      float:
 * Description:
 ---------------------------------------------------------*/
// p is a list of 4 points, 2 to the left, 2 to the right
float cubicInterpolate(float p[], float x) {
    float r = p[1] + (0.5 * x * (p[2] - p[0] + x*(2.0*p[0] - 5.0*p[1] + 4.0*p[2] - p[3] + x*(3.0*(p[1] - p[2]) + p[3] - p[0]))));
  /*
    Serial.print("interpolating: ["); 
    Serial.print(p[0],2); Serial.print(", ");
    Serial.print(p[1],2); Serial.print(", ");
    Serial.print(p[2],2); Serial.print(", ");
    Serial.print(p[3],2); Serial.print("] w/"); Serial.print(x); Serial.print(" = ");
    Serial.println(r);
  */
    return r;
}

/*---------------------------------------------------------
 * Function: bicubicInterpolate
 * Inputs:
 *      float p[]:
 *      float x:
 *      float y:
 * Outputs:
 *      float:
 * Description:
 ---------------------------------------------------------*/
// p is a 16-point 4x4 array of the 2 rows & columns left/right/above/below
float bicubicInterpolate(float p[], float x, float y) {
    float arr[4] = {0,0,0,0};
    arr[0] = cubicInterpolate(p+0, x);
    arr[1] = cubicInterpolate(p+4, x);
    arr[2] = cubicInterpolate(p+8, x);
    arr[3] = cubicInterpolate(p+12, x);
    return cubicInterpolate(arr, y);
}

/*---------------------------------------------------------
 * Function: get_adjacent_2d
 * Inputs:
 *      int8_t *src:
 *      float *dest:
 *      uint8_t rows:
 *      uint8_t cols:
 *      int8_t x:
 *      int8_t y:
 * Outputs:
 *      void (none)
 * Description:
 ---------------------------------------------------------*/
// src is rows*cols and dest is a 16-point array passed in already allocated!
void get_adjacents_2d(int8_t *src, float *dest, uint8_t rows, uint8_t cols, int8_t x, int8_t y) {
    Serial.print("("); Serial.print(x); Serial.print(", "); Serial.print(y); Serial.println(")");
    float arr[4];
    for (int8_t delta_y = -1; delta_y < 3; delta_y++) { // -1, 0, 1, 2
        float *row = dest + 4 * (delta_y+1); // index into each chunk of 4
        for (int8_t delta_x = -1; delta_x < 3; delta_x++) { // -1, 0, 1, 2
            row[delta_x+1] = get_point(src, rows, cols, x+delta_x, y+delta_y);
        }
    }
}
