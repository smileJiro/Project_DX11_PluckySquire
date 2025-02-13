#pragma once
#include <iostream>
#include <type_traits>
#include "MapObject.h"
#include "2DMapObject.h"
#include "3DMapObject.h"
#include "GameInstance.h"
#include "GameInstance.h"

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
		
		if (!NormalDesc.isActive)
			strObjectTag = L"Prototype_GameObject_2DMapObject";
		else
			//strObjectTag = L"Prototype_GameObject_2DMapObject";
			strObjectTag = L"Prototype_GameObject_2DMap_ActionObject";
		
		CBase* pBase = _pGameInstance->
			Clone_Prototype(
					PROTOTYPE::PROTO_GAMEOBJ,
					LEVEL_STATIC,
					strObjectTag, reinterpret_cast<void*>(&NormalDesc));
		return (T*)pBase;
	}

	template<typename T, typename std::enable_if<std::is_base_of<Client::CMapObject, T>::value, _int>::type = 0>
	static T* Bulid_3DObject(LEVEL_ID _eLevelID, CGameInstance* _pGameInstance, HANDLE& _hFile, _bool _isCulling = true, _bool _isStatic = false)
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


		CBase* pBase = nullptr;


		_uint iSksp = 0;
		ReadFile(_hFile, &iSksp, sizeof(_uint), &dwByte, nullptr);

		
		// TODO : 맵 특수케이스 이넘 만들어서 관리할건데, 일단 스케치스페이스만 0210 박예슬
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
				{
					NormalDesc.isSksp = true;
					_char		szSaveSkspName[MAX_PATH];
					ReadFile(_hFile, &szSaveSkspName, (DWORD)(sizeof(_char) * MAX_PATH), &dwByte, nullptr);
					NormalDesc.strSkspTag = szSaveSkspName;
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



		if (pBase)
		{
			DWORD	dwByte(0);
			_uint iOverrideCount = 0;
			C3DModel::COLOR_SHADER_MODE eTextureType;
			_float4 fDefaultDiffuseColor;


			ReadFile(_hFile, &eTextureType, sizeof(C3DModel::COLOR_SHADER_MODE), &dwByte, nullptr);
			T* pMapObject = static_cast<T*>(pBase);

			pMapObject->Set_Color_Shader_Mode(eTextureType);

			switch (eTextureType)
			{
			case Engine::C3DModel::COLOR_DEFAULT:
			case Engine::C3DModel::MIX_DIFFUSE:
			{
				ReadFile(_hFile, &fDefaultDiffuseColor, sizeof(_float4), &dwByte, nullptr);
				pMapObject->Set_Diffuse_Color(fDefaultDiffuseColor);
			}
			break;
			default:
				break;
			}

			ReadFile(_hFile, &iOverrideCount, sizeof(_uint), &dwByte, nullptr);
			if (0 < iOverrideCount)
			{
				for (_uint i = 0; i < iOverrideCount; i++)
				{
					_uint iMaterialIndex, iTexTypeIndex, iTexIndex;
					ReadFile(_hFile, &iMaterialIndex, sizeof(_uint), &dwByte, nullptr);
					ReadFile(_hFile, &iTexTypeIndex, sizeof(_uint), &dwByte, nullptr);
					ReadFile(_hFile, &iTexIndex, sizeof(_uint), &dwByte, nullptr);

					pMapObject->Change_TextureIdx(iTexIndex, iTexTypeIndex, iMaterialIndex);
				}
			}

			return pMapObject;
		}
		
		return nullptr;

	}
};
END
