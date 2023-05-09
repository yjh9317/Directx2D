#pragma once
#include <Engine/CScript.h>

class CMagnumScript :
    public CScript
{
private:
    Vec3    m_Pos;
    float   m_fTime;
    bool    m_Atk;
    bool    m_Dir;


    bool    m_ShootCheck;
    float   m_ShootTime;

public:
    virtual void start() override;
    virtual void update() override;

    CLONE(CMagnumScript)

public:
    CMagnumScript();
    ~CMagnumScript();
};

