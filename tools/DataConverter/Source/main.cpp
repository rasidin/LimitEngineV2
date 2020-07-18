/***************************************
* LimitEngineTextureTool Source Code
* @file main.cpp
* @brief Main
****************************************/
#include <string.h>
#include <malloc.h>

#include <vector>

#include <Managers/ResourceManager.h>
#include <Managers/TaskManager.h>

#include "TextureConverter.h"
#include "ModelConverter.h"
#include "FontConverter.h"

#include "Definitions.h"
#include "Logger.h"

Logger sLog;

enum class ReturnValue {
    OK = 0,
    UnknownCommand,
    FailedToConvert,
    UnknownError,
};

void PrintUsage()
{
    LOG_USAGE<<"LimitEngineTextureTool";
    LOG_USAGE<<"by minseob";
    LOG_USAGE<<Logger::Tag::NextLine;
    LOG_USAGE<<"Usage : DataConverter [command] -i=[InputFile] -o=[OutputFile]";
    LOG_USAGE<<"Commands :";
    LOG_USAGE<<" let - convert to LimitEngineTexture";
    LOG_USAGE<<" mdl - generate model";
    LOG_USAGE<<" font - convert font";
    LOG_USAGE<<" irr - generate irradiance map (Spherical)";
    LOG_USAGE<<" ref - generate reflection map (Spherical)";
    LOG_USAGE<<"Options :";
    LOG_USAGE<<" -i=Input file (or Image file)";
    LOG_USAGE<<" -o=Output file";
    LOG_USAGE<<" -t=Text file";
}

class Arguments
{
    struct Argument
    {
        char *CopiedArg = nullptr;
        char *Name = nullptr;
        char *Value = nullptr;
        Argument() {}
        Argument(const Argument &In) {
            CopyFrom(In);
        }
        Argument(const char *argword) {
            CopiedArg = _strdup(argword);
            size_t arglen = strlen(argword);
            size_t splitter_pos = 0u;
            for (splitter_pos = 0; splitter_pos < arglen; splitter_pos++) {
                if (argword[splitter_pos] == '=') {
                    break;
                }
            }
            if (splitter_pos < arglen) {
                CopiedArg[splitter_pos] = 0;
                Name = CopiedArg;
                Value = &CopiedArg[splitter_pos + 1];
            }
            else {
                Name = CopiedArg;
            }
        }
        ~Argument() {
            if (CopiedArg) free(CopiedArg);
        }
        void operator = (const Argument &In) {
            CopyFrom(In);
        }
        void CopyFrom(const Argument &In) {
            size_t copiedArgSize = strlen(In.Name) + ((In.Value) ? strlen(In.Value) : 0) + 2;
            CopiedArg = (char*)malloc(copiedArgSize);
            memset(CopiedArg, 0, copiedArgSize);
            memcpy(CopiedArg, In.Name, strlen(In.Name));
            Name = CopiedArg;
            if (In.Value) {
                uintptr_t valueOffset = reinterpret_cast<uintptr_t>(In.Value) - reinterpret_cast<uintptr_t>(In.CopiedArg);
                memcpy(CopiedArg + valueOffset, In.Value, strlen(In.Value));
                Value = CopiedArg + valueOffset;
            }
        }
    };
    Argument command;
    std::vector<Argument> arguments;

public:
    Arguments(int Count, char **args) 
    {
        for (int idx = 1; idx < Count; idx++) {
            Argument argument(args[idx]);
            if (argument.Value == nullptr)
                command = argument;
            else
                arguments.push_back(argument);
        }
    }
    bool Contains(const char *Name) const {
        for (size_t idx = 0; idx < arguments.size(); idx++) {
            if (strcmp(arguments[idx].Name, Name) == 0) {
                return true;
            }
        }
        return false;
    }
    char* GetCommand() const { return command.Name; }
    char* operator [](const char *Name) {
        for (size_t idx = 0; idx < arguments.size(); idx++) {
            if (strcmp(arguments[idx].Name, Name) == 0) {
                return arguments[idx].Value;
            }
        }
        return nullptr;
    }
    char* operator [](int Index) {
        return arguments[Index].Value;
    }
    void PrintToLog() const {
        for (size_t idx = 0; idx < arguments.size(); idx++) {
            printf("\n{ ");
            if (arguments[idx].Name && arguments[idx].Value) {
                printf("%s = %s ", arguments[idx].Name, arguments[idx].Value);
            }
            else {
                printf("%s ", arguments[idx].Name);
            }
            printf("}\n");
        }
    }
};

