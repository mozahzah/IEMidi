// SPDX-License-Identifier: GPL-2.0-only
// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#include "IEMidiProfileManager.h"

#include "ryml.hpp"
#include "ryml_std.hpp"

#include "IEUtils.h"

uint32_t IEMidiDeviceProperty::MidiDevicePropertyIDGenerator = 0;

static constexpr char IEMIDI_PROFILES_FILENAME[] = "profiles.yaml";
static constexpr char MIDI_PROFILE_PROPERTIES_NODE_NAME[] = "Properties";
static constexpr char MIDI_MESSAGE_TYPE_KEY_NAME[] = "Midi Message Type";
static constexpr char MIDI_TOGGLE_KEY_NAME[] = "Toogle";
static constexpr char MIDI_ACTION_TYPE_KEY_NAME[] = "Midi Action Type";
static constexpr char CONSOLE_COMMAND_KEY_NAME[] = "Console Command";
static constexpr char OPEN_FILE_PATH_KEY_NAME[] = "Open File Path";
static constexpr char MIDI_MESSAGE_KEY_NAME[] = "Midi Message";
static constexpr char INITIAL_OUTPUT_MIDI_MESSAGES_KEY_NAME[] = "Initial Output Midi Messages";

static constexpr uint32_t INITIAL_TREE_NODE_COUNT = 30;
static constexpr uint32_t INITIAL_TREE_ARENA_CHAR_COUNT = 2048;

IEMidiProfileManager::IEMidiProfileManager()
{
    const std::filesystem::path IEMidiConfigFolderPath = IEUtils::GetIEConfigFolderPath();
    if (!IEMidiConfigFolderPath.empty())
    {
        const std::filesystem::path MidiProfilesFilePath = IEMidiConfigFolderPath / IEMIDI_PROFILES_FILENAME;
        const std::string ProfilesFilePathString = MidiProfilesFilePath.string();
        if (!std::filesystem::exists(MidiProfilesFilePath))
        {
            if (std::FILE* const ProfilesFile = std::fopen(ProfilesFilePathString.c_str(), "w"))
            {
                IELOG_SUCCESS("Successfully created profiles settings file %s", ProfilesFilePathString.c_str());
            }
        }
        IELOG_SUCCESS("Using profiles settings file %s", ProfilesFilePathString.c_str());
    }
}

std::filesystem::path IEMidiProfileManager::GetIEMidiProfilesFilePath() const
{
    std::filesystem::path MidiDeviceProfilesFilePath;
    const std::filesystem::path IEMidiConfigFolderPath = IEUtils::GetIEConfigFolderPath();
    if (std::filesystem::exists(IEMidiConfigFolderPath))
    {
        const std::filesystem::path PotentialMidiDeviceProfilesFilePath = IEMidiConfigFolderPath / IEMIDI_PROFILES_FILENAME;
        if (std::filesystem::exists(PotentialMidiDeviceProfilesFilePath))
        {
            MidiDeviceProfilesFilePath = PotentialMidiDeviceProfilesFilePath;
        }
    }
    return MidiDeviceProfilesFilePath;
}

bool IEMidiProfileManager::HasProfile(const IEMidiDeviceProfile& MidiDeviceProfile) const
{
    bool bHasProfile = false;

    const std::filesystem::path MidiProfilesFilePath = GetIEMidiProfilesFilePath();
    if (std::filesystem::exists(MidiProfilesFilePath))
    {
        const std::string Content = ExtractFileContent(MidiProfilesFilePath);

        ryml::Tree MidiProfilesTree;
        MidiProfilesTree.reserve(INITIAL_TREE_NODE_COUNT);
        MidiProfilesTree.reserve_arena(INITIAL_TREE_ARENA_CHAR_COUNT);
        ryml::parse_in_arena(ryml::to_csubstr(Content), &MidiProfilesTree);

        const ryml::ConstNodeRef Root = MidiProfilesTree.rootref();
        if (Root.is_map())
        {
            if (Root.has_child(MidiDeviceProfile.Name.c_str()))
            {
                bHasProfile = true;
            }
        }
    }
    
    return bHasProfile;
}

