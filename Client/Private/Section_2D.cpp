#include "stdafx.h"
#include "Section_2D.h"
#include "GameInstance.h"
#include "2DMapObject.h"
#include "Map_2D.h"
#include "Section_Manager.h"
#include "Engine_Macro.h"
#include "MapObjectFactory.h"

CSection_2D::CSection_2D(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CSection(_pDevice, _pContext)
{
}

HRESULT CSection_2D::Initialize(SECTION_2D_DESC* _pDesc, _uint _iPriorityKey)
{
	if (_pDesc == nullptr)
		return E_FAIL;
	if (1 == _pDesc->iLayerGroupCount)
		_pDesc->iLayerGroupCount = SECTION_2D_RENDERGROUP::SECTION_2D_RENDERGROUP_LAST;


	if (FAILED(__super::Initialize(_pDesc)))
		return E_FAIL;

	m_iGroupID = RG_2D;
	m_iPriorityID = _iPriorityKey;
	return S_OK;
}

HRESULT CSection_2D::Import(json _SectionJson, _uint _iPriorityKey)
{
	if (_SectionJson.contains("Section_Name"))
	{
		CSection_2D::SECTION_2D_DESC Desc;
		_string strText = _SectionJson["Section_Name"];
		m_strName = StringToWstring(strText);

#pragma region Section Info Desc Import && Initialize
		if (_SectionJson.contains("Section_Default"))
		{
			_uint eSectionDefault = _SectionJson["Section_Default"];
			Desc.Book_2D_Build(m_strName);
		}
		else
		{
			auto SectionInfo = _SectionJson["Section_Info"];
			Desc.eSectionType = SectionInfo["Section_Type"];
			Desc.eSectionPlayType = SectionInfo["Section_Play_Type"];
		}

		switch (Desc.eSectionType)
		{
			case Client::CSection_2D::ON_SECTION_BOOK:
			{
				if (_SectionJson.contains("Book_Info"))
				{
					
					auto BookInfo = _SectionJson["Book_Info"];
					if (BookInfo.contains("Book_Type"))
						Desc.eBookType = BookInfo["Book_Type"];
					if (BookInfo.contains("Next_Page_Tag"))
					{
						_string strNextPageTag = BookInfo["Next_Page_Tag"];
						m_strNextPageTag = StringToWstring(strNextPageTag);
					}
					if (BookInfo.contains("Pre_Page_Tag"))
					{
						_string strPrePageTag = BookInfo["Pre_Page_Tag"];
						m_strPrePageTag = StringToWstring(strPrePageTag);
					}
					
				}
			}
			break;
			case Client::CSection_2D::ON_SECTION_3D_IN_2D:
				break;
			default:
				break;
		}

		if (FAILED(Initialize(&Desc, _iPriorityKey)))
			return E_FAIL;
#pragma endregion


		if (_SectionJson.contains("Section_File_Name"))
		{
			_string strFileName = _SectionJson["Section_File_Name"];

			_wstring strSectionPath = MAP_2D_DEFAULT_PATH;
			strSectionPath += StringToWstring(strFileName);

			HANDLE	hFile = CreateFile(strSectionPath.c_str(),
				GENERIC_READ,
				NULL,
				NULL,
				OPEN_EXISTING,
				FILE_ATTRIBUTE_NORMAL,
				NULL);
			if (INVALID_HANDLE_VALUE == hFile)
			{
				return E_FAIL;
			}
			DWORD	dwByte(0);
			_uint       iObjectCount = 0;
			_char		szSaveMapName[MAX_PATH];

			ReadFile(hFile, &szSaveMapName, (DWORD)(sizeof(_char) * MAX_PATH), &dwByte, nullptr);
			Desc.strMapName = StringToWstring(szSaveMapName);

			if (FAILED(Ready_Map_2D(&Desc, _iPriorityKey)))
				return E_FAIL;

			ReadFile(hFile, &iObjectCount, sizeof(_uint), &dwByte, nullptr);

			for (_uint i = 0;i < iObjectCount; ++i)
			{
				C2DMapObject* pGameObject = 
					CMapObjectFactory::Bulid_2DObject<C2DMapObject>(
						(LEVEL_ID)CSection_Manager::GetInstance()->Get_SectionLeveID(),
															m_pGameInstance, 
															hFile);
				if (nullptr != pGameObject)
				{
					pGameObject->Set_Active(false);
					Event_CreateObject(pGameObject->Get_CurLevelID(), L"Layer_2DMapObject", pGameObject);

					Safe_AddRef(pGameObject);

					auto eRenderLayer = SECTION_2D_OBJECT;

					if (pGameObject->Is_BackGround())
						eRenderLayer = SECTION_2D_BACKGROUND;
					
					Add_GameObject_ToSectionLayer(pGameObject, eRenderLayer);
				}
			}
			CloseHandle(hFile);
		}



	}
	else
	{
		return E_FAIL;
	}




	return S_OK;
}
HRESULT CSection_2D::Register_RTV_ToTargetManager()
{
	return S_OK;
}

HRESULT CSection_2D::Register_RenderGroup_ToRenderer()
{
	/* 현재 미구현, Rendering 관련해서는 좀만 더 고민해볼예정. */
	if (FAILED(m_pMap->Clear_Map()))
		return E_FAIL;


	return m_pMap->Copy_DefaultMap_ToRenderTarget();
}

HRESULT CSection_2D::Add_GameObject_ToSectionLayer(CGameObject* _pGameObject, _uint _iLayerIndex)
{
	// Model
	// 컨테이너 
	HRESULT hr = __super::Add_GameObject_ToSectionLayer(_pGameObject, _iLayerIndex);
	//if (SUCCEEDED(hr) && nullptr != m_pMap)
	//{
	//	_float2 fRenderTargetSize = m_pMap->Get_RenderTarget_Size();

	//	fRenderTargetSize.x /= (_float)DEFAULT_SIZE_BOOK2D_X;
	//	fRenderTargetSize.y /= (_float)DEFAULT_SIZE_BOOK2D_Y;
	//	_pGameObject->Set_Scale(COORDINATE_2D, fRenderTargetSize.x, fRenderTargetSize.y,1.f);
	//}

	return hr;
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
};

HRESULT CSection_2D::Ready_Map_2D(SECTION_2D_DESC* _pDesc, _uint _iPriorityKey)
{
	m_pMap = CMap_2D::Create(m_pDevice, m_pContext, _pDesc->strMapName,_pDesc->fRenderTarget_Size, _iPriorityKey);
	if (nullptr == m_pMap)
		return E_FAIL;

	return S_OK;
}

CSection_2D* CSection_2D::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint _iPriorityKey, SECTION_2D_DESC* _pDesc)
{
	CSection_2D* pInstance = new CSection_2D(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_pDesc, _iPriorityKey)))
	{
		MSG_BOX("Failed Create CSection_2D");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSection_2D* CSection_2D::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint _iPriorityKey, json _SectionJson)
{
	CSection_2D* pInstance = new CSection_2D(_pDevice, _pContext);

	if (FAILED(pInstance->Import(_SectionJson, _iPriorityKey)))
	{
		MSG_BOX("Failed Create CSection_2D");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CSection_2D::Free()
{
	Safe_Release(m_pMap);

	__super::Free();
}
