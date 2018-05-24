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

// Flag for audio input clipping
boolean clipping = 0;

// Storage to compare ADC values
byte newRead = 0;
byte prevRead = 0;

// Frequency calculation variables
unsigned int timer = 0;
unsigned int period;
int frequency;

void setup()   
{
  // SETUP DISPLAY          
  Serial.begin(9600);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)

  // Display splash screen for 1 second
  display.display();
  delay(2000);

  // Show Frequency Text
  show_text();


  // SETUP MICROPHONG ADC
  pinMode(13,OUTPUT);//led indicator pin
  
  cli();//disable interrupts
  
  //set up continuous sampling of analog pin 0
  
  //clear ADCSRA and ADCSRB registers
  ADCSRA = 0;
  ADCSRB = 0;
  
  ADMUX |= (1 << REFS0); //set reference voltage
  ADMUX |= (1 << ADLAR); //left align the ADC value- so we can read highest 8 bits from ADCH register only
  
  ADCSRA |= (1 << ADPS2) | (1 << ADPS0); //set ADC clock with 32 prescaler- 16mHz/32=500kHz
  ADCSRA |= (1 << ADATE); //enabble auto trigger
  ADCSRA |= (1 << ADIE); //enable interrupts when measurement complete
  ADCSRA |= (1 << ADEN); //enable ADC
  ADCSRA |= (1 << ADSC); //start ADC measurements
  
  sei();//enable interrupts
  
}

// INTERRUPT SERVICE ROUTINE FOR ADC READ
ISR(ADC_vect) 
{
  prevRead = newRead;//store previous value
  newRead = ADCH;//get value from A0

  //if increasing and crossing midpoint
  if (prevRead < 127 && newRead >=127)
  {
    period = timer;//get period
    timer = 0;//reset timer
  }
  
  if (newRead == 0 || newRead == 1023){//if clipping
    PORTB |= B00100000;//set pin 13 high- turn on clipping indicator led
    clipping = 1;//currently clipping
  }
  
  timer++;//increment timer at rate of 38.5kHz
}

void loop() 
{  
  if (clipping)
  {
    PORTB &= B11011111; //turn off clipping indicator led
    clipping = 0;
  }

   frequency = 38462/period;//timer rate/period
  
   // Print results
   show_text();

   delay(100);
}

void show_text(void) {
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10,0);
  display.clearDisplay();
  display.println(frequency);
  //int x = 10; 
  //display.println("Frequency in hz is:"); 
  //display.x; 
  display.display();
  delay(1);
}
