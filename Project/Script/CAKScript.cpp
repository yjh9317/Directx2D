#include "pch.h"
#include "CAKScript.h"

#include "CMonsterScript.h"

#include <Engine/CSceneMgr.h>
#include <Engine/CScene.h>

#include <Engine/CAnimator2D.h>
#include <Engine/CEventMgr.h>

#include "CM_Bullet.h"

CAKScript::CAKScript()
	: CScript((int)SCRIPT_TYPE::AKSCRIPT)
{
	m_Pos = Vec3(20.f, 0.f, 100.f);
}

CAKScript::~CAKScript()
{
}


void CAKScript::update()
{
	if (m_fTime > 1.5f)
	{
		m_Atk = true;
	}
	else if (!m_Atk)
	{
		m_fTime += DT;
	}


	Transform()->SetIgnoreParentScale(true);

	vector<CScript*> vScripts = GetOwner()->GetParent()->GetScripts();

	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	CGameObject* pPlayer = pCurScene->GetPlayer();

	if (pPlayer == nullptr)
		return;

	Vec3 PlayerPos = pPlayer->Transform()->GetWorldPos();
	Vec3 MonPos = GetOwner()->GetParent()->Transform()->GetWorldPos();
	Vec3 vPos = Transform()->GetWorldPos();

	float Dist = sqrtf(powf(PlayerPos.x - vPos.x, 2) + powf(PlayerPos.y - vPos.y, 2));

	Vec2 Dir = Vec2(PlayerPos.x - vPos.x, PlayerPos.y - vPos.y);
	Dir.Normalize();

	Vec3 Pos = Transform()->GetRelativePos();

	float m_Angle = atan2f(Dir.y, Dir.x);
	const double Rad_to_deg = 57.2957951;
	float Angle = m_Angle * (float)Rad_to_deg;

	Vec3 vRot = Transform()->GetRelativeRotation();

	if (m_Dir)
	{
		if (Angle > 100.f || Angle < -100.f)
			m_Dir = false;

		Animator2D()->Play(L"R_AK", true);
		Transform()->SetRelativePos(Vec3(m_Pos.x, m_Pos.y, m_Pos.z));
		vRot.z = m_Angle;
	}
	else if (!m_Dir)
	{
		if ((Angle < 80.f && Angle >0.f) || (Angle <0.f && Angle>-80.f))
			m_Dir = true;

		Animator2D()->Play(L"L_AK", true);
		Transform()->SetRelativePos(Vec3(-m_Pos.x, m_Pos.y, m_Pos.z));
		vRot.z = m_Angle - XM_PI;
	}


	Transform()->SetRelativeRotation(vRot);

	if (m_Atk && Dist <250.f)
	{
		Ptr<CSound> pSound = CResMgr::GetInst()->Load<CSound>(L"sound\\ak\\Shot.wav", L"sound\\ak\\Shot.wav");
		pSound->Play(1, 0.12f, true);

		m_fTime = 0.f;
		m_Atk = false;
		Ptr<CPrefab> pPrefab = CResMgr::GetInst()->FindRes<CPrefab>(L"prefab\\M_Bullet.pref");
		CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();

		CGameObject* pObj = pPrefab->Instantiate();
		pObj->Transform()->SetRelativePos(GetOwner()->Transform()->GetWorldPos());

		CM_Bullet* pBullet = (CM_Bullet*)CScriptMgr::GetScript((UINT)SCRIPT_TYPE::M_BULLET);

		pBullet->SetSpeed(300.f);
		pBullet->SetDir(Vec3(Dir.x, Dir.y, 0.f));

		pObj->AddComponent(pBullet);

		tEventInfo evninfo;

		evninfo.eType = EVENT_TYPE::CREATE_OBJ;
		evninfo.lParam = (DWORD_PTR)pObj;
		evninfo.wParam = 6;

		CEventMgr::GetInst()->AddEvent(evninfo);
	}
}

