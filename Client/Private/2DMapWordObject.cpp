#include "stdafx.h"
#include "2DMapWordObject.h"
#include "GameInstance.h"


C2DMapWordObject::C2DMapWordObject(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
    :C2DMapObject(_pDevice, _pContext)
{
}

C2DMapWordObject::C2DMapWordObject(const C2DMapWordObject& _Prototype)
    :C2DMapObject(_Prototype)
{
}

HRESULT C2DMapWordObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT C2DMapWordObject::Initialize(void* _pArg)
{

    return S_OK;
}

void C2DMapWordObject::Priority_Update(_float _fTimeDelta)
{
    __super::Priority_Update(_fTimeDelta);
}
void C2DMapWordObject::Update(_float _fTimeDelta)
{
    __super::Update(_fTimeDelta);

}

void C2DMapWordObject::Late_Update(_float _fTimeDelta)
{
    __super::Late_Update(_fTimeDelta);
}

HRESULT C2DMapWordObject::Render()
{
  
    return __super::Render();
}

HRESULT C2DMapWordObject::Render_Shadow()
{
    return S_OK;
}


C2DMapWordObject* C2DMapWordObject::Create(ID3D11Device* _pDevice, ID3D11DeviceContext* _pContext)
{
    C2DMapWordObject* pInstance = new C2DMapWordObject(_pDevice, _pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : C2DMapWordObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* C2DMapWordObject::Clone(void* _pArg)
{
    C2DMapWordObject* pInstance = new C2DMapWordObject(*this);

    if (FAILED(pInstance->Initialize(_pArg)))
    {
        MSG_BOX("Failed to Cloned : C2DMapWordObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void C2DMapWordObject::Free()
{
    __super::Free();
}
