
#include "TXLib.h"

struct Pictures
{
 int x;
 int y;
 string adress;
 HDC pic;
 int w_scr;
 int h_scr;
 int w;
 int h;
 bool visible;
 string category;

 void draw()
{
   if(visible)
  {
        Win32::TransparentBlt (txDC(),x ,y ,w_scr,h_scr,pic,0,0,w,h,TX_white);
  }
}

bool click()
    {
    return(txMouseButtons() == 1 &&
          txMouseX()>= x && txMouseX()<=x+w_scr &&
          txMouseY()>= y && txMouseY()<=y+h_scr);
    }
};


