#include "stdafx.h"
#include "WorldMapNpc_Thrash.h"
#include "GameInstance.h"
#include "Section_Manager.h"

#include "UI_Manager.h"
#include "StateMachine.h"
#include "RabbitLunch.h"



CWorldMapNpc_Thrash::CWorldMapNpc_Thrash(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CModelObject(_pDevice, _pContext)
{
}

CWorldMapNpc_Thrash::CWorldMapNpc_Thrash(const CWorldMapNpc_Thrash& _Prototype)
	:CModelObject(_Prototype)
{
}



HRESULT CWorldMapNpc_Thrash::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWorldMapNpc_Thrash::Initialize(void* _pArg)
{
	WORLDMAP_PART_Thrash_DESC* pDesc = static_cast<WORLDMAP_PART_Thrash_DESC*>(_pArg);
	
	pDesc->eStartCoord = COORDINATE_2D;
	pDesc->isCoordChangeEnable = false;
	pDesc->strModelPrototypeTag_2D = TEXT("WorldMap_NPC");
	pDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	pDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;
	pDesc->tTransform2DDesc.vInitialPosition = _float3(-40.0f, 0.0f, 0.0f);
	pDesc->tTransform2DDesc.vInitialScaling = _float3(1.6f, 1.6f, 1.f);
	pDesc->iObjectGroupID = OBJECT_GROUP::MAPOBJECT;

	pDesc->tTransform2DDesc.fRotationPerSec = XMConvertToRadians(180.f);
	pDesc->tTransform2DDesc.fSpeedPerSec = 300.f;

	pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);
	pDesc->tTransform3DDesc.fSpeedPerSec = 3.f;
	


	m_pParent = pDesc->pParent;
	pDesc->pParentMatrices[COORDINATE_2D] = m_pParent->Get_ControllerTransform()->Get_WorldMatrix_Ptr(COORDINATE_2D);

	

	//if (FAILED(Ready_ActorDesc(pDesc)))
	//	return E_FAIL;

	if (FAILED(__super::Initialize(pDesc)))
		return E_FAIL;

	//if (FAILED(Ready_Components()))
	//	return E_FAIL;
	//
	//if (FAILED(Ready_PartObjects()))
	//	return E_FAIL;



	//CModelObject* pModelObject = static_cast<CModelObject*>(m_PartObjects[PART_BODY]);
	//
	//pModelObject->Set_AnimationLoop(COORDINATE::COORDINATE_2D, sketchspace_rabbit_idle_hungry,  true);
	//pModelObject->Set_Animation(ANIM_2D::sketchspace_rabbit_idle_hungry);
	//
	//CAnimEventGenerator::ANIMEVTGENERATOR_DESC tAnimEventDesc{};
	//tAnimEventDesc.pReceiver = this;
	//tAnimEventDesc.pSenderModel = static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_2D);
	//m_pAnimEventGenerator = static_cast<CAnimEventGenerator*> (m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_COMPONENT, m_iCurLevelID, TEXT("Sketch_Rabbit"), &tAnimEventDesc));
	//Safe_AddRef(m_pAnimEventGenerator);
	//Add_Component(TEXT("AnimEventGenerator"), m_pAnimEventGenerator);
	//
	//static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Register_OnAnimEndCallBack(bind(&CWorldMapNpc_Thrash::On_AnimEnd, this, placeholders::_1, placeholders::_2));
	//m_pControllerTransform->Set_State(CTransform::STATE_POSITION, _float4(-484.0f, -911.0f, 0.f, 1.f));
	//m_pControllerTransform->Set_Scale(COORDINATE_2D, _float3(1.5f, 1.5f, 0.f));
	

	

	return S_OK;
}

void CWorldMapNpc_Thrash::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CWorldMapNpc_Thrash::Update(_float _fTimeDelta)
{
	//Interact(Uimgr->Get_Player());


	__super::Update(_fTimeDelta);

	//if (false == m_isChangePos && sketchspace_rabbit_idle == static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Get_Model(COORDINATE_2D)->Get_CurrentAnimIndex())
	//{
	//	
	//}

	__super::Update(_fTimeDelta);




}

void CWorldMapNpc_Thrash::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);



	//CGameObject* pPlayer;

	///* 테스트용 코드*/
	//if (KEY_DOWN(KEY::J))
	//{
	//	static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(Martina_MenuOpen);
	//}
	//
	//if (true == m_isColPlayer)
	//{
	//	m_isDialoging = true;
	//	//Throw_Dialogue();
	//	//static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(Martina_talk);
	//}
	////
	//ChangeState_Panel();


}

HRESULT CWorldMapNpc_Thrash::Render()
{
	if (Is_Active())
		return __super::Render();

	return S_OK;
}

void CWorldMapNpc_Thrash::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{

}

void CWorldMapNpc_Thrash::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	
}

void CWorldMapNpc_Thrash::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	
}



void CWorldMapNpc_Thrash::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	
}


void CWorldMapNpc_Thrash::Interact(CPlayer* _pUser)
{
	//Throw_Dialogue();
}

_bool CWorldMapNpc_Thrash::Is_Interactable(CPlayer* _pUser)
{
	if (true == Uimgr->Get_Player()->Is_CarryingObject())
	{
		return false;
	}

	return true;
}

_float CWorldMapNpc_Thrash::Get_Distance(COORDINATE _eCoord, CPlayer* _pUser)
{
	return XMVector3Length(m_pControllerTransform->Get_Transform(_eCoord)->Get_State(CTransform::STATE_POSITION)
		- _pUser->Get_ControllerTransform()->Get_Transform(_eCoord)->Get_State(CTransform::STATE_POSITION)).m128_f32[0];
}








CWorldMapNpc_Thrash* CWorldMapNpc_Thrash::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CWorldMapNpc_Thrash* pInstance = new CWorldMapNpc_Thrash(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CPick_Bulb Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CWorldMapNpc_Thrash::Clone(void* _pArg)
{
	CWorldMapNpc_Thrash* pInstance = new CWorldMapNpc_Thrash(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CWorldMapNpc_Violet Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CWorldMapNpc_Thrash::Free()
{
	__super::Free();
}

HRESULT CWorldMapNpc_Thrash::Cleanup_DeadReferences()
{
	return S_OK;
}
