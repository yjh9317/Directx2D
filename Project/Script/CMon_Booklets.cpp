#include "pch.h"
#include "CMon_Booklets.h"

#include <Engine/CAnimator2D.h>
#include <Engine/CAnimation2D.h>
#include <Engine/CEventMgr.h>
#include "CM_Bullet.h"

CMon_Booklets::CMon_Booklets()
	: CMonsterScript((int)SCRIPT_TYPE::MON_BOOKLETS)
	,m_AtkCycle(0.f)
	, m_Sound(false)
{
	m_HP = 70;
	m_DetectRange = 400.f;
	m_AttackRange = 250.f;
	m_fSpeed = 50.f;
}

CMon_Booklets::~CMon_Booklets()
{
}


void CMon_Booklets::Idle()
{
	if (m_fDistance < m_DetectRange)
	{
		ChangeState(MON_STATE::TRACE);
	}

	Animator2D()->Play(L"Bookllets_Idle",true);
}

void CMon_Booklets::Trace()
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
		m_BehaviorAngle = m_Angle;
		ChangeState(MON_STATE::ATK);
	}

	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	Vec2 vPlayerPos = Vec2(pCurScene->GetPlayerPos().x, pCurScene->GetPlayerPos().y);
	Vec2 vMonPos = Vec2(Transform()->GetRelativePos().x, Transform()->GetRelativePos().y);
	// 몬스터에서 플레이어를 향한 방향을 알아낸다.
	Vec2 vDir = vPlayerPos - vMonPos;
	vDir.Normalize();

	// 몬스터를 플레이어를 향해서 이동시킨다.
	vMonPos += vDir * m_fSpeed * DT;
	Transform()->SetRelativePos(vMonPos.x, vMonPos.y, Transform()->GetRelativePos().y);

	Animator2D()->Play(L"Bookllets_Idle", true);

}

void CMon_Booklets::Attack()
{
	if (!m_Sound)
	{
		m_Sound = true;
		Ptr<CSound> pSound = CResMgr::GetInst()->Load<CSound>(L"sound\\bookllet\\Blast.wav", L"sound\\bookllet\\Blast.wav");
		pSound->Play(1, 0.2f, true);
	}
	m_fTime += DT;

	if (m_fTime > 0.6f && m_fTime<=1.8f)
	{
		m_AtkCycle += DT;
		if (m_AtkCycle >0.1f)
		{
			m_AtkCycle = 0.f;
			Shoot();
		}
	}

	if (m_fTime < 1.9f && m_fTime >=0.f)
	{
		Animator2D()->Play(L"Bookllets_Atk", false);

	}
	else if (m_fTime <= 2.8f && m_fTime >=1.9f)
	{
		Animator2D()->Play(L"Bookllets_Atk_Finish", false);
	}
	else
	{
		m_fTime = 0.f;
		m_AtkCycle = 0.f;
		m_Atkable = false;
		m_Sound=false;
		Animator2D()->FindAnim(L"Bookllets_Atk")->Reset();
		Animator2D()->FindAnim(L"Bookllets_Atk_Finish")->Reset();
		ChangeState(MON_STATE::IDLE);
	}
}

void CMon_Booklets::Dead()
{
	Animator2D()->Play(L"Bookllets_Death", false);
	m_fTime += DT;

	if (!m_DeathSound)
	{
		m_DeathSound = true;
		Ptr<CSound> pSound = CResMgr::GetInst()->Load<CSound>(L"sound\\bookllet\\Dead.wav", L"sound\\bookllet\\Dead.wav");
		pSound->Play(1, 0.1f, true);
	}

	if (m_fTime > 0.4f)
	{
		GetOwner()->Destroy();
	}
}

void CMon_Booklets::Shoot()
{

	Ptr<CPrefab> pPrefab = CResMgr::GetInst()->FindRes<CPrefab>(L"prefab\\M_Bullet.pref");
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	Vec3 pPlayerPos = pCurScene->GetPlayerPos();

	CM_Bullet* pBullet = (CM_Bullet*)CScriptMgr::GetScript((UINT)SCRIPT_TYPE::M_BULLET);

	Vec3 pTrans = Transform()->GetRelativePos();
	CGameObject* pObj = pPrefab->Instantiate();
	pObj->Transform()->SetRelativePos(Vec3(pTrans.x, pTrans.y, 0.f));
	
	
	Vec3 vDir = Vec3(pPlayerPos.x - pTrans.x, pPlayerPos.y - pTrans.y,0.f);
	vDir.Normalize();

	
	pBullet->SetDir(vDir);
	pBullet->SetSpeed(300.f);
	pBullet->WaitCheck(true);
	pBullet->WaitTime(0.1f);
	
	pObj->AddComponent(pBullet);

	tEventInfo evninfo;
	evninfo.eType = EVENT_TYPE::CREATE_OBJ;
	evninfo.lParam = (DWORD_PTR)pObj;
	evninfo.wParam = 6;
	
	CEventMgr::GetInst()->AddEvent(evninfo);
}


void CMon_Booklets::OnCollisionEnter(CGameObject* _OtherObject)
{
	if (_OtherObject->GetName() == L"P_Bullet")
	{
		m_HP -= 15;


	}

	if (_OtherObject->GetName() == L"Ice_Bullet")
	{
		m_HP -= 20;
		m_IceCheck = 1;
	}


	if (m_HP <= 0 && GetState() != MON_STATE::DEAD)
	{
		m_fTime = 0.f;
		ChangeState(MON_STATE::DEAD);
	}

}