#include "stdafx.h"
#include "JumpPad.h"
#include "Player.h"
#include "Actor_Dynamic.h"

CJumpPad::CJumpPad(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCarriableObject(_pDevice, _pContext)
{
}

CJumpPad::CJumpPad(const CJumpPad& _Prototype)
	:CCarriableObject(_Prototype)
{
}

HRESULT CJumpPad::Initialize(void* _pArg)
{

	JUMPPAD_DESC* pJumpPadDesc = static_cast<JUMPPAD_DESC*>(_pArg);
	pJumpPadDesc->eCrriableObjId = CARRIABLE_OBJ_ID::DIEC;
	pJumpPadDesc->vHeadUpRoolPitchYaw3D = { 0.f,0.f,0.f };
	pJumpPadDesc->vHeadUpOffset3D = { 0.f,1.8f,0.f };
	pJumpPadDesc->isCoordChangeEnable = true;
	pJumpPadDesc->iModelPrototypeLevelID_3D = pJumpPadDesc->iCurLevelID;
	pJumpPadDesc->iModelPrototypeLevelID_2D = pJumpPadDesc->iCurLevelID;
	pJumpPadDesc->strModelPrototypeTag_2D = TEXT("jumppad");
	pJumpPadDesc->strModelPrototypeTag_3D = TEXT("BouncyCrate_Rig_01");
	pJumpPadDesc->strShaderPrototypeTag_3D = TEXT("Prototype_Component_Shader_VtxAnimMesh");
	pJumpPadDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	pJumpPadDesc->tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
	pJumpPadDesc->iRenderGroupID_3D = RG_3D;
	pJumpPadDesc->iPriorityID_3D = PR3D_GEOMETRY;

	CActor::ACTOR_DESC ActorDesc = {};
	ActorDesc.pOwner = this;
	ActorDesc.FreezeRotation_XYZ[0] = true;
	ActorDesc.FreezeRotation_XYZ[1] = false;
	ActorDesc.FreezeRotation_XYZ[2] = true;
	ActorDesc.FreezePosition_XYZ[0] = false;
	ActorDesc.FreezePosition_XYZ[1] = false;
	ActorDesc.FreezePosition_XYZ[2] = false;

	SHAPE_BOX_DESC ShapeDesc = {};
	ShapeDesc.vHalfExtents = { 0.5f,0.25f ,0.5f };
	SHAPE_DATA ShapeData;
	ShapeData.pShapeDesc = &ShapeDesc;
	ShapeData.eShapeType = SHAPE_TYPE::BOX;
	ShapeData.eMaterial = ACTOR_MATERIAL::DEFAULT;
	ShapeData.isTrigger = false;
	ShapeData.FilterData.MyGroup = OBJECT_GROUP::DYNAMIC_OBJECT;
	ShapeData.FilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::DYNAMIC_OBJECT | OBJECT_GROUP::PLAYER | OBJECT_GROUP::MONSTER;
	XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, XMMatrixTranslation(0.0f, 0.f, 0.f));
	ActorDesc.ShapeDatas.push_back(ShapeData);

	SHAPE_SPHERE_DESC ShapeDesc2 = {};
	ShapeDesc2.fRadius = 0.1f;
	SHAPE_DATA ShapeData2;
	ShapeData2.pShapeDesc = &ShapeDesc2;
	ShapeData2.eShapeType = SHAPE_TYPE::SPHERE;
	ShapeData2.eMaterial = ACTOR_MATERIAL::DEFAULT;
	ShapeData2.isTrigger = false;
	ShapeData2.FilterData.MyGroup = OBJECT_GROUP::INTERACTION_OBEJCT;
	ShapeData2.FilterData.OtherGroupMask = OBJECT_GROUP::PLAYER_TRIGGER;
	XMStoreFloat4x4(&ShapeData2.LocalOffsetMatrix, XMMatrixTranslation(0.0f, 0.f, 0.f));
	ActorDesc.ShapeDatas.push_back(ShapeData2);


	ActorDesc.tFilterData.MyGroup = OBJECT_GROUP::DYNAMIC_OBJECT;
	ActorDesc.tFilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::BLOCKER | OBJECT_GROUP::PLAYER;
	pJumpPadDesc->pActorDesc = &ActorDesc;
	pJumpPadDesc->eActorType = ACTOR_TYPE::DYNAMIC;
	if (FAILED(__super::Initialize(pJumpPadDesc)))
		return E_FAIL;

	if(COORDINATE_3D == pJumpPadDesc->eStartCoord)
		Set_Animation(PLATFORMER_CARRY_2D);
	else if (m_bPlatformerMode)
		Set_Animation(SMALL_JUMP_3D);
	else
		Set_Animation(TOP_IDLE_2D);
	m_pActorCom->Set_Mass(1.5f);
	return S_OK;
}

