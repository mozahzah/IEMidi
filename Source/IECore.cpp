// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#include "IECore.h"

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
    if (static_cast<int16_t>(ResultType) < 0)
    {
#if ENABLE_IE_RESULT_LOGGING
        std::fprintf(stderr, "\033[31mIEError: %s in %s\033[0m\n", Message.data(), FuncName.data());
#endif
        assert(false);
        return false;
    }
    else if (static_cast<int16_t>(ResultType) > 1)
    {
#if ENABLE_IE_RESULT_LOGGING
        std::printf("\033[33mIEWarning: %s in %s\033[0m\n", Message.data(), FuncName.data());
#endif
        return false;
    }
    else
    {
#if ENABLE_IE_RESULT_LOGGING
        std::printf("\033[32mIELog: %s in %s\033[0m\n", Message.data(), FuncName.data());
#endif
        return ResultType == Type::Success;
    }
}
