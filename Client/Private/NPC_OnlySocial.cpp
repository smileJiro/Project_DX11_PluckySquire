#include "stdafx.h"
#include "NPC_OnlySocial.h"
#include "ModelObject.h"
#include "GameInstance.h"
#include "Section_Manager.h"

#include "UI_Manager.h"
#include "StateMachine.h"
#include "Npc_OnlySocial.h"
#include "NPC_Social.h"
#include "NPC_Manager.h"



CNPC_OnlySocial::CNPC_OnlySocial(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CNPC(_pDevice, _pContext)
{
}

CNPC_OnlySocial::CNPC_OnlySocial(const CNPC_OnlySocial& _Prototype)
	:CNPC(_Prototype)
{
}



HRESULT CNPC_OnlySocial::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CNPC_OnlySocial::Initialize(void* _pArg)
{
	CNPC_OnlySocial::NPC_DESC* pDesc = static_cast<CNPC_OnlySocial::NPC_DESC*>(_pArg);

	pDesc->eStartCoord = COORDINATE_2D;
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	//if (FAILED(LoadFromJson(TEXT("../Bin/Resources/NPC/Create_NPC_Information.json"))))
	//	return E_FAIL;

	return S_OK;
}

void CNPC_OnlySocial::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CNPC_OnlySocial::Update(_float _fTimeDelta)
{
	
	__super::Update(_fTimeDelta);
	
	
}

void CNPC_OnlySocial::Late_Update(_float _fTimeDelta)
{

	__super::Late_Update(_fTimeDelta);

	
}

HRESULT CNPC_OnlySocial::Render()
{
#ifdef _DEBUG

#endif // _DEBUG

	return S_OK;
}


HRESULT CNPC_OnlySocial::NextLevelLoadJson(_int _iNextLevel)
{
	if (3 == (_int)_iNextLevel)
	{
		if (FAILED(LoadFromJson(TEXT("../Bin/Resources/NPC/Create_NPC_Information.json"))))
			return E_FAIL;
	}
	else if (4 == (_int)_iNextLevel)
	{
		if (FAILED(LoadFromJson(TEXT("../Bin/Resources/NPC/Create_NPC_Information_Chapter4.json"))))
			return E_FAIL;
	}
	else if (5 == _iNextLevel)
	{
		if (FAILED(LoadFromJson(TEXT("../Bin/Resources/NPC/Create_NPC_Information_Chapter6.json"))))
			return E_FAIL;
	}
	else if (6 == _iNextLevel)
	{
		if (FAILED(LoadFromJson(TEXT("../Bin/Resources/NPC/Create_NPC_Information_Chapter8.json"))))
			return E_FAIL;
	}

	return S_OK;
}

CNPC_OnlySocial* CNPC_OnlySocial::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CNPC_OnlySocial* pInstance = new CNPC_OnlySocial(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CPick_Bulb Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CNPC_OnlySocial::Clone(void* _pArg)
{
	CNPC_OnlySocial* pInstance = new CNPC_OnlySocial(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CNPC_OnlySocial Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CNPC_OnlySocial::Free()
{
	//Safe_Release(m_pColliderCom);
	__super::Free();
}

HRESULT CNPC_OnlySocial::Cleanup_DeadReferences()
{
	return S_OK;
}

HRESULT CNPC_OnlySocial::LoadFromJson(const std::wstring& filePath)
{
	if (GetFileAttributes(filePath.c_str()) == INVALID_FILE_ATTRIBUTES)
	{
		MSG_BOX("OnlySocial File Not Found");
		return E_FAIL;
	}

	ifstream ifs(filePath, ios::binary);
	json doc;
	ifs >> doc;

	

	if (doc.contains("NPC_OnlySocial") && doc["NPC_OnlySocial"].is_array())
	{
		for (auto& SocialNPC : doc["NPC_OnlySocial"])
		{
			NPC_ONLYSOCIAL tOnlySocial;

			if (SocialNPC.contains("strAnimaionName") && SocialNPC["strAnimaionName"].is_string())
			{
				tOnlySocial.strAnimationName = StringToWstring(SocialNPC["strAnimaionName"].get<string>());
			}

			if (SocialNPC.contains("strCreateSection") && SocialNPC["strCreateSection"].is_number_integer())
			{
				tOnlySocial.strCreateSection = SocialNPC["strCreateSection"].get<_int>();
			}

			if (SocialNPC.contains("strDialogueId") && SocialNPC["strDialogueId"].is_string())
			{
				tOnlySocial.strDialogueId = StringToWstring(SocialNPC["strDialogueId"].get<_string>());

			}
			
			if (SocialNPC.contains("iStartAnimation") && SocialNPC["iStartAnimation"].is_number_integer())
			{
				tOnlySocial.iStartAnimation = SocialNPC["iStartAnimation"].get<_int>();
			}

			if (SocialNPC.contains("vPositionX") && SocialNPC["vPositionX"].is_number_float())
			{
				tOnlySocial.vPositionX = SocialNPC["vPositionX"].get<_float>();
			}

			if (SocialNPC.contains("vPositionY") && SocialNPC["vPositionY"].is_number_float())
			{
				tOnlySocial.vPositionY = SocialNPC["vPositionY"].get<_float>();
			}

			if (SocialNPC.contains("vPositionZ") && SocialNPC["vPositionZ"].is_number_float())
			{
				tOnlySocial.vPositionZ = SocialNPC["vPositionZ"].get<_float>();
			}

			if (SocialNPC.contains("CollisionScaleX") && SocialNPC["CollisionScaleX"].is_number_float())
			{
				tOnlySocial.CollsionScaleX = SocialNPC["CollisionScaleX"].get<_float>();
			}

			if (SocialNPC.contains("CollisionScaleY") && SocialNPC["CollisionScaleY"].is_number_float())
			{
				tOnlySocial.CollsionScaleY = SocialNPC["CollisionScaleY"].get<_float>();
			}

			if (SocialNPC.contains("isInteractable") && SocialNPC["isInteractable"].is_boolean())
			{
				tOnlySocial.isInteractable = SocialNPC["isInteractable"].get<_bool>();
			}

			if (SocialNPC.contains("isDialog") && SocialNPC["isDialog"].is_boolean())
			{
				tOnlySocial.isDialog = SocialNPC["isDialog"].get<_bool>();
			}

			if (SocialNPC.contains("is2D") && SocialNPC["is2D"].is_boolean())
			{
				tOnlySocial.is2D = SocialNPC["is2D"].get<_bool>();
			}

			if (SocialNPC.contains("fCollisionHalfHeight") && SocialNPC["fCollisionHalfHeight"].is_number_float())
			{
				tOnlySocial.fCollisionHalfHeight = SocialNPC["fCollisionHalfHeight"].get<_float>();
			}

			if (SocialNPC.contains("fCollisionRadius") && SocialNPC["fCollisionRadius"].is_number_float())
			{
				tOnlySocial.fCollisionRadius = SocialNPC["fCollisionRadius"].get<_float>();
			}

			if (SocialNPC.contains("fTriggerRadius") && SocialNPC["fTriggerRadius"].is_number_float())
			{
				tOnlySocial.fTriggerRadius = SocialNPC["fTriggerRadius"].get<_float>();
			}
			if (SocialNPC.contains("fRotateAngle") && SocialNPC["fRotateAngle"].is_number_float())
			{
				tOnlySocial.fRotateAngle = SocialNPC["fRotateAngle"].get<_float>();
			}


			if (SocialNPC.contains("strNPCName") && SocialNPC["strNPCName"].is_string())
			{
				tOnlySocial.strNPCName = StringToWstring(SocialNPC["strNPCName"].get<_string>());
			}

			if (SocialNPC.contains("strSectionid") && SocialNPC["strSectionid"].is_string())
			{
				tOnlySocial.strSectionid = StringToWstring(SocialNPC["strSectionid"].get<_string>());
			}

			CGameObject* pGameObject;
			tOnlySocial.iCurLevelID = tOnlySocial.strCreateSection;
			tOnlySocial.tTransform2DDesc.vInitialPosition = _float3(0.f, 0.f, 0.f);
			tOnlySocial.tTransform3DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
			tOnlySocial.iMainIndex = 0;
			tOnlySocial.iSubIndex = 0;
			wsprintf(tOnlySocial.strDialogueIndex, (tOnlySocial.strDialogueId).c_str());
			//tOnlySocial.strDialogueIndex = tOnlySocial.strDialogueId;
			if (FAILED(m_pGameInstance->Add_GameObject_ToLayer(tOnlySocial.iCurLevelID, TEXT("Prototype_GameObject_NPC_Social"),
				tOnlySocial.iCurLevelID,
				tOnlySocial.is2D ?
				TEXT("Layer_SocialNPC")
				:
				TEXT("Layer_Social3DNPC")
				, &pGameObject, &tOnlySocial)))
				return E_FAIL;
			else if(false == tOnlySocial.is2D)
				pGameObject->Set_Render(false);

			CNPC_Manager::GetInstance()->SocialNpc_PushBack(static_cast<CNPC_Social*>(pGameObject));

		}
	}

	return S_OK;

}
