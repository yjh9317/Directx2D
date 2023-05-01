#pragma once

#include "CMonsterScript.h"

class CMon_Booklets :
    public CMonsterScript
{
private:
    float   m_AtkCycle;

    bool    m_Sound;
    bool    m_DeathSound;

public:
    virtual void Idle() override;
    virtual void Trace() override;
    virtual void Attack() override;
    virtual void Dead() override;

    virtual void OnCollisionEnter(CGameObject* _OtherObject) override;

    void Shoot();

    CLONE(CMon_Booklets)
public:
    CMon_Booklets();
    ~CMon_Booklets();
};

