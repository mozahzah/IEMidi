// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#pragma once

#include <cassert>
#include <cstdint>
#include <string>

#define ENABLE_IE_RESULT_LOGGING true

struct IEResult
{
public:  
    enum class Type : int16_t
    {
        Success = 1,
        Fail = 2,
        InvalidArgument = 3,
        Unimplemented = 4,

        OutOfMemory = -1,
        NotSupported = -2,
    };

public:
    IEResult() = delete;
    IEResult(const IEResult& other) = default;
    IEResult(IEResult&& other) = default;

#if ENABLE_IE_RESULT_LOGGING
    explicit IEResult(const IEResult::Type& _ResultType, const char* _Message = "No Message Provided", const char* _FuncName = "Unknown")
        : ResultType(_ResultType), Message(_Message), FuncName(_FuncName)
    {}
#else
    explicit IEResult(const IEResult::Type& _ResultType, const char* _Message = nullptr, const char* _FuncName = nullptr)
        : ResultType(_ResultType)
    {}
#endif

    IEResult& operator=(const IEResult& OtherResult);
    bool operator==(const IEResult& OtherResult) const;
    bool operator!=(const IEResult& OtherResult) const;
    operator bool() const;

public:
    Type ResultType = Type::Fail;
#if ENABLE_IE_RESULT_LOGGING
    std::string Message = "No Message Provided";
    const std::string FuncName;
#endif
};

inline bool IEAssert(bool Expression)
{
    assert(Expression);
    return Expression;
};