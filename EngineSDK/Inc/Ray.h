#pragma once
#include "Component.h"
BEGIN(Engine)
class CGameInstance;
class ENGINE_DLL CRay final : public CComponent
{
public:
	typedef struct
	{
		_float3 fStartPos;
		_float3 fDir;
		_float fViewportWidth, fViewportHeight;
	}RAY_DESC;
private:
	CRay(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	CRay(const CRay& _Prototype);
	virtual ~CRay() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* _pArg) override;

public:
	/* 뷰포트가 분리되며 여러 ViewMatrix, ProjMatrix가 생길 것을 고려하여 매개변수를 통한 함수를 설계함. */
	_bool Compute_Intersect_Triangle(_fvector _v0, _fvector _v1, _fvector _v2, _float& _fDist);
	_bool Compute_IntersectCoord_Triangle(_fvector _v0, _fvector _v1, _fvector _v2, _float3& _vIntersectCoord);
	HRESULT Update_RayInfoFromCursor(_float2 _vCursorPos, _fmatrix _InverseViewMatrix, _cmatrix _InverseProjMatrix);

public:
	// Get

	// Set
	void Set_ViewportInfo(_float _fWidth, _float _fHeight) { m_fViewportX = _fWidth, m_fViewportY = _fHeight; };
private:
	_float3 m_vStartPos = {};
	_float3 m_vDir = {};

	_float m_fViewportX, m_fViewportY;

public:
	static CRay* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual CComponent* Clone(void* _pArg);
	virtual void Free() override;
};

END
