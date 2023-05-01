#pragma once
#include <Engine/CScript.h>

enum class GUN_TYPE
{
    LIE,
    HOLD,
};

class CIceGunScript :
    public CScript
{
private:
    GUN_TYPE m_type;
    Vec3    m_Pos;
    float   m_fTime;
    bool    m_Atk;
    bool    m_Dir;

    bool    m_ShootCheck;
    float   m_ShootTime;

public:
    virtual void update() override;

    virtual void OnCollisionEnter(CGameObject* _pOtherObj) override;


    CLONE(CIceGunScript)


public:
    CIceGunScript();
    ~CIceGunScript();
};

