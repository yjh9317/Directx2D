#include "pch.h"
#include "CMonsterScript.h"

#include <Engine/CEventMgr.h>

#include <Engine/CAnimator2D.h>
#include <Engine/CAnimation2D.h>

#include <Engine/CSceneMgr.h>
#include <Engine/CScene.h>
#include <Engine/CLayer.h>

CMonsterScript::CMonsterScript()
	: CScript((int)SCRIPT_TYPE::MONSTERSCRIPT)
	, m_eState(MON_STATE::IDLE)
	, m_fTime(0.f)
	, m_fDistance(0.f)
	, m_fSpeed(0.f)
	, m_Atkable(true)
	, m_fAtkTime(0.f)
	, m_Dir(0.f,0.f)
	, m_Angle(0.f)
	, m_Atk(false)
	, m_IceCheck(0)
	, m_IceTime(0.f)
{
}

CMonsterScript::CMonsterScript(int _iScript)
	:CScript((int)_iScript)
	,m_eState(MON_STATE::IDLE)
	, m_fTime(0.f)
	, m_fDistance(0.f)
	, m_fSpeed(0.f)
	, m_Atkable(true)
	, m_fAtkTime(0.f)
	, m_Dir(0.f, 0.f)
	, m_Angle(0.f)
	, m_Atk(false)
	, m_IceCheck(0)
	, m_IceTime(0.f)
{
}

CMonsterScript::~CMonsterScript()
{
}

void CMonsterScript::start()
{

}

void CMonsterScript::update()
{
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	Vec3 vMonPos = Transform()->GetWorldPos();
	Vec3 vPos = pCurScene->GetPlayerPos();
	m_fDistance = (float)sqrtf(powf(vMonPos.x - vPos.x, 2.f) + powf(vMonPos.y - vPos.y, 2.f));
	m_Dir = Vec2(vPos.x - vMonPos.x, vPos.y-vMonPos.y);
	m_Dir.Normalize();
	const double Rad_to_deg = 57.2957951;
	m_Angle = atan2f(m_Dir.y, m_Dir.x);
	m_Angle *= (float)Rad_to_deg;

	if (m_eState == MON_STATE::DEAD)
	{
		Dead();
		return;
	}

	switch (m_eState)
	{
	case MON_STATE::START:
		AnimStart();
		break;
	case MON_STATE::IDLE:
		Idle();
		break;
	case MON_STATE::TRACE:
		Trace();
		break;
	case MON_STATE::ATK:
		Attack();
		break;
	case MON_STATE::HIT:
		Hit();
		break;
	case MON_STATE::SKILL1:
		Skill_1();
		break;
	case MON_STATE::SKILL2:
		Skill_2();
		break;
	case MON_STATE::SKILL3:
		Skill_3();
		break;
	case MON_STATE::SKILL4:
		Skill_4();
		break;
	case MON_STATE::SKILL5:
		Skill_5();
		break;
	}

	GetOwner()->GetRenderComponent()->GetSharedMaterial()->SetScalarParam(L"IceCheck", &m_IceCheck);

	if ( 1 == m_IceCheck)
	{
		if (!m_bPrevSpeed)
		{
			m_bPrevSpeed = true;
			m_fPrevSpeed = m_fSpeed;
			m_fSpeed /= 2.f;
		}
		m_IceTime += DT;

		if (m_IceTime > 2.f)
		{
			m_IceTime = 0.f;
			m_IceCheck = 0;

			m_bPrevSpeed = false;
			m_fSpeed = m_fPrevSpeed;
		}
	}
}

void CMonsterScript::ChangeState(MON_STATE _eState)
{
	tEventInfo evn = {};
	evn.eType = EVENT_TYPE::CHANGE_AI_STATE;
	evn.lParam = (DWORD_PTR)this;
	evn.wParam = (DWORD_PTR)_eState;

	CEventMgr::GetInst()->AddEvent(evn);
}

void CMonsterScript::SetState(MON_STATE _eState)
{
	if (m_eState == _eState)
		return;

	m_eState = _eState;
}




void CMonsterScript::Trace()
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
	Vec2 vMonPos = Vec2(Transform()->GetWorldPos().x, Transform()->GetWorldPos().y);
	// 몬스터에서 플레이어를 향한 방향을 알아낸다.
	Vec2 vDir = vPlayerPos - vMonPos;
	vDir.Normalize();

	// 몬스터를 플레이어를 향해서 이동시킨다.
	vMonPos += vDir * m_fSpeed * DT;
	Transform()->SetRelativePos(vMonPos.x, vMonPos.y, Transform()->GetRelativePos().y);
}

void CMonsterScript::Attack()
{
	if (m_fDistance < m_DetectRange)
	{
		ChangeState(MON_STATE::TRACE);
		m_Atkable = false;
	}
}
