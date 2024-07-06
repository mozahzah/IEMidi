// Copyright © 2024 mozahzah (Incus Entertainment). All rights reserved.

#include "IEActions/IEAction.h"

#include "IEActions/Implementation/IEAction_Impl_Win.h"
#include "IEActions/Implementation/IEAction_Impl_Apple.h"

namespace IEAction
{
    std::unique_ptr<IEAction_Volume> GetVolumeAction()
    {
#if defined (_WIN32)
        return std::make_unique<IEAction_Volume_Impl_Win>();
#elif defined (__APPLE__)
        return std::make_unique<IEAction_Volume_Impl_Apple>();
#endif
    }

    std::unique_ptr<IEAction_Mute> GetMuteAction()
    {
#if defined (_WIN32)
            return std::make_unique<IEAction_Mute_Impl_Win>();
#elif defined (__APPLE__)
            return std::make_unique<IEAction_Mute_Impl_Apple>();
#endif
    }

    std::unique_ptr<IEAction_ConsoleCommand> GetConsoleCommandAction()
    {
        return std::make_unique<IEAction_ConsoleCommand>();
    }

    std::unique_ptr<IEAction_OpenFile> GetOpenFileAction()
    {
        return std::make_unique<IEAction_OpenFile>();
    }
}

void IEAction_ConsoleCommand::ExecuteConsoleCommand(const std::string& ConsoleCommand, float CommandParameterValue)
{
    std::string FinalConsoleCommand = ConsoleCommand;

    const size_t ValuePosition = FinalConsoleCommand.find("{V}");
    if (ValuePosition != std::string::npos)
    {
        const std::string ValueStr = std::to_string(CommandParameterValue);
        FinalConsoleCommand.replace(ValuePosition, 3, ValueStr);
    }

    system(FinalConsoleCommand.c_str());
}

void IEAction_OpenFile::OpenFile(const std::string& FilePath)
{
    if (std::filesystem::exists(std::filesystem::path(FilePath)))
    {
        system(FilePath.c_str());
    }
}