#include "framework.h"
#include "globals.h"
#include "local.h" 
#include <vector>
#include <algorithm>
#include <fstream>
#include <cstdint>
#include <cmath>

void UpdateLanguage(HWND hWnd)
{
    ModifyMenu(
        g_hMainMenu,
        0,
        MF_BYPOSITION | MF_POPUP,
        reinterpret_cast<UINT_PTR>(g_hFileMenu),
        local_matrix[_FILE][lang].c_str()
    );

    ModifyMenu(
        g_hMainMenu,
        1,
        MF_BYPOSITION | MF_POPUP,
        reinterpret_cast<UINT_PTR>(g_hSpecialMenu),
        local_matrix[_SPECIAL][lang].c_str()
    );

    ModifyMenu(
        g_hMainMenu,
        2,
        MF_BYPOSITION | MF_POPUP,
        reinterpret_cast<UINT_PTR>(g_hHelpMenu),
        local_matrix[_INFO][lang].c_str()
    );


    ModifyMenu(
        g_hFileMenu,
        ID_FILE_OPEN,
        MF_BYCOMMAND | MF_STRING,
        ID_FILE_OPEN,
        local_matrix[_OPEN][lang].c_str()
    );

    ModifyMenu(
        g_hFileMenu,
        ID_FILE_SAVEAS,
        MF_BYCOMMAND | MF_STRING,
        ID_FILE_SAVEAS,
        local_matrix[_SAVEAS][lang].c_str()
    );

    ModifyMenu(
        g_hFileMenu,
        ID_FILE_EXIT,
        MF_BYCOMMAND | MF_STRING,
        ID_FILE_EXIT,
        local_matrix[_EXIT][lang].c_str()
    );


    ModifyMenu(
        g_hSpecialMenu,
        ID_LANGUAGE,
        MF_BYCOMMAND | MF_STRING,
        ID_LANGUAGE,
        local_matrix[_SELECTLANG][lang].c_str()
    );


    ModifyMenu(
        g_hHelpMenu,
        ID_HELP_ABOUT,
        MF_BYCOMMAND | MF_STRING,
        ID_HELP_ABOUT,
        local_matrix[_ABOUT_BTN][lang].c_str()
    );


    if (g_hZoomToolbar)
    {
        TBBUTTONINFO buttonInfo = {};

        buttonInfo.cbSize = sizeof(TBBUTTONINFO);
        buttonInfo.dwMask = TBIF_TEXT;

        buttonInfo.pszText =
            const_cast<LPWSTR>(
                local_matrix[_MINUS][lang].c_str()
                );

        SendMessage(
            g_hZoomToolbar,
            TB_SETBUTTONINFO,
            ID_ZOOM_OUT,
            reinterpret_cast<LPARAM>(&buttonInfo)
        );


        buttonInfo.pszText =
            const_cast<LPWSTR>(
                local_matrix[_PLUS][lang].c_str()
                );

        SendMessage(
            g_hZoomToolbar,
            TB_SETBUTTONINFO,
            ID_ZOOM_IN,
            reinterpret_cast<LPARAM>(&buttonInfo)
        );
    }


    DrawMenuBar(hWnd);

    UpdateWindowTitle(hWnd);

    InvalidateRect(
        hWnd,
        NULL,
        TRUE
    );
}


void ClearLoadedIcons()
{
    for (auto& entry : g_Icons)
    {
        if (entry.hIcon)
        {
            DestroyIcon(entry.hIcon);
        }
    }

    g_Icons.clear();
    g_SelectedIcon = -1;

    g_AppState.hCurrentIcon = NULL;
}


void ResetView()
{
    g_Zoom = 1.0;
    g_PanX = 0.0;
    g_PanY = 0.0;
}


