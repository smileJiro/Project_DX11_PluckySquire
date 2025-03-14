#include "stdafx.h"
#include "SlipperyObject.h"
#include "Collider_Circle.h"
#include "ModelObject.h"

CSlipperyObject::CSlipperyObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CContainerObject(_pDevice, _pContext)
{
}

CSlipperyObject::CSlipperyObject(const CSlipperyObject& _Prototype)
	:CContainerObject(_Prototype)
{
}


HRESULT CSlipperyObject::Initialize(void* _pArg)
{
	SLIPPERY_DESC* pDesc = static_cast<SLIPPERY_DESC*>(_pArg);
	m_iImpactCollisionFilter = pDesc->iImpactCollisionFilter;
	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;

	return S_OK;
}

void CSlipperyObject::Update(_float _fTimeDelta)
{
	if (m_bSlip && false == Is_Stopped())
	{
		m_pControllerTransform->Go_Direction(m_vSlipDirection, m_fSlipPower, _fTimeDelta);
	}
	if(false == Is_Stopped())
		__super::Update(_fTimeDelta);
}

HRESULT CSlipperyObject::Render()
{
	return __super::Render();
}





void CSlipperyObject::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	if (m_bSlip)
	{
		if (m_iImpactCollisionFilter & _pOtherCollider->Get_CollisionGroupID())
		{
			m_bSlip = false;
			On_Impact(_pOtherObject);
		}
	}
}

void CSlipperyObject::Start_Slip(_fvector _vDirection, _float _fPower)
{
	if (m_bSlip || Is_Stopped())
		return;
	m_bSlip = true;
	m_vSlipDirection = _vDirection;
	m_fSlipPower = _fPower;
	On_StartSlip(_vDirection);
}


CSlipperyObject* CSlipperyObject::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
	CSlipperyObject* pInstance = new CSlipperyObject(_pDevice, _pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : SlipperyObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CSlipperyObject::Clone(void* _pArg)
{
	CSlipperyObject* pInstance = new CSlipperyObject(*this);

	if (FAILED(pInstance->Initialize(_pArg)))
	{
		MSG_BOX("Failed to Cloned : SlipperyObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}
void CSlipperyObject::Free()
{
	__super::Free();
}
