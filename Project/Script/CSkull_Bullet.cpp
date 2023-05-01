#include "pch.h"
#include "CSkull_Bullet.h"

#include <Engine/CAnimator2D.h>
#include <Engine/CCollider2D.h>

#include "CM_Bullet.h"
#include <Engine/CEventMgr.h>

CSkull_Bullet::CSkull_Bullet()
	:CScript((int)SCRIPT_TYPE::SKULL_BULLET)
	, m_RotateSpeed(20.f)
	, m_Speed(1.f)
	, m_fTime(0.f)
	, f_Dir(0)
{
}

CSkull_Bullet::~CSkull_Bullet()
{
}


void CSkull_Bullet::update()
{
	Animator2D()->Play(L"Skull_Bullet", true);

	m_fTime += DT;

	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	CGameObject* pObj = pCurScene->GetPlayer();
	//플레이어 위치

	Vec3 vPlayerPos = pCurScene->GetPlayerPos();
	Vec2 vColPos = pObj->Collider2D()->GetOffsetPos();

	// 총알 위치
	Vec3 vPos = Transform()->GetWorldPos();
	//Vec2 toDir = Vec2(vPlayerPos.x - vPos.x, vPlayerPos.y - vPos.y);
	Vec3 TargetDir = Vec3(vPlayerPos.x + vColPos.x - vPos.x, vPlayerPos.y + vColPos.x - vPos.y, 0);
	TargetDir.Normalize();

	//vPos; // 현재 위치 저장
	//vPlayerPos; // 타겟의 위치
	//Vec3 CrossProduct = m_Dir.Cross(TargetDir);

	//double Rad_to_deg = 57.2957951;
	//float _angle = atan2f(toDir.y, toDir.x);
	//float _z = Quaternion().CreateFromAxisAngle(Vec3(0, 0, 1.f), _angle).z;

	//Transform()->SetRelativeRotation(Vec3(0, 0, _z));
	//Transform()->SetRelativePos(vPos);

	Vec3 vTrans = Transform()->GetRelativePos();


	vTrans += DT * TargetDir * m_Speed;

	Transform()->SetRelativePos(vTrans);

	//Transform()->SetRelativePos(TargetDir);

	if (m_fTime > 3.f)
		GetOwner()->Destroy();
}


void CSkull_Bullet::OnCollisionEnter(CGameObject* _OtherObject)
{
	if (_OtherObject->GetName() == L"Player")
	{
		GetOwner()->Destroy();
	}
	if (_OtherObject->GetName() == L"P_Bullet" || _OtherObject->GetName() == L"Ice_Bullet")
	{
		Vec3 vTrans = Transform()->GetRelativePos();

		Ptr<CPrefab> pPrefab = CResMgr::GetInst()->FindRes<CPrefab>(L"prefab\\M_Bullet.pref");
		CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();




		Vec3 pTrans = Transform()->GetWorldPos();

		for (int i = 0; i < 8; ++i)
		{
			CGameObject* pObj = pPrefab->Instantiate();
			pObj->Transform()->SetRelativePos(Vec3(pTrans.x, pTrans.y, 0.f));

			CM_Bullet* pBullet = (CM_Bullet*)CScriptMgr::GetScript((UINT)SCRIPT_TYPE::M_BULLET);

			f_Dir += 0.785f;

			//if (f_Dir < XM_PI)
			//	f_Dir += 1.04f;
			//else
			//	f_Dir = -XM_PI;

			float cos = cosf(f_Dir);
			float sin = sinf(f_Dir);

			Vec3 vDir = Vec3(cos, sin, 0.f);
			vDir.Normalize();


			pBullet->SetDir(vDir);
			pBullet->SetSpeed(300.f);

			pObj->AddComponent(pBullet);


			tEventInfo evninfo;
			evninfo.eType = EVENT_TYPE::CREATE_OBJ;
			evninfo.lParam = (DWORD_PTR)pObj;
			evninfo.wParam = 6;

			CEventMgr::GetInst()->AddEvent(evninfo);


		}
		GetOwner()->Destroy();

	}
}