void InvalidateCanvas(HWND hWnd)
{
    RECT rect;

    GetClientRect(
        hWnd,
        &rect
    );

    rect.left =
        SIDEBAR_WIDTH;

    rect.top =
        TOOLBARHEIGHT +
        ZOOM_TOOLBAR_HEIGHT;

    InvalidateRect(
        hWnd,
        &rect,
        FALSE
    );
}


bool LoadAllIconsFromFile(
    const WCHAR* fileName)
{
    ClearLoadedIcons();

    std::ifstream file(
        fileName,
        std::ios::binary | std::ios::ate
    );

    if (!file.is_open())
        return false;

    std::streamsize fileSize =
        file.tellg();

    if (fileSize <
        static_cast<std::streamsize>(
            sizeof(ICONDIR) +
            sizeof(ICONDIRENTRY)))
    {
        return false;
    }

    file.seekg(
        0,
        std::ios::beg
    );

    std::vector<BYTE> data(
        static_cast<size_t>(
            fileSize)
    );

    if (!file.read(
        reinterpret_cast<char*>(
            data.data()),
        fileSize))
    {
        return false;
    }

    const ICONDIR* iconDir =
        reinterpret_cast<const ICONDIR*>(
            data.data()
            );

    if (iconDir->idReserved != 0 ||
        iconDir->idType != 1 ||
        iconDir->idCount == 0)
    {
        return false;
    }

    size_t directorySize =
        sizeof(ICONDIR) +
        static_cast<size_t>(
            iconDir->idCount
            ) *
        sizeof(ICONDIRENTRY);

    if (directorySize >
        data.size())
    {
        return false;
    }

    const ICONDIRENTRY* entries =
        reinterpret_cast<const ICONDIRENTRY*>(
            data.data() +
            sizeof(ICONDIR)
            );

    for (WORD i = 0;
        i < iconDir->idCount;
        ++i)
    {
        const ICONDIRENTRY& entry =
            entries[i];

        size_t offset =
            static_cast<size_t>(
                entry.dwImageOffset
                );

        size_t size =
            static_cast<size_t>(
                entry.dwBytesInRes
                );

        if (offset >= data.size())
            continue;

        if (size >
            data.size() - offset)
        {
            continue;
        }

        if (size == 0)
            continue;

        int width =
            (entry.bWidth == 0)
            ? 256
            : entry.bWidth;

        int height =
            (entry.bHeight == 0)
            ? 256
            : entry.bHeight;

        const BYTE* iconData =
            data.data() + offset;

        HICON hIcon =
            CreateIconFromResourceEx(
                const_cast<PBYTE>(
                    iconData),
                static_cast<DWORD>(
                    size),
                TRUE,
                0x00030000,
                0,
                0,
                LR_DEFAULTCOLOR
            );

        if (!hIcon)
        {
            hIcon =
                CreateIconFromResourceEx(
                    const_cast<PBYTE>(
                        iconData),
                    static_cast<DWORD>(
                        size),
                    TRUE,
                    0x00030000,
                    width,
                    height,
                    LR_DEFAULTCOLOR
                );
        }

        if (!hIcon)
            continue;

        IconEntry newEntry;

        newEntry.hIcon = hIcon;
        newEntry.width = width;
        newEntry.height = height;

        g_Icons.push_back(
            newEntry
        );
    }

    std::sort(
        g_Icons.begin(),
        g_Icons.end(),
        [](const IconEntry& a,
            const IconEntry& b)
        {
            if (a.width != b.width)
                return a.width < b.width;

            return a.height < b.height;
        }
    );

    if (!g_Icons.empty())
    {
        g_SelectedIcon = 0;

        g_AppState.selectedSizeIndex = 0;

        g_AppState.hCurrentIcon =
            g_Icons[0].hIcon;
    }

    return !g_Icons.empty();
}


