/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 @file  ShaderParameterParser.h
 @brief Get parameter in source code
 @author minseob (leeminseob@outlook.com)
 ***********************************************************/
#include "Core/Debug.h"
#include "Renderer/ShaderParameterParser.h"

namespace LimitEngine {
    bool ShaderParameterParser::Parse(const char *text)
    {
#define TEXTPARSER_MAX_WORD     256
        char buf[TEXTPARSER_MAX_WORD];

        uint32 word_length = 0u;
        const char *ptr = text;
        char currentWord = *(ptr++);
        bool reading = false;
        while(currentWord) {
            if(reading == false) {
                if(currentWord == '/' && *(ptr) == '/') {
                    ptr++;
                    reading = true;
                    word_length = 0u;
                }
            } else {
                if(currentWord == '\n') { // Parse comment
                    buf[word_length] = 0;
                    String bufString(buf);
                    if(bufString.IsContain("LE_SHADER_PARAMETER")) {
                        bufString = bufString.Replace("LE_SHADER_PARAMETER", "");
                        bufString = bufString.Replace("[", "");
                        bufString = bufString.Replace("]", "");

                        char *bufstr = bufString.GetCharPtr();

                        Parameter param;
                        uint32 paramNameLength = 0;
                        char paramName[TEXTPARSER_MAX_WORD];
                        uint32 paramValueLength = 0;
                        char paramValue[TEXTPARSER_MAX_WORD];
                        bool paramNameReading = true;
                        while(*bufstr) {
                            if(paramNameReading) { // Param name
                                if(*bufstr == ' ' || *bufstr =='\t') {
                                    if(paramNameLength) {
                                        paramValueLength = 0;
                                    }
                                } else if(*bufstr == '=') {
                                    paramName[paramNameLength] = 0;
                                    paramNameReading = false;
                                } else
                                    paramName[paramNameLength++] = *bufstr;
                            } else { // Param value
                                if(*bufstr == ' ' || *bufstr =='\t') {
                                    if(paramNameLength && paramValueLength) {
                                        paramValue[paramValueLength] = 0;
                                        param.attributes[paramName] = paramValue;
                                    }
                                    paramNameLength = 0;
                                    paramValueLength = 0;
                                    paramNameReading = true;
                                } else 
                                    paramValue[paramValueLength++] = *bufstr;
                            }
                            bufstr++;
                        }
                        if(paramNameLength && paramValueLength) {
                            paramValue[paramValueLength] = 0;
                            param.attributes[paramName] = paramValue;
                        }

                        if(param.attributes.FindIndex("name") >= 0) {
                            // Valueの設定
                            bool includedSpace = false;
                            String valueWord;
                            while(currentWord = *(ptr++)) {
                                if(currentWord == ' ' || currentWord == '\t') {
                                    if(valueWord.GetLength() && param.type.IsEmpty()) {
                                        param.type = valueWord;
                                        valueWord = "";
                                        includedSpace = false;
                                    } else {
                                        includedSpace = true;
                                    }
                                } else if(currentWord == '=' || currentWord == ';' || currentWord == '{') { // Exit
                                    param.value = valueWord;
                                    break;
                                } else if(currentWord != '\r' && currentWord != '\n') {
                                    if(includedSpace) {
                                        includedSpace = false;
                                        valueWord = "";
                                    }
                                    valueWord += currentWord;
                                }
                            }
                            mParameters[param.attributes["name"]] = param;
                        }
                    }
                    reading = false;
                } else {
                    buf[word_length++] = currentWord;
                    if(word_length == TEXTPARSER_MAX_WORD) {
                        buf[TEXTPARSER_MAX_WORD-1] = 0;
                        Debug::Warning("[Shader] Fail to parse source parameter... Buffer overflow. %s", buf);
                        break;
                    }
                }
            }
            currentWord = *(ptr++);
        }

        return true;
    }
} // namespace LimitEngine