#include "pch.h"
#include "CHeartScript.h"

#include <Engine/CSceneMgr.h>
#include <Engine/CScene.h>
#include <Engine/CAnimator2D.h>

#include <Engine/CDevice.h>
#include <Engine/CRenderMgr.h>


CHeartScript::CHeartScript()
	:CScript((int)SCRIPT_TYPE::HEARTSCRIPT)
	, m_Num(0)
	, m_HP(2)
{
}

CHeartScript::~CHeartScript()
{
}


void CHeartScript::update()
{
	if (2 == m_HP)
	{
		Animator2D()->Play(L"Heart", false);
	}
	else if (1 == m_HP)
	{
		Animator2D()->Play(L"Heart_half", false);
	}


	
	//Vec3 vCamPos = CRenderMgr::GetInst()->GetMainCamPos();
	//float vCamWidth = CRenderMgr::GetInst()->GetMainCamWidth();
	//float vCamHeight = CRenderMgr::GetInst()->GetMainCamHeight();
	//vCamPos.x -= vCamWidth / 2.f;
	//vCamPos.y += vCamHeight / 2.f - 50.f;

	//vCamPos.x += 50 + (50 * m_Num);
	//Transform()->SetRelativePos(vCamPos);
}

