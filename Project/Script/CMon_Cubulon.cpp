#include "pch.h"
#include "CMon_Cubulon.h"

#include <Engine/CAnimator2D.h>
#include <Engine/CAnimation2D.h>
#include <Engine/CScript.h>
#include <Engine/CEventMgr.h>
#include "CM_Bullet.h"

#include "CAStarScript.h"

CMon_Cubulon::CMon_Cubulon()
	:CMonsterScript((int)SCRIPT_TYPE::MON_CUBULON)
{
	//	SetName(L"CMon_Cubulon");
	m_HP = 110;
	m_DetectRange = 400.f;
	m_AttackRange = 300.f;
	m_fSpeed = 100.f;
}

CMon_Cubulon::~CMon_Cubulon()
{
}


void CMon_Cubulon::Idle()
{
	if (m_fDistance < m_DetectRange)
	{
		ChangeState(MON_STATE::TRACE);
	}

	if(m_Angle<=90.f && m_Angle>=45.f)
		Animator2D()->Play(L"Back_R_Idle", true);
	else if (m_Angle <= 135.f && m_Angle >= 90.f)
		Animator2D()->Play(L"Back_L_Idle", true);
	else if((m_Angle<=181.f && m_Angle >135.f) || (m_Angle <=-90.f && m_Angle>=-181.f))
		Animator2D()->Play(L"Left_Idle", true);
	else if ((m_Angle <= 45.f && m_Angle > 0.f) || (m_Angle <= 0.f && m_Angle >= -90.f))
		Animator2D()->Play(L"Right_Idle", true);
}

void CMon_Cubulon::Trace()
{
	if (!m_Atkable)
	{
		if (m_fAtkTime >= 1.5f)
		{
			m_Atkable = true;
			m_fAtkTime = 0.f;
		}
		m_fAtkTime += DT;
	}

	if (m_fDistance < m_AttackRange && m_Atkable)
	{
		m_fTime=0.f;
		m_BehaviorAngle = m_Angle;
		ChangeState(MON_STATE::ATK);
	}

	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	Vec2 vPlayerPos = Vec2(pCurScene->GetPlayerPos().x, pCurScene->GetPlayerPos().y);
	//Transform()->GetRelativePos();
	Vec2 vMonPos = Vec2(Transform()->GetRelativePos().x, Transform()->GetRelativePos().y);
	// 몬스터에서 플레이어를 향한 방향을 알아낸다.

	//CAStarScript* pAStarScript = GetOwner()->GetScript<CAStarScript>();
	//pAStarScript->GetDir();
	
	Vec2 vDir = vPlayerPos - vMonPos;
	vDir.Normalize();
	//Vec2 vDir = pAStarScript->GetDir();

	// 몬스터를 플레이어를 향해서 이동시킨다.
	vMonPos += vDir * m_fSpeed * DT;
	Transform()->SetRelativePos(vMonPos.x, vMonPos.y, Transform()->GetRelativePos().y);



	if (m_Angle <= 90.f && m_Angle >= 45.f)
		Animator2D()->Play(L"Back_R_Idle", true);
	else if (m_Angle <= 135.f && m_Angle >= 90.f)
		Animator2D()->Play(L"Back_L_Idle", true);
	else if ((m_Angle <= 181.f && m_Angle > 135.f) || (m_Angle <= -90.f && m_Angle >= -181.f))
		Animator2D()->Play(L"Left_Idle", true);
	else if ((m_Angle <= 45.f && m_Angle > 0.f) || (m_Angle <= 0.f && m_Angle >= -90.f))
		Animator2D()->Play(L"Right_Idle", true);
} 

void CMon_Cubulon::Attack()
{
	m_fTime += DT;
	

	if (m_fTime >= 0.7f && m_Atkable)
	{
		Ptr<CSound> pSound = CResMgr::GetInst()->Load<CSound>(L"sound\\cubulon\\Atk.wav", L"sound\\cubulon\\Atk.wav");
		pSound->Play(1, 0.12f, true);
		m_Atkable = false;
		Shoot();
	}

	if ((m_BehaviorAngle <= 90.f && m_BehaviorAngle >= 0.f) || (m_BehaviorAngle >= -90.f && m_BehaviorAngle <= 0.f))
	{
		if(m_fTime<=1.f && m_fTime>=0.f)
		Animator2D()->Play(L"Right_Atk", false);
		else
		{
			Animator2D()->FindAnim(L"Right_Atk")->Reset();
			m_BehaviorAngle = 0.f;
			m_fTime = 0.f;
			m_Atkable = false;
			ChangeState(MON_STATE::IDLE);
		}
	}
	else if ((m_BehaviorAngle <= 181.f && m_BehaviorAngle > 90.f) || (m_BehaviorAngle >= -181.f && m_BehaviorAngle < 90.f))
	{
		if (m_fTime <= 1.f && m_fTime >= 0.f)
			Animator2D()->Play(L"Left_Atk", false);
		else
		{
			Animator2D()->FindAnim(L"Left_Atk")->Reset();
			m_BehaviorAngle = 0.f;
			m_fTime = 0.f;
			m_Atkable = false;
			ChangeState(MON_STATE::IDLE);
		}
	}
}

