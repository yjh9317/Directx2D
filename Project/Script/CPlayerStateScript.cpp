#include "pch.h"
#include "CPlayerStateScript.h"

#include <Engine/CDevice.h>
#include <Engine/CKeyMgr.h>
#include <Engine/CRenderMgr.h>
#include <Engine/CCollider2D.h>
#include <Engine/CAnimator2D.h>
#include <Engine/CAnimation2D.h>

#include <Engine/CEventMgr.h>

#include "CMonsterScript.h"
#include "CHeartScript.h"
#include "CBlankBulletScript.h"
//#define DEGREE_TO_RADIAN(degree) ((PI/180)*(degree)) // 60분법 -> 호도법
//#define RADIAN_TO_DEGREE(radian) ((180/PI)*(radian)) // 호도법 -> 60분법


CPlayerStateScript::CPlayerStateScript()
	:CScript((int)SCRIPT_TYPE::PLAYERSTATESCRIPT)
	, m_MousePos{}
	, m_fSpeed(240.f)
	, m_fTime(0.f)
	, m_ClickMousePos(0.f, 0.f)
	, m_HandCheck(false)
	, m_fDamagedTime(0.f)
	, m_iDamagedCnt(0)
	, m_bDamagedAble(true)
	, m_vHeart(2)
	, m_vBullet(2)
{
	m_eStateType = PLAYER_TYPE::IDLE;
}

CPlayerStateScript::~CPlayerStateScript()
{
}

void CPlayerStateScript::start()
{
	vector<CGameObject*> pObj = GetOwner()->GetChild();

	for (int i = 0; i < pObj.size(); ++i)
	{
		if (pObj[i]->GetName() == L"Gun")
		{
			m_mapGun.insert(make_pair(pObj[i]->GetName(), pObj[i]));
			m_CurrentGun = pObj[i];
		}
	}

	
	if (CSceneMgr::GetInst()->GetCurScene()->GetSceneState() == SCENE_STATE::PLAY)
	{
		tEventInfo evninfo;

		for (int i = 0; i < m_vHeart.size(); ++i)
		{
			Ptr<CPrefab> pPrefab = CResMgr::GetInst()->FindRes<CPrefab>(L"prefab\\Heart.pref");
			CGameObject* pObj = pPrefab->Instantiate();
			CHeartScript* pScript = new CHeartScript;
			pScript->SetNum(i);
			pObj->AddComponent(pScript);

			evninfo.eType = EVENT_TYPE::CREATE_OBJ;
			evninfo.lParam = (DWORD_PTR)pObj;
			evninfo.wParam = 0;

			CEventMgr::GetInst()->AddEvent(evninfo);
			m_vHeart[i] = pObj;
		}

		for (int i = 0; i < m_vBullet.size(); ++i)
		{
			Ptr<CPrefab> pPrefab = CResMgr::GetInst()->FindRes<CPrefab>(L"prefab\\BlankBullet.pref");
			CGameObject* pObj = pPrefab->Instantiate();

			CBlankBulletScript* pScript = new CBlankBulletScript;
			pScript->SetNum(i);
			pObj->AddComponent(pScript);

			evninfo.eType = EVENT_TYPE::CREATE_OBJ;
			evninfo.lParam = (DWORD_PTR)pObj;
			evninfo.wParam = 0;

			CEventMgr::GetInst()->AddEvent(evninfo);
			m_vBullet[i] = pObj;
		}
	}

}

