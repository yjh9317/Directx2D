#include "pch.h"
#include "CIce_Bullet.h"

#include <Engine/CEventMgr.h>
#include <Engine/CAnimator2D.h>

#include "CMonsterScript.h"
#include "CWallScript.h"

CIce_Bullet::CIce_Bullet()
	:CScript((int)SCRIPT_TYPE::ICE_BULLET)
	, m_Dir{}
	, m_Speed(0.f)
	, m_fTime(0.f)
{
}

CIce_Bullet::~CIce_Bullet()
{
}


void CIce_Bullet::update()
{
	Animator2D()->Play(L"Ice_Bullet", true);

	m_fTime += DT;
	if (m_fTime > 2.f)
	{
		GetOwner()->Destroy();
	}

	Vec3 vTrans = Transform()->GetRelativePos();

	vTrans += DT * m_Dir * m_Speed;
	Transform()->SetRelativePos(vTrans);
}

void CIce_Bullet::OnCollisionEnter(CGameObject* _OtherObject)
{
	if (_OtherObject->GetScript<CMonsterScript>())
	{
		CMonsterScript* pScript = _OtherObject->GetScript<CMonsterScript>();
		if (pScript->GetState() != MON_STATE::DEAD)
		{
			GetOwner()->Destroy();
		}
	}
	else if (_OtherObject->GetScript<CWallScript>())
	{
		Ptr<CSound> pSound = CResMgr::GetInst()->Load<CSound>(L"sound\\magnum\\Wall.wav", L"sound\\magnum\\Wall.wav");
		pSound->Play(1, 0.07f, true);
		GetOwner()->Destroy();
	}
	else if (_OtherObject->GetName() == L"Skull_Bullet")
	{
		GetOwner()->Destroy();
	}
}

