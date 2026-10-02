#include "framework.h"

int WINAPI WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    UNREFERENCED_PARAMETER(
        hPrevInstance
    );

    UNREFERENCED_PARAMETER(
        lpCmdLine
    );

    INITCOMMONCONTROLSEX icex;

    icex.dwSize =
        sizeof(INITCOMMONCONTROLSEX);

    icex.dwICC =
        ICC_WIN95_CLASSES;

    InitCommonControlsEx(
        &icex
    );

    if (!InitApplication(
        hInstance))
    {
        return FALSE;
    }

    if (!InitInstance(
        hInstance,
        nCmdShow))
    {
        return FALSE;
    }

    MSG msg;

    while (GetMessage(
        &msg,
        NULL,
        0,
        0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return static_cast<int>(
        msg.wParam
        );
}

LRESULT CALLBACK WndProc(
    HWND hWnd,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        CreateAppToolbar(
            hWnd
        );

        CreateZoomToolbar(
            hWnd
        );

        CreateSidebar(
            hWnd
        );

        break;
    }


    case WM_ERASEBKGND:
    {
        return 1;
    }


    case WM_SIZE:
    {
        int width =
            LOWORD(lParam);

        int height =
            HIWORD(lParam);

        if (g_AppState.hwndToolbar)
        {
            MoveWindow(
                g_AppState.hwndToolbar,

                0,
                0,

                width,
                TOOLBARHEIGHT,

                TRUE
            );
        }

        if (g_hZoomToolbar)
        {
            MoveWindow(
                g_hZoomToolbar,

                0,
                TOOLBARHEIGHT,

                width,
                ZOOM_TOOLBAR_HEIGHT,

                TRUE
            );

            SendMessage(
                g_hZoomToolbar,
                TB_AUTOSIZE,
                0,
                0
            );
        }

        if (g_AppState.hwndListBox)
        {
            int sidebarTop =
                TOOLBARHEIGHT +
                ZOOM_TOOLBAR_HEIGHT;

            int sidebarHeight =
                height -
                sidebarTop;

            if (sidebarHeight < 0)
                sidebarHeight = 0;

            MoveWindow(
                g_AppState.hwndListBox,

                0,
                sidebarTop,

                SIDEBAR_WIDTH,
                sidebarHeight,

                TRUE
            );
        }

        InvalidateRect(
            hWnd,
            NULL,
            FALSE
        );

        break;
    }


    case WM_COMMAND:
    {
        int wmId =
            LOWORD(wParam);

        int wmEvent =
            HIWORD(wParam);

        HandleCommand(
            hWnd,
            wmId,
            wmEvent
        );

        break;
    }


    case WM_MOUSEWHEEL:
    {
        POINT pt;

        pt.x =
            static_cast<int>(
                static_cast<short>(
                    LOWORD(lParam)
                    )
                );

        pt.y =
            static_cast<int>(
                static_cast<short>(
                    HIWORD(lParam)
                    )
                );

        ScreenToClient(
            hWnd,
            &pt
        );

        if (GET_KEYSTATE_WPARAM(wParam) &
            MK_CONTROL)
        {
            short wheelDelta =
                GET_WHEEL_DELTA_WPARAM(
                    wParam
                );

            if (wheelDelta > 0)
            {
                SetZoom(
                    hWnd,
                    g_Zoom * 1.15,
                    pt.x,
                    pt.y
                );
            }
            else if (wheelDelta < 0)
            {
                SetZoom(
                    hWnd,
                    g_Zoom / 1.15,
                    pt.x,
                    pt.y
                );
            }

            return 0;
        }

        break;
    }


    case WM_LBUTTONDOWN:
    {
        POINT pt;

        pt.x =
            static_cast<int>(
                static_cast<short>(
                    LOWORD(lParam)
                    )
                );

        pt.y =
            static_cast<int>(
                static_cast<short>(
                    HIWORD(lParam)
                    )
                );

        int canvasTop =
            TOOLBARHEIGHT +
            ZOOM_TOOLBAR_HEIGHT;

        if (pt.x >= SIDEBAR_WIDTH &&
            pt.y >= canvasTop)
        {
            g_IsDragging = true;

            g_LastMousePoint =
                pt;

            SetCapture(
                hWnd
            );

            SetCursor(
                LoadCursor(
                    NULL,
                    IDC_SIZEALL
                )
            );

            return 0;
        }

        break;
    }


    case WM_MOUSEMOVE:
    {
        if (g_IsDragging)
        {
            POINT pt;

            pt.x =
                static_cast<int>(
                    static_cast<short>(
                        LOWORD(lParam)
                        )
                    );

            pt.y =
                static_cast<int>(
                    static_cast<short>(
                        HIWORD(lParam)
                        )
                    );

            int dx =
                pt.x -
                g_LastMousePoint.x;

            int dy =
                pt.y -
                g_LastMousePoint.y;

            g_PanX +=
                static_cast<double>(
                    dx
                    );

            g_PanY +=
                static_cast<double>(
                    dy
                    );

            g_LastMousePoint =
                pt;

            InvalidateCanvas(
                hWnd
            );

            return 0;
        }

        break;
    }


    case WM_LBUTTONUP:
    {
        if (g_IsDragging)
        {
            g_IsDragging = false;

            ReleaseCapture();

            SetCursor(
                LoadCursor(
                    NULL,
                    IDC_ARROW
                )
            );

            return 0;
        }

        break;
    }


    case WM_CANCELMODE:
    {
        if (g_IsDragging)
        {
            g_IsDragging = false;

            ReleaseCapture();

            SetCursor(
                LoadCursor(
                    NULL,
                    IDC_ARROW
                )
            );
        }

        break;
    }


    case WM_DRAWITEM:
    {
        DRAWITEMSTRUCT* pDrawItem =
            reinterpret_cast<DRAWITEMSTRUCT*>(
                lParam
                );

        if (pDrawItem->CtlID ==
            IDC_SIDE_SIZE_LIST)
        {
            DrawSidebarItems(
                hWnd,
                pDrawItem
            );

            return TRUE;
        }

        break;
    }


    case WM_PAINT:
    {
        PAINTSTRUCT ps;

        HDC hdc =
            BeginPaint(
                hWnd,
                &ps
            );

        RECT rectClient;

        GetClientRect(
            hWnd,
            &rectClient
        );

        int width =
            rectClient.right -
            rectClient.left;

        int height =
            rectClient.bottom -
            rectClient.top;

        if (width <= 0 ||
            height <= 0)
        {
            EndPaint(
                hWnd,
                &ps
            );

            return 0;
        }

        HDC memDC =
            CreateCompatibleDC(
                hdc
            );

        if (!memDC)
        {
            EndPaint(
                hWnd,
                &ps
            );

            return 0;
        }

        HBITMAP memBitmap =
            CreateCompatibleBitmap(
                hdc,
                width,
                height
            );

        if (!memBitmap)
        {
            DeleteDC(memDC);

            EndPaint(
                hWnd,
                &ps
            );

            return 0;
        }

        HBITMAP oldBitmap =
            static_cast<HBITMAP>(
                SelectObject(
                    memDC,
                    memBitmap
                )
                );

        HBRUSH hBackground =
            reinterpret_cast<HBRUSH>(
                COLOR_WINDOW + 1
                );

        FillRect(
            memDC,
            &rectClient,
            hBackground
        );

        RECT canvasRect =
            rectClient;

        canvasRect.left =
            SIDEBAR_WIDTH;

        canvasRect.top =
            TOOLBARHEIGHT +
            ZOOM_TOOLBAR_HEIGHT;

        HBRUSH hCanvasBrush =
            CreateSolidBrush(
                RGB(
                    245,
                    245,
                    245
                )
            );

        FillRect(
            memDC,
            &canvasRect,
            hCanvasBrush
        );

        DeleteObject(
            hCanvasBrush
        );

        DrawIconCenter(
            hWnd,
            memDC,
            rectClient
        );

        BitBlt(
            hdc,
            ps.rcPaint.left,
            ps.rcPaint.top,

            ps.rcPaint.right -
            ps.rcPaint.left,

            ps.rcPaint.bottom -
            ps.rcPaint.top,

            memDC,

            ps.rcPaint.left,
            ps.rcPaint.top,

            SRCCOPY
        );

        SelectObject(
            memDC,
            oldBitmap
        );

        DeleteObject(
            memBitmap
        );

        DeleteDC(
            memDC
        );

        EndPaint(
            hWnd,
            &ps
        );

        break;
    }


    case WM_DESTROY:
    {
        if (g_IsDragging)
        {
            g_IsDragging = false;

            ReleaseCapture();
        }

        ClearLoadedIcons();

        g_AppState.hCurrentIcon =
            NULL;

        PostQuitMessage(
            0
        );

        break;
    }


    default:
        return DefWindowProc(
            hWnd,
            message,
            wParam,
            lParam
        );
    }

    return 0;
}
