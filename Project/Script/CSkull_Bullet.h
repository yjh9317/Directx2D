#pragma once
#include <Engine/CScript.h>

class CSkull_Bullet :
    public CScript
{
private:
    Vec3    m_Dir;
    float   m_Speed;
    float   m_fTime;
    float   m_RotateSpeed;
    float   f_Dir;

public:
    virtual void update() override;

    void SetDir(Vec3 _v) { m_Dir = _v; }
    void SetSpeed(float _f) { m_Speed = _f; }

public:
    virtual void OnCollisionEnter(CGameObject* _OtherObject) override;

    CLONE(CSkull_Bullet)
public:
    CSkull_Bullet();
    ~CSkull_Bullet();
};

