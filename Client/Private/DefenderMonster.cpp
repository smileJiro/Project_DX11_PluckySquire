#include "stdafx.h"
#include "DefenderMonster.h"
#include "Collider_Circle.h"
#include "Collider_AABB.h"
#include "Section_Manager.h"

CDefenderMonster::CDefenderMonster(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CModelObject(_pDevice, _pContext)
{
}

CDefenderMonster::CDefenderMonster(const CDefenderMonster& _Prototype)
	:CModelObject(_Prototype)
{
}

HRESULT CDefenderMonster::Initialize(void* _pArg)
{
	DEFENDER_MONSTER_DESC* pDesc = static_cast<DEFENDER_MONSTER_DESC*>(_pArg);
	m_eTDirection = pDesc->eTDirection;

	pDesc->iObjectGroupID = OBJECT_GROUP::MAPOBJECT;
	pDesc->eStartCoord = COORDINATE_2D;
	pDesc->iModelPrototypeLevelID_2D = pDesc->iCurLevelID;
	pDesc->isCoordChangeEnable = false;

	pDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	pDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;

	pDesc->tTransform2DDesc.vInitialScaling = _float3(1, 1, 1);

	pDesc->eActorType = ACTOR_TYPE::LAST;
	pDesc->pActorDesc = nullptr;



	pDesc->strModelPrototypeTag_2D = TEXT("tiltswappusher");

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	Set_Direction(m_eTDirection);
	m_p2DColliderComs.resize(1);

	return S_OK;
}

void CDefenderMonster::Update(_float _fTimeDelta)
{
}

HRESULT CDefenderMonster::Render()
{
	return E_NOTIMPL;
}

void CDefenderMonster::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CDefenderMonster::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CDefenderMonster::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CDefenderMonster::Set_Direction(T_DIRECTION _eDirection)
{
}

CDefenderMonster* CDefenderMonster::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	return nullptr;
}

CGameObject* CDefenderMonster::Clone(void* _pArg)
{
	return nullptr;
}

void CDefenderMonster::Free()
{
}
