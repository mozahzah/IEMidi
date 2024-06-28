// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#pragma once

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <shlobj.h>
#include <comdef.h> 
#elif defined(__APPLE__)
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#endif

#include <cassert>
#include <charconv>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <filesystem>
#include <format>
#include <locale>
#include <memory>
#include <optional>
#include <source_location>
#include <stack>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <thread>
#include <vector>

/* Time analysers */

using IEClock = std::chrono::high_resolution_clock;
using IEDurationMs = std::chrono::milliseconds;
static constexpr uint32_t TARGET_FRAME_DURATION = 1000 / 60; // 50ms

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
    explicit IEResult(const IEResult::Type& _Type = Type::Unknown, const char* _Message = nullptr, const std::source_location& CallerContext = std::source_location::current())
        : Type(_Type), Message(_Message), CallerContextFuncName(CallerContext.function_name())
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

private:
    const char* CallerContextFuncName = nullptr;
};