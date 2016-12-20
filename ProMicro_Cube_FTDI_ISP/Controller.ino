#include <Arduino.h>

#include "n64.h"
#include "gamecube.h"
#include "gcn64_protocol.h"
#include <avr/wdt.h>
#include <GamepadHID.h>


static GCpad *curGamepad = NULL;

//#define MAX_REPORTS	2

static void hardwareInit(void)
{
	DDRC = 0x00;
	PORTC = 0xff;
}

static void sendBlank()
{
      Gamepad.setAxesBlank();
      Gamepad.setButtonsBlank();
      Gamepad.update();

}

static void sendGamecube()
{
	unsigned char padData[8];
	curGamepad->buildReport(padData, 1);
        
	int x = map(constrain(padData[0], 127 - LEFT_DIFF, 127 + LEFT_DIFF), 127 - LEFT_DIFF, 127+ LEFT_DIFF, 0, 1023);

        int y = map(constrain(padData[1], 127 - LEFT_DIFF, 127 + LEFT_DIFF), 127 - LEFT_DIFF, 127+ LEFT_DIFF, 0, 1023);
        int cx = map(constrain(padData[2],127 - RIGHT_DIFF, 127 + RIGHT_DIFF),127 - RIGHT_DIFF, 127 + RIGHT_DIFF, 0, 1023);
        int cy = map(constrain(padData[3],127 - RIGHT_DIFF, 127 + RIGHT_DIFF),127 - RIGHT_DIFF, 127 + RIGHT_DIFF, 0, 1023);
        int lt = map(constrain(padData[4], 245 - SLIDER_DIFF, 245), 245 - SLIDER_DIFF, 245, 0, 1023);
        int rt = map(constrain(padData[5], 245 - SLIDER_DIFF, 245), 245 - SLIDER_DIFF, 245, 0, 1023);
        
        uint32_t buttons = padData[7];
        buttons <<= 8;
        buttons |= padData[6];
        Gamepad.setButtons(buttons);
        Gamepad.setAxes(Gamepad.packAxisPacket(x, y, cx,cy,lt,rt));
        Gamepad.update();
        
}

static void sendN64()
{
	unsigned char padData[8];
	curGamepad->buildReport(padData, 1);
	int x = map(padData[0],0,255,0,1023);
	int y = map(padData[1],0,255,0,1023);
        uint32_t buttons = padData[7];
        buttons <<= 8;
        buttons |= padData[6];
        Gamepad.setAxes(Gamepad.packAxisPacket(x, y, 512,512,512,512));
        Gamepad.update();
}

int mainController(void)
{
  sendBlank();
        
	char must_report =0, first_run = 1;
	//unsigned char idleCounters[MAX_REPORTS];
	//memset(idleCounters, 0, MAX_REPORTS);

	int error_count = 0, i;

	hardwareInit(); 

        delay(3000);

	gcn64protocol_hwinit();

        wdt_enable(WDTO_2S);

	while (1)
	{
		wdt_reset();

		if (error_count > 30)
		{
			curGamepad = NULL;
		}
		if (curGamepad == NULL)
		{
			error_count = 0;

			sendBlank();
			delay(30);
                        int gcncont = gcn64_detectController();
			switch(gcncont)
			{
			case CONTROLLER_IS_N64:
				curGamepad = n64GetGamepad();
				curGamepad->init();
				error_count = 0;
				break;

			case CONTROLLER_IS_GC:
				curGamepad = gamecubeGetGamepad();
				curGamepad->init();
				error_count = 0;
				break;
                      
				// Unknown means weird reply from the controller
				// try the old, bruteforce approach. */
			case CONTROLLER_IS_UNKNOWN:
				/* Check for gamecube controller */
				curGamepad = gamecubeGetGamepad();
				curGamepad->init();
				if (curGamepad->probe()) {
					curGamepad = curGamepad;
					error_count = 0;
					goto detected;
				}


				delay(40);

				/* Check for n64 controller */
				curGamepad = n64GetGamepad();
				curGamepad->init();
				if (curGamepad->probe())  {
					curGamepad = curGamepad;
					error_count = 0;
					goto detected;
				}

				break;

			}
			continue;
		}
detected:
		wdt_reset();

		if (first_run) {
			if (curGamepad->update()) {
				error_count++;
			} else {
				error_count = 0;
			}
			first_run = 0;
		}
		if (!must_report)
		{
			if (curGamepad->update()) {
					error_count++;
				} else {
					error_count = 0;
				}
                                    //must_report =1;
				/* Check what will have to be reported */

				for (i=0; i<1; i++) {
	                if (curGamepad->changed(i+1)) {
                        must_report =1;
    	                //must_report |= (1<<i);
    
        	        }
				}

		}
		if(must_report)
		{
			for (i = 0; i < 1; i++)
			{
				if ((must_report & (1<<i)) == 0)
					continue;


				if (curGamepad->gamepad == 1)
				{
					sendGamecube();
				}
				else
				{
					sendN64();
				}
			}

			must_report = 0;
		}
	}
}
