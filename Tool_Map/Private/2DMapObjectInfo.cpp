#include "stdafx.h"
#include "2DMapObjectInfo.h"
#include "Engine_Defines.h"
#include "GameInstance.h"
#include "Animation2D.h"

C2DMapObjectInfo::C2DMapObjectInfo()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

C2DMapObjectInfo::C2DMapObjectInfo(const C2DMapObjectInfo& Prototype)
	: CBase(Prototype)
{
}


HRESULT C2DMapObjectInfo::Initialize(json _InfoJson, _string* _arrModelTypeString, _string* _arrActiveTypeString, _string* _arrColliderTypeString)
{
	m_strSearchTag = _InfoJson["SearchTag"];
	m_strTextureName = _InfoJson["TextureName"];
	_string strModelText = _InfoJson["ModelType"];
	m_isActive = _InfoJson["Active"];
	m_isCollider = _InfoJson["Collider"];
	m_isSorting = _InfoJson["Sorting"];

	for (_uint i = 0; i < MODEL_END; ++i)
	{
		if (_arrModelTypeString[i] == strModelText)
			m_eModelType = (MAPOBJ_MODEL_TYPE)i;
	}

	if (m_isActive)
	{
		if (
			_InfoJson.contains("ActivePropertis")
			&&
			_InfoJson["ActivePropertis"].contains("ActiveType")
			)
		{
			_string ActiveTypeTag = _InfoJson["ActivePropertis"]["ActiveType"];
			for (_uint i = 0; i < ACTIVE_END; ++i)
			{
				if (_arrActiveTypeString[i] == ActiveTypeTag)
					m_eActiveType = (MAPOBJ_ACTIVE_TYPE)i;
			}
		}
	}
	if (m_isCollider)
	{
		if (
			_InfoJson.contains("ColliderPropertis")
			)
		{
			auto ColliderPropertis = _InfoJson["ColliderPropertis"];
			if (ColliderPropertis.contains("ColliderType"))
			{
				_string ColliderTypeTag = ColliderPropertis["ColliderType"];
				for (_uint i = 0; i < COLLIDER_END; ++i)
				{
					if (_arrColliderTypeString[i] == ColliderTypeTag)
						m_eColliderType = (MAPOBJ_2D_COLLIDIER_TYPE)i;
				}
			}
			if (ColliderPropertis.contains("ColliderInfo"))
			{
				auto ColliderInfo = ColliderPropertis["ColliderInfo"];

				if (ColliderInfo.contains("Collider_Offset_Pos"))
					m_fColliderOffsetPos = { ColliderInfo["Collider_Offset_Pos"]["X"],ColliderInfo["Collider_Offset_Pos"]["Y"] };

				switch (m_eColliderType)
				{
					case Map_Tool::C2DMapObjectInfo::COLLIDER_AABB:
					if (ColliderInfo.contains("Collider_Extent"))
						m_fColliderExtent = { ColliderInfo["Collider_Extent"]["X"],ColliderInfo["Collider_Extent"]["Y"] };
						break;
					case Map_Tool::C2DMapObjectInfo::COLLIDER_SQUARE:
						if (ColliderInfo.contains("Collider_Radius"))
							m_fColliderRadius = ColliderInfo["Collider_Radius"];
						break;
				}
			}
		}
	}
	if (m_isCollider)
	{
		if (
			_InfoJson.contains("SortingPropertis")
			)
		{
			auto SortingPropertis = _InfoJson["SortingPropertis"];
				if (SortingPropertis.contains("Sorting_Offset_Pos"))
					m_fSortingPosition = { SortingPropertis["Sorting_Offset_Pos"]["X"],SortingPropertis["Sorting_Offset_Pos"]["Y"] };

		}
	}

#ifdef _DEBUG
	if (m_strSearchTag != "")
	{
		CBase* pModel = m_pGameInstance->Find_Prototype(LEVEL_TOOL_2D_MAP, StringToWstring(m_strTextureName));
		if (pModel != nullptr)
		{
			m_isModelCreate = true;
			m_pModel = static_cast<C2DModel*>(pModel);
			m_pTexture = m_pModel->Get_Texture();

			if (m_pTexture != nullptr)
			{
				m_isToolRendering = true;
			}
		}
		else
			int a = 1;
	}
#endif // _DEBUG


	return S_OK;
}

