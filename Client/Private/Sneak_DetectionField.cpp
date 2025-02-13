#include "stdafx.h"
#include "Sneak_DetectionField.h"
#include "Monster.h"
#include "GameInstance.h"
#include "Player.h"
#include "DebugDraw_For_Client.h"

CSneak_DetectionField::CSneak_DetectionField(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CComponent(_pDevice, _pContext)
{
}

CSneak_DetectionField::CSneak_DetectionField(const CSneak_DetectionField& _Prototype)
	: CComponent(_Prototype)
{
}

HRESULT CSneak_DetectionField::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSneak_DetectionField::Initialize(void* _pArg)
{
	SNEAKDETECTIONFIELDDESC* pDesc = static_cast<SNEAKDETECTIONFIELDDESC*>(_pArg);
	m_fRadius = pDesc->fRadius;
	m_fOffset = pDesc->fOffset;
	m_pOwner = pDesc->pOwner;
	m_pTarget = pDesc->pTarget;

	if (nullptr != m_pTarget)
		Safe_AddRef(m_pTarget);

#ifdef _DEBUG
	m_pDraw = pDesc->pDraw;
	if (nullptr != m_pDraw)
		Safe_AddRef(m_pDraw);
#endif // _DEBUG

	return S_OK;
}

void CSneak_DetectionField::Late_Update(_float _fTimeDelta)
{
#ifdef _DEBUG
	//m_pGameInstance->Add_DebugComponent_New(this);
#endif
}

#ifdef _DEBUG

HRESULT CSneak_DetectionField::Render()
{
	if (false == m_isActive)
		return S_OK;
	if (nullptr == m_pOwner)
		return S_OK;

	_vector vColor;
	vColor=(false == m_isColl) ? XMVectorSet(0.f, 1.f, 0.f, 1.f) : XMVectorSet(1.f, 0.f, 0.f, 1.f);
	m_pDraw->Render_Ring(m_pOwner->Get_FinalPosition(), XMVectorSet(m_fRadius, 0.f, 0.f, 0.f), XMVectorSet(0.f, 0.f, m_fRadius, 0.f), vColor);

	return S_OK;
}
#endif

_bool CSneak_DetectionField::IsTarget_In_SneakDetection()
{
	m_isColl = false;

	if (nullptr == m_pOwner)
		return false;
	if (nullptr == m_pTarget)
		return false;

	if (m_fRadius >= m_pOwner->Get_ControllerTransform()->Compute_Distance(m_pTarget->Get_FinalPosition()))
	{
		if (false == static_cast<CPlayer*>(m_pTarget)->Is_Sneaking())
		{
			m_isColl = true;
		}
	}

	return m_isColl;
}


CSneak_DetectionField* CSneak_DetectionField::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CSneak_DetectionField* pInstance = new CSneak_DetectionField(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CSneak_DetectionField");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSneak_DetectionField* CSneak_DetectionField::Clone(void* _pArg)
{
	CSneak_DetectionField* pInstance = new CSneak_DetectionField(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : CSneak_DetectionField");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSneak_DetectionField::Free()
{
	m_pOwner = nullptr;
	Safe_Release(m_pTarget);

#ifdef _DEBUG
	Safe_Release(m_pDraw);
#endif // _DEBUG

	__super::Free();
}
