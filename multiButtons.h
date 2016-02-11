
#ifndef UTFT_Buttons_h
#define UTFT_Buttons_h

#define UTFT_BUTTONS_VERSION  102

#if defined(__AVR__)
  #include "Arduino.h"
#elif defined(__PIC32MX__)
  #include "WProgram.h"
#elif defined(__arm__)
  #include "Arduino.h"
#endif

#include <UTFT.h>
#include <UTouch.h>
#if ((!defined(UTFT_VERSION)) || (UTFT_VERSION<241))
  #error : You will need UTFT v2.41 or higher to use this add-on library...
#endif


#define MAX_BUTTONS  12  // Maximum number of buttons available at one time
// Define presets for button status
#define BUTTON_DISABLED      0x0001
#define BUTTON_SYMBOL       0x0002
#define BUTTON_SYMBOL_REP_3X  0x0004
#define BUTTON_BITMAP       0x0008  
#define BUTTON_NO_BORDER    0x0010
#define BUTTON_UNUSED       0x8000

typedef struct
{
  uint16_t      pos_x, pos_y, width, height, percentage, cir_x,cir_y,cir_r, bar_x1,bar_y1,bar_x2,bar_y2;
  uint16_t      flags;
  char        *label, *orientation;
  boolean     state, showstate, showfillbar;
  long      bar_fact;
  bitmapdatatype  data;
} button_type;

class multiButtons{
  public:
    multiButtons(UTFT *ptrUTFT, UTouch *ptrUTouch);
    int   addButton(uint16_t x, uint16_t y, uint16_t width, uint16_t height, char *label, uint16_t flags, char *orientation, boolean state, boolean showstate, boolean showfillbar);
    int   addButton(uint16_t x, uint16_t y, uint16_t width, uint16_t height, bitmapdatatype data, uint16_t flags, char *orientation, boolean state, boolean showstate, boolean showfillbar);
    void  drawButtons();
    void  drawButton(int buttonID);
    void  enableButton(int buttonID, boolean redraw=false);
    void  disableButton(int buttonID, boolean redraw=false);
    void  enableState(int buttonID, boolean redraw=false);
    void  disableState(int buttonID, boolean redraw=false);
    void  setPercentage(int buttonID, int perCentage);
    boolean getState(int buttonID);
    void  relabelButton(int buttonID, char *label, boolean redraw=false);
    boolean buttonEnabled(int buttonID);
    void  deleteButton(int buttonID);
    void  deleteAllButtons();
    int   checkButtons();
    void  setTextFont(uint8_t* font);
    void  setSymbolFont(uint8_t* font);
    void  setButtonColors(word atxt, word iatxt, word brd, word brdhi, word back);
  protected:
    UTFT    *_UTFT;
    UTouch    *_UTouch;
    button_type buttons[MAX_BUTTONS];
    word    _color_text, _color_text_inactive, _color_background, _color_border, _color_hilite;
    uint8_t   *_font_text, *_font_symbol;
};


multiButtons::multiButtons(UTFT *ptrUTFT, UTouch *ptrUTouch)
{
  _UTFT = ptrUTFT;
  _UTouch = ptrUTouch;
  deleteAllButtons();
  _color_text     = VGA_WHITE;
  _color_text_inactive    = VGA_GRAY;
  _color_background   = VGA_BLUE;
  _color_border     = VGA_WHITE;
  _color_hilite     = VGA_RED;
  _font_text      = NULL;
  _font_symbol      = NULL;
}


