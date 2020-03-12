/***********************************************************
 LIMITEngine Header File
 Copyright (C), LIMITGAME, 2012
 -----------------------------------------------------------
 @file  LE_SceneManager.h
 @brief SceneManager Class
 @author minseob (leeminseob@outlook.com)
 -----------------------------------------------------------
 History:
 - 2012/6/17 Created by minseob
 ***********************************************************/

#ifndef _LE_SCENEMANAGER_H_
#define _LE_SCENEMANAGER_H_

#include <LE_Graphics>

#include "LE_Singleton.h"
#include "LE_VectorArray.h"
#include "LE_TaskManager.h"
#include "LE_EventListener.h"

#define s_SceneManager LimitEngine::SceneManager::GetSingleton()

namespace LimitEngine {
class Camera;
class Light;
class Model;
class SceneFactory;
class SceneManager;
typedef Singleton<SceneManager, LimitEngineMemoryCategory_Graphics> SingletonSceneManager;
class SceneManager : public SingletonSceneManager
{
    friend SceneFactory;
public:
    SceneManager();
    ~SceneManager();

	size_t  GetModelCount()					{ return mModels.size();}
	Model*  GetModel(size_t n)				{ return mModels[n];}
    void    AddModel(Model *m)              { mModels.push_back(m); mOnChangeEvent(); }
    void    AddLight(Light *l);
    Camera* GetCamera() const               { return mCamera; }
    void    SetCamera(Camera *c);

	void	AddOnChangeEventListener(const EventFunctionType &func) { mOnChangeEvent += func; }
	void	RemoveOnChangeEventListener(const EventFunctionType &func) { mOnChangeEvent -= func; }

    void Init();
    void Update();
    void Draw();

private:
    void LoadFromText(const char *text);

private:
    Camera                     *mCamera;
    bool                        mOwnCamera;
    VectorArray<Model*>         mModels;

private:
	EventListener				mOnChangeEvent;
};
#define LE_SceneManager LimitEngine::SceneManager::GetSingleton()
}

#endif // _LE_SCENEMANAGER_H_