// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#pragma once

#include <cassert>
#include <charconv>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <cwchar>
#include <filesystem>
#include <format>
#include <locale>
#include <source_location>
#include <stack>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <vector>

/* Time analysers */

using IEClock = std::chrono::high_resolution_clock;
using IEDuration = std::chrono::duration<double>;

/* Logging and Assertions */

#define ENABLE_IE_ASSERT true
inline bool IEAssert(bool Expression)
{
#if ENABLE_IE_ASSERT
    assert(Expression);
#endif
    return Expression;
};

namespace Private
{
    void IELog(int LogLevel, const char* FuncName, const char* Format, ...);
}
#define IELOG_ERROR(Format, ...)   Private::IELog(-1,  std::source_location::current().function_name(), Format, ##__VA_ARGS__)
#define IELOG_INFO(Format, ...)    Private::IELog( 0,  std::source_location::current().function_name(), Format, ##__VA_ARGS__)
#define IELOG_SUCCESS(Format, ...) Private::IELog( 1,  std::source_location::current().function_name(), Format, ##__VA_ARGS__)
#define IELOG_WARNING(Format, ...) Private::IELog( 2,  std::source_location::current().function_name(), Format, ##__VA_ARGS__)

#define ENABLE_IE_RESULT_LOGGING true
struct IEResult
{
public:  
    enum class Type : int16_t
    {
        NotSupported = -3,
        OutOfMemory = -2,
        Fail = -1,
        Unknown = 0,
        Success = 1,
        InvalidArgument = 2,
        Unimplemented = 3,
    };

public:
    IEResult() = delete;
    IEResult(const IEResult& other) = default;
    IEResult(IEResult&& other) = default;

#if ENABLE_IE_RESULT_LOGGING
    explicit IEResult(const IEResult::Type& _Type = Type::Unknown, const char* _Message = nullptr)
        : Type(_Type), Message(_Message)
    {}
#else
    explicit IEResult(const IEResult::Type& _Type, const char* _Message = nullptr)
        : Type(_Type)
    {}
#endif

    IEResult& operator=(const IEResult& OtherResult);
    bool operator==(const IEResult& OtherResult) const;
    bool operator!=(const IEResult& OtherResult) const;
    operator bool() const;

public:
    Type Type = Type::Unknown;
    std::string Message = {};
};