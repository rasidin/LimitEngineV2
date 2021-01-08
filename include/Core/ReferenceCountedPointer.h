/*********************************************************************
Copyright(c) 2020 LIMITGAME

Permission is hereby granted, free of charge, to any person
obtaining a copy of this softwareand associated documentation
files(the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify,
merge, publish, distribute, sublicense, and /or sell copies of
the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright noticeand this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
----------------------------------------------------------------------
@file ReferenceCountedPointer.h
@brief Reference counted pointer
@author minseob (leeminseob@outlook.com)
**********************************************************************/
#ifndef LIMITENGINEV2_CORE_REFERENCECOUNTEDPOINTER_H_
#define LIMITENGINEV2_CORE_REFERENCECOUNTEDPOINTER_H_

namespace LimitEngine {
    template<typename T>
    class ReferenceCountedPointer
    {
    public:
        ReferenceCountedPointer() : mData(nullptr) {}
        ReferenceCountedPointer(const ReferenceCountedPointer &In) : mData(In.mData) {
            if (mData) {
                mData->AddReferenceCounter();
            }
        }
        ReferenceCountedPointer(T *Data) : mData(Data) {
            if (mData) {
                mData->AddReferenceCounter();
            }
        }
        ~ReferenceCountedPointer() {
            if (mData && mData->SubReferenceCounter() == 0) {
                delete mData;
            }
        }
        T* Get() const { return mData; }
        void Release() {
            if (mData) {
                if (!mData->SubReferenceCounter()) {
                    delete mData;
                }
            }
            mData = nullptr;
        }
        bool IsValid() const {
            return mData != nullptr;
        }
        ReferenceCountedPointer& operator = (const ReferenceCountedPointer &InPointer) {
            Release();
            mData = InPointer.mData;
            if (mData)
                mData->AddReferenceCounter();
            return *this;
        }
        ReferenceCountedPointer& operator = (T *InPointer) {
            Release();
            mData = InPointer;
            if (mData)
                mData->AddReferenceCounter();
            return *this;
        }
        T* operator ->() const {
            return mData;
        }
        //bool operator () const {
        //    return mData != nullptr;
        //}
    private:
        T *mData;
    };
}

#endif // LIMITENGINEV2_CORE_REFERENCECOUNTEDPOINTER_H_