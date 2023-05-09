#pragma once
#include <Engine/CScript.h>

class CCameraMoveScript :
    public CScript
{
private:
    CGameObject*    m_Filter;
    CGameObject*    m_pPlayer;
    float           m_fCamSpeed;
    float           m_fEventTime;

    bool            m_PlayerTrace;

    float           m_DistortionTime;
    float           m_DistortionDist;
    bool            m_bDistortion;

    Vec2            vWinMouse;

    bool            m_Event;
    float           m_fTime;
    Vec3            m_BossPos;

public:
    void SetFilter(CGameObject* _pFilterObject){m_Filter = _pFilterObject;}
    void SetbEvent(bool _b) { m_Event = _b; }
    void SetBossPos(Vec3 _v) { m_BossPos = _v; }

public:
    virtual void SaveToScene(FILE* _pFile) override;
    virtual void LoadFromScene(FILE* _pFile) override;

public:
    virtual void start() override;
    virtual void update() override;
    virtual void lateupdate() override;

    CLONE(CCameraMoveScript)
public:
    CCameraMoveScript();
    ~CCameraMoveScript();
};

