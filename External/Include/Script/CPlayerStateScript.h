#pragma once
#include <Engine/CScript.h>

#include <Engine/define.h>

class CPlayerStateScript :
    public CScript
{
private:
    map<wstring, CGameObject*> m_mapGun;
    CGameObject* m_CurrentGun;

    vector<CGameObject*> m_vHeart;
    vector<CGameObject*> m_vBullet;

    PLAYER_TYPE m_eStateType;
    float       m_fSpeed;
    float       m_fTime;
    Vec2        m_MousePos;
    Vec2        m_ClickMousePos;
    double      m_Angle;
    bool        m_HandCheck;

    int         m_iDamaged;
    int         m_iDamagedCnt;
    float       m_fDamagedTime;
    bool        m_bDamagedAble;
   
public:
    virtual void start() override;
    virtual void update() override;
    virtual void lateupdate() override;

    virtual void OnCollisionEnter(CGameObject* _OtherObject) override;

    
    virtual void SetState(PLAYER_TYPE _eState) override;
    void ChangeState(PLAYER_TYPE);
    PLAYER_TYPE GetPlayerType() const { return m_eStateType; }

    vector<CGameObject*>& GetHeart() { return m_vHeart; }
    void SetHeart(vector<CGameObject*> _v) { m_vHeart = _v; }

    vector<CGameObject*>& GetBlankBullet() { return m_vBullet; }
    void SetBlankBullet(vector<CGameObject*> _v) { m_vBullet = _v; }

public:
    void Idle();
    void Walk();
    void Roll();
    void Dead();
    void Fall();

    bool GetHandCheck() const { return m_HandCheck; }

    CGameObject* FindWeaponWithName(wstring GunName);    
    void RegistGun(CGameObject* pGun);

public:
    void SetClickMousePos(Vec2 _v) { m_ClickMousePos = _v; }
    Vec2 GetClickMousePos() { return m_ClickMousePos; }
    CLONE(CPlayerStateScript);

public:
    CPlayerStateScript();
    ~CPlayerStateScript();
};

