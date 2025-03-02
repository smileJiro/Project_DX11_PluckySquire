#include "NewRenderer.h"
#include "GameInstance.h"
#include "GameObject.h"

#include "RenderGroup_Shadow.h"

CNewRenderer::CNewRenderer(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:m_pDevice(_pDevice)
	,m_pContext(_pContext)
	,m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CNewRenderer::Initialize()
{
	_uint iNumViewport = 1;
	D3D11_VIEWPORT ViewportDesc = {};

	m_pContext->RSGetViewports(&iNumViewport, &ViewportDesc);
	m_iOriginViewportWidth = (_uint)ViewportDesc.Width;
	m_iOriginViewportHeight = (_uint)ViewportDesc.Height;

	/* 직교 투영으로 그리기 위한 Shader, VIBuffer, Matrix Init */
	m_pShader = CShader::Create(m_pDevice, m_pContext, TEXT("../../EngineSDK/Hlsl/Shader_Deferred.hlsl"), VTXPOSTEX::Elements, VTXPOSTEX::iNumElements);
	if (nullptr == m_pShader)
		return E_FAIL;

	m_pVIBuffer = CVIBuffer_Rect::Create(m_pDevice, m_pContext);
	if (nullptr == m_pVIBuffer)
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixScaling(ViewportDesc.Width, ViewportDesc.Height, 1.f));
	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(ViewportDesc.Width, ViewportDesc.Height, 0.f, 1.f));


	/* Create IBL Const Buffer */
	if (FAILED(m_pGameInstance->CreateConstBuffer(m_tGlobalIBLData, D3D11_USAGE_DYNAMIC, &m_pGlobalIBLConstBuffer)))
		return E_FAIL;

	/* Default 값으로 Shader에 바인딩 */
	if(FAILED(m_pShader->Bind_ConstBuffer("GlobalIBLConstData", m_pGlobalIBLConstBuffer)))
		return E_FAIL;

	/* Shadow RenderGroup 생성. */
	CRenderGroup_Shadow::RG_DESC RG_ShadowDesc;
	RG_ShadowDesc.iRenderGroupID = REDNERGROUP_SHADOWID;
	RG_ShadowDesc.iPriorityID = 0;
	CRenderGroup_Shadow* pRenderGroup_Shdow = CRenderGroup_Shadow::Create(m_pDevice, m_pContext, &RG_ShadowDesc);
	if (nullptr == pRenderGroup_Shdow)
	{
		MSG_BOX("Failed Create RenderGroup_Shadow(NewRenderer Class)");
		return E_FAIL;
	}
	if (FAILED(Add_RenderGroup(pRenderGroup_Shdow->Get_RenderGroupID(), pRenderGroup_Shdow->Get_PriorityID(), pRenderGroup_Shdow)))
		return E_FAIL;

	Safe_Release(pRenderGroup_Shdow);
	pRenderGroup_Shdow = nullptr;




	m_pShader->Bind_RawValue("g_vHideColor", &m_vPlayerHideColor, sizeof(_float3));
	return S_OK;
}

