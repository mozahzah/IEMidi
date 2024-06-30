// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#include "IEMidiProfileManager.h"

#include "ryml.hpp"
#include "ryml_std.hpp"

#include "IEUtils.h"

static constexpr char IEMIDI_PROFILES_FILENAME[] = "profiles.yaml";

static constexpr char PROFILE_PROPERTIES_NODE_NAME[] = "Properties";
static constexpr char MIDI_MESSAGE_TYPE_KEY_NAME[] = "Midi Message Type";
static constexpr char MIDI_PARAMETER_TYPE_KEY_NAME[] = "Midi Parameter Type";
static constexpr char MIDI_MESSAGE_KEY_NAME[] = "Midi Message";
static constexpr char INITIAL_OUTPUT_MIDI_MESSAGES_KEY_NAME[] = "Initial Output Midi Messages";

static constexpr uint32_t INITIAL_TREE_NODE_COUNT = 30;
static constexpr uint32_t INITIAL_TREE_ARENA_CHAR_COUNT = 2048;

IEMidiProfileManager::IEMidiProfileManager()
{
    const std::filesystem::path IEMidiConfigFolderPath = IEUtils::GetIEMidiConfigFolderPath();
    if (!IEMidiConfigFolderPath.empty())
    {
        const std::filesystem::path ProfilesFilePath = IEMidiConfigFolderPath / IEMIDI_PROFILES_FILENAME;
        const std::string ProfilesFilePathString = ProfilesFilePath.string();
        if (!std::filesystem::exists(ProfilesFilePath))
        {
            if (std::FILE* const ProfilesFile = std::fopen(ProfilesFilePathString.c_str(), "w"))
            {
                IELOG_SUCCESS("Successfully created profiles settings file %s", ProfilesFilePathString.c_str());
            }
        }
        IELOG_SUCCESS("Using profiles settings file %s", ProfilesFilePathString.c_str());
    }
}

IEResult IEMidiProfileManager::SaveProfile(const IEMidiDeviceProfile& MidiDeviceProfile) const
{
    IEResult Result(IEResult::Type::Fail, "Failed to save profile");

    const std::filesystem::path IEMidiConfigFolderPath = IEUtils::GetIEMidiConfigFolderPath();
    if (!IEMidiConfigFolderPath.empty())
    {
        const std::filesystem::path ProfilesFilePath = IEMidiConfigFolderPath / IEMIDI_PROFILES_FILENAME;
        if (std::FILE* const ProfilesFile = std::fopen(ProfilesFilePath.string().c_str(), "w"))
        {
            const std::string Content = ExtractFileContent(ProfilesFilePath);

            ryml::Tree ProfilesTree;
            ProfilesTree.reserve(INITIAL_TREE_NODE_COUNT);
            ProfilesTree.reserve_arena(INITIAL_TREE_ARENA_CHAR_COUNT);
            ryml::parse_in_arena(ryml::to_csubstr(Content), &ProfilesTree);

            ryml::NodeRef Root = ProfilesTree.rootref();
            if (!Root.is_map() && Root.empty())
            {
                Root |= ryml::MAP;
            }

            const char* MidiDeviceName = MidiDeviceProfile.Name.c_str();
            ryml::NodeRef ProfileNode = Root[MidiDeviceName];
            if (ProfileNode.is_seed())
            {
                ProfileNode.create();
                ProfileNode |= ryml::MAP;
            }

            ryml::NodeRef ProfilePropertiesNode = ProfileNode[PROFILE_PROPERTIES_NODE_NAME];
            if (ProfilePropertiesNode.is_seed())
            {
                ProfilePropertiesNode.create();
                ProfilePropertiesNode |= ryml::SEQ;
            }
            ProfilePropertiesNode.clear_children();
            for (const IEMidiDeviceProperty& MidiDeviceProperty : MidiDeviceProfile.Properties)
            {
                ryml::NodeRef ProfilePropertyNode = ProfilePropertiesNode.append_child();
                ProfilePropertyNode.create();
                ProfilePropertyNode |= ryml::MAP;

                ProfilePropertyNode[MIDI_MESSAGE_TYPE_KEY_NAME] << static_cast<uint8_t>(MidiDeviceProperty.MidiMessageType);
                ProfilePropertyNode[MIDI_PARAMETER_TYPE_KEY_NAME] << static_cast<uint8_t>(MidiDeviceProperty.MidiParameterType);
                ProfilePropertyNode[MIDI_MESSAGE_KEY_NAME] << MidiDeviceProperty.MidiMessage;
            }
            
            ryml::NodeRef ProfileInitialOutputMidiMessagesNode = ProfileNode[INITIAL_OUTPUT_MIDI_MESSAGES_KEY_NAME];
            if (ProfileInitialOutputMidiMessagesNode.is_seed())
            {
                ProfileInitialOutputMidiMessagesNode.create();
                ProfileInitialOutputMidiMessagesNode |= ryml::SEQ;
            }
            ProfileInitialOutputMidiMessagesNode.clear_children();
            ProfileInitialOutputMidiMessagesNode << MidiDeviceProfile.InitialOutputMidiMessages;

            const size_t EmitSize = ryml::emit_yaml(ProfilesTree, ProfilesFile);
            if (EmitSize)
            {
                Result.Type = IEResult::Type::Success;
                Result.Message = std::format("Successfully saved profile {}, into {}", MidiDeviceName, ProfilesFilePath.string());
            }

            std::fclose(ProfilesFile);
        }
    }
    return Result;
}

