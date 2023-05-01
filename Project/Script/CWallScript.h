#pragma once
#include <Engine/CScript.h>

class CWallScript :
    public CScript
{


public:
    virtual void update() override;


    CLONE(CWallScript)
public:
    virtual void OnCollisionEnter(CGameObject* _OtherObject);
    virtual void OnCollision(CGameObject* _OtherObject);

public:
    CWallScript();
    ~CWallScript();
};

