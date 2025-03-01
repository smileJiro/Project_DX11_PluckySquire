#include "stdafx.h"
#include "SlipperyObject.h"
#include "Collider_Circle.h"

CSlipperyObject::CSlipperyObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CModelObject(_pDevice, _pContext)
{
}

CSlipperyObject::CSlipperyObject(const CSlipperyObject& _Prototype)
	:CModelObject(_Prototype)
{
}


HRESULT CSlipperyObject::Initialize(void* _pArg)
{
	CModelObject::MODELOBJECT_DESC* pBodyDesc = static_cast<CModelObject::MODELOBJECT_DESC*>(_pArg);

	pBodyDesc->eStartCoord = COORDINATE_2D;
	pBodyDesc->isCoordChangeEnable = false;

	pBodyDesc->strShaderPrototypeTag_2D = TEXT("Prototype_Component_Shader_VtxPosTex");
	pBodyDesc->iShaderPass_2D = (_uint)PASS_VTXPOSTEX::SPRITE2D;
	pBodyDesc->tTransform2DDesc.vInitialScaling = _float3(1.f, 1.f, 1.f);
	pBodyDesc->tTransform2DDesc.fSpeedPerSec = 500.f;

	pBodyDesc->iObjectGroupID = OBJECT_GROUP::MAPOBJECT;


	if (FAILED(__super::Initialize(_pArg)))
		return E_FAIL;


	m_p2DColliderComs.resize(1);

	return S_OK;
}

void CSlipperyObject::Update(_float _fTimeDelta)
{
	if (m_bSlip)
	{
		m_pControllerTransform->Go_Direction(m_vSlipDirection, _fTimeDelta);
	}
	__super::Update(_fTimeDelta);
}

HRESULT CSlipperyObject::Render()
{
	return __super::Render();
}

void CSlipperyObject::On_Collision2D_Enter(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
	if (OBJECT_GROUP::BLOCKER == _pOtherCollider->Get_CollisionGroupID())
	{
		m_bSlip = false;
	}
}

void CSlipperyObject::On_Collision2D_Stay(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CSlipperyObject::On_Collision2D_Exit(CCollider* _pMyCollider, CCollider* _pOtherCollider, CGameObject* _pOtherObject)
{
}

void CSlipperyObject::Start_Slip(_vector _vDirection)
{
	m_bSlip = true;
	m_vSlipDirection = _vDirection;
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
