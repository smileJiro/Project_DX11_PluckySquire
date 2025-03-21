#include "stdafx.h"
#include "Laser_Container.h"
#include "ModelObject.h"
#include "Pooling_Manager.h"
#include "Section_Manager.h"
#include "GameInstance.h"
#include "Section_Manager.h"
#include "Character.h"
#include "Laser_Beam.h"
#include "Pressure_Plate.h"
#include "Interactable.h"

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

	m_fStartPos = pDesc->fStartPos;
	m_fEndPos = m_fTargetPos = pDesc->fEndPos;
	m_eDir = pDesc->eDir;
	m_IsBeamOn = pDesc->isBeamOn;
	m_IsMove = pDesc->isMove;
	pDesc->iNumPartObjects = (_uint)PART_END;
	pDesc->eStartCoord = COORDINATE_2D;
	pDesc->isCoordChangeEnable = false;
	pDesc->Build_2D_Transform(m_fStartPos);
	m_fMoveSpeed = pDesc->tTransform2DDesc.fSpeedPerSec = pDesc->fMoveSpeed;


	__super::Initialize(_pArg);

	/* 섹션 설정*/
	_wstring strSectionTag = pDesc->strInitSectionTag;
	if (L"" == strSectionTag)
		return E_FAIL;

	if (FAILED(SECTION_MGR->Add_GameObject_ToSectionLayer(strSectionTag, this, SECTION_2D_PLAYMAP_EFFECT)))
		return E_FAIL;

	/* 레이저 방향 설정 */
	_vector vDir = {};
	_float2 vStartLength = {60.f,100.f};
	switch (m_eDir)
	{
	case Client::F_DIRECTION::LEFT:
		vDir = XMVectorSetX(vDir, -1.f);
		break;
	case Client::F_DIRECTION::RIGHT:
		vDir = XMVectorSetX(vDir, 1.f);
		break;
	case Client::F_DIRECTION::UP:
		m_IsBeamRotate = true;
		vDir = XMVectorSetY(vDir, 1.f);
		vStartLength.x = 30.f;

		break;
	case Client::F_DIRECTION::DOWN:
		m_IsBeamRotate = true;
		vStartLength.x = 30.f;
		vDir = XMVectorSetY(vDir, -1.f);
		break;
	}
	XMVector2Normalize(vDir);
	XMStoreFloat2(&m_fDir, vDir);


	XMStoreFloat2(&m_fBeamStartPos, (vDir * vStartLength.x));
	XMStoreFloat2(&m_fBeamEndPos, (vDir * vStartLength.y));
	switch (m_eDir)
	{
	case Client::F_DIRECTION::LEFT:
	case Client::F_DIRECTION::RIGHT:
		m_fBeamStartPos.y = m_fBeamEndPos.y = 40.f;
		break;
	case Client::F_DIRECTION::UP:
	case Client::F_DIRECTION::DOWN:
		//m_fBeamStartPos.x = m_fBeamEndPos.x = 40.f;
		break;
	}
	/* 레이저 콜라이더들 설정*/
	if (FAILED(Ready_Components(pDesc)))
		return E_FAIL;
	/* 레이저 애님 파트들 설정 */
	if (FAILED(Ready_PartObjects(pDesc)))
		return E_FAIL;


	static_cast<CModelObject*>(m_PartObjects[PART_BEAM_START_EFFECT])->Switch_Animation(LASER_BEAM_BEGIN);
	static_cast<CModelObject*>(m_PartObjects[PART_BEAM_EFFECT])->Switch_Animation(LASER_BEAM_LENGTH_5);
	static_cast<CModelObject*>(m_PartObjects[PART_BEAM_END_EFFECT])->Switch_Animation(LASER_BEAM_END);

	static_cast<CModelObject*>(m_PartObjects[PART_BEAM_START_EFFECT])->Set_Position(XMLoadFloat2(&m_fBeamStartPos));
	static_cast<CModelObject*>(m_PartObjects[PART_BEAM_END_EFFECT])->Set_Position(XMLoadFloat2(&m_fBeamEndPos));


	_float fMaxLength = 2781.f * (_float)RATIO_BOOK2D_X;


	switch (m_eDir)
	{
	case Client::F_DIRECTION::LEFT:
		static_cast<CModelObject*>(m_PartObjects[PART_BEAM_EFFECT])->Set_Position(XMVectorSet(fMaxLength * -1.f + m_fBeamStartPos.x, m_fBeamStartPos.y,0.f,1.f));
		break;
	case Client::F_DIRECTION::RIGHT:
		static_cast<CModelObject*>(m_PartObjects[PART_BEAM_EFFECT])->Set_Position(XMVectorLerp(XMLoadFloat2(&m_fBeamStartPos), XMLoadFloat2(&m_fBeamEndPos), 0.5f));
		break;
	case Client::F_DIRECTION::UP:
		break;
	case Client::F_DIRECTION::DOWN:
		static_cast<CModelObject*>(m_PartObjects[PART_BEAM_EFFECT])->Set_Position(XMVectorSet(m_fBeamStartPos.x, fMaxLength * -1.f + m_fBeamStartPos.y, 0.f,1.f));
		break;
	}


	switch (m_eDir)
	{
	case Client::F_DIRECTION::LEFT:
		static_cast<CModelObject*>(m_PartObjects[PART_BODY])->Switch_Animation(LASER_MACHINE_LENGTH_SIDE);
		_vector vRight = m_pControllerTransform->Get_State(CTransform::STATE_RIGHT);
		m_PartObjects[PART_BEAM_START_EFFECT]->Get_ControllerTransform()->Set_State(CTransform::STATE_RIGHT, -1.f * XMVectorAbs(vRight));
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
		static_cast<CModelObject*>(m_PartObjects[PART_BEAM_START_EFFECT])->Get_ControllerTransform()->TurnAngle(XMConvertToRadians(-90.f), { 0.f,0.f,1.f });
		static_cast<CModelObject*>(m_PartObjects[PART_BEAM_EFFECT])->Get_ControllerTransform()->TurnAngle(XMConvertToRadians(90.f), { 0.f,0.f,1.f });
		break;
	}

	for (auto pPart : m_PartObjects)
		if(nullptr != pPart)
			static_cast<CModelObject*>(pPart)->Set_PlayingAnim(true);


	return S_OK;
}

