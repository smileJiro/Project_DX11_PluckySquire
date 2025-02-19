#include "Collider_Fan.h"

CCollider_Fan::CCollider_Fan(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CCollider_Circle(_pDevice, _pContext)
{
}

CCollider_Fan::CCollider_Fan(const CCollider_Fan& _Prototype)
	:CCollider_Circle(_Prototype)
{
}


HRESULT CCollider_Fan::Initialize(void* _pArg)
{
    COLLIDER_FAN_DESC* pDesc = static_cast<COLLIDER_FAN_DESC*>(_pArg);
    m_fRadianAngle = pDesc->fRadianAngle;
    m_vDirection = pDesc->vDirection;
    // Add Desc 
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

    return S_OK;
}


_bool CCollider_Fan::Is_Collision(CCollider* _pOther)
{
    if (__super::Is_Collision(_pOther))
    {
        _float2 vOtherPos = _pOther->Get_Position();
        _float2 vDir = { vOtherPos.x - m_vPosition.x ,vOtherPos.y - m_vPosition.y };
		_float fAngle =XMVectorGetX( XMVector2AngleBetweenVectors(XMLoadFloat2(&m_vDirection),  XMLoadFloat2(&vDir)));
		if (abs(fAngle) <= m_fRadianAngle*0.5)
			return true;
    }
	return false;
}
_bool CCollider_Fan::Is_ContainsPoint(_float2 _vPosition)
{
    return false;
}
void CCollider_Fan::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

void CCollider_Fan::Update_OwnerTransform()
{
	__super::Update_OwnerTransform();
}

CCollider_Fan* CCollider_Fan::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CCollider_Fan* pInstance = new CCollider_Fan(_pDevice, _pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed Create Collider_Fan");
        Safe_Release(pInstance);
    }
    return pInstance;
}

CComponent* CCollider_Fan::Clone(void* _pArg)
{
    CCollider_Fan* pInstance = new CCollider_Fan(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : Collider_Fan");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCollider_Fan::Free()
{
    __super::Free();
}
