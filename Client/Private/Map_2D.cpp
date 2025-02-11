#include "stdafx.h"
#include "Map_2D.h"
#include "GameInstance.h"
#include "RenderTarget.h"
#include "RenderGroup_2D.h"
#include "RenderGroup_WorldPos.h"
#include "ModelObject.h"
#include "3DMapSpskObject.h"
#include "Section_Manager.h"

CMap_2D::CMap_2D(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:m_pDevice(_pDevice)
	,m_pContext(_pContext)
	,m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
	Safe_AddRef(m_pGameInstance);
}

HRESULT CMap_2D::Initialize(const _wstring _strTextureName, _float2 _fRenderTargetSize, _int _iPriorityID)
{
	// 0. Map Data Parsing
	// 1. CRenderTarget Create
	// 2. Create DSV & Register DSV To Renderer (Renderer 작업하면서 고민 필요.)
	// 3. Register_RTV_ToTargetManager();
	// 4. Resister_RenderGroup_ToRenderer();
	// 5. Texture Component Clone


	// 이거, 사실 내가 렌더타겟을 직접 가질필요 없는거아닌가? 
	// 렌더그룹 나누어둔 이상, 아이디만 바인드하면 되니까.
	// 아이디 관리를 해줘야 하겠지만?
	// 어차피 섹션에서 렌더그룹을 아이디로 지정하잖아., 게임오브젝트를
	// 그럼 어차피 렌더러가 자동으로 렌더그룹 찾아서, 렌더타겟 바인딩 해줄건데
	// 굳이 왜 내가 렌더타겟을 가지고 있어야?
	// 섹션이 정리될 때, 맵에서 빼준다면 생성이 되고..
	// 발급받은 렌더그룹 코드를 섹션에 저장해둔다면?
	
	// 어차피 렌더타겟을 '생성' 하는 건 타겟매니저에 이관해도 무방하다는거지


	// 근데 이거  저장을 따로해둬야 카피할때,  편하겠네. 제기랄 
	// 귀찮은데 생성한 렌더타겟 등록하는거.
	// 어차피 같은 맵의 rt / dsv가 여러개 생성될 리 없으니, 키로 타겟매니저에 생성 요청
	
	/* RenderTarget Create */
	size_t iDotIndex = _strTextureName.find_last_of(L".");
	if (iDotIndex != _wstring::npos)
		m_strTextureName = _strTextureName.substr(0, iDotIndex);
	else 
		m_strTextureName = _strTextureName;

	m_strRTKey = L"Target_" + m_strTextureName;
	m_strDSVKey = L"DSV_" + m_strTextureName;
	m_strMRTKey = L"MRT_" + m_strTextureName;
	m_iPriorityID = _iPriorityID;
	m_fRenderTargetSize = _fRenderTargetSize;

	if (FAILED(Create_Default_MapTexture()))
		return E_FAIL;
	if (FAILED(Register_RTV_ToTargetManager()))
		return E_FAIL;
	if (FAILED(Register_RenderGroup_ToRenderer()))
		return E_FAIL;

	Safe_AddRef(m_pDSV);
	Safe_AddRef(m_pRenderTarget);

	return S_OK;
}

HRESULT CMap_2D::Register_RTV_ToTargetManager()
{
	// Renderer 수정 후 작업 예정.
	if (FAILED(m_pGameInstance->Add_RenderTarget(m_strRTKey, (_uint)m_fRenderTargetSize.x, (_uint)m_fRenderTargetSize.y, DXGI_FORMAT_B8G8R8A8_UNORM, _float4(0.f, 0.f, 0.f, 1.f), &m_pRenderTarget)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_DSV_ToRenderer(m_strDSVKey, (_uint)m_fRenderTargetSize.x, (_uint)m_fRenderTargetSize.y)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(m_strMRTKey, m_strRTKey)))
		return E_FAIL;
	return S_OK;
}

HRESULT CMap_2D::Register_RenderGroup_ToRenderer()
{

	CRenderGroup_2D* pRenderGroup_MRT = nullptr;

	CRenderGroup_2D::RG_2D_DESC RG_Map2DDesc;
	RG_Map2DDesc.iRenderGroupID = RENDERGROUP::RG_2D;
	RG_Map2DDesc.iPriorityID = m_iPriorityID;
	RG_Map2DDesc.isViewportSizeChange = true;
	RG_Map2DDesc.strMRTTag = m_strMRTKey;
	RG_Map2DDesc.fRenderTargetSize = m_fRenderTargetSize;
	m_pDSV = RG_Map2DDesc.pDSV = m_pGameInstance->Find_DSV(m_strDSVKey);

	RG_Map2DDesc.vViewportSize = m_fRenderTargetSize;
	RG_Map2DDesc.isClear = false;
	if (nullptr == RG_Map2DDesc.pDSV)
		return E_FAIL;
	pRenderGroup_MRT = CRenderGroup_2D::Create(m_pDevice, m_pContext, &RG_Map2DDesc);
	if (nullptr == pRenderGroup_MRT)
	{
		MSG_BOX("Failed Create PR2D_Map2D");
		return E_FAIL;
	}
	if (FAILED(m_pGameInstance->Add_RenderGroup(pRenderGroup_MRT->Get_RenderGroupID(), pRenderGroup_MRT->Get_PriorityID(), pRenderGroup_MRT)))
		return E_FAIL;

	Safe_Release(pRenderGroup_MRT);
	pRenderGroup_MRT = nullptr;

	return S_OK;
}

