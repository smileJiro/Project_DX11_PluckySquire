#include "Camera_Free.h"
#include "GameInstance.h"

CCamera_Free::CCamera_Free(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera{ pDevice, pContext }
{
}

CCamera_Free::CCamera_Free(const CCamera_Free& Prototype)
	: CCamera{ Prototype }
{

}

HRESULT CCamera_Free::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Free::Initialize(void* pArg)
{
	CAMERA_FREE_DESC* pDesc = static_cast<CAMERA_FREE_DESC*>(pArg);

	pDesc->eStartCoord = COORDINATE_3D;
	pDesc->isCoordChangeEnable = false;
	pDesc->tTransform3DDesc.fSpeedPerSec = 10.f;
	pDesc->tTransform3DDesc.fRotationPerSec = XMConvertToRadians(180.f);
	m_fMouseSensor = pDesc->fMouseSensor;
	switch (pDesc->eMode)
	{
		case Engine::CCamera_Free::INPUT_MODE_WASD:
			m_iModeKey[0] = (_uint)KEY::W;
			m_iModeKey[1] = (_uint)KEY::S;
			m_iModeKey[2] = (_uint)KEY::D;
			m_iModeKey[3] = (_uint)KEY::A;
			break;
		case Engine::CCamera_Free::INPUT_MODE_DEFAULT:
		default:
			m_iModeKey[0] =  (_uint)KEY::UP;
			m_iModeKey[1] =  (_uint)KEY::DOWN;
			m_iModeKey[2] =  (_uint)KEY::RIGHT;
			m_iModeKey[3] =  (_uint)KEY::LEFT;
			break;
	}
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	return S_OK;
}

void CCamera_Free::Priority_Update(_float fTimeDelta)
{
	
}

void CCamera_Free::Update(_float fTimeDelta)
{
	Action_Zoom(fTimeDelta);
	Action_Shake(fTimeDelta);
	Change_AtOffset(fTimeDelta);

	Key_Input(fTimeDelta);
}

void CCamera_Free::Late_Update(_float fTimeDelta)
{
	__super::Late_Update(fTimeDelta);
}

INITIAL_DATA CCamera_Free::Get_InitialData()
{
	INITIAL_DATA tData;

	XMStoreFloat3(&tData.vPosition, m_pControllerTransform->Get_State(CTransform::STATE_POSITION));

	
	_vector vAt = XMLoadFloat3(&tData.vPosition) + m_pControllerTransform->Get_State(CTransform::STATE_LOOK);
	XMStoreFloat3(&tData.vAt, XMVectorSetW(vAt, 1.f));

	tData.iZoomLevel = m_iCurZoomLevel;

	return tData;
}

void CCamera_Free::Key_Input(_float fTimeDelta)
{
	if (KEY_PRESSING((KEY)m_iModeKey[0]))
	{
		m_pControllerTransform->Go_Straight(fTimeDelta);
	}

	if (KEY_PRESSING((KEY)m_iModeKey[1]))
	{
		m_pControllerTransform->Go_Backward(fTimeDelta);
	}

	if (KEY_PRESSING((KEY)m_iModeKey[2]))
	{
		m_pControllerTransform->Go_Right(fTimeDelta);
	}

	if (KEY_PRESSING((KEY)m_iModeKey[3]))
	{
		m_pControllerTransform->Go_Left(fTimeDelta);
	}
	
	if (KEY_PRESSING(KEY::Q))
	{
		_float fSpeed = m_pControllerTransform->Get_SpeedPerSec();
		_vector vPos = m_pControllerTransform->Get_State(CTransform::STATE_POSITION);

		vPos = vPos + (fTimeDelta * fSpeed * XMVectorSet(0.f, -1.f, 0.f, 0.f));

		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, vPos);
	}
	
	if (KEY_PRESSING(KEY::E))
	{
		_float fSpeed = m_pControllerTransform->Get_SpeedPerSec();
		_vector vPos = m_pControllerTransform->Get_State(CTransform::STATE_POSITION);

		vPos = vPos + (fTimeDelta * fSpeed * XMVectorSet(0.f, 1.f, 0.f, 0.f));

		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, vPos);
	}

	if (KEY_PRESSING(KEY::W))
	{
		_float fSpeed = m_pControllerTransform->Get_SpeedPerSec();
		_vector vPos = m_pControllerTransform->Get_State(CTransform::STATE_POSITION);

		vPos = vPos + (fTimeDelta * fSpeed * XMVectorSet(0.f, 0.f, 1.f, 0.f));

		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, vPos);
	}

	if (KEY_PRESSING(KEY::S))
	{
		_float fSpeed = m_pControllerTransform->Get_SpeedPerSec();
		_vector vPos = m_pControllerTransform->Get_State(CTransform::STATE_POSITION);

		vPos = vPos + (fTimeDelta * fSpeed * XMVectorSet(0.f, 0.f, -1.f, 0.f));

		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, vPos);
	}

	if (KEY_PRESSING(KEY::A))
	{
		_float fSpeed = m_pControllerTransform->Get_SpeedPerSec();
		_vector vPos = m_pControllerTransform->Get_State(CTransform::STATE_POSITION);

		vPos = vPos + (fTimeDelta * fSpeed * XMVectorSet(-1.f, 0.f, 0.f, 0.f));

		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, vPos);
	}

	if (KEY_PRESSING(KEY::D))
	{
		_float fSpeed = m_pControllerTransform->Get_SpeedPerSec();
		_vector vPos = m_pControllerTransform->Get_State(CTransform::STATE_POSITION);

		vPos = vPos + (fTimeDelta * fSpeed * XMVectorSet(1.f, 0.f, 0.f, 0.f));

		m_pControllerTransform->Set_State(CTransform::STATE_POSITION, vPos);
	}

	//_long		MouseMove = {};


	//if (MOUSE_PRESSING(MOUSE_KEY::RB)) {
	//	if (MouseMove = MOUSE_MOVE(MOUSE_AXIS::X))
	//	{
	//		m_pControllerTransform->Turn(MouseMove * fTimeDelta * m_fMouseSensor, XMVectorSet(0.f, 1.f, 0.f, 0.f));
	//	}

	//	if (MouseMove = MOUSE_MOVE(MOUSE_AXIS::Y))
	//	{
	//		m_pControllerTransform->Turn(MouseMove * fTimeDelta * m_fMouseSensor, m_pControllerTransform->Get_State(CTransform::STATE_RIGHT));
	//	}
	//}

	_long		MouseMove = {};
	m_fMouseSensor = 0.02f;
	if (KEY_PRESSING(KEY::CTRL)) {
		if (MOUSE_PRESSING(MOUSE_KEY::RB)) {
			if (MouseMove = MOUSE_MOVE(MOUSE_AXIS::Y))
			{
				m_pControllerTransform->Turn(MouseMove * fTimeDelta * m_fMouseSensor, m_pControllerTransform->Get_State(CTransform::STATE_RIGHT));
			}
		}
	}
	else if (KEY_PRESSING(KEY::LSHIFT)) {

		//if (KEY_PRESSING(KEY::LSHIFT))
		//	return;
		if (MOUSE_PRESSING(MOUSE_KEY::RB)) {
			if (MouseMove = MOUSE_MOVE(MOUSE_AXIS::X))
			{
				m_pControllerTransform->Turn(MouseMove * fTimeDelta * m_fMouseSensor, XMVectorSet(0.f, 1.f, 0.f, 0.f));
			}
		}
	}
	else if (MOUSE_PRESSING(MOUSE_KEY::RB)) {

		if (MouseMove = MOUSE_MOVE(MOUSE_AXIS::X))
		{
			m_pControllerTransform->Turn(MouseMove * fTimeDelta * m_fMouseSensor, XMVectorSet(0.f, 1.f, 0.f, 0.f));

		}
		if (MouseMove = MOUSE_MOVE(MOUSE_AXIS::Y))
		{
			m_pControllerTransform->Turn(MouseMove * fTimeDelta * m_fMouseSensor, m_pControllerTransform->Get_State(CTransform::STATE_RIGHT));
		}
	}
}

CCamera_Free* CCamera_Free::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Free* pInstance = new CCamera_Free(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_Free");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Free::Clone(void* pArg)
{
	CCamera_Free* pInstance = new CCamera_Free(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CCamera_Free");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CCamera_Free::Free()
{
	__super::Free();
}
