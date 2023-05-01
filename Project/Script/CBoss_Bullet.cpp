#include "pch.h"
#include "CBoss_Bullet.h"

#include "CWallScript.h"

#include "CPlayerStateScript.h"

CBoss_Bullet::CBoss_Bullet()
	:CScript((int)SCRIPT_TYPE::BOSS_BULLET)
	, m_Dir{}
	, m_Speed(100)
	, m_fTime(0.f)
{
}

CBoss_Bullet::~CBoss_Bullet()
{
}

void CBoss_Bullet::update()
{
	if (m_Wait)
	{
		if (m_fTime > m_WaitTime)
		{
			m_Wait = false;
			m_fTime = 0.f;
		}
		else
		{
			m_fTime += DT;
			return;
		}
	}

	m_fTime += DT;
	if (m_fTime > 2.5f)
	{
		GetOwner()->Destroy();
	}

	Vec3 vTrans = Transform()->GetRelativePos();

	vTrans += DT * m_Dir * m_Speed;

	Transform()->SetRelativePos(vTrans);

	//Vec3 vRot = Transform()->GetRelativeRotation();

	//vRot.z = m_Angle;

	//Transform()->SetRelativeRotation(vRot);
}

void CBoss_Bullet::OnCollisionEnter(CGameObject* _OtherObject)
{
	if (_OtherObject->GetName() == L"Player")
	{
		CPlayerStateScript* pScript = _OtherObject->GetScript<CPlayerStateScript>();

		if (pScript->GetPlayerType() == PLAYER_TYPE::IDLE || pScript->GetPlayerType() == PLAYER_TYPE::WALK)
			GetOwner()->Destroy();
	}
	else if (_OtherObject->GetScript<CWallScript>())
	{
		GetOwner()->Destroy();
	}
}