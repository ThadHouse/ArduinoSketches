#include "gamepad.h"
#include "gamecube.h"
#include "gcn64_protocol.h"


static void gamecubeInit(boolean isCont2);
static char gamecubeUpdate(boolean isCont2);
static char gamecubeChanged(int rid);
//static void gamecubeUpdate(GamepadState_t *padStss);


/* What was most recently read from the controller */
static unsigned char last_built_report[GCN64_REPORT_SIZE];

/* What was most recently sent to the host */
static unsigned char last_sent_report[GCN64_REPORT_SIZE];
static int centers[6] = {0,0,0,0,0,0};//Centers for the sticks




static int gc_analog_lr_disable = 0;

static void setCenters(boolean isCont2)
{ 
		
		int i = 0;
		unsigned char count;
	
		unsigned char tmp = GC_GETORIGIN;
		int tmpdata[6] = {0,0,0,0,0,0};
			
		if (isCont2)
		{
			count = gcn64_transaction2(&tmp, 1);
		}
		else
		{
			count = gcn64_transaction(&tmp, 1);
		}
		if (count == GC_GETORIGIN_REPLY_LENGTH)
		{
			for (i=0; i<8; i++) // X axis
			tmpdata[0] |= gcn64_workbuf[i+16] ? (0x80>>i) : 0;
			
			for (i=0; i<8; i++) // Y axis
			tmpdata[1] |= gcn64_workbuf[i+24] ? (0x80>>i) : 0;
			tmpdata[1] ^= 0xff;

			for (i=0; i<8; i++) // C X axis
				tmpdata[2] |= gcn64_workbuf[i+32] ? (0x80>>i) : 0;

			// Raph: October 2007. C stick vertical orientation 
			// was wrong. But no one complained...
			
		#if INVERTED_VERTICAL_C_STICK
			for (i=0; i<8; i++) // C Y axis
				tmpdata[3] |= gcn64_workbuf[i+40] ? (0x80>>i) : 0;	
		#else
			for (i=0; i<8; i++) // C Y axis
				tmpdata[3] |= gcn64_workbuf[i+40] ? 0 : (0x80>>i);	
		#endif

			for (i=0; i<8; i++) // Left btn value
				tmpdata[4] |= gcn64_workbuf[i+48] ? (0x80>>i) : 0;
			tmpdata[4] ^= 0xff;
			
			for (i=0; i<8; i++) // Right btn value
				tmpdata[5] |= gcn64_workbuf[i+56] ? (0x80>>i) : 0;	
			tmpdata[5] ^= 0xff;
			
			for (i = 0; i < 4; i++)
			{
				centers[i] = 127 - tmpdata[i] ;
				
			}
			for (i = 4; i < 6; i++)
			{
				centers[i] = SLIDER_END - tmpdata[i];
			}
		}
}

