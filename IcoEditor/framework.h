#pragma once

#include <windows.h>
#include <commctrl.h>
#include <vector>
#include <fstream>
#include <cstdint>
#include <cmath>

#pragma comment(lib, "comctl32.lib")


#define IDC_ZOOM_TOOLBAR      5001
#define ID_ZOOM_IN            5002
#define ID_ZOOM_OUT           5003

#define ZOOM_TOOLBAR_HEIGHT   32


#define IDC_MAIN_MENU         101
#define IDC_SIDE_SIZE_LIST    102
#define IDC_MAIN_TOOLBAR      103


#define ID_FILE_OPEN          40001
#define ID_FILE_SAVEAS        40002
#define ID_FILE_EXIT          40003

#define ID_HELP_ABOUT         40101


const int ICON_SIZES[] =
{
    16,
    32,
    48,
    64,
    128,
    256,
    512
};

const int NUM_ICON_SIZES =
sizeof(ICON_SIZES) /
sizeof(ICON_SIZES[0]);


#define SIDEBAR_WIDTH         150
#define TOOLBARHEIGHT         28


typedef struct
{
    HICON hCurrentIcon;
    int selectedSizeIndex;
    HWND hwndListBox;
    HWND hwndToolbar;
    WCHAR szFilePath[MAX_PATH];

} AppState;


struct IconEntry
{
    HICON hIcon;
    int width;
    int height;
};


#pragma pack(push, 1)

struct ICONDIR
{
    WORD idReserved;
    WORD idType;
    WORD idCount;
};

struct ICONDIRENTRY
{
    BYTE  bWidth;
    BYTE  bHeight;
    BYTE  bColorCount;
    BYTE  bReserved;

    WORD  wPlanes;
    WORD  wBitCount;

    DWORD dwBytesInRes;
    DWORD dwImageOffset;
};

#pragma pack(pop)



BOOL InitApplication(
    HINSTANCE hInstance
);

BOOL InitInstance(
    HINSTANCE hInstance,
    int nCmdShow
);

LRESULT CALLBACK WndProc(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam
);


void CreateAppToolbar(
    HWND hWndParent
);

void CreateZoomToolbar(
    HWND hWndParent
);

void CreateSidebar(
    HWND hWndParent
);


void OnOpenFile(
    HWND hWnd
);

void OnSaveFileAs(
    HWND hWnd
);

bool LoadAllIconsFromFile(
    const WCHAR* fileName
);

void ClearLoadedIcons();


void ResetView();

void InvalidateCanvas(
    HWND hWnd
);

void SetZoom(
    HWND hWnd,
    double newZoom,
    int mouseX = -1,
    int mouseY = -1
);

void ZoomIn(
    HWND hWnd
);

void ZoomOut(
    HWND hWnd
);


void DrawIconCenter(
    HWND hWnd,
    HDC hdc,
    RECT rectClient
);

void DrawSidebarItems(
    HWND hWnd,
    DRAWITEMSTRUCT* pDrawItem
);


void UpdateSidebarList();

void UpdateWindowTitle(
    HWND hWnd
);


void HandleCommand(
    HWND hWnd,
    int wmId,
    int wmEvent
);
