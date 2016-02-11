// This code block is only needed to support multiple
// MCU architectures in a single sketch.
#if defined(__AVR__)
	#define imagedatatype  unsigned int
#elif defined(__PIC32MX__)
	#define imagedatatype  unsigned short
#elif defined(__arm__)
	#define imagedatatype  unsigned short
#endif
// End of multi-architecture block

#include <UTFT.h>
#include <UTouch.h>
#include "multiButtons.h"


// Declare which fonts we will be using
extern uint8_t SmallFont[];
extern uint8_t BigFont[];
extern uint8_t Dingbats1_XL[];

// Set up UTFT...
// Set the pins to the correct ones for your development board
// -----------------------------------------------------------
// Standard Arduino 2009/Uno/Leonardo shield   : NOT SUPPORTED DUE TO LACK OF MEMORY
// Standard Arduino Mega/Due shield            : <display model>,38,39,40,41
// CTE TFT LCD/SD Shield for Arduino Due       : <display model>,25,26,27,28
// Standard chipKit Uno32/uC32                 : <display model>,34,35,36,37
// Standard chipKit Max32                      : <display model>,82,83,84,85
// AquaLEDSource All in One Super Screw Shield : <display model>,82,83,84,85
// CC3200 LaunchPad (pins used in the examples): <display model>,15,18,11,32
//
// Remember to change the model parameter to suit your display module!
UTFT    myGLCD(SSD1963_800,38,39,40,41);

// Set up UTouch...
// Set the pins to the correct ones for your development board
// -----------------------------------------------------------
// Standard Arduino 2009/Uno/Leonardo shield   : NOT SUPPORTED DUE TO LACK OF MEMORY
// Standard Arduino Mega/Due shield            : 6,5,4,3,2
// CTE TFT LCD/SD Shield for Arduino Due       : 6,5,4,3,2
// Standard chipKit Uno32/uC32                 : 20,21,22,23,24
// Standard chipKit Max32                      : 62,63,64,65,66
// AquaLEDSource All in One Super Screw Shield : 62,63,64,65,66
// CC3200 LaunchPad (pins used in the examples): 31,13,19,28,17
//
UTouch        myTouch(6,5,4,3,2);

// Finally we set up multiButtons :)
multiButtons  multiButtons(&myGLCD, &myTouch);

void setup()
{

  Serial.begin(9600);
  
  myGLCD.InitLCD();
  myGLCD.clrScr();
  myGLCD.setFont(BigFont);

  myTouch.InitTouch();
  myTouch.setPrecision(PREC_MEDIUM);
  
  multiButtons.setTextFont(BigFont);
  multiButtons.setButtonColors(VGA_WHITE, VGA_GRAY, 0x0BB3, VGA_WHITE, 0x0BB3);

  Serial.println("init");

}

void loop()
{
    int butL1, butL2, butL3, butL4, butR1, butR2, butR3, butR4, pressed_button;
    boolean default_colors = true, buttonState;
    butL1 = multiButtons.addButton( 10, 160, 300,  70, "Trap",0,"right",true,true,false);
    butL2 = multiButtons.addButton( 10, 240, 300,  70, "Ventilatie achter",0,"right",true,true,false);
    butL3 = multiButtons.addButton( 10, 320, 300,  70, "Ventilatie dak",0,"right",true,false,false);
    butR4 = multiButtons.addButton( 10, 400, 300,  70, "Verwarming",0,"right",true,false,false);
    
    butR1 = multiButtons.addButton( 490, 160, 300,  70, "LPG",0,"left",true,true,true);
    butR2 = multiButtons.addButton( 490, 240, 300,  70, "Drinkwater",0,"left",true,true,true);
    butR3 = multiButtons.addButton( 490, 320, 300,  70, "Warm water",0,"left",true,false,false);
    butR4 = multiButtons.addButton( 490, 400, 300,  70, "Afvalwater",0,"left",false,true,true);
    multiButtons.drawButtons();
    multiButtons.disableButton(butR4,false);

    //Set initial measurements
     multiButtons.setPercentage(butR1,30);
     multiButtons.setPercentage(butR2,60);
     multiButtons.setPercentage(butR4,100);


//    myGLCD.print("You pressed:", CENTER, 10);
//    myGLCD.print("None", CENTER, 30 );
    while(1) 
    {
      if (myTouch.dataAvailable() == true)
      {
        Serial.println("Screen touched");
        
        pressed_button = multiButtons.checkButtons();

        switch(pressed_button){
          case 0:
              //myGLCD.print("Button 1", CENTER, 30 );
          break;
          case 1:
              //myGLCD.print("Button 2", CENTER, 30 );
          break;
          case 2:
              //myGLCD.print("Button 3", CENTER, 30 );         
          break;
          case 3:
              //myGLCD.print("Button 4", CENTER, 30 );

          break;
          case 4:
              //myGLCD.print("Button 5", CENTER, 30 );
          break;
          case 5:
                //myGLCD.print("Button 6", CENTER, 30 );
          break;        
          case -1:
                //myGLCD.print("Button 0", CENTER, 30 );
          break;
        }
       }
    }
}