void CLaser_Container::Priority_Update(_float _fTimeDelta)
{
	__super::Priority_Update(_fTimeDelta);
}

void CLaser_Container::Update(_float _fTimeDelta)
{

	__super::Update(_fTimeDelta);

	if (m_IsMove)
	{
		if (L"Chapter8_SKSP_09" == m_strSectionName)
			int a = 1;
		_vector vMyPos = Get_FinalPosition();
		_vector vTargetPos = XMLoadFloat2(&m_fTargetPos);
		m_pControllerTransform->Go_Direction(XMVectorSetW(XMVector2Normalize((vTargetPos - vMyPos)), 0.f), _fTimeDelta);

		if (XMVectorGetX(XMVector2Length(vTargetPos - vMyPos)) < 2.0f)
		{
			m_BackMove = !m_BackMove;
			if (m_BackMove)
				m_fTargetPos = m_fEndPos;
			else
				m_fTargetPos = m_fStartPos;
		}
	}

	if (nullptr != m_pPressurePlate)
		m_pPressurePlate->Update(_fTimeDelta);
	
	Pressured_Process(_fTimeDelta);
	
}

void CLaser_Container::Late_Update(_float _fTimeDelta)
{
	if (nullptr != m_pPressurePlate)
		m_pPressurePlate->Late_Update(_fTimeDelta);
	__super::Late_Update(_fTimeDelta);
}

HRESULT CLaser_Container::Render()
{
#ifdef _DEBUG

	if (COORDINATE_2D == Get_CurCoord())
	{
		for (_uint i = 0; i < m_p2DColliderComs.size(); ++i)
		{
			m_p2DColliderComs[i]->Render(SECTION_MGR->Get_Section_RenderTarget_Size(m_strSectionName));
		}
	}
#endif // _DEBUG

	__super::Render();
	return S_OK;
}

