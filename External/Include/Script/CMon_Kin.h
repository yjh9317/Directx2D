#pragma once
#include "CMonsterScript.h"
class CMon_Kin :
    public CMonsterScript
{

public:
    virtual void start() override;

    virtual void Idle() override;
    virtual void Trace() override;
    virtual void Hit() override;
    virtual void Dead() override;

    CLONE(CMon_Kin)

    virtual void OnCollisionEnter(CGameObject* _OtherObject) override;

public:
    CMon_Kin();
    ~CMon_Kin();
};

