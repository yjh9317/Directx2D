#include "pch.h"
#include "CMaiden_Bullet.h"

#include <Engine/CSceneMgr.h>
#include <Engine/CScene.h>

#include "CPlayerStateScript.h"
#include "CWallScript.h"

CMaiden_Bullet::CMaiden_Bullet()
	:CScript((int)SCRIPT_TYPE::MAIDEN_BULLET)
	, m_Dir{}
	, m_Speed(100.f)
	, m_fTime(0.f)
	, m_Wait(false)
	, m_WaitTime(0.f)
{
}

CMaiden_Bullet::~CMaiden_Bullet()
{
}


void CMaiden_Bullet::update()
{

	m_fTime += DT;
	if (m_fTime > 4.5f)
	{
		GetOwner()->Destroy();
	}


	Vec3 vTrans = Transform()->GetRelativePos();

	vTrans += DT * m_Dir * m_Speed;

	Transform()->SetRelativePos(vTrans);

	float m_Angle = atan2f(m_Dir.y, m_Dir.x);

	Vec3 vRot = Transform()->GetRelativeRotation();
	vRot.z = m_Angle;
	Transform()->SetRelativeRotation(vRot);
}

void CMaiden_Bullet::OnCollisionEnter(CGameObject* _OtherObject)
{
	if (_OtherObject->GetName() == L"Player")
	{
		CPlayerStateScript* pScript = _OtherObject->GetScript<CPlayerStateScript>();

		if (pScript->GetPlayerType() == PLAYER_TYPE::IDLE || pScript->GetPlayerType() == PLAYER_TYPE::WALK)
			GetOwner()->Destroy();
	}
	else if (_OtherObject->GetScript<CWallScript>())
	{

		CScene* pScene = CSceneMgr::GetInst()->GetCurScene();
		Vec3 vPlayerPos = pScene->GetPlayerPos();
		Vec3 vPos = Transform()->GetWorldPos();
		m_Speed = 300.f;

		m_Dir = vPlayerPos - vPos;
		m_Dir.Normalize();
	}
}

