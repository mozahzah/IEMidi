// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#pragma once

#include "IECore.h"
#include "IEMidiProfileManager.h"

class IEMidiEditor
{
public:
    void DrawMidiDeviceProfileEditor(IEMidiDeviceProfile& MidiDeviceProfile) const;

private:
    void DrawMidiDeviceInputPropertyEditor(IEMidiDeviceProperty& MidiDeviceProperty, bool& bDeleteRequested) const;
    void DrawInitialOutputMessageEditor(std::vector<unsigned char>& MidiDeviceInitialOutputMidiMessage, bool& bDeleteRequested) const;
    

private:
    void DrawFileFinder(std::string& SelectedFile) const;
    void DrawFileFinderTree(const std::filesystem::path& CurrentPath, uint8_t Depth, std::string& SelectedFile) const;

private:
    mutable bool m_bFileFinderOpen = false;
    mutable std::filesystem::path m_RootFileFinderSearchPath = std::filesystem::current_path();
};