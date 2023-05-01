#pragma once
#include "CMonsterScript.h"
class CMon_Cubulon :
    public CMonsterScript
{
private:
    bool    m_Sound;
public:
    virtual void Idle();
    virtual void Trace();
    virtual void Attack();
    virtual void Hit();
    virtual void Dead();

    virtual void OnCollisionEnter(CGameObject* _OtherObject) override;

public:
    void Shoot();
    CLONE(CMon_Cubulon)

public:
    CMon_Cubulon();
    ~CMon_Cubulon();
};

