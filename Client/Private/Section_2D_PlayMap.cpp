#include "stdafx.h"
#include "Section_2D_PlayMap.h"
#include "GameInstance.h"
#include "2DMapObject.h"
#include "Map_2D.h"
#include "Section_Manager.h"
#include "Engine_Macro.h"
#include "MapObjectFactory.h"
#include "Trigger_Manager.h"

CSection_2D_PlayMap::CSection_2D_PlayMap(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CSection_2D(_pDevice, _pContext)
{
}

HRESULT CSection_2D_PlayMap::Initialize(SECTION_2D_DESC* _pDesc, _uint _iPriorityKey)
{
	if (FAILED(__super::Initialize(_pDesc, _iPriorityKey)))
		return E_FAIL;
	return S_OK;
}

HRESULT CSection_2D_PlayMap::Import(json _SectionJson, _uint _iPriorityKey)
{
		if (FAILED(__super::Import(_SectionJson, _iPriorityKey)))
			return E_FAIL;

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
			_wstring strMapName = StringToWstring(szSaveMapName);

			if (FAILED(Ready_Map_2D(strMapName)))
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

					auto eRenderLayer = SECTION_2D_PLAYMAP_OBJECT;

					if (pGameObject->Is_BackGround())
						eRenderLayer = SECTION_2D_PLAYMAP_BACKGROUND;

					Add_GameObject_ToSectionLayer(pGameObject, eRenderLayer);
				}
			}
			CloseHandle(hFile);
		}



	_wstring strPath = L"../Bin/MapSaveFiles/2D/Trigger/";
	strPath += m_strName;
	strPath += L".json";

	// 트리거 파일이 있으면, 트리거를 불러온다.
	if (filesystem::exists(filesystem::path(strPath)))
	{
		CTrigger_Manager::GetInstance()->Load_Trigger(
			LEVEL_STATIC,
			(LEVEL_ID)CSection_Manager::GetInstance()->Get_SectionLeveID(),
			strPath,
			this
		);
	}
	return S_OK;
}
HRESULT CSection_2D_PlayMap::Add_GameObject_ToSectionLayer(CGameObject* _pGameObject, _uint _iLayerIndex)
{
	HRESULT hr = __super::Add_GameObject_ToSectionLayer(_pGameObject, _iLayerIndex);
	return hr;
}

HRESULT CSection_2D_PlayMap::Section_AddRenderGroup_Process()
{
	// 텍스쳐 복사
	if (FAILED(Copy_DefaultMap_ToRenderTarget()))
		return E_FAIL;

	// 오브젝트 그룹 소트
	Sort_Layer([](const CGameObject* pLeftGameObject, const CGameObject* pRightGameObject)->_bool {
		return XMVectorGetY(pLeftGameObject->Get_FinalPosition()) > XMVectorGetY(pRightGameObject->Get_FinalPosition());
		}, SECTION_2D_PLAYMAP_OBJECT);

	// 레이어 렌더러 삽입
	if (FAILED(Add_RenderGroup_GameObjects()))
		return E_FAIL;

	return S_OK;
}

CSection_2D_PlayMap* CSection_2D_PlayMap::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint _iPriorityKey, SECTION_2D_DESC* _pDesc)
{
	CSection_2D_PlayMap* pInstance = new CSection_2D_PlayMap(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize(_pDesc, _iPriorityKey)))
	{
		MSG_BOX("Failed Create CSection_2D_PlayMap");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSection_2D_PlayMap* CSection_2D_PlayMap::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext, _uint _iPriorityKey, json _SectionJson)
{
	CSection_2D_PlayMap* pInstance = new CSection_2D_PlayMap(_pDevice, _pContext);

	if (FAILED(pInstance->Import(_SectionJson, _iPriorityKey)))
	{
		MSG_BOX("Failed Create CSection_2D_PlayMap");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CSection_2D_PlayMap::Free()
{
	__super::Free();
}
