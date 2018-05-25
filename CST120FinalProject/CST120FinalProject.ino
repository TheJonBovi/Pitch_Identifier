/*********************************************************************
This is an example for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

This example is for a 128x32 size display using I2C to communicate
3 pins are required to interface (2 I2C and one reset)

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.  
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ffft.h>

// FFT Stuff:
#define  IR_AUDIO  0 // ADC channel to capture
//volatile long zero = 0;
volatile byte position = 0;
int16_t      capture[FFT_N];   /* Wave capture buffer */
complex_t    bfly_buff[FFT_N]; /* FFT buffer */
uint16_t     spectrum[FFT_N/2]; /* Spectrum output buffer */
// Only enough RAM for 64 sample buffer (32 outputs)
// In library file ffft.h, FFT_N must be defined as 64.
// If set to 128 this WILL crash and burn due to lack of RAM.

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2


#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B00000000, B11000000,
  B00000001, B11000000,
  B00000001, B11000000,
  B00000011, B11100000,
  B11110011, B11100000,
  B11111110, B11111000,
  B01111110, B11111111,
  B00110011, B10011111,
  B00011111, B11111100,
  B00001101, B01110000,
  B00011011, B10100000,
  B00111111, B11100000,
  B00111111, B11110000,
  B01111100, B11110000,
  B01110000, B01110000,
  B00000000, B00110000 };

#if (SSD1306_LCDHEIGHT != 32)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif


void setup()   
{
  // SETUP DISPLAY          
  Serial.begin(9600);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)

  // Display splash screen for 2 seconds then clear
  display.display();
  delay(2000);
  display.clearDisplay();
  display.display();
  

  // Show Frequency Text
  //show_text();

  adcInit(); 
}

void loop() 
{  
  fft_input(capture, bfly_buff);
  position = 0;  // Restart A/D
  fft_execute(bfly_buff);
  fft_output(bfly_buff, spectrum);
  
  show_text();

  delay(100);
}

void show_text(void) {
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.clearDisplay();
 // String full_line = spectrum[0]+spectrum[1]+spectrum[2]+spectrum[3];
  //display.println(full_line);
  display.print(spectrum[0]);display.print(" ");
  display.print(spectrum[1]);display.print(" ");
  display.print(spectrum[2]);display.print(" ");
  display.print(spectrum[3]);display.print(" ");
  display.print(spectrum[4]);display.print(" ");
  display.print(spectrum[5]);display.print(" ");
  display.print(spectrum[6]);display.println(" ");

  display.print(spectrum[7]);display.print(" ");
  display.print(spectrum[8]);display.print(" ");
  display.print(spectrum[9]);display.print(" ");
  display.print(spectrum[10]);display.print(" ");
  display.print(spectrum[11]);display.print(" ");
  display.print(spectrum[12]);display.print(" ");
  display.print(spectrum[13]);display.println(" ");
  
  display.print(spectrum[14]);display.print(" ");
  display.print(spectrum[15]);display.print(" ");
  display.print(spectrum[16]);display.print(" ");
  display.print(spectrum[17]);display.print(" ");
  display.print(spectrum[18]);display.print(" ");
  display.print(spectrum[19]);display.print(" ");
  display.print(spectrum[20]);display.println(" ");
  
  display.print(spectrum[21]);display.print(" ");
  display.print(spectrum[22]);display.print(" ");
  display.print(spectrum[23]);display.print(" ");
  display.print(spectrum[24]);display.print(" ");
  display.print(spectrum[25]);display.print(" ");
  display.print(spectrum[26]);display.print(" ");
  display.print(spectrum[27]);display.println(" ");
  
  //int x = 10; 
  //display.println("Frequency in hz is:"); 
  //display.x; 
  display.display();
  delay(1);
}

byte foo = 0;

// Free-running ADC fills capture buffer
ISR(ADC_vect)
{
  // Capture every third sample to get 192:1 prescaler on A/D
  // Equiv to 6400 Hz sampling...seems awfully slow, but makes for
  // more interesting display.
  if(++foo < 3) return;
  foo = 0;

  if(position >= FFT_N) return;
  
//  capture[position] = ADC + zero;
  capture[position] = ADC - 512; // Hardcoded zero calibration
  // Filter out low-level noise
  if((capture[position] >= -2) && (capture[position] <= 2))
    capture[position] = 0;

  position++;
}

void adcInit()
{
  // Free running ADC mode, f = ( 16MHz / prescaler ) / 13 cycles per conversion 
  ADMUX  = IR_AUDIO; // Analog channel selection.  Right-adjusted.  Using AREF pin tued to 3.3V regulator output
  ADCSRA = _BV(ADEN)  | // ADC enable
           _BV(ADSC)  | // ADC start
           _BV(ADATE) | // Auto trigger
           _BV(ADIE)  | // Interrupt enable
//           _BV(ADPS2) | _BV(ADPS1); // prescaler 64, then /3 in interrupt for ~ 6KHz sample rate
           _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0); // prescaler 128 : 9615 Hz - 150 Hz per 64 divisions, better for most music
  ADCSRB = 0;
  DIDR0 = 1 << 0;
  //TIMSK0 = 0;  
  sei(); // Enable interrupts
}