void UpdateSidebarList()
{
    if (!g_AppState.hwndListBox)
        return;

    SendMessage(
        g_AppState.hwndListBox,
        LB_RESETCONTENT,
        0,
        0
    );

    for (size_t i = 0;
        i < g_Icons.size();
        ++i)
    {
        WCHAR szSizeStr[64];

        wsprintf(
            szSizeStr,
            L"  %d x %d px",
            g_Icons[i].width,
            g_Icons[i].height
        );

        SendMessage(
            g_AppState.hwndListBox,
            LB_ADDSTRING,
            0,
            reinterpret_cast<LPARAM>(
                szSizeStr)
        );
    }

    if (!g_Icons.empty())
    {
        SendMessage(
            g_AppState.hwndListBox,
            LB_SETCURSEL,
            g_SelectedIcon,
            0
        );
    }
}


void SetZoom(
    HWND hWnd,
    double newZoom,
    int mouseX,
    int mouseY)
{
    if (newZoom < 0.1)
        newZoom = 0.1;

    if (newZoom > 8.0)
        newZoom = 8.0;

    if (mouseX >= 0 &&
        mouseY >= 0 &&
        newZoom != g_Zoom)
    {
        RECT clientRect;

        GetClientRect(
            hWnd,
            &clientRect
        );

        double centerX =
            SIDEBAR_WIDTH +
            (clientRect.right -
                SIDEBAR_WIDTH) / 2.0;

        double centerY =
            TOOLBARHEIGHT +
            ZOOM_TOOLBAR_HEIGHT +
            (clientRect.bottom -
                TOOLBARHEIGHT -
                ZOOM_TOOLBAR_HEIGHT) / 2.0;

        double imageX =
            (mouseX -
                centerX -
                g_PanX) /
            g_Zoom;

        double imageY =
            (mouseY -
                centerY -
                g_PanY) /
            g_Zoom;

        g_PanX =
            mouseX -
            centerX -
            imageX * newZoom;

        g_PanY =
            mouseY -
            centerY -
            imageY * newZoom;
    }

    g_Zoom = newZoom;

    InvalidateCanvas(hWnd);
}


void ZoomIn(HWND hWnd)
{
    SetZoom(
        hWnd,
        g_Zoom * 1.25
    );
}


void ZoomOut(HWND hWnd)
{
    SetZoom(
        hWnd,
        g_Zoom / 1.25
    );
}


void CreateZoomToolbar(
    HWND hWndParent)
{
    g_hZoomToolbar =
        CreateWindowEx(
            0,
            TOOLBARCLASSNAME,
            NULL,

            WS_CHILD |
            WS_VISIBLE |
            TBSTYLE_FLAT |
            TBSTYLE_TOOLTIPS,

            0,
            TOOLBARHEIGHT,
            0,
            ZOOM_TOOLBAR_HEIGHT,

            hWndParent,

            reinterpret_cast<HMENU>(
                IDC_ZOOM_TOOLBAR
                ),

            GetModuleHandle(NULL),
            NULL
        );

    if (!g_hZoomToolbar)
        return;

    SendMessage(
        g_hZoomToolbar,
        TB_BUTTONSTRUCTSIZE,
        sizeof(TBBUTTON),
        0
    );

    int minusIndex =
        static_cast<int>(
            SendMessage(
                g_hZoomToolbar,
                TB_ADDSTRING,
                0,
                reinterpret_cast<LPARAM>(
                    (local_matrix[_MINUS][lang] + L"\0").c_str()
                    )
            )
            );

    int plusIndex =
        static_cast<int>(
            SendMessage(
                g_hZoomToolbar,
                TB_ADDSTRING,
                0,
                reinterpret_cast<LPARAM>(
                    (local_matrix[_PLUS][lang] + L"\0").c_str()
                    )
            )
            );

    TBBUTTON buttons[2] = {};

    buttons[0].iBitmap =
        I_IMAGENONE;

    buttons[0].idCommand =
        ID_ZOOM_OUT;

    buttons[0].fsState =
        TBSTATE_ENABLED;

    buttons[0].fsStyle =
        BTNS_BUTTON |
        BTNS_AUTOSIZE;

    buttons[0].iString =
        minusIndex;

    buttons[1].iBitmap =
        I_IMAGENONE;

    buttons[1].idCommand =
        ID_ZOOM_IN;

    buttons[1].fsState =
        TBSTATE_ENABLED;

    buttons[1].fsStyle =
        BTNS_BUTTON |
        BTNS_AUTOSIZE;

    buttons[1].iString =
        plusIndex;

    SendMessage(
        g_hZoomToolbar,
        TB_ADDBUTTONS,
        2,
        reinterpret_cast<LPARAM>(
            buttons)
    );

    SendMessage(
        g_hZoomToolbar,
        TB_AUTOSIZE,
        0,
        0
    );
}
 
