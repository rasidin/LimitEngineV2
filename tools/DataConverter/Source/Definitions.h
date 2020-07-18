/***************************************
* LimitEngineTextureTool Header
* @file Definitions.h
* @brief Definitions
****************************************/
#pragma once

static constexpr int LogLevelUsage = 0;
static constexpr int LogLevelError = 0;
static constexpr int LogLevelLog = 1;
static constexpr int LogLevelSubLog = 2;
static constexpr int LogLevelDebug = 3;

#define LOG_USAGE sLog%LogLevelUsage
#define LOG_ERROR sLog%LogLevelError
#define LOG_MAINLOG sLog%LogLevelLog
#define LOG_MAINDEBUG sLog%LogLevelDebug
#define LOG_SUBLOG sLog%LogLevelSubLog