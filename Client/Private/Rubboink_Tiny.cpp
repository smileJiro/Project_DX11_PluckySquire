#include "stdafx.h"
#include "SlipperyObject.h"
#include "Rubboink_Tiny.h"
#include "Collider_Circle.h"
#include "ModelObject.h"
#include "GameInstance.h"
#include "Section_Manager.h"

CRubboink_Tiny::CRubboink_Tiny(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CSlipperyObject(_pDevice, _pContext)
{
}

CRubboink_Tiny::CRubboink_Tiny(const CRubboink_Tiny& _Prototype)
	:CSlipperyObject(_Prototype)
{
}


HRESULT CRubboink_Tiny::Initialize(void* _pArg)
{
	TINY_DESC* pDesc = static_cast<TINY_DESC*>(_pArg);
	m_pMudPit = pDesc->pMudPit;

	pDesc->iNumPartObjects = 3;
	pDesc->eStartCoord = COORDINATE_2D;
	pDesc->isCoordChangeEnable = false;
	pDesc->tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
	pDesc->tTransform2DDesc.vInitialPosition = _float3(1320.f, 515.f, 0.f);
	pDesc->tTransform2DDesc.fSpeedPerSec = 750.f;
	pDesc->iObjectGroupID = OBJECT_GROUP::SLIPPERY;
	
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(Ready_Parts()))
		return E_FAIL;

	m_p2DColliderComs.resize(1);
	/* Test 2D Collider */
	CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};
	CircleDesc.pOwner = this;
	CircleDesc.fRadius = 100.f;
	CircleDesc.vScale = { 1.0f, 1.0f };
	CircleDesc.vOffsetPosition = { 0.f, CircleDesc.fRadius * 0.5f };
	CircleDesc.isBlock = true;
	CircleDesc.isTrigger = false;
	CircleDesc.iCollisionGroupID = OBJECT_GROUP::SLIPPERY;
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
		TEXT("Com_Body2DCollider"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &CircleDesc)))
		return E_FAIL;


	static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Register_OnAnimEndCallBack(bind(&CRubboink_Tiny::On_AnimEnd, this, placeholders::_1, placeholders::_2));

	return S_OK;
}
HRESULT CRubboink_Tiny::Ready_Parts()
{
	CModelObject::MODELOBJECT_DESC tModelDesc;
	tModelDesc.Build_2D_Model(m_iCurLevelID, TEXT("Rubboink_Tiny"), TEXT("Prototype_Component_Shader_VtxPosTex"), (_uint)PASS_VTXPOSTEX::SPRITE2D, false);
	tModelDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
	tModelDesc.tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
	tModelDesc.tTransform2DDesc.vInitialScaling = _float3(1, 1, 1);
	m_PartObjects[PART_BODY] = m_pBody= static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &tModelDesc));
	Safe_AddRef(m_pBody);
	m_pBody->Switch_Animation(BODY_ANIM_FACELESS);

	tModelDesc.strModelPrototypeTag_2D = TEXT("Rubboink_Tiny_Face");
	m_PartObjects[PARTS_FACE] = m_pFace=static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &tModelDesc));
	Safe_AddRef(m_pFace);
	m_pFace->Switch_Animation(FACE_ANIM_SLEEP);
	
	tModelDesc.strModelPrototypeTag_2D = TEXT("Rubboink_Tiny_ZZZ");
	m_PartObjects[PARTS_ZZZ] = m_pZZZ= static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &tModelDesc));
	Safe_AddRef(m_pZZZ);
	m_pFace->Switch_Animation(ZZZ_ANIM_ZZZ);

	return S_OK;
}

void CRubboink_Tiny::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	if (m_bSlip && 
		(CGameObject*)m_pMudPit == _pOtherObject)
	{
		m_bSlip = false;
		On_Impact(_pOtherObject);
	}
}

void CRubboink_Tiny::On_AnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	if (BODY_ANIM::BODY_ANIM_MUD_DIVE == iAnimIdx)
	{
		m_pBody->Switch_Animation(BODY_ANIM::BODY_ANIM_MUD_IDLE);
	}
}

void CRubboink_Tiny::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
}

void CRubboink_Tiny::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}




void CRubboink_Tiny::On_StartSlip(_vector _vDirection)
{
	m_pBody->Switch_Animation(BODY_ANIM_ROLL);
	m_pFace->Set_Active(false);
	m_pZZZ->Set_Active(false);
	m_pZZZ->Set_Render(false);
	m_pFace->Set_Render(false);
}

void CRubboink_Tiny::On_Impact(CGameObject* _pOtherObject)
{
	Set_Position(_pOtherObject->Get_FinalPosition());
	m_pBody->Switch_Animation(BODY_ANIM_MUD_DIVE);
	m_pFace->Set_Active(false);
	m_pZZZ->Set_Active(false);
	m_pZZZ->Set_Render(false);
	m_pFace->Set_Render(false);
	m_p2DColliderComs[0]->Set_Offset(_float2{-200.f,50.f});
}

CRubboink_Tiny* CRubboink_Tiny::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CRubboink_Tiny* pInstance = new CRubboink_Tiny(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : Rubboink_Tiny");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CRubboink_Tiny::Clone(void* _pArg)
{
	CRubboink_Tiny* pInstance = new CRubboink_Tiny(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : Rubboink_Tiny");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CRubboink_Tiny::Free()
{
	Safe_Release(m_pBody);
	Safe_Release(m_pFace);
	Safe_Release(m_pZZZ);
	__super::Free();
}
