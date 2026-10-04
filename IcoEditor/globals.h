#pragma once
#include "framework.h"
#include "string"
#include "local.h"

extern AppState g_AppState;

extern HWND g_hZoomToolbar;

extern double g_Zoom;

extern double g_PanX;
extern double g_PanY;

extern bool g_IsDragging;

extern POINT g_LastMousePoint;

extern std::vector<IconEntry> g_Icons;

extern int g_SelectedIcon;

extern Lang lang;

extern HMENU g_hMainMenu;
extern HMENU g_hFileMenu;
extern HMENU g_hSpecialMenu;
extern HMENU g_hHelpMenu;
