#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _NV_H
#define _NV_H

#include "cpu_type.h"

// from nvi.h

// not used
//#define NV_BUFF_SIZE				1024
//#define NV_READ_TIME_OUT			3000

// **************************************************************
// from usb_desc.h
// Endpoint max packet size
#define MOUSE_TX_MAX_PACKET              				0x06

#define HID_TX_MAX_PACKET              					0x40
#define HID_RX_MAX_PACKET              					0x40

#define KBD_TX_MAX_PACKET              					0x08
#define KBD_RX_MAX_PACKET              					0x02

// **************************************************************
// from mc.h
// calibration bit
#define MODE_CALIBRATION_NONE				DEF_BIT_00
#define MODE_CALIBRATION_SELF				DEF_BIT_03

// **************************************************************
// from snd.h
// calibration tone
#define SND_CAL_ENTER 1
#define SND_CAL_SUCCESS  2
#define SND_CAL_FAIL  3

// **************************************************************
// from mc.h
// calibration bit control
#define ClearCalibrationMode()			(gbl_calibration_mode = MODE_CALIBRATION_NONE)
#define SetCalibrationMode(mode)		(gbl_calibration_mode = mode )
#define GetCalibrationMode(mode)		((gbl_calibration_mode == mode) )

// **************************************************************
// from gpio_key.h
// keyboad value
#define V_KEY_TAB_VALUE				0x2b

#define V_KEY_F1_VALUE					0x3a
#define V_KEY_F2_VALUE					0x3b
#define V_KEY_F3_VALUE					0x3c
#define V_KEY_F4_VALUE					0x3d
#define V_KEY_F5_VALUE					0x3e
#define V_KEY_F6_VALUE					0x3f
#define V_KEY_F7_VALUE					0x40
#define V_KEY_F8_VALUE					0x41
#define V_KEY_F9_VALUE					0x42
#define V_KEY_F10_VALUE				0x43
#define V_KEY_F11_VALUE				0x44
#define V_KEY_F12_VALUE				0x45

#define V_KEY_F13_VALUE				0x68
#define V_KEY_F14_VALUE				0x69
#define V_KEY_F15_VALUE				0x6a
#define V_KEY_F16_VALUE				0x6b
#define V_KEY_F17_VALUE				0x6c
#define V_KEY_F18_VALUE				0x6d
#define V_KEY_F19_VALUE				0x6e
#define V_KEY_F20_VALUE				0x6f


/*
typedef enum
{
	hidDataTypeMode = 0x01,	
	hidDataTypeDebug,
	hidDataTypeNvRead,		
	hidDataTypeNvWrite,			
	hidDataTypeMax
} HIDDataType;
*/

// **************************************************************
// from usb_desc.h
typedef enum
{
	hidReportNull = 0x00,
	hidReportRxCommand,
	hidReportRxDebug,		
	hidReportTxCommand,
	hidReportTxDebug,		

#if 0		
	hidReportRxData,	
	hidReportTxNv,	
	hidReportTxDebug,
	hidReportTxData,		
#endif	
	hidReportMax
} HIDReportType;

/*
// from usb_task.h
typedef enum
{
	hidModeDebug = 0x00,
	hidModeNv,
	hidModeMax
} HIDMode;
*/

/*
typedef enum
{
	bitHID_ReportID = 0x00,
	bitHID_DataSize,
	bitHID_Data,
	bitHID_DataMax
} HIDDataBit;
*/

/*
typedef enum
{
//	keyNone = 0x00,
	keyDown = 0x00,
	keyLeft,
	keyRight,
	keyUp,
	keyTab,
	keyEnter,
	keyCal,
	keyOption,
	keyMode,
	keyAlt,
	keyMaxOut
} KEY_OUT_Def;
*/
/*
typedef enum
{
	nvStatusSucc = 0x00,
	nvStatusFail,
	nvStatusMax
} NV_Status;
*/

// **************************************************************
// from snd.h
typedef enum
{
	SND_ERR_F,
	SND_CLICK_F,
	SND_KEYTONE_F,
	SND_CAL_F,
	SND_CAM_F,
	SND_GENERAL_F	// 일반적인 BUZZER Play 용으로 만듦
}snd_cmd_name_type;

