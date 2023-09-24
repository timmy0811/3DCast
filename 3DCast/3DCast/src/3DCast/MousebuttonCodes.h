#pragma once

#ifdef CAST_PLATFORM_WINDOWS

#define CAST_MOUSE_BUTTON_1         0
#define CAST_MOUSE_BUTTON_2         1
#define CAST_MOUSE_BUTTON_3         2
#define CAST_MOUSE_BUTTON_4         3
#define CAST_MOUSE_BUTTON_5         4
#define CAST_MOUSE_BUTTON_6         5
#define CAST_MOUSE_BUTTON_7         6
#define CAST_MOUSE_BUTTON_8         7
#define CAST_MOUSE_BUTTON_LAST      CAST_MOUSE_BUTTON_8
#define CAST_MOUSE_BUTTON_LEFT      CAST_MOUSE_BUTTON_1
#define CAST_MOUSE_BUTTON_RIGHT     CAST_MOUSE_BUTTON_2
#define CAST_MOUSE_BUTTON_MIDDLE    CAST_MOUSE_BUTTON_3

#else
// Define codes for different platforms here

#endif