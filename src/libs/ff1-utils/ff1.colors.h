#pragma once

#include <afxwin.h> // sigh, WinGdi.h is needed for RGB, but it needs BYTE from other headers, etc.

constexpr auto COLOR_BLACK = RGB(0, 0, 0);
constexpr auto COLOR_GREEN = RGB(32, 192, 32);
constexpr auto COLOR_ORANGE = RGB(255, 224, 32);
constexpr auto COLOR_FFRED = RGB(160, 0, 0);
constexpr auto COLOR_FFBLUE = RGB(32, 64, 255);

