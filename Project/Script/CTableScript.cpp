#include "pch.h"
#include "CTableScript.h"

#include <Engine/CCollider2D.h>
#include <Engine/CAnimator2D.h>
#include <Engine/CAnimation2D.h>

#include <Engine/CSceneMgr.h>
#include <Script/CMovementScript.h>

#include "CMon_Bullats.h"


CTableScript::CTableScript()
	: CScript((int)SCRIPT_TYPE::TABLESCRIPT)
	, m_bDamagedAble(false)
	, m_fTime(0.f)
	, m_type(TABLE_TYPE::Idle)
	, m_bTime(false)
	, m_iDir(0)
	, m_HP(5)
{
}

CTableScript::~CTableScript()
{
}


void CTableScript::update()
{
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	Vec3 vPlayerPos = pCurScene->GetPlayerPos();
	Vec3 vPos = Transform()->GetWorldPos();

	float PlayerXPos = vPlayerPos.x;
	float PlayerYPos = vPlayerPos.y;

	float XPos = vPos.x;
	float YPos = vPos.y+20.f;

	float distance = sqrtf(powf(PlayerXPos - XPos, 2) + powf(PlayerYPos-YPos,2));

	if (m_type == TABLE_TYPE::Idle)
	{
		Animator2D()->Play(L"Table_Idle", true);
		

		if ((distance <= 100.f && distance>=0.f) && KEY_TAP(KEY::E))
		{
			Check();
		}
	}
	else if(m_type == TABLE_TYPE::Stand)
	{
		// 1. 위  2. 아래  3.  오른  4.  왼
		switch (m_iDir)
		{
		case 1:
		{
			if (m_HP == 4)
			{
				Animator2D()->Play(L"Table_UpDamaged1", true);
			}
			else if (m_HP == 2)
			{
				Animator2D()->Play(L"Table_UpDamaged2", true);
			}
			else if (m_HP == 0)
			{
				m_type = TABLE_TYPE::Break;
				Animator2D()->Play(L"Table_UpBreak", false);
			}
			break;
		}
		case 2:
		{
			if (m_HP == 4)
			{
				Animator2D()->Play(L"Table_DownDamaged1", true);
			}
			else if (m_HP == 2)
			{
				Animator2D()->Play(L"Table_DownDamaged2", true);
			}
			else if (m_HP == 0)
			{
				m_type = TABLE_TYPE::Break;
				Animator2D()->Play(L"Table_DownBreak", false);
			}
			break;
		}
		case 3:
		{
			if (m_HP == 4)
			{
				Animator2D()->Play(L"Table_RightDamaged1", true);
			}
			else if (m_HP == 2)
			{
				Animator2D()->Play(L"Table_RightDamaged2", true);
			}
			else if (m_HP == 0)
			{
				m_type = TABLE_TYPE::Break;
				Animator2D()->Play(L"Table_RightBreak", false);
			}
			break;
		}
		case 4:
		{

			if (m_HP == 4)
			{
				Animator2D()->Play(L"Table_LeftDamaged1", true);
			}
			else if (m_HP == 2)
			{
				Animator2D()->Play(L"Table_LeftDamaged2", true);
			}
			else if (m_HP == 0)
			{
				m_type = TABLE_TYPE::Break;
				Animator2D()->Play(L"Table_LeftBreak", false);
			}
			break;
		}
		}
	}





	//if (m_bTime)
	//{
	//	m_fTime += DT;

	//	if (m_fTime > 0.5f)
	//	{
	//		m_type == TABLE_TYPE::Stand;
	//		m_bTime = false;
	//		m_fTime = 0.f;
	//	}
	//}
}

