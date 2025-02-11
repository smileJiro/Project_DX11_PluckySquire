#pragma once

#include "Base.h"

BEGIN(Engine)
class CLight;
class CLight_Manager final : public CBase
{
private:
	CLight_Manager(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLight_Manager() = default;

public:
	const CONST_LIGHT* Get_LightDesc(_uint iIndex) const;

public:
	HRESULT Initialize();
	HRESULT Add_Light(const CONST_LIGHT& LightDesc, LIGHT_TYPE _eType);
	HRESULT Render(class CShader* _pShader, class CVIBuffer_Rect* _pVIBuffer);

public:
	void	Level_Exit();

private:
	ID3D11Device* m_pDevice = { nullptr };
	ID3D11DeviceContext* m_pContext = { nullptr };
	list<CLight*>	 m_Lights;

private:
	void Clear();
public:
	static CLight_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END