HRESULT CNewRenderer::Add_RenderObject(_int _iGroupID, _int _iPriorityID, CGameObject* _pGameObject)
{
	CRenderGroup* pRenderGroup = Find_RenderGroup(_iGroupID, _iPriorityID);
	if (nullptr == pRenderGroup)
		return E_FAIL;

	if (FAILED(pRenderGroup->Add_RenderObject(_pGameObject)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNewRenderer::Draw_RenderObject()
{
	for (auto& Pair : m_RenderGroups)
	{
		if(true == Pair.second->Is_Active())
			Pair.second->Render(m_pShader, m_pVIBuffer);
	}

#ifdef NDEBUG
	if (true == m_isDebugRender)
	{
		if (FAILED(Render_Debug()))
		{
			MSG_BOX("Render Failed Render_Debug");
			return E_FAIL;
		}

		m_pGameInstance->Physx_Render();
	}

	if (true == m_isDebugBase)
	{
		if (FAILED(Render_Base_Debugs()))
		{
			MSG_BOX("Render Failed Render_Base_Debug");
			return E_FAIL;
		}

	}

	if (KEY_DOWN(KEY::F6))
	{
		m_isDebugRender ^= 1;
		m_pGameInstance->Set_Physx_DebugRender(m_isDebugRender);
		if (KEY_PRESSING(KEY::ALT))
		{
			m_isDebugBase ^= 1;
		}
	}

#endif

	return S_OK;
}

HRESULT CNewRenderer::Add_RenderGroup(_int _iGroupID, _int _iPriorityID, CRenderGroup* _pRenderGroup)
{
	if (nullptr != Find_RenderGroup(_iGroupID, _iPriorityID))
	{
		MSG_BOX("비상!!! 렌더러 렌더그룹 중복 키값 발생!");
		return E_FAIL;
	}
	
	if (nullptr == _pRenderGroup)
	{
		MSG_BOX("비상!!! 렌더그룹 nullptr을 등록하려했어!");
		return E_FAIL;
	}
		
	_int iKey = _iGroupID + _iPriorityID;
	m_RenderGroups.emplace(iKey, _pRenderGroup);
	Safe_AddRef(_pRenderGroup);

	return S_OK;
}

CRenderGroup* CNewRenderer::Find_RenderGroup(_int _iGroupID, _int _iPriorityID)
{
	_int iKey = _iGroupID + _iPriorityID;
	auto& iter = m_RenderGroups.find(iKey);

	if (iter == m_RenderGroups.end())
		return nullptr;

	return (*iter).second;
}

HRESULT CNewRenderer::Erase_RenderGroup(_int _iGroupID, _int _iPriorityID)
{
	_int iKey = _iGroupID + _iPriorityID;
	auto& iter = m_RenderGroups.find(iKey);
	if (iter == m_RenderGroups.end())
	{
		MSG_BOX("현재 렌더그룹에 존재하지 않는 렌더그룹 삭제요청함.");
		return E_FAIL;
	}
	Safe_Release(iter->second);
	m_RenderGroups.erase(iter);

	return S_OK;
}

void CNewRenderer::Set_Active_RenderGroup(_int _iGroupID, _int _iPriorityID, _bool _isActive)
{
	CRenderGroup* pRenderGroup = Find_RenderGroup(_iGroupID, _iPriorityID);
	if (nullptr == pRenderGroup)
		return;

	pRenderGroup->Set_Active(_isActive);
}

ID3D11DepthStencilView* CNewRenderer::Find_DSV(const _wstring& _strDSVTag)
{
	auto& iter = m_DSVs.find(_strDSVTag);
	if (iter == m_DSVs.end())
		return nullptr;

	return (*iter).second;
}

HRESULT CNewRenderer::Add_DSV(const _wstring _strDSVTag, _float2 _vDSVSize)
{
	if(FAILED(Ready_DepthStencilView(_strDSVTag, (_uint)_vDSVSize.x, (_uint)_vDSVSize.y)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNewRenderer::Add_DSV(const _wstring _strDSVTag, _uint _iWidth, _uint _iHeight)
{
	if (FAILED(Ready_DepthStencilView(_strDSVTag, _iWidth, _iHeight)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNewRenderer::Add_DSV(const _wstring _strDSVTag, ID3D11DepthStencilView* _pDSV)
{
	if (nullptr != Find_DSV(_strDSVTag))
		return E_FAIL;

	m_DSVs.emplace(_strDSVTag, _pDSV);
	Safe_AddRef(_pDSV);

	return S_OK;
}

HRESULT CNewRenderer::Erase_DSV(const _wstring _strDSVTag)
{
	auto& iter = m_DSVs.find(_strDSVTag);
	if (iter == m_DSVs.end())
	{
		MSG_BOX("컨테이너에 없는 DSV를 삭제요청했음.");
		return E_FAIL;
	}

	Safe_Release((*iter).second);
	m_DSVs.erase(iter);

	return S_OK;
}

HRESULT CNewRenderer::Add_ShadowLight(CLight* _pShadowLight)
{
	CRenderGroup_Shadow* pRenderGroup_Shadow = static_cast<CRenderGroup_Shadow*>(Find_RenderGroup(REDNERGROUP_SHADOWID, 0));
	if (nullptr == pRenderGroup_Shadow)
		return E_FAIL;

	return pRenderGroup_Shadow->Add_ShadowLight(_pShadowLight);
}

HRESULT CNewRenderer::Remove_ShadowLight(_int _iShadowLightID)
{
	CRenderGroup_Shadow* pRenderGroup_Shadow = static_cast<CRenderGroup_Shadow*>(Find_RenderGroup(REDNERGROUP_SHADOWID, 0));
	if (nullptr == pRenderGroup_Shadow)
		return E_FAIL;

	return pRenderGroup_Shadow->Remove_ShadowLight(_iShadowLightID);
}

HRESULT CNewRenderer::Clear_ShadowLight()
{
	CRenderGroup_Shadow* pRenderGroup_Shadow = static_cast<CRenderGroup_Shadow*>(Find_RenderGroup(REDNERGROUP_SHADOWID, 0));
	if (nullptr == pRenderGroup_Shadow)
		return E_FAIL;

	return pRenderGroup_Shadow->Clear_ShadowLight();
}

HRESULT CNewRenderer::Bind_DofConstBuffer(const _char* _szConstBufferName, ID3D11Buffer* _pConstBuffer)
{
	if(FAILED(m_pShader->Bind_ConstBuffer(_szConstBufferName, _pConstBuffer)))
		return E_FAIL;

	return S_OK;
}

HRESULT CNewRenderer::Load_IBL(const _wstring& _strIBLJsonPath)
{
	const std::string filePathIBL = m_pGameInstance->WStringToString(_strIBLJsonPath);
	std::ifstream inputFile(filePathIBL);
	if (!inputFile.is_open()) {
		throw std::runtime_error("파일을 열 수 없습니다: " + filePathIBL);
		return E_FAIL;
	}
	json jsonIBL;
	inputFile >> jsonIBL;
	CONST_IBL tIBLConstData = {};

	tIBLConstData.fExposure = jsonIBL["fExposure"];
	tIBLConstData.fGamma = jsonIBL["fGamma"];
	tIBLConstData.fHDRMaxLuminance = jsonIBL["fHDRMaxLuminance"];
	tIBLConstData.fRoughnessToMipFactor = jsonIBL["fRoughnessToMipFactor"];
	tIBLConstData.fStrengthIBL = jsonIBL["fStrengthIBL"];
	tIBLConstData.iSpecularBaseMipLevel = jsonIBL["iSpecularBaseMipLevel"];
	tIBLConstData.iToneMappingFlag = jsonIBL["iToneMappingFlag"];

	Set_GlobalIBLData(tIBLConstData, true);

	inputFile.close();
	return S_OK;
}

#ifdef NDEBUG
void CNewRenderer::Update_Imgui()
{
	for (auto& Pair : m_RenderGroups)
	{
		Pair.second->Update_Imgui();
	}
}
#endif // _DEBUG



void CNewRenderer::Set_GlobalIBLData(const CONST_IBL& _tGlobalIBLData, _bool _isUpdateConstBuffer)
{
	m_tGlobalIBLData = _tGlobalIBLData;

	if (true == _isUpdateConstBuffer)
	{
		m_pGameInstance->UpdateConstBuffer(m_tGlobalIBLData, m_pGlobalIBLConstBuffer);
		m_pShader->Bind_ConstBuffer("GlobalIBLConstData", m_pGlobalIBLConstBuffer);
	}
}

void CNewRenderer::Set_PlayerHideColor(const _float3 _vPlayerHideColor, _bool _isUpdate)
{
	m_vPlayerHideColor = _vPlayerHideColor;

	if(true == _isUpdate)
		m_pShader->Bind_RawValue("g_vHideColor", &m_vPlayerHideColor, sizeof(_float3));
}


#ifdef NDEBUG

HRESULT CNewRenderer::Render_Debug()
{
	for (auto& pDebugCom : m_DebugComponents)
	{
		if (nullptr != pDebugCom)
			pDebugCom->Render();

		Safe_Release(pDebugCom);
	}

	m_DebugComponents.clear();

	return S_OK;
}

HRESULT CNewRenderer::Render_Base_Debugs()
{
	for (auto& pBase : m_BaseDebugs)
	{
		if (nullptr != pBase)
			pBase->Render_Base_Debug();

		Safe_Release(pBase);
	}

	m_BaseDebugs.clear();

	return S_OK;
}

#endif // _DEBUG

HRESULT CNewRenderer::Ready_DepthStencilView(const _wstring _strDSVTag, _uint _iWidth, _uint _iHeight)
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	ID3D11Texture2D* pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	/* RTV의 사이즈와 DSV의 사이즈가 일치 되어야한다. */
	TextureDesc.Width = _iWidth;
	TextureDesc.Height = _iHeight;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;

	ID3D11DepthStencilView* pDSV = nullptr;
	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &pDSV)))
		return E_FAIL;

	Add_DSV(_strDSVTag, pDSV);


	Safe_Release(pDepthStencilTexture);
	Safe_Release(pDSV);
	return S_OK;
}

CNewRenderer* CNewRenderer::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CNewRenderer* pInstance = new CNewRenderer(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX("Failed Create CNewRenderer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CNewRenderer::Free()
{
	for (auto& Pair : m_RenderGroups)
	{
		Safe_Release(Pair.second);
	}
	m_RenderGroups.clear();

	for (auto& Pair : m_DSVs)
	{
		Safe_Release(Pair.second);
	}
	m_DSVs.clear();
#ifdef NDEBUG
	for (auto& pDebugComponent : m_DebugComponents)
	{
		Safe_Release(pDebugComponent);
	}
	m_DebugComponents.clear();
	for (auto& pBase : m_BaseDebugs)
	{
		Safe_Release(pBase);
	}
	m_BaseDebugs.clear();
	
#endif // _DEBUG

	Safe_Release(m_pGlobalIBLConstBuffer);
	Safe_Release(m_pVIBuffer);
	Safe_Release(m_pShader);
	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	__super::Free();
}
