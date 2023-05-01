#include "pch.h"
#include "CMissileScript.h"

#include <Engine/CTimeMgr.h>
#include <Engine/CTransform.h>
#include <Engine/CEventMgr.h>
#include <Engine/CAnimator2D.h>

#include "CPlayerStateScript.h"
#include "CWallScript.h"
#include "CDoorScript.h"


CMissileScript::CMissileScript()
	: CScript((int)SCRIPT_TYPE::MISSILESCRIPT)
	, m_fSpeed(600.f)
	, m_fAccTime(0.f)
	, m_fExTime(0.f)
	, m_particle(false)
{
}

CMissileScript::~CMissileScript()
{
}



void CMissileScript::update()
{
	if (!m_Explosion)
	{
		Animator2D()->Play(L"Idle", true);

		Vec3 vPos = Transform()->GetRelativePos();

		vPos += DT * m_fSpeed * m_Dir;

		Transform()->SetRelativePos(vPos);



		m_fAccTime += DT;

		if (m_fAccTime >= 3.f)
		{
			GetOwner()->Destroy();
		}


		float m_Angle = atan2f(m_Dir.y, m_Dir.x);

		Vec3 vRot = Transform()->GetRelativeRotation();
		vRot.z = m_Angle;
		Transform()->SetRelativeRotation(vRot);
	}
	else if(m_Explosion)
	{
		m_fExTime += DT;

		//if (!m_particle && m_fExTime > 0.7f)
		//{
		//	Ptr<CPrefab> pPrefab = CResMgr::GetInst()->FindRes<CPrefab>(L"prefab\\particle.pref");
		//	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();

		//	Vec3 pTrans = Transform()->GetRelativePos();
		//	CGameObject* pObj = pPrefab->Instantiate();
		//	pObj->Transform()->SetRelativePos(Vec3(pTrans.x, pTrans.y, 0.f));

		//	tEventInfo evninfo;
		//	evninfo.eType = EVENT_TYPE::CREATE_OBJ;
		//	evninfo.lParam = (DWORD_PTR)pObj;
		//	evninfo.wParam = 0;

		//	CEventMgr::GetInst()->AddEvent(evninfo);
		//}

		if (m_fExTime >= 0.8f)
		{
			GetOwner()->Destroy();
		}

		Animator2D()->Play(L"Bomb", false);
	}

}

void CMissileScript::OnCollisionEnter(CGameObject* _pOtherObj)
{
	if (!m_Explosion)
	{
		if (_pOtherObj->GetName() == L"Player")
		{
			CPlayerStateScript* pScript = _pOtherObj->GetScript<CPlayerStateScript>();

			if (pScript->GetPlayerType() == PLAYER_TYPE::IDLE || pScript->GetPlayerType() == PLAYER_TYPE::WALK)
				m_Explosion = true;
		}
		else if (_pOtherObj->GetScript<CWallScript>() || _pOtherObj->GetScript<CDoorScript>())
		{
			m_Explosion = true;
		}
	}
}