void CPlayerStateScript::update()
{
	vector<CGameObject*> pObj = GetOwner()->GetChild();

		if (KEY_TAP(KEY::_1))
		{
			CGameObject* pGun = FindWeaponWithName(L"Gun");

			if (pGun && pGun != m_CurrentGun)
			{
				m_CurrentGun->Deactivate();
				m_CurrentGun = pGun;
				m_CurrentGun->Activate();
			}
		}
		else if (KEY_TAP(KEY::_2))
		{
			CGameObject* pGun = FindWeaponWithName(L"IceGun");

			if (pGun && pGun != m_CurrentGun)
			{
				m_CurrentGun->Deactivate();
				m_CurrentGun = pGun;
				m_CurrentGun->Activate();
			}
		}

	if (KEY_TAP(KEY::_3))
	{
		m_ClickMousePos.x = 0.f;
		m_ClickMousePos.y = 0.f;
	}

	if (!m_bDamagedAble)
	{
		m_fDamagedTime += DT;

		if (m_iDamagedCnt < 2)
		{

			if (m_fDamagedTime > 0.3f && m_fDamagedTime < 0.6f)
			{
				m_iDamaged = 0;
			}
			else if (m_fDamagedTime >= 0.f && m_fDamagedTime <= 0.3f)
			{
				m_iDamaged = 1;
			}
			else if (m_fDamagedTime > 0.6f)
			{
				m_fDamagedTime = 0.f;
				++m_iDamagedCnt;
			}
		}
		else
		{
			m_iDamaged = 0;
			m_iDamagedCnt = 0;
			m_bDamagedAble = true;
		}

		if (nullptr != GetOwner()->MeshRender())
			GetOwner()->MeshRender()->GetMaterial()->SetScalarParam(SCALAR_PARAM::INT_0, &m_iDamaged);
	}


	Vec2 Resolution = CDevice::GetInst()->GetRenderResolution();
	Vec2 MousePos = CKeyMgr::GetInst()->GetMousePos();
	m_MousePos = Vec2(MousePos.x - Resolution.x / 2.f, -MousePos.y + Resolution.y / 2.f);

	if (m_eStateType == PLAYER_TYPE::IDLE || m_eStateType == PLAYER_TYPE::WALK)
		m_HandCheck = true;
	else
		m_HandCheck = false;

	switch (m_eStateType)
	{
	case PLAYER_TYPE::IDLE:
	{
		Idle();
		break;
	}
	case PLAYER_TYPE::WALK:
	{
		Walk();
		break;
	}
	case PLAYER_TYPE::ROLL:
	{
		Roll();
		break;
	}
	case PLAYER_TYPE::FALL:
	{
		Fall();
		break;
	}
	case PLAYER_TYPE::HIT:
		break;
	case PLAYER_TYPE::DEAD:
	{
		Dead();
		break;
	}
	}

}

void CPlayerStateScript::lateupdate()
{
	if (KEY_TAP(KEY::R))
	{
		ChangeState(PLAYER_TYPE::FALL);
	}
	else if (KEY_TAP(KEY::F))
		ChangeState(PLAYER_TYPE::DEAD);
	else if (KEY_PRESSED(KEY::W) || KEY_PRESSED(KEY::S) || KEY_PRESSED(KEY::A) || KEY_PRESSED(KEY::D))
	{
		if (KEY_TAP(KEY::RBTN))
		{
			if (GetClickMousePos().x == 0.f && GetClickMousePos().y == 0.f)
			{
				Vec2 Resolution = CDevice::GetInst()->GetRenderResolution();
				Vec2 MousePos = CKeyMgr::GetInst()->GetMousePos();
				SetClickMousePos(Vec2(MousePos.x - Resolution.x / 2.f, -MousePos.y + Resolution.y / 2.f));
				ChangeState(PLAYER_TYPE::ROLL);
				Ptr<CSound> pSound = CResMgr::GetInst()->Load<CSound>(L"sound\\player\\dodge_roll.wav", L"sound\\player\\dodge_roll.wav");
				pSound->Play(1, 0.1f, true);
			}
		}
		else
			ChangeState(PLAYER_TYPE::WALK);
	}
	//else
	//	ChangeState(PLAYER_TYPE::IDLE);
}


void CPlayerStateScript::SetState(PLAYER_TYPE _etype)
{
	if (_etype == m_eStateType)
		return;

	m_eStateType = _etype;
}

