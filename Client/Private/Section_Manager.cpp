#include "stdafx.h"
#include "Section_Manager.h"
#include "GameInstance.h"

#include "RenderGroup_WorldPos.h"
#include "Section_2D_PlayMap_Book.h"
#include "Section_2D_PlayMap_Sksp.h"
#include "Section_2D_Narration.h"
#include "Section_2D_MiniGame_Pip.h"
#include "UI_Manager.h"


IMPLEMENT_SINGLETON(CSection_Manager)
CSection_Manager::CSection_Manager()
	:m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CSection_Manager::Initialize(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	if (nullptr == _pDevice)
		return E_FAIL;
	if (nullptr == _pContext)
		return E_FAIL;

	m_pDevice = _pDevice;
	Safe_AddRef(m_pDevice);

	m_pContext = _pContext;
	Safe_AddRef(m_pContext);


	/* Target_WorldPosMap_Book */
	if (FAILED(m_pGameInstance->Add_RenderTarget(TEXT("Target_WorldPosMap_Book"), (_uint)RTSIZE_BOOK2D_X, (_uint)RTSIZE_BOOK2D_Y, DXGI_FORMAT_R32G32B32A32_FLOAT, _float4(0.f, 0.f, 0.f, 1.f))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_MRT(TEXT("MRT_WorldPosMap_Book"), TEXT("Target_WorldPosMap_Book"))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_DSV_ToRenderer(TEXT("DSV_WorldPosMap_Book"), RTSIZE_BOOK2D_X, RTSIZE_BOOK2D_Y)))
		return E_FAIL;

	CRenderGroup_WorldPos::RG_WORLDPOS_DESC Desc;
	Desc.iRenderGroupID = RG_WORLDPOSMAP;
	Desc.iPriorityID = PRWORLD_MAINBOOK;
	Desc.isClear = false;
	Desc.isViewportSizeChange = true;
	Desc.pDSV = m_pGameInstance->Find_DSV(TEXT("DSV_WorldPosMap_Book"));
	Desc.strMRTTag = TEXT("MRT_WorldPosMap_Book");
	Desc.strRTTag = TEXT("Target_WorldPosMap_Book");
	Desc.strSectionTag = TEXT("");
	Desc.vViewportSize = { RTSIZE_BOOK2D_X , RTSIZE_BOOK2D_Y };
	CRenderGroup_WorldPos* pRenderGroup_WorldPos = CRenderGroup_WorldPos::Create(m_pDevice, m_pContext, &Desc);
	if (nullptr == pRenderGroup_WorldPos)
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_RenderGroup(pRenderGroup_WorldPos->Get_RenderGroupID(), pRenderGroup_WorldPos->Get_PriorityID(), pRenderGroup_WorldPos)))
		return E_FAIL;
	Safe_Release(pRenderGroup_WorldPos);


	m_pWordGameGenerator = CWordGame_Generator::Create(m_pDevice, m_pContext);
	if (m_pWordGameGenerator == nullptr)
		return E_FAIL;

	return S_OK;
}
/// <summary>
/// _iNextChangeLevelID가 -1일 때, 리소스를 전부 로드한 뒤의 _iChangeLevelID레벨로 이동할 시점이라고 판단 
/// </summary>
/// <param name="_iChangeLevelID"></param>
/// <param name="_iNextChangeLevelID"></param>
HRESULT CSection_Manager::Level_Exit(_int _iChangeLevelID, _int _iNextChangeLevelID)
{
	m_iCurLevelID = (LEVEL_ID)_iChangeLevelID;

	Safe_Release(m_pBookWorldPosMap);
	m_pBookWorldPosMap = nullptr;
	Clear_Sections();

	return S_OK;
}

