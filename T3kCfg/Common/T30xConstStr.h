#ifndef __T30x_CONST_STRING_H__
#define __T30x_CONST_STRING_H__

static const char cstrQ[] = "?";
static const char cstrD[] = "*";
static const char cstrDD[] = "**";
static const char cstrW[] = "!";

static const char cstrCam1[] = "cam1/";
static const char cstrCam2[] = "cam2/";
static const char cstrCam3[] = "cam3/";
static const char cstrCam4[] = "cam4/";
static const char cstrCam5[] = "cam5/";
static const char cstrModeSideview[] = "mode=sideview";
static const char cstrModeDetection[] = "mode=detection";
static const char cstrModeSilence[] = "mode=silence";
static const char cstrModeFocus[] = "mode=focus";
static const char cstrCamAttach[] = "cam_attach=";

// general
static const char cstrCalibrationKey[] = "calibration_key=";
static const char cstrCalibrationNo[] = "calibration_no=";
static const char cstrCalibrationScreenMargin[] = "calibration_screen_margin=";
static const char cstrCalibration[] = "calibration=";  	// margin, xxx,xxx~ 18ea
static const char cstrScaler[] = "scaler=";             // mat0,~,mat5
static const char cstrFactorialScreenMargin[] = "f41=";
static const char cstrFactoryCalibration[] = "f42=";  	// marginleft, margintop, marginright, marginbottom, xxx,xxx~ 26ea

static const char cstrTouchDisableKey[] = "touch_disable_key=";
static const char cstrTouchDisableNo[] = "touch_disable_no=";
static const char cstrTouchEnable[] = "touch_enable=";
static const char cstrAutoTuning[] = "auto_tuning=";
static const char cstrSimpleDetection[] = "simple_detection=";
static const char cstrInvertDetection[] = "invert_detection=";

static const char cstrBuzzer[] = "buzzer=";

// mouse
	// time
static const char cstrTimeA[] = "time_a=";
static const char cstrTimeL[] = "time_l=";
	// area
static const char cstrAreaC[] = "area_c=";
static const char cstrAreaD[] = "area_d=";
static const char cstrAreaP[] = "area_p=";
static const char cstrAreaM[] = "area_m=";
	// sensitivity
static const char cstrWheelSensitivity[] = "wheel_sensitivity=";
static const char cstrZoomSensitivity[] = "zoom_sensitivity=";
	// profile
static const char cstrMouseProfile[] = "mouse_profile=";
static const char cstrMouseProfile1[] = "mouse_profile_1=";
static const char cstrMouseProfile2[] = "mouse_profile_2=";
static const char cstrMouseProfile3[] = "mouse_profile_3=";
static const char cstrMouseProfile4[] = "mouse_profile_4=";
static const char cstrMouseProfile5[] = "mouse_profile_5=";

// soft-key
static const char cstrSoftkey[]   = "softkey=";
static const char cstrSoftlogic[] = "softlogic=";

static const char cstrFactorialSoftkey[]     = "f51=";
static const char cstrFactorialSoftlogic[]   = "f52=";
static const char cstrFactorialSoftkeyCal[]  = "f53=";
static const char cstrFactorialSoftkeyBind[] = "f54=";
static const char cstrFactorialGPIO[]        = "f55=";
static const char cstrFactorialSoftkeyPos[]  = "f56=";

// cm coefficients
static const char cstrSensorGain[] = "sensor_gain=";
//static const char cstrDetectionAngle[] = "detection_angle=";
static const char cstrFactorialCamPos[] = "f31=";
static const char cstrAmbientLight[] = "ambient_light=";

static const char cstrAdminDetectionLine[] = "admin_detection_line=";
static const char cstrAdminDetectionCenter[] = "admin_detection_center=";
static const char cstrAdminDetectionBarrel[] = "admin_detection_barrel=";

// instant or command
static const char cstrGetNV[] = "get_nv=";
static const char cstrInstantMode[] = "instant_mode="; // hid_flags, expiration_msec_time
static const char cstrEnvironment[] = "environment";
//static const char cstrStateReport[] = "state_report";
static const char cstrCalibrationMode[] = "calibration_mode=";
static const char cstrBuzzerPlay[] = "buzzer_play=";
static const char cstrKeyboard[] = "keyboard=";
static const char cstrFirmwareVersion[] = "firmware_version=";
static const char cstrAdminSerial[] = "admin_serial=";

static const char cstrInputMode[] = "input_mode=";
static const char cstrDisplayOrientation[] = "display_orientation=";
static const char cstrUsbConfigMode[] = "usb_config_mode=";

static const char cstrDetectionThreshold[] = "detection_threshold=";
static const char cstrDetectionRange[] = "detection_range=";
static const char cstrDetectionLine[] = "detection_line=";
static const char cstrDetectionCoef1[] = "detection_coef1=";
static const char cstrDetectionCoef2[] = "detection_coef2=";
static const char cstrDetectionCoef3[] = "detection_coef3=";

static const char cstrAdminSettingTime[]	= "admin_setting_time=";

#endif	// __T30x_CONST_STRING_H__
