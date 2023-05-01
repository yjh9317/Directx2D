#pragma once
#include <Engine/CScript.h>

class CHeadGunScript :
    public CScript
{
private:
    float   m_fTime;
    bool    m_Atk;

public:
    virtual void update() override;

    CLONE(CHeadGunScript)

public:
    void Shot();
public:
    CHeadGunScript();
    ~CHeadGunScript();
};

