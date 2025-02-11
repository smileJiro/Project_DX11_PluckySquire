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

HRESULT C3DMapSpskObject::Initialize(void* _pArg)
{

    if (nullptr == _pArg)
        return E_FAIL;

    MAPOBJ_3D_DESC* pDesc = static_cast<MAPOBJ_3D_DESC*>(_pArg);


    if (!pDesc->isSpsk)
        return E_FAIL;

    m_strRenderSectionTag = pDesc->strSpskTag;


    return __super::Initialize(_pArg);
}

HRESULT C3DMapSpskObject::Render()
{
    return __super::Render();
}

HRESULT C3DMapSpskObject::Render_Shadow()
{
    return __super::Render_Shadow();
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
