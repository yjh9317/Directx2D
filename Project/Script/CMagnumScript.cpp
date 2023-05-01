#include "pch.h"
#include "CMagnumScript.h"

#include <Engine/CEventMgr.h>
#include <Engine/CAnimator2D.h>
#include <Engine/CAnimation2D.h>
#include "CPlayerStateScript.h"
#include "CP_Bullet.h"


CMagnumScript::CMagnumScript()
	: CScript((int)SCRIPT_TYPE::MAGNUMSCRIPT)
	, m_fTime(0.f)
	, m_Atk(true)
	, m_Dir(false)
	, m_ShootCheck(false)
	, m_ShootTime(0.f)
{
	m_Pos = Vec3(21.f, -20.f, 0.f);
}

CMagnumScript::~CMagnumScript()
{
}

void CMagnumScript::start()
{
	Transform()->SetIgnoreParentScale(true);
}

void CMagnumScript::update()
{
	if (m_fTime>0.4f)
	{
		m_Atk = true;
	}
	else if (!m_Atk)
	{
		m_fTime += DT;
	}
	
	Vec2 Resolution = CDevice::GetInst()->GetRenderResolution();
	Vec2 MousePos = CKeyMgr::GetInst()->GetMousePos();

	// 마우스를 화면 중앙으로 맞추기
	Vec2 Win_MousePos = Vec2(MousePos.x - Resolution.x / 2.f, -MousePos.y + Resolution.y / 2.f);

	Vec3 Pos = Transform()->GetRelativePos();
	
	// 플레이어에서 마우스쪽으로의 벡터를 구한 후 각도로 변경
	Vec2 Dir = Vec2(Win_MousePos.x-Pos.x, Win_MousePos.y-Pos.y);
	Dir.Normalize();
	float m_Angle = atan2f(Dir.y, Dir.x);

	// Radian To Degree
	const double Rad_to_deg = 57.2957951;
	float Angle = m_Angle * (float)Rad_to_deg;	 

	Vec3 vRot = Transform()->GetRelativeRotation();

	if (m_ShootCheck)
	{
		m_ShootTime += DT;

		// 애니메이션 리셋
		if (m_ShootTime > 0.4f)
		{
			m_ShootTime = 0.f;
			m_ShootCheck = false;
			Animator2D()->FindAnim(L"R_MagnumShoot")->Reset();
			Animator2D()->FindAnim(L"L_MagnumShoot")->Reset();
		}

		if (m_Dir)
		{
			Animator2D()->Play(L"R_MagnumShoot", false);
			Transform()->SetRelativePos(Vec3(m_Pos.x, m_Pos.y, m_Pos.z));
			vRot.z = m_Angle;
		}
		else if (!m_Dir)
		{
			Animator2D()->Play(L"L_MagnumShoot", false);
			Transform()->SetRelativePos(Vec3(-m_Pos.x, m_Pos.y, m_Pos.z));
			vRot.z = m_Angle - XM_PI;
		}
	}
	else // 총의 위치 좌우 구분
	{
		if (m_Dir)
		{
			Animator2D()->Play(L"R_Magnum", true);
			Transform()->SetRelativePos(Vec3(m_Pos.x, m_Pos.y, m_Pos.z));
			vRot.z = m_Angle;

			if (Angle > 110.f || Angle < -110.f)
				m_Dir = false;
		}
		else if (!m_Dir)
		{
			Animator2D()->Play(L"L_Magnum", true);
			Transform()->SetRelativePos(Vec3(-m_Pos.x, m_Pos.y, m_Pos.z));
			vRot.z = m_Angle - XM_PI;

			if ((Angle < 70.f && Angle >0.f) || (Angle <0.f && Angle>-70.f))
				m_Dir = true;
		}
	}


	if (nullptr == GetOwner()->GetParent())
		return;

	vector<CScript*> vScripts = GetOwner()->GetParent()->GetScripts();

	if (vScripts.size() == 0)
		return;

	CPlayerStateScript* pStateScript = nullptr;

	for (int i = 0; i < vScripts.size(); ++i)
	{
		if (CScriptMgr::GetScriptName(vScripts[i]) == L"CPlayerStateScript")
		{
			pStateScript = (CPlayerStateScript*)vScripts[i];
		}
	}
	if (!pStateScript->GetHandCheck())
	{
		Transform()->Transform()->SetRelativeScale(Vec3(0.f, 0.f, 0.f));
	}
	else
	{
		Transform()->Transform()->SetRelativeScale(Vec3(100.f, 100.f, 0.f));
	}

	Transform()->SetRelativeRotation(vRot);

	if (KEY_TAP(KEY::LBTN) && m_Atk)
	{
		Ptr<CSound> pSound = CResMgr::GetInst()->Load<CSound>(L"sound\\magnum\\Shot.wav", L"sound\\magnum\\Shot.wav");
		pSound->Play(1, 0.12f,true);

		if (!m_ShootCheck)
			m_ShootCheck = true;

		m_fTime = 0.f;
		m_Atk = false;
		Ptr<CPrefab> pPrefab = CResMgr::GetInst()->FindRes<CPrefab>(L"prefab\\P_Bullet.pref");
		CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
		
		CGameObject* pObj = pPrefab->Instantiate();
		
		CP_Bullet* pBullet = (CP_Bullet*)CScriptMgr::GetScript((UINT)SCRIPT_TYPE::P_BULLET);

		pBullet->SetSpeed(500.f);
		pBullet->SetDir(Vec3(Dir.x,Dir.y,0.f));
		pObj->Transform()->SetRelativePos(GetOwner()->Transform()->GetWorldPos() + Dir * Vec2(40.f,40.f));
		pObj->AddComponent(pBullet);

		tEventInfo evninfo;

		evninfo.eType = EVENT_TYPE::CREATE_OBJ;
		evninfo.lParam = (DWORD_PTR)pObj;
		evninfo.wParam = 3;

		CEventMgr::GetInst()->AddEvent(evninfo);
	}

}
