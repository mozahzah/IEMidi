// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#pragma once

#include <cassert>
#include <cstdint>
#include <string>

void IELog(int LogLevel, const char* FuncName, const char* Format, ...);
#define IELOG_ERROR(Format, ...)   IELog(-1, __func__, Format, ##__VA_ARGS__)
#define IELOG_INFO(Format, ...)    IELog(0, __func__, Format, ##__VA_ARGS__)
#define IELOG_SUCCESS(Format, ...) IELog(1, __func__, Format, ##__VA_ARGS__)
#define IELOG_WARNING(Format, ...) IELog(2, __func__, Format, ##__VA_ARGS__)

#define ENABLE_IE_ASSERT true
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
    explicit IEResult(const IEResult::Type& _ResultType = Type::Unknown, const char* _Message = nullptr)
        : ResultType(_ResultType), Message(_Message)
    {}
#else
    explicit IEResult(const IEResult::Type& _ResultType, const char* _Message = nullptr)
        : ResultType(_ResultType)
    {}
#endif

    IEResult& operator=(const IEResult& OtherResult);
    bool operator==(const IEResult& OtherResult) const;
    bool operator!=(const IEResult& OtherResult) const;
    operator bool() const;

public:
    Type ResultType = Type::Unknown;
    std::string Message = {};
};

inline bool IEAssert(bool Expression)
{
#if ENABLE_IE_ASSERT
    assert(Expression);
#endif
    return Expression;
};