int multiButtons::addButton(uint16_t x, uint16_t y, uint16_t width, uint16_t height, char *label, uint16_t flags, char *orientation, boolean showstate, boolean state, boolean showfillbar)
{
  int btcnt = 0;
  
  while (((buttons[btcnt].flags & BUTTON_UNUSED) == 0) and (btcnt<MAX_BUTTONS))
    btcnt++;
  
  if (btcnt == MAX_BUTTONS)
    return -1;
  else
  {
    buttons[btcnt].pos_x        = x;
    buttons[btcnt].pos_y        = y;
    buttons[btcnt].width        = width;
    buttons[btcnt].height       = height;
    buttons[btcnt].flags        = flags;
    buttons[btcnt].label        = label;
    buttons[btcnt].data         = NULL;
    buttons[btcnt].orientation  = orientation;
    buttons[btcnt].state        = state;
    buttons[btcnt].showstate    = showstate;
    buttons[btcnt].showfillbar      = showfillbar;
    return btcnt;
  }
}

int multiButtons::addButton(uint16_t x, uint16_t y, uint16_t width, uint16_t height, bitmapdatatype data, uint16_t flags, char *orientation, boolean showstate, boolean state, boolean showfillbar)
{
  int btcnt = 0;
  
  while (((buttons[btcnt].flags & BUTTON_UNUSED) == 0) and (btcnt<MAX_BUTTONS))
    btcnt++;
  
  if (btcnt == MAX_BUTTONS)
    return -1;
  else
  {
    buttons[btcnt].pos_x        = x;
    buttons[btcnt].pos_y        = y;
    buttons[btcnt].width        = width;
    buttons[btcnt].height       = height;
    buttons[btcnt].flags        = flags | BUTTON_BITMAP;
    buttons[btcnt].label        = NULL;
    buttons[btcnt].data         = data;
    buttons[btcnt].orientation  = orientation;
    buttons[btcnt].state        = state;
    buttons[btcnt].showstate    = showstate;
    buttons[btcnt].showfillbar      = showfillbar;
    return btcnt;
  }
}


void multiButtons::drawButtons()
{
  for (int i=0;i<MAX_BUTTONS;i++)
  {
    if ((buttons[i].flags & BUTTON_UNUSED) == 0)
      drawButton(i);
  }
}

