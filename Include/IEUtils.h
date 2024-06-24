// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#pragma once

#include "IECore.h"

namespace IEUtils
{
    /* String Casting */

    template<typename ToCharType, typename FromCharType>
    struct StringCastImpl 
    {
        static std::basic_string<ToCharType> Cast(const FromCharType* String)
        {
            static_assert(sizeof(ToCharType) != sizeof(ToCharType), "StringCast specialization not implemented for this type combination.");
            return std::basic_string<ToCharType>();
        }
    };

    template<typename CharType>
    struct StringCastImpl<CharType, CharType> 
    {
        static std::basic_string<CharType> Cast(const CharType* String) 
        {
            return std::basic_string<CharType>(String);
        }
    };

    template<>
    struct StringCastImpl<char, wchar_t> 
    {
        static std::string Cast(const wchar_t* String) 
        {
            std::mbstate_t State = std::mbstate_t();
            size_t Size = std::wcsrtombs(nullptr, &String, 0, &State);
            std::vector<char> Buffer(Size + 1);
            std::wcsrtombs(Buffer.data(), &String, Size, &State);
            Buffer[Size] = '\0';
            return std::string(Buffer.data());
        }
    };

    template<typename ToCharType, typename FromCharType>
    std::basic_string<ToCharType> StringCast(const FromCharType* String) 
    {
        return StringCastImpl<ToCharType, FromCharType>::Cast(String);
    }

    /* Path Search Algorithms */
    
    std::filesystem::path FindFolderPathDownwards(const std::filesystem::path& Directory, const std::filesystem::path& FolderName);
    std::filesystem::path FindFolderPathUpwards(const std::filesystem::path& SearchDirectory, const std::filesystem::path& FolderName);
}