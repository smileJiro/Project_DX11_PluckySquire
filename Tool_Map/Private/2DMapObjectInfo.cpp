#include "stdafx.h"
#include "2DMapObjectInfo.h"
#include "Engine_Defines.h"
#include "GameInstance.h"

C2DMapObjectInfo::C2DMapObjectInfo()
{
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
			&&
			_InfoJson["ColliderPropertis"].contains("ColliderType")
			)
		{
			_string ColliderTypeTag = _InfoJson["ColliderPropertis"]["ColliderType"];
			for (_uint i = 0; i < COLLIDER_END; ++i)
			{
				if (_arrColliderTypeString[i] == ColliderTypeTag)
					m_eColliderType = (MAPOBJ_2D_COLLIDIER_TYPE)i;
			}
		}
	}

	return S_OK;
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
	__super::Free();
}