void CPlayerStateScript::ChangeState(PLAYER_TYPE _etype)
{
	if (_etype == m_eStateType)
		return;

	switch (m_eStateType)
	{
	case PLAYER_TYPE::IDLE:
		break;
	case PLAYER_TYPE::WALK:
		break;
	case PLAYER_TYPE::ROLL:
	{
		if (m_fTime >= 0.81f)
		{
			m_ClickMousePos.x = 0.f;
			m_ClickMousePos.y = 0.f;
			break;
		}
		else return;
	}
	case PLAYER_TYPE::FALL:
	{
		if (m_fTime >= 2.f)
		{
			break;
		}
		else return;
	}
	case PLAYER_TYPE::HIT:
	{
		break;
	}
	case PLAYER_TYPE::DEAD:
	{
		if (m_fTime >= 2.f)
		{
			break;
		}
		else return;
	}
	}
	m_fTime = 0.f;


	tEventInfo info;
	info.eType = EVENT_TYPE::CHANGE_PLAYER_STATE;
	info.lParam = (DWORD_PTR)this;
	info.wParam = (DWORD_PTR)_etype;
	CEventMgr::GetInst()->AddEvent(info);
}

void CPlayerStateScript::Idle()
{
	Vec3 MainCamPos = CRenderMgr::GetInst()->GetMainCamPos();
	Vec3 ObjPos = Transform()->GetWorldPos();
	Vec2 Dir = Vec2(m_MousePos.x, -m_MousePos.y);
	Dir.Normalize();
	const double Rad_to_deg = 180/XM_PI;
	double d = atan2(Dir.y, Dir.x);
	d *= Rad_to_deg;

	if (d < 60 && d >= -30)
	{
		Animator2D()->Play(L"Right_Idle", true);
	}
	else if (d >= 60 && d < 120)
	{
		if (Dir.x >= 0.f)
			Animator2D()->Play(L"Front_R_Idle", true);
		else
			Animator2D()->Play(L"Front_L_Idle", true);
	}
	else if ((d >= 120 && d < 180) || (d >= -180 && d < -150))
	{
		Animator2D()->Play(L"Left_Idle", true);
	}
	else if (d >= -150 && d < -120)
	{
		Animator2D()->Play(L"Left_B_Idle", true);
	}
	else if (d >= -120 && d < -60)
	{
		if (Dir.x >= 0.f)
			Animator2D()->Play(L"Back_L_Idle", true);
		else
			Animator2D()->Play(L"Back_R_Idle", true);
	}
	else if (d >= -60 && d < -30)
	{
		Animator2D()->Play(L"Right_B_Idle", true);
	}

}

void CPlayerStateScript::Walk()
{
	Vec3 MainCamPos = CRenderMgr::GetInst()->GetMainCamPos();
	Vec3 ObjPos = Transform()->GetWorldPos();
	Vec2 Dir = Vec2(m_MousePos.x, -m_MousePos.y);
	Dir.Normalize();
	const double Rad_to_deg = 180 / XM_PI;
	double d = atan2(Dir.y, Dir.x);
	d *= Rad_to_deg;


	if (d < 60 && d >= -30)
	{
		Animator2D()->Play(L"Right_Walk", true);
	}
	else if (d >= 60 && d < 120)
	{
		if (Dir.x >= 0.f)
			Animator2D()->Play(L"Front_R_Walk", true);
		else
			Animator2D()->Play(L"Front_L_Walk", true);
	}
	else if ((d >= 120 && d < 180) || (d >= -180 && d < -150))
	{
		Animator2D()->Play(L"Left_Walk", true);
	}
	else if (d >= -150 && d < -120)
	{
		Animator2D()->Play(L"Left_B_Walk", true);
	}
	else if (d >= -120 && d < -60)
	{
		if (Dir.x >= 0.f)
			Animator2D()->Play(L"Back_L_Walk", true);
		else
			Animator2D()->Play(L"Back_R_Walk", true);
	}
	else if (d >= -60 && d < -30)
	{
		Animator2D()->Play(L"Right_B_Walk", true);
	}

	Vec3 vPos = Transform()->GetRelativePos();
	Vec3 vWorldPos = Transform()->GetWorldPos();

	if (KEY_PRESSED(KEY::A))
		vPos.x -= DT * m_fSpeed;

	if (KEY_PRESSED(KEY::D))
		vPos.x += DT * m_fSpeed;

	if (KEY_PRESSED(KEY::W))
		vPos.y += DT * m_fSpeed;

	if (KEY_PRESSED(KEY::S))
		vPos.y -= DT * m_fSpeed;

	if (!(KEY_PRESSED(KEY::W) || KEY_PRESSED(KEY::A) || KEY_PRESSED(KEY::D) || KEY_PRESSED(KEY::S)))
	{
		ChangeState(PLAYER_TYPE::IDLE);
	}

	Transform()->SetRelativePos(vPos);
}