// general default value
#define NV_DEF_CALIBRATION_KEY						2
#define NV_DEF_CALIBRATION_KEY_RANGE_START			0
#define NV_DEF_CALIBRATION_KEY_RANGE_END			3

#define NV_DEF_TOUCH_DISABLE_KEY					1
#define NV_DEF_TOUCH_DISABLE_KEY_RANGE_START		0
#define NV_DEF_TOUCH_DISABLE_KEY_RANGE_END			3

#define NV_DEF_CALIBRATION_SCRNMARGIN				100
#define NV_DEF_CALIBRATION_SCRNMARGIN_START			1.0f
#define NV_DEF_CALIBRATION_SCRNMARGIN_END			90.0f

#define NV_DEF_CALIBRATION_KEY_NO					6
#define NV_DEF_CALIBRATION_KEY_NO_RANGE_START		2
#define NV_DEF_CALIBRATION_KEY_NO_RANGE_END			12

#define NV_DEF_TOUCH_DISABLE_KEY_NO					6
#define NV_DEF_TOUCH_DISABLE_KEY_NO_RANGE_START		2
#define NV_DEF_TOUCH_DISABLE_KEY_NO_RANGE_END		12


#define NV_DEF_TOUCH_ENABLE							1
#define NV_DEF_TOUCH_ENABLE_RANGE_START				0
#define NV_DEF_TOUCH_ENABLE_RANGE_END				1

#define NV_DEF_BUZZER_TYPE_CLICK					0
#define NV_DEF_BUZZER_TYPE_CLICK_RANGE_START		0
#define NV_DEF_BUZZER_TYPE_CLICK_RANGE_END			1

#define NV_DEF_BUZZER_TYPE_CALIBRATION				1
#define NV_DEF_BUZZER_TYPE_CALIBRATION_RANGE_START	0
#define NV_DEF_BUZZER_TYPE_CALIBRATION_RANGE_END	1

#define NV_DEF_BUZZER_TYPE_KEYTONE					0
#define NV_DEF_BUZZER_TYPE_KEYTONE_RANGE_START		0
#define NV_DEF_BUZZER_TYPE_KEYTONE_RANGE_END		1

#define NV_DEF_BUZZER_TYPE_ERROR						1
#define NV_DEF_BUZZER_TYPE_ERROR_RANGE_START			0
#define NV_DEF_BUZZER_TYPE_ERROR_RANGE_END				1

#define NV_DEF_BUZZER_TYPE_SENSOR_ATTACH				1
#define NV_DEF_BUZZER_TYPE_SENSOR_ATTACH_RANGE_START	0
#define NV_DEF_BUZZER_TYPE_SENSOR_ATTACH_RANGE_END		1

#define NV_DEF_BUZZER_TYPE_PEN_PAIRING					1
#define NV_DEF_BUZZER_TYPE_PEN_PAIRING_RANGE_START		0
#define NV_DEF_BUZZER_TYPE_PEN_PAIRING_RANGE_END		1

#define NV_DEF_BUZZER_TYPE_USB_ATTACH					1
#define NV_DEF_BUZZER_TYPE_USB_ATTACH_RANGE_START		0
#define NV_DEF_BUZZER_TYPE_USB_ATTACH_RANGE_END			1

// mouse default value
#define NV_DEF_CLICK_AREA							((int)(1.5*0x7fff/100))
#define NV_DEF_CLICK_AREA_RANGE_START				(0x7fff/200)
#define NV_DEF_CLICK_AREA_RANGE_END					(0x7fff/20)
#define NV_DEF_CLICK_AREA_RANGE_MAX					(0x7fff)

#define NV_DEF_DRAG_ENABLE							1
#define NV_DEF_DRAG_ENABLE_RANGE_START				0
#define NV_DEF_DRAG_ENABLE_RANGE_END				1

#define NV_DEF_MULTI_POINT_TIME						300
#define NV_DEF_MULTI_POINT_TIME_RANGE_START			100
#define NV_DEF_MULTI_POINT_TIME_RANGE_END			1500

