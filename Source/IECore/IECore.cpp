// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#include "IECore/IECore.h"

/* Logging and Assertions */

namespace Private
{
    static constexpr const char* ColorSpecifierReset = "\033[0m";
    static constexpr const char* ColorSpecifierRed = "\033[31m";
    static constexpr const char* ColorSpecifierGreen = "\033[32m";
    static constexpr const char* ColorSpecifierYellow = "\033[33m";
    void IELog(int LogLevel, const char* FuncName, const char* Format, ...)
    {
        const char* ColorCode = nullptr;
        const char* LevelString = nullptr;
        switch (LogLevel)
        {
            case -1:
            {
                ColorCode = ColorSpecifierRed;
                LevelString = "Error";
                break;
            } 
            case 0:
            {
               ColorCode = ColorSpecifierReset;
                LevelString = "Log";
                break; 
            }
            case 1:
            {
              ColorCode = ColorSpecifierGreen;
                LevelString = "Success";
                break;  
            }
            case 2:
            {
                ColorCode = ColorSpecifierYellow;
                LevelString = "Warning";
                break;
            }
            default:
            {
                break;
            }
        };
        std::printf("%sIELog %s: ", ColorCode, LevelString);
        va_list Args;
        va_start(Args, Format);
        std::vprintf(Format, Args);
        va_end(Args);
        std::printf(" [%s]%s\n", FuncName, ColorSpecifierReset);
    }
}

IEResult& IEResult::operator=(const IEResult& OtherResult)
{
    if (this != &OtherResult)
    {
        Type = OtherResult.Type;
        Message = OtherResult.Message;
    }
    return *this;
}

bool IEResult::operator==(const IEResult& OtherResult) const
{
    return this->Type == OtherResult.Type;
}

bool IEResult::operator!=(const IEResult& OtherResult) const
{
    return !(*this == OtherResult);
}

IEResult::operator bool() const
{
    if (static_cast<int16_t>(Type) <= 0)
    {
#if ENABLE_IE_RESULT_LOGGING
        Private::IELog(-1, CallerContextFuncName, Message.c_str());
#endif
        abort();
    }
    else if (static_cast<int16_t>(Type) > 1)
    {
#if ENABLE_IE_RESULT_LOGGING
        Private::IELog(2, CallerContextFuncName, Message.c_str());
#endif
        return false;
    }
    else // this->Type == IEResult::Type::Success
    {
#if ENABLE_IE_RESULT_LOGGING
        Private::IELog(1, CallerContextFuncName, Message.c_str());
#endif
        return true;
    }
}
