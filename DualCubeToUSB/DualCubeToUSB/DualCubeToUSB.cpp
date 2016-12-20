/*
 * DualCubeToUSB.cpp
 *
 * Created: 11/26/2014 5:32:42 PM
 *  Author: thad
 */ 

#include "USB Code/USBAPI.h"

#include "MainIncludes.h"
#include "USB Code\Platform.h"
#include <avr\interrupt.h>

#include "Controller Code/gamepad.h"
#include "Controller Code/n64.h"
#include "Controller Code/gamecube.h"
#include "Controller Code/gcn64_protocol.h"
#include <avr/wdt.h>
#include "USB Code/GamepadHID.h"

static GCpad *curGamepad = NULL;
static GCpad *curGamepad2 = NULL;

Gamepad_ gPad1;
Gamepad_ gPad2;

#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void init()
{
	sei();
}

void hardwareInit()
{
	DDRC = 0x00;
	PORTC = 0xff;
	DDRF = 0x00;
	PORTF = 0xff;
}

static void sendBlank(bool cont2)
{
	if (!cont2)
	{
		gPad1.setAxesBlank();
		gPad1.setButtonsBlank();
		gPad1.update();
		
	}
	else
	{
		gPad2.setAxesBlank();
		gPad2.setButtonsBlank();
		gPad2.update();
		
	}

}

static void sendGamecube(bool cont2)
{
	if (!cont2)
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
		gPad1.setButtons(buttons);
		gPad1.setAxes(Gamepad.packAxisPacket(x, y, cx,cy,lt,rt));
		gPad1.update();
	}
	else
	{
		unsigned char padData[8];
		curGamepad2->buildReport(padData, 1);
		
		int x = map(constrain(padData[0], 127 - LEFT_DIFF, 127 + LEFT_DIFF), 127 - LEFT_DIFF, 127+ LEFT_DIFF, 0, 1023);

		int y = map(constrain(padData[1], 127 - LEFT_DIFF, 127 + LEFT_DIFF), 127 - LEFT_DIFF, 127+ LEFT_DIFF, 0, 1023);
		int cx = map(constrain(padData[2],127 - RIGHT_DIFF, 127 + RIGHT_DIFF),127 - RIGHT_DIFF, 127 + RIGHT_DIFF, 0, 1023);
		int cy = map(constrain(padData[3],127 - RIGHT_DIFF, 127 + RIGHT_DIFF),127 - RIGHT_DIFF, 127 + RIGHT_DIFF, 0, 1023);
		int lt = map(constrain(padData[4], 245 - SLIDER_DIFF, 245), 245 - SLIDER_DIFF, 245, 0, 1023);
		int rt = map(constrain(padData[5], 245 - SLIDER_DIFF, 245), 245 - SLIDER_DIFF, 245, 0, 1023);
		
		uint32_t buttons = padData[7];
		buttons <<= 8;
		buttons |= padData[6];
		gPad2.setButtons(buttons);
		gPad2.setAxes(Gamepad.packAxisPacket(x, y, cx,cy,lt,rt));
		gPad2.update();
	}
	
}

static void sendN64(bool cont2)
{
	if (!cont2)
	{
		unsigned char padData[8];
		curGamepad->buildReport(padData, 1);
		int x = map(padData[0],0,255,0,1023);
		int y = map(padData[1],0,255,0,1023);
		uint32_t buttons = padData[7];
		buttons <<= 8;
		buttons |= padData[6];
		gPad1.setButtons(buttons);
		gPad1.setAxes(Gamepad.packAxisPacket(x, y, 512,512,512,512));
		gPad1.update();
	}
	
	else
	{
		//Set 2nd here
		unsigned char padData[8];
		curGamepad2->buildReport(padData, 1);
		int x = map(padData[0],0,255,0,1023);
		int y = map(padData[1],0,255,0,1023);
		uint32_t buttons = padData[7];
		buttons <<= 8;
		buttons |= padData[6];
		gPad2.setButtons(buttons);
		gPad2.setAxes(Gamepad.packAxisPacket(x, y, 512,512,512,512));
		gPad2.update();
	}
}

int error_count = 0;
int error_count2 = 0;
int enable2ndController;

void detectControllers();
void checkForNullControllers();
void updateControllers();

int main(void)
{
	
	init();
	
	DDRC &= ~(1<<1);
	PORTD |= (1<<1);
	
	
	
	
	//pinMode(2, INPUT_PULLUP);
	
	_delay_ms(20);
	
	enable2ndController = PIND & (1<<1);
	
	
	if (enable2ndController)
	{
		setDC(true);
	}
	else
	{
		setDC(false);
	}
	
	//Check for controller 2 before attaching
	//USBDevice.attach();
	
	
	sendBlank(false);
	
	
	//Only if controller 2 is enabled
	if (enable2ndController)
	{
		gPad2.setController2();
		sendBlank(true);
	}
	
	hardwareInit();

	_delay_ms(1000);
	
	if (enable2ndController)
	{
		gcn64protocol_hwinit2();
	}
	gcn64protocol_hwinit();

	wdt_enable(WDTO_2S);

	while (1)
	{
		wdt_reset();
		checkForNullControllers();
		detectControllers();
		updateControllers();
	}
	
	
}

void detectControllers()
{
	if (curGamepad == NULL)
	{
		int gcncont = gcn64_detectController();
		switch(gcncont)
		{
			case CONTROLLER_IS_N64:
			curGamepad = n64GetGamepad();
			curGamepad->init(false);
			error_count = 0;
			break;

			case CONTROLLER_IS_GC:
			curGamepad = gamecubeGetGamepad();
			curGamepad->init(false);
			error_count = 0;
			break;

		}
	}
	if (enable2ndController)
	{
		if (curGamepad2 == NULL)
		{
			int gcncont = gcn64_detectController2();
			switch(gcncont)
			{
				case CONTROLLER_IS_N64:
				curGamepad2 = n64GetGamepad();
				curGamepad2->init(true);
				error_count2 = 0;
				break;

				case CONTROLLER_IS_GC:
				curGamepad2 = gamecubeGetGamepad();
				curGamepad2->init(true);
				error_count2 = 0;
				break;
			}
		}
	}
}

void checkForNullControllers()
{
	if (error_count > 30)
	{
		curGamepad = NULL;
	}
	if (error_count2 > 30)
	{
		curGamepad2 = NULL;
	}
}

void updateControllers()
{
	if (curGamepad != NULL)
	{
		if (curGamepad->update(false)) {
			error_count++;
			} else {
			error_count = 0;
		}
		if (curGamepad->gamepad == 1)
		{
			sendGamecube(false);
		}
		else
		{
			sendN64(false);
		}
	}
	else
	{
		sendBlank(false);
	}
	
	if (curGamepad2 != NULL)
	{
		if (curGamepad2->update(true)) {
			error_count2++;
			} else {
			error_count2 = 0;
		}
		if (curGamepad2->gamepad == 1)
		{
			sendGamecube(true);
		}
		else
		{
			sendN64(true);
		}
	}
	else if (enable2ndController)
	{
		sendBlank(true);
	}
	
}