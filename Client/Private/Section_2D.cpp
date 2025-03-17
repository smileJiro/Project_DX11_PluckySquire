#include "stdafx.h"
#include "Section_2D.h"
#include "GameInstance.h"
#include "2DMapObject.h"
#include "Map_2D.h"
#include "Section_Manager.h"
#include "Engine_Macro.h"
#include "Trigger_Manager.h"
#include "2DMapWordObject.h"

CSection_2D::CSection_2D(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, SECTION_2D_PLAY_TYPE _ePlayType, SECTION_2D_RENDER_TYPE _eRenderType)
	:	m_eMySectionPlayType(_ePlayType),
		m_eMySectionRenderType(_eRenderType),
		CSection(_pDevice, _pContext)
{
}

HRESULT CSection_2D::Initialize(void* _pDesc)
{
	SECTION_2D_DESC* pDesc = static_cast<SECTION_2D_DESC*>(_pDesc);

	if (_pDesc == nullptr)
		return E_FAIL;
	// 초기화를위한 정보채우기
	if (pDesc->isImport)
		if (FAILED(Import(_pDesc)))
			return E_FAIL;


	m_fRenderResolution = pDesc->fRenderResolution;
	m_fLevelSizePixels = pDesc->fLevelSizePixels;
	pDesc->iLayerGroupCount = SECTION_2D_PLAYMAP_RENDERGROUP_LAST;
	// 객체초기화...
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;
	// 맵생성...
	if (FAILED(Ready_Map_2D(pDesc->strMapName)))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CSection_2D::Import(void* _pDesc)
{
#pragma region Section_2D Info Desc Import && Initialize

	SECTION_2D_DESC* pDesc = static_cast<SECTION_2D_DESC*>(_pDesc);

	// 섹션 이름 Read
	if (pDesc->SectionJson.contains("Section_Name"))
	{
		_string strText = pDesc->SectionJson["Section_Name"];
		m_strName = StringToWstring(strText);
	}

	if (pDesc->SectionJson.contains("Section_BackGround_Name"))
	{
		pDesc->strMapName = StringToWstring(pDesc->SectionJson["Section_BackGround_Name"]);
	}
	// Section Default Bulid Read
	if (pDesc->SectionJson.contains("Section_Default"))
	{
		_uint eSectionDefault = pDesc->SectionJson["Section_Default"];

		pDesc->Book_2D_Build(m_strName);
		if (pDesc->SectionJson.contains("Section_Info"))
		{
			auto SectionInfo = pDesc->SectionJson["Section_Info"];
			if (SectionInfo.contains("Camera_Zoom_Ratio"))
				m_fCameraRatio = (_float)SectionInfo["Camera_Zoom_Ratio"];
		}

	}
	// Default 설정값이 없다? 
	// Section_Info 객체 내부의 커스텀 정보 읽기.
	else if (pDesc->SectionJson.contains("Section_Info"))
	{
		pDesc->Book_2D_Build(m_strName);

		auto SectionInfo = pDesc->SectionJson["Section_Info"];
		if (SectionInfo.contains("Rotation"))
			m_isRotation = SectionInfo["Rotation"];

		if (SectionInfo.contains("Scrolling"))
			m_isScrolling = SectionInfo["Scrolling"];
		
		if (SectionInfo.contains("Platformer"))
			m_isPlatformer = SectionInfo["Platformer"];
		
		if (SectionInfo.contains("Override_Normal"))
			m_iOverride_Normal = SectionInfo["Override_Normal"];

		if (SectionInfo.contains("Camera_Zoom_Ratio"))
			m_fCameraRatio = (_float)SectionInfo["Camera_Zoom_Ratio"];

		if (SectionInfo.contains("RenderResolution"))
		{
			pDesc->fRenderResolution.x = SectionInfo["RenderResolution"]["X"];
			pDesc->fRenderResolution.y = SectionInfo["RenderResolution"]["Y"];
		}
		if (SectionInfo.contains("LevelSizePixels"))
		{
			pDesc->fLevelSizePixels.x = SectionInfo["LevelSizePixels"]["X"];
			pDesc->fLevelSizePixels.y = SectionInfo["LevelSizePixels"]["Y"];
		}

	}
	else 
	{
		MSG_BOX("Section Info Not Exist! : Section_2D ");
		return E_FAIL;
	}


	switch (m_eMySectionPlayType)
	{
		// 250317 박상욱 (월드맵이 끝났을 시 안넘어가는 현상 관련) 월드맵 추가
		case Client::CSection_2D::WORLDMAP:
		case Client::CSection_2D::NARRAION:
		case Client::CSection_2D::PLAYMAP:
		case Client::CSection_2D::MINIGAME:
		{
			if (pDesc->SectionJson.contains("Section_Info"))
			{
				auto SectionInfo = pDesc->SectionJson["Section_Info"];
				if (SectionInfo.contains("Next_Page_Tag"))
				{
					_string strNextPageTag = SectionInfo["Next_Page_Tag"];
					m_strNextPageTag = StringToWstring(strNextPageTag);
				}
				if (SectionInfo.contains("Pre_Page_Tag"))
				{
					_string strPrePageTag = SectionInfo["Pre_Page_Tag"];
					m_strPrePageTag = StringToWstring(strPrePageTag);
				}
			}
		}
		break;
		case Client::CSection_2D::SKSP:
		{
			auto SectionInfo = pDesc->SectionJson["Section_Info"];

			if (SectionInfo.contains("RenderResolution"))
			{
				pDesc->fRenderResolution.x = SectionInfo["RenderResolution"]["X"];
				pDesc->fRenderResolution.y = SectionInfo["RenderResolution"]["Y"];
			}
			if (SectionInfo.contains("LevelSizePixels"))
			{
				pDesc->fLevelSizePixels.x = SectionInfo["LevelSizePixels"]["X"];
				pDesc->fLevelSizePixels.y = SectionInfo["LevelSizePixels"]["Y"];
			}

			pDesc->fRenderResolution.x *= RATIO_BOOK2D_X;
			pDesc->fRenderResolution.y *= RATIO_BOOK2D_Y;
		}
		break;
		default:
			break;
	}
	return S_OK;
}

HRESULT CSection_2D::Copy_DefaultMap_ToRenderTarget()
{
	if (FAILED(m_pMap->Clear_Map()))
		return E_FAIL;
	return m_pMap->Copy_DefaultMap_ToRenderTarget();
}
ID3D11RenderTargetView* CSection_2D::Get_RTV_FromRenderTarget()
{
	if (nullptr == m_pMap)
		return nullptr;

	return m_pMap->Get_RTV_FromRenderTarget();
}

ID3D11ShaderResourceView* CSection_2D::Get_SRV_FromRenderTarget()
{
	if (nullptr == m_pMap)
		return nullptr;

	return m_pMap->Get_SRV_FromRenderTarget();
}

ID3D11ShaderResourceView* CSection_2D::Get_SRV_FromTexture(_uint _iTextureIndex)
{
	if (nullptr == m_pMap)
		return nullptr;

	return m_pMap->Get_SRV_FromTexture(_iTextureIndex);
}

_float2 CSection_2D::Get_RenderTarget_Size()
{
	if (nullptr == m_pMap)
		return _float2(-1,-1);

	return m_pMap->Get_RenderTarget_Size();
}

HRESULT CSection_2D::Word_Action_To_Section(const _wstring& _strSectionTag, _uint _iControllerIndex, _uint _iContainerIndex, _uint _iWordType, _bool _isRegistered)
{

	CLayer* pTargetLayer = m_Layers[SECTION_2D_PLAYMAP_WORDOBJ];
	if (nullptr != pTargetLayer)
	{
		const auto& GameObjects = pTargetLayer->Get_GameObjects();

		if (_isRegistered)
		{
			C2DMapWordObject* pExecuteGameObject = nullptr;
			for_each(GameObjects.begin(), GameObjects.end(), [&pExecuteGameObject, &_iControllerIndex, &_iContainerIndex, &_iWordType](CGameObject* pGameObject) {
				C2DMapWordObject* pWordObj = dynamic_cast<C2DMapWordObject*>(pGameObject);

				if (true == pWordObj->Check_Action(_iControllerIndex, _iContainerIndex, _iWordType))
					pExecuteGameObject = pWordObj;
				});

			if (nullptr != pExecuteGameObject)
			{
				pExecuteGameObject->Register_Action(_iControllerIndex, _iContainerIndex, _iWordType);
				// Action_Execute에서 실제 액션 발생 코드 뺀다,
				// 이벤트 발생.
				// param - pExecuteGameObject, _iControllerIndex,  _iContainerIndex,  _iWordType

				// pExecuteGameObject에 카메라 이동, 0.3초정도 인터벌 뒤 액션 실행 코드 삽입,
				//						//이를 위해서, Check_Action()을 추가해서 액션 실행 코드랑 검사 코드를 분리해도 좋을 듯.

				//	이후 다시 플레이어 타겟(무조건 2D로 가정)
			}
		}
		else 
		{
			for_each(GameObjects.begin(), GameObjects.end(), [&_iControllerIndex, &_iContainerIndex, &_iWordType](CGameObject* pGameObject) {
				C2DMapWordObject* pWordObj = dynamic_cast<C2DMapWordObject*>(pGameObject);

				if (nullptr != pWordObj)
					pWordObj->Action_Execute(_iControllerIndex, _iContainerIndex, _iWordType);
				});
		}
	}

	return S_OK;
}

HRESULT CSection_2D::Register_WorldCapture(CModelObject* _pModel)
{
	if (nullptr == m_pMap)
		return E_FAIL;
	m_isOverride_WorldTex = true;
	return m_pMap->Register_WorldCapture(_pModel);
}

HRESULT CSection_2D::Ready_Map_2D(_wstring _strMapFIleName)
{
	m_pMap = CMap_2D::Create(m_pDevice, m_pContext, m_strName, _strMapFIleName, m_fRenderResolution, m_iPriorityID);
	if (nullptr == m_pMap)
		return E_FAIL;

	return S_OK;
}

void CSection_2D::Free()
{
	Safe_Release(m_pMap);

	__super::Free();
}
