#ifndef T3KPACKETDEF_H
#define T3KPACKETDEF_H

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

#define MM_GESTURE_NO_ACTION                            0x00
#define MM_GESTURE_SINGLE_MOVE                          0x01
#define MM_GESTURE_SINGLE_TAP                           0x02
#define MM_GESTURE_SINGLE_DOUBLE_TAP                    0x03
#define MM_GESTURE_SINGLE_LONG_TAP                      0x04
#define MM_GESTURE_FINGERS_MOVE                         0x11
#define MM_GESTURE_FINGERS_TAP                          0x12
#define MM_GESTURE_FINGERS_DOUBLE_TAP                   0x13
#define MM_GESTURE_FINGERS_LONG_TAP                     0x14
#define MM_GESTURE_PALM_MOVE                            0x21
#define MM_GESTURE_PALM_TAP                             0x22
#define MM_GESTURE_PALM_DOUBLE_TAP                      0x23
#define MM_GESTURE_PALM_LONG_TAP                        0x24
#define MM_GESTURE_PUTAND_TAP                           0x32
#define MM_GESTURE_PUTAND_DOUBLE_TAP                    0x33
#define MM_GESTURE_PUTAND_LONG_TAP                      0x34
#define MM_GESTURE_ACTIONMASK                           0x3f
//#define MM_GESTURE_ROTATE                               0x40
#define MM_GESTURE_ZOOM                                 0x80
#define MM_FEASIBLEGESTURE_SINGLE_MOVE                  0x0001
#define MM_FEASIBLEGESTURE_SINGLE_TAP                   0x0002
#define MM_FEASIBLEGESTURE_SINGLE_DOUBLE_TAP            0x0004
#define MM_FEASIBLEGESTURE_SINGLE_LONG_TAP              0x0008
#define MM_FEASIBLEGESTURE_FINGERS_MOVE                 0x0010
#define MM_FEASIBLEGESTURE_FINGERS_TAP                  0x0020
#define MM_FEASIBLEGESTURE_FINGERS_DOUBLE_TAP           0x0040
#define MM_FEASIBLEGESTURE_FINGERS_LONG_TAP             0x0080
#define MM_FEASIBLEGESTURE_PALM_MOVE                    0x0100
#define MM_FEASIBLEGESTURE_PALM_TAP                     0x0200
#define MM_FEASIBLEGESTURE_PALM_DOUBLE_TAP              0x0400
#define MM_FEASIBLEGESTURE_PALM_LONG_TAP                0x0800
#define MM_FEASIBLEGESTURE_ZOOM                         0x1000
#define MM_FEASIBLEGESTURE_PUTAND_TAP                   0x2000
#define MM_FEASIBLEGESTURE_PUTAND_DOUBLE_TAP            0x4000
#define MM_FEASIBLEGESTURE_PUTAND_LONG_TAP              0x8000
#define MM_FEASIBLEGESTURE_SINGLE                       (MM_FEASIBLEGESTURE_SINGLE_MOVE | MM_FEASIBLEGESTURE_SINGLE_TAP | MM_FEASIBLEGESTURE_SINGLE_DOUBLE_TAP | MM_FEASIBLEGESTURE_SINGLE_LONG_TAP)
#define MM_FEASIBLEGESTURE_FINGERS                      (MM_FEASIBLEGESTURE_FINGERS_MOVE | MM_FEASIBLEGESTURE_FINGERS_TAP | MM_FEASIBLEGESTURE_FINGERS_DOUBLE_TAP | MM_FEASIBLEGESTURE_FINGERS_LONG_TAP)
#define MM_FEASIBLEGESTURE_PALM                         (MM_FEASIBLEGESTURE_PALM_MOVE | MM_FEASIBLEGESTURE_PALM_TAP | MM_FEASIBLEGESTURE_PALM_DOUBLE_TAP | MM_FEASIBLEGESTURE_PALM_LONG_TAP)
#define MM_FEASIBLEGESTURE_PUTAND                       (MM_FEASIBLEGESTURE_PUTAND_TAP | MM_FEASIBLEGESTURE_PUTAND_DOUBLE_TAP | MM_FEASIBLEGESTURE_PUTAND_LONG_TAP)
#define pkt_gesture_msg_body_sizeof_str                 (256 - 14)

#define MM_MOUSEPROFILE_SINGLE_MOVE                     0x0001
#define MM_MOUSEPROFILE_SINGLE_TAP                      0x0002
#define MM_MOUSEPROFILE_SINGLE_DOUBLE_TAP               0x0004
        // MOVE with ZOOM at the same type
//#define MM_MOUSEPROFILE_MOVE_AND_ZOOM                 0x0100
        // Make PUT&TAP to NO PUT&DOUBLE-TAP
#define MM_MOUSEPROFILE_PUTAND_TAP_OR_DOUBLETAP_ONLY    0x0200
        // Use PUT&, PALM action and Multi-touch device action
#define MM_MOUSEPROFILE_HYBRID_MULTITOUCHDEVICE         0x0400
        // Inertial WHEEL
#define MM_MOUSEPROFILE_INERTIAL_WHEEL                  0x1000
        // Invert wheel
#define MM_MOUSEPROFILE_INVERT_WHEEL                    0x2000
        // Use MacOSX margin
#define MM_MOUSEPROFILE_MAC_OS_MARGIN                   0x4000

#endif // T3KPACKETDEF_H
