#pragma once
#include <Engine/CScript.h>

class CBoss_Bullet :
    public CScript
{
private:
    Vec3    m_Dir;
    float   m_Speed;
    float   m_fTime;
    float   m_Angle;

    bool    m_Wait;
    float   m_WaitTime;

public:
    virtual void update() override;

    void SetDir(Vec3 _v) { m_Dir = _v; }
    void SetSpeed(float _f) { m_Speed = _f; }
    void SetAngle(float _f) { m_Angle = _f; }

    void WaitCheck(bool _b) { m_Wait = _b; }
    void WaitTime(float _f) { m_WaitTime = _f; }

    CLONE(CBoss_Bullet)

public:
    virtual void OnCollisionEnter(CGameObject* _OtherObject) override;

public:
    CBoss_Bullet();
    ~CBoss_Bullet();
};

