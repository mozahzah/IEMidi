// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#include "IEMidiProfileManager.h"

#include "IEUtils.h"

static constexpr uint32_t INITIAL_TREE_NODE_COUNT = 30;
static constexpr uint32_t INITIAL_TREE_ARENA_CHAR_COUNT = 2048;

IEResult IEMidiProfileManager::SaveProfile(const IEMidiDeviceProfile& MidiDevice) const
{
    IEResult Result(IEResult::Type::Fail, "Failed to save profile");

    const std::filesystem::path IEMidiDocumentsFolderPath = IEUtils::GetIEMidiDocumentsFolderPath();
    if (!IEMidiDocumentsFolderPath.empty())
    {
        const std::filesystem::path ProfilesFilePath = IEMidiDocumentsFolderPath / "profiles.yaml";
        if (std::filesystem::exists(ProfilesFilePath))
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

            const char* MidiDeviceName = MidiDevice.Name.c_str();
            ryml::NodeRef ProfileNode = Root[MidiDeviceName];
            if (ProfileNode.is_seed())
            {
                ProfileNode.create();
                ProfileNode |= ryml::MAP;
            }

            // Save Midi Device Properties 
            ProfileNode["InputMidiPortNumber"] << MidiDevice.InputPortNumber;
            ProfileNode["OutputMidiPortNumber"] << MidiDevice.OutputPortNumber;

            if (std::FILE* const ProfilesFile = std::fopen(ProfilesFilePath.string().c_str(), "r+"))
            {
                const size_t EmitSize = ryml::emit_yaml(ProfilesTree, ProfilesFile);
                if (EmitSize)
                {
                    Result.Type = IEResult::Type::Success;
                    Result.Message = std::format("Successfully saved profile {}, into {}", MidiDeviceName, ProfilesFilePath.string());
                }
                std::fclose(ProfilesFile);
            }
        }
    }
    return Result;
}

IEResult IEMidiProfileManager::LoadProfile(const IEMidiDeviceProfile& MidiDevice) const
{
    IEResult Result(IEResult::Type::Fail, "Failed to load profile");

    const std::filesystem::path IEMidiDocumentsFolderPath = IEUtils::GetIEMidiDocumentsFolderPath();
    if (!IEMidiDocumentsFolderPath.empty())
    {
        const std::filesystem::path ProfilesFilePath = IEMidiDocumentsFolderPath / "profiles.yaml";
        if (std::filesystem::exists(ProfilesFilePath))
        {
            const std::string Content = ExtractFileContent(ProfilesFilePath);

            ryml::Tree ProfilesTree;
            ProfilesTree.reserve(INITIAL_TREE_NODE_COUNT);
            ProfilesTree.reserve_arena(INITIAL_TREE_ARENA_CHAR_COUNT);
            ryml::parse_in_arena(ryml::to_csubstr(Content), &ProfilesTree);

            ryml::ConstNodeRef Root = ProfilesTree.rootref();
            if (Root.is_map())
            {
                // Load into the app

                Result.Type = IEResult::Type::Success;
                Result.Message = std::format("Successfully loaded profile {}, into {}", MidiDevice.Name, ProfilesFilePath.string());
            }
        }
    }
    return Result;
}

IEResult IEMidiProfileManager::RemoveProfile(const IEMidiDeviceProfile& MidiDevice) const
{
    IEResult Result(IEResult::Type::Fail, "Failed to remove profile");
    const std::filesystem::path IEMidiDocumentsFolderPath = IEUtils::GetIEMidiDocumentsFolderPath();
    if (!IEMidiDocumentsFolderPath.empty())
    {
        const std::filesystem::path ProfilesFilePath = IEMidiDocumentsFolderPath / "profiles.yaml";
        if (std::filesystem::exists(ProfilesFilePath))
        {
            const std::string Content = ExtractFileContent(ProfilesFilePath);

            ryml::Tree ProfilesTree;
            ProfilesTree.reserve(INITIAL_TREE_NODE_COUNT);
            ProfilesTree.reserve_arena(INITIAL_TREE_ARENA_CHAR_COUNT);
            ryml::parse_in_arena(ryml::to_csubstr(Content), &ProfilesTree);

            ryml::NodeRef Root = ProfilesTree.rootref();
            if (Root.is_map())
            {
                const char* MidiDeviceName = MidiDevice.Name.c_str();
                Root.remove_child(MidiDeviceName);

                if (std::FILE* const ProfilesFile = std::fopen(ProfilesFilePath.string().c_str(), "w"))
                {
                    if (!Root.empty())
                    {
                        size_t EmitSize = ryml::emit_yaml(ProfilesTree, ProfilesFile);
                    }

                    Result.Type = IEResult::Type::Success;
                    Result.Message = std::format("Successfully removed profile {}, from {}", MidiDevice.Name, ProfilesFilePath.string());
                    
                    std::fclose(ProfilesFile);
                }
            }
        }
    }
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