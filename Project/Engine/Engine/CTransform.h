#pragma once
#include "CComponent.h"

class CTransform :
    public CComponent
{
private:
    Vec3    m_vRelativePos;
    Vec3    m_vRelativeScale;
    Vec3    m_vRelativeRot;     // �ະ ȸ����

    Vec3    m_arrRelativeDir[(UINT)DIR_TYPE::END]; // ��� ����
    Vec3    m_arrWorldDir[(UINT)DIR_TYPE::END];    // ���� ����󿡼��� ����

    Matrix  m_matWorld; // ��ġ��ȯ ���� ���

    bool    m_bIgnoreParentScale;

public:
    void SetRelativePos(const Vec3& _vPos) { m_vRelativePos = _vPos; }
    void SetRelativePos(float _x, float _y, float _z) { m_vRelativePos = Vec3(_x, _y, _z); }

    void SetRelativeScale(const Vec3& _vScale) { m_vRelativeScale = _vScale; }
    void SetRelativeScale(float _x, float _y, float _z) { m_vRelativeScale = Vec3(_x, _y, _z); }

    void SetRelativeRotation(const Vec3 _vRot) { m_vRelativeRot = _vRot; }
    void SetRelativeRotation(float _x, float _y, float _z) { m_vRelativeRot = Vec3(_x, _y, _z); }


	const Vec3& GetRelativePos() const { return m_vRelativePos; }
	const Vec3& GetRelativeScale() const { return m_vRelativeScale; }
	const Vec3& GetRelativeRotation() const { return m_vRelativeRot; };

	const Vec3& GetRelativeDir(DIR_TYPE _eType) const  { return m_arrRelativeDir[(UINT)_eType]; }
	const Vec3& GetRelativeRightDir() const { return m_arrRelativeDir[(UINT)DIR_TYPE::RIGHT]; }
	const Vec3& GetRelativeUpDir() const { return m_arrRelativeDir[(UINT)DIR_TYPE::UP]; }
	const Vec3& GetRelativeFrontDir() const { return m_arrRelativeDir[(UINT)DIR_TYPE::FRONT]; }

	const Vec3& GetWorldDir(DIR_TYPE _eType) const { return m_arrWorldDir[(UINT)_eType]; }
	const Vec3& GetWorldRightDir() const { return m_arrWorldDir[(UINT)DIR_TYPE::RIGHT]; }
	const Vec3& GetWorldUpDir() const  { return m_arrWorldDir[(UINT)DIR_TYPE::UP]; }
	const Vec3& GetWorldFrontDir()  const { return m_arrWorldDir[(UINT)DIR_TYPE::FRONT]; }

    Vec3 GetWorldPos() const { return m_matWorld.Translation(); }
    Vec3 GetWorldScale();

    const Matrix& GetWorldMat() const { return m_matWorld; }

    void SetIgnoreParentScale(bool _bSet){ m_bIgnoreParentScale = _bSet;}
    bool GetIgnoreParentScale() const { return m_bIgnoreParentScale; }

    virtual void UpdateData() override;
    virtual void finalupdate() override;

    virtual void active();
    virtual void deactive();

public:
    virtual void SaveToScene(FILE* _pFile) override;
    virtual void LoadFromScene(FILE* _pFile) override;

    CLONE(CTransform)
public:
    CTransform();
    ~CTransform();
};
