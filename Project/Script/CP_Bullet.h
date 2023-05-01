#pragma once
#include <Engine/CScript.h>

class CP_Bullet :
    public CScript
{
private:
    Vec3    m_Dir;
    float   m_Speed;
    float   m_fTime;

public:
    virtual void update();

    void SetDir(Vec3 _v) { m_Dir = _v; }
    void SetSpeed(float _f) { m_Speed = _f; }
public:
    virtual void OnCollisionEnter(CGameObject* _OtherObject) override;

    CLONE(CP_Bullet)

public:
    CP_Bullet();
    ~CP_Bullet();
};