IEResult IEMidiProfileManager::SaveProfile(const IEMidiDeviceProfile& MidiDeviceProfile) const
{
    IEResult Result(IEResult::Type::Fail, "Failed to save profile");

    const std::filesystem::path MidiProfilesFilePath = GetIEMidiProfilesFilePath();
    if (std::filesystem::exists(MidiProfilesFilePath))
    {
        if (std::FILE* const ProfilesFile = std::fopen(MidiProfilesFilePath.string().c_str(), "w"))
        {
            const std::string Content = ExtractFileContent(MidiProfilesFilePath);

            ryml::Tree MidiProfilesTree;
            MidiProfilesTree.reserve(INITIAL_TREE_NODE_COUNT);
            MidiProfilesTree.reserve_arena(INITIAL_TREE_ARENA_CHAR_COUNT);
            ryml::parse_in_arena(ryml::to_csubstr(Content), &MidiProfilesTree);

            ryml::NodeRef Root = MidiProfilesTree.rootref();
            if (!Root.is_map() && Root.empty())
            {
                Root |= ryml::MAP;
            }

            const char* MidiDeviceName = MidiDeviceProfile.Name.c_str();
            ryml::NodeRef MidiProfileNode = Root[MidiDeviceName];
            if (MidiProfileNode.is_seed())
            {
                MidiProfileNode.create();
                MidiProfileNode |= ryml::MAP;
            }

            ryml::NodeRef MidiProfilePropertiesNode = MidiProfileNode[MIDI_PROFILE_PROPERTIES_NODE_NAME];
            if (MidiProfilePropertiesNode.is_seed())
            {
                MidiProfilePropertiesNode.create();
                MidiProfilePropertiesNode |= ryml::SEQ;
            }
            MidiProfilePropertiesNode.clear_children();
            for (const IEMidiDeviceProperty& MidiDeviceProperty : MidiDeviceProfile.Properties)
            {
                ryml::NodeRef MidiProfilePropertyNode = MidiProfilePropertiesNode.append_child();
                MidiProfilePropertyNode.create();
                MidiProfilePropertyNode |= ryml::MAP;

                MidiProfilePropertyNode[MIDI_MESSAGE_TYPE_KEY_NAME] << static_cast<uint8_t>(MidiDeviceProperty.MidiMessageType);
                MidiProfilePropertyNode[MIDI_TOGGLE_KEY_NAME] << MidiDeviceProperty.bToggle;
                MidiProfilePropertyNode[MIDI_ACTION_TYPE_KEY_NAME] << static_cast<uint8_t>(MidiDeviceProperty.MidiActionType);
                MidiProfilePropertyNode[CONSOLE_COMMAND_KEY_NAME] << MidiDeviceProperty.ConsoleCommand;
                MidiProfilePropertyNode[OPEN_FILE_PATH_KEY_NAME] << MidiDeviceProperty.OpenFilePath;
                MidiProfilePropertyNode[MIDI_MESSAGE_KEY_NAME] << MidiDeviceProperty.MidiMessage;
            }
            
            ryml::NodeRef ProfileInitialOutputMidiMessagesNode = MidiProfileNode[INITIAL_OUTPUT_MIDI_MESSAGES_KEY_NAME];
            if (ProfileInitialOutputMidiMessagesNode.is_seed())
            {
                ProfileInitialOutputMidiMessagesNode.create();
                ProfileInitialOutputMidiMessagesNode |= ryml::SEQ;
            }
            ProfileInitialOutputMidiMessagesNode.clear_children();
            ProfileInitialOutputMidiMessagesNode << MidiDeviceProfile.InitialOutputMidiMessages;

            const size_t EmitSize = ryml::emit_yaml(MidiProfilesTree, ProfilesFile);
            if (EmitSize)
            {
                Result.Type = IEResult::Type::Success;
                Result.Message = std::format("Successfully saved profile {}, into {}", MidiDeviceName, MidiProfilesFilePath.string());
            }

            std::fclose(ProfilesFile);
        }
    }
    return Result;
}

