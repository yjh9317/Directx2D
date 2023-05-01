#pragma once
#include "CMonsterScript.h"

class CBossScript :
    public CMonsterScript
{
private:
    bool    m_Init;
    float   m_fBossBulletTime;
    int     i_BossBulletCnt;

    float   f_Dir;
    bool    b_Dir;

    bool    m_Summon;
    bool    m_Sound;

    int     m_iOrder;
    CGameObject* m_HeadGun;

public:
    virtual void AnimStart() override;
    virtual void Idle() override;
    virtual void Trace() override;
    virtual void Attack() override;
    virtual void Skill_1() override;
    virtual void Skill_2() override;
    virtual void Skill_3() override;
    virtual void Skill_4() override;
    virtual void Skill_5() override;
    virtual void Dead() override;

public:
    virtual void OnCollisionEnter(CGameObject* _OtherObject) override;

    CLONE(CBossScript)


public:
    void Shoot_Atk();
    void Shoot_Skill1();
    void Shoot_Skill2();
    void Shoot_Skill3();
    void Shoot_Skill4();
    void Shoot_Skill5();


public:
    CBossScript();
    ~CBossScript();
};


