#include "pch.h"
#include "CCameraMoveScript.h"

#include <Engine/CKeyMgr.h>
#include <Engine/CTimeMgr.h>
#include <Engine/CTransform.h>
#include <Engine/CCamera.h>

#include <Engine/CSceneMgr.h>
#include <Engine/CScene.h>
#include <Engine/CLayer.h>

#include <Engine/CEventMgr.h>
#include <Engine/CRenderMgr.h>

#include "CPlayerStateScript.h"
#include "CHeartScript.h"
#include "CBlankBulletScript.h"

CCameraMoveScript::CCameraMoveScript()
	: CScript((int)SCRIPT_TYPE::CAMERAMOVESCRIPT)
	, m_Filter(nullptr)
	, m_fCamSpeed(700.f)	
	, m_PlayerTrace(false)
	, m_DistortionTime(0.f)
	, m_DistortionDist(0.f)
	, m_Event(false)
	, m_fTime(0.f)
	, m_BossPos{}
	, m_fEventTime(6.f)
{
}

CCameraMoveScript::~CCameraMoveScript()
{
}

void CCameraMoveScript::start()
{
	if (m_Filter == nullptr)
	{
		CGameObject* pPostProcess = new CGameObject;
		pPostProcess->SetName(L"PostProcessObject");

		pPostProcess->AddComponent(new CTransform);
		pPostProcess->AddComponent(new CMeshRender);

		pPostProcess->MeshRender()->SetMesh(CResMgr::GetInst()->FindRes<CMesh>(L"RectMesh"));
		pPostProcess->MeshRender()->SetSharedMaterial(CResMgr::GetInst()->FindRes<CMaterial>(L"material\\PostProcessMtrl.mtrl"));
		
		tEventInfo tevninfo;
		
		tevninfo.eType = EVENT_TYPE::CREATE_OBJ;
		tevninfo.lParam = (DWORD_PTR)pPostProcess;
		tevninfo.wParam = 0;

		CEventMgr::GetInst()->AddEvent(tevninfo);
		pPostProcess->Deactivate();
		((CCameraMoveScript*)GetOwner()->GetScript(0))->SetFilter(pPostProcess);
	}

	if (nullptr == m_pPlayer)
	{
		CScene* pScene = CSceneMgr::GetInst()->GetCurScene();
		CLayer* pLayer = pScene->GetLayer(3);

		vector<CGameObject*> vRoot = pLayer->GetRootObjects();
		for (int i = 0; i < vRoot.size(); ++i)
		{
			if (vRoot[i]->GetName() == L"Player")
			{
				m_pPlayer = vRoot[i];
			}
		}
	}

	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	if (pCurScene->GetSceneState() == SCENE_STATE::PLAY)
		m_PlayerTrace = true;
	else
		m_PlayerTrace = false;
}

