#pragma once
#include <Engine/CScript.h>

#include <Engine/define.h>

class CMonsterScript :
    public CScript
{
private:
    MON_STATE       m_eState;


protected:
    float           m_HP;
    float           m_fTime;
    float           m_fAtkTime;
    float           m_DetectRange;
    float           m_AttackRange;
    float           m_fDistance;
    float           m_fSpeed;
    Vec2            m_Dir;
    float           m_Angle;
    bool            m_Atkable;
    bool            m_Atk;
    float           m_BehaviorAngle;

    int             m_IceCheck;
    float           m_IceTime;
    bool            m_bPrevSpeed;
    float           m_fPrevSpeed;

public:
    virtual void start() override;
    virtual void update() override;
    virtual void ChangeState(MON_STATE _eState);

    MON_STATE GetState() { return m_eState; }

public:
    virtual void SetState(MON_STATE _eState);
    virtual void AnimStart() {};
    virtual void Idle() {};
    virtual void Trace();
    virtual void Attack();
    virtual void Hit() {};
    virtual void Skill_1() {};
    virtual void Skill_2() {};
    virtual void Skill_3() {};
    virtual void Skill_4() {};
    virtual void Skill_5() {};
    virtual void Dead() {};

    virtual void OnCollisionEnter(CGameObject* _OtherObject) {};
    virtual void OnCollision(CGameObject* _OtherObject) {};
    virtual void OnCollisionExit(CGameObject* _OtherObject) {};

    CLONE(CMonsterScript);

public:
    CMonsterScript();
    CMonsterScript(int _iScript);
    ~CMonsterScript();

    friend class CEventMgr;
};

