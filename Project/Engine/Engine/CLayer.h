#pragma once
#include "CEntity.h"

class CGameObject;

class CLayer :
    public CEntity
{
private:
    vector<CGameObject*>    m_vecRoot;      // 레이어에 속한 최상위 부모 오브젝트
    vector<CGameObject*>    m_vecObj;       // 매 프레임 마다 파악되는 레이어 소속 모든 오브젝트
    int                     m_iLayerIdx;    // Scene 에서의 인덱스


public:
    int GetLayerIdx() { return m_iLayerIdx; }


public:
    void start();
    void update();
    void lateupdate();
    void finalupdate();
   

private:
    void AddObject(CGameObject* _pObj);
    void RegisterObjectAsRoot(CGameObject* _pObj) { m_vecRoot.push_back(_pObj); }
    void RegisterObject(CGameObject* _pObj){m_vecObj.push_back(_pObj);}
    void DeregisterObject(CGameObject* _pObj);
    void Clear(){m_vecObj.clear();}

public:
    vector<CGameObject*>& GetRootObjects() { return m_vecRoot; }
    vector<CGameObject*>& GetObjects() { return m_vecObj; }



    CLONE_DISABLE(CLayer)

public:
    CLayer();
    ~CLayer();

    friend class CSceneMgr;
    friend class CScene;
    friend class CGameObject;
    friend class CEventMgr;
};

