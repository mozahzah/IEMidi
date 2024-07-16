// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#pragma once

#include "imgui.h"
#include "imgui_internal.h"

#include "IECore/IECore.h"

namespace ImGui
{
    /* Formating */
    void SetSmartCursorPosX(float X);
    void SetSmartCursorPosY(float Y);
    void SetSmartCursorPos(const ImVec2& Pos);
    void SetSmartCursorPosXRelative(float XMultiplier);
    void SetSmartCursorPosYRelative(float YMultiplier);
    void SetSmartCursorPosRelative(const ImVec2& XYMultiplier);

    /* Text */
    void WindowPositionedText(float XMultiplier, float YMultiplier, const char* Fmt, ...);
    void CenteredText(const char* Fmt, ...);

    /* Widgets */
    void FileFinder(const char* Label, int Depth, std::string& SelectedFile);
    
    /* Other */
    void ClearCurrentWindowStoredStates();

    namespace IEStyle
    {
        void StyleIE(ImGuiStyle* StyleDestination = nullptr);

        /* Fonts */
        ImFont* GetBoldFont();
        ImFont* GetSubtitleFont();
        ImFont* GetTitleFont();
        
        /* Widgets */
        ImVec2 GetDefaultButtonSize();
        ImVec2 GetSquareButtonSize();

        bool DefaultButton(const char* Label);
        bool SquareButton(const char* Label);
        bool RedButton(const char* Label);
        bool GreenButton(const char* Label);
    }
}