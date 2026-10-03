#pragma once
#include "framework.h"
#include "string"


extern AppState g_AppState;

extern HWND g_hZoomToolbar;

extern double g_Zoom;

extern double g_PanX;
extern double g_PanY;

extern bool g_IsDragging;

extern POINT g_LastMousePoint;

extern std::vector<IconEntry> g_Icons;

extern int g_SelectedIcon;

