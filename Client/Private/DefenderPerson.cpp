#include "stdafx.h"
#include "DefenderPerson.h"
#include "GameInstance.h"
#include "Collider_Circle.h"
#include "Effect2D_Manager.h"
#include "ModelObject.h"
#include "Section_Manager.h"
#include "ScrollModelObject.h"


CDefenderPerson::CDefenderPerson(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CCharacter(_pDevice, _pContext)
{
}

CDefenderPerson::CDefenderPerson(const CDefenderPerson& _Prototype)
	:CCharacter(_Prototype)
{
}

HRESULT CDefenderPerson::Initialize(void* _pArg)
{
	m_pSectionManager = CSection_Manager::GetInstance();
	CCharacter::CHARACTER_DESC* pDesc = static_cast<CCharacter::CHARACTER_DESC*>(_pArg);

	m_iCurLevelID = pDesc->iCurLevelID;
	pDesc->iObjectGroupID = OBJECT_GROUP::GIMMICK_OBJECT;
	pDesc->eStartCoord = COORDINATE_2D;
	pDesc->isCoordChangeEnable = false;
	pDesc->iNumPartObjects = 3;

	pDesc->tTransform2DDesc.vInitialScaling = _float3(1, 1, 1);

	pDesc->eActorType = ACTOR_TYPE::LAST;
	pDesc->pActorDesc = nullptr;

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	if (FAILED(Ready_PartObjects()))
		return E_FAIL;

	if (FAILED(Ready_Components()))
		return E_FAIL;


	return S_OK;
}


HRESULT CDefenderPerson::Ready_PartObjects()
{
	CModelObject::MODELOBJECT_DESC tModelDesc = {};
	tModelDesc.eStartCoord = COORDINATE_2D;
	tModelDesc.iCurLevelID = m_iCurLevelID;
	tModelDesc.isCoordChangeEnable = false;
	tModelDesc.iModelPrototypeLevelID_2D = m_iCurLevelID;
	tModelDesc.strModelPrototypeTag_2D = TEXT("defenderperson");
	tModelDesc.strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	tModelDesc.iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;
	tModelDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
	tModelDesc.tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
	tModelDesc.tTransform2DDesc.vInitialScaling = _float3(1, 1, 1);

	m_PartObjects[PERSON_PART_BODY] = m_pBody =static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &tModelDesc));
	if (nullptr == m_PartObjects[PERSON_PART_BODY])
		return E_FAIL;
	Safe_AddRef(m_pBody);


	CModelObject::MODELOBJECT_DESC tModelDesc2 = {};
	tModelDesc2.eStartCoord = COORDINATE_2D;
	tModelDesc2.iCurLevelID = m_iCurLevelID;
	tModelDesc2.isCoordChangeEnable = false;
	tModelDesc2.iModelPrototypeLevelID_2D = m_iCurLevelID;
	tModelDesc2.strModelPrototypeTag_2D = TEXT("DefPersonFX");
	tModelDesc2.strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	tModelDesc2.iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;
	tModelDesc2.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
	tModelDesc2.tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
	tModelDesc2.tTransform2DDesc.vInitialScaling = _float3(1, 1, 1);

	m_PartObjects[PERSON_PART_FX] = m_pFX = static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &tModelDesc2));
	if (nullptr == m_PartObjects[PERSON_PART_FX])
		return E_FAIL;
	Safe_AddRef(m_pFX);
	m_pFX->Set_Active(false);

	CModelObject::MODELOBJECT_DESC tModelDesc3 = {};
	tModelDesc3.eStartCoord = COORDINATE_2D;
	tModelDesc3.iCurLevelID = m_iCurLevelID;
	tModelDesc3.isCoordChangeEnable = false;
	tModelDesc3.iModelPrototypeLevelID_2D = m_iCurLevelID;
	tModelDesc3.strModelPrototypeTag_2D = TEXT("Shield_01_Sprite");
	tModelDesc3.strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	tModelDesc3.iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;
	tModelDesc3.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);
	tModelDesc3.tTransform2DDesc.vInitialPosition = _float3(0.0f, 0.0f, 0.0f);
	tModelDesc3.tTransform2DDesc.vInitialScaling = _float3(1, 1, 1);

	m_PartObjects[PERSON_PART_SHIELD] = m_pShield=  static_cast<CModelObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_ModelObject"), &tModelDesc3));
	if (nullptr == m_PartObjects[PERSON_PART_SHIELD])
		return E_FAIL;
	Safe_AddRef(m_pShield);
	m_pShield->Set_Active(false);

	static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Register_OnAnimEndCallBack(bind(&CDefenderPerson::On_BodyAnimEnd, this, placeholders::_1, placeholders::_2));
	static_cast<CModelObject*>(m_PartObjects[PERSON_PART_FX])->Register_OnAnimEndCallBack(bind(&CDefenderPerson::On_FXAnimEnd, this, placeholders::_1, placeholders::_2));

	return S_OK;
}

