#include "pch.h"
#include "CMon_Gigi.h"

#include <Engine/CAnimator2D.h>
#include <Engine/CAnimation2D.h>
#include <Engine/CEventMgr.h>

#include "CM_Bullet.h"


CMon_Gigi::CMon_Gigi()
	:CMonsterScript((int)SCRIPT_TYPE::MON_GIGI)
{
	m_HP = 75;
	m_DetectRange = 500.f;
	m_AttackRange = 350.f;
	m_fSpeed = 80.f;
}

CMon_Gigi::~CMon_Gigi()
{
}

void CMon_Gigi::start()
{
	SetState(MON_STATE::IDLE);
}

void CMon_Gigi::Idle()
{
	if (m_fDistance < m_DetectRange)
	{
		ChangeState(MON_STATE::TRACE);
	}

	if ((m_Angle <= 181.f && m_Angle >= 90.f) || (m_Angle <= -90.f && m_Angle >= -181.f))
	Animator2D()->Play(L"Left_Idle", true);
	else if ((m_Angle < 90.f && m_Angle > 0.f) || (m_Angle <= 0.f && m_Angle >= -90.f))
	Animator2D()->Play(L"Right_Idle", true);
}

void CMon_Gigi::Trace()
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
		m_fTime = 0.f;
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

	if ((m_Angle <= 181.f && m_Angle >= 90.f) || (m_Angle <= -90.f && m_Angle >= -181.f))
		Animator2D()->Play(L"Left_Idle", true);
	else if ((m_Angle < 90.f && m_Angle > 0.f) || (m_Angle <= 0.f && m_Angle >= -90.f))
		Animator2D()->Play(L"Right_Idle", true);
}

void CMon_Gigi::Attack()
{
	if (m_fTime >=1.f && m_Atkable)
	{
		Shoot();
		m_Atkable = false;
	}

	m_fTime += DT;
	if ((m_BehaviorAngle <= 181.f && m_BehaviorAngle >= 90.f) || (m_BehaviorAngle <= -90.f && m_BehaviorAngle >= -181.f))
	{
		if (m_fTime <= 1.2f && m_fTime >= 0.f)
		{
			Animator2D()->Play(L"Left_Atk", false);
		}
		else
		{
			Animator2D()->FindAnim(L"Left_Atk")->Reset();
			m_BehaviorAngle = 0.f;
			m_fTime = 0.f;
			ChangeState(MON_STATE::IDLE);
		}
	}
	else if ((m_BehaviorAngle < 90.f && m_BehaviorAngle > 0.f) || (m_BehaviorAngle <= 0.f && m_BehaviorAngle >= -90.f))
	{
		if (m_fTime <= 1.2f && m_fTime >= 0.f)
		{
			Animator2D()->Play(L"Right_Atk", false);
		}
		else
		{
			Animator2D()->FindAnim(L"Right_Atk")->Reset();
			m_BehaviorAngle = 0.f;
			m_fTime = 0.f;
			ChangeState(MON_STATE::IDLE);
		}
	}
}

void CMon_Gigi::Hit()
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

void CMon_Gigi::Dead()
{
	m_fTime += DT;

	if (!m_Sound)
	{
		m_Sound = true;
		Ptr<CSound> pSound = CResMgr::GetInst()->Load<CSound>(L"sound\\gigi\\Dead.wav", L"sound\\gigi\\Dead.wav");
		pSound->Play(1, 0.12f, true);
	}

	Animator2D()->Play(L"Left_Dead", true);

	if (m_fTime >= 0.4f)
	{
		GetOwner()->Destroy();
	}
}

void CMon_Gigi::Shoot()
{
	Ptr<CPrefab> pPrefab = CResMgr::GetInst()->FindRes<CPrefab>(L"prefab\\M_Bullet.pref");
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	tEventInfo evninfo;

	CGameObject* pObj = pPrefab->Instantiate();

	Vec3 pPlayerPos = pCurScene->GetPlayerPos();
	Vec3 pTrans = Transform()->GetRelativePos();

	pObj->Transform()->SetRelativePos(Vec3(pTrans.x, pTrans.y, 0.f));

	Vec3 vDir = Vec3(pPlayerPos.x - pTrans.x, pPlayerPos.y - pTrans.y, 0.f);
	vDir.Normalize();

	float m_Angle = atan2f(vDir.y, vDir.x);

	

	CM_Bullet* pBullet = (CM_Bullet*)CScriptMgr::GetScript((UINT)SCRIPT_TYPE::M_BULLET);
	pBullet->SetDir(vDir);
	pBullet->SetSpeed(400.f);
	pObj->AddComponent(pBullet);

	evninfo.eType = EVENT_TYPE::CREATE_OBJ;
	evninfo.lParam = (DWORD_PTR)pObj;
	evninfo.wParam = 6;

	CEventMgr::GetInst()->AddEvent(evninfo);

	pObj = pPrefab->Instantiate();

	pPlayerPos = pCurScene->GetPlayerPos();
	pTrans = Transform()->GetRelativePos();

	pObj->Transform()->SetRelativePos(Vec3(pTrans.x, pTrans.y, 0.f));


	float fsin = sinf(m_Angle+0.4f);
	float fcos = cosf(m_Angle+0.4f);

	vDir = Vec3(fcos, fsin, 0.f);
	vDir.Normalize();

	pBullet = (CM_Bullet*)CScriptMgr::GetScript((UINT)SCRIPT_TYPE::M_BULLET);
	pBullet->SetDir(vDir);
	pBullet->SetSpeed(400.f);
	pObj->AddComponent(pBullet);

	evninfo.eType = EVENT_TYPE::CREATE_OBJ;
	evninfo.lParam = (DWORD_PTR)pObj;
	evninfo.wParam = 6;

	CEventMgr::GetInst()->AddEvent(evninfo);

	pObj = pPrefab->Instantiate();

	pPlayerPos = pCurScene->GetPlayerPos();
	pTrans = Transform()->GetRelativePos();

	pObj->Transform()->SetRelativePos(Vec3(pTrans.x, pTrans.y, 0.f));


	fsin = sinf(m_Angle-0.4f);
	fcos = cosf(m_Angle-0.4f);

	vDir = Vec3(fcos, fsin, 0.f);
	vDir.Normalize();

	pBullet = (CM_Bullet*)CScriptMgr::GetScript((UINT)SCRIPT_TYPE::M_BULLET);
	pBullet->SetDir(vDir);
	pBullet->SetSpeed(400.f);
	pObj->AddComponent(pBullet);

	evninfo.eType = EVENT_TYPE::CREATE_OBJ;
	evninfo.lParam = (DWORD_PTR)pObj;
	evninfo.wParam = 6;

	CEventMgr::GetInst()->AddEvent(evninfo);

}


void CMon_Gigi::OnCollisionEnter(CGameObject* _OtherObject)
{
	if (_OtherObject->GetName() == L"P_Bullet")
	{

		m_HP -= 15;

		if (GetState() == MON_STATE::IDLE || GetState() == MON_STATE::TRACE)
			ChangeState(MON_STATE::HIT);

		if (m_HP <= 0 && GetState() != MON_STATE::DEAD)
		{
			m_fTime = 0.f;
			Animator2D()->FindAnim(L"Left_Dead")->Reset();
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
			Animator2D()->FindAnim(L"Left_Dead")->Reset();
			ChangeState(MON_STATE::DEAD);
		}
	}
}