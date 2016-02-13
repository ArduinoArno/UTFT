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
#include <Wire.h>
#include "RTClib.h"


// Declare which fonts we will be using
extern uint8_t SmallFont[];
extern uint8_t BigFont[];
extern uint8_t SevenSegNumFont[];
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
RTC_DS1307    RTC;

// Finally we set up multiButtons :)
multiButtons  multiButtons(&myGLCD, &myTouch);

//Prepare Globals
int butL1, butL2, butL3, butR1, butR2, butR3, butUp, butDn, pressed_button, curMin, curHr, curDay;
boolean default_colors = true, buttonState;
double dblTempSetPoint;
int fwPinAn = 0;

//Set up the interface
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

  //Draw the button sheet
  drawButtonSheet();

  //Set initial temparature
  tempSetPoint(21.5);

  //Initialize the Clock
  initClock();



}

void loop()
{

    //Update the Clock
    if (updateClock()==true){
      measureFWLevel();
    }

    //Handle touch events
    handleTouch();


    //Debug - > measure tank
    //measureFWLevel();  


    delay(1000);
}



void drawButtonSheet(){
//      int butL1, butL2, butL3, butR1, butR2, butR3, butUp, butDn, pressed_button;
    boolean default_colors = true, buttonState;
    butL1 = multiButtons.addButton( 10, 220, 300,  70, "Trap",0,"right",true,true,false);
    butL2 = multiButtons.addButton( 10, 310, 300,  70, "Ventilatie achter",0,"right",true,true,false);
    butR3 = multiButtons.addButton( 10, 400, 300,  70, "Warm water",0,"right",true,false,false);
    
    butR1 = multiButtons.addButton( 490, 220, 300,  70, "LPG",0,"left",true,true,true);
    butR2 = multiButtons.addButton( 490, 310, 300,  70, "Drinkwater",0,"left",true,true,true);
    butR3 = multiButtons.addButton( 490, 400, 300,  70, "Afvalwater",0,"left",false,true,true);

    butUp = multiButtons.addButton( 370, 220, 70,  70, "+",0,"center",false,false,false);
    butDn = multiButtons.addButton( 370, 400, 70,  70, "-",0,"center",false,false,false);

    
    multiButtons.drawButtons();
    multiButtons.disableButton(butR3,false);

    //Set initial measurements
     multiButtons.setPercentage(butR1,0);
     multiButtons.setPercentage(butR2,0);
     multiButtons.setPercentage(butR3,0);

}


void tempSetPoint(double dDelta){
    dblTempSetPoint = dblTempSetPoint + dDelta;
    myGLCD.printNumF(dblTempSetPoint, 1, CENTER, 340);
}


void initClock(){
    Wire.begin();
    RTC.begin();
    RTC.adjust(DateTime(__DATE__, __TIME__));
}

boolean updateClock(){
  DateTime now = RTC.now();
  boolean result = false;

  int perC = (1.66666 * now.second());
  Serial.println("percentage " + String(perC) + " based on " + String(now.second()));
//  multiButtons.setPercentage(butR1,perC);
  multiButtons.setPercentage(butR2,perC);
//  multiButtons.setPercentage(butR3,perC);
  
  if (curMin!=now.minute()){
    //Update the interface
    curMin=now.minute();
    myGLCD.setFont(SevenSegNumFont);
    myGLCD.printNumI(now.minute(), 720, 50, 2,0);
    result = true;
  }

  if (curHr!=now.hour()){
    //Update the interface
    curHr=now.minute();
    myGLCD.setFont(SevenSegNumFont);
    myGLCD.printNumI(now.hour(), 640, 50, 2,0);
  }

  return result;
}

int measureFWLevel(){
  int  raw = analogRead(fwPinAn); 
  int intLevel = 0;

  //Serial.println(raw);
  
  if (raw>1000){
    //Full
    intLevel=100;
  } else if (raw>33){
    intLevel=90;
  } else if (raw>16){
    intLevel=70;
  } else if (raw>10){
    intLevel=50;
  } else if (raw>8){
    intLevel=30;
  } else if (raw>5){
    intLevel=10;
  } else if (raw<=5){
    intLevel=0;
    multiButtons.disableState(butR2, true);
  }
  
  Serial.println(String(raw) + " -> " + String(intLevel));
  multiButtons.setPercentage(butR2, intLevel);
  return intLevel;

  
}


void handleTouch(){
  if (myTouch.dataAvailable() == true)
      {
        Serial.println("Screen touched");
        
        pressed_button = multiButtons.checkButtons();

        switch(pressed_button){
          case 1:
              //myGLCD.print("Button 1", CENTER, 30 );
          break;
          case 2:
              //myGLCD.print("Button 2", CENTER, 30 );
          break;
          case 3:
              //myGLCD.print("Button 3", CENTER, 30 );         
          break;
          case 4:
              //myGLCD.print("Button 4", CENTER, 30 );

          break;
          case 5:
              //myGLCD.print("Button 5", CENTER, 30 );
          break;
          case 6:
                tempSetPoint(0.5);          
          break;        
          case 7:
                tempSetPoint(-0.5);          
          break;        

          case -1:
                //myGLCD.print("Button 0", CENTER, 30 );
          break;
        }
    }
}