#define NV_DEF_MULTI_POINT_AREA						((int)(15*0x7fff/100))
#define NV_DEF_MULTI_POINT_AREA_RANGE_START			(0x7fff/50)
#define NV_DEF_MULTI_POINT_AREA_RANGE_END			(0x7fff/2)
#define NV_DEF_MULTI_POINT_AREA_RANGE_MAX			(0x7fff)

//#define NV_DEF_DOUBLE_CLICK_ENABLE				1
//#define NV_DEF_DOUBLE_CLICK_ENABLE_RANGE_START	0
//#define NV_DEF_DOUBLE_CLICK_ENABLE_RANGE_END		1

#define NV_DEF_DOUBLE_CLICK_TIME					300
#define NV_DEF_DOUBLE_CLICK_TIME_RANGE_START		0
#define NV_DEF_DOUBLE_CLICK_TIME_RANGE_END			1500

#define NV_DEF_DOUBLE_CLICK_AREA					(0x7fff/50)
#define NV_DEF_DOUBLE_CLICK_AREA_RANGE_START		(0x7fff/200)
#define NV_DEF_DOUBLE_CLICK_AREA_RANGE_END			(0x7fff/4)
#define NV_DEF_DOUBLE_CLICK_AREA_RANGE_MAX			(0x7fff)

#define NV_DEF_RIGHT_CLICK_ENABLE					1
#define NV_DEF_RIGHT_CLICK_ENABLE_RANGE_START		0
#define NV_DEF_RIGHT_CLICK_ENABLE_RANGE_END			1

#define NV_DEF_RIGHT_CLICK_TIME						2000
#define NV_DEF_RIGHT_CLICK_TIME_RANGE_START			1500
#define NV_DEF_RIGHT_CLICK_TIME_RANGE_END			3000

#define NV_DEF_MIDDLE_CLICK_ENABLE					1
#define NV_DEF_MIDDLE_CLICK_ENABLE_RANGE_START		0
#define NV_DEF_MIDDLE_CLICK_ENABLE_RANGE_END		1

//#define NV_DEF_WHEEL_ENABLE						1
//#define NV_DEF_WHEEL_ENABLE_RANGE_START			0
//#define NV_DEF_WHEEL_ENABLE_RANGE_END				1

#define NV_DEF_WHEEL_SENSITIVITY					(-0xff/4)
#define NV_DEF_WHEEL_SENSITIVITY_RANGE_START		(-0xff)
#define NV_DEF_WHEEL_SENSITIVITY_RANGE_END			(0xff)

//#define NV_DEF_ZOOM_ENABLE						1
//#define NV_DEF_ZOOM_ENABLE_RANGE_START			0
//#define NV_DEF_ZOOM_ENABLE_RANGE_END				1

#define NV_DEF_ZOOM_SENSITIVITY						(0xff/4)
#define NV_DEF_ZOOM_SENSITIVITY_RANGE_START			(-0xff)
#define NV_DEF_ZOOM_SENSITIVITY_RANGE_END			(0xff)

#define NV_DEF_TWO_TOUCH_ENABLE						0
#define NV_DEF_TWO_TOUCH_ENABLE_RANGE_START			0
#define NV_DEF_TWO_TOUCH_ENABLE_RANGE_END			1

#define NV_DEF_ALT_TAB_ENABLE						1
#define NV_DEF_ALT_TAB_ENABLE_RANGE_START			0
#define NV_DEF_ALT_TAB_ENABLE_RANGE_END				2

#define NV_DEF_FUNCTION_KEY							0x00
#define NV_DEF_FUNCTION_KEY_RANGE_START				0x00
#define NV_DEF_FUNCTION_KEY_RANGE_END				0xfe

// detection default value
#define NV_DEF_CAM_TRIGGER_THRESHOLD     			170
#define NV_DEF_CAM_TRIGGER_THRESHOLD_RANGE_START	0
#define NV_DEF_CAM_TRIGGER_THRESHOLD_RANGE_END		0xff

