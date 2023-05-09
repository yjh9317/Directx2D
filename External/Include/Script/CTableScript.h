#pragma once
#include <Engine/CScript.h>

enum class TABLE_TYPE
{
    Idle,
    Stand,
    Break,
};

enum class TABLE_DIR
{
    LEFT,
    RIGHT,
    UP,
    DOWN,
};

class CTableScript :
    public CScript
{
private:
    TABLE_TYPE  m_type;
    TABLE_DIR m_eDir;
    bool    m_bDamagedAble;

    float   m_fTime;
    bool    m_bTime;
    int    m_HP;

public:
    virtual void update() override;

    virtual void OnCollisionEnter(CGameObject* _OtherObject) override;
    
    void IsCollision(CGameObject* _OtherObj);

    CLONE(CTableScript)

public:
    void Check();
    TABLE_TYPE GetType() { return m_type; }
public:
    CTableScript();
    ~CTableScript();
};

