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
                        AppendMenu(TrayIconMenu, MF_STRING, ID_TRAY_EDIT, TEXT("Edit"));
                        AppendMenu(TrayIconMenu, MF_SEPARATOR, 0, nullptr);
                        AppendMenu(TrayIconMenu, MF_STRING, ID_TRAY_EXIT, TEXT("Exit"));

                        SetForegroundWindow(Window);
                        TrackPopupMenu(TrayIconMenu, TPM_LEFTALIGN | TPM_BOTTOMALIGN | TPM_RIGHTBUTTON, CursorPos.x, CursorPos.y, 0, Window, NULL);
                        DestroyMenu(TrayIconMenu);

                        break;
                    }
                    case WM_LBUTTONDOWN:
                    {
                        Renderer->RestoreAppWindow();
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
                        Renderer->RestoreAppWindow();
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

        if (HMODULE const Uxtheme = LoadLibraryEx(TEXT("uxtheme.dll"), NULL, LOAD_LIBRARY_SEARCH_SYSTEM32))
        {
            pSetPreferredAppMode SetPreferredAppMode = reinterpret_cast<pSetPreferredAppMode>(GetProcAddress(Uxtheme, MAKEINTRESOURCEA(135)));
            SetPreferredAppMode(2); // dark mode
        }

        NOTIFYICONDATA TrayIconData;
        TrayIconData.cbSize = sizeof(NOTIFYICONDATA);
        TrayIconData.hWnd = Win32Window;
        TrayIconData.uID = 1;
        TrayIconData.uVersion = NOTIFYICON_VERSION_4;
        TrayIconData.uFlags = NIF_ICON | NIF_TIP | NIF_GUID | NIF_MESSAGE;
        TrayIconData.uCallbackMessage = WM_TRAYICON;
        TrayIconData.hIcon = reinterpret_cast<HICON>(SendMessage(Win32Window, WM_GETICON, ICON_SMALL, 0));
        lstrcpy(TrayIconData.szTip, TEXT("IEMidi"));
        Shell_NotifyIcon(NIM_ADD, &TrayIconData);

        g_GlfwWndProc = (WNDPROC)GetWindowLongPtr(Win32Window, GWLP_WNDPROC);
        SetWindowLongPtr(Win32Window, GWLP_WNDPROC, (LONG_PTR)IEWndProc);
        SetWindowLongPtr(Win32Window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(Renderer));      
    }
}

extern void ShowRunningInBackgroundWin32Notification(const IERenderer* Renderer)
{
    if (Renderer)
    {
        const HWND Win32Window = glfwGetWin32Window(Renderer->GetGLFWwindow());

        NOTIFYICONDATA NotificationBalloonData = {};
        NotificationBalloonData.cbSize = sizeof(NOTIFYICONDATA);
        NotificationBalloonData.hWnd = Win32Window;
        NotificationBalloonData.uID = 2;
        NotificationBalloonData.uVersion = NOTIFYICON_VERSION_4;
        NotificationBalloonData.uFlags = NIF_INFO | NIF_ICON;
        NotificationBalloonData.dwInfoFlags = NIIF_INFO;
        NotificationBalloonData.hIcon = reinterpret_cast<HICON>(SendMessage(Win32Window, WM_GETICON, ICON_SMALL, 0));
        lstrcpy(NotificationBalloonData.szTip, TEXT("IEMidi"));
        lstrcpy(NotificationBalloonData.szInfo, TEXT("IE Midi is running in the background"));
        
        Shell_NotifyIcon(NIM_ADD, &NotificationBalloonData);
        Shell_NotifyIcon(NIM_DELETE, &NotificationBalloonData);
    }
}
#endif