#define NV_DEF_CAM_COEF1                 			64
#define NV_DEF_CAM_COEF2                 			10
#define NV_DEF_CAM_COEF3                 			200
#define NV_DEF_CAM_COEF_RANGE_START					0
#define NV_DEF_CAM_COEF_RANGE_END					0xff

// mouse default value
	// time_a
#define NV_DEF_TIME_A              400
#define NV_DEF_TIME_A_RANGE_START  100
#define NV_DEF_TIME_A_RANGE_END    800
	// time_l
#define NV_DEF_TIME_L              1000
#define NV_DEF_TIME_L_RANGE_START  800
#define NV_DEF_TIME_L_RANGE_END    3000
	// area_c
#define NV_DEF_AREA_C              ((int)(1.5*0x7fff/100))
#define NV_DEF_AREA_C_RANGE_START  ((int)(0.1*0x7fff/100))
#define NV_DEF_AREA_C_RANGE_END    ((int)(10.0*0x7fff/100))
	// area_d
#define NV_DEF_AREA_D              ((int)(2.0*0x7fff/100))
#define NV_DEF_AREA_D_RANGE_START  ((int)(0.1*0x7fff/100))
#define NV_DEF_AREA_D_RANGE_END    ((int)(25.0*0x7fff/100))
	// area_p
#define NV_DEF_AREA_P              ((int)(6.0*0x7fff/100))
#define NV_DEF_AREA_P_RANGE_START  ((int)(0.1*0x7fff/100))
#define NV_DEF_AREA_P_RANGE_END    ((int)(25.0*0x7fff/100))
	// area_m
#define NV_DEF_AREA_M              ((int)(30.0*0x7fff/100))
#define NV_DEF_AREA_M_RANGE_START  ((int)(2.0*0x7fff/100))
#define NV_DEF_AREA_M_RANGE_END    ((int)(80.0*0x7fff/100))


#define NV_DEF_WHEEL_SENSITIVITY					(-0xff/4)
#define NV_DEF_WHEEL_SENSITIVITY_RANGE_START		(-0xff)
#define NV_DEF_WHEEL_SENSITIVITY_RANGE_END			(0xff)

#define NV_DEF_ZOOM_SENSITIVITY						(0xff/4)
#define NV_DEF_ZOOM_SENSITIVITY_RANGE_START			(-0xff)
#define NV_DEF_ZOOM_SENSITIVITY_RANGE_END			(0xff)

#define NV_MOUSE_PROFILE_MAX  5

#define NV_DEF_MOUSE_PROFILE              1
#define NV_DEF_MOUSE_PROFILE_RANGE_START  0
#define NV_DEF_MOUSE_PROFILE_RANGE_END    (NV_MOUSE_PROFILE_MAX-1)

#define NV_DEF_CAM_DETECTION_ANGLE_LEFT  			0x2000
#define NV_DEF_CAM_DETECTION_ANGLE_RIGHT 			(0xffff-0x2000)
#define NV_DEF_CAM_DETECTION_ANGLE_LEFT_RANGE_START 0
#define NV_DEF_CAM_DETECTION_ANGLE_LEFT_RANGE_END	0xffff

#define NV_DEF_CAM_DETECTION_RANGE_LEFT  			0
#define NV_DEF_CAM_DETECTION_RANGE_RIGHT 			0xffff
#define NV_DEF_CAM_DETECTION_RANGE_RANGE_START		0
#define NV_DEF_CAM_DETECTION_RANGE_RANGE_END		0xffff

// sensor gain default value
#define NV_DEF_CAM_SENSOR_GAIN              6
#define NV_DEF_CAM_SENSOR_GAIN_RANGE_START  1
#define NV_DEF_CAM_SENSOR_GAIN_RANGE_END    10

// detectaion default value
#define NV_DEF_CAM_DETECTION_LINE					0x00
#define NV_DEF_CAM_DETECTION_LINE_RANGE_START 		-500
#define NV_DEF_CAM_DETECTION_LINE_RANGE_END	   		500

// ambient light default value
#define NV_DEF_AMBIENT_LIGHT_LEFT					0x00
#define NV_DEF_AMBIENT_LIGHT_CENTER					0x00
#define NV_DEF_AMBIENT_LIGHT_RIGHT					0x00
#define NV_DEF_AMBIENT_LIGHT_RANGE_START			0
#define NV_DEF_AMBIENT_LIGHT_RANGE_END				100

