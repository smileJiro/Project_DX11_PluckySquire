#pragma once
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CGameObject;
class CComponent;
class CShader;
class CVIBuffer_Rect;
class CRenderGroup;
class CNewRenderer final : public CBase
{

private:
	CNewRenderer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CNewRenderer() = default;

public:
	HRESULT								Initialize();
	HRESULT								Add_RenderObject(_int _iGroupID, _int _iPriorityID, CGameObject* _pGameObject);
	HRESULT								Draw_RenderObject();

public:
	HRESULT								Add_RenderGroup(_int _iGroupID, _int _iPriorityID, CRenderGroup* _pRenderGroup);
	CRenderGroup*						Find_RenderGroup(_int _iGroupID, _int _iPriorityID);
	HRESULT								Erase_RenderGroup(_int _iGroupID, _int _iPriorityID);
public: /* DSV */
	ID3D11DepthStencilView*				Find_DSV(const _wstring& _strDSVTag);
	HRESULT								Add_DSV(const _wstring _strDSVTag, _float2 _vDSVSize);
	HRESULT								Add_DSV(const _wstring _strDSVTag, _uint _iWidth, _uint _iHeight);
	HRESULT								Add_DSV(const _wstring _strDSVTag, ID3D11DepthStencilView* _pDSV);
	HRESULT								Erase_DSV(const _wstring _strDSVTag);

public:
	HRESULT								Load_IBL(const _wstring& _strIBLJsonPath);
#ifdef _DEBUG

public:
	void Update_Imgui();
public: 
	HRESULT Add_DebugComponent(CComponent* pDebugCom)
	{
		m_DebugComponents.push_back(pDebugCom);
		Safe_AddRef(pDebugCom);
		return S_OK;
	}
	void Set_DebugRender(_bool _isBool) { m_isDebugRender = _isBool; }
#endif //debug component render


public:
	// Get 
	const _float4x4* Get_WorldMatrix() const { return &m_WorldMatrix; }
	const _float4x4* Get_ViewMatrix() const { return &m_ViewMatrix; }
	const _float4x4* Get_ProjMatrix() const { return &m_ProjMatrix; }

private:
	ID3D11Device*						m_pDevice = nullptr;
	ID3D11DeviceContext*				m_pContext = nullptr;
	CGameInstance*						m_pGameInstance = nullptr;

private: /* 직교 투영으로 그리기 위한 */
	CShader*							m_pShader = nullptr;
	CVIBuffer_Rect*						m_pVIBuffer = nullptr;
	_float4x4							m_WorldMatrix{}, m_ViewMatrix{}, m_ProjMatrix{};
	_uint								m_iOriginViewportWidth{}, m_iOriginViewportHeight{};
	map<_wstring, ID3D11DepthStencilView*> m_DSVs;

private:
	map<_int, CRenderGroup*>			m_RenderGroups;

public:
	CONST_IBL Get_GlobalIBLData() const { return m_tGlobalIBLData; }
	void Set_GlobalIBLData(const CONST_IBL& _tGlobalIBLData, _bool _isUpdateConstBuffer = false);
private:
	CONST_IBL							m_tGlobalIBLData = {};
	ID3D11Buffer*						m_pGlobalIBLConstBuffer = nullptr;

#ifdef _DEBUG
private:
	list<CComponent*> m_DebugComponents;
	_bool m_isDebugRender = true;
	HRESULT Render_Debug();
#endif // debug component
private:
	HRESULT Ready_DepthStencilView(const _wstring _strDSVTag, _uint _iWidth, _uint _iHeight);
public:
	static CNewRenderer*				Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void						Free() override;
};
END
