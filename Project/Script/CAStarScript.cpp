#include "pch.h"
#include "CAStarScript.h"

#include "CMovementScript.h"

#include <Engine/CTileMap.h>
#include <Engine/CCollider2D.h>


int g_iXCount = 0;
int g_iYCount = 0;

CAStarScript::CAStarScript()
	:CScript((int)SCRIPT_TYPE::ASTARSCRIPT)
	, m_iDestRow(0)
	, m_iDestCol(0)
{
}

CAStarScript::~CAStarScript()
{
}


void CAStarScript::start()
{

}

void CAStarScript::update()
{
	
}


void CAStarScript::Rebuild(priority_queue<tNode*, vector<tNode*>, Compare>& _queue)
{
	priority_queue<tNode*, vector<tNode*>, Compare> tempQueue;

	while (!_queue.empty())
	{
		tempQueue.push(_queue.top());
		_queue.pop();
	}

	_queue.swap(tempQueue);
}

void CAStarScript::FindPath(CGameObject* _OtherObj)
{	
	
	for (int i = 0; i < g_iYCount; ++i)
	{
		for (int j = 0; j < g_iXCount; ++j)
		{
			arrNode[i][j].bPath = false;
			arrNode[i][j].bOpen = false;
			arrNode[i][j].bClosed = false;
			arrNode[i][j].bMove = true;
		}
	}



	WallCheck(_OtherObj);

	arrNode[m_iDestCol][m_iDestRow].bMove = true;
	int iCurX = m_iRow;
	int iCurY = m_iCol;

	tNode* pCurNode = &arrNode[iCurY][iCurX];
	pCurNode->bClosed = true;

	while (true)
	{
		// ���� �������� 4������ ��带 OpenList �� �ִ´�.
		
		// LEFT		
		AddOpenList(pCurNode->iIdxX - 1
			, pCurNode->iIdxY, pCurNode);
		// UP
		AddOpenList(pCurNode->iIdxX
			, pCurNode->iIdxY - 1, pCurNode);

		// RIGHT		
		AddOpenList(pCurNode->iIdxX + 1
			, pCurNode->iIdxY, pCurNode);

		// DOWN		
		AddOpenList(pCurNode->iIdxX
			, pCurNode->iIdxY + 1, pCurNode);

		//// LEFT		
		//AddOpenList(pCurNode->iIdxX - 1
		//	, pCurNode->iIdxY - 1, pCurNode);
		//// UP
		//AddOpenList(pCurNode->iIdxX + 1
		//	, pCurNode->iIdxY - 1, pCurNode);

		//// RIGHT		
		//AddOpenList(pCurNode->iIdxX + 1
		//	, pCurNode->iIdxY + 1, pCurNode);

		//// DOWN		
		//AddOpenList(pCurNode->iIdxX - 1
		//	, pCurNode->iIdxY + 1, pCurNode);


		// 2. Open List ���� ���� ȿ���� ���� ��带 ������.
		//  - �ش� ���� ClostList �� �ִ´�.

		if (OpenList.size() == 0)
		{
			m_MonDir = Vec2(0.f, 0.f);
			return;
		}

		pCurNode = OpenList.top();
		pCurNode->bClosed = true;

		OpenList.pop();

		// �������� ������ ��� Astar ����
		if (pCurNode->iIdxX == m_iDestRow && pCurNode->iIdxY == m_iDestCol)
		{
			break;
		}
	}

	//// �� ǥ��
	while (true)
	{
	

		//arrNode[pCurNode->iIdxY][pCurNode->iIdxX].bPath = true;
		tNode* pPrevNode = pCurNode;
		pCurNode = pCurNode->pPrevNode;
		//pList.push_front(pCurNode);
		// ���⼭ ����Ʈ�� �غ��ؼ� push_front�� �ؼ� ���� ���� �ε����� �������ٸ�(�������� ������ �Ѵٱ��)
		// �� ������������� ������ ����


		if (pCurNode->iIdxX == m_iRow && pCurNode->iIdxY == m_iCol)
		{
			
			pCurNode->bPath = true;

			m_MonDir = Vec2(pPrevNode->iIdxX - m_iRow, -pPrevNode->iIdxY + m_iCol);
			m_MonDir.Normalize();
			break;
		}
	}

	while (!OpenList.empty())
		OpenList.pop();
}


void CAStarScript::CalculateCost(tNode* _pCurNode, tNode* _pOrigin)
{
	_pCurNode->pPrevNode = _pOrigin;
	_pCurNode->fFromParent = 10.f + _pOrigin->fFromParent;
	_pCurNode->fToDest = abs(m_iDestRow - _pCurNode->iIdxX) * 10.f + abs(m_iDestCol - _pCurNode->iIdxY) * 10;
	_pCurNode->fFinal = _pCurNode->fFromParent + _pCurNode->fToDest;
}