// multimonitor default value
#define NV_DEF_MONITOR_NO		    				0x00
#define NV_DEF_MONITOR_NO_RANGE_START		 		0x00
#define NV_DEF_MONITOR_NO_RANGE_END		    		0x07


#define NV_DEF_CM_DETECTION_SHUTTER                5
#define NV_DEF_CM_SIDEVIEW_SHUTTER                 7 

// self calibraiton resolution
#define CALIBRATION_SENSITIVITY_RESOLUTION			( 1 )

typedef enum 
{
	// mm general
	NV_CAL_KEY_I         =0x00,
	NV_CAL_NO_I          ,
	NV_CAL_I         ,  
	NV_CAL_SCR_MARGIN_I,
	NV_TOUCH_ENALBE_I  ,
	NV_SND_CLK_I    ,
	NV_SND_CAL_I    ,
	NV_SND_KEY_I    ,
	NV_SND_ERR_I    ,
	NV_SND_SENSOR_ATTACH_I ,
	NV_SND_PEN_PAIR_I ,
	NV_SND_USB_ATTACH_I ,
	NV_CLK_AREA_I  ,
	NV_DBL_CLK_TIME_I,
	NV_DBL_CLK_AREA_I,
	NV_MULT_POINT_TIME_I,
	NV_MULT_POINT_AREA_I,
	NV_LEFT_DBL_CLK_ENABLE_I,
	NV_RIGHT_CLK_ENABLE_I,
	NV_RIGHT_CLK_TIME_I,
	NV_MID_CLK_ENABLE_I,
	NV_WHEEL_SLIDE_ENABLE_I,
	NV_WHEEL_SENSITIBITY_I,
	NV_ZOOM_SENSITIBITY_I,
	NV_ALT_TABL_ENABLE_I,
	NV_FUNC1_KEY_I,
	NV_FUNC1_KEY_EXIT_I,
	NV_FUNC2_KEY_I,
	NV_FUNC2_KEY_EXIT_I,
	NV_TWO_TOUCH_ENABLE_I,

	// CM1 setting 
	NV_CM1_SENSOR_GAIN_I = 0x100,
	NV_CM1_DETECTION_LINE_I,
	NV_CM1_TRIGGER_THRES_I,
	NV_CM1_COEF1_I,
	NV_CM1_COEF2_I,
	NV_CM1_COEF3_I,		
	NV_CM1_DETECTION_ANGLE_LEFT_I,
	NV_CM1_DETECTION_ANGLE_RIGHT_I,
	NV_CM1_DETECTION_RANGE_LEFT_I,	
	NV_CM1_DETECTION_RANGE_RIGHT_I,	
	NV_CM1_ABMIENT_LIGHT_I,
	
	
	// CM2 setting 
	NV_CM2_SENSOR_GAIN_I =0x200,
	NV_CM2_DETECTION_LINE_I,
	NV_CM2_TRIGGER_THRES_I,
	NV_CM2_COEF1_I,
	NV_CM2_COEF2_I,
	NV_CM2_COEF3_I,		
	NV_CM2_DETECTION_ANGLE_LEFT_I,
	NV_CM2_DETECTION_ANGLE_RIGHT_I,
	NV_CM2_DETECTION_RANGE_LEFT_I,	
	NV_CM2_DETECTION_RANGE_RIGHT_I,	
	NV_CM2_ABMIENT_LIGHT_I,
	
	// key
	NV_KEY_DOWN_I =0x300,
	NV_KEY_LEFT_I,
	NV_KEY_RIGHT_I,
	NV_KEY_UP_I,
	NV_KEY_TAB_I,
	NV_KEY_ENTER_I,
	NV_KEY_CAL_I,
	NV_KEY_OPTION_I,
	NV_KEY_MODE_I,
	NV_KEY_ALT_I,
	NV_CM1_DETECTION_SHUTTER_I,
	NV_CM2_DETECTION_SHUTTER_I,
	NV_CM1_SIDEVIEW_SHUTTER_I,
	NV_CM2_SIDEVIEW_SHUTTER_I,
	NV_MAX_I,
	NV_INVALID_I=0xFFFF
} nv_index_type;

