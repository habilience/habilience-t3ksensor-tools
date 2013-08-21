#ifndef SOFTKEYDEF_H
#define SOFTKEYDEF_H

#define SOFT_KEY_MAX  30
#define SOFT_LOGIC_MAX  50

#define MM_MOUSE_KEY0_CTRL    0x01
#define MM_MOUSE_KEY0_SHIFT   0x02
#define MM_MOUSE_KEY0_ALT     0x04
#define MM_MOUSE_KEY0_WINDOW  0x08
#define MM_MOUSE_KEY0_CKEYS   (MM_MOUSE_KEY0_CTRL | MM_MOUSE_KEY0_SHIFT | MM_MOUSE_KEY0_ALT | MM_MOUSE_KEY0_WINDOW)
#define MM_MOUSE_KEY0_MOUSE   0x80
#define MM_MOUSE_KEY1_NOACTION             0x0
#define MM_MOUSE_KEY1_MOUSE_L_MOVE         0x01
#define MM_MOUSE_KEY1_MOUSE_L_CLICK        0x02
#define MM_MOUSE_KEY1_MOUSE_L_DOUBLECLICK  0x03
#define MM_MOUSE_KEY1_MOUSE_R_MOVE         0x04
#define MM_MOUSE_KEY1_MOUSE_R_CLICK        0x05
#define MM_MOUSE_KEY1_MOUSE_R_DOUBLECLICK  0x06
#define MM_MOUSE_KEY1_MOUSE_M_MOVE         0x07
#define MM_MOUSE_KEY1_MOUSE_M_CLICK        0x08
#define MM_MOUSE_KEY1_MOUSE_M_DOUBLECLICK  0x09
#define MM_MOUSE_KEY1_MOUSE_WHEEL          0xfe
#define MM_MOUSE_KEY1_MOUSE_MOVE           0xff


// softlogic
#define MM_SOFTLOGIC_NOACTION               0
#define MM_SOFTLOGIC_OP_KEY_AND             1  // in1:boolean [in2]  out:key+ckey+triggertime
#define MM_SOFTLOGIC_OP_KEY_OR              2  // in1:boolean [in2]  out:key+ckey+triggertime
#define MM_SOFTLOGIC_OP_KEY_XOR             3  // in1:boolean [in2]  out:key+ckey+triggertime
#define MM_SOFTLOGIC_OP_KEY_AND_RISINGEDGE  4  // in1:boolean [in2]  out:key+ckey+triggertime
//#define MM_SOFTLOGIC_OP_KEY_UPDOWN          5  // in1:softkey        out:(key+ckey)*2
//#define MM_SOFTLOGIC_OP_KEY_LEFTRIGHT       6  // in1:softkey        out:(key+ckey)*2
//#define MM_SOFTLOGIC_OP_KEY_ZOOM            7  // in1:softkey        out:(key+ckey)*2
#define MM_SOFTLOGIC_OP_MKEY                13 // mouse-key
#define MM_SOFTLOGIC_OP_STAT                14 // state-button
#define MM_SOFTLOGIC_OP_REPORT              15 //
#define MM_SOFTLOGIC_OP_TRIGGER_INF0        0x0f

#define MM_SOFTLOGIC_OP_TRIGGER01       128
#define MM_SOFTLOGIC_OP_TRIGGER02       256
#define MM_SOFTLOGIC_OP_TRIGGER03       512
#define MM_SOFTLOGIC_OP_TRIGGER04      1024
#define MM_SOFTLOGIC_OP_TRIGGER05      2048
#define MM_SOFTLOGIC_OP_TRIGGER06_UP   (256-SOFTLOGIC_TRIGGER_TICKTIME)
#define MM_SOFTLOGIC_OP_TRIGGER07_UP   (512-SOFTLOGIC_TRIGGER_TICKTIME)
#define MM_SOFTLOGIC_OP_TRIGGER08_UP  (1024-SOFTLOGIC_TRIGGER_TICKTIME)
#define MM_SOFTLOGIC_OP_TRIGGER09_UP  (2048-SOFTLOGIC_TRIGGER_TICKTIME)
#define MM_SOFTLOGIC_OP_TRIGGER10_UP  (3072-SOFTLOGIC_TRIGGER_TICKTIME)
#define MM_SOFTLOGIC_OP_TRIGGER11_UP    192
#define MM_SOFTLOGIC_OP_TRIGGER11_DN     64
#define MM_SOFTLOGIC_OP_TRIGGER12_UP    384
#define MM_SOFTLOGIC_OP_TRIGGER12_DN    128
#define MM_SOFTLOGIC_OP_TRIGGER13_UP    768
#define MM_SOFTLOGIC_OP_TRIGGER13_DN    256
#define MM_SOFTLOGIC_OP_TRIGGER14_UP   1536
#define MM_SOFTLOGIC_OP_TRIGGER14_DN    512
#define MM_SOFTLOGIC_OP_TRIGGER15_UP   2048
#define MM_SOFTLOGIC_OP_TRIGGER15_DN   1024

