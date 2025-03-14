#include "stdafx.h"
#include "Laser_Container.h"
#include "ModelObject.h"
#include "Pooling_Manager.h"
#include "Section_Manager.h"
#include "GameInstance.h"
#include "Section_Manager.h"
#include "Character.h"

CLaser_Container::CLaser_Container(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CContainerObject(_pDevice, _pContext)
{
}

CLaser_Container::CLaser_Container(const CLaser_Container& _Prototype)
	: CContainerObject(_Prototype)
{
}

HRESULT CLaser_Container::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLaser_Container::Initialize(void* _pArg)
{
	LASER_DESC* pDesc = static_cast<LASER_DESC*>(_pArg);

	m_fStartPos = m_fTargetPos = pDesc->fStartPos;
	m_fEndPos = pDesc->fEndPos;
	m_eDir = pDesc->eDir;
	m_fMoveSpeed = pDesc->tTransform2DDesc.fSpeedPerSec = pDesc->fMoveSpeed;
	pDesc->iNumPartObjects = (_uint)PART_END;
	pDesc->eStartCoord = COORDINATE_2D;
	pDesc->isCoordChangeEnable= false;
	pDesc->Build_2D_Transform(m_fTargetPos);


	__super::Initialize(_pArg);

	/* 섹션 설정*/
	_wstring strSectionTag = pDesc->strInitSectionTag;
	if (L"" == strSectionTag)
		return E_FAIL;

	if (FAILED(SECTION_MGR->Add_GameObject_ToSectionLayer(strSectionTag, this, SECTION_2D_PLAYMAP_EFFECT)))
		return E_FAIL;

	/* 레이저 방향 설정 */
	_vector vDir = {};
	switch (m_eDir)
	{
	case Client::F_DIRECTION::LEFT:
		vDir = XMVectorSetX(vDir, -1.f);
		break;
	case Client::F_DIRECTION::RIGHT:
		vDir = XMVectorSetX(vDir, 1.f);
		break;
	case Client::F_DIRECTION::UP:
		m_bIsBeamRotate = true;
		vDir = XMVectorSetY(vDir, 1.f);
		break;
	case Client::F_DIRECTION::DOWN:
		m_bIsBeamRotate = true;
		vDir = XMVectorSetY(vDir, -1.f);
		break;
	}
	XMVector2Normalize(vDir);
	XMStoreFloat2(&m_fDir, vDir);


	XMStoreFloat2(&m_fBeamStartPos,(vDir * 30.f));
	XMStoreFloat2(&m_fBeamEndPos,(vDir * 500.f));

	/* 레이저 콜라이더들 설정*/
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	/* 레이저 애님 파트들 설정 */
	if (FAILED(Ready_PartObjects()))
		return E_FAIL;


	static_cast<CModelObject*>(m_PartObjects[PART_BEAM_START_EFFECT])->Switch_Animation(LASER_BEAM_BEGIN);
	static_cast<CModelObject*>(m_PartObjects[PART_BEAM_EFFECT])->Switch_Animation(LASER_BEAM_LENGTH_3);
	static_cast<CModelObject*>(m_PartObjects[PART_BEAM_END_EFFECT])->Switch_Animation(LASER_BEAM_END);

	static_cast<CModelObject*>(m_PartObjects[PART_BEAM_START_EFFECT])->Set_Position(XMLoadFloat2(&m_fBeamStartPos));
	static_cast<CModelObject*>(m_PartObjects[PART_BEAM_END_EFFECT])->Set_Position(XMLoadFloat2(&m_fBeamEndPos));
	static_cast<CModelObject*>(m_PartObjects[PART_BEAM_EFFECT])->Set_Position(XMVectorLerp(XMLoadFloat2(&m_fBeamStartPos), XMLoadFloat2(&m_fBeamEndPos),0.5f));

	switch (m_eDir)
	{
	case Client::F_DIRECTION::LEFT:
		static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(LASER_MACHINE_LENGTH_SIDE);
		_vector vRight = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
		m_PartObjects[PART_BODY]->Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, -1.f * XMVectorAbs(vRight));
		break;
	case Client::F_DIRECTION::RIGHT:
		static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(LASER_MACHINE_LENGTH_SIDE);
		break;
	case Client::F_DIRECTION::UP:
		static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(LASER_MACHINE_LENGTH_UP);
		static_cast<CModelObject*>(m_PartObjects[PART_BEAM_START_EFFECT])->Get_ControllerTransform()->TurnAngle(XMConvertToRadians(90.f), { 0.f,0.f,1.f });
		static_cast<CModelObject*>(m_PartObjects[PART_BEAM_EFFECT])->Get_ControllerTransform()->TurnAngle(XMConvertToRadians(90.f), { 0.f,0.f,1.f });
		break;
	case Client::F_DIRECTION::DOWN:
		static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(LASER_MACHINE_LENGTH_TOP);
		static_cast<CModelObject*>(m_PartObjects[PART_BEAM_START_EFFECT])->Get_ControllerTransform()->TurnAngle(XMConvertToRadians(90.f), { 0.f,0.f,1.f });
		static_cast<CModelObject*>(m_PartObjects[PART_BEAM_EFFECT])->Get_ControllerTransform()->TurnAngle(XMConvertToRadians(90.f), { 0.f,0.f,1.f });
		break;
	}




	return S_OK;
}