IEResult IEMidiProfileManager::LoadProfile(IEMidiDeviceProfile& MidiDeviceProfile) const
{
    IEResult Result(IEResult::Type::Fail, "Failed to load profile");

    const std::filesystem::path IEMidiConfigFolderPath = IEUtils::GetIEMidiConfigFolderPath();
    if (!IEMidiConfigFolderPath.empty())
    {
        const std::filesystem::path ProfilesFilePath = IEMidiConfigFolderPath / IEMIDI_PROFILES_FILENAME;
        if (std::filesystem::exists(ProfilesFilePath))
        {
            Result.Type = IEResult::Type::Success;
            Result.Message = std::format("Successfully found profiles config file {}", ProfilesFilePath.string());

            const std::string Content = ExtractFileContent(ProfilesFilePath);

            ryml::Tree ProfilesTree;
            ProfilesTree.reserve(INITIAL_TREE_NODE_COUNT);
            ProfilesTree.reserve_arena(INITIAL_TREE_ARENA_CHAR_COUNT);
            ryml::parse_in_arena(ryml::to_csubstr(Content), &ProfilesTree);

            const ryml::ConstNodeRef Root = ProfilesTree.rootref();
            if (Root.is_map())
            {
                const ryml::ConstNodeRef ProfileNode = Root[MidiDeviceProfile.Name.c_str()];
                const ryml::ConstNodeRef ProfilePropertiesNode = ProfileNode[PROFILE_PROPERTIES_NODE_NAME];
                MidiDeviceProfile.Properties.resize(ProfilePropertiesNode.num_children());

                for (int ChildPos = 0; ChildPos < ProfilePropertiesNode.num_children(); ChildPos++)
                {
                    const ryml::ConstNodeRef ProfilePropertyNode = ProfilePropertiesNode.at(ChildPos);

                    if (ProfilePropertyNode.has_child(MIDI_MESSAGE_TYPE_KEY_NAME))
                    {
                        uint8_t MidiMessageType = 0;
                        ProfilePropertyNode[MIDI_MESSAGE_TYPE_KEY_NAME] >> MidiMessageType;
                        MidiDeviceProfile.Properties[ChildPos].MidiMessageType = static_cast<IEMidiMessageType>(MidiMessageType);
                    }
                    
                    if (ProfilePropertyNode.has_child(MIDI_PARAMETER_TYPE_KEY_NAME))
                    {
                        uint8_t MidiParameterType = 0;
                        ProfilePropertyNode[MIDI_PARAMETER_TYPE_KEY_NAME] >> MidiParameterType;
                        MidiDeviceProfile.Properties[ChildPos].MidiParameterType = static_cast<IEMidiParameterType>(MidiParameterType);
                    }

                    if (ProfilePropertyNode.has_child(MIDI_MESSAGE_KEY_NAME))
                    {
                        ProfilePropertyNode[MIDI_MESSAGE_KEY_NAME] >> MidiDeviceProfile.Properties[ChildPos].MidiMessage;
                    }
                }

                if (ProfileNode.has_child(INITIAL_OUTPUT_MIDI_MESSAGES_KEY_NAME))
                {
                    const ryml::ConstNodeRef ProfileInitialOutputMidiMessagesNode = ProfileNode[INITIAL_OUTPUT_MIDI_MESSAGES_KEY_NAME];
                    ProfileInitialOutputMidiMessagesNode >> MidiDeviceProfile.InitialOutputMidiMessages;
                }

                Result.Type = IEResult::Type::Success;
                Result.Message = std::format("Successfully loaded profile {} from {}", MidiDeviceProfile.Name, ProfilesFilePath.string());
            }
        }
    }
    return Result;
}

IEResult IEMidiProfileManager::RemoveProfile(const IEMidiDeviceProfile& MidiDeviceProfile) const
{
    IEResult Result(IEResult::Type::Fail, "Failed to remove profile");
    const std::filesystem::path IEMidiConfigFolderPath = IEUtils::GetIEMidiConfigFolderPath();
    // TODO
    // if (!IEMidiConfigFolderPath.empty())
    // {
    //     const std::filesystem::path ProfilesFilePath = IEMidiConfigFolderPath / IEMIDI_PROFILES_FILENAME;
    //     if (std::filesystem::exists(ProfilesFilePath))
    //     {
    //         const std::string Content = ExtractFileContent(ProfilesFilePath);

    //         ryml::Tree ProfilesTree;
    //         ProfilesTree.reserve(INITIAL_TREE_NODE_COUNT);
    //         ProfilesTree.reserve_arena(INITIAL_TREE_ARENA_CHAR_COUNT);
    //         ryml::parse_in_arena(ryml::to_csubstr(Content), &ProfilesTree);

    //         ryml::NodeRef Root = ProfilesTree.rootref();
    //         if (Root.is_map())
    //         {
    //             const char* MidiDeviceName = MidiDeviceProfile.Name.c_str();
    //             Root.remove_child(MidiDeviceName);

    //             if (std::FILE* const ProfilesFile = std::fopen(ProfilesFilePath.string().c_str(), "w"))
    //             {
    //                 if (!Root.empty())
    //                 {
    //                     size_t EmitSize = ryml::emit_yaml(ProfilesTree, ProfilesFile);
    //                 }

    //                 Result.Type = IEResult::Type::Success;
    //                 Result.Message = std::format("Successfully removed profile {}, from {}", MidiDeviceProfile.Name, ProfilesFilePath.string());
                    
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