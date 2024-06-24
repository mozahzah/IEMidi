// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#include "IEUtils.h"

namespace IEUtils
{
    /* Path Search Algorithms */

    std::filesystem::path FindFolderPathDownwards(const std::filesystem::path& SearchDirectory, const std::filesystem::path& FolderName)
    {
        std::filesystem::path SanitizedFolderName = FolderName;
        std::filesystem::path CurrentPath = SearchDirectory;
        SanitizedFolderName.make_preferred();
        CurrentPath.make_preferred();

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
        std::filesystem::path SanitizedFolderName = FolderName;
        std::filesystem::path CurrentPath = SearchDirectory;
        SanitizedFolderName.make_preferred();
        CurrentPath.make_preferred();
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
}