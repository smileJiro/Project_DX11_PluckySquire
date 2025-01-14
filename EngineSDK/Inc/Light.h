#pragma once

#include "Base.h"

BEGIN(Engine)
class CShader;
class CVIBuffer_Rect;
class CLight final : public CBase
{
private:
	CLight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLight() = default;

public:
	const LIGHT_DESC* Get_LightDesc() const {
		return &m_LightDesc;
	}

public:
	HRESULT Initialize(const LIGHT_DESC& LightDesc);
	HRESULT Render(CShader* _pShader, CVIBuffer_Rect* _pVIBuffer);
private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	LIGHT_DESC				m_LightDesc{};

public:
	static CLight* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHT_DESC& LightDesc);
	virtual void Free() override;
};

END