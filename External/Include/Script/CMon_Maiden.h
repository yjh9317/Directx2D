#pragma once
#include "CMonsterScript.h"
class CMon_Maiden :
    public CMonsterScript
{
private:
    bool    m_bAtk;
    bool    m_bAtk2;
    float   i_Dir;
    
    bool    m_Sound;

    float   m_WalkTime;
    bool    m_WalkSound;
public:


    virtual void Idle();
    virtual void Trace();
    virtual void Attack();
    virtual void Dead();

    CLONE(CMon_Maiden)

    virtual void OnCollisionEnter(CGameObject* _OtherObject) override;

public:
    void Shoot();
    void Shoot2();
public:
    CMon_Maiden();
    ~CMon_Maiden();
};

