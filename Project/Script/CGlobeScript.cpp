#include "pch.h"
#include "CGlobeScript.h"

#include <Engine/CAnimator2D.h>
#include <Engine/CAnimation2D.h>

#include "CMonsterScript.h"

CGlobeScript::CGlobeScript()
	:CScript((int)SCRIPT_TYPE::GLOBESCRIPT)
	,m_b(false)
{
}

CGlobeScript::~CGlobeScript()
{
}


void CGlobeScript::update()
{
	if (!m_b)
		Animator2D()->Play(L"Idle", false);
	else
		Animator2D()->Play(L"Break", false);
}

void CGlobeScript::OnCollisionEnter(CGameObject* _OtherObj)
{
	if (_OtherObj->GetName() == L"Player" || _OtherObj->GetScript<CMonsterScript>() || _OtherObj->GetName() == L"P_Bullet" || _OtherObj->GetName() == L"M_Bullet" && !m_b)
	{
		Ptr<CSound> pSound = CResMgr::GetInst()->Load<CSound>(L"sound\\item\\GlassBreak.wav", L"sound\\item\\GlassBreak.wav");
		pSound->Play(1, 0.12f, true);
		Animator2D()->FindAnim(L"Break")->Reset();
		m_b = true;
	}
}