void multiButtons::drawButton(int buttonID)
{
  int   text_x, text_y;
  uint8_t *_font_current = _UTFT->getFont();;
  word  _current_color = _UTFT->getColor();
  word  _current_back  = _UTFT->getBackColor();

  if (buttons[buttonID].flags & BUTTON_BITMAP)
  {
    _UTFT->drawBitmap(buttons[buttonID].pos_x, buttons[buttonID].pos_y, buttons[buttonID].width, buttons[buttonID].height, buttons[buttonID].data);
    if (!(buttons[buttonID].flags & BUTTON_NO_BORDER))
    {
      if ((buttons[buttonID].flags & BUTTON_DISABLED))
        _UTFT->setColor(_color_text_inactive);
      else
        _UTFT->setColor(_color_border);
      _UTFT->drawRect(buttons[buttonID].pos_x, buttons[buttonID].pos_y, buttons[buttonID].pos_x+buttons[buttonID].width, buttons[buttonID].pos_y+buttons[buttonID].height);
    }
  }
  else
  {
    _UTFT->setColor(_color_background);
    _UTFT->fillRoundRect(buttons[buttonID].pos_x, buttons[buttonID].pos_y, buttons[buttonID].pos_x+buttons[buttonID].width, buttons[buttonID].pos_y+buttons[buttonID].height);
    _UTFT->setColor(_color_border);
    _UTFT->drawRoundRect(buttons[buttonID].pos_x, buttons[buttonID].pos_y, buttons[buttonID].pos_x+buttons[buttonID].width, buttons[buttonID].pos_y+buttons[buttonID].height);
    if (buttons[buttonID].flags & BUTTON_DISABLED)
      _UTFT->setColor(_color_text_inactive);
    else
      _UTFT->setColor(_color_text);
    if (buttons[buttonID].flags & BUTTON_SYMBOL)
    {
      _UTFT->setFont(_font_symbol);
      text_x = (buttons[buttonID].width/2) - (_UTFT->getFontXsize()/2) + buttons[buttonID].pos_x;
      text_y = (buttons[buttonID].height/2) - (_UTFT->getFontYsize()/2) + buttons[buttonID].pos_y;
    }
    else
    {
      _UTFT->setFont(_font_text);
      if (buttons[buttonID].orientation=="left"){
        //Right align
        text_x = ((buttons[buttonID].width) + buttons[buttonID].pos_x - ((strlen(buttons[buttonID].label) * _UTFT->getFontXsize()))) - 10 ;
        text_y = (buttons[buttonID].height*0.3) - (_UTFT->getFontYsize()/2) + buttons[buttonID].pos_y;
      } else if (buttons[buttonID].orientation="right"){
        //Left align
        text_x = 10 + buttons[buttonID].pos_x;
        text_y = (buttons[buttonID].height*0.3) - (_UTFT->getFontYsize()/2) + buttons[buttonID].pos_y;
      } else {
        //Center
        text_x = ((buttons[buttonID].width/2) - ((strlen(buttons[buttonID].label) * _UTFT->getFontXsize())/2)) + buttons[buttonID].pos_x;
        text_y = (buttons[buttonID].height*0.3) - (_UTFT->getFontYsize()/2) + buttons[buttonID].pos_y;
      }
    }

    if (buttons[buttonID].showstate && buttons[buttonID].cir_r==0){
       buttons[buttonID].cir_r = buttons[buttonID].height/7;
       buttons[buttonID].cir_y = (buttons[buttonID].height*0.7) + buttons[buttonID].pos_y;
       if (buttons[buttonID].orientation=="left"){
        buttons[buttonID].cir_x = buttons[buttonID].pos_x + buttons[buttonID].cir_r/2 + (buttons[buttonID].width*0.05);
       } else if (buttons[buttonID].orientation="right"){
        buttons[buttonID].cir_x = buttons[buttonID].pos_x + buttons[buttonID].cir_r/2 + (buttons[buttonID].width*0.92);
       } else {
        buttons[buttonID].cir_x = buttons[buttonID].pos_x + (buttons[buttonID].width/2) + buttons[buttonID].cir_r/2;
       };
       _UTFT->drawCircle(buttons[buttonID].cir_x,buttons[buttonID].cir_y,buttons[buttonID].cir_r+1);
    };

    if (buttons[buttonID].showstate && buttons[buttonID].cir_r!=0){
        if (buttons[buttonID].state){
            _UTFT->setColor(VGA_GREEN);  
        } else {
            _UTFT->setColor(VGA_RED);
        }
        _UTFT->fillCircle(buttons[buttonID].cir_x,buttons[buttonID].cir_y,buttons[buttonID].cir_r);
        _UTFT->setColor(_current_color);
     }

    if (buttons[buttonID].showfillbar){
      uint16_t bar_h = buttons[buttonID].height/7;      
      uint16_t bar_w = buttons[buttonID].width * 0.8;      
      buttons[buttonID].bar_y1 = (buttons[buttonID].height*0.7) + buttons[buttonID].pos_y - (bar_h/2);
      if (buttons[buttonID].orientation=="left"){
        buttons[buttonID].bar_x1 = buttons[buttonID].pos_x + (buttons[buttonID].width*0.17);
      } else if (buttons[buttonID].orientation="right"){
        buttons[buttonID].bar_x1 = buttons[buttonID].pos_x  + (buttons[buttonID].width*0.03);
      }
      buttons[buttonID].bar_x2 = (buttons[buttonID].bar_x1 + bar_w);
      buttons[buttonID].bar_y2 = (buttons[buttonID].bar_y1 + bar_h);
      buttons[buttonID].bar_fact= (buttons[buttonID].bar_x2 - buttons[buttonID].bar_x1);
      Serial.println("factor (" + String(buttons[buttonID].bar_x2)  + " - " +  String(buttons[buttonID].bar_x1) + ") = " + String(buttons[buttonID].bar_fact));
      
      _UTFT->drawRoundRect(buttons[buttonID].bar_x1,buttons[buttonID].bar_y1,buttons[buttonID].bar_x2,buttons[buttonID].bar_y2);
    }
    
    _UTFT->setBackColor(_color_background);
    _UTFT->print(buttons[buttonID].label, text_x, text_y);
    if ((buttons[buttonID].flags & BUTTON_SYMBOL) and (buttons[buttonID].flags & BUTTON_SYMBOL_REP_3X))
    {
      _UTFT->print(buttons[buttonID].label, text_x-_UTFT->getFontXsize(), text_y);
      _UTFT->print(buttons[buttonID].label, text_x+_UTFT->getFontXsize(), text_y);
    }
  }
  _UTFT->setFont(_font_current);
  _UTFT->setColor(_current_color);
  _UTFT->setBackColor(_current_back);
}