void CLaser_Container::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	if (RAY_TRIGGER & _pMyCollider->Get_CollisionGroupID())
	{
		// 블로커랑 닿았을 때, 거리 계산
		if (BLOCKER & _pOtherCollider->Get_CollisionGroupID() || BLOCKER_JUMPPASS & _pOtherCollider->Get_CollisionGroupID())
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
				Compute_Beam(_pMyCollider, _pOtherCollider, _pOtherObject);
			}
			else if (m_iBeamTargetIndex == _pOtherObject->Get_GameObjectInstanceID())
			{
				m_iBeamTargetIndex = _pOtherObject->Get_GameObjectInstanceID();
				m_fBeamLength = fLength;
				Compute_Beam(_pMyCollider, _pOtherCollider, _pOtherObject);
			}
		}
	}

	else if (MONSTER_PROJECTILE & _pMyCollider->Get_CollisionGroupID() && PLAYER & _pOtherCollider->Get_CollisionGroupID())
	{
		if (m_IsBeamOn)
		{
			_vector vPos = Get_FinalPosition();
			_vector vCheckPos = _pOtherObject->Get_FinalPosition();
			_float fTargetX = XMVectorGetX(vPos) + m_fBeamEndPos.x;
			_float fTargetY = XMVectorGetY(vPos) + m_fBeamEndPos.y;
			// 플레이어면 걍 죽어
			// 아니야 굳이 콜라이더를 수정하지말고 걍 체크하자?

			_bool isKill = false;

			switch (m_eDir)
			{
			case Client::F_DIRECTION::LEFT:
				isKill = fTargetX < XMVectorGetX(_pOtherObject->Get_FinalPosition());
				break;
			case Client::F_DIRECTION::RIGHT:
				isKill = fTargetX > XMVectorGetX(_pOtherObject->Get_FinalPosition());
				break;
			case Client::F_DIRECTION::UP:
				break;
			case Client::F_DIRECTION::DOWN:
				isKill = fTargetY < XMVectorGetY(_pOtherObject->Get_FinalPosition());
				break;
			}

			if (isKill)
				Event_Hit(this, static_cast<CCharacter*>(_pOtherObject), 99, XMVectorZero());
		}
	}

}

void CLaser_Container::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	if (RAY_TRIGGER & _pMyCollider->Get_CollisionGroupID())
	{
		// 블로커랑 닿았을 때, 거리 계산
		if (BLOCKER & _pOtherCollider->Get_CollisionGroupID() || BLOCKER_JUMPPASS & _pOtherCollider->Get_CollisionGroupID())
		{
			if (_pOtherObject->Get_GameObjectInstanceID() != m_iBeamTargetIndex)
				On_Collision2D_Enter(_pMyCollider, _pOtherCollider, _pOtherObject);
			else
			{
				auto pOther = dynamic_cast<IInteractable*>(_pOtherObject);
				if(nullptr != pOther && INTERACT_ID::DRAGGABLE == pOther->Get_InteractID())
					On_Collision2D_Enter(_pMyCollider, _pOtherCollider, _pOtherObject);
			}
			
		}
	}

	if (MONSTER_PROJECTILE & _pMyCollider->Get_CollisionGroupID() && PLAYER & _pOtherCollider->Get_CollisionGroupID())
	{
		if (m_IsBeamOn)
		{
			_vector vPos = Get_FinalPosition();
			_vector vCheckPos = _pOtherObject->Get_FinalPosition();
			_float fTargetX = XMVectorGetX(vPos) + m_fBeamEndPos.x;
			_float fTargetY = XMVectorGetY(vPos) + m_fBeamEndPos.y;
			// 플레이어면 걍 죽어
			// 아니야 굳이 콜라이더를 수정하지말고 걍 체크하자?

			_bool isKill = false;

			switch (m_eDir)
			{
			case Client::F_DIRECTION::LEFT:
				isKill = fTargetX < XMVectorGetX(_pOtherObject->Get_FinalPosition());
				break;
			case Client::F_DIRECTION::RIGHT:
				isKill = fTargetX > XMVectorGetX(_pOtherObject->Get_FinalPosition());
				break;
			case Client::F_DIRECTION::UP:
				break;
			case Client::F_DIRECTION::DOWN:
				isKill = fTargetY < XMVectorGetY(_pOtherObject->Get_FinalPosition());
				break;
			}

			if (isKill)
				Event_Hit(this, static_cast<CCharacter*>(_pOtherObject), 99, XMVectorZero());
		}
	}
}

