
#include <n64.h>
#include <gcn64_protocol.h>
#include <gamepad.h>
#include <gamecube.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include <avr/wdt.h>


static Gamepad *curGamepad = NULL;
static unsigned char reportBuffer[GCN64_REPORT_SIZE];

#define MAX_REPORTS 2

static void gamepadVibrate(char on)
{
	if (curGamepad)
		if (curGamepad->setVibration)
			curGamepad->setVibration(on);
}

static int getGamepadReport(unsigned char *dstbuf, int id)
{
	if (curGamepad == NULL) {
		if (id==1) {
			dstbuf[0] = 1;
			dstbuf[1] = 0x7f;
			dstbuf[2] = 0x7f;
			dstbuf[3] = 0x7f;
			dstbuf[4] = 0x7f;
			dstbuf[5] = 0x7f;
			dstbuf[6] = 0x7f;
			dstbuf[7] = 0;
			dstbuf[8] = 0;

			return 9;
		}
		return 0;
	}
	else {
		return curGamepad->buildReport(dstbuf, id);
	}
}

void transferGamepadReport(int id)
{
	char len;
	len = getGamepadReport(reportBuffer, id);
	Serial.println(len);
	delay(50);
	//HID_SendReport(5, reportBuffer, len);
}


int main()
{
	char must_report = 0, first_run = 1;
	init();
#if defined(USBCON)
	USBDevice.attach();
#endif
	Serial.begin(9600);
	unsigned char   idleCounters[MAX_REPORTS];

	memset(idleCounters, 0, MAX_REPORTS);
	GCN64_DATA_DDR = 0x00;
	GCN64_DATA_PORT = 0xff;


	int error_count = 0, i;

	TCCR1A = (1<<WGM11);
	TCCR1B = (1<<CS12) | (1<<CS11) | (1<<CS10);
	OCR1A = 67;

#define mustPollControllers() (TIFR1 & (1<<OCF1A))
#define clrPollControllers() do { TIFR1 = 1<<OCF1A; } while (0)

	gcn64protocol_hwinit();

	wdt_enable(WDTO_2S);
	sei();


	while (1)
	{
		wdt_reset();
		if (error_count > 30)
		{
			curGamepad = NULL;
		}
		if (curGamepad == NULL)
		{
			gamepadVibrate(0);
			error_count = 0;

			transferGamepadReport(1);

			_delay_ms(30);


			switch(gcn64_detectController())
			{
			case CONTROLLER_IS_N64:
				curGamepad = n64GetGamepad();
				curGamepad->init();
				gamepadVibrate(0);
				error_count = 0;
				break;

			case CONTROLLER_IS_GC:
				curGamepad = gamecubeGetGamepad();
				curGamepad->init();
				gamepadVibrate(0);
				error_count = 0;
				break;

				// Unknown means weird reply from the controller
				// try the old, bruteforce approach.
			case CONTROLLER_IS_UNKNOWN:
				/* Check for gamecube controller */
				curGamepad = gamecubeGetGamepad();
				curGamepad->init();
				if (curGamepad->probe()) {
					curGamepad = curGamepad;
					gamepadVibrate(0);
					error_count = 0;
					goto detected;
				}

				//usbPoll();
				_delay_ms(40);
				//usbPoll();

				/* Check for n64 controller */
				curGamepad = n64GetGamepad();
				curGamepad->init();
				if (curGamepad->probe())  {
					curGamepad = curGamepad;
					gamepadVibrate(0);
					error_count = 0;
					goto detected;
				}

				break;
			}
			continue;
		}
detected:
		wdt_reset();

		if (first_run)
		{
			if (curGamepad->update()) {
				error_count++;
			} else {
				error_count = 0;
			}
			first_run = 0;
		}
		if (mustPollControllers())
		{
			clrPollControllers();
			if (!must_report)
			{
				sleep_enable();
				sleep_cpu();
				sleep_disable();
				_delay_us(100);
				if (curGamepad->update()) {
					error_count++;
				} else {
					error_count = 0;
				}
				for (i=0; i<curGamepad->num_reports; i++) {
					if (curGamepad->changed(i+1)) {
						must_report |= (1<<i);
					}
				}
			}
		}
		if (must_report)
		{
			for (i = 0; i < curGamepad->num_reports; i++)
			{
				if ((must_report & (1<<i)) == 0)
					continue;
				transferGamepadReport(i+1);
			}
			must_report = 0;
		}
	}
}
