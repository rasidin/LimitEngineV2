/*****************************************************************************
 LIMITEngine Source File
 Copyright (C), LIMITGAME, 2020
 -----------------------------------------------------------
 * @file	String.cpp
 * @brief	LIMITEngine String
 * @author	minseob
 *****************************************************************************/

#include "Core/String.h"

#include "Core/Common.h"
#include "Core/MemoryAllocator.h"

namespace LimitEngine {
	String::String(const char *str)
		: mBuffer(NULL)
	{
		if (str)
		{
			size_t strlen = ::strlen(str);
			mBuffer = (char *)malloc(strlen+1);
			memcpy(mBuffer, str, strlen+1);
		}
	}
	String::String(const String &str)
		: mBuffer(NULL)
	{
		size_t length = str.GetLength();
		if (length)
		{
			mBuffer = (char *)malloc(length+1);
			memcpy(mBuffer, str.GetCharPtr(), length+1);
		}
	}
    String::String(const float &f)
		: mBuffer(NULL)
    {
        char buf[256];
        sprintf_s<256>(buf, "%f", f);
        char *buffer = (char *)malloc(strlen(buf) + 1);
        ::memcpy(buffer, buf, strlen(buf)+1);
        mBuffer = buffer;
    }
	String::~String()
	{
		Release();
	}
	void String::Release()
	{
		if (mBuffer) free(mBuffer);
		mBuffer = NULL;
	}
	uint32	String::FindWord(const String &str) const
	{
		size_t length = GetLength();
		size_t targetLength = str.GetLength();
		if(length == 0 || targetLength == 0 || length < targetLength) return -1;

		for(uint32 cur=0;cur<=length-targetLength;cur++) {
			if(mBuffer[cur] == str.GetCharPtr()[0]) {
				if(strncmp(mBuffer + cur, str.GetCharPtr(), targetLength)==0) {
					return cur;
				}
			}
		}
		return -1;
	}
	bool String::IsContain(const String &str) const
	{
		return FindWord(str) < GetLength();
	}
	String String::Replace(const String &str, const String &tar) 
	{
		String output(*this);
		uint32 foundPos = FindWord(str);
		uint32 srcLength = static_cast<uint32>(GetLength());
		uint32 strLength = static_cast<uint32>(str.GetLength());
		uint32 tarLength = static_cast<uint32>(tar.GetLength());
		if(foundPos < GetLength()) {
			output.GetCharPtr()[foundPos] = 0; // Cut tail

			if(tarLength) {
				if(foundPos + strLength >= srcLength) {
					output += tar;
				}
				else {
					output += tar;
					output += GetCharPtr()[foundPos + strLength];
				}
			} else {
				if(foundPos + strLength < srcLength)
					output += &(GetCharPtr()[foundPos + strLength]);
			}
		}
		return output;
	}
}