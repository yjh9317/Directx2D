#pragma once
#include <Engine/CScript.h>

class CAKScript :
    public CScript
{
private:
    Vec3    m_Pos;
    float   m_fTime;
    bool    m_Atk;
    bool    m_Dir;

public:
    virtual void update() override;

    CLONE(CAKScript)

public:
    CAKScript();
    ~CAKScript();
};

