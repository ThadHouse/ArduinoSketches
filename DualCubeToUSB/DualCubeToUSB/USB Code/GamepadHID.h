#ifndef Gamepad_H
#define Gamepad_H

#include "..\MainIncludes.h"

//Gamepad
typedef struct
{
	uint32_t buttons;
	uint64_t axes;
} GamepadState;

#define CENTER_ALL 0x802008020080200ULL
#define AXIS_X 0
#define AXIS_Y 1
#define AXIS_A 2
#define AXIS_SLIDER 3
#define AXIS_THROTTLE 4
#define AXIS_RUDDER 5

class Gamepad_
{
	private:
		GamepadState gamepad;
		
		bool isController2;
	public:
		Gamepad_();
		void begin();
		void end();
		uint64_t packAxisPacket(int x, int y, int z, int rx, int ry, int rz);
		int map8to10(int input);
		void setAxes(uint64_t axes);
		void setButtons(uint32_t buttons);
		void update();
		
		void setController2();
		
		
		long map(long, long, long, long, long);
		
		
		//void setAxis(int axis, unsigned int value);
		
		void setAxesBlank();
		void setButtonsBlank();
		
		//void setButton(int button, int value);
};
extern Gamepad_ Gamepad;



#endif