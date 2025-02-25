#pragma once
#include "Base.h"
BEGIN(Engine)
class CRenderTarget;
class CTexture;
class CGameInstance;
class CModelObject;
END
BEGIN(Client)


class CMap_2D final : public CBase
{
private:
	CMap_2D(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext);
	virtual ~CMap_2D() = default;

public:
	HRESULT	Initialize(const _wstring _strSectionTag, const _wstring _strTextureName, _float2 _fRenderTargetSize, _int _iPriorityID);

private: /* 외부 요청을 통해 Register할지, 생성시점에 무조건 Register 할지, 그냥 Section채로 그릴지 Renderer 작업하면서 생각 필요. */
	// 1. RenderTarget을 Target_Manager에 등록하는 기능.(DSV는 어떻게할지 생각해보자.) (x)
	HRESULT Register_RTV_ToTargetManager();
	HRESULT Register_RenderGroup_ToRenderer();
	// 2. Texture의 SRV를 RenderTarget의 RTV로 Copy하는 기능. (x)

public:
	// 3. RenderTarget의 RTV를 Get (o)
	ID3D11RenderTargetView*		Get_RTV_FromRenderTarget();
	// 4. RenderTarget의 SRV를 Get (o)
	ID3D11ShaderResourceView*	Get_SRV_FromRenderTarget();


	HRESULT						Clear_Map();
	HRESULT						Copy_DefaultMap_ToRenderTarget();
	HRESULT						Create_Default_MapTexture();

public:
	HRESULT						Register_WorldCapture(CModelObject* _pModel);
	void						Set_WorldTexture(ID3D11Texture2D* _pTexture) { Safe_Release(m_pWorldTexture); m_pWorldTexture = _pTexture; }
	ID3D11Texture2D*			Get_WorldTexture() { return m_pWorldTexture; }

public:
	ID3D11ShaderResourceView*	Get_SRV_FromTexture(_uint _iTextureIndex);
	_int						Get_PriorityID() { return m_iPriorityID; };
	
	const _wstring&				Get_RT_Key() { return m_strRTKey; }
	const _wstring&				Get_DSV_Key() { return m_strDSVKey; }
	const _wstring&				Get_MRT_Key() { return m_strMRTKey; }

	const _float2&				Get_RenderTarget_Size() { return m_fRenderTargetSize; }


private:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;
	CGameInstance*				m_pGameInstance = nullptr;




private:
	
	CRenderTarget*				m_pRenderTarget = nullptr;
	ID3D11DepthStencilView*		m_pDSV = nullptr;				// Map 별로 별도의 DSV를 가진다.
	ID3D11Texture2D*			m_pWorldTexture = nullptr;


	_wstring					m_strSectionTag;
	_wstring					m_strTextureName;
	_wstring					m_strRTKey;
	_wstring					m_strDSVKey;
	_wstring					m_strMRTKey;

	_wstring					m_strWorldRTKey = L"";
	_int						m_iWorldRenderGroupID;
	_int						m_iWorldPriorityID;
	

	
	CTexture*					m_pTextureCom = nullptr;		// Map Origin Texture;
	_int						m_iPriorityID = 0;
	_float2						m_fRenderTargetSize = {};
public:
	static CMap_2D* Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _wstring _strSectionTag, const _wstring _strTextureName, _float2 _fRenderTargetSize, _int _iPriorityID);
	void Free() override;
};
END