void multiButtons::enableButton(int buttonID, boolean redraw)
{
  if (!(buttons[buttonID].flags & BUTTON_UNUSED))
  {
    buttons[buttonID].flags = buttons[buttonID].flags & ~BUTTON_DISABLED;
    if (redraw)
      drawButton(buttonID);
  }
}

void multiButtons::disableButton(int buttonID, boolean redraw)
{
  if (!(buttons[buttonID].flags & BUTTON_UNUSED))
  {
    buttons[buttonID].flags = buttons[buttonID].flags | BUTTON_DISABLED;
    if (redraw)
      drawButton(buttonID);
  }
}

boolean multiButtons::getState(int buttonID){
     return buttons[buttonID].state;
}

void multiButtons::enableState(int buttonID, boolean redraw)
{
         word  _current_color = _UTFT->getColor();
        _UTFT->setColor(VGA_GREEN);  
        _UTFT->fillCircle(buttons[buttonID].cir_x,buttons[buttonID].cir_y,buttons[buttonID].cir_r);
        _UTFT->setColor(_current_color);
}

void multiButtons::disableState(int buttonID, boolean redraw)
{
          word  _current_color = _UTFT->getColor();
        _UTFT->setColor(VGA_RED);
        _UTFT->fillCircle(buttons[buttonID].cir_x,buttons[buttonID].cir_y,buttons[buttonID].cir_r);
        _UTFT->setColor(_current_color);
}

void multiButtons::setPercentage(int buttonID, int percentage)
{
  //Get current settings
  word  _current_color = _UTFT->getColor();

  if ( percentage!=100){
    uint16_t bar_fill = ((buttons[buttonID].bar_fact * percentage)/100);
    _UTFT->setColor(_current_color);
    _UTFT->fillRect((buttons[buttonID].bar_x1+1),(buttons[buttonID].bar_y1+1),(buttons[buttonID].bar_x1 + bar_fill-2),(buttons[buttonID].bar_y2-1));
    _UTFT->setColor(_color_background);
    _UTFT->fillRect((buttons[buttonID].bar_x1 + bar_fill),(buttons[buttonID].bar_y1+1),(buttons[buttonID].bar_x2 -1),(buttons[buttonID].bar_y2-1));
  } else {
     _UTFT->setColor(_current_color);
    _UTFT->fillRect((buttons[buttonID].bar_x1 + 1),(buttons[buttonID].bar_y1 + 1),(buttons[buttonID].bar_x2 -1 ),(buttons[buttonID].bar_y2-1));
  }
  //Reset stuff
  _UTFT->setColor(_current_color);
}

void multiButtons::relabelButton(int buttonID, char *label, boolean redraw)
{
  if (!(buttons[buttonID].flags & BUTTON_UNUSED))
  {
    buttons[buttonID].label = label;
    if (redraw)
      drawButton(buttonID);
  }
}

boolean multiButtons::buttonEnabled(int buttonID)
{
  return !(buttons[buttonID].flags & BUTTON_DISABLED);
}
    
