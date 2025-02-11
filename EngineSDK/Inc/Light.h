#pragma once

#include "Base.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CGameInstance;
class CLight : public CBase
{
private:
	CLight(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, LIGHT_TYPE _eLightType);
	virtual ~CLight() = default;

public:
	const CONST_LIGHT* Get_LightDesc() const {
		return &m_tLightConstData;
	}

public:
	HRESULT Initialize(const CONST_LIGHT& _LightDesc);
	void Update(_float _fTimeDelta);
	void Late_Update(_float _fTimeDelta);
	HRESULT Render(CShader* _pShader, CVIBuffer_Rect* _pVIBuffer);

public:
	HRESULT Set_LightConstData_AndUpdateBuffer(const CONST_LIGHT& _LightConstData);

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;
	CGameInstance*			m_pGameInstance = nullptr;
private:
	LIGHT_TYPE				m_eType = LIGHT_TYPE::LAST;
	CONST_LIGHT				m_tLightConstData = {};
	ID3D11Buffer*			m_pLightConstbuffer = nullptr;
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