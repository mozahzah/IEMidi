// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#include "IEUtils.h"

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

    std::filesystem::path GetIEMidiDocumentsFolderPath()
    {
#ifdef _WIN32
        PWSTR DocumentsFolderPath = NULL;
        const HRESULT Result = SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &DocumentsFolderPath);
        if (SUCCEEDED(Result))
        {
            const _bstr_t BstrPDocumentsFolderPath(DocumentsFolderPath);
            const std::wstring StrDocumentsFolderPath(BstrPDocumentsFolderPath, SysStringLen(BstrPDocumentsFolderPath));

            const std::filesystem::path IEMidiDocumentsFolderPath = std::filesystem::path(StrDocumentsFolderPath) / "IEMidi";
            if (std::filesystem::exists(IEMidiDocumentsFolderPath) || std::filesystem::create_directory(IEMidiDocumentsFolderPath))
            {
                return IEMidiDocumentsFolderPath;
            }
        }
#elif defined(__APPLE__)
        const char* HomeFolderPath;
        if ((HomeFolderPath = getenv("HOME")) == NULL)
        {
            HomeFolderPath = getpwuid(getuid())->pw_dir;
        }

        const std::filesystem::path IEMidiDocumentsFolderPath = std::filesystem::path(HomeFolderPath) / "Documents/IEMidi";
        if (std::filesystem::create_directory(IEMidiDocumentsFolderPath))
        {
            return IEMidiDocumentsFolderPath;
        }
#endif  

        IELOG_ERROR("Failed to create ../Documents/IEMidi folder");
        return std::filesystem::path();
    }
}