#define MM_SOFTLOGIC_OPEX_MKEY_AND               0  // in1:boolean [in2]         out:mkey+ckey+triggertime
#define MM_SOFTLOGIC_OPEX_MKEY_AND_RISINGEDGE    1  // in1:boolean [in2]         out:mkey+ckey+triggertime
//#define MM_SOFTLOGIC_OPEX_MKEY_TAP               2  // in1:softkey [in2:enable]  out:mkey+ckey+triggertime
//#define MM_SOFTLOGIC_OPEX_MKEY_UPDOWN            3  // in1:softkey [in2:enable]  out:whl+ckey+triggertime
//#define MM_SOFTLOGIC_OPEX_MKEY_LEFTRIGHT         4  // in1:softkey [in2:enable]  out:whl+ckey+triggertime
//#define MM_SOFTLOGIC_OPEX_MKEY_MOVE              5  // in1:softkey [in2:enable]  out:move+ckey+triggertime
//#define MM_SOFTLOGIC_OPEX_MKEY_ZOOM              6  // in1:softkey [in2:enable]  out:whl+ckey+triggertime
//#define MM_SOFTLOGIC_OPEX_MKEY_                  7  //!!
#define MM_SOFTLOGIC_OPEX_MKEY_ACT_MASK          0x1f
#define MM_SOFTLOGIC_OPEX_MKEY_ACT_BTN_MASK      0x07
#define MM_SOFTLOGIC_OPEX_MKEY_ACT_LBTN          0x01
#define MM_SOFTLOGIC_OPEX_MKEY_ACT_RBTN          0x02
#define MM_SOFTLOGIC_OPEX_MKEY_ACT_MBTN          0x04
//#define MM_SOFTLOGIC_OPEX_MKEY_ACT_TOGGLEWHL     0x08

#define MM_SOFTLOGIC_OPEX_STAT_AND             0    // in1:boolean [in2]  out:(state,gpio)+triggertime
#define MM_SOFTLOGIC_OPEX_STAT_OR              1    // in1:boolean [in2]  out:(state,gpio)+triggertime
#define MM_SOFTLOGIC_OPEX_STAT_XOR             2    // in1:boolean [in2]  out:(state,gpio)+triggertime
#define MM_SOFTLOGIC_OPEX_STAT_AND_RISINGEDGE  3    // in1:boolean [in2]  out:(state,gpio)+triggertime
#define MM_SOFTLOGIC_OPEX_STAT_OUTPORT         0x3f // 0~31

#define MM_SOFTLOGIC_OPEX_REPORT_AND             0    // in1:boolean [in2]  out:report+triggertime
#define MM_SOFTLOGIC_OPEX_REPORT_OR              1    // in1:boolean [in2]  out:report+triggertime
#define MM_SOFTLOGIC_OPEX_REPORT_XOR             2    // in1:boolean [in2]  out:report+triggertime
#define MM_SOFTLOGIC_OPEX_REPORT_AND_RISINGEDGE  3    // in1:boolean [in2]  out:report+triggertime
#define MM_SOFTLOGIC_OPEX_REPORT_ID              0x3f // 0~63

#define MM_SOFTLOGIC_IN_NOT                0x80
#define MM_SOFTLOGIC_IN_MASK               0x7f
// 0, 1~7, 8~31(0x1f), 32~63(0x3f), 64~127(0x7f)
// |   |    |            |            +- softlogic
// |   |    |            +- softkey
// |   |    +- GPIO in/out (0~23)
// |   +- engine state (0~6)
// +- no action
#define MM_SOFTLOGIC_PORT_STATE_START      1
#define MM_SOFTLOGIC_PORT_GPIO_START       8
#define MM_SOFTLOGIC_PORT_SOFTKEY_START    32
#define MM_SOFTLOGIC_PORT_SOFTLOGIC_START  64

#define MM_SOFTLOGIC_PORT_SOFTKEY_ALL  (MM_SOFTLOGIC_PORT_SOFTLOGIC_START - 1)

#define MM_SOFTLOGIC_STATE_WORKING_AND_BEEP1  0
#define MM_SOFTLOGIC_STATE_TOUCH_AND_BEEP2    1
#define MM_SOFTLOGIC_STATE_CALIBRATION        3
#define MM_SOFTLOGIC_STATE_TOUCHSCREEN        4
#define MM_SOFTLOGIC_STATE_MULTITOUCH_DEVICE  5
#define MM_SOFTLOGIC_STATE_INVERT_DETECTION   6

#define MM_SOFTLOGIC_ENABLE_CKEY_MASK  0x0f
#define MM_SOFTLOGIC_ENABLE_NOT        0x08
#define MM_SOFTLOGIC_ENABLE_PORTMASK   0x07

//#define MM_SOFTLOGIC_2CKEY_MASK   0xf0
#define MM_SOFTLOGIC_CKEY_CTRL    0x01
#define MM_SOFTLOGIC_CKEY_SHIFT   0x02
#define MM_SOFTLOGIC_CKEY_ALT     0x04
#define MM_SOFTLOGIC_CKEY_WINDOW  0x08

#define NO_GROUP		(NULL)

#endif // SOFTKEYDEF_H
