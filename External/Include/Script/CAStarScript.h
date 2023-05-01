#pragma once
#include <Engine/Cscript.h>



typedef struct _tagNode
{
	_tagNode*   pPrevNode;   // 이전 노드
	float		fFromParent; // 이전 노드에서 현재 노드까지의 거리
	float		fToDest;	 // 현재 노드에서 목적지 까지의 거리
	float		fFinal;		 // 위에 두 값을 합친 값(우선순위 기준)

	int			iIdxX;
	int			iIdxY;
	bool		bMove;   // 이동 가능 불가능
	bool		bOpen;   // OpenList 에 들어갔는지
	bool		bClosed; // ClostList 에 들어있는지

	bool		bPath;
}tNode;

// Open List
class Compare
{
public:
	bool operator() (tNode* _pLeft, tNode* _pRight)
	{
		return _pLeft->fFinal > _pRight->fFinal;
	}
};


class CAStarScript :
    public CScript
{
private:
    priority_queue<tNode*, vector<tNode*>, Compare>	OpenList;
	list<tNode*> pList;

	tNode   arrNode[60][60];

	float	m_ftime;

	int		m_iDestRow;
	int		m_iDestCol;

	int		m_iRow;
	int		m_iCol;
	int		m_iTileIdx;

	Vec2	m_MonDir;


public:
	virtual void start() override;
    virtual void update() override;

	void CurObjIdx(CGameObject* _OtherObj);

public:
	virtual void OnCollisionEnter(CGameObject* _OtherObj) override;
	virtual void OnCollision(CGameObject* _OtherObj) override;
    CLONE(CAStarScript)    

public:
	void Rebuild(priority_queue<tNode*, vector<tNode*>, Compare>& _queue);
	void FindPath(CGameObject* _OtherObj);
	void CalculateCost(tNode* _pCurNode, tNode* _pOrigin);
	void AddOpenList(int _iXIdx, int _iYIdx, tNode* _pOrigin);

	Vec2 GetDir() {	return m_MonDir;}
	void WallCheck(CGameObject* _OtherObj);


public:
    CAStarScript();
    ~CAStarScript();
};

