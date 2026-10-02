#include "framework.h"

AppState g_AppState =
{
    NULL,
    1,
    NULL,
    NULL,
    L""
};

HWND g_hZoomToolbar = NULL;

double g_Zoom = 1.0;

double g_PanX = 0.0;

double g_PanY = 0.0;

bool g_IsDragging = false;

POINT g_LastMousePoint =
{
    0,
    0
};

std::vector<IconEntry> g_Icons;

int g_SelectedIcon = -1;
