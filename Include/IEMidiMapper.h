// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#pragma once

#include <memory>

#include "IERenderer.h"

class IEMidiMapper
{
public:
    IEMidiMapper() : m_Renderer(std::make_unique<IERenderer_Vulkan>()) {};

public:
    IERenderer& GetRenderer() const { return *m_Renderer; }

private:
    std::unique_ptr<IERenderer> m_Renderer;
};