HRESULT C2DMapObjectInfo::Export(json& _OutputJson, _string* _arrModelTypeString, _string* _arrActiveTypeString, _string* _arrColliderTypeString)
{
	_OutputJson["SearchTag"] = m_strSearchTag;
	_OutputJson["TextureName"] = m_strTextureName;
	_OutputJson["ModelType"] = _arrModelTypeString[m_eModelType];
	_OutputJson["Active"] = m_isActive;
	_OutputJson["Collider"] = m_isCollider;
	_OutputJson["Sorting"] = m_isSorting;
	if (m_isActive)
	{
		_OutputJson["ActivePropertis"]["ActiveType"] = _arrActiveTypeString[m_eActiveType];
	}
	if (m_isCollider)
	{
		_OutputJson["ColliderPropertis"]["ColliderType"] = _arrColliderTypeString[m_eColliderType];
		if (m_eColliderType != COLLIDER_END)
		{
			_OutputJson["ColliderPropertis"]["ColliderInfo"]["Collider_Offset_Pos"]["X"] = m_fColliderOffsetPos.x;
			_OutputJson["ColliderPropertis"]["ColliderInfo"]["Collider_Offset_Pos"]["Y"] = m_fColliderOffsetPos.y;
			switch (m_eColliderType)
			{
				case Map_Tool::C2DMapObjectInfo::COLLIDER_AABB:
					_OutputJson["ColliderPropertis"]["ColliderInfo"]["Collider_Extent"]["X"] = m_fColliderExtent.y;
					_OutputJson["ColliderPropertis"]["ColliderInfo"]["Collider_Extent"]["Y"] = m_fColliderExtent.y;
					break;
				case Map_Tool::C2DMapObjectInfo::COLLIDER_SQUARE:
					_OutputJson["ColliderPropertis"]["ColliderInfo"]["Collider_Radius"] = m_fColliderRadius;
					break;
			}
		}
	}
	if (m_isSorting)
	{
		_OutputJson["SortingPropertis"]["Sorting_Offset_Pos"]["X"] = m_fSortingPosition.x;
		_OutputJson["SortingPropertis"]["Sorting_Offset_Pos"]["Y"] = m_fSortingPosition.y;
	}
	return S_OK;
}


ID3D11ShaderResourceView* C2DMapObjectInfo::Get_SRV(_float2* _pReturnSize)
{
	if (!m_isToolRendering || nullptr == m_pTexture)
		return nullptr;

	if (nullptr != _pReturnSize)
		*_pReturnSize = m_pTexture->Get_Size();
	;
	return m_pTexture->Get_SRV(0);
}

void C2DMapObjectInfo::Set_Model(C2DModel* _pModel)
{
	if (_pModel != nullptr)
	{
		m_isModelCreate = true;
		m_pModel = static_cast<C2DModel*>(_pModel);
		m_pTexture = _pModel->Get_Texture();
		if (m_pTexture != nullptr)
		{
			const _wstring* pTextureName = m_pTexture->Get_SRVName(0);
			if(nullptr != pTextureName)
			m_strTextureName = WstringToString(*pTextureName);
			m_eModelType = (MAPOBJ_MODEL_TYPE)_pModel->Get_AnimType();

			m_isToolRendering = true;
		}
	}
}


C2DMapObjectInfo* C2DMapObjectInfo::Create(json _InfoJson, _string* _arrModelTypeString, _string* _arrActiveTypeString, _string* _arrColliderTypeString)
{
	C2DMapObjectInfo* pInstance = new C2DMapObjectInfo();

	if (FAILED(pInstance->Initialize(_InfoJson, _arrModelTypeString, _arrActiveTypeString, _arrColliderTypeString)))
	{
		MSG_BOX("Failed to Created : C2DMapObjectInfo");
		Safe_Release(pInstance);
	}

	return pInstance;
}

C2DMapObjectInfo* C2DMapObjectInfo::Create()
{
	C2DMapObjectInfo* pInstance = new C2DMapObjectInfo();

	return pInstance;
}

void C2DMapObjectInfo::Free()
{
	Safe_Release(m_pGameInstance);
	__super::Free();
}
