#include "pch.h"
#include "CM_Bullet.h"

#include "CWallScript.h"
#include "CDoorScript.h"
#include "CPlayerStateScript.h"

CM_Bullet::CM_Bullet()
	: CScript((int)SCRIPT_TYPE::M_BULLET)
	, m_Dir{}
	, m_Speed(0.f)
	, m_fTime(0.f)
	, m_Wait(false)
	, m_WaitTime(0.f)
{
}

CM_Bullet::~CM_Bullet()
{

}



void CM_Bullet::update()
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
}

void CM_Bullet::OnCollisionEnter(CGameObject* _OtherObject)
{
	if (_OtherObject->GetName() == L"Player")
	{
		CPlayerStateScript* pScript = _OtherObject->GetScript<CPlayerStateScript>();

		if(pScript->GetPlayerType() == PLAYER_TYPE::IDLE || pScript->GetPlayerType() == PLAYER_TYPE::WALK)
			GetOwner()->Destroy();
	}
	else if (_OtherObject->GetScript<CWallScript>() || _OtherObject->GetScript<CDoorScript>())
	{
		GetOwner()->Destroy();
	}
}
