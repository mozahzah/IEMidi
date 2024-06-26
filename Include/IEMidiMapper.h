// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#pragma once

#include "IERenderer.h"

class IEMidiMapper
{
public:
#ifdef GLFW_INCLUDE_VULKAN
    IEMidiMapper() : m_Renderer(std::make_unique<IERenderer_Vulkan>()) {};
#endif

public:
    IERenderer& GetRenderer() const { return *m_Renderer; }
    void StyleColorsIE(const ImGuiStyle& Style);

private:
    std::unique_ptr<IERenderer> m_Renderer;
};