void CMon_Cubulon::Hit()
{
	m_fTime += DT;

	if ((m_Angle >= 0.f && m_Angle <= 90.f) || (m_Angle <= 0.f && m_Angle >= -90.f))
		Animator2D()->Play(L"Right_Hit", true);
	else if ((m_Angle >= 90.f && m_Angle <= 181.f) || (m_Angle <= -90.f && m_Angle >= -181.f))
		Animator2D()->Play(L"Left_Hit", true);


	if (m_fTime > 0.5f)
	{
		Animator2D()->FindAnim(L"Right_Hit")->Reset();
		Animator2D()->FindAnim(L"Left_Hit")->Reset();
		m_fTime = 0.f;
		ChangeState(MON_STATE::IDLE);
	}
}

void CMon_Cubulon::Dead()
{
	m_fTime += DT;
	
	if (!m_Sound)
	{
		m_Sound = true;
		Ptr<CSound> pSound = CResMgr::GetInst()->Load<CSound>(L"sound\\cubulon\\Dead.wav", L"sound\\cubulon\\Dead.wav");
		pSound->Play(1, 0.12f, false);
	}

	Animator2D()->Play(L"Dead", false);

	if(m_fTime>0.4f)
		GetOwner()->Destroy();
}

void CMon_Cubulon::OnCollisionEnter(CGameObject* _OtherObject)
{
	if (_OtherObject->GetName() == L"P_Bullet")
	{
		m_HP -= 15;

		if(GetState() == MON_STATE::IDLE || GetState() == MON_STATE::TRACE)
			ChangeState(MON_STATE::HIT);

		if (m_HP <= 0 && GetState() != MON_STATE::DEAD)
		{
			m_fTime = 0.f;
			ChangeState(MON_STATE::DEAD);
		}
	}

	if (_OtherObject->GetName() == L"Ice_Bullet")
	{
		m_HP -= 20;
		m_IceCheck = 1;

		if (GetState() == MON_STATE::IDLE || GetState() == MON_STATE::TRACE)
			ChangeState(MON_STATE::HIT);

		if (m_HP <= 0 && GetState() != MON_STATE::DEAD)
		{
			m_fTime = 0.f;
			ChangeState(MON_STATE::DEAD);
		}
	}
}

void CMon_Cubulon::Shoot()
{
	Ptr<CPrefab> pPrefab = CResMgr::GetInst()->FindRes<CPrefab>(L"prefab\\M_Bullet.pref");
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	tEventInfo evninfo;
	CGameObject* pObj;
	
	for (int i = 0; i <= 4; ++i)
	{
		pObj = pPrefab->Instantiate();
		pObj->Transform()->SetRelativePos(GetOwner()->Transform()->GetWorldPos());

		CM_Bullet* pBullet = (CM_Bullet*)CScriptMgr::GetScript((UINT)SCRIPT_TYPE::M_BULLET);
		pBullet->SetSpeed(200.f);
		pBullet->SetDir(Vec3(1.f - (i * 0.25f), (i * 0.25f), 0.f));
		pObj->AddComponent(pBullet);

		evninfo.eType = EVENT_TYPE::CREATE_OBJ;
		evninfo.lParam = (DWORD_PTR)pObj;
		evninfo.wParam = 6;

		CEventMgr::GetInst()->AddEvent(evninfo);
	}

	for (int i = 1; i <= 4; ++i)
	{
		pObj = pPrefab->Instantiate();
		pObj->Transform()->SetRelativePos(GetOwner()->Transform()->GetWorldPos());

		CM_Bullet* pBullet = (CM_Bullet*)CScriptMgr::GetScript((UINT)SCRIPT_TYPE::M_BULLET);
		pBullet->SetSpeed(200.f);
		pBullet->SetDir(Vec3(1.f -(i * 0.25f), -(i * 0.25f), 0.f));
		pObj->AddComponent(pBullet);


		evninfo.eType = EVENT_TYPE::CREATE_OBJ;
		evninfo.lParam = (DWORD_PTR)pObj;
		evninfo.wParam = 6;

		CEventMgr::GetInst()->AddEvent(evninfo);
	}

	for (int i = 0; i <= 4; ++i)
	{
		//CM_Bullet* Bullet = (CM_Bullet*)pObj->GetScripts()[0];
		pObj = pPrefab->Instantiate();
		pObj->Transform()->SetRelativePos(GetOwner()->Transform()->GetWorldPos());

		CM_Bullet* pBullet = (CM_Bullet*)CScriptMgr::GetScript((UINT)SCRIPT_TYPE::M_BULLET);
		pBullet->SetSpeed(200.f);
		pBullet->SetDir(Vec3((i * 0.25f)-1.f, (i * 0.25f), 0.f));
		pObj->AddComponent(pBullet);
		

		evninfo.eType = EVENT_TYPE::CREATE_OBJ;
		evninfo.lParam = (DWORD_PTR)pObj;
		evninfo.wParam = 6;

		CEventMgr::GetInst()->AddEvent(evninfo);
	}

	for (int i = 1; i <= 4; ++i)
	{
		pObj = pPrefab->Instantiate();
		pObj->Transform()->SetRelativePos(GetOwner()->Transform()->GetWorldPos());

		CM_Bullet* pBullet = (CM_Bullet*)CScriptMgr::GetScript((UINT)SCRIPT_TYPE::M_BULLET);
		pBullet->SetSpeed(200.f);
		pBullet->SetDir(Vec3((i * 0.25f) - 1.f, -(i * 0.25f), 0.f));
		pObj->AddComponent(pBullet);


		evninfo.eType = EVENT_TYPE::CREATE_OBJ;
		evninfo.lParam = (DWORD_PTR)pObj;
		evninfo.wParam = 6;

		CEventMgr::GetInst()->AddEvent(evninfo);
	}
}
