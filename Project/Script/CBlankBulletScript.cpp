#include "pch.h"
#include "CBlankBulletScript.h"

#include <Engine/CSceneMgr.h>
#include <Engine/CScene.h>
#include <Engine/CAnimator2D.h>

#include <Engine/CRenderMgr.h>

CBlankBulletScript::CBlankBulletScript()
	:CScript((int)SCRIPT_TYPE::BLANKBULLETSCRIPT)
{
}

CBlankBulletScript::~CBlankBulletScript()
{
}


void CBlankBulletScript::update()
{
	Vec3 vCamPos = CRenderMgr::GetInst()->GetMainCamPos();
	float vCamWidth = CRenderMgr::GetInst()->GetMainCamWidth();
	float vCamHeight = CRenderMgr::GetInst()->GetMainCamHeight();
	vCamPos.x -= vCamWidth / 2.f;
	vCamPos.y += vCamHeight / 2.f - 100.f;

	vCamPos.x += 50 + (50 * m_Num);
	Transform()->SetRelativePos(vCamPos);
}