#pragma once
#include <Engine/CScript.h>

class CMaiden_Bullet :
    public CScript
{
private:
    Vec3    m_Dir;
    float   m_Speed;
    float   m_fTime;

    bool    m_Wait;
    float   m_WaitTime;

public:
    virtual void update() override;
    virtual void OnCollisionEnter(CGameObject* _OtherObject) override;


    void SetDir(Vec3 _v) { m_Dir = _v; }
    void SetSpeed(float _f) { m_Speed = _f; }

    void WaitCheck(bool _b) { m_Wait = _b; }
    void WaitTime(float _f) { m_WaitTime = _f; }

    bool GetWait() { return m_Wait; }
    void SetWait(bool _b) { m_Wait = _b; }
    CLONE(CMaiden_Bullet)
public:
    CMaiden_Bullet();
    ~CMaiden_Bullet();
};

