/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  LE_String.h
 @brief String Class
 @author minseob (leeminseob@outlook.com)
 -----------------------------------------------------------
 History:
 - 2012/6/11 Created by minseob
 ***********************************************************/

#ifndef _LE_STRING_H_
#define _LE_STRING_H_

#include <stdlib.h>
#include <string.h>
#include "Core/MemoryAllocator.h"
#include "Core/Object.h"

namespace LimitEngine {
class String : public Object<LimitEngineMemoryCategory_Common>
{
public:
    String(const char *str = 0);
    String(const String &str);
    String(const float &f);
    virtual ~String();

    // ============================================
    // Operators (inline)
    // ============================================
    inline         void    operator =  (const String &str)
    {
        Release();
        if (str.IsEmpty())
            return;
        if (mBuffer)
        {
            free(mBuffer);
            mBuffer = 0;
        }
        size_t strlen = str.GetLength();
        mBuffer = (char *)malloc(strlen+1);
        memcpy(mBuffer, str.GetCharPtr(), strlen+1);
    }
    inline       void   operator =  (const char *str)
    {
        Release();
        if (!str || !strlen(str))
            return;
        if (mBuffer)
        {
            free(mBuffer);
            mBuffer = 0;
        }
        size_t strlen = ::strlen(str);
        mBuffer = (char *)malloc(strlen+1);
        memcpy(mBuffer, str, strlen+1);
    }
    inline       bool   operator == (const String &str)
    {
        if (str.IsEmpty())
        {
            if (IsEmpty()) return true;
            else return false;
        }
        else if (!IsEmpty())
        {
            return (::strcmp(mBuffer, str.mBuffer) == 0);
        }
        return false;
    }
    inline       bool   operator == (const String &str) const
    {
        if (str.IsEmpty())
        {
            if (IsEmpty()) return true;
            else return false;
        }
        else if (!IsEmpty())
        {
            return (::strcmp(mBuffer, str.mBuffer) == 0);
        }
        return false;
    }
    inline       bool   operator == (const char *str)
    {
        if (!strlen(str))
        {
            if (IsEmpty()) return true;
            else return false;
        }
        else if (!IsEmpty())
        {
            return (::strcmp(mBuffer, str) == 0);
        }
        return false;
    }
    inline       bool   operator == (const char *str) const
    {
        if (!strlen(str))
        {
            if (IsEmpty()) return true;
            else return false;
        }
        else if (!IsEmpty())
        {
            return (::strcmp(mBuffer, str) == 0);
        }
        return false;
    }
    inline         char    operator [] (int n)
    { 
        if (mBuffer) return mBuffer[n]; 
        else return 0;
    }
    inline         void    operator += (const String &str)
    {
        size_t thisLength = GetLength();
        size_t tarLength = str.GetLength();
        char *dstBuf = (char *)malloc(thisLength + tarLength + 1);
        memcpy(dstBuf, mBuffer, thisLength);
        memcpy(&dstBuf[thisLength], str.GetCharPtr(), tarLength+1);
        free(mBuffer);
        mBuffer = dstBuf;
    }
    inline       void    operator += (const char* str)
    {
        size_t thisLength = GetLength();
        size_t tarLength = strlen(str);
        char *dstBuf = (char *)malloc(thisLength + tarLength + 1);
        memcpy( dstBuf, mBuffer, thisLength);
        memcpy(&dstBuf[thisLength], str, tarLength+1);
        free(mBuffer);
        mBuffer = dstBuf;
    }
    inline       void   operator += (char c)
    {
        size_t thisLength = GetLength();
        char *dstBuf = (char *)malloc(thisLength + 2);
        ::memset(dstBuf, 0, thisLength + 2);
        memcpy(dstBuf, mBuffer, thisLength);
        dstBuf[thisLength] = c;
        free(mBuffer);
        mBuffer = dstBuf;        
    }
    inline const String    operator + (const String &s1) const
    {
        String output(*this);
        output += s1;
        return output;
    }
    inline operator const char*() const {
        return mBuffer;
    }
    inline operator char*() {
        return mBuffer;
    }

    // ============================================
    // Get & Set
    // ============================================
    bool        IsEmpty() const                        { return (!mBuffer || strlen(mBuffer) == 0); }
    size_t        GetLength() const                    { if (mBuffer) return strlen(mBuffer); else return 0; }
    char*        GetCharPtr()                        { return mBuffer; }
    const char*    GetCharPtr() const                    { return mBuffer; }
    char*       GetCopiedCharPtr()
    {
        if (!GetLength()) return NULL;
        char *output = (char *)malloc(GetLength() + 1);
        ::memcpy(output, mBuffer, GetLength() + 1);
        return output;
    }

    // ============================================
    // Convert
    // ============================================
    int         ToInt() const                       { if (GetLength()) return atoi(mBuffer); else return 0; }
    float       ToFloat() const                     { if (GetLength()) return float(atof(mBuffer)); else return 0.0f; }
    
    // ============================================
    // Public
    // ============================================
    void    Release();
    bool    IsContain(const String &str) const;
    uint32    FindWord(const String &str) const;
    String    Replace(const String &str, const String &tar);

private:
    char        *mBuffer;

    friend class Archive;
};
}

#endif // _LE_STRING_H_