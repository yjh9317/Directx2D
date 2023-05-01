#include "pch.h"
#include "CBossScript.h"

#include <Engine/CAnimator2D.h>
#include <Engine/CAnimation2D.h>
#include <Engine/CEventMgr.h>

#include "CScriptMgr.h"
#include "CM_Bullet.h"
#include "CBoss_Bullet.h"
#include "CSkull_Bullet.h"
#include "CBarrier_Bullet.h"

#include <time.h>

#define W 32
#define R 32
#define M1 3
#define M2 24
#define M3 10

#define MAT0POS(t,v) (v^(v>>t))
#define MAT0NEG(t,v) (v^(v<<(-(t))))
#define Identity(v) (v)

#define V0            STATE[state_i                   ]
#define VM1           STATE[(state_i+M1) & 0x0000001fU]
#define VM2           STATE[(state_i+M2) & 0x0000001fU]
#define VM3           STATE[(state_i+M3) & 0x0000001fU]
#define VRm1          STATE[(state_i+31) & 0x0000001fU]
#define newV0         STATE[(state_i+31) & 0x0000001fU]
#define newV1         STATE[state_i                   ]

#define FACT 2.32830643653869628906e-10

static unsigned int state_i = 0;
static unsigned int STATE[R];
static unsigned int z0, z1, z2;

void InitWELLRNG1024a(unsigned int* init) {
	int j;
	state_i = 0;
	for (j = 0; j < R; j++)
		STATE[j] = init[j];
}

double WELLRNG1024a(void) {
	z0 = VRm1;
	z1 = Identity(V0) ^ MAT0POS(8, VM1);
	z2 = MAT0NEG(-19, VM2) ^ MAT0NEG(-14, VM3);
	newV1 = z1 ^ z2;
	newV0 = MAT0NEG(-11, z0) ^ MAT0NEG(-7, z1) ^ MAT0NEG(-13, z2);
	state_i = (state_i + 31) & 0x0000001fU;
	return ((double)STATE[state_i] * FACT);
}





CBossScript::CBossScript()
	:CMonsterScript((int)SCRIPT_TYPE::BOSSSCRIPT)
	, m_fBossBulletTime(0.f)
	, i_BossBulletCnt(0)
	, m_Init(false)
	, f_Dir(0)
	, b_Dir(false)
	, m_Summon(false)
	, m_iOrder(0)
{
	SetState(MON_STATE::START);
	m_HP = 300;
	m_DetectRange = 1000.f;
	m_AttackRange = 500.f;
	m_fSpeed = 70.f;

}

CBossScript::~CBossScript()
{
}


void CBossScript::AnimStart()
{
	m_fTime += DT;

	if (!m_Summon && m_fTime>2.4f)
	{
		m_Summon = true;

		Ptr<CPrefab> pPrefab = CResMgr::GetInst()->FindRes<CPrefab>(L"prefab\\HeadGun_Start.pref");
		CGameObject* pObj = pPrefab->Instantiate();
		m_HeadGun = pObj;
		pObj->Animator2D()->Play(L"Start", false);
		Vec3 pTrans = Transform()->GetRelativePos();

		pObj->Transform()->SetRelativePos(Vec3(pTrans.x, pTrans.y + 50.f, 400.f));

		tEventInfo evninfo;
		evninfo.eType = EVENT_TYPE::CREATE_OBJ;
		evninfo.lParam = (DWORD_PTR)pObj;
		evninfo.wParam = 5;

		CEventMgr::GetInst()->AddEvent(evninfo);	
	}

	if (m_fTime > 4.8f)
		m_HeadGun->Destroy();

	if (m_fTime <= 2.f)
	{
		Animator2D()->Play(L"Idle", true);
	}
	else if (m_fTime <= 2.8f && m_fTime >= 2.f)
	{
		Animator2D()->Play(L"HeadOpen", false);
	}
	else if (m_fTime >= 2.8f && m_fTime < 4.6f)
	{
		Animator2D()->Play(L"HeadOpening", true);
	}
	else if (m_fTime >= 4.6f && m_fTime < 5.1f)
	{
		Animator2D()->Play(L"HeadClose", false);
	}
	else if (m_fTime >= 5.1f && m_fTime <6.1f)
	{
		Animator2D()->Play(L"Idle", true);
	}
	else if (m_fTime >= 6.1f)
	{
		m_Atkable = false;
		Animator2D()->FindAnim(L"HeadOpen")->Reset();
		Animator2D()->FindAnim(L"HeadClose")->Reset();
		m_fTime = 0.f;
		m_Summon = false;
		ChangeState(MON_STATE::IDLE);
	}

}

