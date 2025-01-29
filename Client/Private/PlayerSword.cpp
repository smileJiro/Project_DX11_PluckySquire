#include "stdafx.h"
#include "PlayerSword.h"
#include "Actor_Dynamic.h"

CPlayerSword::CPlayerSword(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	:CModelObject(_pDevice, _pContext)
{
}


CPlayerSword::CPlayerSword(const CPlayerSword& _Prototype)
	:CModelObject(_Prototype)
{
}

void CPlayerSword::Update(_float _fTimeDelta)
{
	__super::Update(_fTimeDelta);
}

void CPlayerSword::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

void CPlayerSword::Throw( _fvector _vDirection)
{
	m_bFlying = true;
    m_pSocketMatrix = nullptr;
	m_pParentMatrices[COORDINATE_3D] = nullptr;
	_matrix matWorld = XMLoadFloat4x4( &m_WorldMatrices[COORDINATE_3D]);
    _float4 vPositon;
    XMStoreFloat4(&vPositon, matWorld.r[3]);
	m_pControllerTransform->Set_State(CTransform::STATE_POSITION, vPositon);
	static_cast<CActor_Dynamic*>(m_pActorCom)->Set_Dynamic();
    _float3 f3Direction;
	XMStoreFloat3(&f3Direction, _vDirection * m_fThrowForce);
	m_pActorCom->Add_Impulse(f3Direction);
}

CPlayerSword* CPlayerSword::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CPlayerSword* pInstance = new CPlayerSword(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : PlayerSword");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPlayerSword::Clone(void* _pArg)
{
    CPlayerSword* pInstance = new CPlayerSword(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : PlayerSword");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayerSword::Free()
{
    __super::Free();
}
