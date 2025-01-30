#include "stdafx.h"
#include "DetectionField.h"
#include "DebugDraw_For_Client.h"
#include "Monster.h"
#include "GameInstance.h"

CDetectionField::CDetectionField(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComponent(_pDevice, _pContext)
{
}

CDetectionField::CDetectionField(const CDetectionField& _Prototype)
	: CComponent(_Prototype)
{
}

HRESULT CDetectionField::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CDetectionField::Initialize(void* _pArg)
{
	DETECTIONFIELDDESC* pDesc = static_cast<DETECTIONFIELDDESC*>(_pArg);
	m_fRange = pDesc->fRange;
	m_fFOVX = pDesc->fFOVX;
	m_fFOVY = pDesc->fFOVY;
	m_fOffset = pDesc->fOffset;
	m_pOwner = pDesc->pOwner;
	m_pTarget = pDesc->pTarget;
	m_pDraw = pDesc->pDraw;
	
	if (nullptr != m_pTarget)
		Safe_AddRef(m_pTarget);
	if (nullptr != m_pDraw)
		Safe_AddRef(m_pDraw);

	return S_OK;
}

void CDetectionField::Late_Update(_float _fTimeDelta)
{
#ifdef _DEBUG
	m_pGameInstance->Add_DebugComponent_New(this);
#endif
}

#ifdef _DEBUG

HRESULT CDetectionField::Render()
{
	if (false == m_isActive)
		return S_OK;
	if (nullptr == m_pOwner)
		return S_OK;

	_float3 vPos, vScale;
	_float4 vRot;
	_float Near;
	m_pGameInstance->MatrixDecompose(&vScale, &vRot, &vPos, m_pOwner->Get_WorldMatrix());
	Near = vScale.z;
	BoundingFrustum Frustum(vPos, vRot, tanf(m_fFOVX / 2.f), -tanf(m_fFOVX / 2.f), tanf(m_fFOVY / 2.f), -tanf(m_fFOVY / 2.f), m_fOffset, m_fOffset + m_fRange);	//근평면 거리 다시 잡아야할듯
	_float4 vColor;
	XMStoreFloat4(&vColor, (false == m_isColl) ? XMVectorSet(0.f, 1.f, 0.f, 1.f) : XMVectorSet(1.f, 0.f, 0.f, 1.f));
	m_pDraw->Render_Frustum(Frustum, vColor);

	return S_OK;
}
#endif

_bool CDetectionField::IsTarget_In_Detection()
{
	m_isColl = false;

	if (nullptr == m_pOwner)
		return false;
	if (nullptr == m_pTarget)
		return false;

	//거리 먼저 판단(일단 오프셋 적용안했음)
	if (m_fRange >= m_pOwner->Get_ControllerTransform()->Compute_Distance(m_pTarget->Get_FinalPosition()))
	{
		//이후 각도로 판단 (예외처리는....)
		_float fAngle = m_pGameInstance->Get_Angle_Between_Vectors(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMVectorSet(0.f, 0.f, 1.f, 0.f), m_pTarget->Get_FinalPosition() - m_pOwner->Get_FinalPosition());
		_float fLookAngle = m_pGameInstance->Get_Angle_Between_Vectors(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMVectorSet(0.f, 0.f, 1.f, 0.f), m_pOwner->Get_ControllerTransform()->Get_State(CTransform::STATE_LOOK));
		_float fRightAngle = m_pGameInstance->Clamp_Degrees(fLookAngle + m_fFOVX / 2.f);
		_float fLeftAngle = m_pGameInstance->Clamp_Degrees(fLookAngle - m_fFOVX / 2.f);
		_float fUpAngle = m_pGameInstance->Clamp_Degrees(fLookAngle + m_fFOVY / 2.f);
		_float fDownAngle = m_pGameInstance->Clamp_Degrees(fLookAngle - m_fFOVY / 2.f);

		//x 시야각 먼저 체크
		if (fRightAngle >= fAngle && fLeftAngle <= fAngle)
		{
			// y 시야각까지 들어오면 true 리턴
			if (fUpAngle >= fAngle && fDownAngle <= fAngle)
			{
				m_isColl = true;
				return m_isColl;
			}
		}

	}
	return m_isColl;
}


CDetectionField* CDetectionField::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CDetectionField* pInstance = new CDetectionField(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CDetectionField");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CDetectionField* CDetectionField::Clone(void* _pArg)
{
	CDetectionField* pInstance = new CDetectionField(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CDetectionField");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDetectionField::Free()
{
	m_pOwner = nullptr;

	if (nullptr != m_pTarget)
		Safe_Release(m_pTarget);
	if (nullptr != m_pDraw)
		Safe_Release(m_pDraw);
	m_pDraw = nullptr;

	__super::Free();
}
