#pragma once
#include "CMonsterScript.h"

class CMon_Gigi :
    public CMonsterScript
{
private:
    bool    m_Sound;

public:
    virtual void start() override;

    virtual void Idle();
    virtual void Trace();
    virtual void Attack();
    virtual void Hit();
    virtual void Dead();

    void Shoot();

    CLONE(CMon_Gigi)

    virtual void OnCollisionEnter(CGameObject* _OtherObject) override;

public:
    CMon_Gigi();
    ~CMon_Gigi();

};

