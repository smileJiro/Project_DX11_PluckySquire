#pragma once

#include "Base.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CGameInstance;
class CRenderTarget;
class ENGINE_DLL CLight : public CBase
{
protected:
	CLight(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LIGHT_TYPE _eLightType);
	virtual ~CLight() = default;

public:
	virtual HRESULT Initialize(const CONST_LIGHT& _LightDesc);
	virtual void	Update(_float _fTimeDelta);
	virtual HRESULT Render_Light(CShader* _pShader, CVIBuffer_Rect* _pVIBuffer);

#ifdef _DEBUG
	HRESULT Render_Base_Debug() override;
	void Set_DrawLightColor(const _vector& _vColor) { m_vDrawLightColor = _vColor; }
#endif // !_DEBUG

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
	_int Get_LightID() const { return m_iLightID; }
	_float Get_DirectionalLightLength() const { return m_fDirectionalLightLength; }
	const string& Get_Name() const { return m_strName; }
	_bool Is_ShadowLight() const { return m_tLightConstData.isShadow; }
	_bool Is_NotClear() const { return m_isNotClear; }
	// Set
	void Set_DirectionalLightLength(_float _fDirectionalLength) { m_fDirectionalLightLength = _fDirectionalLength; }
	HRESULT Set_LightConstData_AndUpdateBuffer(const CONST_LIGHT& _LightConstData);
	HRESULT Update_LightConstBuffer();
	void Set_Shadow(_bool _isShadow);
	void Set_Name(const string& _strNewName) { m_strName = _strNewName; }
	
protected:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;
	CGameInstance*			m_pGameInstance = nullptr;
protected:
	string					m_strName;
	LIGHT_TYPE				m_eType = LIGHT_TYPE::LAST;
	CONST_LIGHT				m_tLightConstData = {};
	ID3D11Buffer*			m_pLightConstbuffer = nullptr;

protected:
	static _int				s_iLightIDCount;
	_float4x4				m_ProjMatrix = {};

protected: /* Shadow */
	_int					m_iLightID = -1;
	_float					m_fFovy = 120.f;
	_float2					m_vNearFarPlane = { 0.1f, 300.f };
	_float4x4				m_ViewMatrix = {};
	CRenderTarget*			m_pShadowRenderTarget = nullptr;

protected:
	_float					m_fDirectionalLightLength = 70.f;
protected:
	_bool					m_isNotClear = false; // Clear_Load 호출 시 삭제되지 않는 


#ifdef _DEBUG
	PrimitiveBatch<VertexPositionColor>* m_pBatch = nullptr;
	BasicEffect* m_pEffect = nullptr;
	ID3D11InputLayout* m_pInputLayout = nullptr;
	_vector m_vDrawLightColor = { 1.f, 1.0f, 0.0f, 1.0f };
#endif // _DEBUG

private:
	virtual void			Active_OnEnable();
	virtual void			Active_OnDisable();

public:
	static CLight* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const CONST_LIGHT& _LightDesc, LIGHT_TYPE _eLightType);
	virtual void Free() override;
};

END