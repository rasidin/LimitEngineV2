/*****************************************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 * @file    LE_Function.h
 * @brief    Function class
 * @author    minseob
 -----------------------------------------------------------
 History:
 - 2016/10/27 Created by minseob
 *****************************************************************************/
#ifndef _LE_FUNCTION_H_
#define _LE_FUNCTION_H_

namespace LimitEngine {
    template <typename ReturnType>
    class Function
    {
    private:
        /// for Single class
        class DummyClass1 { 
        public:
            ~DummyClass1(){}
            ReturnType dummyClass();
        };
        /// for vitual class
        class DummyClass2{
        public:
            virtual ~DummyClass2() = 0;
            virtual ReturnType dummy2() = 0;
        };
        /// for class that have parent
        class DummyClass3 : public DummyClass1, public DummyClass2{
        public:
            virtual ~DummyClass3(){}
        };
        typedef ReturnType(*FunctionType0)();                    /// No class
        typedef ReturnType(DummyClass1::*FunctionType1)();        /// Class type 1 (Single)
        typedef ReturnType(DummyClass2::*FunctionType2)();        /// Class type 2 (Virtual)
        typedef ReturnType(DummyClass3::*FunctionType3)();        /// Class type 3 (Have parent)
    public:
        Function()
            : mClassPtr(nullptr)
        {
            memset(mFunction, 0, sizeof(mFunction));
        }
        template<typename ClassType>
        Function(ClassType *classPtr, ReturnType (ClassType::*func)())
            : mClassPtr((void*)classPtr)
        {
            if(sizeof(FunctionType1) == sizeof(func)) {
                memcpy((void*)&mFunction1, (const void*)&func, sizeof(func));
                mType = 1;
            } else if(sizeof(FunctionType2) == sizeof(func)) {
                memcpy((void*)&mFunction2, (const void*)&func, sizeof(func));
                mType = 2;
            } else if(sizeof(FunctionType3) == sizeof(func)) {
                memcpy((void*)&mFunction3, (const void*)&func, sizeof(func));
                mType = 3;
            } else {
                mType = 0;
            }
        }
        Function(ReturnType (*func)())
            : mClassPtr(nullptr)
            , mFunction0(func)
        {}
        ReturnType operator()() {
            switch(mType) {
            case 1:
                return (mClassPtr1->*mFunction1)();
            case 2:
                return (mClassPtr2->*mFunction2)();
            case 3:
                return (mClassPtr3->*mFunction3)();
            default:
                return (*mFunction0)();
            }
        }
		bool operator==(const Function &func)
		{
			if (mClassPtr == func.mClassPtr &&
				mFunction[0] == func.mFunction[0] &&
				mFunction[1] == func.mFunction[1] &&
				mFunction[2] == func.mFunction[2] &&
				mFunction[3] == func.mFunction[3]
				)
				return true;
			return false;
		}
    private:
        // Pointer of class
        union {
            DummyClass1 *mClassPtr1;
            DummyClass2 *mClassPtr2;
            DummyClass3 *mClassPtr3;
            void *mClassPtr;
        };
        // Pointer of function
        union {
            FunctionType0 mFunction0;
            FunctionType1 mFunction1;
            FunctionType2 mFunction2;
            FunctionType3 mFunction3;
            ReturnType *mFunction[4];
        };
        uint32 mType;
    };
}; // namespace LimitEngine

#endif // _LE_FUNCTION_H_