BOOL InitApplication(
    HINSTANCE hInstance)
{
    WNDCLASSEX wcex = { 0 };

    wcex.cbSize =
        sizeof(WNDCLASSEX);

    wcex.style =
        CS_HREDRAW |
        CS_VREDRAW;

    wcex.lpfnWndProc =
        WndProc;

    wcex.hInstance =
        hInstance;

    wcex.hCursor =
        LoadCursor(
            NULL,
            IDC_ARROW
        );

    wcex.hbrBackground =
        reinterpret_cast<HBRUSH>(
            COLOR_WINDOW + 1
            );

    wcex.lpszClassName =
        L"IconViewerClass";

    wcex.lpszMenuName =
        MAKEINTRESOURCE(
            IDC_MAIN_MENU
        );

    return RegisterClassEx(
        &wcex
    );
}


BOOL InitInstance(
    HINSTANCE hInstance,
    int nCmdShow)
{
    g_hMainMenu =
        CreateMenu();

    g_hFileMenu =
        CreatePopupMenu();

    g_hHelpMenu =
        CreatePopupMenu();

    g_hSpecialMenu =
        CreatePopupMenu();

    AppendMenu(
        g_hSpecialMenu,
        MF_STRING,
        ID_LANGUAGE,
        (local_matrix[_SELECTLANG][lang] + L"\0").c_str()
    );

    AppendMenu(
        g_hFileMenu,
        MF_STRING,
        ID_FILE_OPEN,
        (local_matrix[_OPEN][lang] + L"\0").c_str()
    );

    AppendMenu(
        g_hFileMenu,
        MF_STRING,
        ID_FILE_SAVEAS,
        (local_matrix[_SAVEAS][lang] + L"\0").c_str()
    );

    AppendMenu(
        g_hFileMenu,
        MF_SEPARATOR,
        0,
        NULL
    );

    AppendMenu(
        g_hFileMenu,
        MF_STRING,
        ID_FILE_EXIT,
        (local_matrix[_EXIT][lang] + L"\0").c_str()
    );

    AppendMenu(
        g_hHelpMenu,
        MF_STRING,
        ID_HELP_ABOUT,
        (local_matrix[_ABOUT_BTN][lang] + L"\0").c_str()
    );

    AppendMenu(
        g_hMainMenu,
        MF_POPUP,
        reinterpret_cast<UINT_PTR>(
            g_hFileMenu
            ),
        (local_matrix[_FILE][lang] + L"\0").c_str()
    );

    AppendMenu(
        g_hMainMenu,
        MF_POPUP,
        reinterpret_cast<UINT_PTR>(
            g_hSpecialMenu
            ),
        (local_matrix[_SPECIAL][lang] + L"\0").c_str()
    );

    AppendMenu(
        g_hMainMenu,
        MF_POPUP,
        reinterpret_cast<UINT_PTR>(
            g_hHelpMenu
            ),
        (local_matrix[_INFO][lang] + L"\0").c_str()
    );

    HWND hWnd =
        CreateWindowW(
            L"IconViewerClass",
            L"Impostor View",
            WS_OVERLAPPEDWINDOW,

            CW_USEDEFAULT,
            CW_USEDEFAULT,

            800,
            600,

            NULL,
            g_hMainMenu,
            hInstance,
            NULL
        );

    if (!hWnd)
        return FALSE;

    ShowWindow(
        hWnd,
        nCmdShow
    );

    UpdateWindow(hWnd);

    return TRUE;
}