void CLaser_Container::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	if (RAY_TRIGGER & _pMyCollider->Get_CollisionGroupID())
	{
		// 블로커랑 닿았을 때, 거리 계산
		if (BLOCKER & _pOtherCollider->Get_CollisionGroupID() || BLOCKER_JUMPPASS & _pOtherCollider->Get_CollisionGroupID())
		{
			if (m_iBeamTargetIndex == _pOtherObject->Get_GameObjectInstanceID())
				m_iBeamTargetIndex = -1;
		}
	}
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
	_float  fBeamWeight = 6.f;
	// 움직임 판단, 
	_vector vSectionEndPos = XMLoadFloat2(&fSectionSize);
	vSectionEndPos *= 0.5f;

	if (false == m_IsBeamRotate)
	{
		vSectionEndPos *= m_fDir.x;
		fSizeColliderOffset *= m_fDir.x;
		vSizeExtent = { fabs(XMVectorGetX(vPos) - XMVectorGetX(vSectionEndPos)) * 0.5f, fBeamWeight };
		vSizeOffsetPos = { Lerp_Pos(XMVectorGetX(vPos), XMVectorGetX(vSectionEndPos), 0.5f)
			- XMVectorGetX(vPos) + fSizeColliderOffset
			, 0.f };
		vSizeOffsetPos.y += 40.f;
	}
	else
	{
		vSectionEndPos *= m_fDir.y;
		fSizeColliderOffset *= m_fDir.y;
		vSizeExtent = { fBeamWeight, fabs(XMVectorGetY(vPos) - XMVectorGetY(vSectionEndPos)) * 0.5f };
		vSizeOffsetPos = { 0.f, Lerp_Pos(XMVectorGetY(vPos),XMVectorGetY(vSectionEndPos),0.5f)
			- XMVectorGetY(vPos) + fSizeColliderOffset };
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

#ifdef _DEBUG
		m_pSizeTriggerCollider->Set_DebugColor({ 1.f,0.f,0.f,1.f });

#endif // DEBUG
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

HRESULT CLaser_Container::Ready_PartObjects(LASER_DESC* _pDesc)
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

		if(i != PART_BEAM_EFFECT)
		m_PartObjects[i] = 
			static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, 
				LEVEL_STATIC, TEXT("Prototype_GameObject_Monster_Body"), &BodyDesc));
		else
			m_PartObjects[i] =
			static_cast<CPartObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ,
				m_iCurLevelID, TEXT("Prototype_GameObject_Laser_Beam"), &BodyDesc));

		if (nullptr == m_PartObjects[i])
			return E_FAIL;
	}
	if (_pDesc->isPressurePlate)
	{
		CModelObject::MODELOBJECT_DESC Desc = {};

		Desc.tTransform2DDesc.vInitialPosition = _float3(_pDesc->fPressurePlatePos.x, _pDesc->fPressurePlatePos.y,0.f);
		Desc.iCurLevelID = m_iCurLevelID;

		m_pPressurePlate = dynamic_cast<CPressure_Plate*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::PROTO_GAMEOBJ, LEVEL_STATIC,
			TEXT("Prototype_GameObject_Pressure_Plate"), &Desc));

		if (nullptr == m_pPressurePlate)
			return E_FAIL;

		if (FAILED(CSection_Manager::GetInstance()->Add_GameObject_ToSectionLayer(m_strSectionName, m_pPressurePlate, SECTION_2D_PLAYMAP_BACKGROUND)))
			return E_FAIL;
	}

	return S_OK;
}

void CLaser_Container::Pressured_Process(_float _fTimeDelta)
{
	if (nullptr != m_pPressurePlate)
	{
		m_pPressurePlate->Update(_fTimeDelta);

		// 문 열려있음 & 감압판 눌려있음 -> OPEN!
		if (m_pPressurePlate->Is_Down())
		{
			if (true == m_IsMove)
			{
				Set_BeamOn(false);
				Set_Move(false);
			}
		}

		// 감압판 눌려있지 않음 -> Close
		else if (false == m_pPressurePlate->Is_Down())
		{
			if (false == m_IsMove)
			{
				Set_BeamOn(true);
				Set_Move(true);
			}
		}

	}

}