HRESULT CMap_2D::Clear_Map()
{
	return m_pGameInstance->Clear_MRT(m_strMRTKey, m_pDSV);
}

HRESULT CMap_2D::Copy_DefaultMap_ToRenderTarget()
{
	ID3D11Resource* pSrcResource = nullptr; 
	ID3D11Resource* pDestResource = m_pRenderTarget->Get_Resource();
	m_pTextureCom->Get_SRV(0)->GetResource(&pSrcResource);
	
	if (nullptr != pSrcResource && nullptr != pDestResource)
		m_pContext->CopyResource(pDestResource, pSrcResource);
	Safe_Release(pSrcResource);

	return S_OK;
}

HRESULT CMap_2D::Create_Default_MapTexture()
{
	_wstring strPath = L"../Bin/Resources/Textures/Map/" + m_strTextureName + L".dds";
	m_pTextureCom = CTexture::Create(m_pDevice, m_pContext, strPath.c_str());
	
	if (nullptr == m_pTextureCom)
		return E_FAIL;
	return S_OK;
}

HRESULT CMap_2D::Register_Capcher_WorldTexture(C3DMapSpskObject* _pModel)
{
	_wstring strWorldRVTag = m_strRTKey + L"_WorldPosMap";
	_wstring strWorldMRTTag = m_strMRTKey + L"_WorldPosMap";
	_wstring strWorldDSVTag = m_strDSVKey + L"_WorldPosMap";

	_uint iRenderGroupID = RG_WORLDPOSMAP;
	_uint iPriorityID = SECTION_MGR->Generate_WorldPos_Priority_ID();

	/* Target_WorldPosMap_Book */
	if (FAILED(m_pGameInstance->Add_RenderTarget(strWorldRVTag, (_uint)m_fRenderTargetSize.x, (_uint)m_fRenderTargetSize.y, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(strWorldMRTTag, strWorldRVTag)))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_DSV_ToRenderer(strWorldDSVTag, (_uint)m_fRenderTargetSize.x, (_uint)m_fRenderTargetSize.y)))
		return E_FAIL;
	CRenderGroup_WorldPos::RG_MRT_DESC Desc;
	Desc.iRenderGroupID = iRenderGroupID;
	Desc.iPriorityID = iPriorityID;
	Desc.isClear = false;
	Desc.isViewportSizeChange = true;
	Desc.pDSV = m_pGameInstance->Find_DSV(strWorldDSVTag);
	Desc.strMRTTag = strWorldMRTTag;
	Desc.vViewportSize = { m_fRenderTargetSize.x , m_fRenderTargetSize.y };

	CRenderGroup_WorldPos* pRenderGroup_WorldPos = CRenderGroup_WorldPos::Create(m_pDevice, m_pContext, &Desc);
	if (nullptr == pRenderGroup_WorldPos)
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderGroup(pRenderGroup_WorldPos->Get_RenderGroupID(),
		pRenderGroup_WorldPos->Get_PriorityID(), pRenderGroup_WorldPos)))
		return E_FAIL;
	Safe_Release(pRenderGroup_WorldPos);

	D3D11_TEXTURE2D_DESC desc = {};
	desc.Width = (_uint)m_fRenderTargetSize.x; // 원본 텍스처 너비
	desc.Height = (_uint)m_fRenderTargetSize.y; // 원본 텍스처 높이
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; // 원본 텍스처와 동일한 포맷
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_STAGING; // CPU 읽기 전용
	desc.BindFlags = 0; // 바인딩 없음
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	m_pDevice->CreateTexture2D(&desc, nullptr, &m_pWorldTexture);

	m_pGameInstance->Add_RenderObject_New(iRenderGroupID, iPriorityID, _pModel);

	m_strWorldRTKey = strWorldRVTag;

	return S_OK;
}

ID3D11RenderTargetView* CMap_2D::Get_RTV_FromRenderTarget()
{
	if (nullptr == m_pRenderTarget)
		return nullptr;

	return m_pRenderTarget->Get_RTV();
}

ID3D11ShaderResourceView* CMap_2D::Get_SRV_FromRenderTarget()
{
	if (nullptr == m_pRenderTarget)
		return nullptr;

	return m_pRenderTarget->Get_SRV();
}

ID3D11ShaderResourceView* CMap_2D::Get_SRV_FromTexture(_uint _iTextureIndex)
{
	if (nullptr == m_pTextureCom)
		return nullptr;

	return m_pTextureCom->Get_SRV(_iTextureIndex);
}

CMap_2D* CMap_2D::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, const _wstring _strTextureName, _float2 _fRenderTargetSize, _int _iPriorityID)
{
	CMap_2D* pInstance = new CMap_2D(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_strTextureName, _fRenderTargetSize, _iPriorityID)))
	{
		MSG_BOX("Failed Create CMap_2D");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CMap_2D::Free()
{
	m_pGameInstance->Erase_RenderGroup_New(RENDERGROUP::RG_2D,m_iPriorityID);
	m_pGameInstance->Erase_DSV_ToRenderer(m_strDSVKey);
	// TODO :: 0131 박예슬 m_pGameInstance->rt, mrt 삭제필요
	m_pGameInstance->Erase_RenderTarget(m_strRTKey);
	m_pGameInstance->Erase_MRT(m_strMRTKey);
	Safe_Release(m_pDSV);
	Safe_Release(m_pRenderTarget);
	Safe_Release(m_pTextureCom);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	__super::Free();
}
