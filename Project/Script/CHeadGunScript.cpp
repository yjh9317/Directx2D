#include "pch.h"
#include "CHeadGunScript.h"

#include <Engine/CAnimator2D.h>

#include <Engine/CSceneMgr.h>
#include <Engine/CScene.h>
#include <Engine/CEventMgr.h>
#include "CMissileScript.h"


CHeadGunScript::CHeadGunScript()
	:CScript((int)SCRIPT_TYPE::HEADGUNSCRIPT)
	, m_fTime(0.f)

{
}

CHeadGunScript::~CHeadGunScript()
{
}

void CHeadGunScript::update()
{
	if (m_fTime > 1.6f && !m_Atk)
	{
		m_Atk = true;
		Shot();
	}

	m_fTime += DT;
	Vec3 vPos = Transform()->GetWorldPos();

	if (m_fTime >= 0.f && m_fTime <= 0.6f)
	{
		Animator2D()->Play(L"HeadGun_Start",false);
	}
	else if(m_fTime>0.6f && m_fTime<2.5f)
	{
		CScene* pScene = CSceneMgr::GetInst()->GetCurScene();
		Vec3 vPlayerPos = pScene->GetPlayerPos();


		Vec2 Dir = Vec2(vPlayerPos.x - vPos.x, vPlayerPos.y - vPos.y);
		Dir.Normalize();
		float m_Angle = atan2f(Dir.y, Dir.x);

		const double Rad_to_deg = 57.2957951;
		float Angle = m_Angle * (float)Rad_to_deg;

		if (Angle >= -22.5f && Angle <= 22.5f)
		{
			Animator2D()->Play(L"Right", false);
		}
		else if (Angle > 22.5f && Angle <= 67.5f)
		{
			Animator2D()->Play(L"Up_Right", false);
		}
		else if (Angle > 67.5f && Angle <= 112.5f)
		{
			Animator2D()->Play(L"Up", false);
		}
		else if (Angle > 112.5f && Angle <= 157.5f)
		{
			Animator2D()->Play(L"Up_Left", false);
		}
		else if ((Angle >= 157.5f && Angle <= 181.f) || (Angle >= -181.f && Angle < -157.5f))
		{
			Animator2D()->Play(L"Left", false);
		}
		else if (Angle > -157.5f && Angle <= -112.5f)
		{
			Animator2D()->Play(L"Down_Left", false);
		}
		else if (Angle > -112.5f && Angle <= -67.5f)
		{
			Animator2D()->Play(L"Down", false);
		}
		else if (Angle > -67.5f && Angle <= -22.5f)
		{
			Animator2D()->Play(L"Down_Right", false);
		}
	}
	else
	{
		GetOwner()->Destroy();
	}

	Vec3 vRelPos = Transform()->GetRelativePos();
	Transform()->SetRelativePos(vRelPos.x, vRelPos.y, 400.f);
}

void CHeadGunScript::Shot()
{
	Ptr<CSound> pSound = CResMgr::GetInst()->Load<CSound>(L"sound\\headgun\\Shot.wav", L"sound\\headgun\\Shot.wav");
	pSound->Play(1, 0.12f, true);

	Ptr<CPrefab> pPrefab = CResMgr::GetInst()->FindRes<CPrefab>(L"prefab\\BombBullet.pref");
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	Vec3 pPlayerPos = pCurScene->GetPlayerPos();

	Vec3 pTrans = Transform()->GetRelativePos();
	CGameObject* pObj = pPrefab->Instantiate();
	pObj->Transform()->SetRelativePos(Vec3(pTrans.x, pTrans.y, 0.f));

	Vec3 vDir = Vec3(pPlayerPos.x - pTrans.x, pPlayerPos.y - pTrans.y, 0.f);
	vDir.Normalize();

	CMissileScript* pBullet = (CMissileScript*)CScriptMgr::GetScript((UINT)SCRIPT_TYPE::MISSILESCRIPT);
	pBullet->SetDir(vDir);

	pObj->AddComponent(pBullet);

	tEventInfo evninfo;
	evninfo.eType = EVENT_TYPE::CREATE_OBJ;
	evninfo.lParam = (DWORD_PTR)pObj;
	evninfo.wParam = 6;

	CEventMgr::GetInst()->AddEvent(evninfo);

}