void CLaser_Container::Set_BeamOn(_bool _IsBeamOn)
{
	m_IsBeamOn = _IsBeamOn;
	static_cast<CPartObject*>(m_PartObjects[PART_BEAM_START_EFFECT])->Set_Render(m_IsBeamOn);
	static_cast<CPartObject*>(m_PartObjects[PART_BEAM_EFFECT])->Set_Render(m_IsBeamOn);
	static_cast<CPartObject*>(m_PartObjects[PART_BEAM_END_EFFECT])->Set_Render(m_IsBeamOn);
}

void CLaser_Container::Compute_Beam(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{

	_float fMaxLength = 2781.f * (_float)RATIO_BOOK2D_X;

	_vector vLayerPosition = static_cast<CPartObject*>(m_PartObjects[PART_BEAM_END_EFFECT])->Get_OffsetPosition();
	_float2 fPos = static_cast<CCollider_AABB*>(_pMyCollider)->Get_CollisionPos();
	_float2 fExtent = static_cast<CCollider_AABB*>(_pOtherCollider)->Get_FinalExtents();
	_vector vContPos = Get_FinalPosition();

	_float fOffset = (fExtent.x * 0.8f);
	switch (m_eDir)
	{
	case Client::F_DIRECTION::RIGHT:
	case Client::F_DIRECTION::UP:
		fOffset *= -1.f;
	break;
	case Client::F_DIRECTION::LEFT:
		break;
	case Client::F_DIRECTION::DOWN:
		fOffset = (fExtent.y * 0.5f);
		if (fOffset > 400.f)
		{
			fOffset = (fExtent.y);
		}
		//_float fOffset = 0.f;

		break;
	}

	switch (m_eDir)
	{
	case Client::F_DIRECTION::RIGHT:
	case Client::F_DIRECTION::LEFT:
		vLayerPosition = XMVectorSetX(vLayerPosition, fPos.x - XMVectorGetX(vContPos) + fOffset);
		break;
	case Client::F_DIRECTION::DOWN:
		vLayerPosition = XMVectorSetY(vLayerPosition, fPos.y - XMVectorGetY(vContPos) + fOffset);
		break;
	case Client::F_DIRECTION::UP:
		vLayerPosition = XMVectorSetY(vLayerPosition, fPos.y - XMVectorGetY(vContPos) + fOffset);
		break;
	}








	m_PartObjects[PART_BEAM_END_EFFECT]->Set_Position(vLayerPosition);
	XMStoreFloat2(&m_fBeamEndPos, vLayerPosition);

	

	_float2 fStartUV = {0.f,0.f};
	_float2 fEndUV = {1.f,1.f};


	switch (m_eDir)
	{
	case Client::F_DIRECTION::LEFT:
	case Client::F_DIRECTION::RIGHT:
	{
		_float fX = fabs(m_fBeamStartPos.x - m_fBeamEndPos.x);
		_float fLengthUV = fX / fMaxLength;
		if(F_DIRECTION::RIGHT == m_eDir)
			fEndUV.x = fLengthUV;
		else
			fStartUV.x = 1.f - fLengthUV;
	}
		break;
	case Client::F_DIRECTION::UP:
	case Client::F_DIRECTION::DOWN:
	{
		_float fY = fabs(m_fBeamStartPos.y - m_fBeamEndPos.y);
		_float fLengthUV = fY / fMaxLength;
		if (F_DIRECTION::UP == m_eDir)
			fEndUV.x = fLengthUV;
		else
			fStartUV.x = 1.f - fLengthUV;
	}
		break;
	default:
		break;
	}

	static_cast<CLaser_Beam*>(m_PartObjects[PART_BEAM_EFFECT])->Set_StartUV(fStartUV);
	static_cast<CLaser_Beam*>(m_PartObjects[PART_BEAM_EFFECT])->Set_EndUV(fEndUV);
	//static_cast<CModelObject*>(m_PartObjects[PART_BEAM_EFFECT])->Switch_Animation(LASER_BEAM_LENGTH_5);
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
	Safe_Release(m_pPressurePlate);
	__super::Free();
}

void CLaser_Container::On_BombSwitch(_bool _bOn)
{
	if (true != _bOn)
	{
		// sd
		Set_BeamOn(false);
		//Set_Move(false);
	}
	else 
	{
		//Set_BeamOn(true);
		//Set_Move(true);
		//se
	}
}
