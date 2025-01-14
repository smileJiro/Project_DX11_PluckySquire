#include "Camera_Free.h"
#include "GameInstance.h"
#include "Key_Manager.h"
CCamera_Free::CCamera_Free(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CCamera(_pDevice, _pContext)
	, m_bCameraRotation(false)
{
}

CCamera_Free::CCamera_Free(const CCamera_Free& _Prototype)
	: CCamera(_Prototype)
	, m_bCameraRotation(false)
{
}

HRESULT CCamera_Free::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCamera_Free::Initialize(void* _pArg)
{
	CAMERA_FREE_DESC* pDesc = static_cast<CAMERA_FREE_DESC*>(_pArg);

	m_fMouseSensor = pDesc->fMouseSensor;
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;


	return S_OK;
}

void CCamera_Free::Priority_Update(_float _fTimeDelta)
{
	Key_Input(_fTimeDelta);

	__super::Compute_PipeLineMatrices();
}

void CCamera_Free::Update(_float _fTimeDelta)
{

}

void CCamera_Free::Late_Update(_float _fTimeDelta)
{
}

void CCamera_Free::Key_Input(_float _fTimeDelta)
{
	if (MOUSE_DOWN(MOUSE_KEY::RB))
	{
		if (m_bCameraRotation == false)
			m_bCameraRotation = true;
		else if (m_bCameraRotation == true)
			m_bCameraRotation = false;
	}

	if (m_bCameraRotation)
	{
		_long		MouseMove = {};
		if (MouseMove = MOUSE_MOVE(MOUSE_MOVE::X))
		{
			m_pControllerTransform->Turn(MouseMove * _fTimeDelta * m_fMouseSensor, XMVectorSet(0.f, 1.f, 0.f, 0.f));
		}

		if (MouseMove = MOUSE_MOVE(MOUSE_MOVE::Y))
		{
			m_pControllerTransform->Turn(MouseMove * _fTimeDelta * m_fMouseSensor, m_pControllerTransform->Get_State(CTransform::STATE_RIGHT));
		}
	}


	if (m_bCameraMove)
	{
		if (KEY_PRESSING(KEY::W))
		{
			m_pControllerTransform->Go_Straight(_fTimeDelta);
		}
		if (KEY_PRESSING(KEY::S))
		{
			m_pControllerTransform->Go_Backward(_fTimeDelta);
		}
		if (KEY_PRESSING(KEY::D))
		{
			m_pControllerTransform->Go_Right(_fTimeDelta);
		}
		if (KEY_PRESSING(KEY::A))
		{
			m_pControllerTransform->Go_Left(_fTimeDelta);
		}
	}
	



}

CCamera_Free* CCamera_Free::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CCamera_Free* pInstance = new CCamera_Free(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CCamera_Free");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CCamera_Free::Clone(void* _pArg)
{
	CCamera_Free* pInstance = new CCamera_Free(*this);

	// 단순히 복사만해서 주는 것이 아니라, Prototype을 통해 복사받은 후, 추가적으로 필요한 Initialize는 따로 수행한다.
	if (FAILED(pInstance->Initialize(_pArg)))
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
