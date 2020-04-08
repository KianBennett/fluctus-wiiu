#pragma once

#define BUTTON_A        0x8000
#define BUTTON_B        0x4000
#define BUTTON_X        0x2000
#define BUTTON_Y        0x1000
#define BUTTON_LEFT     0x0800
#define BUTTON_RIGHT    0x0400
#define BUTTON_UP       0x0200
#define BUTTON_DOWN     0x0100
#define BUTTON_ZL       0x0080
#define BUTTON_ZR       0x0040
#define BUTTON_L        0x0020
#define BUTTON_R        0x0010
#define BUTTON_PLUS     0x0008
#define BUTTON_MINUS    0x0004
#define BUTTON_HOME     0x0002
#define BUTTON_SYNC     0x0001
#define BUTTON_STICK_R  0x00020000
#define BUTTON_STICK_L  0x00040000
#define BUTTON_TV       0x00010000

#ifdef __cplusplus
extern "C" {
#endif

// C wrapper for our C++ functions
int start(void);

#ifdef __cplusplus
}
#endif