void CBossScript::Idle()
{
	if (!m_Init)
	{
		srand((unsigned)time(NULL));
		unsigned int init[32];
		for (int i = 0; i < 32; i++) {
			init[i] = rand() << 16 | rand();
			// WELL Random 을 초기화 하기 위해, C 표준 rand() 함수를 이용하여 init 값을 생성합니다
		}
		InitWELLRNG1024a(init); // WELL Random 초기화
		m_Init = true;
	}


	Animator2D()->Play(L"Idle",true);

	ChangeState(MON_STATE::TRACE);
}

void CBossScript::Trace()
{
	WELLRNG1024a();

	//int rnd = (int)((double)WELLRNG1024a() * (max - min + 1)) + min; // min~max
	int random_cnt = (int)((double)WELLRNG1024a() * (14));

	Animator2D()->Play(L"Idle", true);

	if (!m_Atkable)
	{
		if (m_fAtkTime >= 4.f)
		{
			m_Atkable = true;
			m_fAtkTime = 0.f;
		}
		m_fAtkTime += DT;
	}

	if (m_fDistance < m_AttackRange && m_Atkable)
	{
		m_BehaviorAngle = m_Angle;
		m_fBossBulletTime = 0.f;

		++m_iOrder;

		if (m_iOrder <= 2)
		{
			ChangeState(MON_STATE::ATK);
		}
		else if (m_iOrder <= 3)
		{
			ChangeState(MON_STATE::SKILL1);
		}
		else if (m_iOrder <= 4)
		{
			ChangeState(MON_STATE::SKILL2);
		}
		else if (m_iOrder <= 5)
		{
			ChangeState(MON_STATE::SKILL3);
		}
		else if (m_iOrder <= 6)
		{
			ChangeState(MON_STATE::SKILL4);
		}
		else if (m_iOrder <= 7)
		{
			ChangeState(MON_STATE::SKILL5);
		}
		else
		{
			m_iOrder = 0;
		}
	}

	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	Vec2 vPlayerPos = Vec2(pCurScene->GetPlayerPos().x, pCurScene->GetPlayerPos().y);
	Vec2 vMonPos = Vec2(Transform()->GetWorldPos().x, Transform()->GetWorldPos().y);
	// 몬스터에서 플레이어를 향한 방향을 알아낸다.
	Vec2 vDir = vPlayerPos - vMonPos;
	vDir.Normalize();

	// 몬스터를 플레이어를 향해서 이동시킨다.
	vMonPos += vDir * m_fSpeed * DT;
	Transform()->SetRelativePos(vMonPos.x, vMonPos.y, 800.f);
	
}

void CBossScript::Attack()
{
	if (!m_Sound)
	{
		m_Sound = true;
		Ptr<CSound> pSound = CResMgr::GetInst()->Load<CSound>(L"sound\\highpriest\\Atk.wav", L"sound\\highpriest\\Atk.wav");
		pSound->Play(1, 0.07f, true);
	}
	if (m_fTime >= 0.1f && m_Atkable)
	{
		Shoot_Atk();
	}

	m_fTime += DT;
	if ((m_BehaviorAngle <= 181.f && m_BehaviorAngle >= 90.f) || (m_BehaviorAngle <= -90.f && m_BehaviorAngle >= -181.f))
	{
		if (m_fTime <= 1.2f && m_fTime >= 0.f)
		{
			Animator2D()->Play(L"Left_Atk", false);
		}
		else
		{
			Animator2D()->FindAnim(L"Left_Atk")->Reset();
			i_BossBulletCnt = 0;
			m_fBossBulletTime = 0.f;
			m_BehaviorAngle = 0.f;
			m_fTime = 0.f;
			m_Atkable = false;
			m_fAtkTime = 0.f;
			m_Sound = false;
			ChangeState(MON_STATE::IDLE);
		}
	}
	else if ((m_BehaviorAngle <= 90.f && m_BehaviorAngle >= 0.f) || (m_BehaviorAngle <= 0.f && m_BehaviorAngle >= -90.f))
	{
		if (m_fTime <= 1.2f && m_fTime >= 0.f)
		{
			Animator2D()->Play(L"Right_Atk", false);
		}
		else
		{
			Animator2D()->FindAnim(L"Right_Atk")->Reset();
			i_BossBulletCnt = 0;
			m_fBossBulletTime = 0.f;
			m_BehaviorAngle = 0.f;
			m_fTime = 0.f;
			m_Atkable = false;
			m_fAtkTime = 0.f;
			m_Sound = false;
			ChangeState(MON_STATE::IDLE);
		}
	}
}

