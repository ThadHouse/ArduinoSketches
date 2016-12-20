#include "GamepadHID.h"

#include "USBAPI.h"

Gamepad_ Gamepad;

Gamepad_::Gamepad_()
{
	gamepad.buttons = 0;
	gamepad.axes = 0;
	isController2 = false;
}

void Gamepad_::begin()
{
}

void Gamepad_::end()
{
}

uint64_t Gamepad_::packAxisPacket(int x, int y, int z, int rx, int ry, int rz)
{
	uint64_t result = rz;
	result = result << 10;
	result |= ry;
	result = result << 10;
	result |= rx;
	result = result << 10;
	result |= z;
	result = result << 10;
	result |= y;
	result = result << 10;
	result |= x;


	return result;
}

void Gamepad_::setController2()
{
	isController2 = true;
}

int Gamepad_::map8to10(int input)
{
	return map(input, 0,255,0,1023);
}

void Gamepad_::setAxes(uint64_t axes)
{
	gamepad.axes = axes;
}

void Gamepad_::setButtons(uint32_t buttons)
{
	gamepad.buttons = buttons;
}

void Gamepad_::update()
{
	if (!isController2)
	{
		HID_SendReport(1, &gamepad, sizeof(gamepad));
	}
	else
	{
		HID_SendReport(2, &gamepad, sizeof(gamepad));
	}
}

void Gamepad_::setAxesBlank()
{
	gamepad.axes = CENTER_ALL;
}

void Gamepad_::setButtonsBlank()
{
	gamepad.buttons = 0;
}

long Gamepad_::map(long x, long in_min, long in_max, long out_min, long out_max)
{
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/*

void Gamepad_::setAxis(int axis, unsigned int value)
{
	if (axis >= 0 && axis <= 5)
	{
		if (axis == 0)
		{
		gamepad.axes ^= 1 << ((9));
		}
		else
		{
		gamepad.axes ^= 1 << (19);
		}
		unsigned long long v = value & 0x3FF;
		v = v << (10 * axis);
		gamepad.axes |= v;
	}
}

void Gamepad_::setButton(int button, int value)
{
	if (button >= 0 && button <= 31)
	{
		bitWrite(gamepad.buttons, button, value);
	}
}

*/