void CAStarScript::AddOpenList(int _iXIdx, int _iYIdx, tNode* _pOrigin)
{
		// ���� �������� �� �� �ִ� ���� OpenList �� �ִ´�.
	// ��� ������ ��� ���
		if (_iXIdx < 0 || _iXIdx >= g_iXCount || _iYIdx < 0 || _iYIdx >= g_iYCount
			|| !arrNode[_iYIdx][_iXIdx].bMove)
			return;
	
		// �ش� ���� Clost List �� �ִ� ���, Open List �� ���� �ʴ´�.
		if (arrNode[_iYIdx][_iXIdx].bClosed)
			return;
	
		// Open List �� ����� ����ؼ� �ִ´�.
		if (false == arrNode[_iYIdx][_iXIdx].bOpen)
		{
			CalculateCost(&arrNode[_iYIdx][_iXIdx], _pOrigin);
			// Open List �� �ִ´�.
			arrNode[_iYIdx][_iXIdx].bOpen = true;
			OpenList.push(&arrNode[_iYIdx][_iXIdx]);
		}
		else // �̹� OpenList �� �ִ� ���,
		{
			//����� ����ؼ� �� ȿ���� ���� ������ ��ü�Ѵ�.
			tNode node = arrNode[_iYIdx][_iXIdx];
			CalculateCost(&node, _pOrigin);
	
			if (arrNode[_iYIdx][_iXIdx].fFinal > node.fFinal)
			{
				arrNode[_iYIdx][_iXIdx] = node;
	
				// ���¸���Ʈ(�켱����ť) �缳��
				Rebuild(OpenList);
			}
		}
}

void CAStarScript::WallCheck(CGameObject* _OtherObj)
{
	if (nullptr == _OtherObj->TileMap())
		return;

	int XCount = _OtherObj->TileMap()->GetTileCountX();
	int YCount = _OtherObj->TileMap()->GetTileCountY();

	vector<tTileData> vTileData = _OtherObj->TileMap()->GetTileData();

	for (int i = 0; i < YCount; ++i)
	{
		for (int j = 0; j < XCount; ++j)
		{
			arrNode[i][j].iIdxX = j;
			arrNode[i][j].iIdxY = i;

			int Idx = i * (XCount - 1) + j;

			if (vTileData[Idx].TilePass == 1)
				arrNode[i][j].bMove = false;
		}
	}
}





void CAStarScript::OnCollisionEnter(CGameObject* _OtherObj)
{

}

void CAStarScript::OnCollision(CGameObject* _OtherObj)
{

	
}



void CAStarScript::CurObjIdx(CGameObject* _OtherObj)
{
	CTileMap* pTileMap = _OtherObj->TileMap();

	//_OtherObj->GetScript<CTileMapScript>();

	//CScene* pCurScene = CSceneMgr::GetInst()->GetCurScene();
	//CGameObject* pObj = pCurScene->GetPlayer();
	//Vec3 vPlayerPos = pObj->Transform()->GetWorldPos();
	//Vec3 vPlayerScale = pObj->Transform()->GetWorldScale();

	Vec3 vOtherPos = _OtherObj->Transform()->GetWorldPos();
	Vec3 vOtherScale = _OtherObj->Transform()->GetWorldScale();

	float OtherLeft = vOtherPos.x - vOtherScale.x / 2.f;
	float OtherRight = vOtherPos.x + vOtherScale.x / 2.f;
	float OtherDown = vOtherPos.y - vOtherScale.y / 2.f;
	float OtherUp = vOtherPos.y + vOtherScale.y / 2.f;

	Vec3 vPos = Transform()->GetWorldPos();
	Vec3 vScale = Transform()->GetWorldScale();

	Vec2 vColPos = Collider2D()->GetOffsetPos();
	Vec2 vColScale = Collider2D()->GetOffsetScale();
	

	//Vec2(TileMapLT.x - vPos.x + vColPos.x , TileMapLT.y - vPos.y + vColPos.y);
	
	Vec2 TileMapLT = Vec2(vOtherPos.x - vOtherScale.x / 2.f, vOtherPos.y + vOtherScale.y / 2.f);
	Vec2 ObjOnTile =  Vec2(TileMapLT.x - vPos.x + vColPos.x, TileMapLT.y - vPos.y + vColPos.y);
	ObjOnTile.y -= vScale.y / 2.f;
	ObjOnTile.y -= vColPos.y / 2.f;
	int TileXSize = (int)vOtherScale.x / pTileMap->GetTileCountX();
	int TileYSize = (int)vOtherScale.y / pTileMap->GetTileCountY();

	vector<tTileData> vTileData = pTileMap->GetTileData();

	m_iRow = abs(ObjOnTile.x) / TileXSize;
	m_iCol = abs(ObjOnTile.y) / TileYSize;
	//���� �� ���� ������ ���ؼ� Col* RowCnt +Row�ϸ�� 
	m_iTileIdx = m_iCol * (pTileMap->GetTileCountX()) + m_iRow;
}