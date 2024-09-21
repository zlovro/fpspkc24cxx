#pragma once
#include <source_location>
#include <string>

typedef struct ErrorContextStruct
{
    enum Status
    {
        OK,
        WARNING,
        ERROR
    };

private:
    Status               mStatus = OK;
    std::string          mMsg;
    std::source_location mErrLoc;

public:
    void setErr(const std::string& pMsg, const std::source_location& pSrcLoc = std::source_location::current())
    {
        mMsg    = pMsg;
        mStatus = ERROR;

        mErrLoc = pSrcLoc;
    }

    bool isErr() const
    {
        return mStatus == ERROR;
    }

    std::string getMsg()
    {
        return std::format("Error occured in {}:{}:{}, function {}\nMessage:\n{}", mErrLoc.file_name(), mErrLoc.line(),
                           mErrLoc.column(), mErrLoc.function_name(), mMsg);
    }
} ErrorContext;

inline ErrorContext gErrCtx = {};
