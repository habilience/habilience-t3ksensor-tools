#ifndef T3KSOFTKEYDEF_H
#define T3KSOFTKEYDEF_H

#define SOFT_KEY_MAX    30
#define SOFT_LOGIC_MAX  50

#define MM_SOFTLOGIC_NOACTION               0
#define MM_SOFTLOGIC_OP_KEY_AND             1  // in1:boolean [in2]  out:key+ckey
#define MM_SOFTLOGIC_OP_KEY_OR              2  // in1:boolean [in2]  out:key+ckey
#define MM_SOFTLOGIC_OP_KEY_XOR             3  // in1:boolean [in2]  out:key+ckey
#define MM_SOFTLOGIC_OP_KEY_AND_RISINGEDGE  4  // in1:boolean [in2]  out:key+ckey
//#define MM_SOFTLOGIC_OP_KEY_UPDOWN          5  // in1:softkey        out:(key+ckey)*2
//#define MM_SOFTLOGIC_OP_KEY_LEFTRIGHT       6  // in1:softkey        out:(key+ckey)*2
//#define MM_SOFTLOGIC_OP_KEY_ZOOM            7  // in1:softkey        out:(key+ckey)*2
#define MM_SOFTLOGIC_OP_MKEY                13 // mouse-key
#define MM_SOFTLOGIC_OP_STAT                14 // state-button
#define MM_SOFTLOGIC_OP_REPORT              15 //
#define MM_SOFTLOGIC_OP_TRIGGER_INF0        0x0f

#define MM_SOFTLOGIC_ENABLE_CKEY_MASK  0x0f
#define MM_SOFTLOGIC_ENABLE_NOT        0x08
#define MM_SOFTLOGIC_ENABLE_PORTMASK   0x07

#define MM_SOFTLOGIC_IN_NOT                0x80
#define MM_SOFTLOGIC_IN_MASK               0x7f

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

#define MM_SOFTLOGIC_OPEX_MKEY_AND               0  // in1:boolean [in2]         out:mkey+ckey
#define MM_SOFTLOGIC_OPEX_MKEY_AND_RISINGEDGE    1  // in1:boolean [in2]         out:mkey+ckey
//#define MM_SOFTLOGIC_OPEX_MKEY_TAP               2  // in1:softkey [in2:enable]  out:mkey+ckey
//#define MM_SOFTLOGIC_OPEX_MKEY_UPDOWN            3  // in1:softkey [in2:enable]  out:whl+ckey
//#define MM_SOFTLOGIC_OPEX_MKEY_LEFTRIGHT         4  // in1:softkey [in2:enable]  out:whl+ckey
//#define MM_SOFTLOGIC_OPEX_MKEY_MOVE              5  // in1:softkey [in2:enable]  out:move+ckey
//#define MM_SOFTLOGIC_OPEX_MKEY_ZOOM              6  // in1:softkey [in2:enable]  out:whl+ckey
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

#endif // T3KSOFTKEYDEF_H