void CBossScript::Skill_1()
{
	if (!m_Sound)
	{
		m_Sound = true;
		Ptr<CSound> pSound = CResMgr::GetInst()->Load<CSound>(L"sound\\highpriest\\Atk.wav", L"sound\\highpriest\\Atk.wav");
		pSound->Play(1, 0.07f, true);
	}
	m_fTime += DT;

	if (m_fTime >= 0.1f && m_Atkable)
	{
		Shoot_Skill1();
	}

	Animator2D()->Play(L"Both_Atk", false);

	if (m_fTime > 0.95f)
	{
		m_Atkable = false;
		m_fAtkTime = 1.3f;
		Animator2D()->FindAnim(L"Both_Atk")->Reset();
		m_fTime = 0.f;
		i_BossBulletCnt = 0.f;
		m_Sound = false;
		ChangeState(MON_STATE::IDLE);
	}
}

void CBossScript::Skill_2()
{
	if (!m_Sound)
	{
		m_Sound = true;
		Ptr<CSound> pSound = CResMgr::GetInst()->Load<CSound>(L"sound\\highpriest\\Machinegun.wav", L"sound\\highpriest\\Machinegun.wav");
		pSound->Play(1, 0.07f, true);
	}
	m_fTime += DT;

	if (m_fTime >= 0.65f)
	{
		Shoot_Skill2();
	}

	if (m_fTime <= 0.65f && m_fTime >= 0.f)
	{
		Animator2D()->Play(L"Atk_Prev", false);
	}
	else if (m_fTime >= 0.65f)
		Animator2D()->Play(L"MachineGun", true);

	if (m_fTime >= 5.f)
	{
		m_fBossBulletTime = 0.f;
		m_Atkable = false;
		Animator2D()->FindAnim(L"Atk_Prev")->Reset();
		m_fTime = 0.f;
		m_Sound = false;
		ChangeState(MON_STATE::IDLE);
	}
}

void CBossScript::Skill_3()
{
	m_fTime += DT;

	if (m_fTime > 0.65f)
	{
		Shoot_Skill3();
	}

	if (m_fTime <= 0.65f && m_fTime >= 0.f)
	{
		Animator2D()->Play(L"Atk_Prev", false);
	}
	else if (m_fTime >= 0.65f)
	{
		Animator2D()->Play(L"Barrier", true);
	}

	if (m_fTime >= 3.f)
	{
		m_fBossBulletTime = 0.f;
		m_Atkable = false;
		Animator2D()->FindAnim(L"Atk_Prev")->Reset();
		m_fTime = 0.f;
		ChangeState(MON_STATE::IDLE);
	}
}

void CBossScript::Skill_4()
{
	m_fTime += DT;

	if (m_fTime >= 0.7f)
	{
		Shoot_Skill4();
	}

	if (m_fTime <= 0.8f && m_fTime >= 0.f)
	{
		Animator2D()->Play(L"HeadOpen", false);
	}
	else if (m_fTime >= 0.8f && m_fTime<3.f)
	{
		Animator2D()->Play(L"HeadOpening", true);
	}
	else if (m_fTime >= 3.f)
	{
		Animator2D()->Play(L"HeadClose", false);
	}

	if (m_fTime >= 3.5f) {
		m_Atkable = false;
		Animator2D()->FindAnim(L"HeadOpen")->Reset();
		Animator2D()->FindAnim(L"HeadClose")->Reset();
		m_fTime = 0.f;
		m_Summon = false;
		ChangeState(MON_STATE::IDLE);
	}
}