void CLaser_Container::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CLaser_Container::Update(_float _fTimeDelta)
{
	_vector vMyPos = Get_FinalPosition();
	_vector vTargetPos = XMLoadFloat2(&m_fTargetPos);
	m_pControllerTransform->Go_Direction(XMVector2Normalize((vTargetPos - vMyPos)), _fTimeDelta);

	if (1.f > m_pControllerTransform->Compute_Distance(vTargetPos))
	{
		m_BackMove = !m_BackMove;
		if (m_BackMove)
			m_fTargetPos = m_fEndPos;
		else
			m_fTargetPos = m_fStartPos;
	}


	__super::Update(_fTimeDelta);
}

void CLaser_Container::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CLaser_Container::Render()
{
#ifdef _DEBUG

	if (COORDINATE_2D == Get_CurCoord())
	{
		for (_uint i = 0; i < m_p2DColliderComs.size(); ++i)
		{
			m_p2DColliderComs[i]->Render();
		}
	}
#endif // _DEBUG

	__super::Render();
	return S_OK;
}

void CLaser_Container::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	if (RAY_TRIGGER & _pOtherCollider->Get_CollisionGroupID())
	{
		// 블로커랑 닿았을 때, 거리 계산
		if (BLOCKER & _pOtherCollider->Get_CollisionGroupID())
		{
			_vector vPos = Get_FinalPosition();

			_float2 fPos = {};

			XMStoreFloat2(&fPos, vPos);

			_float2 fTargetPos = _pOtherCollider->Get_Position();

			_float fLength = {};

			switch (m_eDir)
			{
			case Client::F_DIRECTION::LEFT:
			case Client::F_DIRECTION::RIGHT:
				fLength = fabs(fTargetPos.x - fPos.x);
				break;
			case Client::F_DIRECTION::UP:
			case Client::F_DIRECTION::DOWN:
				fLength = fabs(fTargetPos.y - fPos.y);
				break;
			}

			if (round(fLength) < round(m_fBeamLength) || m_iBeamTargetIndex == -1)
			{
				m_iBeamTargetIndex = _pOtherObject->Get_GameObjectInstanceID();
				m_fBeamLength = fLength;
				Compute_Beam();
			}
		}
		else if (PLAYER & _pOtherCollider->Get_CollisionGroupID())
		{
			// 플레이어면 걍 죽어
			Event_Hit(this, static_cast<CCharacter*>(_pOtherObject), 99, XMVectorZero());
		}
	}

}

void CLaser_Container::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CLaser_Container::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	if (m_iBeamTargetIndex == _pOtherObject->Get_GameObjectInstanceID())
		m_iBeamTargetIndex = -1;
}


void CLaser_Container::Active_OnEnable()
{
	__super::Active_OnEnable();
}

void CLaser_Container::Active_OnDisable()
{
	__super::Active_OnDisable();
}

