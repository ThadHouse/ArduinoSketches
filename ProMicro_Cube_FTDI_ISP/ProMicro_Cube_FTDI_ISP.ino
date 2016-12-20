#include <Arduino.h>

#define BOARD_SELECT  2
#define FTDI_ISP      10

int main(void)
{
  init();
  
#if defined(USBCON)
  USBDevice.attach();
#endif

  pinMode(BOARD_SELECT, INPUT_PULLUP);
  pinMode(FTDI_ISP, INPUT_PULLUP);
  
  delay(50);
  
  if (digitalRead(BOARD_SELECT))
  {
    mainController();
  }
  else
  {
    if(digitalRead(FTDI_ISP))
    {
      mainFTDI();
    }
    else
    {
      mainISP();
    }
  }
}
