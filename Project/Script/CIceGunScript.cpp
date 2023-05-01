#include "pch.h"
#include "CIceGunScript.h"

#include <Engine/CEventMgr.h>
#include <Engine/CAnimator2D.h>
#include <Engine/CAnimation2D.h>
#include "CPlayerStateScript.h"
#include "CP_Bullet.h"
#include "CIce_Bullet.h"

CIceGunScript::CIceGunScript()
	:CScript((int)SCRIPT_TYPE::ICEGUNSCRIPT)
	, m_fTime(0.f)
	, m_Atk(true)
	, m_Dir(false)
	, m_ShootCheck(false)
	, m_ShootTime(0.f)
	, m_type(GUN_TYPE::LIE)
{
	m_Pos = Vec3(21.f, -20.f, 0.f);
}


CIceGunScript::~CIceGunScript()
{
}


void CIceGunScript::update()
{
	if (m_type == GUN_TYPE::LIE)
	{
		CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
		Vec3 vPlayerPos = pCurScene->GetPlayerPos();
		Vec3 vPos = Transform()->GetWorldPos();

		float PlayerXPos = vPlayerPos.x;
		float PlayerYPos = vPlayerPos.y;

		float XPos = vPos.x;
		float YPos = vPos.y + 20.f;

		float distance = sqrtf(powf(PlayerXPos - XPos, 2) + powf(PlayerYPos - YPos, 2));

		if ((distance <= 70.f && distance >= 0.f) && KEY_TAP(KEY::E))
		{
			Ptr<CSound> pSound = CResMgr::GetInst()->Load<CSound>(L"sound\\item\\Pickup.wav", L"sound\\item\\Pickup.wav");
			pSound->Play(1, 0.2f, true);

			CGameObject* pObj = pCurScene->GetPlayer();
			tEventInfo evninfo;
			evninfo.eType = EVENT_TYPE::ADD_CHILD;
			evninfo.lParam = (DWORD_PTR)pObj;
			evninfo.wParam = (DWORD_PTR)GetOwner();
			CEventMgr::GetInst()->AddEvent(evninfo);

			m_type = GUN_TYPE::HOLD;

			CPlayerStateScript* pScript = pObj->GetScript<CPlayerStateScript>();
			pScript->RegistGun(GetOwner());

		}
	}
	else if (m_type == GUN_TYPE::HOLD)
	{
		if (m_fTime > 0.4f)
		{
			m_Atk = true;
		}
		else if (!m_Atk)
		{
			m_fTime += DT;
		}


		Transform()->SetIgnoreParentScale(true);

		Vec2 Resolution = CDevice::GetInst()->GetRenderResolution();//Vec2(CRenderMgr::GetInst()->GetMainCamWidth(), CRenderMgr::GetInst()->GetMainCamHeight());
		Vec2 MousePos = CKeyMgr::GetInst()->GetMousePos();

		Vec2 Win_MousePos = Vec2(MousePos.x - Resolution.x / 2.f, -MousePos.y + Resolution.y / 2.f);

		Vec3 Pos = Transform()->GetRelativePos();

		Vec2 Dir = Vec2(Win_MousePos.x - Pos.x, Win_MousePos.y - Pos.y);
		Dir.Normalize();
		float m_Angle = atan2f(Dir.y, Dir.x);


		const double Rad_to_deg = 57.2957951;

		float Angle = m_Angle * (float)Rad_to_deg;

		Vec3 vRot = Transform()->GetRelativeRotation();

		if (m_Dir)
		{
			if (Angle > 110.f || Angle < -110.f)
				m_Dir = false;

			Animator2D()->Play(L"R_IceGun", true);
			Transform()->SetRelativePos(Vec3(m_Pos.x, m_Pos.y, m_Pos.z));
			vRot.z = m_Angle;
		}
		else if (!m_Dir)
		{
			if ((Angle < 70.f && Angle >0.f) || (Angle <0.f && Angle>-70.f))
				m_Dir = true;

			Animator2D()->Play(L"L_IceGun", true);
			Transform()->SetRelativePos(Vec3(-m_Pos.x, m_Pos.y, m_Pos.z));
			vRot.z = m_Angle - XM_PI;
		}

		Transform()->SetRelativeRotation(vRot);

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



		if (KEY_TAP(KEY::LBTN) && m_Atk)
		{
			Ptr<CSound> pSound = CResMgr::GetInst()->Load<CSound>(L"sound\\IceGun\\Shot.wav", L"sound\\IceGun\\Shot.wav");
			pSound->Play(1, 0.12f, true);

			if (!m_ShootCheck)
				m_ShootCheck = true;

			m_fTime = 0.f;
			m_Atk = false;
			Ptr<CPrefab> pPrefab = CResMgr::GetInst()->FindRes<CPrefab>(L"prefab\\Ice_Bullet.pref");
			CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();

			CGameObject* pObj = pPrefab->Instantiate();


			CIce_Bullet* pBullet = (CIce_Bullet*)CScriptMgr::GetScript((UINT)SCRIPT_TYPE::ICE_BULLET);

			pBullet->SetSpeed(500.f);
			pBullet->SetDir(Vec3(Dir.x, Dir.y, 0.f));
			pObj->Transform()->SetRelativePos(GetOwner()->Transform()->GetWorldPos() + Dir * Vec2(40.f, 40.f));
			pObj->AddComponent(pBullet);

			tEventInfo evninfo;

			evninfo.eType = EVENT_TYPE::CREATE_OBJ;
			evninfo.lParam = (DWORD_PTR)pObj;
			evninfo.wParam = 3;

			CEventMgr::GetInst()->AddEvent(evninfo);
		}
	}
}

void CIceGunScript::OnCollisionEnter(CGameObject* _pOtherObj)
{

}
