#pragma once

#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CLight;
class CLight_Manager final : public CBase
{
private:
	CLight_Manager(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CLight_Manager() = default;

public:
	const CONST_LIGHT* Get_LightDesc_Ptr(_uint _iIndex) const;

public:
	HRESULT Initialize();
	HRESULT Add_Light(const CONST_LIGHT& LightDesc, LIGHT_TYPE _eType);
	void	Update(_float _fTimeDelta);
	HRESULT Render(class CShader* _pShader, class CVIBuffer_Rect* _pVIBuffer);
public:
	HRESULT Load_Lights(const _wstring& _strLightsJsonPath);

	HRESULT Delete_Light(_uint _iLightIndex);
public:
	void	Level_Exit();
	const list<CLight*>& Get_Lights() { return m_Lights; }
private:
	CGameInstance* m_pGameInstance = nullptr;
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
	list<CLight*>	 m_Lights;

private:
	void Clear();
public:
	static CLight_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END