static void gamecubeInit(boolean isCont2)
{
	if (0 == gamecubeUpdate(isCont2)) {
		if (gcn64_workbuf[GC_BTN_L] && gcn64_workbuf[GC_BTN_R]) {
			gc_analog_lr_disable = 1;
		} else {
			gc_analog_lr_disable = 0;
		}
	}
	
}
unsigned char map(unsigned char x, unsigned char in_min, unsigned char in_max, unsigned char out_min, unsigned char out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

static void constrainAxis(unsigned char* data, boolean isCont2)
{
	setCenters(isCont2);
	int i;
	for (i = 0; i < 6; i++)
	{
		data[i] = data[i] + centers[i];
	}
}

static char gamecubeUpdate(boolean isCont2)
{
	int i;
	unsigned char tmp=0;
	unsigned char tmpdata[8];	
	unsigned char count;
	
	/* Get ID command.
	 * 
	 * If we don't do that, the wavebird does not work.  
	 */
	tmp = GC_GETID;
	
	if (isCont2)
	{
		count = gcn64_transaction2(&tmp, 1);
	}
	else
	{
		count = gcn64_transaction(&tmp, 1);
	}
	if (count != GC_GETID_REPLY_LENGTH) {
		return 1;
	}

	tmpdata[0] = GC_GETSTATUS1;
	tmpdata[1] = GC_GETSTATUS2;
	tmpdata[2] = GC_GETSTATUS3;

	if (isCont2)
	{
		count = gcn64_transaction2(tmpdata, 3);
	}
	else
	{
		count = gcn64_transaction(tmpdata, 3);
	}
	if (count != GC_GETSTATUS_REPLY_LENGTH) {
		return 1; // failure
	}
	
/*
	(Source: Nintendo Gamecube Controller Protocol
		updated 8th March 2004, by James.)

	Bit		Function
	0-2		Always 0 
	3		Start
	4		Y
	5		X
	6		B
	7		A
	8		Always 1
	9		L
	10		R
	11		Z
	12-15	Up,Down,Right,Left
	16-23	Joy X
	24-31	Joy Y
	32-39	C Joystick X
	40-47	C Joystick Y
	48-55	Left Btn Val
	56-63	Right Btn Val
 */
	
	/* Convert the one-byte-per-bit data generated
	 * by the assembler mess above to nicely packed
	 * binary data. */	
	memset(tmpdata, 0, sizeof(tmpdata));

	for (i=0; i<8; i++) // X axis
		tmpdata[0] |= gcn64_workbuf[i+16] ? (0x80>>i) : 0;
	
	for (i=0; i<8; i++) // Y axis
		tmpdata[1] |= gcn64_workbuf[i+24] ? (0x80>>i) : 0;
	tmpdata[1] ^= 0xff;

	for (i=0; i<8; i++) // C X axis
		tmpdata[2] |= gcn64_workbuf[i+32] ? (0x80>>i) : 0;

	// Raph: October 2007. C stick vertical orientation 
	// was wrong. But no one complained...
	
#if INVERTED_VERTICAL_C_STICK
	for (i=0; i<8; i++) // C Y axis
		tmpdata[3] |= gcn64_workbuf[i+40] ? (0x80>>i) : 0;	
#else
	for (i=0; i<8; i++) // C Y axis
		tmpdata[3] |= gcn64_workbuf[i+40] ? 0 : (0x80>>i);	
#endif

	for (i=0; i<8; i++) // Left btn value
		tmpdata[4] |= gcn64_workbuf[i+48] ? (0x80>>i) : 0;
	tmpdata[4] ^= 0xff;
	
	for (i=0; i<8; i++) // Right btn value
		tmpdata[5] |= gcn64_workbuf[i+56] ? (0x80>>i) : 0;	
	tmpdata[5] ^= 0xff;
        //i+3
	for (i=0; i<5; i++) // St Y X B A
		tmpdata[6] |= gcn64_workbuf[7-i] ? (0x01<<i) : 0;
	for (i=0; i<3; i++) // L R Z
		tmpdata[6] |= gcn64_workbuf[i+9] ? (0x20<<i) : 0;
	
	for (i=0; i<4; i++) // Up,Down,Right,Left
		tmpdata[7] |= gcn64_workbuf[i+12] ? (0x01<<i) : 0;
		
	constrainAxis(tmpdata, isCont2);
	// report id
	//last_built_report[0] = 1;

	memcpy(last_built_report, tmpdata, 6);

	if (gc_analog_lr_disable) {
		last_built_report[4] = 0x7f;
		last_built_report[5] = 0x7f;
	}
	last_built_report[6] = tmpdata[6];
	last_built_report[7] = tmpdata[7];

	return 0; // success
}

static char gamecubeProbe(boolean isCont2)
{
	if (0 == gamecubeUpdate(isCont2))
		return 1;

	return 0;
}

static char gamecubeChanged(int id)
{
	return memcmp(last_built_report, last_sent_report, GCN64_REPORT_SIZE);
}

static int gamecubeBuildReport(unsigned char *reportBuffer, int id)
{
	if (reportBuffer != NULL)
		memcpy(reportBuffer, last_built_report, GCN64_REPORT_SIZE);
	
	memcpy(last_sent_report, last_built_report, GCN64_REPORT_SIZE);	
	return GCN64_REPORT_SIZE;
}


GCpad GamecubeGamepad = {
	.init = gamecubeInit,
	.update = gamecubeUpdate,
	.changed = gamecubeChanged,
	.buildReport = gamecubeBuildReport,
	.probe = gamecubeProbe,
	.gamepad = 1,
};

GCpad *gamecubeGetGamepad()
{
	return &GamecubeGamepad;
}
