#include "stdafx.h"
#include "3DMapDefaultObject.h"
#include "GameInstance.h"


C3DMapDefaultObject::C3DMapDefaultObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :C3DMapObject(_pDevice, _pContext)
{
}

C3DMapDefaultObject::C3DMapDefaultObject(const C3DMapDefaultObject& _Prototype)
    :C3DMapObject(_Prototype)
{
}

C3DMapDefaultObject* C3DMapDefaultObject::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    C3DMapDefaultObject* pInstance = new C3DMapDefaultObject(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : C3DMapDefaultObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* C3DMapDefaultObject::Clone(void* _pArg)
{
    C3DMapDefaultObject* pInstance = new C3DMapDefaultObject(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : C3DMapDefaultObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void C3DMapDefaultObject::Free()
{
    __super::Free();
}