// from nv_pkt.h
#define SELF_CAL_POINT  (9) // see mc.h( SELF_CALIBRATION_POINT_COUNT )
#define NUM_OF_AMBIENT_LIGHT	   3      // see nvi.h ( AMBIENT_LIGHT_COUNT )

typedef union
{


//////////////////////////////////////////////////////////////////////////
// general
	word calibration_key;
	word calibration_no;
	int  calibration[SELF_CAL_POINT*2];
	word calibration_screenmargin;
	word touch_enable;

//////////////////////////////////////////////////////////////////
// buzzer
	word buzzerClick;
	word buzzerCalibration;
	word buzzerKeytone;
	word buzzerError;
	word buzzerSensorAttach;
	word buzzerPenPairing;
	word buzzerUsbAttach;	

//////////////////////////////////////////////////////////////////////////	
// mouse 
	word click_area;				// 0 ~ 0x7fff, default : 0x7fff / 50
	word double_click_time;			// 0msec ~ 1500msec, default : 400msec
	word double_click_area;			// 0 ~ 0x7fff, default : 0x7fff / 50
	word multi_point_time;			// 100msec ~ 2000msec, default : 1000msec
	word multi_point_area;			// 0 ~ 0x7fff, default : 0x7fff / 10
	// double-click
	word left_double_click_enable;	// 0:off, 1:on //!!not used yet

	// deep-click
	word right_click_enable;		// 0:off, 1:on
	word right_click_time;			// 1500msec ~ 3000msec, default : 2500msec

	// multipoint 3points
	word middle_click_enable;		// 3touch, 0:off, 1:on

	// multipoint up-down
	word  wheel_sliding_enable;		// 0:off, 1:on //!!not used yet
	int16 wheel_sensitivity;		// -0xff ~ 0xff, 0:off

	// multipoint near-far
	int16 zoom_sensitivity;			// -0xff ~ 0xff, 0:off

	// multipoint click
	word alt_tab_enable;			// alt + tab, 0:off, 1:alt+tab, 2:window+tab

	// multipoint double-click
	byte function_1_key;			// f1 ~ f12, default : 0x00
	byte function_1_key_ext;		// ctrl+alt+ //!!not used yet

	// multipoint deep-click
	byte function_2_key;			// f1 ~ f12, default : 0x00
	byte function_2_key_ext;		// ctrl+alt+ //!!not used yet

	word two_touch_enable;			// 2touch(click+click), 0:off, 1:on


////////////////////////////////////////////////////////////////////////
// CM setting 
	int16 sensor_gain;
	int16 detection_line;
	word trigger_threshold;
	int16 coef1;
	int16 coef2;
	int16 coef3;
	word detection_angle_left;
	word detection_angle_right;
	word detection_range_left;
	word detection_range_right;
	word ambient_light[NUM_OF_AMBIENT_LIGHT];

	word shutter;
	word key;
}nv_pkt_item_type;

// InputMode
#define INPUTMODE_MOUSE                      0x00
#define INPUTMODE_SINGLE_TOUCH               0x01
#define INPUTMODE_MULTI_TOUCH                0x02
#define INPUTMODE_UNDEFINED                  0xFF

// 화면 회전
#define SCREENDIRECTION_UP          0x00		 // 가로방향 / Landscape
#define SCREENDIRECTION_RIGHT       0x01		 // 세로방향 / Portrait
#define SCREENDIRECTION_DOWN        0x02		 // 가로방향(대칭 이동) / Landscape(flipped)
#define SCREENDIRECTION_LEFT        0x03		 // 세로방향(대칭 이동) / Protrait(flipped)
#define SCREENDIRECTION_BACK_UP     0x04
#define SCREENDIRECTION_BACK_RIGHT  0x05
#define SCREENDIRECTION_BACK_DOWN   0x06
#define SCREENDIRECTION_BACK_LEFT   0x07
#define SCREENDIRECTION_MASK        0x07


#endif