void CreateAppToolbar(
    HWND hWndParent)
{
    g_AppState.hwndToolbar =
        CreateWindowEx(
            0,
            TOOLBARCLASSNAME,
            NULL,

            WS_CHILD |
            WS_VISIBLE |
            TBSTYLE_FLAT,

            0,
            0,
            0,
            TOOLBARHEIGHT,

            hWndParent,

            reinterpret_cast<HMENU>(
                IDC_MAIN_TOOLBAR
                ),

            GetModuleHandle(NULL),
            NULL
        );

    if (g_AppState.hwndToolbar)
    {
        SendMessage(
            g_AppState.hwndToolbar,
            TB_BUTTONSTRUCTSIZE,
            sizeof(TBBUTTON),
            0
        );
    }
}


void CreateSidebar(
    HWND hWndParent)
{
    g_AppState.hwndListBox =
        CreateWindowEx(
            WS_EX_CLIENTEDGE,
            L"LISTBOX",
            NULL,

            WS_CHILD |
            WS_VISIBLE |
            WS_VSCROLL |
            LBS_NOTIFY |
            LBS_OWNERDRAWFIXED |
            LBS_HASSTRINGS,

            0,

            TOOLBARHEIGHT +
            ZOOM_TOOLBAR_HEIGHT,

            SIDEBAR_WIDTH,

            400,

            hWndParent,

            reinterpret_cast<HMENU>(
                IDC_SIDE_SIZE_LIST
                ),

            GetModuleHandle(NULL),
            NULL
        );
}


void OnOpenFile(HWND hWnd)
{
    OPENFILENAME ofn = { 0 };

    ofn.lStructSize =
        sizeof(ofn);

    ofn.hwndOwner =
        hWnd;

    ofn.lpstrFilter =
        L"Icon Files (*.ico)\0*.ico\0"
        L"All Files (*.*)\0*.*\0";

    ofn.lpstrFile =
        g_AppState.szFilePath;

    ofn.nMaxFile =
        MAX_PATH;

    ofn.Flags =
        OFN_FILEMUSTEXIST |
        OFN_PATHMUSTEXIST;

    if (!GetOpenFileName(&ofn))
        return;

    if (!LoadAllIconsFromFile(
        g_AppState.szFilePath))
    {
        MessageBox(
            hWnd,

            L"Не удалось загрузить изображения "
            L"из ICO файла.",

            L"Ошибка",

            MB_OK |
            MB_ICONERROR
        );

        g_AppState.szFilePath[0] =
            L'\0';

        UpdateSidebarList();

        InvalidateRect(
            hWnd,
            NULL,
            FALSE
        );

        UpdateWindowTitle(
            hWnd
        );

        return;
    }

    ResetView();

    g_SelectedIcon = 0;

    g_AppState.selectedSizeIndex =
        0;

    g_AppState.hCurrentIcon =
        g_Icons[0].hIcon;

    UpdateSidebarList();

    UpdateWindowTitle(
        hWnd
    );

    InvalidateRect(
        hWnd,
        NULL,
        FALSE
    );
}


void OnSaveFileAs(
    HWND hWnd)
{
    if (g_Icons.empty())
    {
        MessageBox(
            hWnd,

            L"ERROR: "
            L"NOTHING TO SAVE",

            L"ERROR",

            MB_OK |
            MB_ICONERROR
        );

        return;
    }

    OPENFILENAME ofn = { 0 };

    WCHAR szSavePath[MAX_PATH] =
        L"";

    ofn.lStructSize =
        sizeof(ofn);

    ofn.hwndOwner =
        hWnd;

    ofn.lpstrFilter =
        L"Icon Files (*.ico)\0*.ico\0";

    ofn.lpstrFile =
        szSavePath;

    ofn.nMaxFile =
        MAX_PATH;

    ofn.Flags =
        OFN_OVERWRITEPROMPT |
        OFN_PATHMUSTEXIST;

    if (GetSaveFileName(&ofn))
    {

    }
}