void CBossScript::Skill_5()
{
	if (!m_Sound)
	{
		m_Sound = true;
		Ptr<CSound> pSound = CResMgr::GetInst()->Load<CSound>(L"sound\\highpriest\\Whips.wav", L"sound\\highpriest\\Whips.wav");
		pSound->Play(1, 0.07f, true);
	}

	m_fTime += DT;

	if (m_fTime >= 0.65f)
	{
		Shoot_Skill5();
	}

	if (m_fTime <= 0.65f && m_fTime >= 0.f)
	{
		Animator2D()->Play(L"Atk_Prev", false);
	}
	else if (m_fTime >= 0.65f)
		Animator2D()->Play(L"MachineGun", true);

	if (m_fTime >= 4.f)
	{
		m_fBossBulletTime = 0.f;
		m_Atkable = false;
		Animator2D()->FindAnim(L"Atk_Prev")->Reset();
		m_fTime = 0.f;
		m_Sound = false;
		ChangeState(MON_STATE::IDLE);
	}
}



void CBossScript::Dead()
{
	

	Animator2D()->Play(L"Dead", false);

	//m_fTime += DT;
	//if (m_fTime > 1.f)
	//	GetOwner()->Destroy();
}

void CBossScript::OnCollisionEnter(CGameObject* _OtherObject)
{
	if (_OtherObject->GetName() == L"P_Bullet" && !(GetState() == MON_STATE::DEAD))
	{
		m_HP -= 15;

		if (m_HP <= 0)
		{
			m_fTime = 0.f;
			ChangeState(MON_STATE::DEAD);
		}
	}

	if (_OtherObject->GetName() == L"Ice_Bullet" && !(GetState() == MON_STATE::DEAD))
	{
		m_HP -= 20;

		if (m_HP <= 0)
		{
			m_fTime = 0.f;
			ChangeState(MON_STATE::DEAD);
		}
	}
}

void CBossScript::Shoot_Atk()
{
	m_fBossBulletTime += DT;

	if ((m_BehaviorAngle <= 181.f && m_BehaviorAngle >= 90.f) || (m_BehaviorAngle <= -90.f && m_BehaviorAngle >= -181.f))
	{
		if (m_fBossBulletTime >= 0.033f && i_BossBulletCnt < 10)
		{
			m_fBossBulletTime = 0.f;
			Ptr<CPrefab> pPrefab = CResMgr::GetInst()->FindRes<CPrefab>(L"prefab\\M_Bullet.pref");
			CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
			Vec3 pPlayerPos = pCurScene->GetPlayerPos();

			CBoss_Bullet* pBullet = (CBoss_Bullet*)CScriptMgr::GetScript((UINT)SCRIPT_TYPE::BOSS_BULLET);

			Vec3 pTrans = Transform()->GetWorldPos();
			CGameObject* pObj = pPrefab->Instantiate();

			Vec3 vDir = Vec3(pPlayerPos.x - pTrans.x, pPlayerPos.y - pTrans.y, 0.f);
			vDir.Normalize();


			pObj->Transform()->SetRelativeScale(Vec3(40.f, 40.f, 10.f));
			pBullet->SetDir(vDir);
			pBullet->SetSpeed(300.f);
			pBullet->WaitCheck(true);
			pBullet->WaitTime(1.f - i_BossBulletCnt * 0.1f);

			if (i_BossBulletCnt < 5)
				pObj->Transform()->SetRelativePos(Vec3(-50.f + pTrans.x - 15.f * i_BossBulletCnt, -60.f + pTrans.y + 20.f * i_BossBulletCnt, 0.f));
			else
				pObj->Transform()->SetRelativePos(Vec3(-50.f + pTrans.x - 15.f * (10 - i_BossBulletCnt), -60.f + pTrans.y + 20.f * i_BossBulletCnt, 0.f));

			pObj->AddComponent(pBullet);

			//pObj->Transform()->SetRelativeRotation(Vec3(0,0,-180.f));

			++i_BossBulletCnt;
			tEventInfo evninfo;
			evninfo.eType = EVENT_TYPE::CREATE_OBJ;
			evninfo.lParam = (DWORD_PTR)pObj;
			evninfo.wParam = 6;

			CEventMgr::GetInst()->AddEvent(evninfo);
		}
	}
	else if ((m_BehaviorAngle <= 90.f && m_BehaviorAngle >= 0.f) || (m_BehaviorAngle <= 0.f && m_BehaviorAngle >= -90.f))
	{
		if (m_fBossBulletTime >= 0.033f && i_BossBulletCnt < 10)
		{
			m_fBossBulletTime = 0.f;
			Ptr<CPrefab> pPrefab = CResMgr::GetInst()->FindRes<CPrefab>(L"prefab\\M_Bullet.pref");
			CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
			Vec3 pPlayerPos = pCurScene->GetPlayerPos();

			CBoss_Bullet* pBullet = (CBoss_Bullet*)CScriptMgr::GetScript((UINT)SCRIPT_TYPE::BOSS_BULLET);

			Vec3 pTrans = Transform()->GetWorldPos();
			CGameObject* pObj = pPrefab->Instantiate();

			Vec3 vDir = Vec3(pPlayerPos.x - pTrans.x, pPlayerPos.y - pTrans.y, 0.f);
			vDir.Normalize();


			pObj->Transform()->SetRelativeScale(Vec3(40.f, 40.f, 10.f));
			pBullet->SetDir(vDir);
			pBullet->SetSpeed(300.f);
			pBullet->WaitCheck(true);
			pBullet->WaitTime(1.f - i_BossBulletCnt * 0.1f);

			if (i_BossBulletCnt < 5)
				pObj->Transform()->SetRelativePos(Vec3(70.f + pTrans.x + 15.f * i_BossBulletCnt, -60.f + pTrans.y + 20.f * i_BossBulletCnt, 0.f));
			else
				pObj->Transform()->SetRelativePos(Vec3(70.f + pTrans.x + 15.f * (10 - i_BossBulletCnt), -60.f + pTrans.y + 20.f * i_BossBulletCnt, 0.f));

			pObj->AddComponent(pBullet);

			++i_BossBulletCnt;
			tEventInfo evninfo;
			evninfo.eType = EVENT_TYPE::CREATE_OBJ;
			evninfo.lParam = (DWORD_PTR)pObj;
			evninfo.wParam = 6;

			CEventMgr::GetInst()->AddEvent(evninfo);
		}
	}
}