IEResult IEMidiProfileManager::LoadProfile(IEMidiDeviceProfile& MidiDeviceProfile) const
{
    IEResult Result(IEResult::Type::Fail, "Failed to load profile");

    const std::filesystem::path MidiProfilesFilePath = GetIEMidiProfilesFilePath();
    if (std::filesystem::exists(MidiProfilesFilePath))
    {
        Result.Type = IEResult::Type::Success;
        Result.Message = std::format("Successfully found profiles config file {}", MidiProfilesFilePath.string());

        const std::string Content = ExtractFileContent(MidiProfilesFilePath);

        ryml::Tree MidiProfilesTree;
        MidiProfilesTree.reserve(INITIAL_TREE_NODE_COUNT);
        MidiProfilesTree.reserve_arena(INITIAL_TREE_ARENA_CHAR_COUNT);
        ryml::parse_in_arena(ryml::to_csubstr(Content), &MidiProfilesTree);

        const ryml::ConstNodeRef Root = MidiProfilesTree.rootref();
        if (Root.is_map() && Root.has_child(MidiDeviceProfile.Name.c_str()))
        {
            const ryml::ConstNodeRef MidiProfileNode = Root[MidiDeviceProfile.Name.c_str()];
            const ryml::ConstNodeRef MidiProfilePropertiesNode = MidiProfileNode[MIDI_PROFILE_PROPERTIES_NODE_NAME];
            MidiDeviceProfile.Properties.clear();
            MidiDeviceProfile.Properties.reserve(MidiProfilePropertiesNode.num_children());

            for (int ChildPos = 0; ChildPos < MidiProfilePropertiesNode.num_children(); ChildPos++)
            {
                const ryml::ConstNodeRef MidiProfilePropertyNode = MidiProfilePropertiesNode.at(ChildPos);
                IEMidiDeviceProperty& MidiDeviceProperty = MidiDeviceProfile.Properties.emplace_back(MidiDeviceProfile.Name);

                if (MidiProfilePropertyNode.has_child(MIDI_MESSAGE_TYPE_KEY_NAME))
                {
                    uint8_t MidiMessageType = 0;
                    MidiProfilePropertyNode[MIDI_MESSAGE_TYPE_KEY_NAME] >> MidiMessageType;
                    MidiDeviceProperty.MidiMessageType = static_cast<IEMidiMessageType>(MidiMessageType);
                }

                if (MidiProfilePropertyNode.has_child(MIDI_TOGGLE_KEY_NAME))
                {
                    MidiProfilePropertyNode[MIDI_TOGGLE_KEY_NAME] >> MidiDeviceProperty.bToggle;
                }

                if (MidiProfilePropertyNode.has_child(MIDI_ACTION_TYPE_KEY_NAME))
                {
                    uint8_t MidiActionType = 0;
                    MidiProfilePropertyNode[MIDI_ACTION_TYPE_KEY_NAME] >> MidiActionType;
                    MidiDeviceProperty.MidiActionType = static_cast<IEMidiActionType>(MidiActionType);
                }

                if (MidiProfilePropertyNode.has_child(CONSOLE_COMMAND_KEY_NAME))
                {
                    if (!MidiProfilePropertyNode[CONSOLE_COMMAND_KEY_NAME].val().empty())
                    {
                        MidiProfilePropertyNode[CONSOLE_COMMAND_KEY_NAME] >> MidiDeviceProperty.ConsoleCommand;
                    }
                }

                if (MidiProfilePropertyNode.has_child(OPEN_FILE_PATH_KEY_NAME))
                {
                    if (!MidiProfilePropertyNode[OPEN_FILE_PATH_KEY_NAME].val().empty())
                    {
                        MidiProfilePropertyNode[OPEN_FILE_PATH_KEY_NAME] >> MidiDeviceProperty.OpenFilePath;
                    }
                }

                if (MidiProfilePropertyNode.has_child(MIDI_MESSAGE_KEY_NAME))
                {
                    MidiProfilePropertyNode[MIDI_MESSAGE_KEY_NAME] >> MidiDeviceProperty.MidiMessage;
                }
            }

            if (MidiProfileNode.has_child(INITIAL_OUTPUT_MIDI_MESSAGES_KEY_NAME))
            {
                const ryml::ConstNodeRef ProfileInitialOutputMidiMessagesNode = MidiProfileNode[INITIAL_OUTPUT_MIDI_MESSAGES_KEY_NAME];
                ProfileInitialOutputMidiMessagesNode >> MidiDeviceProfile.InitialOutputMidiMessages;
            }

            Result.Type = IEResult::Type::Success;
            Result.Message = std::format("Successfully loaded profile {} from {}", MidiDeviceProfile.Name, MidiProfilesFilePath.string());
        }
    }
    return Result;
}

IEResult IEMidiProfileManager::RemoveProfile(const IEMidiDeviceProfile& MidiDeviceProfile) const
{
    IEResult Result(IEResult::Type::Fail, "Failed to remove profile");
    const std::filesystem::path IEMidiConfigFolderPath = IEUtils::GetIEConfigFolderPath();
    // TODO
    // if (!IEMidiConfigFolderPath.empty())
    // {
    //     const std::filesystem::path MidiProfilesFilePath = IEMidiConfigFolderPath / IEMIDI_PROFILES_FILENAME;
    //     if (std::filesystem::exists(MidiProfilesFilePath))
    //     {
    //         const std::string Content = ExtractFileContent(MidiProfilesFilePath);

    //         ryml::Tree MidiProfilesTree;
    //         MidiProfilesTree.reserve(INITIAL_TREE_NODE_COUNT);
    //         MidiProfilesTree.reserve_arena(INITIAL_TREE_ARENA_CHAR_COUNT);
    //         ryml::parse_in_arena(ryml::to_csubstr(Content), &MidiProfilesTree);

    //         ryml::NodeRef Root = MidiProfilesTree.rootref();
    //         if (Root.is_map())
    //         {
    //             const char* MidiDeviceName = MidiDeviceProfile.Name.c_str();
    //             Root.remove_child(MidiDeviceName);

    //             if (std::FILE* const ProfilesFile = std::fopen(MidiProfilesFilePath.string().c_str(), "w"))
    //             {
    //                 if (!Root.empty())
    //                 {
    //                     size_t EmitSize = ryml::emit_yaml(MidiProfilesTree, ProfilesFile);
    //                 }

    //                 Result.Type = IEResult::Type::Success;
    //                 Result.Message = std::format("Successfully removed profile {}, from {}", MidiDeviceProfile.Name, MidiProfilesFilePath.string());
                    
    //                 std::fclose(ProfilesFile);
    //             }
    //         }
    //     }
    // }
    return Result;
}

std::string IEMidiProfileManager::ExtractFileContent(const std::filesystem::path& FilePath) const
{
    std::string Content;
    if (std::FILE* const File = std::fopen(FilePath.string().c_str(), "rb"))
    {
        std::fseek(File, 0, SEEK_END);
        const long Size = std::ftell(File);
        if (Size)
        {
            Content.resize(Size);
            std::rewind(File);
            std::fread(Content.data(), 1, Size, File);
        }
        std::fclose(File);
    }
    return Content;
}