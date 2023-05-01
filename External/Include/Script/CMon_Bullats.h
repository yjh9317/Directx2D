#pragma once
#include "CMonsterScript.h"
class CMon_Bullats :
    public CMonsterScript
{
private:
    bool    m_Sound;
    bool    m_DeathSound;
    Vec2    m_Dir;
public:
    virtual void Idle() override;
    virtual void Trace() override;
    virtual void Attack() override;
    virtual void Dead() override;

    virtual void OnCollisionEnter(CGameObject* _OtherObject) override;

    CLONE(CMon_Bullats)
public:
    CMon_Bullats();
    ~CMon_Bullats();
};