HRESULT CDefenderPerson::Ready_Components()
{

	CCollider_Circle::COLLIDER_CIRCLE_DESC CircleDesc = {};
	CircleDesc.pOwner = this;
	CircleDesc.fRadius = 10.f;
	CircleDesc.vScale = { 1.0f, 1.0f };
	CircleDesc.vOffsetPosition = { 0.f, 0.f };
	CircleDesc.isBlock = false;
	CircleDesc.isTrigger = false;
	CircleDesc.iCollisionGroupID = OBJECT_GROUP::GIMMICK_OBJECT;
	CircleDesc.iColliderUse = (_uint)COLLIDER2D_USE::COLLIDER2D_BODY;
	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_Circle"),
		TEXT("Com_Body2DCollider"), reinterpret_cast<CComponent**>(&m_pBodyCollider), &CircleDesc)))
		return E_FAIL;

	m_p2DColliderComs.push_back(m_pBodyCollider);
	Safe_AddRef(m_pBodyCollider);


	return S_OK;
}

void CDefenderPerson::Update(_float _fTimeDelta)
{
	if (m_pFollowObject)
	{
		_vector vFollowPos = m_pFollowObject->Get_FinalPosition();
		_vector vMyPos = Get_FinalPosition();
		_vector vDir = XMVector3Normalize(vFollowPos - vMyPos);
		_float fDistance = XMVector3Length(vFollowPos - vMyPos).m128_f32[0];
		_float2 vSectionSize = m_pSectionManager->Get_Section_RenderTarget_Size(m_strSectionName);
		if (fDistance > vSectionSize.x * 0.5f)
			vDir *= -1.f;
		if (STOP == m_eFollowState)
		{
			if (fDistance > m_fFollowDistance)
				m_eFollowState = FOLLOW;
		}
		else if (FOLLOW == m_eFollowState)
		{
			Move(vDir * m_fMoveSpeed, _fTimeDelta);
			if (fDistance < m_fArrivalDistance)
				m_eFollowState = STOP;
		}

	}
	__super::Update(_fTimeDelta);
}
void CDefenderPerson::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{

}

void CDefenderPerson::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CDefenderPerson::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CDefenderPerson::Enter_Section(const _wstring _strIncludeSectionName)
{
	__super::Enter_Section(_strIncludeSectionName);
	m_pFX->Set_Active(true);
	m_pFX->Switch_Animation(APPEAR);

}

void CDefenderPerson::Start_FollowObject(CGameObject* _pObject)
{
	m_pFX->Set_Active(false);
	m_pShield->Set_Active(true);
	m_pFollowObject = _pObject;
}

void CDefenderPerson::Dissapear()
{
	m_pFollowObject = nullptr;
	m_pBody->Set_Active(false);
	m_pShield->Set_Active(false);
	m_pFX->Set_Active(true);
	m_pFX->Switch_Animation(DISAPPEAR);
	wstring strSFX = TEXT("A_sfx_rescued_vocal-") + to_wstring(rand() % 7);
	END_SFX(strSFX);
	START_SFX_DELAY(strSFX, 0.f, g_SFXVolume*0.65f, false);


	strSFX = TEXT("A_sfx_humans_rescued");
	END_SFX(strSFX);
	START_SFX_DELAY(strSFX, 0.f, g_SFXVolume * 0.65f, false);
}

void CDefenderPerson::On_BodyAnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
}

void CDefenderPerson::On_FXAnimEnd(COORDINATE _eCoord, _uint iAnimIdx)
{
	if (APPEAR == iAnimIdx)
	{
		m_pFX->Switch_Animation(BEACON);
	}
	else if (DISAPPEAR == iAnimIdx)
	{

		Event_DeleteObject(this);
	}
}

_vector CDefenderPerson::Get_ScrolledPosition(_vector _vPosition)
{
	_float2 fSize = m_pSectionManager->Get_Section_RenderTarget_Size(m_strSectionName);
	_float fDefaultWitdh = (fSize.x * 0.5f);

	if (-fDefaultWitdh > _vPosition.m128_f32[0])
	{
		_vPosition = XMVectorSetX(_vPosition, _vPosition.m128_f32[0] + fSize.x);
	}
	if (fDefaultWitdh < _vPosition.m128_f32[0])
	{
		_vPosition = XMVectorSetX(_vPosition, _vPosition.m128_f32[0] - fSize.x);
	}
	return _vPosition;
}

CDefenderPerson* CDefenderPerson::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CDefenderPerson* pInstance = new CDefenderPerson(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : DefenderPerson");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CDefenderPerson::Clone(void* _pArg)
{
	CDefenderPerson* pInstance = new CDefenderPerson(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : DefenderPerson");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDefenderPerson::Free()
{
	Safe_Release(m_pBodyCollider);
	Safe_Release(m_pBody);
	Safe_Release(m_pFX);
	Safe_Release(m_pShield); 
	__super::Free();
}