void DrawIconCenter(
    HWND hWnd,
    HDC hdc,
    RECT rectClient)
{
    UNREFERENCED_PARAMETER(
        hWnd
    );

    if (g_SelectedIcon < 0 ||
        g_SelectedIcon >=
        static_cast<int>(
            g_Icons.size()))
    {
        RECT rectText =
            rectClient;

        rectText.left +=
            SIDEBAR_WIDTH;

        rectText.top +=
            TOOLBARHEIGHT +
            ZOOM_TOOLBAR_HEIGHT;

        DrawText(
            hdc,

            (local_matrix[_FILE_NOT_SELECTED][lang] + L"\0").c_str(),

            -1,

            &rectText,

            DT_CENTER |
            DT_SINGLELINE |
            DT_VCENTER
        );

        return;
    }

    HICON hIcon =
        g_Icons[
            g_SelectedIcon
        ].hIcon;

    int originalWidth =
        g_Icons[
            g_SelectedIcon
        ].width;

    int originalHeight =
        g_Icons[
            g_SelectedIcon
        ].height;

    int drawWidth =
        static_cast<int>(
            std::round(
                originalWidth *
                g_Zoom
            )
            );

    int drawHeight =
        static_cast<int>(
            std::round(
                originalHeight *
                g_Zoom
            )
            );

    if (drawWidth < 1)
        drawWidth = 1;

    if (drawHeight < 1)
        drawHeight = 1;

    int canvasTop =
        TOOLBARHEIGHT +
        ZOOM_TOOLBAR_HEIGHT;

    int canvasWidth =
        rectClient.right -
        SIDEBAR_WIDTH;

    int canvasHeight =
        rectClient.bottom -
        canvasTop;

    double centerX =
        SIDEBAR_WIDTH +
        canvasWidth / 2.0;

    double centerY =
        canvasTop +
        canvasHeight / 2.0;

    int x =
        static_cast<int>(
            std::round(
                centerX -
                drawWidth / 2.0 +
                g_PanX
            )
            );

    int y =
        static_cast<int>(
            std::round(
                centerY -
                drawHeight / 2.0 +
                g_PanY
            )
            );


    HPEN hBorderPen =
        CreatePen(
            PS_SOLID,
            1,
            RGB(
                120,
                120,
                120
            )
        );

    HGDIOBJ oldPen =
        SelectObject(
            hdc,
            hBorderPen
        );

    HGDIOBJ oldBrush =
        SelectObject(
            hdc,
            GetStockObject(
                NULL_BRUSH
            )
        );

    Rectangle(
        hdc,

        x,
        y,

        x + drawWidth,
        y + drawHeight
    );

    SelectObject(
        hdc,
        oldBrush
    );

    SelectObject(
        hdc,
        oldPen
    );

    DeleteObject(
        hBorderPen
    );


    DrawIconEx(
        hdc,
        x,
        y,
        hIcon,
        drawWidth,
        drawHeight,
        0,
        NULL,
        DI_NORMAL
    );
}


void UpdateWindowTitle(
    HWND hWnd)
{
    WCHAR szTitle[
        MAX_PATH + 50
    ];

    if (!g_Icons.empty() &&
        wcslen(
            g_AppState.szFilePath
        ) > 0)
    {
        WCHAR* pszFileName =
            wcsrchr(
                g_AppState.szFilePath,
                L'\\'
            );

        if (pszFileName)
        {
            pszFileName++;
        }
        else
        {
            pszFileName =
                g_AppState.szFilePath;
        }

        wsprintf(
            szTitle,

            L"[%s] - Impostor Viewer",

            pszFileName
        );
    }
    else
    {
        wsprintf(
            szTitle,

            L"Файл не выбран - "
            L"Impostor Viewer"
        );
    }

    SetWindowTextW(
        hWnd,
        szTitle
    );
}