void CBossScript::Shoot_Skill1()
{
	m_fBossBulletTime += DT;

	if (m_fBossBulletTime >= 0.033f && i_BossBulletCnt < 10)
	{
		m_fBossBulletTime = 0.f;
		Ptr<CPrefab> pPrefab = CResMgr::GetInst()->FindRes<CPrefab>(L"prefab\\M_Bullet.pref");
		CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
		Vec3 pPlayerPos = pCurScene->GetPlayerPos();

		CBoss_Bullet* pBullet = (CBoss_Bullet*)CScriptMgr::GetScript((UINT)SCRIPT_TYPE::BOSS_BULLET);

		Vec3 pTrans = Transform()->GetWorldPos();
		CGameObject* pObj = pPrefab->Instantiate();

		Vec3 vDir = Vec3(pPlayerPos.x - pTrans.x, pPlayerPos.y - pTrans.y, 0.f);
		vDir.Normalize();


		pObj->Transform()->SetRelativeScale(Vec3(40.f, 40.f, 10.f));
		pBullet->SetDir(vDir);
		pBullet->SetSpeed(300.f);
		pBullet->WaitCheck(true);
		pBullet->WaitTime(1.f - i_BossBulletCnt * 0.1f);


		//float m_Angle = atan2f(Dir.y, Dir.x);

		//const double Rad_to_deg = 57.2957951;
		//float Angle = m_Angle * (float)Rad_to_deg;

		////float m_Angle = atan2f(-1, 0);
		//Vec3 vRot = pObj->Transform()->GetRelativeRotation();
		//pObj->Transform()->SetRelativeRotation(Vec3(0, 0, 180));
		

		if (i_BossBulletCnt < 5)
			pObj->Transform()->SetRelativePos(Vec3(-50.f + pTrans.x - 15.f * i_BossBulletCnt, -60.f + pTrans.y + 20.f * i_BossBulletCnt, 0.f));
		else
			pObj->Transform()->SetRelativePos(Vec3(-50.f + pTrans.x - 15.f * (10 - i_BossBulletCnt), -60.f + pTrans.y + 20.f * i_BossBulletCnt, 0.f));

		pObj->AddComponent(pBullet);



		tEventInfo evninfo;
		evninfo.eType = EVENT_TYPE::CREATE_OBJ;
		evninfo.lParam = (DWORD_PTR)pObj;
		evninfo.wParam = 6;

		CEventMgr::GetInst()->AddEvent(evninfo);
		m_fBossBulletTime = 0.f;

		pBullet = (CBoss_Bullet*)CScriptMgr::GetScript((UINT)SCRIPT_TYPE::BOSS_BULLET);
		pObj = pPrefab->Instantiate();

		//Vec3 vDir = Vec3(pPlayerPos.x - pTrans.x, pPlayerPos.y - pTrans.y, 0.f);
		//vDir.Normalize();


		pObj->Transform()->SetRelativeScale(Vec3(40.f, 40.f, 10.f));
		pBullet->SetDir(vDir);
		pBullet->SetSpeed(300.f);
		pBullet->WaitCheck(true);
		pBullet->WaitTime(1.f - i_BossBulletCnt * 0.1f);

		if (i_BossBulletCnt < 5)
			pObj->Transform()->SetRelativePos(Vec3(70.f + pTrans.x + 15.f * i_BossBulletCnt, -60.f + pTrans.y + 20.f * i_BossBulletCnt, 0.f));
		else
			pObj->Transform()->SetRelativePos(Vec3(70.f + pTrans.x + 15.f * (10 - i_BossBulletCnt), -60.f + pTrans.y + 20.f * i_BossBulletCnt, 0.f));

		pObj->AddComponent(pBullet);


		evninfo.eType = EVENT_TYPE::CREATE_OBJ;
		evninfo.lParam = (DWORD_PTR)pObj;
		evninfo.wParam = 6;

		CEventMgr::GetInst()->AddEvent(evninfo);

		++i_BossBulletCnt;
	}
}


