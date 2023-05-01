#include "pch.h"
#include "CChestScript.h"

#include <Engine/CAnimator2D.h>
#include "CMovementScript.h"
#include <Engine/CCollider2D.h>
#include <Engine/CEventMgr.h>

#include "CIceGunScript.h"

void CChestScript::Check()
{
}

CChestScript::CChestScript()
	:CScript((int)SCRIPT_TYPE::CHESTSCRIPT)
	, m_type(CHEST_TYPE::Idle)
	, m_fTime(0.f)
	, m_bOpen(false)
	, m_Gun(false)
{
}

CChestScript::~CChestScript()
{
}


void CChestScript::update()
{
	switch (m_type)
	{
	case CHEST_TYPE::Idle:
		Animator2D()->Play(L"Chest_Idle", false);
		break;
	case CHEST_TYPE::Open:
		Animator2D()->Play(L"Chest_Open", false);
		break;
	case CHEST_TYPE::Opened:
		Animator2D()->Play(L"Chest_Opened", false);
		break;
	}

	if (m_type == CHEST_TYPE::Idle)
	{
		CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
		Vec3 vPlayerPos = pCurScene->GetPlayerPos();
		Vec3 vPos = Transform()->GetWorldPos();

		float PlayerXPos = vPlayerPos.x;
		float PlayerYPos = vPlayerPos.y;

		float XPos = vPos.x;
		float YPos = vPos.y + 20.f;

		float distance = sqrtf(powf(PlayerXPos - XPos, 2) + powf(PlayerYPos - YPos, 2));

		if ((distance <= 100.f && distance >= 0.f) && KEY_TAP(KEY::E) && m_type == CHEST_TYPE::Idle)
		{
			Ptr<CSound> pSound = CResMgr::GetInst()->Load<CSound>(L"sound\\item\\ChestOpen.wav", L"sound\\item\\ChestOpen.wav");
			pSound->Play(1, 0.2f, true);

			m_type = CHEST_TYPE::Open;
		}
	}
	else if (m_type == CHEST_TYPE::Open)
	{
		m_fTime += DT;

		if (!m_Gun && m_fTime > 0.3f)
		{
			m_Gun = true;

			Ptr<CPrefab> pPrefab = CResMgr::GetInst()->FindRes<CPrefab>(L"prefab\\IceGun.pref");
			CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();

			CGameObject* pObj = pPrefab->Instantiate();


			pObj->Transform()->SetRelativePos(Vec3(GetOwner()->Transform()->GetWorldPos().x +5.f, GetOwner()->Transform()->GetWorldPos().y, 100.f));
			CIceGunScript* pScript = new CIceGunScript;
			pObj->AddComponent(pScript);
			tEventInfo evninfo;

			evninfo.eType = EVENT_TYPE::CREATE_OBJ;
			evninfo.lParam = (DWORD_PTR)pObj;
			evninfo.wParam = 0;

			CEventMgr::GetInst()->AddEvent(evninfo);

		}

		if (m_fTime > 0.5f)
		{
			m_type = CHEST_TYPE::Opened;
			m_fTime = 0.f;
		}
	}
}

void CChestScript::OnCollisionEnter(CGameObject* _OtherObject)
{
	CMovementScript* pScript = _OtherObject->GetScript<CMovementScript>();

	if (nullptr == pScript)
		return;

	Vec3 PrevPos = pScript->GetPrevPos();

	float TransXpos = Transform()->GetWorldPos().x;
	float TransYpos = Transform()->GetWorldPos().y;

	float ColWidth = Collider2D()->GetOffsetScale().x / 2.f;
	float ColHeight = Collider2D()->GetOffsetScale().y / 2.f;

	float ColXPos = Collider2D()->GetOffsetPos().x;
	float ColYPos = Collider2D()->GetOffsetPos().y;

	float Left = TransXpos - ColWidth + ColXPos;
	float Right = TransXpos + ColWidth + ColXPos;
	float Up = TransYpos + ColHeight + ColYPos;
	float Down = TransYpos - ColHeight + ColYPos;

	Vec3 OtherPos = _OtherObject->Transform()->GetWorldPos();
	Vec2 OtherColOffset = _OtherObject->Collider2D()->GetOffsetPos();
	Vec2 OtherColScale = _OtherObject->Collider2D()->GetOffsetScale();

	float OtherLeft = OtherPos.x - OtherColScale.x / 2.f + OtherColOffset.x;
	float OtherRight = OtherPos.x + OtherColScale.x / 2.f + OtherColOffset.x;
	float OtherUp = OtherPos.y + OtherColScale.y / 2.f + OtherColOffset.y;
	float OtherDown = OtherPos.y - OtherColScale.y / 2.f + OtherColOffset.y;

	float OtherPrevUp = PrevPos.y + OtherColScale.y / 2.f + OtherColOffset.y;
	float OtherPrevDown = PrevPos.y - OtherColScale.y / 2.f + OtherColOffset.y;
	float OtherPrevLeft = PrevPos.x - OtherColScale.x / 2.f + OtherColOffset.x;
	float OtherPrevRight = PrevPos.x + OtherColScale.x / 2.f + OtherColOffset.x;

	// ���� ���� ���� ��
	if ((OtherDown <= Up)  && OtherPrevDown > Up)
	{
		_OtherObject->Transform()->SetRelativePos(Vec3(OtherPos.x, TransYpos + ColHeight + OtherColScale.y / 2.f - OtherColOffset.y + 1.f + ColYPos, OtherPos.z));
	}
	// ���� �Ʒ��� ���� ��
	else if ((OtherUp >= Down)  && OtherPrevUp < Down)
	{
		_OtherObject->Transform()->SetRelativePos(Vec3(OtherPos.x, TransYpos - ColHeight - OtherColScale.y / 2.f - OtherColOffset.y - 1.f + ColYPos, OtherPos.z));
	}

	//���� �����ʿ� ������
	if ((OtherLeft <= Right)  && OtherPrevLeft > Right) 
	{
		//if (OtherPos.y < TransYpos)
		_OtherObject->Transform()->SetRelativePos(Vec3(TransXpos + ColWidth + OtherColScale.x / 2.f + OtherColOffset.x + 1.f + ColXPos, OtherPos.y, OtherPos.z));
	}
	//���� ���ʿ� ���� ��
	else if ((OtherRight >= Left)  && OtherPrevRight < Left)
	{
		_OtherObject->Transform()->SetRelativePos(Vec3(TransXpos - ColWidth - OtherColScale.x / 2.f - OtherColOffset.x - 1.f + ColXPos, OtherPos.y, OtherPos.z));
	}
}