HRESULT CSection_Manager::Level_Enter(_int _iChangeLevelID)
{
	m_iLoadLevel = LEVEL_END == m_iLoadLevel ? m_iCurLevelID : m_iLoadLevel;
	
	/* 전환되는 레벨에 따른 Section 설정 필요. */
	_wstring strJsonPath = L"";
	_wstring strMapInfoJson = L"_Data";
	_wstring strModelInfoJsonName = L"MapObject_TagMatchingData";
	_wstring strWordJsonName = L"_Word_List";




	_bool isSectionLoading = false;
	switch (m_iLoadLevel)
	{
	case Client::LEVEL_CHAPTER_2:
		strJsonPath = L"Chapter2";
		isSectionLoading = true;
		break;
	case Client::LEVEL_CHAPTER_4:
		strJsonPath = L"Chapter4";
		isSectionLoading = true;
		break;
	case Client::LEVEL_CHAPTER_6:
		strJsonPath = L"Chapter6";
		isSectionLoading = true;
		break;
	case Client::LEVEL_CHAPTER_8:
		strJsonPath = L"Chapter8";
		isSectionLoading = true;
		break;
	case Client::LEVEL_CAMERA_TOOL: // 효림 임시...
		strJsonPath = L"Chapter8";
		isSectionLoading = true;
		break;
	default:
		return E_FAIL;
	}

	if (isSectionLoading)
	{

		if (nullptr != m_pWordGameGenerator)
		{
			if (FAILED(m_pWordGameGenerator->Load_Word(strJsonPath + strWordJsonName)))
				return E_FAIL;
		}
		if (FAILED(Ready_CurLevelSectionModels(strModelInfoJsonName)))
			return E_FAIL;

		if (FAILED(Ready_CurLevelSections(strJsonPath + strMapInfoJson)))
			return E_FAIL;

	}

	m_iLoadLevel = LEVEL_END;
	return S_OK;
}


CSection* CSection_Manager::Find_Section(const _wstring& _strSectionTag)
{
	auto& iter = m_CurLevelSections.find(_strSectionTag);

	if (iter == m_CurLevelSections.end())
		return nullptr;

	return (*iter).second;
}

HRESULT CSection_Manager::Remove_Section(const _wstring& _strSectionTag)
{
	// 현재 맨 위에 활성화중인 책 섹션은 지울 수 없도록.
	if (m_pCurSection->Get_SectionName() == _strSectionTag)
		return E_FAIL;

	auto pSection = Find_Section(_strSectionTag);

	// 없는 섹션 Remove 호출
	if (nullptr == pSection)
		return E_FAIL;


	if (FAILED(pSection->CleanUp_Section()))
		return E_FAIL;


	m_CurLevelSections.erase(_strSectionTag);
	
	Safe_Release(pSection);

	return S_OK;
}


