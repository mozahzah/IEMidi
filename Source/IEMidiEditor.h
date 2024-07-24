// SPDX-License-Identifier: GPL-2.0-only
// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#pragma once

#include "IECore.h"

#include "IEMidiTypes.h"
#include "IEMidiProcessor.h"

class IEMidiEditor
{
public:
    IEMidiEditor() = delete;
    IEMidiEditor(const std::shared_ptr<IEMidiProcessor>& MidiDeviceProcessor) :
        m_MidiDeviceProcessor(MidiDeviceProcessor)
    {}

public:
    void DrawMidiDeviceProfileEditor(IEMidiDeviceProfile& MidiDeviceProfile) const;

private:
    void DrawMidiDevicePropertyEditor(IEMidiDeviceProperty& MidiDeviceProperty, bool& bDeleteRequested) const;
    void DrawInitialOutputMessageEditor(std::vector<unsigned char>& MidiDeviceInitialOutputMidiMessage, bool& bDeleteRequested) const;

private:
    std::shared_ptr<IEMidiProcessor> m_MidiDeviceProcessor;
};