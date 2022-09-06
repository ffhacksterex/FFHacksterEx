#pragma once

#include <afxwin.h> // sigh, WinGdi.h is needed for RGB, but it needs BYTE from other headers, etc.
//TODO - better off specifying these as 24-bit hex values?

constexpr auto COLOR_BLACK = RGB(0, 0, 0);
constexpr auto COLOR_WHITE = RGB(255, 255, 255);
constexpr auto COLOR_GRAY = RGB(128, 128, 128);
constexpr auto COLOR_LTGRAY = RGB(192, 192, 192);
constexpr auto COLOR_RED = RGB(255, 0, 0);
constexpr auto COLOR_GREEN = RGB(0, 255, 0);
constexpr auto COLOR_BLUE = RGB(0, 0, 255);
constexpr auto COLOR_FFRED = RGB(160, 0, 0);
constexpr auto COLOR_FFGREEN = RGB(32, 192, 32);
constexpr auto COLOR_FFBLUE = RGB(32, 64, 255);
constexpr auto COLOR_ORANGE = RGB(255, 224, 32);

