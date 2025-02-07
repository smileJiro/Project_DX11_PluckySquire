#include "stdafx.h"
#include "2DMapObjectInfo.h"
#include "Engine_Defines.h"
#include "GameInstance.h"
#include "Animation2D.h"
#include "Collider.h"

C2DMapObjectInfo::C2DMapObjectInfo()
	: m_pGameInstance(CGameInstance::GetInstance())
{
	Safe_AddRef(m_pGameInstance);
}

C2DMapObjectInfo::C2DMapObjectInfo(const C2DMapObjectInfo& Prototype)
	: CBase(Prototype)
{
}


HRESULT C2DMapObjectInfo::Initialize(json _InfoJson)
{
	m_strSearchTag = _InfoJson["SearchTag"];
	m_strModelName = _InfoJson["TextureName"];
	m_isActive = _InfoJson["Active"];
	m_isCollider = _InfoJson["Collider"];
	m_isSorting = _InfoJson["Sorting"];
	m_isBackGround = _InfoJson["BackGround"];

	m_eModelType = _InfoJson["ModelType"];;

	if (m_isActive)
	{
		if (
			_InfoJson.contains("ActivePropertis")
			&&
			_InfoJson["ActivePropertis"].contains("ActiveType")
			)
		{
			m_eActiveType = _InfoJson["ActivePropertis"]["ActiveType"];
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
				m_eColliderType = ColliderPropertis["ColliderType"];
			}
			if (ColliderPropertis.contains("ColliderInfo"))
			{
				auto ColliderInfo = ColliderPropertis["ColliderInfo"];

				if (ColliderInfo.contains("Collider_Offset_Pos"))
					m_fColliderOffsetPos = { ColliderInfo["Collider_Offset_Pos"]["X"],ColliderInfo["Collider_Offset_Pos"]["Y"] };

				switch (m_eColliderType)
				{
				case CCollider::AABB_2D:
					if (ColliderInfo.contains("Collider_Extent"))
						m_fColliderExtent = { ColliderInfo["Collider_Extent"]["X"],ColliderInfo["Collider_Extent"]["Y"] };
						break;
				case CCollider::CIRCLE_2D:
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
		CBase* pModel = m_pGameInstance->Find_Prototype(LEVEL_TOOL_2D_MAP, StringToWstring(m_strModelName));
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

HRESULT C2DMapObjectInfo::Export(json& _OutputJson)
{
	_OutputJson["SearchTag"] = m_strSearchTag;
	_OutputJson["TextureName"] = m_strModelName;
	_OutputJson["ModelType"] = m_eModelType;
	_OutputJson["Active"] = m_isActive;
	_OutputJson["Collider"] = m_isCollider;
	_OutputJson["Sorting"] = m_isSorting;
	_OutputJson["BackGround"] = m_isBackGround;
	if (m_isActive)
	{
		_OutputJson["ActivePropertis"]["ActiveType"] = m_eActiveType;
	}
	if (m_isCollider)
	{
		_OutputJson["ColliderPropertis"]["ColliderType"] = m_eColliderType;
		if (m_eColliderType != CCollider::TYPE_LAST)
		{

			_OutputJson["ColliderPropertis"]["ColliderInfo"]["Collider_Offset_Pos"]["X"] = m_fColliderOffsetPos.x;
			_OutputJson["ColliderPropertis"]["ColliderInfo"]["Collider_Offset_Pos"]["Y"] = m_fColliderOffsetPos.y;
			switch (m_eColliderType)
			{
				case CCollider::AABB_2D:
					_OutputJson["ColliderPropertis"]["ColliderInfo"]["Collider_Extent"]["X"] = m_fColliderExtent.y;
					_OutputJson["ColliderPropertis"]["ColliderInfo"]["Collider_Extent"]["Y"] = m_fColliderExtent.y;
					break;
				case CCollider::CIRCLE_2D:
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
			m_strModelName = WstringToString(*pTextureName);
			m_eModelType = _pModel->Get_AnimType();

			m_isToolRendering = true;
		}
	}
}


C2DMapObjectInfo* C2DMapObjectInfo::Create(json _InfoJson)
{
	C2DMapObjectInfo* pInstance = new C2DMapObjectInfo();

	if (FAILED(pInstance->Initialize(_InfoJson)))
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
