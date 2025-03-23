#include "stdafx.h"
#include "Gear.h"
#include "GameInstance.h"
#include "ModelObject.h"
#include "Section_Manager.h"
#include "PlayerData_Manager.h"

CGear::CGear(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CCharacter(_pDevice, _pContext)
{
}

CGear::CGear(const CGear& _Prototype)
	:CCharacter(_Prototype)
{
}

HRESULT CGear::Initialize_Prototype()
{

    return S_OK;
}

HRESULT CGear::Initialize(void* _pArg)
{
	GEAR_DESC* pDesc = static_cast<GEAR_DESC*>(_pArg);

	// Save Desc

	// Add Desc
	pDesc->iNumPartObjects = GEAR_PART_LAST;
	pDesc->eStartCoord = COORDINATE_2D;
	pDesc->iObjectGroupID = OBJECT_GROUP::BLOCKER;
	pDesc->isCoordChangeEnable = false;
	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;

	if (FAILED(Ready_PartObjects(pDesc)))
		return E_FAIL;

    return S_OK;
}

void CGear::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);

	// 사운드 플레이 중 & 플레이어가 바깥에 있음
	//if (m_pGameInstance->Is_SFXPlaying(TEXT("A_sfx_machines_loop")))
	if (m_isOnSound)
	{
		if (COORDINATE_3D == CPlayerData_Manager::GetInstance()->Get_PlayerCoord())
		{
			m_pGameInstance->Stop_SFX(TEXT("A_sfx_machines_loop"));
			m_isOnSound = false;
		}
	}
	else
	{
		if (false == m_isStop && COORDINATE_2D == CPlayerData_Manager::GetInstance()->Get_PlayerCoord())
		{
			if (CSection_Manager::GetInstance()->Is_PlayerInto(m_strSectionName))
			{
				m_pGameInstance->Start_SFX(TEXT("A_sfx_machines_loop"), g_SFXVolume, true);
				m_isOnSound = true;
			}
		}
	}
}

HRESULT CGear::Render()
{
	//if (KEY_DOWN(KEY::K))
	//{
	//	On_Stop();
	//	//Set_Position(XMVectorSet(472.f, 185.0f, 0.0f, 1.0f));
	//	//m_PartObjects[GEAR_PART_TEETH]->Set_Position(XMVectorSet(447.f, 0.0f, 0.0f, 1.0f));
	//}
	//if (KEY_DOWN(KEY::J))
	//{
	//	On_UnStop();
	//	//Set_Position(XMVectorSet(472.f, 185.0f, 0.0f, 1.0f));
	//	//m_PartObjects[GEAR_PART_TEETH]->Set_Position(XMVectorSet(447.f, 0.0f, 0.0f, 1.0f));
	//}
#ifdef _DEBUG
	if (m_p2DColliderComs[0]->Is_Active())
		m_p2DColliderComs[0]->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));
#endif

	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CGear::On_Stop()
{
	static_cast<CModelObject*>(m_PartObjects[GEAR_PART_GEAR])->Set_PlayingAnim(false);
	static_cast<CModelObject*>(m_PartObjects[GEAR_PART_GEAR])->Start_StoppableRender();
	static_cast<CModelObject*>(m_PartObjects[GEAR_PART_TEETH])->Switch_Animation(GEAR_PART_LAST);
	static_cast<CModelObject*>(m_PartObjects[GEAR_PART_TEETH])->Start_StoppableRender();
	Event_SetActive(m_p2DColliderComs[0], false);

	m_isStop = true;

	m_pGameInstance->Stop_SFX(TEXT("A_sfx_machines_loop"));
}

void CGear::On_UnStop()
{
	static_cast<CModelObject*>(m_PartObjects[GEAR_PART_GEAR])->Set_PlayingAnim(true);
	static_cast<CModelObject*>(m_PartObjects[GEAR_PART_GEAR])->End_StoppableRender();
	static_cast<CModelObject*>(m_PartObjects[GEAR_PART_TEETH])->Switch_Animation(GEAR_PART_TEETH);
	static_cast<CModelObject*>(m_PartObjects[GEAR_PART_TEETH])->End_StoppableRender();
	Event_SetActive(m_p2DColliderComs[0], true);

	m_isStop = false;

	m_pGameInstance->Start_SFX(TEXT("A_sfx_machines_loop"), g_SFXVolume, true);
}

HRESULT CGear::Ready_Components(GEAR_DESC* _pDesc)
{
	/* Teeth collider */
	m_p2DColliderComs.resize(1);

	CCollider_AABB::COLLIDER_AABB_DESC AABBDesc = {};
	AABBDesc.pOwner = this;
	AABBDesc.vExtents = { 700.0f, 40.0f };
	AABBDesc.vScale = { 1.0f, 1.0f };
	AABBDesc.vOffsetPosition = { 447.f, -30.0f };
	AABBDesc.isBlock = true;
	AABBDesc.isTrigger = false;
	AABBDesc.iCollisionGroupID = OBJECT_GROUP::BLOCKER;
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &AABBDesc)))
		return E_FAIL;

    return S_OK;
}

HRESULT CGear::Ready_PartObjects(GEAR_DESC* _pDesc)
{	
	{/* Part Gear */
		CModelObject::MODELOBJECT_DESC Desc{};
		Desc.iCurLevelID = m_iCurLevelID;
		Desc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
		Desc.Build_2D_Transform(_float2(0.0f, 0.0f), _float2(1.0f, 1.0f));
		Desc.Build_2D_Model(m_iCurLevelID, TEXT("Gear"), TEXT("Prototype_Component_Shader_VtxPosTex"));
		Desc.eStartCoord = COORDINATE_2D;
		
		m_PartObjects[GEAR_PART_GEAR] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &Desc));
		if (nullptr == m_PartObjects[GEAR_PART_GEAR])
			return E_FAIL;
		static_cast<CModelObject*>(m_PartObjects[GEAR_PART_GEAR])->Switch_Animation(GEAR_PART_GEAR);
	}/* Part Gear */

	{/* Part Teeth */
		CModelObject::MODELOBJECT_DESC Desc{};
		Desc.iCurLevelID = m_iCurLevelID;
		Desc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
		Desc.Build_2D_Transform(_float2(447.f, 0.0f), _float2(1.0f, 1.0f));
		Desc.Build_2D_Model(m_iCurLevelID, TEXT("Gear"), TEXT("Prototype_Component_Shader_VtxPosTex"));
		Desc.eStartCoord = COORDINATE_2D;

		m_PartObjects[GEAR_PART_TEETH] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &Desc));
		if (nullptr == m_PartObjects[GEAR_PART_TEETH])
			return E_FAIL;
		static_cast<CModelObject*>(m_PartObjects[GEAR_PART_TEETH])->Switch_Animation(GEAR_PART_TEETH);
	}/* Part Teeth */

    return S_OK;
}

CGear* CGear::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CGear* pInstance = new CGear(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CGear");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CGear::Clone(void* _pArg)
{
	CGear* pInstance = new CGear(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CGear");
		Safe_Release(pInstance);
	}

	return pInstance;
}


void CGear::Free()
{

	__super::Free();
}
