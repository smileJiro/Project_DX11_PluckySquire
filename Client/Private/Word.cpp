#include "stdafx.h"
#include "Word.h"
#include "Player.h"
#include "Section_Manager.h"    

#include "Collider_Circle.h"
#include "Actor_Dynamic.h"
#include "Controller_Model.h"
#include "Model.h"

CWord::CWord(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CCarriableObject(_pDevice, _pContext)
{
}

CWord::CWord(const CWord& _Prototype)
	:CCarriableObject(_Prototype)
{
}

HRESULT CWord::Render()
{
	return __super::Render();
}

HRESULT CWord::Initialize(void* _pArg)
{
	WORD_DESC* pDesc = static_cast<WORD_DESC*>(_pArg);

	m_pWordTexture = pDesc->pSRV;
	m_fSize = pDesc->fSize;
	m_eWordType = pDesc->eType;


	CActor::ACTOR_DESC ActorDesc;
	ActorDesc.pOwner = this;
	ActorDesc.FreezeRotation_XYZ[0] = false;
	ActorDesc.FreezeRotation_XYZ[1] = false;
	ActorDesc.FreezeRotation_XYZ[2] = false;
	ActorDesc.FreezePosition_XYZ[0] = false;
	ActorDesc.FreezePosition_XYZ[1] = false;
	ActorDesc.FreezePosition_XYZ[2] = false;

	SHAPE_BOX_DESC ShapeDesc = {};
	ShapeDesc.vHalfExtents = { 0.5f,0.1f ,0.5f };
	SHAPE_DATA ShapeData;
	ShapeData.pShapeDesc = &ShapeDesc;
	ShapeData.eShapeType = SHAPE_TYPE::BOX;
	ShapeData.eMaterial = ACTOR_MATERIAL::DEFAULT;
	ShapeData.isTrigger = false;
	XMStoreFloat4x4(&ShapeData.LocalOffsetMatrix, XMMatrixTranslation(0.0f, 0.f, 0.f));
	ActorDesc.ShapeDatas.push_back(ShapeData);
	ActorDesc.tFilterData.MyGroup = OBJECT_GROUP::BLOCKER;
	ActorDesc.tFilterData.OtherGroupMask = OBJECT_GROUP::MAPOBJECT | OBJECT_GROUP::PLAYER_TRIGGER | OBJECT_GROUP::BLOCKER | OBJECT_GROUP::PLAYER;
	pDesc->pActorDesc = &ActorDesc;
	pDesc->eActorType = ACTOR_TYPE::DYNAMIC;




	if (FAILED(CActorObject::Initialize(pDesc)))
		return E_FAIL;
	return S_OK;
}

void CWord::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
}

void CWord::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
	
}

HRESULT CWord::Change_Coordinate(COORDINATE _eCoordinate, _float3* _pNewPosition)
{

	if (FAILED(__super::Change_Coordinate(_eCoordinate, _pNewPosition)))
		return E_FAIL;
	return S_OK;

}


HRESULT CWord::Ready_Components()
{
	return S_OK;
}



void CWord::Interact(CPlayer* _pUser)
{
	_pUser->Set_CarryingObject(this);
}

_bool CWord::Is_Interactable(CPlayer* _pUser)
{
	return false == _pUser->Is_CarryingObject();
}

_float CWord::Get_Distance(COORDINATE _eCoord, CPlayer* _pUser)
{
	return XMVector3Length(m_pControllerTransform->Get_Transform(_eCoord)->Get_State(CTransform::STATE_POSITION)
		- _pUser->Get_ControllerTransform()->Get_Transform(_eCoord)->Get_State(CTransform::STATE_POSITION)).m128_f32[0];
}



CWord* CWord::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CWord* pInstance = new CWord(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Created CWord Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CGameObject* CWord::Clone(void* _pArg)
{
	CWord* pInstance = new CWord(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Clone CWord Failed");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CWord::Free()
{
	__super::Free();
}
