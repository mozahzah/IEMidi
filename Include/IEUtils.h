// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#include <string>
#include <codecvt>

namespace IEUtils
{
    template<typename CharType>
    std::string ConvertToChar(const CharType* String)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> Converter;
        return Converter.to_bytes(String);
    }

    template<>
    std::string ConvertToChar<char>(const char* String)
    {
        return std::string(String);
    }
}