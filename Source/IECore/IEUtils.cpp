// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#include "IECore/IEUtils.h"

namespace IEUtils
{
    /* Path Search Algorithms */

    std::filesystem::path FindFolderPathDownwards(const std::filesystem::path& SearchDirectory, const std::filesystem::path& FolderName)
    {
        const std::filesystem::path SanitizedFolderName(FolderName);
        std::filesystem::path CurrentPath(SearchDirectory);
        if (std::filesystem::exists(CurrentPath) && std::filesystem::is_directory(CurrentPath))
        {
            std::stack<std::filesystem::path> Directories;
            Directories.push(CurrentPath);

            while (!Directories.empty())
            {
                for (const std::filesystem::directory_entry& Entry : std::filesystem::directory_iterator(CurrentPath))
                {
                    if (Entry.is_directory())
                    {
                        std::filesystem::path EntryPath = Entry.path();
                        if (EntryPath.string().ends_with(SanitizedFolderName.string()))
                        {
                            return EntryPath;
                        }
                        Directories.push(EntryPath);
                    }
                }
                Directories.pop();
            }
        }
        return std::filesystem::path();
    }

    std::filesystem::path FindFolderPathUpwards(const std::filesystem::path& SearchDirectory, const std::filesystem::path& FolderName)
    {
        const std::filesystem::path SanitizedFolderName(FolderName);
        std::filesystem::path CurrentPath(SearchDirectory);
        if (std::filesystem::exists(CurrentPath) && std::filesystem::is_directory(CurrentPath))
        {
            while (CurrentPath != CurrentPath.parent_path())
            {
                for (const std::filesystem::directory_entry& Entry : std::filesystem::directory_iterator(CurrentPath))
                {
                    if (std::filesystem::is_directory(Entry))
                    {
                        std::filesystem::path EntryPath = Entry.path();
                        if (EntryPath.string().ends_with(SanitizedFolderName.string()))
                        {
                            return EntryPath;
                        }
                    }
                }
                CurrentPath = CurrentPath.parent_path();
            }
        }
        return std::filesystem::path();
    }

    std::filesystem::path GetIEMidiConfigFolderPath()
    {
        static constexpr char IEMidiConfigFolderName[] = "IEMidi";
#ifdef _WIN32
        PWSTR AppDataFolderPath = NULL;
        const HRESULT Result = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &AppDataFolderPath);
        if (SUCCEEDED(Result))
        {
            const _bstr_t BstrAppDataFolderPath(AppDataFolderPath);
            const std::wstring StrAppDataFolderPath(BstrAppDataFolderPath, SysStringLen(BstrAppDataFolderPath));

            const std::filesystem::path IEMidiConfigFolderPath = std::filesystem::path(StrAppDataFolderPath) / IEMidiConfigFolderName;
            if (std::filesystem::exists(IEMidiConfigFolderPath) || std::filesystem::create_directory(IEMidiConfigFolderPath))
            {
                return IEMidiConfigFolderPath;
            }
        }
#elif defined(__APPLE__)
        const char* HomeFolderPath;
        if ((HomeFolderPath = getenv("HOME")) == NULL)
        {
            HomeFolderPath = getpwuid(getuid())->pw_dir;
        }

        const std::filesystem::path IEMidiConfigFolderPath = std::filesystem::path(HomeFolderPath) / IEMidiConfigFolderName;
        if (std::filesystem::exists(IEMidiConfigFolderPath) || std::filesystem::create_directory(IEMidiConfigFolderPath))
        {
            return IEMidiConfigFolderPath;
        }
#endif  

        IELOG_ERROR("Failed to create IEMidiConfig folder");
        return std::filesystem::path();
    }

    bool IsFileHidden(const std::filesystem::path& Path)
    {
        bool bIsHidden = false;
        if (std::filesystem::exists(Path))
        {
#ifdef _WIN32
            const DWORD Attributes = GetFileAttributes(Path.string().c_str());
            if ((Attributes & FILE_ATTRIBUTE_HIDDEN) != 0)
            {
                bIsHidden = true;
            }
#elif defined(__APPLE__)
            const std::string FileName = Path.filename().string();
            if (!FileName.empty() && FileName[0] == '.')
            {
                bIsHidden = true;
            }
#endif
        }
        return bIsHidden;
    }
}