HRESULT CSection_Manager::Add_GameObject_ToSectionLayer(const _wstring& _strSectionTag, CGameObject* _pGameObject, _uint _iLayerIndex)
{
	CSection* pSection = Find_Section(_strSectionTag);
	if (nullptr == pSection)
		return E_FAIL;

	if (FAILED(pSection->Add_GameObject_ToSectionLayer(_pGameObject, _iLayerIndex)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSection_Manager::Change_GameObject_LayerIndex(const _wstring& _strSectionTag, CGameObject* _pGameObject, _uint _iLayerIndex)
{
	CSection* pSection = Find_Section(_strSectionTag);
	if (nullptr == pSection)
		return E_FAIL;

	if (FAILED(pSection->Change_GameObject_LayerIndex(_pGameObject, _iLayerIndex)))
		return E_FAIL;

	return S_OK;
}


HRESULT CSection_Manager::Remove_GameObject_FromSectionLayer(const _wstring& _strSectionTag, CGameObject* _pGameObject)
{
	CSection* pSection = Find_Section(_strSectionTag);
	if (nullptr == pSection)
		return E_FAIL;

	if (FAILED(pSection->Remove_GameObject_ToSectionLayer(_pGameObject)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSection_Manager::Add_GameObject_ToCurSectionLayer(CGameObject* _pGameObject, _uint _iLayerIndex)
{
	if (nullptr == m_pCurSection)
		return E_FAIL;
	return m_pCurSection->Add_GameObject_ToSectionLayer(_pGameObject, _iLayerIndex);
}

HRESULT CSection_Manager::Remove_GameObject_ToCurSectionLayer(CGameObject* _pGameObject)
{
	if (nullptr == m_pCurSection)
		return E_FAIL;
	return m_pCurSection->Remove_GameObject_ToSectionLayer(_pGameObject);
}


HRESULT CSection_Manager::SetActive_Section(CSection* _pSection, _bool _isActive)
{
	if (_pSection->Is_Active() != _isActive)
	{
		_pSection->Set_Active(_isActive);
		_pSection->SetActive_GameObjects(_isActive);


		if (_isActive)
			m_pGameInstance->Register_Section(_pSection->Get_SectionName());
		else
			m_pGameInstance->Unregister_Section(_pSection->Get_SectionName());
	}



	return S_OK;
}

HRESULT CSection_Manager::SetActive_Section(const _wstring& _strSectionTag, _bool _isActive)
{
	CSection* pSection = Find_Section(_strSectionTag);
	if (nullptr == pSection)
		return E_FAIL;
	return SetActive_Section(pSection, _isActive);
}


HRESULT CSection_Manager::Section_AddRenderGroup_Process()
{
	for (auto& SectionPair : m_CurLevelSections)
	{
		if (nullptr == SectionPair.second || !SectionPair.second->Is_Active())
			continue;

		CSection_2D* pSection2D = static_cast<CSection_2D*>(SectionPair.second);
		// 텍스쳐 복사
		if (FAILED(pSection2D->Copy_DefaultMap_ToRenderTarget()))
			return E_FAIL;
	}

	m_pGameInstance->Get_ThreadPool()->EnqueueJob([this]()
		{
			for (auto& SectionPair : m_CurLevelSections)
			{
				if (nullptr == SectionPair.second || !SectionPair.second->Is_Active())
					continue;

				//CSection_2D* pSection2D = static_cast<CSection_2D*>(SectionPair.second);
				SectionPair.second->Section_AddRenderGroup_Process();
			}
			
		});

	return S_OK;
}


_float2 CSection_Manager::Get_Section_RenderTarget_Size(const _wstring _strSectionKey)
{
	CSection* pSection = Find_Section(_strSectionKey);
	if (nullptr == pSection)
		return _float2(-1.f, -1.f);
	CSection_2D* pSection_2D = dynamic_cast<CSection_2D*>(pSection);

	if (nullptr == pSection_2D)
		return _float2(-1.f, -1.f);
	return pSection_2D->Get_RenderTarget_Size();
}


HRESULT CSection_Manager::Change_CurSection(const _wstring _strSectionKey)
{
	// Clear_Section
	//m_pGameInstance->Register_Section();
	CSection* pTargetSection = Find_Section(_strSectionKey);

	if (nullptr != pTargetSection)
	{
		_wstring strPreSectionTag = L"";
		// 현재 활성화된 섹션이 있는지 검사한다.
		if (nullptr != m_pCurSection)
		{
			strPreSectionTag = m_pCurSection->Get_SectionName();
			//활성화된 섹션 제거 로직을 돌린다.
			if (FAILED(SetActive_Section(m_pCurSection, false)))
				return E_FAIL;
			m_pCurSection->Section_Exit(_strSectionKey);
			//TODO:: CollMgr 정리로직 추가 필요
		}

		// 섹션 2D로 생성하여, 현재 활성화된 페이지로 등록한다.
		CSection_2D* pSection_2D = dynamic_cast<CSection_2D*>(pTargetSection);

		if (nullptr == pSection_2D)
			return E_FAIL;
		m_pCurSection = pSection_2D;

		m_pCurSection->Section_Enter(strPreSectionTag);
		Main_Section_Active_Process(m_pCurSection->Get_SectionName());



		if (CSection_2D::SECTION_2D_PLAY_TYPE::NARRAION == pSection_2D->Get_Section_2D_PlayType())
		{
			CSection_2D_Narration* pNarrationSection = static_cast<CSection_2D_Narration*>(pSection_2D);

			if (nullptr == pNarrationSection)
				return S_OK;

			pNarrationSection->Start_Narration();
		}

		return S_OK;
	}
	else
		return E_FAIL;
}


_bool CSection_Manager::Is_CurSection(CGameObject* _pGameObject)
{
	if (nullptr != m_pCurSection)
		return m_pCurSection->Is_CurSection(_pGameObject);
	return false;
}


const _wstring* CSection_Manager::Get_SectionKey(CGameObject* _pGameObject)
{
	const _wstring* pReturn = nullptr;
	for (auto& SectionPair : m_CurLevelSections)
	{
		auto pLayer = SectionPair.second->Get_Include_Layer(_pGameObject);
		if (nullptr != pLayer)
			return &SectionPair.second->Get_SectionName();
	}
	return pReturn;
}

_vector CSection_Manager::Get_WorldPosition_FromWorldPosMap(const _wstring& _strSectionTag, _float2 _v2DTransformPosition)
{
	CSection* pSection = Find_Section(_strSectionTag);

	if (nullptr == pSection)
		return _vector();

	CSection_2D* p2DSection = dynamic_cast<CSection_2D*>(pSection);

	if (nullptr == p2DSection)
		return _vector();

	ID3D11Texture2D* pTexture2D = p2DSection->Get_WorldTexture();

	if (nullptr == pTexture2D)
		return Get_WorldPosition_FromWorldPosMap(_v2DTransformPosition);
	else
		return Get_WorldPosition_FromWorldPosMap(pTexture2D, _v2DTransformPosition);
}

/// <summary>
/// 책 렌더타겟을 언맵하여 책 렌더타겟 기준 좌표에서 3D 공간의 World Position을 획득한다.
/// </summary>
/// <param name="_v2DTransformPosition">책 렌더타겟 기준 좌표(Proj 좌표)</param>
_vector CSection_Manager::Get_WorldPosition_FromWorldPosMap(_float2 _v2DTransformPosition)
{
	return Get_WorldPosition_FromWorldPosMap(m_pBookWorldPosMap, _v2DTransformPosition);
}

_vector CSection_Manager::Get_WorldPosition_FromWorldPosMap(ID3D11Texture2D* _pTargetTexture, _float2 _v2DTransformPosition)
{
	if (nullptr == _pTargetTexture)
		return _vector();

	// 맵핑하여 데이터 접근
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(m_pContext->Map(_pTargetTexture, 0, D3D11_MAP_READ, 0, &mappedResource)))
		return _vector();

	// 2D Transform 위치를 픽셀 좌표계로 변환. 해당 텍스쳐의 가로 세로 사이즈를 알아야함.
	_int iWidth = mappedResource.RowPitch / sizeof(_float) / 4;
	_int iHeight = mappedResource.DepthPitch / mappedResource.RowPitch;

	// 원하는 픽셀 좌표 (픽셀 좌표는 UV 좌표를 변환해서 계산)
	_int iPixelX = (_int)std::ceil(_v2DTransformPosition.x) + iWidth / 2;            // X 좌표
	_int iPixelY = (_int)(std::ceil(_v2DTransformPosition.y) * -1.0f) + iHeight / 2; // Y 좌표

	// RowPitch는 한 줄의 바이트 수를 나타냄
	_float* fData = static_cast<_float*>(mappedResource.pData);

	// 픽셀 위치 계산 (4 floats per pixel)
	_int rowPitchInPixels = mappedResource.RowPitch / sizeof(_float) / 4;
	_int iIndex = iPixelY * rowPitchInPixels + iPixelX;

	if (iWidth * iHeight <= iIndex || 0 > iIndex)
		return XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);


	_uint iDefaultIndex = iIndex * 4;

	// float4 데이터 읽기
	_float x = fData[iDefaultIndex]; // Red 채널
	_float y = fData[iDefaultIndex + 1]; // Green 채널
	_float z = fData[iDefaultIndex + 2]; // Blue 채널
	_float w = fData[iDefaultIndex + 3]; // Alpha 채널

	//if (0.f == x &&
	//    0.f == y &&
	//    0.f == z
	//    )
	//{
	//    _vector vLerpPos = XMVectorLerp(XMVectorSet(
	//        fData[iDefaultIndex - 4],
	//        fData[iDefaultIndex - 3],
	//        fData[iDefaultIndex - 2],
	//        fData[iDefaultIndex - 1]
	//    ),
	//        XMVectorSet(
	//            fData[iDefaultIndex + 4],
	//            fData[iDefaultIndex + 5],
	//            fData[iDefaultIndex + 6],
	//            fData[iDefaultIndex + 7]
	//        ), 0.5f);

	//    x = XMVectorGetX(vLerpPos);
	//    y = XMVectorGetY(vLerpPos);
	//    z = XMVectorGetZ(vLerpPos);
	//}


	// 맵핑 해제
	m_pContext->Unmap(_pTargetTexture, 0);

	return XMVectorSet(x, y, z, w);
}

ID3D11RenderTargetView* CSection_Manager::Get_RTV_FromRenderTarget(const _wstring& _strSectionTag)
{
	/* Section 2D 인경우만 사용가능하다 */
	/* Get RTV, SRV 모두 프레임당 호출횟수가 많지 않을 것으로 예상되어 일단 이렇게 처리 함.*/
	/* 추후 성능적 개선이 필요하다면 section에 TypeID를 부여 */
	CSection_2D* pSection_2D = dynamic_cast<CSection_2D*>(Find_Section(_strSectionTag));
	if (nullptr == pSection_2D)
		return nullptr;

	return pSection_2D->Get_RTV_FromRenderTarget();
}

ID3D11ShaderResourceView* CSection_Manager::Get_SRV_FromRenderTarget(const _wstring& _strSectionTag)
{
	CSection_2D* pSection_2D = dynamic_cast<CSection_2D*>(Find_Section(_strSectionTag));
	if (nullptr == pSection_2D)
		return nullptr;

	return pSection_2D->Get_SRV_FromRenderTarget();
}


ID3D11ShaderResourceView* CSection_Manager::Get_SRV_FromRenderTarget()
{
	CSection_2D* pSection_2D = dynamic_cast<CSection_2D*>(m_pCurSection);
	if (nullptr == pSection_2D)
		return nullptr;

	return pSection_2D->Get_SRV_FromRenderTarget();
}
//뭐지 이거 필요한가
ID3D11ShaderResourceView* CSection_Manager::Get_SRV_FromTexture(const _wstring& _strSectionTag, _uint _iTextureIndex)
{
	CSection_2D* pSection_2D = dynamic_cast<CSection_2D*>(Find_Section(_strSectionTag));
	if (nullptr == pSection_2D)
		return nullptr;

	return pSection_2D->Get_SRV_FromTexture(_iTextureIndex);
}

void CSection_Manager::Set_BookWorldPosMapTexture(ID3D11Texture2D* _pBookWorldPosMap)
{
	if (nullptr != m_pBookWorldPosMap)
		Safe_Release(m_pBookWorldPosMap);

	m_pBookWorldPosMap = _pBookWorldPosMap;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(m_pContext->Map(m_pBookWorldPosMap, 0, D3D11_MAP_READ, 0, &mappedResource)))
		return;
	// 2D Transform 위치를 픽셀 좌표계로 변환. 해당 텍스쳐의 가로 세로 사이즈를 알아야함.
	m_vBookWorldPixelSize.x = mappedResource.RowPitch / sizeof(_float) / (_float)4;
	m_vBookWorldPixelSize.y = (_float)mappedResource.DepthPitch / (_float)mappedResource.RowPitch;

	_uint iLeftBotIndex = (_uint)m_vBookWorldPixelSize.x * ((_uint)m_vBookWorldPixelSize.y - 1) * 4;
	_uint iRightTopIndex = ((_uint)m_vBookWorldPixelSize.x - 2) * 4;
	// float4 데이터 읽기
	_float* fData = static_cast<_float*>(mappedResource.pData);
	m_vBookWorldMin = { fData[iLeftBotIndex],fData[iLeftBotIndex + 1],fData[iLeftBotIndex + 2] };
	m_vBookWorldMax = { fData[iRightTopIndex],fData[iRightTopIndex + 1],fData[iRightTopIndex + 2] };
	m_pContext->Unmap(m_pBookWorldPosMap, 0);
	for (auto& SectionPair : m_CurLevelSections)
	{
		if (nullptr != SectionPair.second)
		{
			CSection_2D* pSection2D = static_cast<CSection_2D*>(SectionPair.second);
			if (pSection2D->Get_Section_2D_RenderType() == CSection_2D::SECTION_2D_BOOK && !pSection2D->Is_Override_WorldTex())
			{
				pSection2D->Set_WorldTexture(m_pBookWorldPosMap);
				Safe_AddRef(m_pBookWorldPosMap);
			}
		}
	}

}

HRESULT CSection_Manager::Word_Action_To_Section(const _wstring& _strSectionTag, _uint _iControllerIndex, _uint _iContainerIndex, _uint _iWordType, _bool _isRegistered)
{
	CSection_2D* pSection_2D = dynamic_cast<CSection_2D*>(Find_Section(_strSectionTag));
	if (nullptr == pSection_2D)
		return E_FAIL;

	return pSection_2D->Word_Action_To_Section(_strSectionTag, _iControllerIndex, _iContainerIndex, _iWordType, _isRegistered);
}

HRESULT CSection_Manager::Register_WorldCapture(const _wstring& _strSectionTag, CModelObject* _pObject)
{
	CSection_2D* pSection_2D = dynamic_cast<CSection_2D*>(Find_Section(_strSectionTag));
	if (nullptr == pSection_2D)
		return E_FAIL;

	return pSection_2D->Register_WorldCapture(_pObject);
}

void CSection_Manager::Main_Section_Active_Process(const _wstring& _strSectionTag)
{
	CSection* pCurSection = Find_Section(_strSectionTag);
	if (nullptr == pCurSection)
		return;

	if (FAILED(SetActive_Section(pCurSection, true)))
		return;

	CSection_2D* pSection_2D = dynamic_cast<CSection_2D*>(pCurSection);
	if (nullptr == pSection_2D)
		return;

	_wstring strPageTag;
	CSection_2D* pTargetSection = nullptr;
	if (pSection_2D->Has_NextPage(strPageTag))
	{
		pTargetSection = static_cast<CSection_2D*>(Find_Section(strPageTag));
		pTargetSection->Copy_DefaultMap_ToRenderTarget();
	}

	if (pSection_2D->Has_PrePage(strPageTag))
	{
		pTargetSection = static_cast<CSection_2D*>(Find_Section(strPageTag));
		pTargetSection->Copy_DefaultMap_ToRenderTarget();
	}
}

void CSection_Manager::Clear_Sections()
{
	for (auto& Pair : m_CurLevelSections)
		Safe_Release(Pair.second);
	m_CurLevelSections.clear();
	m_pCurSection = nullptr;
	m_pGameInstance->Clear_GroupFilter();
}

HRESULT CSection_Manager::Ready_CurLevelSectionModels(const _wstring& _strJsonPath)
{
	m_2DModelInfos.clear();
	_wstring strChapterPath = MAP_2D_DEFAULT_PATH;
	strChapterPath += L"ComponentTagMatching/";
	strChapterPath += _strJsonPath + L".json";

	json ChapterJson;
	std::ifstream inputFile(strChapterPath);
	if (!inputFile.is_open()) {
		throw std::runtime_error("json Error :  " + WstringToString(strChapterPath));
		return E_FAIL;
	}

	inputFile >> ChapterJson;
	if (ChapterJson.is_array())
	{
		_uint iModelSize = (_uint)ChapterJson.size();
		m_2DModelInfos.resize(iModelSize);
		_uint iIndex = 0;
		for (auto ChildJson : ChapterJson)
		{
			m_2DModelInfos[iIndex].strModelName = ChildJson["TextureName"];
			m_2DModelInfos[iIndex].eModelType = ChildJson["ModelType"];
			m_2DModelInfos[iIndex].isActive = ChildJson["Active"];
			m_2DModelInfos[iIndex].isCollider = ChildJson["Collider"];
			m_2DModelInfos[iIndex].isSorting = ChildJson["Sorting"];
			m_2DModelInfos[iIndex].isBackGround = ChildJson["BackGround"];

			if (m_isActive &&
				ChildJson.contains("ActivePropertis") &&
				ChildJson["ActivePropertis"].contains("ActiveType"))
			{
				m_2DModelInfos[iIndex].eActiveType = ChildJson["ActivePropertis"]["ActiveType"];
			}

			if (m_2DModelInfos[iIndex].isCollider && ChildJson.contains("ColliderPropertis"))
			{
				auto ColliderPropertis = ChildJson["ColliderPropertis"];
				if (ColliderPropertis.contains("ColliderType"))
					m_2DModelInfos[iIndex].eColliderType = ColliderPropertis["ColliderType"];
				if (ColliderPropertis.contains("ColliderInfo"))
				{
					auto ColliderInfo = ColliderPropertis["ColliderInfo"];

					if (ColliderInfo.contains("Collider_Offset_Pos"))
						m_2DModelInfos[iIndex].fCollider_Offset_Pos = { ColliderInfo["Collider_Offset_Pos"]["X"],ColliderInfo["Collider_Offset_Pos"]["Y"] };
					if (ColliderInfo.contains("Collider_Extent"))
						m_2DModelInfos[iIndex].fCollider_Extent = { ColliderInfo["Collider_Extent"]["X"],ColliderInfo["Collider_Extent"]["Y"] };
					if (ColliderInfo.contains("Collider_Radius"))
						m_2DModelInfos[iIndex].fCollider_Radius = ColliderInfo["Collider_Radius"];

				}
			}
			if (m_2DModelInfos[iIndex].isCollider && ChildJson.contains("SortingPropertis"))
			{
				auto SortingPropertis = ChildJson["SortingPropertis"];
				if (SortingPropertis.contains("Sorting_Offset_Pos"))
					m_2DModelInfos[iIndex].fSorting_Offset_Pos = { SortingPropertis["Sorting_Offset_Pos"]["X"],SortingPropertis["Sorting_Offset_Pos"]["Y"] };
			}
			iIndex++;
		}
	}

	return S_OK;
}

HRESULT CSection_Manager::Ready_CurLevelSections(const _wstring& _strJsonPath)
{
	m_CurLevelSections.clear();
	_wstring strChapterPath = MAP_2D_DEFAULT_PATH;
	strChapterPath += _strJsonPath + L".json";

	json ChapterJson;
	std::ifstream inputFile(strChapterPath);
	if (!inputFile.is_open()) {
		throw std::runtime_error("json Error :  " + WstringToString(strChapterPath));
		return E_FAIL;
	}

	inputFile >> ChapterJson;

	_wstring strStartSectionKey = L"";

	if (ChapterJson.is_array())
	{
		_uint iIndex = 0;
		for (auto ChildJson : ChapterJson)
		{
			_bool isStart = false;

			if (!ChildJson.contains("Section_Type"))
				continue;
			if (ChildJson.contains("Section_Start"))
				isStart = true;
			CSection_2D::SECTION_2D_PLAY_TYPE eType = ChildJson["Section_Type"];


			CSection_2D* pSection = nullptr;


			// TODO :: SECTION CREATE
			switch (eType)
			{
			case Client::CSection_2D::PLAYMAP:
			{
				CSection_2D_PlayMap_Book::SECTION_2D_PLAYMAP_DESC Desc = {};
				Desc.isImport = true;
				Desc.iPriorityID = m_iPriorityGenKey;
				Desc.SectionJson = ChildJson;
				pSection = CSection_2D_PlayMap_Book::Create(m_pDevice, m_pContext, &Desc);
				if (nullptr == pSection)
				{
					MSG_BOX("Failed Create CSection_2D");
					return E_FAIL;
				}
				if (FAILED(SetActive_Section(pSection, false)))
					return E_FAIL;
				if (pSection->Is_Rotation())
				{
					CGameObject* pGameObject = m_pGameInstance->Get_GameObject_Ptr(m_iCurLevelID, L"Layer_Book", 0);

					if (nullptr != pGameObject)
					{
						pSection->Register_WorldCapture((CModelObject*)pGameObject);
					}
				}
			}
			break;
			case Client::CSection_2D::NARRAION:
			case Client::CSection_2D::WORLDMAP:
			{
				CSection_2D_Narration::SECTION_2D_NARRATION_DESC Desc = {};
				Desc.isImport = true;
				Desc.iPriorityID = m_iPriorityGenKey;
				Desc.SectionJson = ChildJson;
				// TODO :: 0316 박예슬 나레이션-월드맵 수정요청 
				pSection = CSection_2D_Narration::Create(m_pDevice, m_pContext, eType, &Desc);
				//pSection = CSection_2D_Narration::Create(m_pDevice, m_pContext, CSection_2D::NARRAION, &Desc);
				if (nullptr == pSection)
				{
					MSG_BOX("Failed Create CNarration_2D");
					return E_FAIL;
				}
				if (FAILED(SetActive_Section(pSection, false)))
					return E_FAIL;

			}
			break;
			case Client::CSection_2D::SKSP:
			{
				CSection_2D_PlayMap_Sksp::SECTION_2D_DESC Desc = {};
				Desc.isImport = true;
				Desc.iPriorityID = m_iPriorityGenKey;
				Desc.SectionJson = ChildJson;
				pSection = CSection_2D_PlayMap_Sksp::Create(m_pDevice, m_pContext, &Desc);
				if (nullptr == pSection)
				{
					MSG_BOX("Failed Create CSection_2D_PlayMap_Sksp");
					return E_FAIL;
				}
				if (FAILED(SetActive_Section(pSection, true)))
					return E_FAIL;
			}
			break;
			case Client::CSection_2D::MINIGAME:
			{
				if (!ChildJson.contains("MiniGame_Info"))
					return E_FAIL;


				CSection_2D_MiniGame::SECTION_2D_MINIGAME_DESC Desc = {};
				Desc.iPriorityID = m_iPriorityGenKey;
				Desc.SectionJson = ChildJson;
				Desc.isImport = true;


				auto& SectionMiniGameInfo = ChildJson["MiniGame_Info"];

				_string strMiniGameName = "";

				if (SectionMiniGameInfo.contains("MiniGame_Type")
					&& SectionMiniGameInfo["MiniGame_Type"].is_number_unsigned())
				{
					Desc.eMiniGameType = SectionMiniGameInfo["MiniGame_Type"];
				}
				if (SectionMiniGameInfo.contains("MiniGame_Name")) 
				{
					strMiniGameName = SectionMiniGameInfo["MiniGame_Name"];
					Desc.strMiniGameName = StringToWstring(strMiniGameName);
				}

				switch (Desc.eMiniGameType)
				{
				case CSection_2D_MiniGame::SECTION_2D_MINIGAME_TYPE::PIP:
				{
					pSection = CSection_2D_MiniGame_Pip::Create(m_pDevice, m_pContext, &Desc);
					break;
				}
				default:
					break;
				}


				if (nullptr == pSection)
				{
					MSG_BOX("Failed Create CSection_2D_PlayMap_Sksp");
					return E_FAIL;
				}
				if (FAILED(SetActive_Section(pSection, true)))
					return E_FAIL;
			}
			break;
			case Client::CSection_2D::SECTION_2D_PLAY_TYPE_LAST:
			default:
			{
				MSG_BOX("Failed - Section_Manager Section Create Logic Error");
				return E_FAIL;
			}
			}

			m_iPriorityGenKey += 10;


			if (m_CurLevelSections.empty() || isStart)
				strStartSectionKey = pSection->Get_SectionName();
			m_CurLevelSections.try_emplace(pSection->Get_SectionName(), pSection);

			if (nullptr != pSection && ChildJson.contains("MiniGame_Info"))
			{
				auto& MiniGameJson = ChildJson["MiniGame_Info"];

				if (
					MiniGameJson.contains("MiniGame_Type")
					&&
					MiniGameJson.contains("MiniGame_Name")
					)
				{
					_uint eType = MiniGameJson["MiniGame_Type"];
					_string strName = MiniGameJson["MiniGame_Name"];

					if (eType == CSection_2D_MiniGame::WORD)
					{
						m_pWordGameGenerator->WordGame_Generate(pSection, StringToWstring(strName));
					}
				}
			}

			iIndex++;
		}
	}


	Change_CurSection(strStartSectionKey);

	return S_OK;
}

void CSection_Manager::Free()
{
	Clear_Sections();

	Safe_Release(m_pBookWorldPosMap);
	Safe_Release(m_pWordGameGenerator);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	__super::Free();
}