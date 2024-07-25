// SPDX-License-Identifier: GPL-2.0-only
// Copyright © 2024 Immortal Echoes. All rights reserved.
// Author: mozahzah

#pragma once

#include "IECore.h"

#include "IEMidiTypes.h"

class IEMidiProfileManager
{
public:
    IEMidiProfileManager();
    
public:
    std::filesystem::path GetIEMidiProfilesFilePath() const;
    bool HasProfile(const IEMidiDeviceProfile& MidiDeviceProfile) const;
    IEResult SaveProfile(const IEMidiDeviceProfile& MidiDeviceProfile) const;
    IEResult LoadProfile(IEMidiDeviceProfile& MidiDeviceProfile) const;
    IEResult RemoveProfile(const IEMidiDeviceProfile& MidiDeviceProfile) const;

private:
    std::string ExtractFileContent(const std::filesystem::path& FilePath) const;
};