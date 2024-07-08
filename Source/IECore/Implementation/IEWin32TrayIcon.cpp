// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#include "IECore/IERenderer.h"

#if defined (_WIN32)

#define GLFW_EXPOSE_NATIVE_WIN32 1
#define WM_TRAYICON (WM_USER + 1)
#define ID_TRAY_EXIT 1001

WNDPROC g_GlfwWndProc = WNDPROC();

LRESULT IEWndProc(HWND Window, UINT MessageID, WPARAM WordParam, LPARAM LongParam)
{
    IEAssert(m_GlfwWndProc);
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
                        AppendMenu(TrayIconMenu, MF_STRING, ID_TRAY_EXIT, "Exit");

                        SetForegroundWindow(Window);
                        TrackPopupMenu(TrayIconMenu, TPM_LEFTALIGN | TPM_BOTTOMALIGN | TPM_RIGHTBUTTON, CursorPos.x, CursorPos.y, 0, Window, NULL);
                        DestroyMenu(TrayIconMenu);

                        break;
                    }
                    case WM_LBUTTONDOWN:
                    {
                        Renderer->RestoreWindow();
                        break;
                    }
                }

                break;
            }
            case WM_COMMAND:
            {
                if (LOWORD(WordParam) == ID_TRAY_EXIT)
                {
                    Renderer->RequestExit();
                }
                break;
            }
        }
    }
    return CallWindowProc(m_GlfwWndProc, Window, MessageID, WordParam, LongParam);
}

void InitializeIEWin32TrayIcon(IERenderer* Renderer)
{
    if (Renderer)
    {
        const HWND Win32Window = glfwGetWin32Window(Renderer->GetGLFWwindow());

        NOTIFYICONDATA NotifyIconData;
        NotifyIconData.cbSize = sizeof(NOTIFYICONDATA);
        NotifyIconData.hWnd = Win32Window;
        NotifyIconData.uID = 1;
        NotifyIconData.uVersion = NOTIFYICON_VERSION_4;
        NotifyIconData.uFlags = NIF_ICON | NIF_TIP | NIF_GUID | NIF_MESSAGE;
        NotifyIconData.uCallbackMessage = WM_TRAYICON;
        NotifyIconData.hIcon = reinterpret_cast<HICON>(SendMessage(Win32Window, WM_GETICON, ICON_SMALL, 0));
        Shell_NotifyIcon(NIM_ADD, &NotifyIconData);

        g_GlfwWndProc = (WNDPROC)GetWindowLongPtr(Win32Window, GWLP_WNDPROC);
        SetWindowLongPtr(Win32Window, GWLP_WNDPROC, (LONG_PTR)IEWndProc);
        SetWindowLongPtr(Win32Window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(Renderer));
    }
}
#endif