void CTableScript::OnCollisionEnter(CGameObject* _OtherObject)
{
	if (m_type == TABLE_TYPE::Idle)
	{
		
		if (_OtherObject->GetName() == L"Bullats")
		{
			CMon_Bullats* pBullatsScript = _OtherObject->GetScript<CMon_Bullats>();
			if (pBullatsScript->GetState() == MON_STATE::ATK)
				return;
		}

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

		// 벽의 위에 있을 떄
		if ((OtherDown <= Up) && (TransYpos < OtherPos.y) && OtherPrevDown > Up)
		{
			_OtherObject->Transform()->SetRelativePos(Vec3(OtherPos.x, TransYpos + ColHeight + OtherColScale.y / 2.f - OtherColOffset.y + 2.f + ColYPos, OtherPos.z));
		}
		// 벽의 아래에 있을 때
		else if ((OtherUp >= Down) && (TransYpos > OtherPos.y) && OtherPrevUp < Down)
		{
			_OtherObject->Transform()->SetRelativePos(Vec3(OtherPos.x, TransYpos - ColHeight - OtherColScale.y / 2.f - OtherColOffset.y - 2.f + ColYPos, OtherPos.z));
		}

		//벽의 오른쪽에 있을때
		if ((OtherLeft <= Right) && (TransXpos < OtherPos.x) && OtherPrevLeft > Right)
		{
			//if (OtherPos.y < TransYpos)
			_OtherObject->Transform()->SetRelativePos(Vec3(TransXpos + ColWidth + OtherColScale.x / 2.f + OtherColOffset.x + 2.f + ColXPos, OtherPos.y, OtherPos.z));
		}
		//벽의 왼쪽에 있을 때
		else if ((OtherRight >= Left) && (TransXpos > OtherPos.x) && OtherPrevRight < Left)
		{
			_OtherObject->Transform()->SetRelativePos(Vec3(TransXpos - ColWidth - OtherColScale.x / 2.f - OtherColOffset.x - 2.f + ColXPos, OtherPos.y, OtherPos.z));
		}
	}
	else if (m_type == TABLE_TYPE::Stand)
	{
		if (_OtherObject->GetName() == L"P_Bullet" || _OtherObject->GetName() == L"M_Bullet" || _OtherObject->GetName() == L"Maiden_Bullet" || _OtherObject->GetName() == L"Ice_Bullet" || _OtherObject->GetName() == L"Skull_Bullet")
		{
			--m_HP;
			_OtherObject->Destroy();
			return;
		}

		if (_OtherObject->GetScript<CMon_Bullats>())
		{
			m_HP = 0;
			return;
		}

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

		// 벽의 위에 있을 떄
		if ((OtherDown <= Up) && (TransYpos < OtherPos.y) && OtherPrevDown > Up)
		{
			_OtherObject->Transform()->SetRelativePos(Vec3(OtherPos.x, TransYpos + ColHeight + OtherColScale.y / 2.f - OtherColOffset.y + 2.f + ColYPos, OtherPos.z));
		}
		// 벽의 아래에 있을 때
		else if ((OtherUp >= Down) && (TransYpos > OtherPos.y) && OtherPrevUp < Down)
		{
			_OtherObject->Transform()->SetRelativePos(Vec3(OtherPos.x, TransYpos - ColHeight - OtherColScale.y / 2.f - OtherColOffset.y - 2.f + ColYPos, OtherPos.z));
		}

		//벽의 오른쪽에 있을때
		if ((OtherLeft <= Right) && (TransXpos < OtherPos.x) && OtherPrevLeft > Right)
		{
			//if (OtherPos.y < TransYpos)
			_OtherObject->Transform()->SetRelativePos(Vec3(TransXpos + ColWidth + OtherColScale.x / 2.f + OtherColOffset.x + 2.f + ColXPos, OtherPos.y, OtherPos.z));
		}
		//벽의 왼쪽에 있을 때
		else if ((OtherRight >= Left) && (TransXpos > OtherPos.x) && OtherPrevRight < Left)
		{
			_OtherObject->Transform()->SetRelativePos(Vec3(TransXpos - ColWidth - OtherColScale.x / 2.f - OtherColOffset.x - 2.f + ColXPos, OtherPos.y, OtherPos.z));
		}
	}
}



void CTableScript::Check()
{
	//Vec3 PrevPos = pScript->GetPrevPos();
	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	CGameObject* _OtherObject = pCurScene->GetPlayer();

	if (nullptr == _OtherObject)
		return;

	float TransXpos = Transform()->GetWorldPos().x;
	float TransYpos = Transform()->GetWorldPos().y;
	float ColWidth = Collider2D()->GetOffsetScale().x / 2.f;
	float ColHeight = Collider2D()->GetOffsetScale().y / 2.f;

	float Left = TransXpos - ColWidth;
	float Right = TransXpos + ColWidth;
	float Up = TransYpos + ColHeight;
	float Down = TransYpos - ColHeight;

	Vec3 OtherPos = _OtherObject->Transform()->GetWorldPos();
	Vec2 OtherColScale = _OtherObject->Collider2D()->GetOffsetScale();
	Vec2 OtherColOffset = _OtherObject->Collider2D()->GetOffsetPos();

	float OtherLeft = OtherPos.x - OtherColScale.x / 2.f + OtherColOffset.x;
	float OtherRight = OtherPos.x + OtherColScale.x / 2.f + OtherColOffset.x;
	float OtherUp = OtherPos.y + OtherColScale.y / 2.f + OtherColOffset.y;
	float OtherDown = OtherPos.y - OtherColScale.y / 2.f + OtherColOffset.y;

	//m_bTime = true;
	// 플레이어가 아래 있음.
	if (OtherUp <= Down)
	{
		Animator2D()->FindAnim(L"Table_Up")->Reset();
		Animator2D()->Play(L"Table_Up", false);
		m_iDir = 1;
	}
	// 플레이어가 위에 있음.
	else if (OtherDown >= Up)
	{
		Animator2D()->FindAnim(L"Table_Down")->Reset();
		Animator2D()->Play(L"Table_Down", false);
		m_iDir = 2;
	}
	// 플레이어가 왼쪽에 있음
	else if (OtherRight <= Left)
	{
		Animator2D()->FindAnim(L"Table_Right")->Reset();
		Animator2D()->Play(L"Table_Right", false);
		m_iDir = 3;
	}
	// 플레이어가 오른쪽에 있음
	else if (OtherLeft >= Right)
	{
		Animator2D()->FindAnim(L"Table_Left")->Reset();
		Animator2D()->Play(L"Table_Left", false);
		m_iDir = 4;
	}
	else
		return;
	m_type = TABLE_TYPE::Stand;

	Ptr<CSound> pSound = CResMgr::GetInst()->Load<CSound>(L"sound\\item\\TableFlip.wav", L"sound\\item\\TableFlip.wav");
	pSound->Play(1, 0.12f, true);
}