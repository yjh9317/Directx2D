#include "pch.h"
#include "CMon_Bullats.h"

#include <Engine/CAnimator2D.h>
#include <Engine/CAnimation2D.h>

#include "CWallScript.h"
#include "CTableScript.h"
#include "CDoorScript.h"

CMon_Bullats::CMon_Bullats()
	:CMonsterScript((int)SCRIPT_TYPE::MON_BULLATS)
	, m_Dir(0.f, 0.f)
	, m_Sound(false)
	, m_DeathSound(false)
{
	m_HP = 15;
	m_DetectRange = 300.f;
	m_AttackRange = 150.f;
	m_fSpeed = 120.f;
}

CMon_Bullats::~CMon_Bullats()
{
}


void CMon_Bullats::Idle()
{
	if (m_fDistance < m_DetectRange)
	{
		ChangeState(MON_STATE::TRACE);
	}

	Animator2D()->Play(L"Bullats_Idle", true);
}

void CMon_Bullats::Trace()
{
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	Vec2 vPlayerPos = Vec2(pCurScene->GetPlayerPos().x, pCurScene->GetPlayerPos().y);
	Vec2 vMonPos = Vec2(Transform()->GetRelativePos().x, Transform()->GetRelativePos().y);
	if (m_fDistance < m_AttackRange)
	{
		m_Dir = vPlayerPos - vMonPos;
		m_Dir.Normalize();
		ChangeState(MON_STATE::ATK);
	}

	// 몬스터에서 플레이어를 향한 방향을 알아낸다.
	Vec2 vDir = vPlayerPos - vMonPos;
	vDir.Normalize();

	// 몬스터를 플레이어를 향해서 이동시킨다.
	vMonPos += vDir * m_fSpeed * DT;
	Transform()->SetRelativePos(vMonPos.x, vMonPos.y, Transform()->GetRelativePos().y);

	Animator2D()->Play(L"Bullats_Idle", true);
}

void CMon_Bullats::Attack()
{
	m_fTime +=DT;

	Ptr<CSound> pSound;

	if (m_fTime >= 0.f && m_fTime < 0.8f)
	{
		if (!m_Sound)
		{
			m_Sound = true;
			pSound = CResMgr::GetInst()->Load<CSound>(L"sound\\bullat\\Charge.wav", L"sound\\bullat\\Charge.wav");
			pSound->Play(1, 0.2f, true);
		}
		Animator2D()->Play(L"Bullats_Atk_Prev", true);
	}
	else if (m_fTime >= 0.8f)
	{
		if (!m_Sound)
		{
			m_Sound = true;
			pSound = CResMgr::GetInst()->Load<CSound>(L"sound\\bullat\\Tackle.wav", L"sound\\bullat\\Tackle.wav");
			pSound->Play(1, 0.2f, true);
		}
		Vec2 vMonPos = Vec2(Transform()->GetRelativePos().x, Transform()->GetRelativePos().y);
		vMonPos += m_Dir * m_fSpeed * DT *2.f;
		Transform()->SetRelativePos(vMonPos.x, vMonPos.y, Transform()->GetRelativePos().y);
		Animator2D()->Play(L"Bullats_Atk", true);
	}

}

void CMon_Bullats::Dead()
{
	m_fTime += DT;

	if (!m_DeathSound)
	{
		m_DeathSound = true;
		Ptr<CSound> pSound = CResMgr::GetInst()->Load<CSound>(L"sound\\bullat\\Death.wav", L"sound\\bullat\\Death.wav");
		pSound->Play(1, 0.15f, true);
	}

	Animator2D()->Play(L"Bullats_Dead", false);

	if (m_fTime > 0.4f)
	{
		GetOwner()->Destroy();
	}
}

void CMon_Bullats::OnCollisionEnter(CGameObject* _OtherObject)
{
	MON_STATE _State = GetState();

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
		Animator2D()->FindAnim(L"Bullats_Dead")->Reset();
		ChangeState(MON_STATE::DEAD);
	}


	if (_State == MON_STATE::ATK)
	{
		if (_OtherObject->GetName() == L"Player" || _OtherObject->GetScript<CWallScript>() || _OtherObject->GetScript<CDoorScript>())
		{
			m_fTime = 0.f;
			Animator2D()->FindAnim(L"Bullats_Dead")->Reset();
			ChangeState(MON_STATE::DEAD);
		}

		if (_OtherObject->GetScript<CTableScript>())
		{
			CTableScript* pScript = _OtherObject->GetScript<CTableScript>();
			if (TABLE_TYPE::Stand == pScript->GetType() && GetState() != MON_STATE::DEAD)
			{
				m_fTime = 0.f;
				Animator2D()->FindAnim(L"Bullats_Dead")->Reset();
				ChangeState(MON_STATE::DEAD);
			}
		}
	}
}