void CJumpPad::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
}

void CJumpPad::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CJumpPad::Render()
{
	return __super::Render();
}

HRESULT CJumpPad::Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition)
{
	if (FAILED(__super::Change_Coordinate(_eCoordinate, _pNewPosition)))
		return E_FAIL;
	if (COORDINATE_2D == _eCoordinate)
	{
		if (m_bPlatformerMode)
			Set_Animation(SMALL_JUMP_3D);
		else
			Set_Animation(TOP_IDLE_2D);
	}
	else
	{
		Set_Animation(PLATFORMER_CARRY_2D);
	}

	return S_OK;
}

void CJumpPad::OnContact_Enter(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
 	if((_uint)SHAPE_USE::SHAPE_BODY == _My.pShapeUserData->iShapeUse)
	{
		for (auto& point : _ContactPointDatas)
		{
			if (abs(point.normal.y) >= abs(m_fCollisionSlopeThreshold)
				&& Get_FinalPosition().m128_f32[1] <= _Other.pActorUserData->pOwner->Get_FinalPosition().m128_f32[1])
			{
				if (OBJECT_GROUP::PLAYER == _Other.pActorUserData->iObjectGroup)
				{
					//static_cast<CPlayer*>(_Other.pActorUserData->pOwner)->Switch_Animation((_uint)CPlayer::ANIM_STATE_3D::LATCH_ANIM_JUMP_UP_02_GT);
					Event_SetPlayerState(static_cast<CPlayer*>(_Other.pActorUserData->pOwner), CPlayer::JUMP_UP);
				}
				_float fVelocity = XMVector3Length( static_cast<CActor_Dynamic*>(_Other.pActorUserData->pOwner->Get_ActorCom())->Get_LinearVelocity()).m128_f32[0];
				if(fVelocity < 10.f)
				{
					Switch_Animation(SMALL_JUMP_3D);
					Event_KnockBack(static_cast<CCharacter*>(_Other.pActorUserData->pOwner), { 0.f,1.f,0.f }, m_fBouncePower3D * 0.5f);
				}
				else
				{
					Switch_Animation(LARGE_JUMP_3D);
					Event_KnockBack(static_cast<CCharacter*>(_Other.pActorUserData->pOwner), { 0.f,1.f,0.f }, m_fBouncePower3D);
				}
				
				return;
			}
		}
	}
}

void CJumpPad::OnContact_Stay(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
}

void CJumpPad::OnContact_Exit(const COLL_INFO& _My, const COLL_INFO& _Other, const vector<PxContactPairPoint>& _ContactPointDatas)
{
}

void CJumpPad::OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

void CJumpPad::OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

void CJumpPad::OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

void CJumpPad::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CJumpPad::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CJumpPad::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

CJumpPad* CJumpPad::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CJumpPad* pInstance = new CJumpPad(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : JumpPad");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CJumpPad::Clone(void* _pArg)
{
	CJumpPad* pInstance = new CJumpPad(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : JumpPad");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CJumpPad::Free()
{
	__super::Free();
}
