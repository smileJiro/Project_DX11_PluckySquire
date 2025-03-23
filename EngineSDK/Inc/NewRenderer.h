#pragma once
#include "Base.h"

BEGIN(Engine)
class CGameInstance;
class CGameObject;
class CComponent;
class CShader;
class CVIBuffer_Rect;
class CRenderGroup;
class CLight;
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
	void								Set_Active_RenderGroup(_int _iGroupID, _int _iPriorityID, _bool _isActive);
public: /* DSV */
	ID3D11DepthStencilView*				Find_DSV(const _wstring& _strDSVTag);
	HRESULT								Add_DSV(const _wstring _strDSVTag, _float2 _vDSVSize);
	HRESULT								Add_DSV(const _wstring _strDSVTag, _uint _iWidth, _uint _iHeight);
	HRESULT								Add_DSV(const _wstring _strDSVTag, ID3D11DepthStencilView* _pDSV);
	HRESULT								Erase_DSV(const _wstring _strDSVTag);

public: /* Shadow Light */
	HRESULT								Add_ShadowLight(CLight* _pShadowLight);
	HRESULT								Remove_ShadowLight(_int _iShadowLightID);
	HRESULT								Clear_ShadowLight();

public:/* Bind Defferd ConstBufferData */
	HRESULT								Bind_DofConstBuffer(const _char* _szConstBufferName, ID3D11Buffer* _pConstBuffer);
public:
	HRESULT								Load_IBL(const _wstring& _strIBLJsonPath);
#ifdef _DEBUG

public:
	void Update_Imgui();
public: 
	HRESULT Add_DebugComponent(CComponent* _pDebugCom)
	{
		if (false == m_isDebugRender)
			return S_OK;

		m_DebugComponents.push_back(_pDebugCom);
		Safe_AddRef(_pDebugCom);
		return S_OK;
	}

	HRESULT Add_BaseDebug(CBase* _pBaseDebug)
	{
		if (false == m_isDebugBase)
			return S_OK;

		m_BaseDebugs.push_back(_pBaseDebug);
		Safe_AddRef(_pBaseDebug);
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

	/* Shader Const Data */
public:
	CONST_IBL Get_GlobalIBLData() const { return m_tGlobalIBLData; }
	void Set_GlobalIBLData(const CONST_IBL& _tGlobalIBLData, _bool _isUpdateConstBuffer = false);
	void Set_PlayerHideColor(const _float3 _vHideColor, _bool _isUpdate = false);
private:
	CONST_IBL							m_tGlobalIBLData = {};
	ID3D11Buffer*						m_pGlobalIBLConstBuffer = nullptr;

private: /* Player Hide */
	_float3								m_vPlayerHideColor = { 1.0f, 0.24f, 0.4666f };
	_float3								m_vOtherHideColor = { 0.8f,0.8f ,0.8f };

public:
	void Set_VtxMesh(CShader* _pShader) { Safe_Release(m_pVtxMesh); m_pVtxMesh = _pShader; Safe_AddRef(m_pVtxMesh); }
	void Set_VtxAnimMesh(CShader* _pShader) { Safe_Release(m_pVtxAnimMesh); m_pVtxAnimMesh = _pShader; Safe_AddRef(m_pVtxAnimMesh); }
	void Set_VtxPosTex(CShader* _pShader) { 
		Safe_Release(m_pVtxPosTex); m_pVtxPosTex = _pShader; Safe_AddRef(m_pVtxPosTex); 
	}
	void Set_GrayScale_VtxMesh(_int _isGrayScale);
	void Set_GrayScale_VtxAnimMesh(_int _isGrayScale);
	void Set_GrayScale_VtxPosTex(_int _isGrayScale);

private: /* VtxMesh, VtxAnimMesh Shader */
	_int m_isGray_VtxMesh = false;
	_int m_isGray_VtxAnimMesh = false;
	_int m_isGray_VtxPosTex = false;
	CShader* m_pVtxMesh = nullptr;
	CShader* m_pVtxAnimMesh = nullptr;
	CShader* m_pVtxPosTex = nullptr;
#ifdef _DEBUG
private:
	list<CComponent*> m_DebugComponents;
	list<CBase*> m_BaseDebugs;
	_bool m_isDebugRender = true;
	_bool m_isDebugBase = true;
	HRESULT Render_Debug();
	HRESULT Render_Base_Debugs();
#endif // debug component
private:
	HRESULT Ready_DepthStencilView(const _wstring _strDSVTag, _uint _iWidth, _uint _iHeight);
public:
	static CNewRenderer*				Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual void						Free() override;
};
END
