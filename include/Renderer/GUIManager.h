/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2013
 -----------------------------------------------------------
 @file  LE_GUIManager.h
 @brief GUIManager Class
 @author minseob (leeminseob@outlook.com)
 -----------------------------------------------------------
 History:
 - 2013/7/28 Created by minseob
 ***********************************************************/

#ifndef _LE_GUIMANAGER_H_
#define _LE_GUIMANAGER_H_

#include "LE_Singleton.h"

#include "LE_FVector2.h"

namespace LimitEngine {
    class Widget;
	class GUIManager;
	typedef Singleton<GUIManager, LimitEngineMemoryCategory_Graphics> SingletonGUIManager;
    class GUIManager : public SingletonGUIManager
    {
    public:
        GUIManager();
        virtual ~GUIManager();
        
        // Interface
        static void AddWidget(Widget *w)        { if (mInstance) mInstance->addWidget(w); }
        static void Resize(const fVector2 &s)   { if (mInstance) mInstance->resize(s); }
        static void Update()                    { if (mInstance) mInstance->update(); }
        static void Draw()                      { if (mInstance) mInstance->draw(); }
    private:
        void addWidget(Widget *w);
        void resize(const fVector2 &s);
        void update();
        void draw();
    
    private:
        Widget      *_rootWidget;
    };
}

#endif
