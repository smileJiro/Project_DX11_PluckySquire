#pragma once
#include "Base.h"

/* 그림자 깊이 비교용 카메라를 위한 뷰, 투영 행렬을 보관한다 >> 그림자를 생성하고자하는 기준 광원에 대한 뷰 투영 행렬 */
BEGIN(Engine)
class CShadow final : public CBase
{
public:
	enum D3DTRANSFORMSTATE { D3DTS_VIEW, D3DTS_PROJ, D3DTS_END };

private:
	CShadow(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CShadow() = default;

public:
	const _float4x4* Get_Shadow_Transform_Ptr(D3DTRANSFORMSTATE _eState) {
		return &m_TransformMatrices[_eState];
	}

public:
	void SetUp_TransformMatrix(D3DTRANSFORMSTATE _eState, _fmatrix _TransformMatrix) {
		XMStoreFloat4x4(&m_TransformMatrices[_eState], _TransformMatrix);
	}

public:
	void SetUp_TargetShadowMatrix(_fvector _vLightDirection, _fvector _vWorldPos, _float _fDistance, _float2 _vNearFar);
public:
	HRESULT Initialize();
	void Update();

private:
	_float4x4			 m_TransformMatrices[D3DTS_END];
	_uint				 m_iWidth{}, m_iHeight{};
	ID3D11Device*		 m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;

public:
	static CShadow* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void Free() override;
};

END