// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#pragma once

#include "imgui.h"
#include "imgui_internal.h"

namespace ImGui
{
    void StyleIE(ImGuiStyle* StyleDestination = nullptr);
    void ClearCurrentWindowStoredStates();
}