#pragma once
#include <iostream>
#include <type_traits>
#include "MapObject.h"
#include "2DMapObject.h"
#include "2DMapActionObject.h"
#include "3DMapObject.h"
#include "GameInstance.h"



typedef	struct tagDiffuseColorInfo
{
	_uint iMaterialIndex;
	C3DModel::COLOR_SHADER_MODE eColorMode;
	_float4 fDiffuseColor;
}DIFFUSE_COLOR_INFO;



BEGIN(Client)
class CMapObjectFactory {
public:
	template<typename T, typename std::enable_if<std::is_base_of<Client::CMapObject, T>::value, _int>::type = 0>
	static T* Bulid_2DObject(LEVEL_ID _eLevelID, CGameInstance* _pGameInstance, HANDLE& _hFile)
	{
		DWORD	dwByte(0);

		_uint		iModelIndex = 0;
		_float2		fPos = {};
		_float2		fScale = {};
		_bool		isOverride = false;

		_wstring strObjectTag = L"";

		ReadFile(_hFile, &iModelIndex, sizeof(_uint), &dwByte, nullptr);
		ReadFile(_hFile, &fPos, sizeof(_float2), &dwByte, nullptr);
		ReadFile(_hFile, &fScale, sizeof(_float2), &dwByte, nullptr);
		ReadFile(_hFile, &isOverride, sizeof(_bool), &dwByte, nullptr);

		const auto& tInfo = CSection_Manager::GetInstance()->Get_2DModel_Info(iModelIndex);

		typename T::MAPOBJ_DESC NormalDesc = {};

		NormalDesc.is2DImport = true;
		NormalDesc.Build_2D_Model(
			_eLevelID
			, StringToWstring(tInfo.strModelName)
			, L"Prototype_Component_Shader_VtxPosTex"
			, (_uint)PASS_VTXPOSTEX::SPRITE2D
		);
		NormalDesc.Build_2D_Transform(fPos, fScale);

		NormalDesc.isSorting = tInfo.isSorting;
		NormalDesc.isCollider = tInfo.isCollider;
		NormalDesc.isActive = tInfo.isActive;
		NormalDesc.isBackGround = tInfo.isBackGround;

		NormalDesc.eActiveType = tInfo.eActiveType;
		NormalDesc.eColliderType = tInfo.eColliderType;

		NormalDesc.fSorting_Offset_Pos = tInfo.fSorting_Offset_Pos;

		NormalDesc.fCollider_Offset_Pos = tInfo.fCollider_Offset_Pos;
		NormalDesc.fCollider_Extent = tInfo.fCollider_Extent;
		NormalDesc.fCollider_Radius = tInfo.fCollider_Radius;
		
		LEVEL_ID eObjectCloneLevel = LEVEL_STATIC;

		if (!NormalDesc.isActive)
			strObjectTag = L"Prototype_GameObject_2DMapObject";
		else
		{
			switch (NormalDesc.eActiveType)
			{
				case C2DMapActionObject::ACTIVE_TYPE_DROPBLOCK:
					strObjectTag = L"Prototype_GameObject_CollapseBlock";
					eObjectCloneLevel = LEVEL_CHAPTER_2;
					break;
				default:
					strObjectTag = L"Prototype_GameObject_2DMap_ActionObject";
					break;
			}
		
		}
			//strObjectTag = L"Prototype_GameObject_2DMapObject";
		//
		///* Test CollapseBlock */
		//{
		//	CCollapseBlock::MAPOBJ_DESC CollapseBlockDesc{};
		//	CollapseBlockDesc.Build_2D_Model(LEVEL_CHAPTER_2, TEXT("Prototype_Model2D_FallingRock"), TEXT("Prototype_Component_Shader_VtxPosTex"));
		//	CollapseBlockDesc.Build_2D_Transform(_float2(-100.f, -300.f));
		//	CollapseBlockDesc.eStartCoord = COORDINATE_2D;
		//	CGameObject* pGameObject = nullptr;
		//	if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(LEVEL_CHAPTER_2, TEXT("Prototype_GameObject_CollapseBlock"), m_eLevelID, TEXT("Layer_CollapseBlock"), &pGameObject, &CollapseBlockDesc)))
		//		return E_FAIL;

		//	CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(TEXT("Chapter2_P0102"), pGameObject, SECTION_2D_PLAYMAP_OBJECT);
		//}


		CBase* pBase = _pGameInstance->
			Clone_Prototype(
					PROTOTYPE::PROTO_GAMEOBJ,
					eObjectCloneLevel,
					strObjectTag, reinterpret_cast<void*>(&NormalDesc));


		return (T*)pBase;
	}

