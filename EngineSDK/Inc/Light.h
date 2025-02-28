#pragma once

#include "Base.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CGameInstance;
class CRenderTarget;
class CLight : public CBase
{
private:
	CLight(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LIGHT_TYPE _eLightType);
	virtual ~CLight() = default;

public:
	HRESULT Initialize(const CONST_LIGHT& _LightDesc);
	HRESULT Render(CShader* _pShader, CVIBuffer_Rect* _pVIBuffer);

public: /* Shadow View Proj */
	HRESULT Compute_ViewProjMatrix(); // view 따로, proj 따로 저장해야함, 기존 모델들 vs shader 그대로 가려면

public:
	
public:
	// Get
	const CONST_LIGHT* Get_LightDesc_Ptr() const { return &m_tLightConstData; }
	CONST_LIGHT Get_LightDesc() const { return m_tLightConstData; }
	LIGHT_TYPE Get_Type() const { return m_eType; }
	_float4x4* Get_ViewMatrix() { return &m_ViewMatrix; }
	_float4x4* Get_ProjMatrix() { return &m_ProjMatrix; }
	CRenderTarget* Get_ShadowRenderTarget() { return m_pShadowRenderTarget; }
	_int Get_ShadowLightID() const { return m_iShadowLightID; }
	_bool Is_ShadowLight() const { return m_tLightConstData.isShadow; }
	// Set
	HRESULT Set_LightConstData_AndUpdateBuffer(const CONST_LIGHT& _LightConstData);
	void Set_Shadow(_bool _isShadow);
private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;
	CGameInstance*			m_pGameInstance = nullptr;
private:
	LIGHT_TYPE				m_eType = LIGHT_TYPE::LAST;
	CONST_LIGHT				m_tLightConstData = {};
	ID3D11Buffer*			m_pLightConstbuffer = nullptr;

private:
	static _int				s_iShadowLightID;
	_float4x4				m_ProjMatrix = {};
private: /* Shadow */
	_int					m_iShadowLightID = -1;
	_float					m_fFovy = 120.f;
	_float2					m_vNearFarPlane = { 0.1f, 300.f };
	_float4x4				m_ViewMatrix = {};
	CRenderTarget*			m_pShadowRenderTarget = nullptr;


#ifdef _DEBUG
	PrimitiveBatch<VertexPositionColor>* m_pBatch = nullptr;
	BasicEffect* m_pEffect = nullptr;
	ID3D11InputLayout* m_pInputLayout = nullptr;
#endif // _DEBUG
public:
	static CLight* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const CONST_LIGHT& _LightDesc, LIGHT_TYPE _eLightType);
	virtual void Free() override;
};

END