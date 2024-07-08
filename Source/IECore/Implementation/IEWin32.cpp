// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#include "IECore/IERenderer.h"

#if defined (_WIN32)

#include <dwmapi.h>
#include <uxtheme.h>

#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_EDIT 1002
#define ID_TRAY_EXIT 1001

typedef BOOL(WINAPI* pSetPreferredAppMode)(UINT);
WNDPROC g_GlfwWndProc = WNDPROC();

LRESULT IEWndProc(HWND Window, UINT MessageID, WPARAM WordParam, LPARAM LongParam)
{
    IEAssert(g_GlfwWndProc);
    if (IERenderer* const Renderer = reinterpret_cast<IERenderer*>(GetWindowLongPtr(Window, GWLP_USERDATA)))
    {
        switch (MessageID)
        {
            case WM_TRAYICON:
            {
                switch (LOWORD(LongParam))
                {
                    case WM_RBUTTONDOWN:
                    {
                        POINT CursorPos;
                        GetCursorPos(&CursorPos);

                        HMENU TrayIconMenu = CreatePopupMenu();
                        AppendMenu(TrayIconMenu, MF_STRING, ID_TRAY_EDIT, "Edit");
                        AppendMenu(TrayIconMenu, MF_SEPARATOR, 0, nullptr);
                        AppendMenu(TrayIconMenu, MF_STRING, ID_TRAY_EXIT, "Exit");

                        SetForegroundWindow(Window);
                        TrackPopupMenu(TrayIconMenu, TPM_LEFTALIGN | TPM_BOTTOMALIGN | TPM_RIGHTBUTTON, CursorPos.x, CursorPos.y, 0, Window, NULL);
                        DestroyMenu(TrayIconMenu);

                        break;
                    }
                    case WM_LBUTTONDOWN:
                    {
                        Renderer->ShowAppWindow();
                        break;
                    }
                }

                break;
            }
            case WM_COMMAND:
            {
                switch (LOWORD(WordParam))
                {
                    case ID_TRAY_EDIT:
                    {
                        Renderer->ShowAppWindow();
                        break;
                    }
                    case ID_TRAY_EXIT:
                    {
                        Renderer->RequestExit();
                        break;
                    }
                }
                break;
            }
        }
    }
    return CallWindowProc(g_GlfwWndProc, Window, MessageID, WordParam, LongParam);
}

extern void InitializeIEWin32App(IERenderer* Renderer)
{
    if (Renderer)
    {
        const HWND Win32Window = glfwGetWin32Window(Renderer->GetGLFWwindow());

        const BOOL bDarkMode = TRUE;
        DwmSetWindowAttribute(Win32Window, DWMWA_USE_IMMERSIVE_DARK_MODE, &bDarkMode, sizeof(bDarkMode));

        if (HMODULE const Uxtheme = LoadLibraryEx("uxtheme.dll", NULL, LOAD_LIBRARY_SEARCH_SYSTEM32))
        {
            pSetPreferredAppMode SetPreferredAppMode = reinterpret_cast<pSetPreferredAppMode>(GetProcAddress(Uxtheme, MAKEINTRESOURCEA(135)));
            SetPreferredAppMode(2); // dark mode
        }

        NOTIFYICONDATA NotifyIconData;
        NotifyIconData.cbSize = sizeof(NOTIFYICONDATA);
        NotifyIconData.hWnd = Win32Window;
        NotifyIconData.uID = 1;
        NotifyIconData.uVersion = NOTIFYICON_VERSION_4;
        NotifyIconData.uFlags = NIF_ICON | NIF_TIP | NIF_GUID | NIF_MESSAGE;
        NotifyIconData.uCallbackMessage = WM_TRAYICON;
        NotifyIconData.hIcon = reinterpret_cast<HICON>(SendMessage(Win32Window, WM_GETICON, ICON_SMALL, 0));
        lstrcpy(NotifyIconData.szTip, "IEMidi");
        Shell_NotifyIcon(NIM_ADD, &NotifyIconData);

        g_GlfwWndProc = (WNDPROC)GetWindowLongPtr(Win32Window, GWLP_WNDPROC);
        SetWindowLongPtr(Win32Window, GWLP_WNDPROC, (LONG_PTR)IEWndProc);
        SetWindowLongPtr(Win32Window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(Renderer));        
    }
}
#endif