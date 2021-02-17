/*********************************************************************
Copyright (c) 2020 LIMITGAME

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
----------------------------------------------------------------------
@file  Memory.h
@brief Definitions of memory
@author minseob (https://github.com/rasidin)
**********************************************************************/
#ifndef LIMITENGINEV2_CORE_MEMORY_H_
#define LIMITENGINEV2_CORE_MEMORY_H_

namespace LimitEngine
{
enum class LimitEngineMemoryCategory : int {
	Unknown = 0,

	Common,
	Debug,
	Form,
	Graphics,

	Count,
};
static const char* LimitEngineMemoryCategoryName[] = {
    "Unknown",

    "Common",
    "Debug",
    "Form",
    "Graphics",
};
class Memory
{
protected:
	static void* Malloc(size_t size);
	static void* MallocAlign(size_t size, size_t align);
	static void Free(void *data);
}; // Memory
} // namespace LimitEngine

#endif // LIMITENGINEV2_CORE_MEMORY_H_