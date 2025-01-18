#include "stdafx.h"
#include "Projectile_BarfBug.h"

CProjectile_BarfBug::CProjectile_BarfBug(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CModelObject(_pDevice, _pContext)
{
}

CProjectile_BarfBug::CProjectile_BarfBug(const CProjectile_BarfBug& _Prototype)
	: CModelObject(_Prototype)
{
}

HRESULT CProjectile_BarfBug::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CProjectile_BarfBug::Initialize(void* _pArg)
{
    if (FAILED(__super::Initialize(_pArg)))
        return E_FAIL;

	return S_OK;
}

void CProjectile_BarfBug::Update(_float _fTimeDelta)
{
    m_pControllerTransform->Go_Straight(_fTimeDelta);

    __super::Update(_fTimeDelta);
}

void CProjectile_BarfBug::Late_Update(_float _fTimeDelta)
{
	__super::Late_Update(_fTimeDelta);
}

HRESULT CProjectile_BarfBug::Render()
{
    __super::Render();
    return S_OK;
}

CProjectile_BarfBug* CProjectile_BarfBug::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CProjectile_BarfBug* pInstance = new CProjectile_BarfBug(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CProjectile_BarfBug");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CProjectile_BarfBug::Clone(void* _pArg)
{
    CProjectile_BarfBug* pInstance = new CProjectile_BarfBug(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : CProjectile_BarfBug");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CProjectile_BarfBug::Free()
{

    __super::Free();
}