void CBossScript::Shoot_Skill2()
{
	m_fBossBulletTime += DT;
	WELLRNG1024a();

	//int rnd = (int)((double)WELLRNG1024a() * (max - min + 1)) + min; // min~max

	int rnd = (int)((double)WELLRNG1024a() * 100);
	int rnd2 = (int)((double)WELLRNG1024a() * 200) - 100;

	int rnd3 = (int)((double)WELLRNG1024a() * 100);
	int rnd4 = (int)((double)WELLRNG1024a() * 200) - 100;

	if (m_fBossBulletTime >= 0.04f)
	{
		// 오른쪽
		Ptr<CPrefab> pPrefab = CResMgr::GetInst()->FindRes<CPrefab>(L"prefab\\M_Bullet.pref");
		CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();


		CM_Bullet* pBullet = (CM_Bullet*)CScriptMgr::GetScript((UINT)SCRIPT_TYPE::M_BULLET);

		Vec3 pTrans = Transform()->GetWorldPos();
		CGameObject* pObj = pPrefab->Instantiate();
		pObj->Transform()->SetRelativePos(Vec3(pTrans.x+120.f, pTrans.y +40.f , 0.f));

		float _frandom = (float)rnd / 100.f;
		float _frandom2 = (float)rnd2 / 100.f;

		Vec3 vDir = Vec3(_frandom - 0.3f, _frandom2, 0.f);
		vDir.Normalize();


		pBullet->SetDir(vDir);
		pBullet->SetSpeed(300.f);

		pObj->AddComponent(pBullet);

		tEventInfo evninfo;
		evninfo.eType = EVENT_TYPE::CREATE_OBJ;
		evninfo.lParam = (DWORD_PTR)pObj;
		evninfo.wParam = 6;

		CEventMgr::GetInst()->AddEvent(evninfo);


		// 왼쪽

		pBullet = (CM_Bullet*)CScriptMgr::GetScript((UINT)SCRIPT_TYPE::M_BULLET);

		pObj = pPrefab->Instantiate();


		pObj->Transform()->SetRelativePos(Vec3(pTrans.x - 120.f, pTrans.y + 40.f, 0.f));

		_frandom = (float)rnd3 / 100.f;
		_frandom2 = (float)rnd4 / 100.f;

		vDir = Vec3(-_frandom +0.3f, _frandom2, 0.f);
		vDir.Normalize();


		pBullet->SetDir(vDir);
		pBullet->SetSpeed(300.f);

		pObj->AddComponent(pBullet);

		evninfo.eType = EVENT_TYPE::CREATE_OBJ;
		evninfo.lParam = (DWORD_PTR)pObj;
		evninfo.wParam = 6;

		CEventMgr::GetInst()->AddEvent(evninfo);
		m_fBossBulletTime = 0.f;
	}
}

