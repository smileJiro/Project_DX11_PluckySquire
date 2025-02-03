#include "stdafx.h"
#include "Camera_Trigger.h"

#include "Camera_Manager.h"

CCamera_Trigger::CCamera_Trigger(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CTriggerObject(_pDevice, _pContext)
{
}

CCamera_Trigger::CCamera_Trigger(const CCamera_Trigger& _Prototype)
	:CTriggerObject(_Prototype)
{
}

HRESULT CCamera_Trigger::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Trigger::Initialize(void* _pArg)
{
	CAMERA_TRIGGER_DESC* pDesc = static_cast<CAMERA_TRIGGER_DESC*>(_pArg);

	m_iCameraTriggerType = pDesc->iCameraTriggerType;
	m_szEventTag = pDesc->szEventTag;
	m_iReturnMask = pDesc->iReturnMask;

	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	return S_OK;
}

void CCamera_Trigger::Priority_Update(_float _fTimeDelta)
{
}

void CCamera_Trigger::Update(_float _fTimeDelta)
{
}

void CCamera_Trigger::Late_Update(_float _fTimeDelta)
{
	CTriggerObject::Late_Update(_fTimeDelta);
}

void CCamera_Trigger::OnTrigger_Enter(const COLL_INFO& _My, const COLL_INFO& _Other)
{
	switch (m_iCameraTriggerType) {
	case ARM_TRIGGER:
		Event_CameraTrigger(m_iCameraTriggerType, m_szEventTag, m_iTriggerID);
		break;
	case CUTSCENE_TRIGGER:
		//Event_CameraTrigger(m_iCameraTriggerType, m_szEventTag, m_iTriggerID);
		break;
	case FREEZE_X:
		break;
	case FREEZE_Z:
		break;
	}
}

void CCamera_Trigger::OnTrigger_Stay(const COLL_INFO& _My, const COLL_INFO& _Other)
{
}

void CCamera_Trigger::OnTrigger_Exit(const COLL_INFO& _My, const COLL_INFO& _Other)
{
	//if (true == m_isReturn) {
		// Pre로 나가기
		// Move_To_PreArm 모드로 변경함
		// (변경할 모드)
	//}

	switch (m_iCameraTriggerType) {
	case ARM_TRIGGER:
	{
		_vector vOtherPos = _Other.pActorUserData->pOwner->Get_ControllerTransform()->Get_Transform()->Get_State(CTransform::STATE_POSITION);
		_vector vPos = _My.pActorUserData->pOwner->Get_ControllerTransform()->Get_Transform()->Get_State(CTransform::STATE_POSITION);

		PxBoxGeometry Box;
		_My.pActorUserData->pOwner->Get_ActorCom()->Get_Shapes()[0]->getBoxGeometry(Box);

		// Right
		_uint iExitDir = Calculate_ExitDir(vPos, vOtherPos, Box);

		_bool isReturn = m_iReturnMask & iExitDir;

		// True -> PreArm Data로 리턴, False -> 현재 상태 유지
		Event_CameraTriggerExit(m_iTriggerID, isReturn);
	}
		break;
	case CUTSCENE_TRIGGER:
		break;
	}
}

_uint CCamera_Trigger::Calculate_ExitDir(_fvector _vPos, _fvector _vOtherPos, PxBoxGeometry& _Box)
{

	// Right
	if (XMVectorGetX(_vPos) + _Box.halfExtents.x <= XMVectorGetX(_vOtherPos)
		&& XMVectorGetZ(_vPos) + _Box.halfExtents.z >= XMVectorGetZ(_vOtherPos)
		&& XMVectorGetZ(_vPos) - _Box.halfExtents.z <= XMVectorGetZ(_vOtherPos)) {
		return EXIT_RETURN_MASK::RIGHT;
	}

	// LEFT
	if (XMVectorGetX(_vPos) - _Box.halfExtents.x >= XMVectorGetX(_vOtherPos)
		&& XMVectorGetZ(_vPos) + _Box.halfExtents.z >= XMVectorGetZ(_vOtherPos)
		&& XMVectorGetZ(_vPos) - _Box.halfExtents.z <= XMVectorGetZ(_vOtherPos)) {
		return EXIT_RETURN_MASK::LEFT;
	}

	// UP
	if (XMVectorGetZ(_vPos) + _Box.halfExtents.z <= XMVectorGetZ(_vOtherPos)
		&& XMVectorGetX(_vPos) + _Box.halfExtents.x >= XMVectorGetX(_vOtherPos)
		&& XMVectorGetX(_vPos) - _Box.halfExtents.x <= XMVectorGetX(_vOtherPos)) {
		return EXIT_RETURN_MASK::UP;
	}

	// DOWN
	if (XMVectorGetZ(_vPos) - _Box.halfExtents.z >= XMVectorGetZ(_vOtherPos)
		&& XMVectorGetX(_vPos) + _Box.halfExtents.x >= XMVectorGetX(_vOtherPos)
		&& XMVectorGetX(_vPos) - _Box.halfExtents.x <= XMVectorGetX(_vOtherPos)) {
		return EXIT_RETURN_MASK::DOWN;
	}

	return EXIT_RETURN_MASK::NONE;
}

CCamera_Trigger* CCamera_Trigger::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CCamera_Trigger* pInstance = new CCamera_Trigger(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_Trigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Trigger::Clone(void* _pArg)
{
	CCamera_Trigger* pInstance = new CCamera_Trigger(*this);
	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Trigger");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_Trigger::Free()
{
	__super::Free();
}