void CPlayerStateScript::Roll()
{
	m_fTime += DT;

	if (m_fTime > 0.81f)
	{
		Animator2D()->GetCurAnim()->Reset();
		m_ClickMousePos = Vec2(0.f, 0.f);
		ChangeState(PLAYER_TYPE::IDLE);
	}

	Vec3 MainCamPos = CRenderMgr::GetInst()->GetMainCamPos();
	Vec3 ObjPos = Transform()->GetWorldPos();
	Vec2 Dir = Vec2(m_ClickMousePos.x, m_ClickMousePos.y);
	Dir.Normalize();
	const double Rad_to_deg = 57.2957951;
	double d = atan2(Dir.y, Dir.x);
	d *= Rad_to_deg;

	if (d < 60 && d >= 30)
	{
		Animator2D()->Play(L"Back_R_Roll", false);
	}
	else if (d >= 60 && d < 120)
	{
		Animator2D()->Play(L"Back_Roll", false);
	}
	else if ((d >= 120 && d < 150))// ||(d >= -180 && d <-150)
	{
		Animator2D()->Play(L"Back_L_Roll", false);
	}
	else if ((d >= -180 && d < -120) || (d >= 150 && d <= 180))
	{
		Animator2D()->Play(L"Left_Roll", false);
	}
	else if (d >= -120 && d < -60)
	{
		Animator2D()->Play(L"Front_Roll", false);
	}
	else if ((d >= -60 && d < 0) || (d >= 0 && d < 30))
	{
		Animator2D()->Play(L"Right_Roll", false);
	}


	Vec3 vPos = Transform()->GetRelativePos();

	Vec3 vWorldPos = Transform()->GetWorldPos();

	vPos.x += Dir.x * DT * (m_fSpeed + 100.f);
	vPos.y += Dir.y * DT * (m_fSpeed + 100.f);


	Transform()->SetRelativePos(vPos);
}

void CPlayerStateScript::Dead()
{
	m_fTime += DT;
	Animator2D()->Play(L"Dead", false);

	if (m_fTime > 2.f)
	{
		Animator2D()->GetCurAnim()->Reset();
		ChangeState(PLAYER_TYPE::IDLE);
	}
}

void CPlayerStateScript::Fall()
{
	m_fTime += DT;
	Animator2D()->Play(L"Fall", false);
	if (m_fTime > 2.f)
	{
		Animator2D()->GetCurAnim()->Reset();
		ChangeState(PLAYER_TYPE::IDLE);
	}
}

CGameObject* CPlayerStateScript::FindWeaponWithName(wstring GunName)
{
	map<wstring, CGameObject*>::iterator iter = m_mapGun.find(GunName);

	if(iter == m_mapGun.end())
		return nullptr;

	return  iter->second;
}

void CPlayerStateScript::RegistGun(CGameObject* pGun)
{
	if (nullptr != pGun)
	{
		wstring GunName = pGun->GetName();
		m_mapGun.insert(make_pair(GunName, pGun));

		m_CurrentGun->Deactivate();
		m_CurrentGun = pGun;
		m_CurrentGun->Activate();
	}
}


void CPlayerStateScript::OnCollisionEnter(CGameObject* _OtherObject)
{
	if (_OtherObject->GetName() == L"M_Bullet" || _OtherObject->GetName() == L"Skull_Bullet" || _OtherObject->GetName() == L"Maiden_Bullet" || _OtherObject->GetScript<CMonsterScript>())
	{
		if ((m_eStateType == PLAYER_TYPE::IDLE || m_eStateType == PLAYER_TYPE::WALK) && m_bDamagedAble)
		{
			m_bDamagedAble = false;

			size_t _size = m_vHeart.size();
			if (_size == 0)
				return;
			CHeartScript* pScript = m_vHeart[_size - 1]->GetScript<CHeartScript>();
			pScript->Damaged();

			if (pScript->GetHP() <= 0)
			{
				CGameObject* pObj = m_vHeart[_size - 1];
				m_vHeart.pop_back();
				pObj->Destroy();
			}
		}
	}

}
