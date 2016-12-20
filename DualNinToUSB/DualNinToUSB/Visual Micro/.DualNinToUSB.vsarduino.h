/* 
	Editor: http://www.visualmicro.com
	        visual micro and the arduino ide ignore this code during compilation. this code is automatically maintained by visualmicro, manual changes to this file will be overwritten
	        the contents of the Visual Micro sketch sub folder can be deleted prior to publishing a project
	        all non-arduino files created by visual micro and all visual studio project or solution files can be freely deleted and are not required to compile a sketch (do not delete your own code!).
	        note: debugger breakpoints are stored in '.sln' or '.asln' files, knowledge of last uploaded breakpoints is stored in the upload.vmps.xml file. Both files are required to continue a previous debug session without needing to compile and upload again
	
	Hardware: SparkFun Pro Micro 5V/16MHz - Dual Nintendo USB, Platform=avr, Package=arduino
*/

#define __AVR_ATmega32u4__
#define __AVR_ATmega32U4__
#define ARDUINO 101
#define ARDUINO_MAIN
#define F_CPU 16000000L
#define __AVR__
#define __cplusplus
extern "C" void __cxa_pure_virtual() {;}

static void hardwareInit(void);
static void sendBlank(bool cont2);
static void sendGamecube(bool cont2);
static void sendN64(bool cont2);
int main(void);
void detectControllers();
void checkForNullControllers();
void updateControllers();

#include "C:\Program Files (x86)\Arduino\hardware\arduino\variants\promicro\pins_arduino.h" 
#include "C:\Program Files (x86)\Arduino\hardware\arduino\cores\arduino\arduino.h"
#include "C:\Users\thad\Documents\Atmel Studio\6.2\DualNinToUSB\DualNinToUSB\DualNinToUSB.ino"
#include "C:\Users\thad\Documents\Atmel Studio\6.2\DualNinToUSB\DualNinToUSB\MainBackup.h"
