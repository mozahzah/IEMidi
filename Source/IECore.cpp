// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#include "IECore.h"

#include <stdio.h>
#include <stdarg.h>

// Define color codes
#define COLOR_RESET   "\033[0m"
#define COLOR_SUCCESS "\033[32m"  // Green
#define COLOR_WARNING "\033[33m"  // Yellow
#define COLOR_ERROR   "\033[31m"  // Red

void IELog(int LogLevel, const char* FuncName, const char* Format, ...)
{
    const char* ColorCode = nullptr;
    const char* LevelString = nullptr;
    switch (LogLevel)
    {
    case -1:
        ColorCode = COLOR_ERROR;
        LevelString = "Error";
        break;
    case 0:
        ColorCode = COLOR_RESET;
        LevelString = "Log";
        break;
    case 1:
        ColorCode = COLOR_SUCCESS;
        LevelString = "Success";
        break;
    case 2:
        ColorCode = COLOR_WARNING;
        LevelString = "Warning";
    default:
        break;
    };
    std::printf("%sIELog %s: ", ColorCode, LevelString);
    va_list Args;
    va_start(Args, Format);
    std::vprintf(Format, Args);
    va_end(Args);
    std::printf(" [%s]%s\n", FuncName, COLOR_RESET);
}

IEResult& IEResult::operator=(const IEResult& OtherResult)
{
    if (this != &OtherResult)
    {
        ResultType = OtherResult.ResultType;
        Message = OtherResult.Message;
    }
    return *this;
}

bool IEResult::operator==(const IEResult& OtherResult) const
{
    return this->ResultType == OtherResult.ResultType;
}

bool IEResult::operator!=(const IEResult& OtherResult) const
{
    return !(*this == OtherResult);
}

IEResult::operator bool() const
{
    if (static_cast<int16_t>(ResultType) <= 0)
    {
#if ENABLE_IE_RESULT_LOGGING
        IELOG_ERROR(Message.c_str());
#endif
        abort();
    }
    else if (static_cast<int16_t>(ResultType) > 1)
    {
#if ENABLE_IE_RESULT_LOGGING
        IELOG_WARNING(Message.c_str());
#endif
        return false;
    }
    else // this->Type == IEResul::Type::Success
    {
#if ENABLE_IE_RESULT_LOGGING
        IELOG_SUCCESS(Message.c_str());
#endif
        return true;
    }
}
