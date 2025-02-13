#include "stdafx.h"
#include "Section_2D.h"
#include "GameInstance.h"
#include "2DMapObject.h"
#include "Map_2D.h"
#include "Section_Manager.h"
#include "Engine_Macro.h"
#include "Trigger_Manager.h"

CSection_2D::CSection_2D(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, SECTION_2D_PLAY_TYPE _ePlayType, SECTION_2D_RENDER_TYPE _eRenderType)
	:	m_eMySectionPlayType(_ePlayType),
		m_eMySectionRenderType(_eRenderType),
		CSection(_pDevice, _pContext)
{
}

HRESULT CSection_2D::Initialize(SECTION_2D_DESC* _pDesc, _uint _iPriorityKey)
{
	if (_pDesc == nullptr)
		return E_FAIL;

	m_fRenderResolution = _pDesc->fRenderResolution;
	m_fLevelSizePixels = _pDesc->fLevelSizePixels;
	_pDesc->iLayerGroupCount = SECTION_2D_PLAYMAP_RENDERGROUP_LAST;
	if (FAILED(__super::Initialize(_pDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSection_2D::Import(json _SectionJson, _uint _iPriorityKey)
{
#pragma region Section_2D Info Desc Import && Initialize

	CSection_2D::SECTION_2D_DESC Desc;

	Desc.iGroupID = RG_2D;
	Desc.iPriorityID = _iPriorityKey;

	// 섹션 이름 Read
	if (_SectionJson.contains("Section_Name"))
	{
		_string strText = _SectionJson["Section_Name"];
		m_strName = StringToWstring(strText);
	}

	// Section Default Bulid Read
	if (_SectionJson.contains("Section_Default"))
	{
		_uint eSectionDefault = _SectionJson["Section_Default"];

		Desc.Book_2D_Build(m_strName);

	}
	// Default 설정값이 없다? 
	// Section_Info 객체 내부의 커스텀 정보 읽기.
	else if (_SectionJson.contains("Section_Info"))
	{
		Desc.Book_2D_Build(m_strName);

		auto SectionInfo = _SectionJson["Section_Info"];
		if (SectionInfo.contains("Rotation"))
		{
			m_isRotation = SectionInfo["Rotation"];
		}
		if (SectionInfo.contains("RenderResolution"))
		{
			Desc.fRenderResolution.x = SectionInfo["RenderResolution"]["X"];
			Desc.fRenderResolution.y = SectionInfo["RenderResolution"]["Y"];
		}
		if (SectionInfo.contains("LevelSizePixels"))
		{
			Desc.fLevelSizePixels.x = SectionInfo["LevelSizePixels"]["X"];
			Desc.fLevelSizePixels.y = SectionInfo["LevelSizePixels"]["Y"];
		}

	}
	else 
	{
		MSG_BOX("Section Info Not Exist! : Section_2D ");
		return E_FAIL;
	}


	switch (m_eMySectionPlayType)
	{
		case Client::CSection_2D::NARRAION:
		case Client::CSection_2D::PLAYMAP:
		{
			if (_SectionJson.contains("Section_Info"))
			{
				auto SectionInfo = _SectionJson["Section_Info"];
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
			auto SectionInfo = _SectionJson["Section_Info"];

			if (SectionInfo.contains("RenderResolution"))
			{
				Desc.fRenderResolution.x = SectionInfo["RenderResolution"]["X"];
				Desc.fRenderResolution.y = SectionInfo["RenderResolution"]["Y"];
			}
			if (SectionInfo.contains("LevelSizePixels"))
			{
				Desc.fLevelSizePixels.x = SectionInfo["LevelSizePixels"]["X"];
				Desc.fLevelSizePixels.y = SectionInfo["LevelSizePixels"]["Y"];
			}

			Desc.fRenderResolution.x *= RATIO_BOOK2D_X;
			Desc.fRenderResolution.y *= RATIO_BOOK2D_Y;
		}
		break;
		default:
			break;
	}



	if (FAILED(Initialize(&Desc, _iPriorityKey)))
		return E_FAIL;

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

const _wstring CSection_2D::Get_WorldRenderTarget_Tag()
{
	if (nullptr == m_pMap)
		return L"";

	return m_pMap->Get_WorldRenderTarget_Tag();
}

HRESULT CSection_2D::Register_Capcher_WorldTexture(C3DMapSkspObject* _pModel)
{
	if (nullptr == m_pMap)
		return E_FAIL;

	return m_pMap->Register_Capcher_WorldTexture(_pModel);
}

HRESULT CSection_2D::Ready_Map_2D(_wstring _strMapFIleName)
{
	m_pMap = CMap_2D::Create(m_pDevice, m_pContext, _strMapFIleName, m_fRenderResolution, m_iPriorityID);
	if (nullptr == m_pMap)
		return E_FAIL;

	return S_OK;
}

void CSection_2D::Free()
{
	Safe_Release(m_pMap);

	__super::Free();
}