int PrintError(ReturnValue Value)
{
    LOG_ERROR << "!!Error!! ";
    switch (Value)
    {
    case ReturnValue::UnknownCommand:
        LOG_ERROR | "Unkown Command";
        break;
    default:
        LOG_ERROR | "Unkown Error";
        Value = ReturnValue::UnknownError;
        break;
    }
    return static_cast<int>(Value);
}

int main(int argc, char **argv)
{
    sLog.SetVerbose(3);

    PrintUsage();

    Arguments arguments(argc, argv);
    arguments.PrintToLog();

    const char *command = arguments.GetCommand();
    if (command == nullptr)
        return static_cast<int>(ReturnValue::UnknownCommand);

    sLog % LogLevelDebug << "Command : " | command;

    LimitEngine::ResourceManager *leResourceManager = new LimitEngine::ResourceManager();
    LimitEngine::TaskManager *leTaskManager = new LimitEngine::TaskManager();

    leTaskManager->Init();

    if (strcmp(command, "let") == 0) {
        TextureConverter converter;
        TextureConverter::ConvertOptions Options;
        if (!converter.Convert(arguments["-i"], arguments["-o"], Options)) {
            return PrintError(ReturnValue::FailedToConvert);
        }
    }
    else if (strcmp(command, "irr") == 0) {
        TextureConverter converter;
        TextureConverter::ConvertOptions Options;
        if (arguments.Contains("-w") && arguments.Contains("-h")) {
            Options.FilteredImageSize.SetWidth(atoi(arguments["-w"]));
            Options.FilteredImageSize.SetHeight(atoi(arguments["-h"]));
        }
        if (arguments.Contains("-s")) {
            Options.SampleCount = atoi(arguments["-s"]);
        }
        Options.GenerateIrradiance = true;
        if (!converter.Convert(arguments["-i"], arguments["-o"], Options)) {
            return PrintError(ReturnValue::FailedToConvert);
        }
    }
    else if (strcmp(command, "ref") == 0) {
        TextureConverter converter;
        TextureConverter::ConvertOptions Options;
        if (arguments.Contains("-w") && arguments.Contains("-h")) {
            Options.FilteredImageSize.SetWidth(atoi(arguments["-w"]));
            Options.FilteredImageSize.SetHeight(atoi(arguments["-h"]));
        }
        if (arguments.Contains("-s")) {
            Options.SampleCount = atoi(arguments["-s"]);
        }
        Options.GenerateReflection = true;
        if (!converter.Convert(arguments["-i"], arguments["-o"], Options)) {
            return PrintError(ReturnValue::FailedToConvert);
        }
    }
    else if (strcmp(command, "evb") == 0) {
        TextureConverter converter;
        TextureConverter::ConvertOptions Options;
        if (arguments.Contains("-w") && arguments.Contains("-h")) {
            Options.FilteredImageSize.SetWidth(atoi(arguments["-w"]));
            Options.FilteredImageSize.SetHeight(atoi(arguments["-h"]));
        }
        if (arguments.Contains("-s")) {
            Options.SampleCount = atoi(arguments["-s"]);
        }
        Options.GenerateEnvironmentBRDF = true;
        if (!converter.Convert(arguments["-i"], arguments["-o"], Options)) {
            return PrintError(ReturnValue::FailedToConvert);
        }
    }
    else if (strcmp(command, "mdl") == 0) {
        ModelConverter converter;
        ModelConverter::ConvertOptions options;
        if (!converter.Convert(arguments["-i"], arguments["-o"], options)) {
            return PrintError(ReturnValue::FailedToConvert);
        }
    }
    else if (strcmp(command, "font") == 0) {
        FontConverter converter;
        if (!converter.Convert(arguments["-i"], arguments["-t"], arguments["-o"])) {
            return PrintError(ReturnValue::FailedToConvert);
        }
    }
    else {
        return PrintError(ReturnValue::UnknownCommand);
    }
    
    leTaskManager->Term();

    delete leResourceManager;
    delete leTaskManager;

    return static_cast<int>(ReturnValue::OK);
}