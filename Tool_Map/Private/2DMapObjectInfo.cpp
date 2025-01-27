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

#ifdef _DEBUG
	if (m_strSearchTag != "")
	{
		CBase* pModel = m_pGameInstance->Find_Prototype(LEVEL_TOOL_2D_MAP, StringToWstring(m_strTextureName));
		if (pModel != nullptr)
		{
			m_isModelCreate = true;
			m_pModel = static_cast<C2DModel*>(pModel);
			if (m_eModelType == MODEL_NONANIM)
			{
				CSpriteFrame* pFrame = m_pModel->Get_SpriteFrame();

				if (nullptr != pFrame)
				{
					m_pTexture = pFrame->Get_Texture();
					m_isToolRendering = true;
				}
			}
		}
		else
			int a = 1;
	}
#endif // _DEBUG

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
