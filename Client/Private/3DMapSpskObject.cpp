#include "stdafx.h"
#include "3DMapSpskObject.h"
#include "GameInstance.h"


C3DMapSpskObject::C3DMapSpskObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :C3DMapObject(_pDevice, _pContext)
{
}

C3DMapSpskObject::C3DMapSpskObject(const C3DMapSpskObject& _Prototype)
    :C3DMapObject(_Prototype)
{
}

C3DMapSpskObject* C3DMapSpskObject::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    C3DMapSpskObject* pInstance = new C3DMapSpskObject(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : C3DMapSpskObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* C3DMapSpskObject::Clone(void* _pArg)
{
    C3DMapSpskObject* pInstance = new C3DMapSpskObject(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : C3DMapSpskObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void C3DMapSpskObject::Free()
{
    __super::Free();
}
