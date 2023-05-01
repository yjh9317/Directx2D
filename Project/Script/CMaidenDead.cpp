#include "pch.h"
#include "CMaidenDead.h"

#include <Engine/CAnimator2D.h>

CMaidenDead::CMaidenDead()
	:CScript((int)SCRIPT_TYPE::MAIDENDEAD)
	,m_fTime(0.f)
{
}

CMaidenDead::~CMaidenDead()
{
}


void CMaidenDead::update()
{
	m_fTime += DT;

	Animator2D()->Play(L"Dead", false);

	if (m_fTime > 0.7f)
		GetOwner()->Destroy();
}

