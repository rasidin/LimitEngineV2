/**********************************************
* LimitEngineTextureTool Header
* @file Logger.h
* @brief Logger for converters
************************************************/
#pragma once

#include <stdio.h>

struct Logger {
    enum class Tag {
        NextLine = 1,
        Tab
    };

    bool IsLogging = false;
    int Verbose = 0;
    int CurrentVerbose = 0;
    int TabSpace = 4;

    Logger& SetVerbose(int InVerbose) {
        Verbose = InVerbose;
        return *this;
    }
    Logger& SetCurrentVerbose(int InVerbose) {
        CurrentVerbose = InVerbose;
        return *this;
    }
    Logger& operator << (const char *Text) {
        if (Verbose < CurrentVerbose) return *this;
        if (IsLogging) { printf("\n"); }
        printf(Text);
        IsLogging = true;
        return *this;
    }
    Logger& operator << (int Number) {
        if (Verbose < CurrentVerbose) return *this;
        if (IsLogging) { printf("\n"); }
        printf("%d", Number);
        IsLogging = true;
        return *this;
    }
    Logger& operator << (float Decimal) {
        if (Verbose < CurrentVerbose) return *this;
        if (IsLogging) { printf("\n"); }
        printf("%f", Decimal);
        IsLogging = true;
        return *this;
    }
    Logger& operator << (Tag InTag) {
        if (Verbose < CurrentVerbose) return *this;
        if (IsLogging) { printf("\n"); }
        switch (InTag) {
        case Tag::NextLine:
            printf("\n");
            break;
        case Tag::Tab:
            for (int i = 0; i < TabSpace; i++) printf(" ");
            break;
        }
        IsLogging = true;
        return *this;
    }
    Logger& operator | (const char *Text) {
        if (Verbose < CurrentVerbose || Text == nullptr) return *this;
        printf(Text);
        IsLogging = true;
        return *this;
    }
    Logger& operator | (int Number) {
        if (Verbose < CurrentVerbose) return *this;
        printf("%d", Number);
        IsLogging = true;
        return *this;
    }
    Logger& operator | (float Decimal) {
        if (Verbose < CurrentVerbose) return *this;
        printf("%f", Decimal);
        IsLogging = true;
        return *this;
    }
    Logger& operator | (Tag InTag) {
        if (Verbose < CurrentVerbose) return *this;
        switch (InTag) {
        case Tag::NextLine:
            printf("\n");
            break;
        case Tag::Tab:
            for (int i = 0; i < TabSpace; i++) printf(" ");
            break;
        }
        IsLogging = true;
        return *this;
    }
    Logger& operator % (int Verbose) {
        if (Verbose < CurrentVerbose) return *this;
        SetCurrentVerbose(Verbose);
        return *this;
    }
};
extern Logger sLog;
