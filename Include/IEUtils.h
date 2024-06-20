// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#include <cwchar>
#include <iostream>
#include <string>
#include <vector>

namespace IEUtils
{
    template<typename FromCharType, typename ToCharType>
    std::string StringCast(const FromCharType* String)
    {
        std::cerr << "StringCast specialization not implemented!" << std::endl;
        return std::string();
    }

    template<typename FromCharType, typename ToCharType>
    std::string StringCast(const ToCharType* String)
    {
        return std::string(String);
    }

    template<>
    std::string StringCast<wchar_t, char>(const wchar_t* String)
    {
        std::mbstate_t State = std::mbstate_t();
        size_t Size = std::wcsrtombs(nullptr, &String, 0, &State) + 1;
        std::vector<char> Buffer(Size);
        std::wcsrtombs(Buffer.data(), &String, Size, &State);
        return std::string(Buffer.data());
    }
}