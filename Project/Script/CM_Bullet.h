#pragma once
#include <Engine/CScript.h>

class CM_Bullet :
    public CScript
{
private:
    Vec3    m_Dir;
    float   m_Speed;
    float   m_fTime;

    bool    m_Wait;
    float   m_WaitTime;


public:
    virtual void update();

    void SetDir(Vec3 _v) { m_Dir = _v; }
    void SetSpeed(float _f) { m_Speed=_f; }

    void WaitCheck(bool _b) { m_Wait = _b; }
    void WaitTime(float _f) { m_WaitTime = _f; }

public:
    virtual void OnCollisionEnter(CGameObject* _OtherObject) override;

    CLONE(CM_Bullet)

public:
    CM_Bullet();
    ~CM_Bullet();
};