void DrawSidebarItems(
    HWND hWnd,
    DRAWITEMSTRUCT* pDrawItem)
{
    UNREFERENCED_PARAMETER(
        hWnd
    );

    if (pDrawItem->itemID == -1)
        return;

    HDC hdc =
        pDrawItem->hDC;

    RECT rect =
        pDrawItem->rcItem;

    WCHAR szText[64] =
        L"";

    SendMessage(
        pDrawItem->hwndItem,

        LB_GETTEXT,

        pDrawItem->itemID,

        reinterpret_cast<LPARAM>(
            szText)
    );

    if (pDrawItem->itemState &
        ODS_SELECTED)
    {
        HBRUSH hBlueBrush =
            CreateSolidBrush(
                RGB(
                    0,
                    102,
                    204
                )
            );

        FillRect(
            hdc,
            &rect,
            hBlueBrush
        );

        DeleteObject(
            hBlueBrush
        );

        SetTextColor(
            hdc,
            RGB(
                255,
                255,
                255
            )
        );
    }
    else
    {
        FillRect(
            hdc,
            &rect,

            reinterpret_cast<HBRUSH>(
                COLOR_WINDOW + 1
                )
        );

        SetTextColor(
            hdc,

            GetSysColor(
                COLOR_WINDOWTEXT
            )
        );
    }

    SetBkMode(
        hdc,
        TRANSPARENT
    );

    rect.left += 5;

    DrawText(
        hdc,

        szText,

        -1,

        &rect,

        DT_LEFT |
        DT_SINGLELINE |
        DT_VCENTER
    );
}


void HandleCommand(
    HWND hWnd,
    int wmId,
    int wmEvent)
{
    if (wmId ==
        IDC_SIDE_SIZE_LIST &&
        wmEvent ==
        LBN_SELCHANGE)
    {
        int sel =
            static_cast<int>(
                SendMessage(
                    g_AppState.hwndListBox,
                    LB_GETCURSEL,
                    0,
                    0
                )
                );

        if (sel != LB_ERR &&
            sel >= 0 &&
            sel <
            static_cast<int>(
                g_Icons.size()))
        {
            g_SelectedIcon =
                sel;

            g_AppState.selectedSizeIndex =
                sel;

            g_AppState.hCurrentIcon =
                g_Icons[
                    sel
                ].hIcon;

            InvalidateCanvas(hWnd);
        }

        return;
    }

    if (wmId ==
        ID_ZOOM_IN)
    {
        ZoomIn(hWnd);
        return;
    }

    if (wmId ==
        ID_ZOOM_OUT)
    {
        ZoomOut(hWnd);
        return;
    }

    switch (wmId)
    {
    case ID_FILE_OPEN:
    {
        OnOpenFile(hWnd);
        break;
    }

    case ID_FILE_SAVEAS:
    {
        OnSaveFileAs(hWnd);
        break;
    }

    case ID_HELP_ABOUT:
    {
        MessageBox(
            hWnd,

            (local_matrix[_ABOUT][lang] + L"\0").c_str(),

            (local_matrix[_ABOUT_BTN][lang] + L"\0").c_str(),

            MB_OK |
            MB_ICONINFORMATION
        );

        break;
    }
    case ID_LANGUAGE:
    {
        lang = (lang == EN)
            ? RU
            : EN;

        UpdateLanguage(hWnd);

        break;
    }

    case ID_FILE_EXIT:
    {
        DestroyWindow(hWnd);
        break;
    }

    default:
        break;
    }
}


