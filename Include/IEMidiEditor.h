// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#pragma once

#include "IECore.h"
#include "IEMidiProfileManager.h"

class IEMidiEditor
{
public:
    void DrawMidiDeviceProfileEditor(IEMidiDeviceProfile& MidiDeviceProfile) const;

private:
    void DrawMidiDeviceInputPropertyEditor(IEMidiDeviceProperty& MidiDeviceProperty) const;
    void DrawInitialOutputMessageEditor(std::vector<unsigned char>& MidiDeviceInitialOutputMidiMessage) const;
};