void CBossScript::Shoot_Skill3()
{
	m_fBossBulletTime += DT;

	int rnd = (int)((double)WELLRNG1024a() * 200) - 100;
	int rnd2 = (int)((double)WELLRNG1024a() * 200) - 100;

	if (m_fBossBulletTime >= 0.5f)
	{
		// 오른쪽
		Ptr<CPrefab> pPrefab = CResMgr::GetInst()->FindRes<CPrefab>(L"prefab\\Skull_Bullet.pref");
		CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();


		CSkull_Bullet* pBullet = (CSkull_Bullet*)CScriptMgr::GetScript((UINT)SCRIPT_TYPE::SKULL_BULLET);

		Vec3 pTrans = Transform()->GetWorldPos();
		CGameObject* pObj = pPrefab->Instantiate();
		pObj->Transform()->SetRelativePos(Vec3(pTrans.x, pTrans.y, 0.f));

		float _frandom = (float)rnd / 100.f;
		float _frandom2 = (float)rnd2 / 100.f;

		Vec3 vDir = Vec3(_frandom, _frandom2, 0.f);
		vDir.Normalize();


		pBullet->SetDir(vDir);
		pBullet->SetSpeed(170.f);

		pObj->AddComponent(pBullet);

		tEventInfo evninfo;
		evninfo.eType = EVENT_TYPE::CREATE_OBJ;
		evninfo.lParam = (DWORD_PTR)pObj;
		evninfo.wParam = 6;

		CEventMgr::GetInst()->AddEvent(evninfo);
		m_fBossBulletTime = 0.f;
	}

}

void CBossScript::Shoot_Skill4()
{
	if (!m_Summon)
	{
		m_Summon = true;
		Ptr<CPrefab> pPrefab = CResMgr::GetInst()->FindRes<CPrefab>(L"prefab\\HeadGun.pref");
		CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();

		Vec3 pTrans = Transform()->GetWorldPos();
		CGameObject* pObj = pPrefab->Instantiate();
		pObj->Transform()->SetRelativePos(Vec3(pTrans.x, pTrans.y + 50.f, 400.f));

		tEventInfo evninfo;
		evninfo.eType = EVENT_TYPE::CREATE_OBJ;
		evninfo.lParam = (DWORD_PTR)pObj;
		evninfo.wParam = 5;

		CEventMgr::GetInst()->AddEvent(evninfo);
	}
}

void CBossScript::Shoot_Skill5()
{
	m_fBossBulletTime += DT;


	if (m_fBossBulletTime >= 0.05f)
	{

		// 오른쪽
		Ptr<CPrefab> pPrefab = CResMgr::GetInst()->FindRes<CPrefab>(L"prefab\\M_Bullet.pref");
		CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();


		CM_Bullet* pBullet = (CM_Bullet*)CScriptMgr::GetScript((UINT)SCRIPT_TYPE::M_BULLET);

		Vec3 pTrans = Transform()->GetWorldPos();
		CGameObject* pObj = pPrefab->Instantiate();
		pObj->Transform()->SetRelativePos(Vec3(pTrans.x + 120.f, pTrans.y + 40.f, 0.f));

		if (f_Dir < XM_PI)
			f_Dir += 0.15f;
		else
			f_Dir = -XM_PI;
		
		float cos = cosf(f_Dir);
		float sin = sinf(f_Dir);

		Vec3 vDir = Vec3(cos,sin, 0.f);
		vDir.Normalize();


		pBullet->SetDir(vDir);
		pBullet->SetSpeed(300.f);

		pObj->AddComponent(pBullet);

		tEventInfo evninfo;
		evninfo.eType = EVENT_TYPE::CREATE_OBJ;
		evninfo.lParam = (DWORD_PTR)pObj;
		evninfo.wParam = 6;

		CEventMgr::GetInst()->AddEvent(evninfo);


		// 왼쪽

		pBullet = (CM_Bullet*)CScriptMgr::GetScript((UINT)SCRIPT_TYPE::M_BULLET);

		pObj = pPrefab->Instantiate();

		pObj->Transform()->SetRelativePos(Vec3(pTrans.x - 120.f, pTrans.y + 40.f, 0.f));



		vDir = Vec3(-cos, sin, 0.f);
		vDir.Normalize();


		pBullet->SetDir(vDir);
		pBullet->SetSpeed(300.f);

		pObj->AddComponent(pBullet);

		evninfo.eType = EVENT_TYPE::CREATE_OBJ;
		evninfo.lParam = (DWORD_PTR)pObj;
		evninfo.wParam = 6;

		CEventMgr::GetInst()->AddEvent(evninfo);
		m_fBossBulletTime = 0.f;
		
	}
}
