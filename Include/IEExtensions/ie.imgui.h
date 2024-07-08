// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#pragma once

#include "imgui.h"
#include "imgui_internal.h"

#include "IECore/IECore.h"

namespace ImGui
{
    /* Styles */
    void StyleIE(ImGuiStyle* StyleDestination = nullptr);

    /* Widgets */
    void FileFinder(const char* Label, int Depth, std::string& SelectedFile);
    bool RedButton(const char* Label);
    bool GreenButton(const char* Label);

    /* Other */
    void ClearCurrentWindowStoredStates();
}