	template<typename T, typename std::enable_if<std::is_base_of<Client::CMapObject, T>::value, _int>::type = 0>
	static T* Bulid_3DObject(LEVEL_ID _eLevelID, CGameInstance* _pGameInstance, HANDLE& _hFile, _bool _isCulling = true, _bool _isActorToScene = true, _bool _isStatic = false)
	{
		DWORD	dwByte(0);

		_char		szSaveMeshName[MAX_PATH];
		_float4x4	vWorld = {};


		ReadFile(_hFile, &szSaveMeshName, (DWORD)(sizeof(_char) * MAX_PATH), &dwByte, nullptr);
		ReadFile(_hFile, &vWorld, sizeof(_float4x4), &dwByte, nullptr);


		T::MAPOBJ_3D_DESC NormalDesc = {};
		NormalDesc.Build_3D_Model(
			_eLevelID,
			_pGameInstance->StringToWString(szSaveMeshName).c_str(),
			L"Prototype_Component_Shader_VtxMesh"
			);
		NormalDesc.iModelPrototypeLevelID_3D = _isStatic ? LEVEL_STATIC : NormalDesc.iModelPrototypeLevelID_3D;
		NormalDesc.isCulling = _isCulling;
		NormalDesc.Build_3D_Transform(vWorld);
		NormalDesc.isAddActorToScene = _isActorToScene;
		CBase* pBase = nullptr;

		// 1 스케치스페이스 사용여부?
		_uint iSksp = 0;
		ReadFile(_hFile, &iSksp, sizeof(_uint), &dwByte, nullptr);

		if (0 < iSksp)
		{
			NormalDesc.iSksp = iSksp;
			_char		szSaveSkspName[MAX_PATH];
			ReadFile(_hFile, &szSaveSkspName, (DWORD)(sizeof(_char) * MAX_PATH), &dwByte, nullptr);
			NormalDesc.strSkspTag = szSaveSkspName;
		}

		// 2. 컬러 정보가 있는가의 여부?
		_uint iMaterialCount = 0;
		vector<DIFFUSE_COLOR_INFO> m_MaterialColors;

		ReadFile(_hFile, &iMaterialCount, sizeof(_uint), &dwByte, nullptr);
		if (0 < iMaterialCount)
		{
			m_MaterialColors.resize(iMaterialCount);

			for (_uint i = 0; i < iMaterialCount; i++)
			{
				ReadFile(_hFile, &m_MaterialColors[i].iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
				ReadFile(_hFile, &m_MaterialColors[i].eColorMode, sizeof(C3DModel::COLOR_SHADER_MODE), &dwByte, nullptr);
				ReadFile(_hFile, &m_MaterialColors[i].fDiffuseColor, sizeof(_float4), &dwByte, nullptr);
			}
		}
		// 컬러 정보가 있으면, 상수 버퍼를 만든다.
		if (0 < iMaterialCount)
			NormalDesc.isDeepCopyConstBuffer = true;


		
		switch (iSksp)
		{
				// Default
			case 0 :
			{
				pBase = _pGameInstance->
					Clone_Prototype(
						PROTOTYPE::PROTO_GAMEOBJ,
						LEVEL_STATIC,
						L"Prototype_GameObject_3DMapObject",
						reinterpret_cast<void*>(&NormalDesc));
			}
				break;
				// Sksp
			case 1 :
			case 2 :
			case 3 :
			case 4 :
			case 5 :
			case 6 :
				{
					pBase = _pGameInstance->
						Clone_Prototype(
							PROTOTYPE::PROTO_GAMEOBJ,
							LEVEL_STATIC,
							L"Prototype_GameObject_3DMap_SkspObject",
							reinterpret_cast<void*>(&NormalDesc));
				}
				break;
			default:
				break;
		}


		_uint iOverrideCount = 0;

		ReadFile(_hFile, &iOverrideCount, sizeof(_uint), &dwByte, nullptr);
		if (0 < iOverrideCount)
		{
			for (_uint i = 0; i < iOverrideCount; i++)
			{
				_uint iMaterialIndex, iTexTypeIndex, iTexIndex;
				ReadFile(_hFile, &iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
				ReadFile(_hFile, &iTexTypeIndex, sizeof(_uint), &dwByte, nullptr);
				ReadFile(_hFile, &iTexIndex, sizeof(_uint), &dwByte, nullptr);

				if (nullptr != pBase)
				{
					T* pMapObject = static_cast<T*>(pBase);
					pMapObject->Change_TextureIdx(iTexIndex, iTexTypeIndex, iMaterialIndex);
				}
			}
		}

		if (0 < iMaterialCount)
		{
			for (auto& tColorInfo : m_MaterialColors)
			{
				if (nullptr != pBase)
				{
					T* pMapObject = static_cast<T*>(pBase);
					pMapObject->Set_MaterialConstBuffer_Albedo(tColorInfo.iMaterialIndex, tColorInfo.eColorMode, tColorInfo.fDiffuseColor);
				}
			}
		}
		if (nullptr != pBase)
		{
			T* pMapObject = static_cast<T*>(pBase);
			return pMapObject;
		}


		return nullptr;

	}
};
END
