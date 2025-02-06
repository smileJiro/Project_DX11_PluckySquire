#include "stdafx.h"
#include "Camera_2D.h"

#include "GameInstance.h"

CCamera_2D::CCamera_2D(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext }
{
}

CCamera_2D::CCamera_2D(const CCamera_2D& Prototype)
	: CCamera{ Prototype }
{
}

HRESULT CCamera_2D::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_2D::Initialize(void* pArg)
{
	CAMERA_2D_DESC* pDesc = static_cast<CAMERA_2D_DESC*>(pArg);

	pDesc->eStartCoord = COORDINATE_3D;
	pDesc->isCoordChangeEnable = false;
	pDesc->tTransform3DDesc.fSpeedPerSec = 10.f;
	pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);

	m_fSmoothSpeed = pDesc->fSmoothSpeed;
	m_eCameraMode = pDesc->eCameraMode;
	m_vAtOffset = pDesc->vAtOffset;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCamera_2D::Priority_Update(_float fTimeDelta)
{
}

void CCamera_2D::Update(_float fTimeDelta)
{
}

void CCamera_2D::Late_Update(_float fTimeDelta)
{
	Action_Mode(fTimeDelta);
	__super::Compute_PipeLineMatrices();
}

void CCamera_2D::Add_CurArm(CCameraArm* _pCameraArm)
{
}

void CCamera_2D::Action_Mode(_float _fTimeDelta)
{
	Action_Zoom(_fTimeDelta);
	Action_Shake(_fTimeDelta);
	Change_AtOffset(_fTimeDelta);

	switch (m_eCameraMode) {
	case DEFAULT:
		Defualt_Move(_fTimeDelta);
		break;
	case MOVE_TO_DESIREPOS:
		Move_To_DesirePos(_fTimeDelta);
		break;
	case MOVE_TO_SHOP:
		Move_To_Shop(_fTimeDelta);
		break;
	case RETURN_TO_DEFUALT:
		Return_To_Default(_fTimeDelta);
		break;
	case FLIPPING:
		Fliping(_fTimeDelta);
		break;
	}
}

void CCamera_2D::Defualt_Move(_float fTimeDelta)
{
	//_vector vCameraPos = m_pCurArm->Calculate_CameraPos(fTimeDelta);
	//Get_ControllerTransform()->Set_State(CTransform::STATE_POSITION, vCameraPos);

	//Look_Target(fTimeDelta);
}

void CCamera_2D::Move_To_DesirePos(_float _fTimeDelta)
{
}

void CCamera_2D::Move_To_Shop(_float _fTimeDelta)
{
}

void CCamera_2D::Return_To_Default(_float _fTimeDelta)
{
}

void CCamera_2D::Fliping(_float _fTimeDelta)
{
}

void CCamera_2D::Look_Target(_float fTimeDelta)
{
	/*_vector vTargetPos = m_pCurArm->Get_TargetState(CCameraArm::POS);

	_vector vAt = vTargetPos + XMLoadFloat3(&m_vAtOffset) + XMLoadFloat3(&m_vShakeOffset);
	m_pControllerTransform->LookAt_3D(XMVectorSetW(vAt, 1.f));*/
}

CCamera_2D* CCamera_2D::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_2D* pInstance = new CCamera_2D(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_2D");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_2D::Clone(void* pArg)
{
	CCamera_2D* pInstance = new CCamera_2D(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_2D");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCamera_2D::Free()
{
	Safe_Release(m_pCurArm);

	__super::Free();
}
