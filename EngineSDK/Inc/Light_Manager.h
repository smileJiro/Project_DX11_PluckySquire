#pragma once

#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CLight;
class CGameObject;
class CLight_Target;
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
	HRESULT Add_Light_Target(const CONST_LIGHT& LightDesc, LIGHT_TYPE _eType, CGameObject* _pGameObject, const _float3& _vOffsetPosition, CLight_Target** _ppOut, _bool _isNotClear = true);
	void	Update(_float _fTimeDelta);
	HRESULT Render(class CShader* _pShader, class CVIBuffer_Rect* _pVIBuffer);

public:
	HRESULT Load_Lights(const _wstring& _strLightsJsonPath);
	HRESULT Delete_Light(_uint _iLightIndex);
	HRESULT Delete_Light(list<CLight*>::iterator& _iter);

public:
	void	Level_Exit();
	const list<CLight*>& Get_Lights() { return m_Lights; }

private:
	CGameInstance* m_pGameInstance = nullptr;
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;
	list<CLight*>	 m_Lights;

private:
	void Clear_Load(); // Target Light 제외(런타임 기믹용)
	void Clear();
public:
	static CLight_Manager* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

END