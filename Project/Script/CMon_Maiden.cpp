#include "pch.h"
#include "CMon_Maiden.h"

#include <Engine/CAnimator2D.h>
#include <Engine/CAnimation2D.h>

#include <Engine/CEventMgr.h>
#include "CMaiden_Bullet.h"

CMon_Maiden::CMon_Maiden()
	:CMonsterScript((int)SCRIPT_TYPE::MON_MAIDEN)
	, m_bAtk(true)
	, m_bAtk2(true)
	, i_Dir(-3.14f)
	, m_Sound(false)
	, m_WalkSound(false)
	, m_WalkTime(0.f)
{
	m_HP = 150;
	m_DetectRange = 600.f;
	m_AttackRange = 300.f;
	m_fSpeed = 100.f;
}

CMon_Maiden::~CMon_Maiden()
{
}


void CMon_Maiden::Idle()
{

	if (m_fDistance < m_DetectRange)
	{
		ChangeState(MON_STATE::TRACE);
	}

	Animator2D()->Play(L"Maiden_Idle", true);
}

void CMon_Maiden::Trace()
{
	if (!m_WalkSound)
	{
		m_WalkTime += DT;
	}
	if (m_WalkTime > 0.2f)
	{
		m_WalkSound = true;
	}
	if (m_WalkSound)
	{
		m_WalkTime = 0.f;
		m_WalkSound = false;
		Ptr<CSound> pSound = CResMgr::GetInst()->Load<CSound>(L"sound\\leadmaiden\\Walk.wav", L"sound\\leadmaiden\\Walk.wav");
		pSound->Play(1, 0.03f, true);
	}

	if (!m_Atkable)
	{
		if (m_fAtkTime >= 2.5f)
		{
			m_bAtk = true;
			m_bAtk2 = true;
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

	Animator2D()->Play(L"Maiden_Trace", true);
}

void CMon_Maiden::Attack()
{
	if (!m_Sound)
	{
		Ptr<CSound> pSound = CResMgr::GetInst()->Load<CSound>(L"sound\\leadmaiden\\Open.wav", L"sound\\leadmaiden\\Open.wav");
		pSound->Play(1, 0.07f, true);
		m_Sound = true;
	}
	m_Atkable = false;
	m_fTime += DT;

	if (m_fTime > 0.5f && m_bAtk)
	{
		m_bAtk = false;
		Shoot();
	}

	if (m_fTime > 1.1f && m_bAtk2)
	{
		m_bAtk2 = false;
		Shoot2();
	}

	if (m_fTime >= 0.f && m_fTime <= 3.0f)
	{
		Animator2D()->Play(L"Maiden_Atk", false);
	}
	else
	{
		//Ptr<CSound> pSound = CResMgr::GetInst()->Load<CSound>(L"sound\\leadmaiden\\Close.wav", L"sound\\leadmaiden\\Close.wav");
		//pSound->Play(1, 0.12f, true);
		m_fTime = 0.f;
		i_Dir = -3.14f;
		Animator2D()->FindAnim(L"Maiden_Atk")->Reset();
		m_Sound = false;
		ChangeState(MON_STATE::IDLE);
	}
}

void CMon_Maiden::Dead()
{
	m_fTime += DT;
	Animator2D()->Play(L"Maiden_Dead", true);



	if (m_fTime > 0.4f)
	{
		Ptr<CPrefab> pPrefab = CResMgr::GetInst()->FindRes<CPrefab>(L"prefab\\Maiden_Dead.pref");
		CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
		Vec3 pTrans = Transform()->GetRelativePos();
		tEventInfo evninfo;

		CGameObject* pObj = pPrefab->Instantiate();

		pObj->Transform()->SetRelativePos(Vec3(pTrans.x, pTrans.y, 100.f));
		pObj->Transform()->SetRelativeScale(Vec3(250.f, 250.f, 100.f));

		evninfo.eType = EVENT_TYPE::CREATE_OBJ;
		evninfo.lParam = (DWORD_PTR)pObj;
		evninfo.wParam = 0;

		CEventMgr::GetInst()->AddEvent(evninfo);

		GetOwner()->Destroy();
	}
}

void CMon_Maiden::OnCollisionEnter(CGameObject* _OtherObject)
{
	if (_OtherObject->GetName() == L"P_Bullet" && !(GetState() == MON_STATE::DEAD))
	{
		m_HP -= 15;

		if (m_HP <= 0 && GetState() != MON_STATE::DEAD)
		{
			Ptr<CSound> pSound = CResMgr::GetInst()->Load<CSound>(L"sound\\leadmaiden\\Dead.wav", L"sound\\leadmaiden\\Dead.wav");
			pSound->Play(1, 0.12f, true);
			m_fTime = 0.f;
			ChangeState(MON_STATE::DEAD);
		}
	}

	if (_OtherObject->GetName() == L"Ice_Bullet")
	{
		m_HP -= 20;
		m_IceCheck = 1;

		if (m_HP <= 0 && GetState() != MON_STATE::DEAD)
		{
			Ptr<CSound> pSound = CResMgr::GetInst()->Load<CSound>(L"sound\\leadmaiden\\Dead.wav", L"sound\\leadmaiden\\Dead.wav");
			pSound->Play(1, 0.12f, true);
			m_fTime = 0.f;
			ChangeState(MON_STATE::DEAD);
		}
	}
}


void CMon_Maiden::Shoot()
{
	Ptr<CPrefab> pPrefab = CResMgr::GetInst()->FindRes<CPrefab>(L"prefab\\Maiden_Bullet.pref");
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	Vec3 pTrans = Transform()->GetRelativePos();
	tEventInfo evninfo;

	CGameObject* pObj;
	for (int i = 0; i < 10; ++i)
	{

	 pObj= pPrefab->Instantiate();

	 pObj->Transform()->SetRelativePos(Vec3(pTrans.x, pTrans.y, 0.f));


	 i_Dir += 0.628f;

	 float cos = cosf(i_Dir);
	 float sin = sinf(i_Dir);

	 Vec3 vDir = Vec3(cos, sin, 0.f);
	 vDir.Normalize();

	 CMaiden_Bullet* pBullet = (CMaiden_Bullet*)CScriptMgr::GetScript((UINT)SCRIPT_TYPE::MAIDEN_BULLET);
	 pBullet->SetDir(vDir);
	 pBullet->SetSpeed(200.f);
	 pObj->AddComponent(pBullet);

	 evninfo.eType = EVENT_TYPE::CREATE_OBJ;
	 evninfo.lParam = (DWORD_PTR)pObj;
	 evninfo.wParam = 6;

	 CEventMgr::GetInst()->AddEvent(evninfo);

	}
}

void CMon_Maiden::Shoot2()
{
	i_Dir = 0.4f;

	Ptr<CPrefab> pPrefab = CResMgr::GetInst()->FindRes<CPrefab>(L"prefab\\Maiden_Bullet.pref");
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	Vec3 pTrans = Transform()->GetRelativePos();
	tEventInfo evninfo;

	CGameObject* pObj;
	for (int i = 0; i < 10; ++i)
	{

		pObj = pPrefab->Instantiate();

		pObj->Transform()->SetRelativePos(Vec3(pTrans.x, pTrans.y, 0.f));


		i_Dir += 0.628f;

		float cos = cosf(i_Dir);
		float sin = sinf(i_Dir);

		Vec3 vDir = Vec3(cos, sin, 0.f);
		vDir.Normalize();

		CMaiden_Bullet* pBullet = (CMaiden_Bullet*)CScriptMgr::GetScript((UINT)SCRIPT_TYPE::MAIDEN_BULLET);
		pBullet->SetDir(vDir);
		pBullet->SetSpeed(200.f);
		pObj->AddComponent(pBullet);

		evninfo.eType = EVENT_TYPE::CREATE_OBJ;
		evninfo.lParam = (DWORD_PTR)pObj;
		evninfo.wParam = 6;

		CEventMgr::GetInst()->AddEvent(evninfo);

	}
}