void CCameraMoveScript::update()
{
	Vec3 vPos = Transform()->GetRelativePos();
	float CamWidth = Camera()->GetWidth();
	float fSpeed = m_fCamSpeed;


	if (KEY_PRESSED(KEY::LSHFT))
	{
		fSpeed *= 2.f;
	}

	if (PROJ_TYPE::PERSPECTIVE == Camera()->GetProjType())
	{
		if (KEY_PRESSED(KEY::UP))
		{
			Vec3 vFront = Transform()->GetWorldFrontDir();
			vPos += DT * vFront * fSpeed;
		}

		if (KEY_PRESSED(KEY::DOWN))
		{
			Vec3 vFront = Transform()->GetWorldFrontDir();
			vPos -= DT * vFront * fSpeed;
		}

		if (KEY_PRESSED(KEY::LEFT))
		{
			Vec3 vRight = Transform()->GetWorldRightDir();
			vPos -= DT * vRight * fSpeed;
		}

		if (KEY_PRESSED(KEY::RIGHT))
		{
			Vec3 vRight = Transform()->GetWorldRightDir();
			vPos += DT * vRight * fSpeed;
		}
	}

	else
	{
		if (KEY_PRESSED(KEY::W))
		{			
			vPos.y += DT * fSpeed;
		}

		if (KEY_PRESSED(KEY::S))
		{
			vPos.y -= DT * fSpeed;
		}

		if (KEY_PRESSED(KEY::A))
		{
			vPos.x -= DT * fSpeed;
		}

		if (KEY_PRESSED(KEY::D))
		{
			vPos.x += DT * fSpeed;
		}
		if (KEY_PRESSED(KEY::_4))
		{
			CamWidth += DT * fSpeed;
			Camera()->SetWidth(CamWidth);
		}
		if (KEY_PRESSED(KEY::_5))
		{
			CamWidth -= DT * fSpeed;
			Camera()->SetWidth(CamWidth);
		}
	}

	CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	if (KEY_TAP(KEY::_9) && pCurScene->GetSceneState() != SCENE_STATE::PLAY)
	{
		m_PlayerTrace = !m_PlayerTrace;
	}


	if (m_Filter != nullptr)
	{

		if (KEY_TAP(KEY::Q) && !m_bDistortion)
		{
			CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
			CGameObject* pPlayer = pCurScene->GetPlayer();

			if (nullptr == pPlayer)
				return;

			CPlayerStateScript* pScript = pPlayer->GetScript<CPlayerStateScript>();
			vector<CGameObject*>& vBlankObj = pScript->GetBlankBullet();
			size_t BlankSize = vBlankObj.size();
			if (BlankSize > 0)
			{
				CGameObject* Obj = vBlankObj.back();
				vBlankObj.pop_back();
				Obj->Destroy();
			}


			if (m_Filter->IsActive())
				m_Filter->Deactivate();
			else
				m_Filter->Activate();

			if (!m_bDistortion)
			{
				m_bDistortion = !m_bDistortion;
			}

			// ø÷∞Ó Ω¶¿Ã¥ı
			Ptr<CPrefab> pPrefab = CResMgr::GetInst()->FindRes<CPrefab>(L"prefab\\BlankBulletEffect.pref");
			CGameObject* pObj = pPrefab->Instantiate();

			tEventInfo tevninfo;
			pObj->Transform()->SetRelativePos(pPlayer->Transform()->GetRelativePos());

			tevninfo.eType = EVENT_TYPE::CREATE_OBJ;
			tevninfo.lParam = (DWORD_PTR)pObj;
			tevninfo.wParam = 7;
			CEventMgr::GetInst()->AddEvent(tevninfo);
			Ptr<CSound> pSound = CResMgr::GetInst()->Load<CSound>(L"sound\\BlankBullet\\Sound.wav", L"sound\\BlankBullet\\Sound.wav");
			pSound->Play(1, 0.1f, true);
		}

		m_Filter->MeshRender()->GetMaterial()->SetScalarParam(SCALAR_PARAM::FLOAT_0, &m_DistortionTime);
		m_Filter->MeshRender()->GetMaterial()->SetScalarParam(SCALAR_PARAM::FLOAT_1, &m_DistortionDist);

		if (m_bDistortion)
		{
			m_DistortionTime += DT;
			m_DistortionDist += DT;
		}
		else
		{
			m_DistortionTime = 0.f;
			m_DistortionDist = 0.f;
		}

		if (m_DistortionTime > 0.7f)
		{
			m_Filter->Deactivate();
			m_bDistortion = false;
		}
	}

	if (m_PlayerTrace && m_pPlayer)
	{
		Vec3 pPlayerPos = m_pPlayer->Transform()->GetWorldPos();
		Vec2 vResolution = CDevice::GetInst()->GetRenderResolution();
		Vec2 vMousePos = CKeyMgr::GetInst()->GetMousePos();

		Vec2 vWinMouse = Vec2(vMousePos.x - vResolution.x / 2.f, vMousePos.y - vResolution.y / 2.f);
		vWinMouse /= 6.f;
		vPos = Vec3(pPlayerPos.x + vWinMouse.x, pPlayerPos.y - vWinMouse.y, 0.f);
		Transform()->SetRelativePos(vPos);
	}


	if (m_Event)
	{
		m_PlayerTrace = false;
		m_fTime += DT;

		CPlayerStateScript* pScript = m_pPlayer->GetScript<CPlayerStateScript>();
		pScript->SetState(PLAYER_TYPE::WAIT);

		if (m_fTime > m_fEventTime)
		{
			m_fTime = 0.f;
			m_Event = false;
			m_BossPos = {};
			m_PlayerTrace = true;
			pScript->SetState(PLAYER_TYPE::IDLE);
		}
		Vec3 vPlayerPos = CSceneMgr::GetInst()->GetCurScene()->GetPlayerPos();
		Vec2 vDir = Vec2(m_BossPos.x - vPlayerPos.x, m_BossPos.y - vPlayerPos.y);
		vDir.Normalize();
		double PlayerBossDist = sqrt(pow(vPlayerPos.x - m_BossPos.x, 2) + pow(vPlayerPos.y - m_BossPos.y, 2));

		Vec3 vPos = Transform()->GetRelativePos();
		vPos.x += vDir.x *(PlayerBossDist / m_fEventTime) * DT * 3;
		vPos.y += vDir.y * (PlayerBossDist / m_fEventTime) * DT * 3;

		double CameraBossDist = sqrt(pow(vPos.x - m_BossPos.x, 2) + pow(vPos.y - m_BossPos.y, 2));

		if(CameraBossDist >= 10.f)
		Transform()->SetRelativePos(vPos);
	}

}

void CCameraMoveScript::lateupdate()
{


	CScene* pScene = CSceneMgr::GetInst()->GetCurScene();
	CGameObject* pPlayer = pScene->GetPlayer();

	if (nullptr == pPlayer)
		return;

	CPlayerStateScript* pScript = pPlayer->GetScript<CPlayerStateScript>();
	if (nullptr == pScript)
		return;

	vector<CGameObject*> v = pScript->GetHeart();

	for (int i = 0; i < v.size(); ++i)
	{
		CHeartScript* _pScript = v[i]->GetScript<CHeartScript>();
		Vec3 vPos = Transform()->GetRelativePos();

		float vCamWidth = CRenderMgr::GetInst()->GetMainCamWidth();
		float vCamHeight = CRenderMgr::GetInst()->GetMainCamHeight();
		vPos.x -= vCamWidth / 2.f;
		vPos.y += vCamHeight / 2.f - 50.f;

		vPos.x += 50 + (50 * _pScript->GetNum());

		v[i]->Transform()->SetRelativePos(vPos);
	}

	vector<CGameObject*> v2 = pScript->GetBlankBullet();

	for (int i = 0; i < v2.size(); ++i)
	{
		CBlankBulletScript* _pScript = v2[i]->GetScript<CBlankBulletScript>();
		Vec3 vPos = Transform()->GetRelativePos();

		float vCamWidth = CRenderMgr::GetInst()->GetMainCamWidth();
		float vCamHeight = CRenderMgr::GetInst()->GetMainCamHeight();
		vPos.x -= vCamWidth / 2.f;
		vPos.y += vCamHeight / 2.f - 100.f;

		vPos.x += 50 + (50 * _pScript->GetNum());
		v2[i]->Transform()->SetRelativePos(vPos);
	}
}



void CCameraMoveScript::SaveToScene(FILE* _pFile)
{
	
}

void CCameraMoveScript::LoadFromScene(FILE* _pFile)
{
}
