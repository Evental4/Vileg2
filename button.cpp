
#include "TXLib.h"

struct Button
{
 int x;
 int y;
 const char* name;
 string category;

 void draw()
    {
    txSetColor (TX_TRANSPARENT);
    txSetFillColor (TX_BLACK);
    Win32::RoundRect(txDC(),x+5,y,x+145 ,y+45,30,30);
    txSetColor (TX_BLACK,2);
    txSetFillColor (TX_WHITE);
    if (click())  txSetFillColor(RGB(52,41,143));
    Win32::RoundRect(txDC(),x,y,x+140 ,y+40,30,30);
    txSetColor (TX_BLACK);
    txSelectFont("Arial",24);
    txDrawText (x,y,x+140,y+40,name);
    }
    bool click()
    {
    return(txMouseButtons() == 1 &&
          txMouseX()>= x && txMouseX()<=x+145 &&
          txMouseY()>= y+5 && txMouseY()<=y+40);
    }


};