HRESULT CLaser_Container::Ready_Components(LASER_DESC* _pDesc)
{
	_vector vDir = XMLoadFloat2(&m_fDir);

	_float2 fSectionSize = SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName);

	// 올바르지 않은 섹션 사이즈 체크
	if (0.f > fSectionSize.x
		||
		0.f > fSectionSize.y)
		return E_FAIL;


	_vector vPos = Get_FinalPosition();

	_float2 vSizeExtent = {};
	_float2 vSizeOffsetPos = {};
	_float  fSizeColliderOffset = 50.f;
	_float  fBeamWeight = 10.f;
	// 움직임 판단, 
	if (false == m_bIsBeamRotate)
	{
		vSizeExtent = { fSectionSize.x * 0.4f, fBeamWeight * 0.5f };
		vSizeOffsetPos = { fSectionSize.x * 0.5f - XMVectorGetX(vPos) - fSizeColliderOffset , 0.f };
	}
	else
	{
		vSizeExtent = { fBeamWeight * 0.5f, fSectionSize.y * 0.4f };
		vSizeOffsetPos = { 0.f, fSectionSize.y * 0.5f - XMVectorGetY(vPos) - fSizeColliderOffset };
	}
	m_p2DColliderComs.resize(2);
	/* Size Trigger Collider */
	CCollider_AABB::COLLIDER_AABB_DESC AABBDesc = {};
	AABBDesc.pOwner = this;
	AABBDesc.vExtents = vSizeExtent;
	AABBDesc.vScale = { 1.0f, 1.0f };
	AABBDesc.vOffsetPosition = vSizeOffsetPos;
	AABBDesc.isBlock = false;
	AABBDesc.isTrigger = true;
	AABBDesc.iCollisionGroupID = RAY_TRIGGER;

	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider_Trigger"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[0]), &AABBDesc)))
		return E_FAIL;
	else
	{
		m_pSizeTriggerCollider = m_p2DColliderComs[0];
		Safe_AddRef(m_pSizeTriggerCollider);
	}

	/* Beam Collider */
	AABBDesc.vExtents = vSizeExtent;
	AABBDesc.vScale = { 1.0f, 1.0f };
	AABBDesc.vOffsetPosition = vSizeOffsetPos;
	AABBDesc.isBlock = false;
	AABBDesc.iCollisionGroupID = MONSTER_PROJECTILE;

	if (FAILED(Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TEXT("Com_Collider_Beam"), reinterpret_cast<CComponent**>(&m_p2DColliderComs[1]), &AABBDesc)))
		return E_FAIL;
	else
	{
		m_pBeamCollider = m_p2DColliderComs[1];
		Safe_AddRef(m_pBeamCollider);
	}
	return S_OK;
}

HRESULT CLaser_Container::Ready_PartObjects()
{
	/* Part Body */
	CModelObject::MODELOBJECT_DESC BodyDesc{};

	BodyDesc.eStartCoord = COORDINATE_2D;
	BodyDesc.iCurLevelID = m_iCurLevelID;
	BodyDesc.pParentMatrices[COORDINATE_2D] = m_pControllerTransform->Get_WorldMatrix_Ptr(COORDINATE_2D);

	BodyDesc.Build_2D_Model(m_iCurLevelID,
		L"Laser_Machine",
		L"Prototype_Component_Shader_VtxPosTex"
	);
	BodyDesc.Build_2D_Transform({ 0.f,0.f });

	m_PartObjects[PART_BODY] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_Monster_Body"), &BodyDesc));
	if (nullptr == m_PartObjects[PART_BODY])
		return E_FAIL;

	for (_uint i = PART_BEAM_START_EFFECT; i < PART_END; i++)
	{
		BodyDesc.Build_2D_Model(m_iCurLevelID,
			L"Laser_Beam",
			L"Prototype_Component_Shader_VtxPosTex"
		);
		BodyDesc.Build_2D_Transform({ 0.f,0.f });

		m_PartObjects[i] = static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC, TEXT("Prototype_GameObject_Monster_Body"), &BodyDesc));
		if (nullptr == m_PartObjects[i])
			return E_FAIL;
	}


	return S_OK;
}

void CLaser_Container::Compute_Beam()
{
}

CLaser_Container* CLaser_Container::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CLaser_Container* pInstance = new CLaser_Container(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLaser_Container");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CLaser_Container::Clone(void* _pArg)
{
	CLaser_Container* pInstance = new CLaser_Container(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CLaser_Container");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLaser_Container::Free()
{
	Safe_Release(m_pSizeTriggerCollider);
	Safe_Release(m_pBeamCollider);
	__super::Free();
}
