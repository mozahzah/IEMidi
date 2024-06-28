// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#pragma once

#include "ryml.hpp"
#include "ryml_std.hpp"

#include "IECore.h"

struct IEMidiDeviceProfile
{
    std::string Name;
    uint32_t InputPortNumber = -1;
    uint32_t OutputPortNumber = -1;
};

class IEMidiProfileManager
{
public:
    IEResult SaveProfile(const IEMidiDeviceProfile& MidiDevice) const;
    IEResult LoadProfile(const IEMidiDeviceProfile& MidiDevice) const;
    IEResult RemoveProfile(const IEMidiDeviceProfile& MidiDevice) const;

private:
    std::string ExtractFileContent(const std::filesystem::path& FilePath) const;

private:
    static constexpr uint32_t INITIAL_TREE_NODE_COUNT = 30;
    static constexpr uint32_t INITIAL_TREE_ARENA_CHAR_COUNT = 2048;
};