void multiButtons::deleteButton(int buttonID)
{
  if (!(buttons[buttonID].flags & BUTTON_UNUSED))
    buttons[buttonID].flags = BUTTON_UNUSED;
}

void multiButtons::deleteAllButtons()
{
  for (int i=0;i<MAX_BUTTONS;i++)
  {
    buttons[i].pos_x=0;
    buttons[i].pos_y=0;
    buttons[i].width=0;
    buttons[i].height=0;
    buttons[i].flags=BUTTON_UNUSED;
    buttons[i].label="";
  }
}

int multiButtons::checkButtons()
{
    if (_UTouch->dataAvailable() == true)
    {
    _UTouch->read();
    int   result = -1;
    int   touch_x = _UTouch->getX();
    int   touch_y = _UTouch->getY();
    word  _current_color = _UTFT->getColor();
  
    for (int i=0;i<MAX_BUTTONS;i++)
    {
      if (((buttons[i].flags & BUTTON_UNUSED) == 0) and ((buttons[i].flags & BUTTON_DISABLED) == 0) and (result == -1))
      {
        if ((touch_x >= buttons[i].pos_x) and (touch_x <= (buttons[i].pos_x + buttons[i].width)) and (touch_y >= buttons[i].pos_y) and (touch_y <= (buttons[i].pos_y + buttons[i].height)))
        {
          result = i;

          if (buttons[result].showstate==true){
              buttons[result].state = !buttons[result].state;
              if (buttons[result].state==true){
                  _UTFT->setColor(VGA_GREEN);  
                  _UTFT->fillCircle(buttons[result].cir_x,buttons[result].cir_y,buttons[result].cir_r);
                  _UTFT->setColor(_current_color);
              } else {
                  _UTFT->setColor(VGA_RED);  
                  _UTFT->fillCircle(buttons[result].cir_x,buttons[result].cir_y,buttons[result].cir_r);
                  _UTFT->setColor(_current_color);
              }
           }          
        }        
      }
    }
    if (result != -1)
    {
      if (!(buttons[result].flags & BUTTON_NO_BORDER))
      {
        _UTFT->setColor(_color_hilite);
        if (buttons[result].flags & BUTTON_BITMAP)
          _UTFT->drawRect(buttons[result].pos_x, buttons[result].pos_y, buttons[result].pos_x+buttons[result].width, buttons[result].pos_y+buttons[result].height);
        else
          _UTFT->drawRoundRect(buttons[result].pos_x, buttons[result].pos_y, buttons[result].pos_x+buttons[result].width, buttons[result].pos_y+buttons[result].height);
      }
    }

    while (_UTouch->dataAvailable() == true) {};

    if (result != -1)
    {
      if (!(buttons[result].flags & BUTTON_NO_BORDER))
      {
        _UTFT->setColor(_color_border);
        if (buttons[result].flags & BUTTON_BITMAP)
          _UTFT->drawRect(buttons[result].pos_x, buttons[result].pos_y, buttons[result].pos_x+buttons[result].width, buttons[result].pos_y+buttons[result].height);
        else
          _UTFT->drawRoundRect(buttons[result].pos_x, buttons[result].pos_y, buttons[result].pos_x+buttons[result].width, buttons[result].pos_y+buttons[result].height);
      }
    }

    _UTFT->setColor(_current_color);
    return result;
  }
  else
    return -1;
}

void multiButtons::setTextFont(uint8_t* font)
{
  _font_text = font;
}

void multiButtons::setSymbolFont(uint8_t* font)
{
  _font_symbol = font;
}

void multiButtons::setButtonColors(word atxt, word iatxt, word brd, word brdhi, word back)
{
  _color_text       = atxt;
  _color_text_inactive  = iatxt;
  _color_background   = back;
  _color_border     = brd;
  _color_hilite     = brdhi;
}

#endif
