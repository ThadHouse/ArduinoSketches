#ifndef _gcn64_protocol_h__
#define _gcn64_protocol_h__

#define CONTROLLER_IS_ABSENT	0
#define CONTROLLER_IS_N64		1
#define CONTROLLER_IS_GC		2
#define CONTROLLER_IS_UNKNOWN	4

//IO Port Definitions
#define GCN64_DATA_PORT	PORTC //C
#define GCN64_DATA_DDR	DDRC // C
#define GCN64_DATA_PIN	PINC // C
#define GCN64_DATA_BIT	(1<<6) //6

#define GCN64_DATA_PORT2	PORTF
#define GCN64_DATA_DDR2	DDRF
#define GCN64_DATA_PIN2	PINF
#define GCN64_DATA_BIT2	(1<<7)

/*
#define GCN64_DATA_PORT	PORTC
#define GCN64_DATA_DDR	DDRC
#define GCN64_DATA_PIN	PINC
#define GCN64_DATA_BIT	(1<<6)
*/

/* Return many unknown bits, but two are about the expansion port. */
#define N64_GET_CAPABILITIES		0x00
#define N64_CAPS_REPLY_LENGTH		24

#define OFFSET_EXT_REMOVED			22
#define OFFSET_EXT_PRESENT			23

/* Returns button states and axis values */
#define N64_GET_STATUS				0x01
#define N64_GET_STATUS_REPLY_LENGTH	32

/* Read from the expansion bus. */
#define N64_EXPANSION_READ			0x02

/* Write to the expansion bus. */
#define N64_EXPANSION_WRITE			0x03

/* Return information about controller. */
#define GC_GETID					0x00
#define GC_GETID_REPLY_LENGTH		24

/* Return origin data. */
#define GC_GETORIGIN				0x41
#define GC_GETORIGIN_REPLY_LENGTH	80

/* 3-byte get status command. Returns axis and buttons. Also 
 * controls motor. */
#define GC_GETSTATUS1				0x40
#define GC_GETSTATUS2				0x03
#define GC_GETSTATUS3	            0x00 
#define GC_GETSTATUS_REPLY_LENGTH	64

#define GC_BTN_L	9
#define GC_BTN_R	10
#define GC_BTN_Z	11

#ifdef __cplusplus
extern "C"{
#endif

void gcn64protocol_hwinit();
int gcn64_detectController();
int gcn64_transaction(unsigned char *data_out, int data_out_len);

void gcn64protocol_hwinit2();
int gcn64_detectController2();
int gcn64_transaction2(unsigned char *data_out, int data_out_len);



#ifdef __cplusplus
} // extern "C"
#endif
extern volatile unsigned char gcn64_workbuf[];
extern volatile unsigned char gcn64_workbuf2[];


#endif // _gcn64_protocol_h__

