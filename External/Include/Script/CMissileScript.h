#pragma once
#include <Engine/CScript.h>

class CMissileScript :
    public CScript
{
private:
    float       m_fSpeed;
    float       m_fAccTime;
    Vec3        m_Dir;
    bool        m_Explosion;

    float       m_fExTime;
    bool        m_particle;
public:

    virtual void update();
    virtual void OnCollisionEnter(CGameObject* _pOtherObj) override;

    CLONE(CMissileScript);

public:
    void SetDir(Vec3 _v) { m_Dir = _v; }

public:
    CMissileScript();
    ~CMissileScript();
};

