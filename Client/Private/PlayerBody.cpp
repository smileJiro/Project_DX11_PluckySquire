#include "stdafx.h"
#include "PlayerBody.h"

CPlayerBody::CPlayerBody(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
	: CModelObject(_pDevice, _pContext)
{
}


CPlayerBody::CPlayerBody(const CPlayerBody& _Prototype)
	: CModelObject(_Prototype)
{
}

CModelObject* CPlayerBody::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    CPlayerBody* pInstance = new CPlayerBody(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : PlayerBody");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CPlayerBody::Clone(void* _pArg)
{
    CPlayerBody* pInstance = new CPlayerBody(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : PlayerBody");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPlayerBody::Free()
{
	__super::Free();
}
