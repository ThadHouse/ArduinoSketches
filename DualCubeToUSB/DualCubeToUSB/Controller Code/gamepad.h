#ifndef _gamepad_h__
#define _gamepad_h__

#include "..\MainIncludes.h"

#define GCN64_REPORT_SIZE	8

typedef struct {
	int gamepad;
	void (*init)(boolean isCont2);
	char (*update)(boolean isCont2);
	char (*changed)(int);
	char (*buildReport)(unsigned char *buf, int id);

	/* Check for the controller */
	char (*probe)(boolean isCont2); /* return true if found */

} GCpad;




#endif // _gamepad_h__